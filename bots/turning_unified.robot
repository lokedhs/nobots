name:		Turning unified code
accel:		20
brake:		50
maxspeed:	250
turnspeed:	35
shields:	1000
end

$define CB_WALL_NOT_FOUND = 4
$define CB_COLLIDE_WALL = 5

callback (CB_COLLIDE_WALL),%collide
callback (CB_WALL_NOT_FOUND),%no_wallfound
#callback 5,%collide
#callback 4,%no_wallfound

move 0,mode
getmaxspeed
pop tmpvar_1
push tmpvar_1
setspeed
L1:
jeq 1,%L2
move mode,tmpvar_1
tsteq 0,tmpvar_1
jeq tmpvar_1,%L3
random tmpvar_2
mod 3600,tmpvar_2
move tmpvar_2,heading
push heading
setheading
L4:
getheading
pop tmpvar_1
tstne heading,tmpvar_1
jeq tmpvar_1,%L5
jmp %L4
L5:
jmp %L6
L3:
move 1,found_wall
L7:
jeq found_wall,%L8
random tmpvar_1
mod 3600,tmpvar_1
move tmpvar_1,dir
push dir
setheading
L9:
getheading
pop tmpvar_1
tstne dir,tmpvar_1
jeq tmpvar_1,%L10
jmp %L9
L10:
push dir
wallscan
jmp %L7
L8:
getmaxspeed
pop tmpvar_1
push tmpvar_1
setspeed
move 0,mode
L6:
jmp %L1
L2:
jmp %L11
collide:
move 1,mode
L12:
rtc
L11:
jmp %L13
no_wallfound:
move 0,found_wall
L14:
rtc
L13:
