import serial
import sys

port = 'COM6'
ser = 0

'''
 * Call these functions to control the BWAS!
'''



def heater(state) :
    cmd = "h%02X\n" % state
    ser.write(cmd);
    resp = ser.readline();

def cooler(state):
    cmd = "c%02X\n" % state
    ser.write(cmd);
    ser.readline()

def coolerFan(speed):
    if (speed > 255 or speed < 0):
        print "bad speed, must be [0,255]"
        return
    cmd = "C%02X\n" % speed
    ser.write(cmd)
    ser.readline();

def heaterFan(speed):
    if (speed > 255 or speed < 0):
        print "bad speed, must be [0,255]"
        return
    cmd = "H%02X\n" % speed
    ser.write(cmd)
    ser.readline();
  
def readTemp(channel):
    if (channel < 0 or channel > 8) :
        print "bad channel, must be [0,7]"
        return float("inf")
    cmd = "t%02X\n" % channel
    ser.write(cmd)
    resp = ser.readline();
    try:
        return float(resp);
    except ValueError, e:
        return float('nan')

def readIR():
    ser.write("i00\n");
    resp = ser.readline();
    return float(resp);

def red(val):
    cmd = "r%02X\n" % val
    ser.write(cmd)
    ser.readline()
 
def green(val):
    cmd = "g%02X\n" % val
    ser.write(cmd)
    ser.readline()
    
def blue(val):
    cmd = "b%02X\n" % val
    ser.write(cmd)
    ser.readline()

def off(val=0):
    cmd = "o00\n"
    ser.write(cmd)
    ser.readline()
    
def connect(port) :
    global ser
    ser = serial.Serial(port,115200,timeout=1)
    #i don't know why, but the first two responses are always blank...
    heater(0)
    heater(0)
'''
 * Functions for testing this library
'''
 
def echoTemp(channel):
    temperature = readTemp(channel)
    print "Temperature is %fC " % temperature

def echoIR(arg):
    temperature = readIR();
    print "IR reading is %fC" % temperature
    
def printMenu():
    print "BWAS Serial Interface"
    print "h 0/1   - heater off"
    print "c 0/1   - heater on"
    print "H XXX   - heater fan pwm (0-255)"
    print "C XXX   - cooler fan pwm (0-255)"
    print "t [0-7] - read temp channel"
    print "r XX    - red pwm"
    print "g XX    - green pwm"
    print "b XX    - blue pwm"
    print "i       - read IR temperature"
    print "o       - all off"
    print

options = {'h': heater, 'c': cooler, 'H': heaterFan, 'C': coolerFan,
           't': echoTemp, 'i': echoIR, 'r':red, 'g':green, 'b':blue,
           'o': off}
           
      
def menuLoop() :
    while(1) : 
        input = raw_input("input: ")
        parts = input.split(" ")
        parts = [item for item in parts if item != ""]
        arg = 0
        try :
            arg = int(parts[1])
        except ValueError, e:
            print "bad value"
            continue;
        except IndexError, e:
            arg = 0
        options[parts[0]](arg)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        port = sys.argv[1]
    connect(port)
    printMenu()
    menuLoop()