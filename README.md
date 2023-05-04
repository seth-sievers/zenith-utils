# About
This repo will be used to organize various utilities used with Missouri S&T's
Rocket Design Team's competition rocket, Zenith, which will be competing in the 
2023 Spaceport America Cup. 

## teensy/
This directory contains the datalogging code for the teensy in Zenith's payload. 
The teensy acts as a basic datalogger, logging acceleration, rotational rates, and
geiger counter pulse counts. 

To flash the teensy, open the teensy/ directory as a project in the platformio 
extension from within vscode and all dependencies an toolchains should be automatically
installed. 

## geiger/ 
This directory contains the usb driver and software for accessing the data from the
GMC-320s geiger counter in zenith's payload. <a href="http://www.gqelectronicsllc.com/comersus/store/download.asp">Official Website</a>

## dji-fpv-goggles-video-out/
This directory contains the programs and directions to enable viewing a video transmission
from the DJI FPV Goggles V2 on a Laptop screen. A usb c data cable is required to 
for the connection. Credit to <a href="https://oscarliang.com/dji-fpv-goggles-video-out/">OscarLiang</a>
