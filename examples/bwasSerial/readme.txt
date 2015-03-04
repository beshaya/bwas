High speed bwas communication library, for controlling the bwas with python

Included python programs:
 * bwasSerial.py - library for serial communication, also provides basic menu
 * logger.py     - library for logging to sequential files
 * heating.py    - example of commanding the bwas to heat and sampling


serial protocol for communicating with bwas

commands are one character
data to the board is two hex characters
data from the board is base 10
all transactions end with a \n

h[0/1] - heater on/off
HXX - heater fan speed
c[0/1] - cooler on/off
CXX - cooler fan speed
t[0-7] - temperature
rXX - red
gXX - green
bXX - blue

returns:
for t: VALUE\n
else: \n
