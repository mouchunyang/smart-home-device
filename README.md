# Smart Home Device

This is a home IoT system with easily deployable, battery-powered sensor nodes that will detect the intensity of light in each room of the house in order to determine when the lights have been turned on and off throughout the day. 

## Getting Started

This IoT System runs on OpenMote cc2538 controlled by Rapberry Pi. To run the program, first clone this reporsitory onto the Raspberry Pi. To setup the Raspberry Pi, reference the following manual:

https://www.imore.com/how-get-started-using-raspberry-pi

After setting up the Raspberry pi, plug in the openmote through any USB port of the Raspberry Pi and run 
```
sudo usermod -a -G dialout pi
```
and then reboot. This is because by default, your RPi’s user account does not have access to
arbitrary USB devices plugged in. To access the USB device, you either need elevated rights
(via `sudo`), or you need to give yourself access the proper way, adding the user `pi` to the
`dialout` user group.


### Installing and running

cd into the directory and run
```
make BOARD=openmote -b -PROGRAMMER=cc2538bsl
  \PORT=/dev/ttyUSB0 flash term
  
```

