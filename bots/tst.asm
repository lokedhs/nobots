#
#  Immediate:		10
#  Named Immediate:	%foo
#  Absolute:		foo
#  Indirect:		&foo
#

$array ary [ 8, 3, 5, 6, 7, 3, 4, 2 ]

	move	(8-1),arraysize


	move	%ary,ptr
	move	arraysize,ctr
next_push:
	push	&ptr
	add	1,ptr
	sub	1,ctr
	jpl	ctr,%next_push


	move	arraysize,ctr
next_pop:
	jsr	%pop_and_print
	sub	1,ctr
	jpl	ctr,%next_pop

	exit


pop_and_print:
	pop	value
	print	value
	rts
