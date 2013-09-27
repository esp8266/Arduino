#Firmata

Firmata is a protocol for communicating with microcontrollers from software on a host computer. The [protocol](http://firmata.org/wiki/Protocol) can be implemented in firmware on any microcontroller architecture as well as software on any host computer software package. The arduino repository described here is a Firmata library for Arduino and Arduino-compatible devices. See the [firmata wiki](http://firmata.org/wiki/Main_Page) for additional informataion. If you would like to contribute to Firmata, please see the [Contributing](#contributing) section below.

##Usage

There are two main models of usage of Firmata. In one model, the author of the Arduino sketch uses the various methods provided by the Firmata library to selectively send and receive data between the Arduino device and the software running on the host computer. For example, a user can send analog data to the host using ``` Firmata.sendAnalog(analogPin, analogRead(analogPin)) ``` or send data packed in a string using ``` Firmata.sendString(stringToSend) ```. See File -> Examples -> Firmata -> AnalogFirmata & EchoString respectively for examples.

The second and more common model is to load a general purpose sketch called StandardFirmata on the Arduino board and then use the host computer exclusively to interact with the Arduino board. StandardFirmata is located in the Arduino IDE in File -> Examples -> Firmata.

##Firmata Client Libraries
Most of the time you will be interacting with arduino with a client library on the host computers. Several Firmata client libraries have been implemented in a variety of popular programming languages:

* procesing
  * [https://github.com/firmata/processing]
  * [http://funnel.cc]
* python
  * [https://github.com/firmata/pyduino]
  * [https://github.com/lupeke/python-firmata]
  * [https://github.com/tino/pyFirmata]
* perl
  * [https://github.com/ntruchsess/perl-firmata]
  * [https://github.com/rcaputo/rx-firmata]
* ruby 
  * [https://github.com/hardbap/firmata]
  * [https://github.com/PlasticLizard/rufinol]
  * [http://funnel.cc]
* clojure
  * [https://github.com/nakkaya/clodiuno]
* javascript 
  * [https://github.com/jgautier/firmata]
  * [http://breakoutjs.com]
  * [https://github.com/rwldrn/johnny-five]
* java 
  * [https://github.com/4ntoine/Firmata]
  * [https://github.com/shigeodayo/Javarduino]
* .NET
  * [http://www.imagitronics.org/projects/firmatanet/]
* Flash/AS3
  * [http://funnel.cc]
  * [http://code.google.com/p/as3glue/]
* PHP
  * [https://bitbucket.org/ThomasWeinert/carica-firmata]

Note: The above libraries may support various versions of the Firmata protocol and therefore may not support all features of the latest Firmata spec nor all arduino and arduino-compatible boards. Refer to the respective projects for details.


<a name="contributing" />
##Contributing

If you discover a bug or would like to propose a new feature, please open a new [issue](https://github.com/firmata/arduino/issues?sort=created&state=open). Due to the limited memory of standard Arduino boards we cannot add every requested feature to StandardFirmata. Requests to add new features to StandardFirmata will be evaluated by the Firmata developers. However it is still possible to add new features to other Firmata implementations (Firmata is a protocol whereas StandardFirmata is just one of many possible implementations).

To contribute, fork this respository and create a new topic branch for the bug, feature or other existing issue you are addressing. Submit the pull request against the *dev* branch.

If you would like to contribute but don't have a specific bugfix or new feature to contribute, you can take on an existing issue, see issues labeled "pull-request-encouraged". Add a comment to the issue to express your intent to begin work and/or to get any additional information about the issue.

You must thorougly test your contributed code. In your pull request, describe tests performed to ensure that no existing code is broken and that any changes maintain backwards compatibility with the existing api. Test on multiple Arduino board variants if possible. We hope to enable some form of automated (or at least semi-automated) testing in the future, but for now any tests will need to be executed manually by the contributor and reviewsers.

Maintain the existing code style:

- Indentation is 2 spaces
- Use spaces instead of tabs
- Use camel case for both private and public properties and methods
- Document functions (specific doc style is TBD... for now just be sure to document)
- Insert first block bracket on line following the function definition:

<pre>void someFunction()
{
  // do something
}
</pre>
