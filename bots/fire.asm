callback 1,%enemy_found
callback 2,%enemy_not_found
L1:
jeq 1,%L2
move 0,dir
move 0,found
L3:
move found,tmpvar_1
not tmpvar_1
move dir,tmpvar_2
tstlt 3600,tmpvar_2
tstand tmpvar_2,tmpvar_1
jeq tmpvar_1,%L4
add 225,dir
L5:
push 225
push dir
scan
pop tmpvar_1
tstne 0,tmpvar_1
jeq tmpvar_1,%L6
jmp %L5
L6:
jmp %L3
L4:
jeq found,%L7
push founddir
fire
pop tmpvar_1
jmp %L8
L7:
jsr %move_random
L8:
jmp %L1
L2:
jmp %L9
move_random:
random tmpvar_move_random_1
mod 3600,tmpvar_move_random_1
move tmpvar_move_random_1,newheading
push newheading
setheading
uptime
pop tmpvar_move_random_1
move tmpvar_move_random_1,ustart
getmaxspeed
pop tmpvar_move_random_1
push tmpvar_move_random_1
setspeed
L11:
getspeed
pop tmpvar_move_random_1
getmaxspeed
pop tmpvar_move_random_2
tstne tmpvar_move_random_2,tmpvar_move_random_1
getheading
pop tmpvar_move_random_3
tstne newheading,tmpvar_move_random_3
tstand tmpvar_move_random_3,tmpvar_move_random_1
jeq tmpvar_move_random_1,%L12
jmp %L11
L12:
uptime
pop tmpvar_move_random_1
sub ustart,tmpvar_move_random_1
move tmpvar_move_random_1,acctime
uptime
pop tmpvar_move_random_1
move tmpvar_move_random_1,wstart
L13:
uptime
pop tmpvar_move_random_1
sub wstart,tmpvar_move_random_1
tstlt acctime,tmpvar_move_random_1
jeq tmpvar_move_random_1,%L14
jmp %L13
L14:
push 0
setspeed
L15:
getspeed
pop tmpvar_move_random_1
tstne 0,tmpvar_move_random_1
jeq tmpvar_move_random_1,%L16
jmp %L15
L16:
L10:
rts
L9:
jmp %L17
enemy_found:
move 1,found
getcb 0,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,foundx
getcb 1,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,foundy
getcb 2,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,founddir
getcb 3,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,founddist
L18:
rtc
L17:
jmp %L19
enemy_not_found:
move 0,found
L20:
rtc
L19:
