#An example script for using the bwas library for logging heating

import logger
import bwasSerial as bwas
import time
import sys

#assign your thermistors names here!
thermistors = {"heater":0, "air":1, "bottle":2}

#default samples per second
default_rate = 2
rate = default_rate



if __name__ == "__main__":

    port = "COM14"
    duration = float("Inf")
    if len(sys.argv) < 2:
        print "Usage:"
        print "python heating.py PORT [-d duration] [-r rate]"
        print "  PORT     - USB serial port for Arduino"
        print "  duration - Duration of test in seconds (default infinity)"
        print "  rate     - Seconds per sample (default 2)"
        sys.exit();
    if len(sys.argv) > 1:
        port = sys.argv[1]
    for i in range(len(sys.argv[2:])):
        if sys.argv[i+2] == '-d':
            duration = float(sys.argv[i+3])
        if sys.argv[i+2] == '-r':
            rate = float(sys.argv[i+3])
    bwas.connect(port);
    #start a new log once we've connected
    logger.newLog("heating");
    logger.logDict({"comment": "this is a test"})
    bwas.heater(1)
    bwas.heaterFan(255)
    
    data = {}
    last_time = 0;
    start_time = time.time();
    #log every seconds
    while(1):
        now = time.time();
        if (now < last_time + rate):
            continue;
        if (now > start_time + duration):
            break;
        last_time = now
        for thermistor in thermistors.keys():
            data[thermistor] = bwas.readTemp(thermistors[thermistor])
        data["ir"] = bwas.readIR();
        data["time"] = now
        print data
        logger.logDict(data)
    logger.close();
    print "Test complete"
    #ending the script disconnects serial, which causes the board to reset
    #this will turn off all outputs (yay!)
            
        


