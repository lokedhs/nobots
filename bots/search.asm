move 100000,MAP_MAX_X
move 100000,MAP_MAX_Y
move 500,CLOSE_DISTANCE
move 10000,ROW_HEIGHT
move 500,DIST_FROM_WALL
callback 1,%enemy_found
move DIST_FROM_WALL,curr_row
move 0,curr_col
L1:
jeq 1,%L2
push curr_row
push curr_col
jsr %move_to
move 1,tmpvar_1
neg tmpvar_1
move return_val,tmpvar_2
tsteq tmpvar_1,tmpvar_2
jeq tmpvar_2,%L3
L4:
jeq found,%L5
push founddir
setheading
getheading
pop tmpvar_1
push founddir
push tmpvar_1
jsr %direction_diff
move return_val,tmpvar_2
tstlt 450,tmpvar_2
jeq tmpvar_2,%L6
push foundspeed
setspeed
jmp %L7
L6:
push 0
setspeed
L7:
push founddir
fire
pop tmpvar_1
L8:
getweaponstatus
pop tmpvar_1
tstgt 0,tmpvar_1
getscannerstatus
pop tmpvar_2
tstgt 0,tmpvar_2
tstand tmpvar_2,tmpvar_1
jeq tmpvar_1,%L9
jmp %L8
L9:
getscannerstatus
pop tmpvar_1
tsteq 0,tmpvar_1
jeq tmpvar_1,%L10
move 0,found
push 200
push founddir
scan
pop tmpvar_1
L10:
jmp %L4
L5:
L3:
add ROW_HEIGHT,curr_row
move MAP_MAX_Y,tmpvar_1
sub DIST_FROM_WALL,tmpvar_1
move curr_row,tmpvar_2
tstge tmpvar_1,tmpvar_2
jeq tmpvar_2,%L11
move 0,curr_row
L11:
move curr_col,tmpvar_1
tstgt DIST_FROM_WALL,tmpvar_1
jeq tmpvar_1,%L12
move DIST_FROM_WALL,curr_col
jmp %L13
L12:
move MAP_MAX_X,tmpvar_1
sub DIST_FROM_WALL,tmpvar_1
move tmpvar_1,curr_col
L13:
jmp %L1
L2:
jmp %L14
move_to:
pop tmpvar_move_to_1
pop tmpvar_move_to_2
push x
push y
move tmpvar_move_to_1,x
move tmpvar_move_to_2,y
getcallback 5,tmpvar_move_to_3
move tmpvar_move_to_3,old_cw
callback 5,%move_to_collide_wall
move 0,stopped
move 0,found
getxcoordinate
pop tmpvar_move_to_1
move tmpvar_move_to_1,my_x
getycoordinate
pop tmpvar_move_to_1
move tmpvar_move_to_1,my_y
move x,tmpvar_move_to_1
sub my_x,tmpvar_move_to_1
move tmpvar_move_to_1,dx
move y,tmpvar_move_to_1
sub my_y,tmpvar_move_to_1
move tmpvar_move_to_1,dy
push dy
push dx
c_to_a
pop tmpvar_move_to_1
move tmpvar_move_to_1,heading
push dy
push dx
c_to_r
pop tmpvar_move_to_1
move tmpvar_move_to_1,range
move range,tmpvar_move_to_1
tstlt CLOSE_DISTANCE,tmpvar_move_to_1
jeq tmpvar_move_to_1,%L16
callback 5,old_cw
move range,return_val
jmp %L15
L16:
push heading
setheading
L17:
getheading
pop tmpvar_move_to_1
tstne heading,tmpvar_move_to_1
jeq tmpvar_move_to_1,%L18
jmp %L17
L18:
getmaxspeed
pop tmpvar_move_to_1
push tmpvar_move_to_1
setspeed
move 0,scandir
L19:
move stopped,tmpvar_move_to_1
not tmpvar_move_to_1
move found,tmpvar_move_to_2
not tmpvar_move_to_2
tstand tmpvar_move_to_2,tmpvar_move_to_1
move range,tmpvar_move_to_3
tstgt CLOSE_DISTANCE,tmpvar_move_to_3
tstand tmpvar_move_to_3,tmpvar_move_to_1
jeq tmpvar_move_to_1,%L20
getxcoordinate
pop tmpvar_move_to_4
move x,tmpvar_move_to_5
sub tmpvar_move_to_4,tmpvar_move_to_5
move tmpvar_move_to_5,dx
getycoordinate
pop tmpvar_move_to_1
move y,tmpvar_move_to_2
sub tmpvar_move_to_1,tmpvar_move_to_2
move tmpvar_move_to_2,dy
push dy
push dx
c_to_a
pop tmpvar_move_to_1
move tmpvar_move_to_1,heading
push dy
push dx
c_to_r
pop tmpvar_move_to_1
move tmpvar_move_to_1,range
push heading
setheading
getscannerstatus
pop tmpvar_move_to_1
tsteq 0,tmpvar_move_to_1
jeq tmpvar_move_to_1,%L21
add 450,scandir
move scandir,tmpvar_move_to_1
tstge 3600,tmpvar_move_to_1
jeq tmpvar_move_to_1,%L22
move 0,scandir
L22:
push 450
push scandir
scan
pop tmpvar_move_to_1
L21:
jmp %L19
L20:
move stopped,tmpvar_move_to_1
not tmpvar_move_to_1
jeq tmpvar_move_to_1,%L23
push 0
setspeed
L24:
getspeed
pop tmpvar_move_to_1
tstgt 0,tmpvar_move_to_1
jeq tmpvar_move_to_1,%L25
jmp %L24
L25:
L23:
jeq found,%L26
move 1,tmpvar_move_to_1
neg tmpvar_move_to_1
move tmpvar_move_to_1,return_val
jmp %L15
L26:
move range,return_val
jmp %L15
L15:
pop y
pop x
rts
L14:
jmp %L27
direction_diff:
pop tmpvar_direction_diff_1
pop tmpvar_direction_diff_2
push r1
push r2
move tmpvar_direction_diff_1,r1
move tmpvar_direction_diff_2,r2
move r2,tmpvar_direction_diff_3
sub r1,tmpvar_direction_diff_3
push tmpvar_direction_diff_3
jsr %abs
move return_val,diff
move diff,tmpvar_direction_diff_1
tstgt 1800,tmpvar_direction_diff_1
jeq tmpvar_direction_diff_1,%L29
move 3600,tmpvar_direction_diff_2
sub diff,tmpvar_direction_diff_2
move tmpvar_direction_diff_2,diff
L29:
move diff,return_val
jmp %L28
L28:
pop r2
pop r1
rts
L27:
jmp %L30
abs:
pop tmpvar_abs_1
push n
move tmpvar_abs_1,n
move n,tmpvar_abs_2
tstlt 0,tmpvar_abs_2
jeq tmpvar_abs_2,%L32
move n,tmpvar_abs_3
neg tmpvar_abs_3
move tmpvar_abs_3,n
L32:
move n,return_val
jmp %L31
L31:
pop n
rts
L30:
jmp %L33
move_to_collide_wall:
move 1,stopped
L34:
rtc
L33:
jmp %L35
enemy_found:
move 1,found
getcb 2,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,founddir
getcb 3,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,founddist
getcb 4,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,foundheading
getcb 5,tmpvar_enemy_found_1
move tmpvar_enemy_found_1,foundspeed
L36:
rtc
L35:
