Arduino_E1.31_Pixel_Controller

This code was written for the primary use of controlling christmas lights but can be used for any purpose you like using the 
sACN/E1.31 protocol

It can control up to 240 pixels spread across 2 lighting subnets.  This allows us to sacrifice some of the packet receive buffer
to store more pixel data.

The code on each is mostly identical but just set up to use 2 different types of arduino compatible ethernet modules:

Wiznet - The default 'ethernet shield' for arduino.  A simple Uno and ethernet shield will set you back about $15
  Advantages
    -> Easy to Program
    -> Simple to use
	-> Quite robust/reliable
	-> Good if driving 12v Pixels (due to the onboard vreg)
  Disadvantages
    -> Big (hard to put in small enclosures
	-> not overly cheap if you need lots of them
	
ENC28J60 - The cheaper alternative.  Paired up with an Arduino Pro Mini. Ideal for 5v Pixels. Wiring instructions in code.
  Advantages
    -> Small
	-> Cheap
  Disadvantages
    -> Requires external programmer
	-> Manual Assembly required
	-> Sometimes randomly locks up (code includes a watchdog to help with this)
