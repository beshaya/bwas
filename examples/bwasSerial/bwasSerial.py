import serial
import sys

port = 'COM6'
ser = 0

#state variables are scaled to between 0 and 1 inclusive
bwas_state = {'heater':0,'heater fan':0,'cooler':0,'cooler fan':0}
'''
 * Call these functions to control the BWAS!
'''



def heater(val) :
    if (val > 255 or val < 0):
        print "bad value, must be [0.255]"
        return
    bwas_state['heater'] = val / 255.
    cmd = "h%02X\n" % val
    ser.write(cmd);
    
    resp = ser.readline();

def cooler(state):
    if (val > 255 or val < 0):
        print "bad value, must be [0.255]"
        return
    bwas_state['cooler'] = val / 255.
    cmd = "c%02X\n" % val
    ser.write(cmd);
    ser.readline()

def coolerFan(speed):
    if (speed != 0 and speed != 1):
        print "warning: pwm fan has been deprectiated, use 0 or 1"
        return
    speed = 1 if speed else 0;
    bwas_state['cooler fan'] = speed
    cmd = "C%02X\n" % speed
    ser.write(cmd)
    ser.readline();

def heaterFan(speed):
    if (speed != 0 and speed != 1):
        print "warning: pwm fan has been depreciated, use 0 or 1"
    speed = 1 if speed else 0;
    bwas_state['heater fan'] = speed / 255.
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
        print "bad input: '"+resp+"'"
        return float('nan')

def readIR():
    ser.write("i00\n");
    resp = ser.readline();
    try: 
        return float(resp);
    except ValueError, e:
        print "IR error"
        return float('nan')

def readHall():
    ser.write("h00\n");
    resp = ser.readline();
    return int(resp)

def readCoolerCurrent():
    ser.write("I01\n");
    resp = ser.readline();
    return float(resp);

def readHeaterCurrent():
    ser.write("I00\n");
    resp = ser.readline();
    return float(resp);

def readTouchSensor(channel):
    if (channel != 1 and channel != 2):
        print "Invalid touch channel"
        return
    cmd = "s%02X\n" % channel
    ser.write(cmd)
    resp = ser.readline();
    return int(resp)


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

#Turn off everything.
#Since this is used in the SIGTERM routine, return the response
def off(val=0):
    bwas_state['heater'] = 0
    bwas_state['cooler'] = 0
    bwas_state['cooler fan'] = 0
    bwas_state['heater fan'] = 0
    cmd = "o00\n"
    ser.write(cmd)
    response = ser.readline()
    return response
    
    
def connect(port) :
    global ser
    ser = serial.Serial(port,115200,timeout=10)
    #i don't know why, but the first two responses are always blank...
    resp = ''
    while(resp.find("BWAS READY") < 0):
        resp = ser.readline()
        print resp
    print "ready"
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
