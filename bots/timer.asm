$define CB_TAKE_DAMAGE		= 0
$define CB_TD_AMOUNT		= 0

$define CB_ENEMY_FOUND		= 1
$define CB_EF_X			= 0
$define CB_EF_Y			= 1
$define CB_EF_DIR		= 2
$define CB_EF_DISTANCE		= 3
$define CB_EF_HEADING		= 4
$define CB_EF_SPEED		= 5
$define CB_EF_ID		= 6

$define CB_ENEMY_NOT_FOUND	= 2

$define CB_WALL_FOUND		= 3
$define CB_WF_X			= 0
$define CB_WF_Y			= 1
$define CB_WF_DISTANCE		= 2

$define CB_WALL_NOT_FOUND	= 4

$define CB_COLLIDE_WALL		= 5

$define CB_MESSAGE_RECEIEVED	= 6
$define CB_MR_SENDER		= 0
$define CB_MR_ID		= 1
$define CB_MR_LENGTH		= 2

$define CB_TIMEOUT		= 7
$define CB_TO_ID		= 0


	callback (CB_TIMEOUT),%timeout
	push	50
	addtimer

	move	0,currtime
loop:
	uptime
	pop	ut
	move	ut,tmp
	tsteq	currtime,tmp
	jne	tmp,%nochange
	print	ut
	move	ut,currtime
nochange:
	jmp	%loop


timeout:
	getcb	(CB_TO_ID),id
	print	(-9999)
	print	id

	uptime
	pop	timeouttmp
	add	20,timeouttmp
	push	timeouttmp
	addtimer

	rtc
