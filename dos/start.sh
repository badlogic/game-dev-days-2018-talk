#!/bin/sh
/Applications/DOSBox.app/Contents/MacOS/DOSBox \
	-conf dosbox.conf \
	-c "mount c ./" \
	-c "PATH=z:\;c:\masm;c:\qb45" \
	-c "c:" \
	-c "cd rtm"
	# -c "qb source\rtm.bas /l source\d3d"