# Vic-20-Emulator
This was a hobby project for me during the summer. The emulator was written from scratch in C++ using my MOS6502 implementation which is available in one of my repositories. The emulator is largely based on old technical documents and prior published analysis regarding the Vic-20. 

The SDL + OpenGL front-end implementation is a bit messy but it should be regarded as a prototype more than anything else. Most games will run fine on the current implementation, however, the implementation is not yet robust enough to run all Vic-20 software without problems so this repository will continue to be updated with future additions... If I eventually bother to do so. The main problem currently is that information on the missing features is pretty scarce and takes a lot of time to decode.

The front-end implementation starts the application with a program loader, this was specifically written to avoid using os-specific or too much third party dependent code to keep the emulator portable.

The following screenshots demonstrate the emulator in action.
![Screen5](http://i.imgur.com/9a1JfGN.png)
![Screen1](http://i.imgur.com/owgH1sA.png)
![Screen2](http://i.imgur.com/qo5oudc.png)
![Screen3](http://i.imgur.com/bG6h4f2.png)
![Screen4](http://i.imgur.com/gRnpHGm.png)
![Screen7](http://i.imgur.com/4hd368E.png)
