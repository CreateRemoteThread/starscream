# starscream

*This is a keylogger. There are many like it, but this one is mine.*

## Overview

Starscream is a prototype hardware keylogger, which offers wifi exfiltration via malformed probe responses. It costs less than a (decent) coffee, is small enough to hide in the slack space of a keyboard and most importantly, doesn't interact with USB.

This uses an ATMega328p to sniff the keyboard scan matrix, and an ESP8266 module to exfiltrate (and feasibly, could do some covert CNC via malformed packets). Theoretically, it wouldn't be too far of a stretch to inject keystrokes as well, but that's a slippery slope of scope creep.

