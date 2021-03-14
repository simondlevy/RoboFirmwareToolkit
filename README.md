<p align="center"> 
<img src="extras/media/logo.png" width=450>
</p>

RoboFirmwareToolkit (RFT) is a simple, platform-independent, header-only C++ toolkit
for building robot firmware to run on Arduino and other microcontrollers.  Because
of its platform-indepdence it can also be used as the control code in a
C++-based robot [simualtor](https://github.com/simondlevy/MulticopterSim).  As with 
other software [platforms](https://github.com/simondlevy) I've developed, the
focus is on simplicity and ease-of-use rather than multiple features.

RFT is geared toward people like me who want to tinker with firmware and use
it to teach students about ideas like
[open-loop / closed loop control](https://circuitglobe.com/difference-between-open-loop-and-closed-loop-system.html),
[sensor fusion](https://en.wikipedia.org/wiki/Sensor_fusion),
and related important ideas in robotics.  People will typically learn about such important ideas from a textbook,
and then implement them a &ldquo;bottom-up&rdquo; (ad-hoc) way on Arduino, Teensy, STM32, ESP32, and
other micrcontrollers. This approach is useful for rapid prototyping but makes it
difficult to extend the project beyond the prototyping stage.  RFT aims to fill
this gap by taking a top-down approach in which controllers, sensors, and actuators can be
plugged into a pre-existing framework in a way that is easily extended for more advanced work.
(If you're familiar with [ROS](https://www.ros.org/), this approach will already make sense, but
whereas ROS is a massive package providing an entire operating system
supporting multiple robots and programming languages, RFT is focused entirely
on simple C++ firmware for a single robot.)

RFT grew out my experience developing a simple [toolkit](https://github.com/simondlevy/Hackflight)
for flight-control firmware of multi-rotor vehicles.  At some point I realized that most of this
code would work just as well with other kinds of robotic vehicles.  The object-oriented approach 
of C++ made it straightforward to support other robot types through abstract classes:

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_board.hpp">Board</a>
class specifies an abstract (pure virtual) <tt>getTime()</tt> method that you must
implement for a particular microcontroller or simulator.

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_sensor.hpp">Sensor</a>
class specifies abstract (pure virtual) methods <tt>ready()</tt> state for checking whether the sensor
has new data avaiable, and  <tt>modifyState()</tt> for modifying the vehicle's state based on that data.

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_openloop.hpp">OpenLoopController</a>
class performs basic functions associated with open-loop control, and specifies a set of abstract methods that you
implement for a particular controller like an R/C receiver.

<hr>
NEED TO FIX BELOW
<hr>

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/actuators/mixer.hpp">Mixer</a>
class is an abstract class that can be subclassed for various motor
configurations (QuadX, Hexacopter, Tricopter, etc.).  The 
<a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/actuators/mixers/quadxcf.hpp">QuadXCF</a>
(quad-X using Cleanflight numbering conventions)  and
<a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/actuators/mixers/quadxap.hpp">QuadXAP</a>
(quad-X using ArduPilot numbering conventions) subclasses are already implemented.  

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/motor.hpp">Motor</a> class
supports different kinds of motors (brushed, brushless).

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/pidcontroller.hpp">PidController</a>
class provides a constructor where you specify the PID values appropriate for your model (see
<b>PID Controllers</b> discussion below).

Because it is useful to get some visual feedback on things like vehicle orientation and RC receiver
channel values,  we also provide a very simple &ldquo;Ground Control Station&rdquo; (GCS) program.
that allows you to connect to the board and see what's going on. Windows users
can run this program directly: just download
[this zipfile](https://simondlevy.academic.wlu.edu/files/software/hackflight-gcs.zip),
unzip the file, open the folder, and double-click on <b>hackflight.exe</b>.
Others can run the <b>hackflight.py</b> Python script in the
<b>extras/gcs/python</b> folder.  To run the Python script you'll
need to install [MSPPG](https://github.com/simondlevy/hackflight/tree/master/extras/parser), a
parser generator for the Multiwii Serial Protocol (MSP) messages used by the
firmware. Follow the directions in that folder to install MSPPG for Python.

To support working with new new sensors and PID control algorithms, the <b>RoboFirmwareToolkit</b> C++ class provides
two methods: <tt>addSensor</tt> and <tt>addPidController</tt>.   For an example of how to use these
methods, take a look at 
this
[sketch](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/examples/LadybugFC/LadybugFC_DSMX_AltHold/LadybugFC_DSMX_AltHold.ino),
which uses the [VL53L1X long-range proximity sensor](https://www.tindie.com/products/onehorse/vl53l1-long-range-proximity-sensor/)
to provide altitude hold.

To get started with RoboFirmwareToolkit, take a look at the [build wiki](https://github.com/simondlevy/RoboFirmwareToolkit/wiki).
To understand the principles behind the software, contniue reading.

## Design Principles

There are two basic data types in RoboFirmwareToolkit: 
[state](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/datatypes.hpp#L40-L52) and 
[demands](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/datatypes.hpp#L31-L38).  For anyone
who's studied [Kalman filtering](https://simondlevy.academic.wlu.edu/kalman-tutorial/), the state will be familiar:
it is the set of values that define the state of the vehicle at a given time
(altitude, orientation, angular velocity, ...), which gets modified by a set of
sensors (gyrometer, accelerometer, barometer, rangefinder, ...).  Once the
state has been determined, it is used by a set of [PID
controllers](https://en.wikipedia.org/wiki/PID_controller) to modify the
demands (throttle, roll, pitch, yaw) received by the R/C receiver or other
control device. Then the demands are then
sent to the [mixer](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/actuators/mixer.hpp), which determines the
values to be sent to each motor.  The motors spin the propellers, which in turn modifies the state of the vehicle:

<img src="extras/media/dataflow2.png" width=800>

### Sensors

As discussed above, RoboFirmwareToolkit requires a bare minimum of two sensor readings:
[quaternion and gyrometer](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/imu.hpp#L47-L51).
Technically, the quaternion is more properly part of
the vehicle state, but because of the availability of &ldquo;hardware
quaternion&rdquo; data from modern sensors like the 
[USFSMAX](https://www.tindie.com/products/onehorse/max32660-motion-co-processor/).
we find it convenient to treat the quaternion as a sensor reading.  For
inertial measurement units (IMUs) like the MPU9250 that do not deliver a
hardware quaternion, RoboFirmwareToolkit provides a
[QuaternionFilter](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/filters.hpp#L109-L129)
class that can be used to compute the quaternion using your microcontroller.

If you're mathematically-minded, you can think of a sensor as a function from states to states:
<i>Sensor</i>: <i>State</i> &rarr; <i>State</i>

To provide access to other popular surface-mount sensors that you may wish to read, RoboFirmwareToolkit also has classes to support
[accelerometers](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/surfacemount/accelerometer.hpp), 
[magnetometers](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/surfacemount/magnetometer.hpp), and 
[barometers](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/surfacemount/barometer.hpp).
Together with the
[quaternion](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/surfacemount/quaternion.hpp)
and
[gyrometer](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/surfacemount/gyrometer.hpp),
these are all sub-classes of the
[SurfaceMountSensor](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/surfacemount.hpp)
class, which is in turn a sub-class of the
[Sensor](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensor.hpp#L27-L37)
class.  Each surface-mount sensor accesses the appropriate virtual method of
the [IMU](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/imu.hpp)
class (<tt>getQuaternion()</tt>, <tt>getGyrometer()</tt>, ...).  The Sensor
class is an abstract (virtual) class (a.k.a. interface) specifying two methods
that any sensor must implement: (1) reporting whether the sensor is ready to
deliver new data; (2) modifying the vehicle state.  By requiring each sensor to
report its readiness, we can avoid the need to write a separate timing loop for
each sensor in the main [loop
code](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/hackflight.hpp#L261-L274). 

To implement additional sensors, you can directly sub-class the Sensor class, as we've done with the 
[Rangefinder](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/sensors/rangefinder.hpp) 
class that we use to support the
[VL53L1](https://www.tindie.com/products/onehorse/vl53l1-long-range-proximity-sensor/) time-of-flight rangefinder 
in an 
[example sketch](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/examples/LadybugFC/LadybugFC_DSMX_AltHold/LadybugFC_DSMX_AltHold.ino).
Once you've implemented the sub-class(es) for a new sensor, you can call
<tt>RoboFirmwareToolkit::addSensor()</tt> to ensure that the sensor
code will be called by the <tt>checkOptionalSensors</tt> method.

Here is the C++ class hierarchy for currently-supported sensors:

<p align="center"> 
<img src="extras/media/sensors2.png" width=800>
</p>

### PID Controllers

Like sensors, PID controllers in RoboFirmwareToolkit are subclasses of an abstract
[class](https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/pidcontroller.hpp#L27-L44),
whose <tt>modifyDemands()</tt> method takes the current state and demands, and
modifies the demands based on the state.  (This class also provides an optional
<tt>shouldFlashLed()</tt> method, to help you see when the PID controller is
active.)  

As with sensors, you can sub-class the <tt>PidController</tt> class and call
```RoboFirmwareToolkit::addPidController()``` to ensure that your PID controller is called in the
```RoboFirmwareToolkit::runPidControllers()``` method.
The <tt>addPidController()</tt> method allows you to to specify the
auxiliary-switch state (aux state) in which the specified PID controller will be active.
For example, you can specify that a Rate controller will be active in aux
state 0 and a Level controller in aux state 1.  If you leave out the aux state,
the PID controller will be active in all states.

Note these two important points about PID controllers in RoboFirmwareToolkit:

1. <b>A PID controller is not the same as a
[flight mode](https://oscarliang.com/rate-acro-horizon-flight-mode-level/).</b>
For example, so-called [Acro mode](http://ardupilot.org/copter/docs/acro-mode.html#acro-mode) 
requires a PID controller based on angular
velocity (a.k.a. rate, computed from the gyrometer) for each of the three angles
(roll, pitch yaw). So-called [Stabilize](http://ardupilot.org/copter/docs/stabilize-mode.html#stabilize-mode) 
mode requires these three angular-velocity controllers,
plus a PID controller based on angle (computed from the quaternion) for the
roll and pitch axes.   To support this arrangement in RoboFirmwareToolkit, PID
controllers for aux state 0 will also run in aux states 1 and 2, and PID
controllers for aux state 1 will also run in aux state 2.

2. <b>It matters in which order you add PID controllers</b>, because the output of one PID controller is the input to 
the next.  For example, to get Stabilize mode, you want the Level controller to go first, setting the desired pitch/roll angles,
and the Rate controller to go next, to control the rate at which the desired angle will be reached.

<p align="center"> <img src="extras/media/pidcontrollers.png" width=600> </p>

If you're mathematically-minded, you can think of a PID Controller as a function from a (<i>State</i>, <i>Demands</i>) pair to <i>Demands</i>:
<b><i>PID Controller</i>: <i>State</i> &times; <i>Demands</i> &rarr; <i>Demands</i></b>

### Board classes

As described above, the <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/board.hpp">Board</a>
class specifies a set of abstract  methods that you implement for a particular flight controller or simulator.
The figure below shows the class hierarchy for currently implemented and tested boards.  As the figure shows, RoboFirmwareToolkit
makes extensive use of C++ inheritance to minimize the amount of redundant code among these classes.  Here is a brief
description of each member of the Board class hierarchy:

* <b>Board</b>: Ancestor class for all boards, real and simulated

* <b>RealBoard</b>: Ancestor class for real (physical) boards

* <b><i>SimulatedBoard</i></b>: Anything other than a real board; for example, the 
[SimBoard](https://github.com/simondlevy/MulticopterSim/blob/master/Source/FlightModule/SimBoard.hpp)
class used in [MulticopterSim](https://github.com/simondlevy/MulticopterSim).

* <b>ArduinoBoard</b>: Ancestor class for Arduino-compatible boards

* <b>MockBoard</b>: Parent class for Arduino development boards; enables algorithm, sensor, and receiver prototyping

<p align="center"> 
<img src="extras/media/boards2.png" width=800>
</p>
