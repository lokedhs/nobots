	callback 3,%wallfound
	callback 4,%nowallfound

	move	0,curdir

loop:

	getmaxspeed
	setspeed

walk_until_wall:
	getheading
	wallscan
	jeq	wf,%walk_until_wall

	jsr	%stop_robot

turn_free_wall:
	random	curdir
	mod	3600,curdir
	push	curdir
	jsr	%turn_to

	getheading
	wallscan
	
	jne	wf,%turn_free_wall

	jmp	%loop


stop_robot:
	push	0
	setspeed
ws:	getspeed
	pop	spd
	jne	spd,%ws
	rts

turn_to:
	pop	tmpdir
	push	tmpdir
	setheading
wturn:	getheading
	pop	heading
	tsteq	tmpdir,heading
	jeq	heading,%wturn
	rts

wallfound:
	move	1,wf
	rtc

nowallfound:
	move	0,wf
	rtc
