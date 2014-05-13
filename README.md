Windows Phone Streamer for MotionBuilder TCP Device
===================================================
This is a Windows Phone 8.1 application that will stream the phone's current orientation to a TCP server
running on a computer with MotionBuilder. The server will, in turn, relay the information to a MotionBuilder
device plugin, using the format the plugin expects.

This repository only contains the Windows Phone app. For the other two components of the toolchain -- the
TCP server and the MotionBuilder Open Reality SDK device plugin, please see the repository
[mb-device-tcp](https://github.com/sdao/mb-device-tcp).

Quick Walkthrough
=================
1. See the walkthrough at the [mb-device-tcp](https://github.com/sdao/mb-device-tcp) repository for how to
set up the server and MotionBuilder plugin.
2. Once the server is running, start this application on your Windows Phone. It should display a floating cube
that stays oriented upwards in "world space", i.e., like a level, as you rotate the phone.
3. On the computer running the server and MotionBuilder, run the `ipconfig` command to discover the server's
IP address.
4. In the Windows Phone app, type the server's IP address into the text box and click Start. If the connection
fails, check the IP address again. In addition, make sure inbound connections to the server are allowed.
The phone app will connect to the server on port 3002, so make sure this port is open in your firewall.
5. The server should display a message saying that the phone is connected. If MotionBuilder has already
been connected, you should see the orientation of the device match the orientation of your Windows Phone.
Note that the quaternions are adjusted so that the rotations match up when you are facing North.
The "default" position is setting your phone down on a level surface, face-up, with the top facing North.
