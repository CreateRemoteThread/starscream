#!/usr/bin/python

import sys
import binascii
import time
import os
import random
from scapy.all import *

def get_packet_layers(packet):
  counter = 0
  while True:
    layer = packet.getlayer(counter)
    if layer is None:
      break
    yield layer
    counter += 1

f = open("keys.csv","r")
data = f.readlines()
f.close()

keymatrix = {}

c = 0
for l_ in data:
  (key,code) = l_.rstrip().split(":")
  keymatrix[code] = key
  c += 1

print "[+] Loading composite key recognizer..."

keydetail = {}
for i in range(0,0x20):
  keydetail[i] = {}

for l_ in data:
  (key,code) = l_.rstrip().split(":")
  col = int(code[0:2],16)
  row = 0xFF ^ int(code[2:4],16)
  keydetail[col][row] = key

print "[+] Key lookups enabled, %d raw keys..." % c

def collectKeys(keycode):
  col = int(code[0:2],16)
  row = 0xFF ^ int(code[2:4],16)
  for or_l in keydetail[col].keys():
    if or_l & row != 0:
      pass
      # out += keydetail[col][or_l]
  # return out

def packetCallback(pkt):
  if pkt.haslayer(Dot11):
    if pkt.addr2 == "22:44:66:88:ff:00":
      t = binascii.hexlify( pkt.info)
      if t not in keymatrix.keys():
        return collectKeys(t)
        # return "** COMPOSITE KEY : %s **" % t
      elif len(keymatrix[t]) == 1:
        return keymatrix[t]
      elif keymatrix[t] == "SPACE":
        return " "
      else:
        return "[" + keymatrix[t] + "]"

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print "=" * 80
    print "usage: ./decode-live.py [iface]"
    sys.exit(0)
  print "[+] Initialization complete!"
  print "=" * 80
  sniff(iface=sys.argv[1],prn=packetCallback)
