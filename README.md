<p align="center"> 
<img src="extras/media/logo.png" width=450>
</p>

**RoboFirmwareToolkit** (RFT) is a simple, platform-independent, header-only C++ toolkit
for building robot firmware to run on Arduino and other microcontrollers.  Because
of its platform-indepdence it can also be used as the control code in a
C++-based robot [simualtor](https://github.com/simondlevy/MulticopterSim).  As with 
other software [platforms](https://github.com/simondlevy) I've developed, the
focus is on simplicity and ease-of-use rather than lots of features.

RFT is geared toward people like me who want to tinker with firmware and use
it to teach students about ideas like
[open-loop / closed loop control](https://circuitglobe.com/difference-between-open-loop-and-closed-loop-system.html),
[sensor fusion](https://en.wikipedia.org/wiki/Sensor_fusion),
and related important ideas in robotics.  People will typically learn about such important ideas from a textbook,
and then implement them in a &ldquo;bottom-up&rdquo; (ad-hoc) way on Arduino, Teensy, STM32, ESP32, and
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

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_openloop.hpp">OpenLoopController</a>
class performs basic functions associated with open-loop control, and specifies a set of abstract methods that you
implement for a particular controller like an R/C receiver.

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_actuator.hpp">Actuator</a>
class is an abstract class that can be subclassed for various kinds of actuators; for example, a multirotor
mixer controlling a number of motors.  In principle, you need only one Actuator object for each project,
since it can be subclassed to control any number of motors, arms, grippers, etc.

With these three classes (Board, OpenLoopController, Actuator) you can implement a traditional vehicle like
an R/C car, that involves no closed-loop control.  For most projects, of course, you'll want to add closed-loop
control via sensors.  Hence RFT also provides the following abstract classes:

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_closedloop.hpp">ClosedLoopController</a>
class specifies an abstract method <tt>modifyDemands()</tt> that inputs the robot's current state and
outputs an array of floating-point values representing how that controller affects the demands. (For example,
an altitude-hold controller for a quadcopter would use the 'copter's altitude and vertical velocity to 
adjust the throttle demand.)  Because [PID control](https://en.wikipedia.org/wiki/PID_controller) is the most
popular kind of closed-loop control, RFT sub-classed ClosedLoopController with a 
[PidController](https://github.com/simondlevy/RoboFirmwareToolkit/blob/main/src/rft_closedloops/pidcontroller.hpp)
class.

* The <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_sensor.hpp">Sensor</a>
class specifies abstract methods <tt>ready()</tt> state for checking whether the sensor
has new data avaiable, and  <tt>modifyState()</tt> for modifying the vehicle's state based on that data.

* The abstract <a href="https://github.com/simondlevy/RoboFirmwareToolkit/blob/master/src/RFT_timertask.hpp">TimerTask</a>
class allows you to run different processes (closed-loop control, serial communication) at different rates.

