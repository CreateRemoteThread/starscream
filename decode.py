#!/usr/bin/python

import sys
import binascii
from scapy.all import *

def get_packet_layers(packet):
  counter = 0
  while True:
    layer = packet.getlayer(counter)
    if layer is None:
      break
    yield layer
    counter += 1

if __name__ == "__main__":
  if len(sys.argv) != 2:
    print "usage: ./decode.py [chan7-pcap]"
    sys.exit(0)

f = open("keys.csv","r")
data = f.readlines()
f.close()

keymatrix = {}

c = 0
for l_ in data:
  (key,code) = l_.rstrip().split(":")
  keymatrix[code] = key
  c += 1

print "Key matrix loaded, %d keys" % c

out = ""
packets = rdpcap(sys.argv[1])
for pkt in packets:
  if pkt.haslayer(Dot11):
    if pkt.addr2 == "22:44:66:88:ff:00":
      t = binascii.hexlify( pkt.info)
      if t not in keymatrix.keys():
        print "** UNKNOWN : %s **" % t
      if len(keymatrix[t]) == 1:
        out += keymatrix[t]
      elif keymatrix[t] == "SPACE":
        out += " "
      else:
        out += "[" + keymatrix[t] + "]"

print out

