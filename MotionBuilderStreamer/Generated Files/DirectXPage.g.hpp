﻿

//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"
#include "DirectXPage.xaml.h"




void ::MotionBuilderStreamer::DirectXPage::InitializeComponent()
{
    if (_contentLoaded)
        return;

    _contentLoaded = true;

    // Call LoadComponent on ms-appx:///DirectXPage.xaml
    ::Windows::UI::Xaml::Application::LoadComponent(this, ref new ::Windows::Foundation::Uri(L"ms-appx:///DirectXPage.xaml"), ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);

    // Get the SwapChainPanel named 'swapChainPanel'
    swapChainPanel = safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"swapChainPanel"));
    // Get the TextBox named 'IpAddressTextBox'
    IpAddressTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"IpAddressTextBox"));
    // Get the Button named 'StartButton'
    StartButton = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"StartButton"));
    // Get the Button named 'StopButton'
    StopButton = safe_cast<::Windows::UI::Xaml::Controls::Button^>(static_cast<Windows::UI::Xaml::IFrameworkElement^>(this)->FindName(L"StopButton"));
}

void ::MotionBuilderStreamer::DirectXPage::Connect(int connectionId, Platform::Object^ target)
{
    switch (connectionId)
    {
    case 1:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::MotionBuilderStreamer::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::StartButton_Click);
        break;
    case 2:
        (safe_cast<::Windows::UI::Xaml::Controls::Primitives::ButtonBase^>(target))->Click +=
            ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::MotionBuilderStreamer::DirectXPage::*)(Platform::Object^, Windows::UI::Xaml::RoutedEventArgs^))&DirectXPage::StopButton_Click);
        break;
    }
    (void)connectionId; // Unused parameter
    (void)target; // Unused parameter
    _contentLoaded = true;
}

