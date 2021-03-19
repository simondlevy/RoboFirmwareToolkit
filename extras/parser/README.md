# MSPPG: Multiwii Serial Protocol Parser Generator

<img src="https://github.com/simondlevy/RoboFirmwareToolkit/blob/main/extras/media/msppg.png" width=600>

**msppgy.py** is a standalone Python script that outputs code for parsing and generating
[MSP](http://www.armazila.com/MultiwiiSerialProtocol(draft)v02.pdf) messages
parsing code based on a simple JSON specification.  By using this script you can avoid the
error-prone task of writing your own parsing code from scratch.

## Usage

Running **msppg.py** in a directory that contains a file **messages.json** will produce a new **output** directory
containing:

* **serialtask.hpp**, C++ header file that you can include in your firmware to parse the messages defined in
**messages.json**

## Extending

The messages.json file currently contains just a few message specifications,
but you can easily add to it by specifying additional messages from the the MSP
[standard](http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol),
or add some of your own new message types.  MSPPG currently supports types
byte, short, int, and float, but we will likely add int as the need arises.
