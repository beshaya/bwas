import os.path
import json

#A little utility for logging
#Accepts a prefix, then finds the first available filename with that prefix
#can log strings or dictionaries (as json)

logfile = 0
filename = ""

def newLog(prefix):
    global filename
    global logfile
    suffix = 0
    while(1) :
        filename =  "%s%04d.txt" % (prefix, suffix)
        if (not (os.path.isfile(filename))):
            break;
        suffix += 1
    print "logger set to " + filename
    logfile = open(filename, 'w')
    
def logString(string):
    logfile.write(str(string)+"\n")
    logfile.flush()
    #flush to make sure data is stored

def logDict(yourdict):
    json.dump(yourdict, logfile)
    logfile.write("\n")
    logfile.flush()
    
def close():
    logfile.flush()
    logfile.close()
    
#test!

if __name__ == "__main__":
    newLog("logtest")
    for i in range(10):
        logString(str(i))
        print(i)
    mydict = {"this": "was", "a":"triumph"}
    logDict(mydict)
    logDict(mydict)
    
    