Introduction
------------

An Arduino library for the Mitsubishi 50530 LCD


Usage
-----

Arduino Pins to use can be assigned in the constructor.

Sample configuration for wiring:

Also check the included image optres_dmc50652n_backside.jpg for the pin layout of the display. lcd display pins are numbered 1-12 from left to right.

digital 7 ->  DB7 (databit 7) ( lcd display pin 1 )
digital 6 ->  DB6 (databit 6) ( lcd display pin 2 )
digital 5 ->  DB5 (databit 5) ( lcd display pin 3 )
digital 4 ->  DB4 (databit 4) ( lcd display pin 4 )
digital 3 ->  EX (Execute)    ( lcd display pin 5 )
digital 2 ->  R/W (Read/Write) ( lcd display pin 6 )
digital 1 ->  I/OC1 (IO Control 1) ( lcd display pin 7 )
digital 0 ->  I/OC2 (IO Control 2) ( lcd display pin 8 )

5V -> VSS (GND) ( lcd display pin 9 )
Gnd -> Vdd (logic power)  ( lcd display pin 10 )

VLCD (LCD power) ( lcd display pin 11 )should get a voltage of 12v in our tests , or the LCD display shows no characters ( in our test ), so you need an external power source for that...


Example
-------
Check the HelloWorld example included in this library



Credits
-------
Inital Development : Jeroen Van Beirendonck
Patches & suggestions : Stefaan Ponnet
Inspired on the work of these guys : http://www.weethet.nl/dutch/basicstamp2_optrexdmc50652n_lcd.php (the image image optres_dmc50652n_backside.jpg was used from this website)

