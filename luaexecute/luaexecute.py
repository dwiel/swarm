#!/usr/bin/env python
# -*- coding: utf-8 -*-
import osc
import sys

osc.init()

#for line in sys.stdin :
lua = ""
print "enter command:"
while lua != "quit\n" and lua != "exit\n":
	line = sys.stdin.readline()
	lua += line
	if line == "" or line == "\n" :
		bundle = osc.createBundle()
		# osc.appendToBundle(bundle, "/lua/execute", 'groups["swarm1"].pos.z = -30')
		osc.appendToBundle(bundle, "/lua/execute", lua)
		osc.sendBundle(bundle, "localhost", 9001)
		print "sent:\n  >" + lua.replace('\n', '\n  >')
		print "------------"
		print "enter command:"
		lua = ""



