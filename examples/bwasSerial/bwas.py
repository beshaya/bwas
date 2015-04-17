import logger
import bwasSerial as bwas
import time
import sys
import bwasLogic as logic

#assign your thermistors names here!
thermistors = {"coolring_1":4, "coolring_2":5, "heatsink_1":2,"heatsink_2":3,"heater":0,"bottle":1}

#default samples per second
default_rate = 2
rate = default_rate

module = "observe"
filename = "log"
comment = ""
#comment = None

if __name__ == "__main__":

    port = "COM3"
    duration = float("Inf")
    if len(sys.argv) < 2:
        print "Usage:"
        print "python heating.py [-pdrlfc] [--list]"
        print " -p PORT           - USB serial port for Arduino"
        print " -d duration     - Duration of test in seconds (default infinity)"
        print " -r rate         - Seconds per sample (default 2)"
        print " -l logic module - logic from bwasLogic to use (default heat)"
        print " -f filename     - log file"
        print " -c comment"
        print " --list          - list all logic modules and exit"
        sys.exit();
    args = sys.argv[1:]
    for i in range(len(args)):
        if args[i] == '-d':
            duration = float(args[i+1])
        if args[i] == '-r':
            rate = float(args[i+1])
        if args[i] == '-l':
            module = args[i+1]
        if args[i] == '--list':
            print "installed logic modules:"
            print logic.logic.keys()
            sys.exit()
        if args[i] == '-p':
            port = args[i+1]
        if args[i] == '-f':
            filename = args[i+1]
        if args[i] == '-c':
            comment = args[i+1]
            
    #begin!
    bwas.connect(port);
    
    #initialize logic module
    logic.init[module]()
    
    #start a new log once we've connected
    logger.newLog(filename);
    if comment != None:
        logger.logDict({"comment": comment})
    
    #initialize data
    data = {}
    data['state'] = bwas.bwas_state; #assign internal state as a sub-dictionary
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
        data["time"] = now
        
        for thermistor in thermistors.keys():
            data[thermistor] = bwas.readTemp(thermistors[thermistor])
        data["ir"] = bwas.readIR();
        logic.logic[module](data)
        
        print data
        logger.logDict(data)
    logger.close();
    print "Test complete"
    bwas.off();
    


