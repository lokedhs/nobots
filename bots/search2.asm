move 0,CB_TAKE_DAMAGE
move 1,CB_ENEMY_FOUND
move 2,CB_ENEMY_NOT_FOUND
move 3,CB_WALL_FOUND
move 4,CB_WALL_NOT_FOUND
move 5,CB_COLLIDE_WALL
move 6,CB_MESSAGE_RECEIEVED
move 7,CB_TIMEOUT
move 1000,CLOSE_WALL
move 0,target_dir
move 0,robot_search_dir
move 0,is_hit
move 1,tmpvar_1
neg tmpvar_1
move tmpvar_1,take_damage_timer
move 1,tmpvar_1
neg tmpvar_1
move tmpvar_1,evade_dir
callback CB_ENEMY_FOUND,%enemy_found_cb
callback CB_ENEMY_NOT_FOUND,%enemy_not_found_cb
callback CB_TAKE_DAMAGE,%take_damage
callback CB_TIMEOUT,%timeout_cb
L1:
jeq 1,%L2
push target_dir
jsr %move_until_wall
push target_dir
jsr %track_wall
jeq return_val,%L3
move target_dir,tmpvar_1
add 1800,tmpvar_1
mod 3600,tmpvar_1
move tmpvar_1,target_dir
L3:
jmp %L1
L2:
jmp %L4
move_until_wall:
pop tmpvar_move_until_wall_1
push dir
move tmpvar_move_until_wall_1,dir
getcallback CB_WALL_FOUND,tmpvar_move_until_wall_2
move tmpvar_move_until_wall_2,old_wall_found
callback CB_WALL_FOUND,%move_until_wall_callback
push dir
setheading
L6:
getheading
pop tmpvar_move_until_wall_1
tstne dir,tmpvar_move_until_wall_1
jeq tmpvar_move_until_wall_1,%L7
jsr %search_for_robots
jmp %L6
L7:
getmaxspeed
pop tmpvar_move_until_wall_1
push tmpvar_move_until_wall_1
setspeed
move 0,foundwall
L8:
move foundwall,tmpvar_move_until_wall_1
not tmpvar_move_until_wall_1
jeq tmpvar_move_until_wall_1,%L9
push CLOSE_WALL
push dir
wallscan
jsr %search_for_robots
jmp %L8
L9:
push 0
setspeed
callback CB_WALL_FOUND,old_wall_found
L5:
pop dir
rts
L4:
jmp %L10
move_until_wall_callback:
move 1,foundwall
L11:
rtc
L10:
jmp %L12
track_wall:
pop tmpvar_track_wall_1
push dir
move tmpvar_track_wall_1,dir
getcallback CB_WALL_FOUND,tmpvar_track_wall_2
move tmpvar_track_wall_2,old_wall_found
getcallback CB_WALL_NOT_FOUND,tmpvar_track_wall_1
move tmpvar_track_wall_1,old_wall_not_found
callback CB_WALL_NOT_FOUND,%track_wall_no_found_wall
getcallback CB_COLLIDE_WALL,tmpvar_track_wall_1
move tmpvar_track_wall_1,old_collide_wall
callback CB_COLLIDE_WALL,%track_wall_collide
jsr %rand_sign
move return_val,rs
move 900,tmpvar_track_wall_1
mul rs,tmpvar_track_wall_1
push tmpvar_track_wall_1
push dir
jsr %change_dir
move return_val,newdir
move 0,clear_way_found
move CLOSE_WALL,tmpvar_track_wall_1
mul 4,tmpvar_track_wall_1
push tmpvar_track_wall_1
push newdir
wallscan
move clear_way_found,tmpvar_track_wall_1
not tmpvar_track_wall_1
jeq tmpvar_track_wall_1,%L14
move rs,tmpvar_track_wall_2
neg tmpvar_track_wall_2
move 900,tmpvar_track_wall_3
mul tmpvar_track_wall_2,tmpvar_track_wall_3
push tmpvar_track_wall_3
push dir
jsr %change_dir
move return_val,newdir
move 0,clear_way_found
move CLOSE_WALL,tmpvar_track_wall_1
mul 4,tmpvar_track_wall_1
push tmpvar_track_wall_1
push newdir
wallscan
move clear_way_found,tmpvar_track_wall_1
not tmpvar_track_wall_1
jeq tmpvar_track_wall_1,%L15
move 1,return_val
jmp %L13
L15:
L14:
push newdir
setheading
L16:
getheading
pop tmpvar_track_wall_1
tstne newdir,tmpvar_track_wall_1
jeq tmpvar_track_wall_1,%L17
jsr %search_for_robots
jmp %L16
L17:
getmaxspeed
pop tmpvar_track_wall_1
push tmpvar_track_wall_1
setspeed
move 0,clear_way_found
move 0,collided
L18:
move clear_way_found,tmpvar_track_wall_1
not tmpvar_track_wall_1
move collided,tmpvar_track_wall_2
not tmpvar_track_wall_2
tstand tmpvar_track_wall_2,tmpvar_track_wall_1
jeq tmpvar_track_wall_1,%L19
move CLOSE_WALL,tmpvar_track_wall_3
mul 4,tmpvar_track_wall_3
push tmpvar_track_wall_3
push dir
wallscan
jsr %search_for_robots
jmp %L18
L19:
callback CB_WALL_FOUND,old_wall_found
callback CB_WALL_NOT_FOUND,old_wall_not_found
callback CB_COLLIDE_WALL,old_collide_wall
move collided,return_val
jmp %L13
L13:
pop dir
rts
L12:
jmp %L20
track_wall_no_found_wall:
move 1,clear_way_found
L21:
rtc
L20:
jmp %L22
track_wall_collide:
move 1,collided
L23:
rtc
L22:
jmp %L24
search_for_robots:
getheading
pop tmpvar_search_for_robots_1
move tmpvar_search_for_robots_1,old_heading
jsr %evade_shots
move 0,was_found
move 0,enemy_found
push 900
push scan_direction
scan
pop tmpvar_search_for_robots_1
tsteq 0,tmpvar_search_for_robots_1
jeq tmpvar_search_for_robots_1,%L26
push 900
push scan_direction
jsr %change_dir
move return_val,scan_direction
L26:
L27:
jeq enemy_found,%L28
jsr %evade_shots
move 1,was_found
push enemy_dir
fire
pop tmpvar_search_for_robots_1
move is_hit,tmpvar_search_for_robots_1
not tmpvar_search_for_robots_1
jeq tmpvar_search_for_robots_1,%L29
push enemy_dir
setheading
L29:
move enemy_dist,tmpvar_search_for_robots_1
tstgt 200,tmpvar_search_for_robots_1
jeq tmpvar_search_for_robots_1,%L30
push enemy_speed
setspeed
jmp %L31
L30:
push 0
setspeed
L31:
push 450
push enemy_dir
scan
pop tmpvar_search_for_robots_1
jmp %L27
L28:
jeq was_found,%L32
push oldheading
setheading
getmaxspeed
pop tmpvar_search_for_robots_1
push tmpvar_search_for_robots_1
setspeed
L32:
L25:
rts
L24:
jmp %L33
evade_shots:
jeq is_hit,%L35
move 1,tmpvar_evade_shots_1
neg tmpvar_evade_shots_1
move evade_dir,tmpvar_evade_shots_2
tstne tmpvar_evade_shots_1,tmpvar_evade_shots_2
getheading
pop tmpvar_evade_shots_3
tsteq evade_dir,tmpvar_evade_shots_3
tstand tmpvar_evade_shots_3,tmpvar_evade_shots_2
jeq tmpvar_evade_shots_2,%L36
getheading
pop tmpvar_evade_shots_4
jsr %rand_sign
move 450,tmpvar_evade_shots_5
mul return_val,tmpvar_evade_shots_5
push tmpvar_evade_shots_5
push tmpvar_evade_shots_4
jsr %change_dir
move return_val,evade_dir
push evade_dir
setheading
L36:
getmaxspeed
pop tmpvar_evade_shots_1
push tmpvar_evade_shots_1
setspeed
L35:
L34:
rts
L33:
jmp %L37
enemy_found_cb:
move 1,enemy_found
getcb 0,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_x
getcb 1,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_y
getcb 2,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_dir
getcb 3,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_dist
getcb 4,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_heading
getcb 5,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_speed
getcb 6,tmpvar_enemy_found_cb_1
move tmpvar_enemy_found_cb_1,enemy_id
L38:
rtc
L37:
jmp %L39
enemy_not_found_cb:
move 0,enemy_found
L40:
rtc
L39:
jmp %L41
take_damage:
move 1,is_hit
move 1,tmpvar_take_damage_1
neg tmpvar_take_damage_1
move take_damage_timer,tmpvar_take_damage_2
tstne tmpvar_take_damage_1,tmpvar_take_damage_2
jeq tmpvar_take_damage_2,%L43
push take_damage_timer
deltimer
L43:
push 50
addtimer
pop tmpvar_take_damage_1
move tmpvar_take_damage_1,take_damage_timer
L42:
rtc
L41:
jmp %L44
timeout_cb:
getcb 0,tmpvar_timeout_cb_1
tsteq take_damage_timer,tmpvar_timeout_cb_1
jeq tmpvar_timeout_cb_1,%L46
move 0,is_hit
move 1,tmpvar_timeout_cb_1
neg tmpvar_timeout_cb_1
move tmpvar_timeout_cb_1,take_damage_timer
move 1,tmpvar_timeout_cb_1
neg tmpvar_timeout_cb_1
move tmpvar_timeout_cb_1,evade_dir
L46:
L45:
rtc
L44:
jmp %L47
rand_sign:
random tmpvar_rand_sign_1
mod 2,tmpvar_rand_sign_1
tsteq 0,tmpvar_rand_sign_1
jeq tmpvar_rand_sign_1,%L49
move 1,tmpvar_rand_sign_2
neg tmpvar_rand_sign_2
move tmpvar_rand_sign_2,return_val
jmp %L48
jmp %L50
L49:
move 1,return_val
jmp %L48
L50:
L48:
rts
L47:
jmp %L51
change_dir:
pop tmpvar_change_dir_1
pop tmpvar_change_dir_2
push dir
push val
move tmpvar_change_dir_1,dir
move tmpvar_change_dir_2,val
move dir,tmpvar_change_dir_3
add val,tmpvar_change_dir_3
mod 3600,tmpvar_change_dir_3
move tmpvar_change_dir_3,tmp
move tmp,tmpvar_change_dir_1
tstlt 0,tmpvar_change_dir_1
jeq tmpvar_change_dir_1,%L53
add 3600,tmp
L53:
move tmp,return_val
jmp %L52
L52:
pop val
pop dir
rts
L51:
