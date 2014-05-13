#include "pch.h"
#include "MotionBuilderStreamerMain.h"
#include "Common\DirectXHelper.h"

using namespace MotionBuilderStreamer;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading;
using namespace Concurrency;

#define BYTE_HEADER  0x3c;
#define BYTE_DATA_PACKET 0x44;
#define BYTE_TRAILER 0x3e;
#define MAX_DATA_SIZE 32

union BinDouble {
	double dValue;
	unsigned long long iValue;
};

// Loads and initializes application assets when the application is loaded.
MotionBuilderStreamerMain::MotionBuilderStreamerMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources), m_pointerLocationX(0.0f), m_sock(nullptr)
{
	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources));

	m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	/*
	m_timer.SetFixedTimeStep(true);
	m_timer.SetTargetElapsedSeconds(1.0 / 60);
	*/
}

MotionBuilderStreamerMain::~MotionBuilderStreamerMain()
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void MotionBuilderStreamerMain::CreateWindowSizeDependentResources() 
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	m_sceneRenderer->CreateWindowSizeDependentResources();
}

void MotionBuilderStreamerMain::StartRenderLoop()
{
	// If the animation render loop is already running then do not start another thread.
	if (m_renderLoopWorker != nullptr && m_renderLoopWorker->Status == AsyncStatus::Started)
	{
		return;
	}

	// Create a task that will be run on a background thread.
	auto workItemHandler = ref new WorkItemHandler([this](IAsyncAction ^ action)
	{
		// Calculate the updated frame and render once per vertical blanking interval.
		while (action->Status == AsyncStatus::Started)
		{
			critical_section::scoped_lock lock(m_criticalSection);
			Update();
			if (Render())
			{
				m_deviceResources->Present();
			}
		}
	});

	// Run task on a dedicated high priority background thread.
	m_renderLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);
}

void MotionBuilderStreamerMain::StopRenderLoop()
{
	m_renderLoopWorker->Cancel();
}

// Updates the application state once per frame.
void MotionBuilderStreamerMain::Update() 
{
	ProcessInput();

	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_sceneRenderer->Update(m_timer);
		m_fpsTextRenderer->Update(m_timer);
	});
}

// Process all input from the user before updating game state
void MotionBuilderStreamerMain::ProcessInput()
{
	// TODO: Add per frame input handling here.
	m_sceneRenderer->TrackingUpdate(m_pointerLocationX);
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool MotionBuilderStreamerMain::Render() 
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	m_sceneRenderer->Render();
	m_fpsTextRenderer->Render();

	return true;
}

// Notifies renderers that device resources need to be released.
void MotionBuilderStreamerMain::OnDeviceLost()
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void MotionBuilderStreamerMain::OnDeviceRestored()
{
	m_sceneRenderer->CreateDeviceDependentResources();
	m_fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

task<void> MotionBuilderStreamerMain::StartStreaming(Windows::Networking::HostName^ hostName)
{
	StreamSocket^ sock = ref new StreamSocket();
	return create_task(sock->ConnectAsync(hostName, "3002")).then([this, sock](task<void> t)
	{
		t.get();
		m_sock = sock;
	});
}

void MotionBuilderStreamerMain::StopStreaming()
{
	if (m_sock != nullptr)
	{
		delete m_sock;
		m_sock = nullptr;
	}
}

void MotionBuilderStreamerMain::SensorUpdate(Windows::Devices::Sensors::OrientationSensorReading^ orReading)
{
	m_sceneRenderer->SensorUpdate(orReading);

	// TODO: stream data here.
	if (m_sock != nullptr)
	{
		auto q = orReading->Quaternion;
		BinDouble qBin[4];
		qBin[0].dValue = -q->W;
		qBin[1].dValue = q->Y;
		qBin[2].dValue = q->Z;
		qBin[3].dValue = q->X;

		Array<unsigned char>^ packet = ref new Array<unsigned char>(36);
		packet[0] = BYTE_HEADER;
		packet[1] = BYTE_DATA_PACKET;

		for (int i = 0; i < 4; i++)
		{
			int off = i * 8;
			packet[off + 2] = (unsigned char)(qBin[i].iValue >> 56);
			packet[off + 3] = (unsigned char)(qBin[i].iValue >> 48);
			packet[off + 4] = (unsigned char)(qBin[i].iValue >> 40);
			packet[off + 5] = (unsigned char)(qBin[i].iValue >> 32);
			packet[off + 6] = (unsigned char)(qBin[i].iValue >> 24);
			packet[off + 7] = (unsigned char)(qBin[i].iValue >> 16);
			packet[off + 8] = (unsigned char)(qBin[i].iValue >> 8);
			packet[off + 9] = (unsigned char)(qBin[i].iValue >> 0);
		}

		unsigned char checksum = 0;
		for (int i = 0; i < MAX_DATA_SIZE; i++)
		{
			checksum += packet[2 + i];
		}
		packet[34] = checksum;

		packet[35] = BYTE_TRAILER;

		DataWriter^ sockWriter = ref new DataWriter(m_sock->OutputStream);
		sockWriter->WriteBytes(packet);
		create_task(sockWriter->StoreAsync()).then([sockWriter](unsigned int _)
		{
			sockWriter->DetachStream();
		});
	}
}
