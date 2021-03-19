# MSPPG
Multiwii Serial Protocol Parser Generator

<img src="https://github.com/simondlevy/RoboFirmwareToolkit/blob/main/extras/media/msppg.png" width=600>

## Extending

The messages.json file currently contains just a few message specifications,
but you can easily add to it by specifying additional messages from the the MSP
[standard](http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol),
or add some of your own new message types.  MSPPG currently supports types
byte, short, int, and float, but we will likely add int as the need arises.
