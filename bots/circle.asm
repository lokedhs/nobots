	getmaxspeed
	setspeed

loop:
	move	0,curdir
next_turn_part:
	push	curdir
	jsr	%set_and_wait
	add	1000,curdir
	move	curdir,tmp
	tstle	3600,tmp
	jne	tmp,%next_turn_part
	jmp	%loop

set_and_wait:
	pop	todir
	push	todir
	setheading
next_wait_heading:
	getheading
	pop	currheading
	tsteq	todir,currheading
	jeq	currheading,%next_wait_heading
	rts
