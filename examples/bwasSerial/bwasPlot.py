# -*- coding: utf-8 -*-
"""
Created on Wed Feb 11 14:24:49 2015

@author: Silas
"""

import json
import os
import sys
import pprint
import matplotlib.pyplot as plt
from matplotlib import gridspec
import datetime

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Usage:"
        print "python bwasPlot.py FILENAME"
        sys.exit();
    if len(sys.argv) > 1:
        filename = sys.argv[1]
    for i in range(len(sys.argv[2:])):
        if sys.argv[i+2] == '-s':
            duration = float(sys.argv[i+3])
            
#empty lists for all the things!
raw_data = []
time = []
coolring_1 = []
heatsink_1 = []
coolring_2 = []
heatsink_2 = []
ir = []
heater = []
bottle = []

state = []
cooler_s = []
heater_s = []
heaterFan = []
coolerFan = []

sensor_names = ["coolring_1", "coolring_2", "heatsink_1", "heatsink_2", "ir", "heater", "bottle"]
sensors = [coolring_1, coolring_2, heatsink_1, heatsink_2, ir, heater, bottle]

state_names = ["cooler", "heater", "heater fan", "cooler fan"]
states = [cooler_s, heater_s, heaterFan, coolerFan]

try:
    with open(filename) as data_file:
        for line in data_file:
            try:
                raw_data.append(json.loads(line))
            except:
                print "Could not read line "


    start = raw_data[1]['time']
    
    for i in range(len(raw_data)-1):
        time.append((raw_data[i+1]['time'])-start)
        
    try:
        for i in range(len(raw_data)-1):
            state.append(raw_data[i+1]['state'])
        for j in range(len(state_names)):
            for k in range(len(state)):
                states[j].append(state[k][state_names[j]])
    except:
        print "State not found in log"
    
    for j in range(len(sensor_names)):
        try:
            for i in range(len(raw_data)-1):
                if (raw_data[i+1][sensor_names[j]]) > -10.0:
                    sensors[j].append(raw_data[i+1][sensor_names[j]])
                else:
                    sensors[j].append(NaN)
        except:
            print "Sensor:", sensor_names[j], "not found in log"
    
    
    
    fig = plt.figure()
    gs = gridspec.GridSpec(2, 1, height_ratios=[3, 1]) 
    ax = plt.subplot(gs[0])
  
    
    for i in range(len(sensors)):
        try:
            plt.plot(time,sensors[i], label = sensor_names[i])
        except:
            print "Cannot plot sensor:", sensor_names[i]

    plt.title(datetime.datetime.fromtimestamp(int(start)).strftime('%Y-%m-%d %H:%M:%S')+'\n'+raw_data[0]['comment'])
    box = ax.get_position()
    ax.set_position([box.x0, box.y0, box.width * 0.8, box.height])
    ax.legend(loc='center left', bbox_to_anchor=(1, 0.5), prop={'size':12})
    
    if state != []:
        bx = plt.subplot(gs[1])
        for i in range(len(states)):
            try:
                plt.plot(time,states[i], '-',label = state_names[i])
            except:
                print "Cannot plot state:", state_names[i]

        box = bx.get_position()
        bx.set_position([box.x0, box.y0, box.width * 0.8, box.height])
        bx.legend(loc='center left', bbox_to_anchor=(1, 0.5), prop={'size':10})
        bx.set_ylim([-.2,1.2])
    plt.show()
    

except KeyboardInterrupt:
    print('You cancelled the operation early.')
'''
except:
    print "Some other error?"
    '''