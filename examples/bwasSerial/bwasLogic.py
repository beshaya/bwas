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

def skip(arg=0) :
    pass
    
def heat(state) :
    bwas.heater(1)
    bwas.heaterFan(255)
    
def cool(state) :
    bwas.cooler(1)
    bwas.coolerFan(255)
    
def fanTest(state) :
    bwas.cooler(1)
    if state['coolring_2'] < 0:
        bwas.coolerFan(0)
    elif state['coolring_2'] > 5:
        bwas.coolerFan(255)
    else:
        bwas.coolerFan(int((state['coolring_2'])*51))

def insulationTest(state) :
    bwas.cooler(1)
    bwas.coolerFan(255)
    if state['coolring_2'] > 0:
        bwas.heaterFan(0)
    elif state['coolring_2'] < -5:
        bwas.heaterFan(255)
    else:
        bwas.heaterFan(abs(int((state['coolring_2'])*51)))
    
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

init =  {"heat":skip, "cool":skip, "user":userInit, "observe":skip, "fanTest":skip,     "insulationTest":skip}
logic = {"heat":heat, "cool":cool, "user":user,     "observe":skip, "fanTest":fanTest,  "insulationTest":insulationTest}

