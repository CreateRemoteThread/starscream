#!/usr/bin/python

import sys
import serial
import binascii

log = open("keys.csv","a+")
ser = serial.Serial("/dev/ttyUSB0",9600)

internal_log = {}

while 1:  
  data = ser.read(2)
  klol = binascii.hexlify(data)
  if klol in internal_log.keys():
    print "I know this: you pressed %s" % internal_log[klol]
    continue
  k = raw_input("Which key was %s" % binascii.hexlify(data)).rstrip()
  if k == "quit":
    print "bye!"
    log.close()
    sys.exit(0)
  internal_log[binascii.hexlify(data)] = k
  log.write("%s:%s\n" % (k,binascii.hexlify(data)))  
