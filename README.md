## Softbody 2d simulation (pbd, kind of at this point)

Current build does not use the exact algo from pbd paper, or the right constraint for inflated body. This leads to ghost forces, but this is all I've managed to do in time.

There is a build for win64, and you can build from source, on win64 as well (the app uses a winapi main loop)
https://github.com/JackOfBlades232/MIPTPhysHW_PKiyashko/blob/hw3_pbd/DemoWin64.exe -- build
https://github.com/JackOfBlades232/MIPTPhysHW_PKiyashko/blob/hw3_pbd/SimulationVideo.mkv -- live video
