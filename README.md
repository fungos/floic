[![Build Status](https://travis-ci.org/fungos/floic.svg?branch=master)](https://travis-ci.org/fungos/floic)

fungos' low orbit ion cannon v1.0

intro

This is my way to contribute to the Freedom fighters, hope it will be useful.
I'm planning other features focused at distributing bots between a wide range 
of servers and a tool to control from various servers at one time.

I release this code freely and take no responsability on how anyone uses it.
It is yours, have fun.

usage

./floic

this will auto-find hivemind control server and start working.

for more advanced usage:

-s <server> 	- specify which irc server connect.
-p <port> 	- specify witch irc port to use.
-c <channel> 	- specify the name of the command channel.
-! "lazor ..."  - start in manual mode (ie. will not use hivemind).
		  you shoud use the command line as a control server would use, 
		  but without "!lazor", use "lazor" instead.
		  ie.: ./floic -! "lazor target=www.example.com port=80 threads=10 speed=1 start"
-v		- verbosity level (default is quiet), repeat this to increment
	          the verbosity level ie.: ./floic -v -v -v
-h		- print floic help message.

config.txt

this file contains the address, port and channel for various servers to try 
until find a control channel, ie:

irc.example.com:6667:#loic
111.222.111.222:6666:#chanx

floic will try to connect each server and join the channel specified, if 
connection is not possible, it will go to the next entry and so on. If it can 
connect the server, it will try to join the specified channel to accept 
commands, but if the channel is inacessible, it will try another common 
channels (#loic, #floic, #ciol, #loicswe that are hardcoded for now).

compiling

just type 'make' it should do it, otherwise I need volunteers.

dependency

libircclient
libpthreads

both are common and your distro (should) have it. just run make.

windows

use cygwin to compile, download libircclient from sourceforge. fix any paths to
the libircclient if needed. should build on cygwin without any issue.

macosx

I will try to build for maxos soon, but for now, any volunteers?

have fun!
stay safe.
