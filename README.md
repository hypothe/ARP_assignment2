# PLACEHOLDER README

## Time table
Approximative, not hard constraints but suggestions, can obviosuly be modified to accomodate for busy days.

* ~~__Dec 03 testing the waters__: simple client and simple server, simply establish connection and transfer a message and a 
								second socket for the reply, platform for the delivery of status messages~~
* ~~__Dec 04 echoes from the web__: communication made cyclic, interface sends repeated messages and server that many replies; 
								3 different messages types, each with its appropriate reply (basic state machine)~~
* __Dec 05 preparing the ground__: server produces messages '+' and '-' cyclicaly when in the appropriate statuses, client outputs status updates periodically
* __Dec 06 hoisting up__: hoist process generated, transfer of '+', '-' to it via pipe
* __Dec 07 finalize__: _day to finalize communication not already managed, and clean things up_
* __Dec 08 gone gold__: all code should be completely done by now
* __Dec 09 shippable__: documentation completed
* __Dec 11 hit the shelves__: Assignment2 is due

## Tips/ Notes
A makefile and libraries shouldn't be necessary in this case.
Function documentation should be done lastly, in order to avoid writing and re-writing again things before they're
finalized.
