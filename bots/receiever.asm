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


$array msgbuf:10

	callback (CB_MESSAGE_RECEIEVED),%get_message
	push	1212
	setchannel
loop:
	jmp	%loop

get_message:
	print	(-9999)

	getcb	(CB_MR_SENDER),sender
	getcb	(CB_MR_ID),id
	getcb	(CB_MR_LENGTH),length

	print	sender
	print	id
	print	length

	push	%msgbuf
	receieve
	move	%msgbuf,w
printloop:
	print	&w
	add	1,w
	sub	1,length
	jne	length,%printloop

	rtc
