	callback	1,%found
#	callback	2,%notfound

	push	0
	setspeed

loop:
	move	0,scandir
	move	0,hasfound
next_scan:
	push	450
	push	scandir

waitforscanner:
	getscannerstatus
	pop	tmp
	jne	tmp,%waitforscanner

	scan
	add	450,scandir
	move	scandir,tmp
	tstlt	3600,tmp
	jne	tmp,%next_scan
	
	jne	hasfound,%xfound
	push	0
	setspeed
	jmp	%xnofound
xfound:
	getmaxspeed
	setspeed
xnofound:

	jmp	%loop


found:
	getcb	0,x
	getcb	1,y

	move	1,hasfound

#	move	10000,x
#	move	10000,y

	getxcoordinate
	pop	tmp
	sub	tmp,x
	getycoordinate
	pop	tmp
	sub	tmp,y

	push	y
	push	x
	c_to_a
	setheading

	getmaxspeed
	setspeed

#	callback	2,(-1)

	rtc


notfound:
	push	0
	setspeed
	rtc
