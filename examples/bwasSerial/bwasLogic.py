'''
 file bwasLogic.py
 
 put logic functions into this file to add define how the bwas
 controls the warmer
 
 in order for bwas.py to use the logic module, it must be referenced by
 init and logic, the lookup tables for the functions.
 
 The function linked to by init will be called once at the start of bwas,
 then logic will be called each time step of bwas.
 
 logic functions take a dictionary input,
 this dictionary should include a timestamp, and various temperatures
'''

import bwasSerial as bwas
import getch

internalState = {}

init =  {"heat":skip, "cool":skip, "user":userInit, "observe":skip}
logic = {"heat":heat, "cool":cool, "user":user,     "observe":skip}

def skip() :
    pass
    
def heat(state) :
    print bwas.ser
    bwas.heater(1)
    bwas.heaterFan(255)
    
def cool(state) :
    bwas.cooler(1)
    bwas.heaterFan(255)
    
def user(state) :
    cmd = getch.charPressed()
    if (cmd == 'h'):
        print "heating"
        bwas.heater(1)
        bwas.heaterFan(255)
        bwas.coolerFan(0)
        bwas.cooler(0)
    elif (cmd == 'c'):
        print "cooling"
        bwas.cooler(1)
        bwas.coolerFan(255)
        bwas.heaterFan(0)
        bwas.heater(0)
    elif (cmd =='0'):
        print "off"
        bwas.off()
    
def userInit() :
    getch.listen();
    print "now listening to key inputs;"
    print "press h to heat, c to cool, o to off"


