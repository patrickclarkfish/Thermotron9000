Thermotron9000
==============

MRF24J based thermostat with LM35 temp sensor.


This project uses Karl Palsson's MRF24j40 library from karlp/karlnet/common:
https://github.com/karlp/karlnet/tree/master/common

*and*

Peter Fluery's LCD library from:
http://homepage.hispeed.ch/peterfleury/lcdlibrary.zip

Which will be modified to use Davide Gironi's 75hc595 library:
https://code.google.com/p/davidegironi/downloads/detail?name=avr_lib_l74hc595_01.zip
to control the lcd using 3 pins.