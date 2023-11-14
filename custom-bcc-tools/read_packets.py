#!/usr/bin/python
#
# tcpv4connect	Trace TCP IPv4 connect()s.
#		For Linux, uses BCC, eBPF. Embedded C.
#
# USAGE: tcpv4connect [-h] [-t] [-p PID]
#
# This is provided as a basic example of TCP connection & socket tracing.
#
# All IPv4 connection attempts are traced, even if they ultimately fail.
#
# Copyright (c) 2015 Brendan Gregg.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# 15-Oct-2015	Brendan Gregg	Created this.

from __future__ import print_function
from bcc import BPF
from bcc.utils import printb

# define BPF program
bpf_text = open('read_packets.c').read()

# initialize BPF
b = BPF(text=bpf_text)

# header
print("%-6s %-12s %-16s %-16s %-4s" % ("PID", "COMM", "SADDR", "DADDR",
    "DPORT"))

def inet_ntoa(addr):
	dq = b''
	for i in range(0, 4):
		dq = dq + str(addr & 0xff).encode()
		if (i != 3):
			dq = dq + b'.'
		addr = addr >> 8
	return dq

# filter and format output
while 1:
	# Read messages from kernel pipe
	try:
	    (task, pid, cpu, flags, ts, msg) = b.trace_fields()
	    (_tag, saddr_hs, daddr_hs, dport_s) = msg.split(b" ")
	except ValueError:
	    # Ignore messages from other tracers
	    continue
	except KeyboardInterrupt:
	    exit()

	# Ignore messages from other tracers
	if _tag.decode() != "trace_tcp4connect":
	    continue

	printb(b"%-6d %-12.12s %-16s %-16s %-4s" % (pid, task,
	    inet_ntoa(int(saddr_hs, 16)),
	    inet_ntoa(int(daddr_hs, 16)),
	    dport_s))
