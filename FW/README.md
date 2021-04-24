# Introduction

This folder contains `VS-MP3P` firmware.  
In order to build this firmware you need at least one of following items

## Keil 
You need `Keil µVision V5.16.0.0` installed.
1. `git clone git@github.com:ArashEM/VS-MP3.git`
1. open project file in `FW/MDK-ARM/MP3P.uvprojx`
2. press `F7`
3. find hex/axf file in `FW/MDK-ARM/MP3P/MP3P.axf`

## GCC
You need `arm-none-eabi-gcc` and `GNU make` installed. 
1. `git clone git@github.com:ArashEM/VS-MP3.git`
2. `cd FW/ && make all`
3. find hex/bin file in `FW/build/MP3P.hex`

