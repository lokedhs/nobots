	move	5000,walklenght

loop:

	jsr	%stop_robot

	push	0
	jsr	%set_heading
	
	getxcoordinate
	pop	destx
	add	walklenght,destx

	getmaxspeed
	setspeed

waitright:
	getxcoordinate
	pop	xpos
	tstge	destx,xpos	# xpos >= destx?
	jeq	xpos,%waitright

	jsr	%stop_robot

	push	1800
	jsr	%set_heading

	getxcoordinate
	pop	destx
	sub	walklenght,destx

	getmaxspeed
	setspeed

waitleft:
	getxcoordinate
	pop	xpos
	tstle	destx,xpos	# xpos <= destx?
	jeq	xpos,%waitleft

	jmp	%loop



#
#  Set the speed to 0 and wait for the robot to stop
#
stop_robot:
	push	0
	setspeed
waitstop:
	getspeed
	pop	spd
	jne	spd,%waitstop

	rts

#
#  Set the heading
#
set_heading:
	pop	newdir
	push	newdir
	setheading
waitheading:
	getheading
	pop	heading

	tsteq	newdir,heading
	jeq	heading,%waitheading

	rts
