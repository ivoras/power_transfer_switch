# Automatic power transfer switch

This program uses the [PZEM004T](http://www.ebay.com/sch/i.html?_from=R40&_trksid=p2050601.m570.l1313.TR0.TRC0.H0.Xpzem004t.TRS0&_nkw=pzem004t&_sacat=0) dogital multimeter with TTY output to monitor a power source, like an off-grid solar panel, and transfer between it and another, presumably stable power source, like the mains.

It uses a 4-port relay, where it turns on the first two relays when the measured voltage on the first power source is above a threshold (buffered over time), or the second two relays when it's lower. The switching is done intelligently, by disconnecting the currently enabled pair before enabling the other.

It is indended to be used with a UPS or a similar battery-powered device.


