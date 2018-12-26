
		ORG $8000

		BANK_START $F5

; ---------------------------------------------------------------------------
; --------------E-X-T-E-R-N-A-L--S-Y-S-T-E-M--R-O-U-T-I-N-E-S----------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
_obj_handlers:
		LDA		_game_msg_wnd_active_flag
		BEQ		loc_88006
		RTS
loc_88006:
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_8800F
		LDA		#$50
		STA		_spr_buf_pos
loc_8800F:
		JSR		_obj_spr_bank_select
		LDA		_obj_active_flags
		LSR
		BCC		loc_88032
		JSR		_obj_hndl01_heli
loc_88032:
		LDA		_obj_active_flags
		AND		#$02
		BEQ		loc_88029
		JSR		_obj_hndl02_plane
loc_88029:
		LDA		_obj_active_flags
		AND		#$04
		BEQ		loc_8805A
		JSR		_obj_hndl04_ship
loc_8805A:
		LDA		_obj_active_flags
		AND		#$08
		BEQ		loc_88050
		JSR		_obj_hndl08_train
loc_88050:
		LDA		_obj_active_flags
		AND		#$10
		BEQ		loc_88046
		JSR		_obj_hndl10_monster
loc_88046:
		LDA		_obj_active_flags
		AND		#$20
		BEQ		loc_8803C
		JSR		_obj_hndl20_tornado
loc_8803C:
		LDA		_airplane_crash_req				; special check for airplane crash
		BEQ		loc_8801C
;		JSR		_airplane_crash_start
		LDA		#$00
		STA		_airplane_crash_req
		BEQ		_airplane_crash_start			; OPTIMIZED
loc_8801C:
		LDA		_obj_active_flags
		AND		#$03
		CMP		#$03
		BNE		locret_88076					; special test if heli and plane are in the same
		LDA		_obj_pos._heli._COL				; position currently, they CRAAASHHHHH!!
		CMP		_obj_pos._plane._COL				; so we can crash airplane without request
		BNE		locret_88076					; NOTE, with this crash we destroy only plane, not heli!
		LDA		_obj_pos._heli._ROW
		CMP		_obj_pos._plane._ROW
		BNE		locret_88076
;		JMP		_airplane_crash_start			; OPTIMIZED
_airplane_crash_start:
		LDA		#$22
		STA		_game_msg_idx
		LDX		_obj_pos._plane._COL				; cur plane obj pos set to goto pos
		STX		_goto_dst_tile._COL				; the same time X,Y loaded with the same pos
		LDY		_obj_pos._plane._ROW
;		STX		_goto_dst_tile._ROW				; BUG! Y loaded, X stored instead!
		STY		_goto_dst_tile._ROW				; FIX: correct opcode
		LDA		#$01
		STA		_goto_alert_req
		STA		_game_msg_awaits_flag
		JSR		_far_fire_or_destroy_map_tile_ex; X,Y aregs goes here
		LDA		_obj_active_flags				; destroy plane
		AND		#$FD
		STA		_obj_active_flags
		LDA		#$04
		STA		_apu_se_flags+1					; crash sound
		PUSHB	_ptr0
		PUSHB	_ptr0+1
		PUSHB	_tmp2
		PUSHB	_tmp3
		PUSHB	_tmp4
		PUSHB	_tmp5
		LDY		#$08
		FJSRA	_city_history_write,SRAM,PRGA	; to memories!
		POPB	_tmp5
		POPB	_tmp4
		POPB	_tmp3
		POPB	_tmp2
		POPB	_ptr0+1
		POPB	_ptr0
locret_88076:
		RTS

; =============== S U B	R O U T	I N E =======================================
; every	50 years the CHR bank for object are changes
; 1900 - bank 38, 39
; 1950 - bank 3A, 3B
; 2000 - bank 3C, 3D
; 2050 - bank 3E, 3F
_obj_spr_bank_select:
		LDX		#$00							; subtract 1950 by default
		LDA		_city._cur_year
		SEC
		SBC		#$9E
		STA		_tmp15
		LDA		_city._cur_year+1
		SBC		#$07
		STA		_tmp15+1
		BCC		loc_88D25						; all lower gets default sprites
loc_88D11:
		INX
		CPX		#$03							; then subtract 50 from the rest
		BCS		loc_88D25						; up to 3 times
		LDA		_tmp15
		SEC
		SBC		#$32
		STA		_tmp15
		LDA		_tmp15+1
		SBC		#$00
		STA		_tmp15+1
		BCS		loc_88D11
loc_88D25:
		LDA		byte_88D31,X					; we have 4 periods with different
		STA		_MMC5_CHR_BANKSA+2				; vehicle graphics
		TAX
		INX
		STX		_MMC5_CHR_BANKSA+3
		RTS
byte_88D31:
		.BYTE	$38,$3A,$3C,$3E

; =============== S U B	R O U T	I N E =======================================
_obj_hndl01_heli:
		LDA		#$00
		STA		_cur_obj_idx
		JSR		_obj_fetch
		LDX		_frames_div8_counter
		CPX		_cur_obj_idx
		BNE		loc_8822A
		LDA		_obj_spr_delta._heli
		BEQ		loc_8821A
		JMP		_obj_rotate
loc_8821A:
		JSR		_obj_flying_dst_test_adjust
		CMP		#$FF
		BNE		loc_88224
		JSR		_obj_flying_retarget
		JMP		_obj_draw
loc_88224:
		STA		_obj_cur_spr_idx._heli
		JSR		_cur_obj_move
loc_8822A:
		JMP		_obj_draw

; =============== S U B	R O U T	I N E =======================================
_obj_hndl02_plane:
		LDA		#$01
		STA		_cur_obj_idx
		JSR		_obj_fetch
		BIT		_obj_plane_anim_idx				; cur plane anim mode, 80 means
		BPL		loc_880DC						; take-off/landing
		JMP		_obj_plane_landing_draw
loc_880DC:
		LDX		_frames_div8_counter
		CPX		_cur_obj_idx
		BNE		loc_88101
		LDA		_obj_spr_delta._plane			; spr delta 0 - no need to change spr
		BEQ		loc_880EB
		JMP		_obj_rotate						; or else do it
loc_880EB:
		JSR		_obj_flying_dst_test_adjust
		CMP		#$FF
		BNE		loc_880F8
		JSR		_obj_flying_retarget
		JMP		_obj_draw
loc_880F8:
		STA		_obj_cur_spr_idx._plane
		JSR		_cur_obj_move
		JSR		_obj_plane_landing_test
loc_88101:
		JMP		_obj_draw

; =============== S U B	R O U T	I N E =======================================
_obj_plane_landing_draw:
		LDX		_frames_div8_counter
		CPX		_cur_obj_idx
		BNE		loc_88171
		JSR		_cur_obj_move
loc_88171:
		DEC		_obj_plane_anim_timer
		BNE		loc_88196
		LDA		#$20
		STA		_obj_plane_anim_timer
		LDA		_obj_plane_anim_idx
		AND		#$7F
		SEC
		SBC		#$01
		BEQ		loc_881A1
		ORA		#$80
		STA		_obj_plane_anim_idx
		CMP		#$84
		BNE		loc_88193
		LDA		#$06
		STA		_obj_cur_spr_idx._plane
loc_88193:
		JSR		_cur_obj_move
loc_88196:
		LDA		_obj_plane_anim_idx
		AND		#$7F
		TAX
		LDA		_obj_plane_takoff_anim_list,X
		BNE		loc_881A6
loc_881A1:
		STA		_obj_plane_anim_idx
		LDA		#$08
loc_881A6:
		STA		_spr_insert_args._idx
		LDX		_game_wnd_spr_mode_flag
		BEQ		loc_881B3
		JSR		_map_scr_obj_spr_insert
		JMP		_obj_flush
loc_881B3:
		JSR		_obj_spr_draw
		JMP		_obj_flush
_obj_plane_takoff_anim_list:
		.BYTE	$00,$08,$09,$0A,$0B,$0F,$0E,$0D,$0C

; =============== S U B	R O U T	I N E =======================================
_obj_plane_landing_test:
		LDA		_obj_cur_spr_idx._plane
		BEQ		locret_88202
		CMP		#$04
		BCS		locret_88202
		LDA		_obj_pos._plane._COL
		CMP		_last_airport_pos._COL
		BNE		locret_88202
		LDA		_obj_pos._plane._ROW
		CMP		_last_airport_pos._ROW
		BNE		locret_88202
		LDA		#$88
		STA		_obj_plane_anim_idx
		LDA		#$20
		STA		_obj_plane_anim_timer
		LDA		#$02
		STA		_obj_cur_spr_idx._plane
locret_88202:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_flying_retarget:
		LDA		_cur_obj_idx
		ASL
		TAY
		JSR		_obj_flying_get_rand_pos		; choose two random map coords
		STA		_obj_dst_pos._COL,Y
		JSR		_obj_flying_get_rand_pos
		STA		_obj_dst_pos._ROW,Y
		JSR		_obj_fetch
		JSR		_obj_flying_dst_test_adjust		; check if we already there
		CMP		#$FF							; repeat if yes
		BEQ		_obj_flying_retarget
		LDY		_cur_obj_idx					; store destination spr idx
		STA		_obj_dst_spr_idx,Y				; corresponds to plane sprite
		ASL										; orientation 7\ |0/1
		ASL										;			  6<-*->2
		ASL										;			  5/4| \3
		ADC		_obj_cur_spr_idx,Y				; calc direction of sprite rorate
		TAX										; from cur spr idx to dst spr
		LDA		_obj_spr_rotate_delta_table,X	; table 64x64 consists of 0, 1 and -1
		STA		_obj_spr_delta,Y				; we will inc/dec our spr by this value
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_flying_get_rand_pos:
		LDA		_frames_counter
		AND		#$0F
		TAX
		LDA		_rand_seed
		LSR
		LSR
		BNE		loc_88BE9
		LDA		#$24
loc_88BE9:
		STA		_tmp15
loc_88BEB:
		ADC		_frames_counter
		ADC		_frames_div8_counter
		STA		_rand_seed
		DEX
		BNE		loc_88BEB
loc_88BF4:
		CMP		#$48
		BCC		locret_88BFE
		SEC
		SBC		_tmp15
		JMP		loc_88BF4
locret_88BFE:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_flying_dst_test_adjust:
		LDA		_cur_obj_pos._COL				; we have dst pos and
		CMP		_cur_dst_pos._COL				; relative vector to it
		BEQ		.test_vert						; so we decide the direction
		BCS		.fly_left_test_hor
		LDA		_cur_obj_pos._ROW				; fly right, test hor
		CMP		_cur_dst_pos._ROW
		BEQ		.fly_right
		BCC		.fly_right_down
		LDA		#$01							; fly right-up
		RTS
.fly_right:
		LDA		#$02
		RTS
.fly_right_down:
		LDA		#$03
		RTS
.test_vert:
		LDA		_cur_obj_pos._ROW
		CMP		_cur_dst_pos._ROW
		BEQ		.stop
		BCC		.fly_down
		LDA		#$00							; fly up
		RTS
.fly_down:
		LDA		#$04
		RTS
.fly_left_test_hor:
		LDA		_cur_obj_pos._ROW
		CMP		_cur_dst_pos._ROW
		BEQ		.fly_left
		BCS		.fly_left_up
		LDA		#$05							; fly left-down
		RTS
.fly_left:
		LDA		#$06
		RTS
.fly_left_up:
		LDA		#$07
		RTS
.stop:
; -
;		CPY		#$03							; REDUNDANT, something from early
;		BNE		loc_88BD9						; dev stage, seems this sub were used
;		LDA		_obj_cur_spr_idx._train			; also for train retarget, but not now
;		AND		#$02							; Y do not used here as an argument with
;		BEQ		loc_88BCE						; _cur_obj_idx. the only place it get's
;		LDA		_obj_shift._train._COL			; similar value, in _obj_flying_retarget
;		CMP		#$04							; but Y there is _cur_obj_idx*2, so we
;		BEQ		loc_88BD9						; can't have 3 here ever.
;		BEQ		loc_88BD5						; <- luke, i'm a second BEQ in row to a different location lol
;loc_88BCE:
;		LDA		_obj_shift._train._ROW			; this code is just a trash, never get executed
;		CMP		#$04
;		BEQ		loc_88BD9
;loc_88BD5:
;		LDA		_obj_cur_spr_idx._train
;		RTS
;loc_88BD9:
; -
		LDA		#$FF							; all we need here is $FF
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_hndl04_ship:
		LDA		#$02
		STA		_cur_obj_idx
		JSR		_obj_fetch
		LDX		_frames_div8_counter			; every object moves only
		CPX		_cur_obj_idx					; at its own frame, separately
		BNE		loc_8826A						; otherwize it's only drawn
		LDA		_obj_spr_delta._ship
		BEQ		loc_88260
		JMP		_obj_rotate
loc_88260:
		JSR		_obj_ship_dst_test				; test if we in a dst point
		CMP		#$FF
		BNE		loc_88269						; FIX: the same as other handlers
		JSR		_obj_ship_retarget
		JMP		_obj_draw
loc_88269:
		JSR		_cur_obj_move
loc_8826A:
		JMP		_obj_draw

; =============== S U B	R O U T	I N E =======================================
_obj_ship_dst_test:
; -
; WRONG: similar function to train object
; as against the train ship can move diagonally
; but here we test if he moves always diagonally lol
; so if ship moving vertically or horizontally,
; we never arrive to the dest
; -
;		LDA		_obj_shift._ship._COL
;		CMP		#$04
;		BNE		loc_8828E
;		LDA		_obj_shift._ship._ROW
;		CMP		#$04
;		BNE		loc_8828E
; -
; NEW: some possible logic flaw still left. there is no
; test here for virtually possible not moving at all state
; but actually it handled with rotate routine
; -
		LDX		_obj_shift._ship._COL
		BEQ		._ship_no_x_move
		LDA		_obj_shift._ship._COL
		CMP		#$04
		BNE		loc_8828E
._ship_no_x_move:
		LDX		_obj_shift._ship._ROW
		BEQ		._ship_no_y_move
		LDA		_obj_shift._ship._ROW
		CMP		#$04
		BNE		loc_8828E
._ship_no_y_move:
; -
		LDA		_cur_obj_pos._COL
		CMP		_obj_dst_pos._ship._COL
		BNE		loc_8828E
		LDA		_cur_obj_pos._ROW
		CMP		_obj_dst_pos._ship._ROW
		BNE		loc_8828E
		LDA		#$FF
		RTS
loc_8828E:
;		LDA		_obj_cur_spr_idx._ship			; REDUNDANT, not used here, like for train
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_ship_retarget:
		JSR		_get_cur_obj_map_ptr			; _tmp15/_tmp15+1 = map_buf_pos ofs
;		LDA		#$00							; REDUNDANT, unused here
;		STA		_tmp17
		LDA		_obj_cur_spr_idx._ship			; X = cur ship spr idx
		TAX
		CLC
		ADC		#$04
		AND		#$07
		STA		_ptr18							; _ptr18 = opposite direction spr idx
		JSR		_obj_ship_look_for_new_dir
		TXA										; A gets new direction (opposite, if no other cases)
		CMP		_obj_cur_spr_idx._ship			; we still on the same course?
		BEQ		.ship_course_same
		LDY		_cur_obj_idx					; no
		PHA										; save new direction for now
		LDX		_obj_cur_spr_idx,Y
		STX		_ptr18
		STA		_obj_dst_spr_idx,Y				; set new direction index
		ASL
		ASL
		ASL
		ADC		_ptr18
		TAX
		LDA		_obj_spr_rotate_delta_table,X	; select rotate delta
		STA		_obj_spr_delta,Y				; setup rotation
; WRONG: this code does the same work as _obj_rotate,
; when angles less than 2, _obj_rotate will do full
; round, because starting from desired value already...
; so let the right code do the right work instead.
; -
;		LDA		_ptr18
;		CLC
;		ADC		_obj_spr_delta,Y
;		AND		#$07
;		STA		_obj_cur_spr_idx._ship
; -
		PLA										; restore new direction
.ship_course_same:
		ASL										; set new target 1 tile ahead
		TAX										; in a desired direction
		LDA		_cur_obj_idx
		ASL
		TAY
		LDA		_obj_dst_pos._COL,Y
		CLC
		ADC		_obj_dir_delta_list,X
		STA		_obj_dst_pos._COL,Y
		LDA		_obj_dst_pos._ROW,Y
		CLC
		ADC		_obj_dir_delta_list+1,X
		STA		_obj_dst_pos._ROW,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_ship_look_for_new_dir:
; -
; NEW: let's make ship more interesting
; -
		LDA		_rand_seed
		CMP		#$F0							; probability of course change	1/16
		BCC		.loop
		AND		#$02
		BNE		.rleft
		INX										; rotate right
		BNE		.rrot
.rleft:
		DEX										; rotate left
.rrot:
		TXA
		AND		#$07
		TAX
; -
.loop:
		LDA		_obj_map_tile_dir_buf_ofs_list,X; map tile ofs for spr directions
		TAY
		BPL		loc_882FB
		DEC		_tmp15+1						; adjust memory page for negative offsets
		LDA		(_tmp15),Y
		INC		_tmp15+1
		BNE		loc_882FD
loc_882FB:
		LDA		(_tmp15),Y						; get the corresponding map tile
loc_882FD:
;		CMP		#$FE							; UNUSED: test if it is a special sea tile
		CMP		#$BD							; FIX: let's use regular sea tile
		BNE		loc_8830B
		JSR		_obj_ship_max_range_test
		BCC		loc_8830B
		CPX		_ptr18							; forbid opposite pos change
		BEQ		loc_8830B
		RTS
loc_8830B:
		INX										; look for all tiles around
		TXA
		AND		#$07
		TAX
		CMP		_obj_cur_spr_idx._ship			; make full circle clockwise
		BNE		.loop
		LDX		_ptr18							; default value - opposite one
		RTS
_obj_map_tile_dir_buf_ofs_list:
		.BYTE	$B4,$B5,$01,$4D,$4C,$4B,$FF,$B3

; =============== S U B	R O U T	I N E =======================================
_obj_ship_max_range_test:
		LDA		_cur_obj_pos._COL				; here we should decide if we can
		BEQ		.ship_leftmost_pos				; go in the selected direction
		CMP		#$4B
		BEQ		.ship_rightmost_pos
		LDA		_cur_obj_pos._ROW
		BEQ		.ship_top_pos
		CMP		#$4B
		BEQ		.ship_bottom_pos
.ship_can_go:
		SEC
		RTS
.ship_cant_go:
		INX										; this code somehow (not ideal) does
		TXA										; some kind of bouncing of the map borders
		AND		#$07							; or else the ship will float along sides of the map forever
		TAX
		CLC
		RTS
.ship_leftmost_pos:
		CPX		#$05							; leftmost position, but opposite direction
		BCC		.ship_can_go					; can go
		BCS		.ship_cant_go					; otherwise cant
.ship_rightmost_pos:
		CPX		#$01							; etc. all corner cases
		BCC		.ship_can_go
		CPX		#$04
		BCC		.ship_cant_go
		BCS		.ship_can_go
.ship_top_pos:
		CPX		#$02
		BCC		.ship_cant_go
		CPX		#$07
		BEQ		.ship_cant_go
		BNE		.ship_can_go
.ship_bottom_pos:
		CPX		#$03
		BCC		.ship_can_go
		CPX		#$06
		BCC		.ship_cant_go
		BCS		.ship_can_go

; REDUNDANT
;_obj_dir_delta_list:
;		.BYTE	$00,$FF
;		.BYTE	$01,$FF
;		.BYTE	$01,$00
;		.BYTE	$01,$01
;		.BYTE	$00,$01
;		.BYTE	$FF,$01
;		.BYTE	$FF,$00
;		.BYTE	$FF,$FF

; =============== S U B	R O U T	I N E =======================================
_obj_hndl08_train:
		LDA		#$03
		STA		_cur_obj_idx
		JSR		_obj_fetch
		LDX		_frames_div8_counter
		CPX		_cur_obj_idx
		BNE		loc_88347
		LDA		_obj_spr_delta._train
		BEQ		loc_88337
		JMP		_obj_rotate
loc_88337:
		JSR		_obj_train_dst_test
		CMP		#$FF
		BNE		loc_88344
		JSR		_obj_train_retarget
		JMP		_obj_draw
loc_88344:
		JSR		_cur_obj_move
loc_88347:
		JMP		_obj_draw

; =============== S U B	R O U T	I N E =======================================
_obj_train_dst_test:
		LDX		_obj_shift._train._COL
		LDA		_obj_cur_spr_idx._train			; select movement direction
		AND		#$02							; may be only vert or hor
		BNE		loc_88357
		LDX		_obj_shift._train._ROW
loc_88357:
		CPX		#$04
		BNE		locret_8836D					; if timers are max
		LDA		_cur_obj_pos._COL				; test if we in dest point
		CMP		_obj_dst_pos._train._COL
		BNE		locret_8836D
		LDA		_cur_obj_pos._ROW
		CMP		_obj_dst_pos._train._ROW
		BNE		locret_8836D
		LDA		#$FF							; if yes, return FF
locret_8836D:
		RTS										; otherwise return nothing

; =============== S U B	R O U T	I N E =======================================
_obj_train_retarget:
		JSR		_get_cur_obj_map_ptr			; cur map tile under obj in _tmp15/_tmp16
;		LDA		#$00							; REDUNDANT, seems unused here
;		STA		_tmp17
		LDA		_obj_cur_spr_idx._train
		TAX
		CLC
		ADC		#$04
		AND		#$07
		STA		_ptr18							; calc opposite direction
		LDX		#$00
		LDY		#$00
		LDA		(_tmp15),Y						; check cur tile
loc_88386:
		CMP		_rail_tiles_allowed,X			; search if tile is allowed to ride
		BEQ		.path_decide
		INX
		CPX		#$11
		BCC		loc_88386						; X has its index now in arrays
		LDA		_train_hide_mode				; not on the rails, hidden object,
		BNE		.building_bounce				; continue to ride inside only
		JMP		.remove_train					; building available for it - rail station
.building_bounce:
		LDA		_obj_cur_spr_idx._train			; any other building except rail station
		AND		#$02							; should bounce the train, else
		BEQ		.search_vertically				; ride straigth forward inside
		BNE		.search_horisontally
.path_decide:
		CPX		#$04
		BCS		loc_883AA
.search_horisontally:
		LDY		#$00							; 00-03, Y=0 (X ofs)
		JMP		.search_straigth_path
loc_883AA:
		CPX		#$08
		BCS		loc_883B3
.search_vertically:
		LDY		#$01							; 04-07, Y=1 (Y ofs)
		JMP		.search_straigth_path
loc_883B3:
		CPX		#$0C
		BCS		loc_883BA
		JMP		.train_dst_rotate_set			; 08-0B
loc_883BA:
		CPX		#$10
		BCS		.search_cross_path				; 10
		BCC		.search_t_cross_path			; 0C-0F

_rail_tiles_allowed:
		.BYTE	$9C,$E2,$BB,$B7					; 00 horizontal rails
		.BYTE	$9D,$E3,$BC,$B8					; 04 vertical rails
		.BYTE	$9E,$9F,$A0,$A1					; 08 bend rails
		.BYTE	$A2,$A3,$A4,$A5					; 0C t-cross rails
		.BYTE	$A6								; 10 cross rails

.search_cross_path:
		INC		_train_turns_counter			; can turn any direction, select next one
		LDA		_obj_cur_spr_idx._train			; current train position
		ASL
		STA		_tmp118
		LDA		_train_turns_counter			; current turn direction
		AND		#$03
		CLC
		ADC		_tmp118
		TAX
		LDA		_train_cross_allow_table,X		; test if we positioned right
		BEQ		loc_883DE
		TAX
		JMP		.train_dst_rotate_set			; if no, rotate first
loc_883DE:
		JMP		.train_dest_pos_set				; if yes, just move

_train_cross_allow_table:
		.BYTE	$00,$08,$00,$09
		.BYTE	$00,$09,$00,$0B
		.BYTE	$00,$0A,$00,$0B
		.BYTE	$00,$08,$00,$0A

.search_t_cross_path:
		INC		_train_turns_counter
		TXA
		SEC
		SBC		#$0C
		ASL
		ASL
		ASL
		CLC
		ADC		_obj_cur_spr_idx._train			; t-cross little bit harder
		TAX										; but still can be calculated
		LDA		_train_turns_counter
		AND		#$01
		BEQ		loc_884BA
		INX
loc_884BA:
		LDA		_train_t_cross_allow_table,X
		BEQ		loc_884C3
		TAX
		JMP		.train_dst_rotate_set			; again, turn or move
loc_884C3:
		JMP		.train_dest_pos_set

_train_t_cross_allow_table:
		.BYTE	$00,$08,$00,$00,$00,$0A,$08,$0A
		.BYTE	$00,$09,$09,$0B,$00,$0B,$00,$00
		.BYTE	$08,$09,$00,$09,$00,$00,$00,$08
		.BYTE	$00,$00,$00,$0B,$0A,$0B,$00,$0A

.search_straigth_path:
		LDA		_cur_obj_pos,Y
		BEQ		.map_side_test
		CMP		#$4B
		BNE		.map_normal_test
		INY
		INY
.map_side_test:
		LDX		_obj_cur_spr_idx._train			; check the direction
		TXA
		CMP		_train_straight_allow_list,Y
		BEQ		loc_8841E						; if out of the map, then
		LDX		_ptr18							; turn opposite direction
		STX		_obj_cur_spr_idx._train
		STX		_obj_dst_spr_idx._train
loc_8841E:
		JSR		_obj_train_near_tile_test		; test next tile
		BCC		.remove_train
		BCS		.train_dest_pos_set
.map_normal_test:
		LDX		_obj_cur_spr_idx._train
		JSR		_obj_train_near_tile_test		; if next tile in the same direction
		BCS		.train_dest_pos_set				; is ok, then ride continue
		LDX		_ptr18
		JSR		_obj_train_near_tile_test		; or test for the opposite direction
		BCS		.set_opposite_direction
.remove_train:
		LDA		_obj_active_flags
		AND		#$F7
		STA		_obj_active_flags
		LDA		#$00
		STA		_train_hide_mode
		RTS
.set_opposite_direction:
		LDA		_ptr18
		STA		_obj_cur_spr_idx._train
		STA		_obj_dst_spr_idx._train
		JMP		.train_dest_pos_set

_train_straight_allow_list:
		.BYTE	$02,$04,$06,$00

.train_dst_rotate_set:
		TXA
		SEC
		SBC		#$08
		ASL
		ASL
		TAX
		LDA		_obj_cur_spr_idx._train
		LSR
		LSR
		BCS		loc_88461
		INX
		INX
loc_88461:
		LDA		_train_dst_data_table,X
		STA		_obj_dst_spr_idx._train
		LDA		_train_dst_data_table+1,X
		STA		_obj_spr_delta._train
		LDX		_obj_shift._train._COL
		LDA		_obj_shift._train._ROW
		STA		_obj_shift._train._COL
		STX		_obj_shift._train._ROW
.train_dest_pos_set:
		LDA		_obj_dst_spr_idx._train
		ASL
		TAX
		LDA		_obj_dst_pos._train._COL
		CLC
		ADC		_obj_dir_delta_list,X
		STA		_obj_dst_pos._train._COL
		LDA		_obj_dst_pos._train._ROW
		CLC
		ADC		_obj_dir_delta_list+1,X
		STA		_obj_dst_pos._train._ROW
		RTS

_train_dst_data_table:
		.BYTE	$04,$FF							; down, counter clockwise
		.BYTE	$02,$01							; right, clockwise
		.BYTE	$04,$01							; down, counter clockwise
		.BYTE	$06,$FF							; left, counter clockwise
		.BYTE	$00,$01							; up, clockwise
		.BYTE	$02,$FF							; right, counter clockwise
		.BYTE	$00,$FF							; up, counter clockwise
		.BYTE	$06,$01							; left, clockwise

; =============== S U B	R O U T	I N E =======================================
; most of the following code is for determine if we run into the rail station
; or not. this is not trivial task because we won't know for sure wether it
; rail station or not if just bounce into a special building tile (80-8E) so we
; need here carefully calc the side you run into the building, then calc
; position of building index tile, read it and check again
;
_obj_train_near_tile_test:
		LDY		_obj_map_tile_dir_buf_ofs_list,X; tiles around special building are special
		BPL		loc_884F3						; arranged. if you know tile index (0-E),
		DEC		_tmp15+1						; you know for sure where the actual building
		LDA		(_tmp15),Y						; tile is.
		INC		_tmp15+1
		BNE		loc_884F5
loc_884F3:
		LDA		(_tmp15),Y
loc_884F5:
		CMP		#$7D							; lucky! run straigth into the rail station!
		BNE		loc_884FC
		JMP		.rail_station_ride
loc_884FC:
		CMP		#$80							; run into top-left corner of any building
		BCC		.stop_move						; we know it is not the rail station for sure
		CMP		#$88							; this time it's harder, we know this is
		BCC		.rail_station_ride_test			; a building, but need to check which one
		CMP		#$9C							; run into buldozered ground and roads,
		BCC		.stop_move						; stop
		CMP		#$A7							; all kind of rails here,
		BCC		.continue_move					; move along
		CMP		#$B7							; the rest of not rideable grounds here
		BCC		.stop_move						; stop
		CMP		#$B9							; special road/rail crosses
		BCC		.continue_move					; move along
		CMP		#$BB							; skip again
		BCC		.stop_move
		CMP		#$BD							; electric/rail crosses
		BCC		.continue_move					; move further
		CMP		#$E2							; test for two special tiles, rails on water
		BCC		.stop_move						; it seems they planned some special
		CMP		#$E4							; behavior for this, but currently no matter what
		BCS		.stop_move						; value in _train_hide_mode, it always just hiding
		LDA		#$03							; the train if non-zero, so here train will be
		BNE		.continue_move_set				; hidden the same way as in train station while ride
.continue_move:
		LDA		#$00
.continue_move_set:
		STA		_train_hide_mode				; if non-zero, hide the train and continue ride
.continue_move_set_ex:
		SEC										; may be either 0, 3 here and 1 or 2 when
		RTS										; inside train station (see below)
.stop_move:
		CLC
		RTS
.rail_station_ride_test:
		LDA		_train_hide_mode				; already hidden, just continue
		BNE		.continue_move_set_ex
		PUSHB	_ptr18
		TXA
		PHA
		CPY		#$80
		BCC		loc_8855B
		DEC		_tmp15+1
		LDA		(_tmp15),Y
		AND		#$0F
		TAX
		LDA		_tmp15
		SEC
		SBC		_special_tiles_ofs_list,X
		STA		_ptr18
		LDA		_tmp15+1
		SBC		#$00
		STA		_ptr18+1
		INC		_tmp15+1
;		LDA		(_ptr18),Y						; OPTIMIZE
		JMP		loc_88570
loc_8855B:
		LDA		(_tmp15),Y
		AND		#$0F
		TAX
		LDA		_tmp15
		SEC
		SBC		_special_tiles_ofs_list,X
		STA		_ptr18
		LDA		_tmp15+1
		SBC		#$00
		STA		_ptr18+1
loc_88570:
		LDA		(_ptr18),Y
		STA		_ptr18+1
		PLA
		TAX
		POPB	_ptr18
		LDA		_ptr18+1
		CMP		#$7D
;		BEQ		.rail_station_ride
;		CLC
;		RTS
		BNE		.stop_move						; OPTIMIZE size
.rail_station_ride:
		LDA		_obj_cur_spr_idx._train			; attempt to keep track of move direction
		AND		#$02							; inside the building, but never used this way
		LSR										; all this just REDUNDANT
		ADC		#$01
;		STA		_train_hide_mode
;loc_8858A:
;		SEC
;		RTS
		BNE		.continue_move_set				; OPTIMIZE size

; REDUNDANT
;_special_tiles_ofs_list:
;		.BYTE	$4C,$4D,$01,$98,$99,$9A,$4E,$02

; =============== S U B	R O U T	I N E =======================================
_obj_hndl10_monster:
;		LDX		#$00							; WARNING: DUMB DELAY HERE!
;loc_88596:										; stealing cycles
;		DEX										;
;		BNE		loc_88596						; may I remove it, please?
		LDA		#$04
		STA		_cur_obj_idx					; mostly the same common handler
		BIT		_obj_monster_anim_phase1		; with some difference
		BMI		.monster_regular_mode
		JMP		_monster_retarget				; OPTIMIZED
;		JMP		locret_885D8
.monster_regular_mode:
		JSR		_obj_fetch						; move only at it's own frame
		LDA		_frames_div8_counter
		CMP		_cur_obj_idx
		BNE		.monster_draw
		DEC		_obj_monster_delay
		BNE		.monster_skip_anim
;		BEQ		.monster_anim1
;		CMP		#$07							; REDUNDANT, branch takes in any case
;		BCC		.monster_skip_anim				; this counter counts backwards
;		LDA		#$02							; also have no values above 6
;		STA		_obj_monster_delay
;		BNE		.monster_skip_anim
;.monster_anim1:
		JSR		_monster_anim
.monster_skip_anim:
		LDA		_obj_monster_anim_phase0
		CMP		#$20
		BEQ		.monster_draw
		JSR		_monster_move
		BCS		locret_885D8
		JSR		_obj_fetch
.monster_draw:
		JMP		_obj_draw
locret_885D8:
		RTS

; =============== S U B	R O U T	I N E =======================================
_monster_chr_select0:
		LDA		_obj_monster_anim_phase1
		AND		#$7F
		BEQ		loc_885E9
		LDA		_obj_monster_chr_idx
		LSR
		LSR
		LSR
		LSR
		BNE		loc_885EE
loc_885E9:
		LDA		_obj_monster_chr_idx
		AND		#$0F
loc_885EE:
		TAX
		LDA		_monster_in_water_flag
		BNE		loc_885FB
		LDA		_obj_monster_anim_phase0
		CMP		#$20
		BEQ		loc_88602
loc_885FB:
		LDA		_obj_monster_chr_anim_list0,X
		STA		_MMC5_CHR_BANKSA+1
		RTS
loc_88602:
		LDA		_obj_monster_chr_anim_list1,X
		STA		_MMC5_CHR_BANKSA+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_monster_chr_select1:
		LDX		#$00
		LDA		_obj_monster_chr_idx
		AND		#$01
		BEQ		loc_88613
		INX
loc_88613:
		LDA		_obj_monster_chr_idx
		AND		#$10
		BEQ		loc_8861C
		INX
		INX
loc_8861C:
		LDA		_obj_monster_anim_phase1
		AND		#$7F
		BNE		loc_88628
		TXA
		CLC
		ADC		#$04
		TAX
loc_88628:
		LDA		_obj_monster_chr_anim_list2,X
		STA		_MMC5_CHR_BANKSA+1
		TXA
		RTS

; =============== S U B	R O U T	I N E =======================================
_monster_move:
		LDA		_obj_monster_anim_phase1		; select vert/hor direction
		AND		#$01
		TAY
		LDA		_obj_monster_chr_idx			; get direction delta
		AND		byte_8873F,Y					; from sprite idx
		BNE		.monster_pos_dec
		LDA		_obj_shift._monster,Y			; precise positions increment
		CLC
		ADC		#$01
		AND		#$07
		STA		_obj_shift._monster,Y
		BNE		loc_8872E
		LDA		_obj_pos._monster,Y				; tile position increment
		CLC
		ADC		#$01
		STA		_obj_pos._monster,Y
		JMP		loc_88705
.monster_pos_dec:
		LDA		_obj_shift._monster,Y			; the same for decrement
		SEC
		SBC		#$01
		AND		#$07
		STA		_obj_shift._monster,Y
		CMP		#$07
		BNE		loc_8872E
		LDA		_obj_pos._monster,Y
		SEC
		SBC		#$01
		STA		_obj_pos._monster,Y
loc_88705:
		LDA		_obj_pos._monster,Y
		CMP		_obj_dst_pos._monster,Y
		BNE		loc_8872E						; dst point reached for current axis?
		TYA
		EOR		#$01
		TAY
		LDA		_obj_pos._monster,Y
		CMP		_obj_dst_pos._monster,Y
		BEQ		.monster_remove					; test another axis then
		LDA		#$00
		STA		_obj_monster_delay				; reload counters
		LDA		#$1F
		STA		_obj_monster_anim_phase0
		LDA		_obj_monster_anim_phase1		; change direction
		EOR		#$01
		STA		_obj_monster_anim_phase1
		JSR		_monster_anim					; turn
loc_8872E:
		CLC
		RTS
.monster_remove:
		LDA		_obj_active_flags
		AND		#$EF
		STA		_obj_active_flags
		LDA		#$00
		STA		_obj_monster_anim_phase1
		SEC
		RTS

byte_8873F:
		.BYTE	$01,$10

; =============== S U B	R O U T	I N E =======================================
_monster_retarget:
		LDA		#$20							; default direction
		STA		_obj_monster_chr_idx
		LDA		#$01
		STA		_obj_monster_delay
		LDX		#$00
		LDA		_frames_counter
		LSR
		BCC		loc_88754
		LDX		#$01
loc_88754:
		TXA
		ORA		#$80
		STA		_obj_monster_anim_phase1		; retargeted flag
		LDA		_obj_dst_pos._monster._COL		; choose direction delta
		CMP		_obj_pos._monster._COL
		BCS		loc_8876A
		LDA		_obj_monster_chr_idx			; adjust direction to the dst
		ORA		#$01							; the same time change CHR mode
		STA		_obj_monster_chr_idx			; and bank
loc_8876A:
		LDA		_obj_dst_pos._monster._ROW		; the same for Y
		CMP		_obj_pos._monster._ROW
		BCS		locret_8877A
		LDA		_obj_monster_chr_idx
		ORA		#$30
		STA		_obj_monster_chr_idx
locret_8877A:
		RTS

;_unused_0:
;		.BYTE	$02,$04,$02,$00,$06,$04,$06,$00

; =============== S U B	R O U T	I N E =======================================
_monster_anim:
		INC		_obj_monster_anim_phase0
loc_88633:
		JSR		_monster_chr_select0
		LDA		_obj_monster_anim_phase1
		AND		#$7F
		BEQ		loc_88645
		LDA		_obj_monster_chr_idx
		LSR
		LSR
		LSR
		BNE		loc_8864B
loc_88645:
		LDA		_obj_monster_chr_idx
		AND		#$0F
		ASL
loc_8864B:
;		STA		_tmp17							; REDUNDANT
		LDY		_monster_in_water_flag
		BEQ		loc_88657
		CLC
		ADC		#$08
;		STA		_tmp17							; REDUNDANT
loc_88657:
		STA		_tmp17							; OPTIMIZED
		TAX
		LDA		_obj_monster_anim_lib0,X
		STA		_tmp15
		LDA		_obj_monster_anim_lib0+1,X
		STA		_tmp15+1
		LDY		_obj_monster_anim_phase0
loc_88665:
		LDA		(_tmp15),Y
		BNE		loc_886A2
		LDY		#$00
		STY		_obj_monster_anim_phase0
		LDA		_obj_monster_anim_phase1
		AND		#$7F
		EOR		#$01
		TAY
		LDA		_obj_pos._monster,Y
		CMP		_obj_dst_pos._monster,Y
		BEQ		loc_88665
		LDA		_obj_monster_anim_phase1
		AND		#$7F
		STA		_tmp17
		LDA		_obj_monster_anim_phase1
		EOR		#$01
		STA		_obj_monster_anim_phase1
		LDA		_monster_in_water_flag
		BNE		loc_88633
		JSR		_monster_chr_select1
		CLC
		ADC		#$14
		STA		_obj_monster_spr_idx
		LDA		#$02
		STA		_obj_monster_delay
		BNE		locret_886C5
loc_886A2:
		STA		_obj_monster_delay
		LDX		_tmp17
		LDA		_obj_monster_anim_lib1,X
		STA		_tmp15
		LDA		_obj_monster_anim_lib1+1,X
		STA		_tmp15+1
		LDY		_obj_monster_anim_phase0
		LDA		(_tmp15),Y
		STA		_obj_monster_spr_idx
		CMP		#$10
		BCC		locret_886C5
		CMP		#$14
		BCS		locret_886C5
		LDA		#$01
		STA		_apu_se_flags+1
locret_886C5:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_hndl20_tornado:
		LDA		#$4C							; restore CHR bank if needed
		STA		_MMC5_CHR_BANKSA+1
		LDA		#$05
		STA		_cur_obj_idx
		JSR		_obj_fetch
		LDX		_frames_div8_counter
		CPX		_cur_obj_idx
		BNE		loc_887B6
		LDA		_obj_tornado_counter
		CLC
		ADC		#$01
		CMP		#$1B
		BCC		loc_887A3
		LDA		#$00
loc_887A3:
		STA		_obj_tornado_counter
		JSR		_obj_tornado_move
		CMP		#$FF
		BEQ		locret_887B9
		LDX		_cur_obj_pos._COL
		LDY		_cur_obj_pos._ROW
		JSR		_far_fire_or_destroy_map_tile_ex
loc_887B6:
		JMP		_obj_draw
locret_887B9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_tornado_move:
		LDX		#$00
		STX		_tmp15
		LDA		_cur_obj_pos._COL
		CMP		_cur_dst_pos._COL
		BEQ		loc_887D8
		BCC		loc_887CF
		LDX		#$FF
		SBC		_cur_dst_pos._COL
		BCS		loc_887D8
loc_887CF:
		LDX		#$01
		LDA		_cur_dst_pos._COL
		SEC
		SBC		_cur_obj_pos._COL
loc_887D8:
		STA		_tmp15
		LDY		#$00
		LDA		_cur_obj_pos._ROW
		CMP		_cur_dst_pos._ROW
		BEQ		loc_88834
		BCC		loc_887F1
		SBC		_cur_dst_pos._ROW
		CMP		_tmp15
		BCC		loc_88806
		LDY		#$FF
		BMI		loc_887FE
loc_887F1:
		LDA		_cur_dst_pos._ROW
		SEC
		SBC		_cur_obj_pos._ROW
		CMP		_tmp15
		BCC		loc_88806
		LDY		#$01
loc_887FE:
		STY		_tmp15
		LDX		#$0B
		LDY		#$01
		BNE		loc_8880C
loc_88806:
		STX		_tmp15
		LDX		#$0A
		LDY		#$00
loc_8880C:
		LDA		_obj_shift._tornado,Y
		CLC
		ADC		_tmp15
		AND		#$07
		STA		_obj_shift._tornado,Y
		BNE		loc_8881F
		LDA		_tmp15
		BMI		locret_88833
		BPL		loc_88827
loc_8881F:
		CMP		#$07
		BNE		locret_88833
		LDA		_tmp15
		BPL		locret_88833
loc_88827:
		LDA		_obj_pos,X
		CLC
		ADC		_tmp15
		STA		_obj_pos,X
		STA		_cur_obj_pos,Y
locret_88833:
		RTS
loc_88834:
		LDA		_obj_active_flags
		AND		#$DF
		STA		_obj_active_flags
		LDA		#$FF
		RTS

; =============== S U B	R O U T	I N E =======================================
; common draw routine for all objects with it's own sprite idx calculations
; all this can be actually put inside individual handlers and and the whole
; routine is redundant then.
;
_obj_draw:
		LDX		_game_wnd_spr_mode_flag
		BEQ		loc_88110
		JSR		_map_scr_obj_spr_insert
		JMP		_obj_flush
loc_88110:
		LDY		_cur_obj_idx
		BEQ		.obj_heli_draw
		CPY		#$01
		BEQ		.obj_common_draw				; plane default draw
		CPY		#$02
		BEQ		.obj_ship_draw
		CPY		#$03
		BEQ		.obj_train_draw
		CPY		#$04
		BEQ		.obj_monster_draw
		CPY		#$05
		BEQ		.obj_tornado_draw
.obj_common_draw:
		LDA		_obj_cur_spr_idx,Y				; default draw
.obj_common_draw_ex:
		STA		_spr_insert_args._idx
		JSR		_obj_spr_draw
		JMP		_obj_flush
.obj_heli_draw:
		LDA		_obj_cur_spr_idx._heli
		LDX		_frames_div4_counter
;		JSR		_obj_spr_heli_draw
		ASL										; fetch every 4th sprite idx
		ASL
		STA		_spr_insert_args._idx			; set +1 idx every 1/8 frames
		TXA
		LSR
;		AND		#$01							; BUG? two frames are unused with that
		AND		#$03							; FIX: use all 4
		CLC
		ADC		_spr_insert_args._idx
		JMP		.obj_common_draw_ex
.obj_ship_draw:
		CMPWI	_city._cur_year,1950			; for years 1900-1949 used _spr_shipB set
		BCS		.obj_common_draw				; the rest sprites uses _spr_shipA set
		LDA		_obj_cur_spr_idx,Y
		CLC
		ADC		#$08
		BNE		.obj_common_draw_ex				; always non-zero
.obj_train_draw:
		LDA		_train_hide_mode
		BEQ		.obj_common_draw				; hide if inside train station
		RTS
.obj_monster_draw:
		LDA		_obj_monster_spr_idx
		STA		_spr_insert_args._idx
		JSR		_obj_spr_draw
		LDX		_cur_obj_pos._COL
		LDY		_cur_obj_pos._ROW
		JMP		_far_fire_or_destroy_map_tile_ex
.obj_tornado_draw:
		LDX		_obj_tornado_counter
		LDA		_tornado_spr_anim_list,X
		JMP		.obj_common_draw_ex

; =============== S U B	R O U T	I N E =======================================
;_obj_spr_heli_draw:
;		ASL										; fetch every 4th sprite idx
;		ASL
;		STA		_spr_insert_args._idx			; set +1 idx every 1/8 frames
;		TXA
;		LSR
;;		AND		#$01							; BUG? two frames are unused with that
;		AND		#$03							; FIX: use all 4
;		CLC
;		ADC		_spr_insert_args._idx
;		STA		_spr_insert_args._idx

; =============== S U B	R O U T	I N E =======================================
_obj_spr_draw:
		LDY		#$00							; OPTIMIZED size
		JSR		_out_of_screen_test
;		LDA		_cur_obj_pos._COL					; out-of-screen test for X
;		SEC
;		SBC		_cur_map_wnd_pos._COL
;		BCS		loc_889F7
;		CMP		#$FD
;		BCS		loc_889FB
;		BCC		locret_88A45
;loc_889F7:
;		CMP		#$1B
;		BCS		locret_88A45
;loc_889FB:
		LDY		#$01
		JSR		_out_of_screen_test
;		LDA		_cur_obj_pos._ROW					; the same for Y
;		SEC
;		SBC		_cur_map_wnd_pos._ROW
;		BCS		loc_88A09
;		CMP		#$FD
;		BCS		loc_88A0D
;		BCC		locret_88A45
;loc_88A09:
;		CMP		#$18
;		BCS		locret_88A45
loc_88A0D:
		LDA		_cur_obj_idx					; map to screen pos convert
		ASL
		TAX
		LDY		#$00
		JSR		_map_to_screen_conv				; OPTIMIZED size
;		LDA		_cur_obj_pos._COL
;		SEC
;		SBC		_cur_map_wnd_pos._COL
;		CLC
;		ADC		#$07
;		ASL
;		ASL
;		ASL
;		CLC
;		ADC		_obj_shift._COL,X
;		STA		_spr_insert_args._pos._COL
		LDY		#$01
		INX
		JSR		_map_to_screen_conv
;		LDA		_cur_obj_pos._ROW
;		SEC
;		SBC		_cur_map_wnd_pos._ROW
;		CLC
;		ADC		#$07
;		ASL
;		ASL
;		ASL
;		CLC
;		ADC		_obj_shift._ROW,X
;		STA		_spr_insert_args._pos._ROW
; -
;		LDA		_cur_obj_idx					; REDUNDANT, check for special train hide
;		CMP		#$03							; mode, can be done only for train
;		BNE		loc_88A42
;		LDA		_train_hide_mode
;		BNE		locret_88A45
;loc_88A42:
;		JMP		_obj_spr_clip_draw				; REDUNDANT
;locret_88A45:
;		RTS

; NOTE: clip value never used in sprite drawing routines,
; several sprite drawing routines has its own hardwritten
; clipping code and no one uses the common one.
; the _spr_insert_args._clip only zeroed at some point
; never change it's value
;
; for some unknown reason, they used _tornado_spr_anim_list
; as a list of sprite sizes lol
;
; =============== S U B	R O U T	I N E =======================================
;_obj_clip_select_unused:
;		TAX
;		LDA		#$00
;		STA		_spr_insert_args._clip
;		LDA		_spr_insert_args._pos._COL
;		CLC
;		ADC		_tornado_spr_anim_list,X
;		CMP		#$38
;		BCS		loc_88A5B
;		LDA		_spr_insert_args._clip
;		ORA		#$01
;		STA		_spr_insert_args._clip
;loc_88A5B:
;		LDA		_spr_insert_args._pos._ROW
;		CLC
;		ADC		_tornado_spr_anim_list,X
;		CMP		#$38
;		BCS		loc_88A6D
;		LDA		_spr_insert_args._clip
;		ORA		#$02
;		STA		_spr_insert_args._clip
;		BNE		locret_88A7D
;loc_88A6D:
;		LDA		_spr_insert_args._pos._ROW
;		CLC
;		ADC		_tornado_spr_anim_list,X
;		CMP		#$D9
;		BCC		locret_88A7D
;		LDA		_spr_insert_args._clip
;		ORA		#$04
;		STA		_spr_insert_args._clip
;locret_88A7D:
;		RTS

; !FALLTHROUGH!

; NOTE: clipping embedded in this routine, no need in additional
; clipping function or variable.
;
; =============== S U B	R O U T	I N E =======================================
;_obj_spr_clip_draw:
;		LDA		#$00
;		STA		_spr_insert_args._clip
		LDA		_cur_obj_idx
		CMP		#$08
		BCC		loc_88A8C
loc_88A89:
		JMP		loc_88A89						; debug assert
loc_88A8C:
		ASL
		TAX
		LDA		_obj_spr_tbl_lib,X
		STA		_tmp15
		LDA		_obj_spr_tbl_lib+1,X
		STA		_tmp15+1
		LDA		_spr_insert_args._idx
		ASL
		TAY
		LDA		(_tmp15),Y
		TAX
		INY
		LDA		(_tmp15),Y
		STA		_tmp15+1
		STX		_tmp15
		DEC		_spr_insert_args._pos._ROW
		LDX		_spr_buf_pos
		LDY		#$00
loc_88AAC:
		STX		_tmp17
		LDA		(_tmp15),Y
		CMP		#$80
		BEQ		loc_88B09
		INY
		CLC
		ADC		_spr_insert_args._pos._ROW
		CMP		#$30
		BCS		loc_88AC4
loc_88ABC:
		INY
		INY
		INY
loc_88ABF:
		LDX		_tmp17
		JMP		loc_88AAC
loc_88AC4:
		CMP		#$38
		BCS		loc_88ACF
		PHA
		JSR		_obj_spr_clipB_insert
		PLA
		BNE		loc_88ADC
loc_88ACF:
		CMP		#$DE
		BCS		loc_88ABC
		CMP		#$D6
		BCC		loc_88ADC
		PHA
		JSR		_obj_spr_clipA_insert
		PLA
loc_88ADC:
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		INY
		CLC
		ADC		_spr_insert_args._pos._COL
		CMP		#$31
		BCC		loc_88ABF
		CMP		#$39
		BCS		loc_88B02
		JSR		_obj_spr_clipC_insert
		JMP		loc_88AAC
loc_88B02:
		STA		_spr_buf,X
		INX
		JMP		loc_88AAC
loc_88B09:
		STX		_spr_buf_pos
		RTS

; =============== S U B	R O U T	I N E =======================================
_out_of_screen_test:
		LDA		_cur_obj_pos,Y
		SEC
		SBC		_cur_map_wnd_pos,Y
		BCS		loc_889F7
		CMP		#$FD
		BCC		loc_889F8
		RTS
loc_889F7:
		CMP		#$1B
		BCC		loc_889FB
loc_889F8:
		PLA
		PLA
loc_889FB:
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_to_screen_conv:
		LDA		_cur_obj_pos,Y
		SEC
		SBC		_cur_map_wnd_pos,Y
		CLC
		ADC		#$07
		ASL
		ASL
		ASL
		CLC
		ADC		_obj_shift,X
		STA		_spr_insert_args._pos,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_spr_clipA_insert:
		LDA		#$DF
		BNE		_obj_spr_insert_ex

; =============== S U B	R O U T	I N E =======================================
_obj_spr_clipB_insert:
		LDA		#$2F

; =============== S U B	R O U T	I N E =======================================
_obj_spr_insert_ex:
		PHA
		TYA
		AND		#$FC
		ORA		#$03
		TAY
		PLA
		STA		_spr_buf,X
		INX
		LDA		#$AE
		STA		_spr_buf,X
		INX
		LDA		#$20
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		CLC
		ADC		_spr_insert_args._pos._COL
		STA		_spr_buf,X
		INX
		TYA
		AND		#$FC
		TAY
		INY
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_spr_clipC_insert:
		DEY
		PUSHB	#$2F
		TYA
		AND		#$FC
		TAY
		TXA
		AND		#$FC
		TAX
		INX
		LDA		#$AE
		STA		_spr_buf,X
		INX
		LDA		#$20
		STA		_spr_buf,X
		INX
		PLA
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		CLC
		ADC		_spr_insert_args._pos._ROW
		STA		_spr_buf,X
		INX
		INY
		LDA		(_tmp15),Y
		STA		_spr_buf,X
		INX
		INY
		LDA		(_tmp15),Y
		STA		_spr_buf,X
		INX
		INY
		LDA		(_tmp15),Y
		CLC
		ADC		_spr_insert_args._pos._COL
		STA		_spr_buf,X
		INX
		INY
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_scr_obj_spr_insert:
		LDA		#$36
		STA		_MMC5_CHR_BANKSA+2
		LDA		_cur_obj_idx
		ASL
		TAX
		LDY		_spr_buf_pos
		LDA		_obj_pos._ROW,X
		LDX		_cur_obj_idx
		ASL
		CLC
		ADC		#$3C
		ADC		_map_scr_obj_spr_deltaY,X
		STA		_spr_buf,Y
		INY
		LDX		_cur_obj_idx
		LDA		_map_scr_obj_spr_list,X
		STA		_spr_buf,Y
		INY
		LDA		_map_scr_obj_spr_attrs,X
		STA		_spr_buf,Y
		INY
		LDA		_cur_obj_idx
		ASL
		TAX
		LDA		_obj_pos,X
		LDX		_cur_obj_idx
		ASL
		CLC
		ADC		#$45
		ADC		_map_scr_obj_spr_deltaX,X
		STA		_spr_buf,Y
		INY
		STY		_spr_buf_pos
		RTS

_map_scr_obj_spr_list:
		.BYTE	$A9,$AA,$AB,$AC,$BD,$AD
_map_scr_obj_spr_attrs:
		.BYTE	$01,$02,$03,$00,$03,$02
_map_scr_obj_spr_deltaY:
		.BYTE	$04,$04,$04,$04,$04,$FC
_map_scr_obj_spr_deltaX:
		.BYTE	$04,$04,$04,$04,$04,$FC

; =============== S U B	R O U T	I N E =======================================
_far_fire_or_destroy_map_tile_ex:
		PUSHB	_ptr0
		PUSHB	_ptr0+1
		PUSHB	_tmp2
		PUSHB	_tmp3
		PUSHB	_tmp4
		PUSHB	_tmp5
		FJSRA	_fire_or_destroy_map_tile_ex,SRAM,PRGB
		POPB	_tmp5
		POPB	_tmp4
		POPB	_tmp3
		POPB	_tmp2
		POPB	_ptr0+1
		POPB	_ptr0
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_obj_move:
		LDY		_cur_obj_idx
		LDA		_obj_cur_spr_idx,Y				; all arrays inside are with 2-byte items
		ASL										; every item is for X and Y correspondingly
		TAX										; calculations performed with the same routine
		LDA		_cur_obj_idx					; separately for X and Y
		ASL
		TAY
		PHA
		JSR		_cur_obj_move_set				; X - cur spr param, Y - cur obj parame (+0=X, +1=Y)
		INX
		PLA
		TAY
		INY
;		JMP		_cur_obj_move_set

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_cur_obj_move_set:
		LDA		_obj_dir_delta_list,X			; no move needed for this sprite state
		BEQ		locret_888A9
		BPL		.increase						; increase or decrease
		CLC
		ADC		_obj_shift,Y
		AND		#$07
		STA		_obj_shift,Y
		CMP		#$07
		BNE		locret_888A9					; adjust timers
		TXA
		AND		#$01
		TAY
		LDA		_cur_obj_pos,Y					; recalc X/Y pos for local var
		SEC
		SBC		#$01
		STA		_cur_obj_pos,Y
		RTS
.increase:
		CLC
		ADC		_obj_shift,Y
		AND		#$07
		STA		_obj_shift,Y
		BNE		locret_888A9
		TXA
		AND		#$01
		TAY
		LDA		_cur_obj_pos,Y
		CLC
		ADC		#$01
		STA		_cur_obj_pos,Y
locret_888A9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_rotate:
		LDY		_cur_obj_idx
		LDA		_obj_spr_delta,Y				; change cur spr idx
		CLC
		ADC		_obj_cur_spr_idx,Y
		AND		#$07
		STA		_obj_cur_spr_idx,Y				; wrap around 7, there are
		CMP		_obj_dst_spr_idx,Y				; always 8 sprite states here
		BNE		loc_88246
		LDA		#$00
		STA		_obj_spr_delta,Y				; desired sprite selected
loc_88246:
		JMP		_obj_draw

; =============== S U B	R O U T	I N E =======================================
_obj_fetch:
		LDA		_cur_obj_idx
		ASL
		TAX
		LDA		_obj_pos._COL,X
		STA		_cur_obj_pos._COL
		LDA		_obj_pos._ROW,X
		STA		_cur_obj_pos._ROW
		LDA		_obj_dst_pos._COL,X
		STA		_cur_dst_pos._COL
		LDA		_obj_dst_pos._ROW,X
		STA		_cur_dst_pos._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_flush:
		LDA		_cur_obj_idx
		ASL
		TAX
		LDA		_cur_obj_pos._COL
		STA		_obj_pos._COL,X
		LDA		_cur_obj_pos._ROW
		STA		_obj_pos._ROW,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_cur_obj_map_ptr:
;		LDY		_cur_obj_pos._ROW
;		LDA		off_888F5,Y
;		CLC
;		ADC		_cur_obj_pos._COL
;		STA		_tmp15
;		LDA		off_88941,Y
;		ADC		#$00
;		STA		_tmp15+1
; FIX: WRANING, make sure wram_map_buf is at the beginning of the page, instead
; use full 16-bit addition
;
		BUFPOS	_tmp15,_cur_obj_pos._COL,_cur_obj_pos._ROW,#$4C,#>[_wram_map_buf]
		RTS

; FIX: common dir delta list
;
_obj_dir_delta_list:
		.BYTE	$00,$FF
		.BYTE	$01,$FF
		.BYTE	$01,$00
		.BYTE	$01,$01
		.BYTE	$00,$01
		.BYTE	$FF,$01
		.BYTE	$FF,$00
		.BYTE	$FF,$FF

; REDUNDANT: calcs used
;off_888F5:
;		.BYTE	<[_wram_map_buf]
;		.BYTE	<[_wram_map_buf+$4C]
;		.BYTE	<[_wram_map_buf+$98]
;		.BYTE	<[_wram_map_buf+$E4]
;		.BYTE	<[_wram_map_buf+$130]
;		.BYTE	<[_wram_map_buf+$17C]
;		.BYTE	<[_wram_map_buf+$1C8]
;		.BYTE	<[_wram_map_buf+$214]
;		.BYTE	<[_wram_map_buf+$260]
;		.BYTE	<[_wram_map_buf+$2AC]
;		.BYTE	<[_wram_map_buf+$2F8]
;		.BYTE	<[_wram_map_buf+$344]
;		.BYTE	<[_wram_map_buf+$390]
;		.BYTE	<[_wram_map_buf+$3DC]
;		.BYTE	<[_wram_map_buf+$428]
;		.BYTE	<[_wram_map_buf+$474]
;		.BYTE	<[_wram_map_buf+$4C0]
;		.BYTE	<[_wram_map_buf+$50C]
;		.BYTE	<[_wram_map_buf+$558]
;		.BYTE	<[_wram_map_buf+$5A4]
;		.BYTE	<[_wram_map_buf+$5F0]
;		.BYTE	<[_wram_map_buf+$63C]
;		.BYTE	<[_wram_map_buf+$688]
;		.BYTE	<[_wram_map_buf+$6D4]
;		.BYTE	<[_wram_map_buf+$720]
;		.BYTE	<[_wram_map_buf+$76C]
;		.BYTE	<[_wram_map_buf+$7B8]
;		.BYTE	<[_wram_map_buf+$804]
;		.BYTE	<[_wram_map_buf+$850]
;		.BYTE	<[_wram_map_buf+$89C]
;		.BYTE	<[_wram_map_buf+$8E8]
;		.BYTE	<[_wram_map_buf+$934]
;		.BYTE	<[_wram_map_buf+$980]
;		.BYTE	<[_wram_map_buf+$9CC]
;		.BYTE	<[_wram_map_buf+$A18]
;		.BYTE	<[_wram_map_buf+$A64]
;		.BYTE	<[_wram_map_buf+$AB0]
;		.BYTE	<[_wram_map_buf+$AFC]
;		.BYTE	<[_wram_map_buf+$B48]
;		.BYTE	<[_wram_map_buf+$B94]
;		.BYTE	<[_wram_map_buf+$BE0]
;		.BYTE	<[_wram_map_buf+$C2C]
;		.BYTE	<[_wram_map_buf+$C78]
;		.BYTE	<[_wram_map_buf+$CC4]
;		.BYTE	<[_wram_map_buf+$D10]
;		.BYTE	<[_wram_map_buf+$D5C]
;		.BYTE	<[_wram_map_buf+$DA8]
;		.BYTE	<[_wram_map_buf+$DF4]
;		.BYTE	<[_wram_map_buf+$E40]
;		.BYTE	<[_wram_map_buf+$E8C]
;		.BYTE	<[_wram_map_buf+$ED8]
;		.BYTE	<[_wram_map_buf+$F24]
;		.BYTE	<[_wram_map_buf+$F70]
;		.BYTE	<[_wram_map_buf+$FBC]
;		.BYTE	<[_wram_map_buf+$1008]
;		.BYTE	<[_wram_map_buf+$1054]
;		.BYTE	<[_wram_map_buf+$10A0]
;		.BYTE	<[_wram_map_buf+$10EC]
;		.BYTE	<[_wram_map_buf+$1138]
;		.BYTE	<[_wram_map_buf+$1184]
;		.BYTE	<[_wram_map_buf+$11D0]
;		.BYTE	<[_wram_map_buf+$121C]
;		.BYTE	<[_wram_map_buf+$1268]
;		.BYTE	<[_wram_map_buf+$12B4]
;		.BYTE	<[_wram_map_buf+$1300]
;		.BYTE	<[_wram_map_buf+$134C]
;		.BYTE	<[_wram_map_buf+$1398]
;		.BYTE	<[_wram_map_buf+$13E4]
;		.BYTE	<[_wram_map_buf+$1430]
;		.BYTE	<[_wram_map_buf+$147C]
;		.BYTE	<[_wram_map_buf+$14C8]
;		.BYTE	<[_wram_map_buf+$1514]
;		.BYTE	<[_wram_map_buf+$1560]
;		.BYTE	<[_wram_map_buf+$15AC]
;		.BYTE	<[_wram_map_buf+$15F8]
;		.BYTE	<[_wram_map_buf+$1644]
;off_88941:
;		.BYTE	>[_wram_map_buf]
;		.BYTE	>[_wram_map_buf+$4C]
;		.BYTE	>[_wram_map_buf+$98]
;		.BYTE	>[_wram_map_buf+$E4]
;		.BYTE	>[_wram_map_buf+$130]
;		.BYTE	>[_wram_map_buf+$17C]
;		.BYTE	>[_wram_map_buf+$1C8]
;		.BYTE	>[_wram_map_buf+$214]
;		.BYTE	>[_wram_map_buf+$260]
;		.BYTE	>[_wram_map_buf+$2AC]
;		.BYTE	>[_wram_map_buf+$2F8]
;		.BYTE	>[_wram_map_buf+$344]
;		.BYTE	>[_wram_map_buf+$390]
;		.BYTE	>[_wram_map_buf+$3DC]
;		.BYTE	>[_wram_map_buf+$428]
;		.BYTE	>[_wram_map_buf+$474]
;		.BYTE	>[_wram_map_buf+$4C0]
;		.BYTE	>[_wram_map_buf+$50C]
;		.BYTE	>[_wram_map_buf+$558]
;		.BYTE	>[_wram_map_buf+$5A4]
;		.BYTE	>[_wram_map_buf+$5F0]
;		.BYTE	>[_wram_map_buf+$63C]
;		.BYTE	>[_wram_map_buf+$688]
;		.BYTE	>[_wram_map_buf+$6D4]
;		.BYTE	>[_wram_map_buf+$720]
;		.BYTE	>[_wram_map_buf+$76C]
;		.BYTE	>[_wram_map_buf+$7B8]
;		.BYTE	>[_wram_map_buf+$804]
;		.BYTE	>[_wram_map_buf+$850]
;		.BYTE	>[_wram_map_buf+$89C]
;		.BYTE	>[_wram_map_buf+$8E8]
;		.BYTE	>[_wram_map_buf+$934]
;		.BYTE	>[_wram_map_buf+$980]
;		.BYTE	>[_wram_map_buf+$9CC]
;		.BYTE	>[_wram_map_buf+$A18]
;		.BYTE	>[_wram_map_buf+$A64]
;		.BYTE	>[_wram_map_buf+$AB0]
;		.BYTE	>[_wram_map_buf+$AFC]
;		.BYTE	>[_wram_map_buf+$B48]
;		.BYTE	>[_wram_map_buf+$B94]
;		.BYTE	>[_wram_map_buf+$BE0]
;		.BYTE	>[_wram_map_buf+$C2C]
;		.BYTE	>[_wram_map_buf+$C78]
;		.BYTE	>[_wram_map_buf+$CC4]
;		.BYTE	>[_wram_map_buf+$D10]
;		.BYTE	>[_wram_map_buf+$D5C]
;		.BYTE	>[_wram_map_buf+$DA8]
;		.BYTE	>[_wram_map_buf+$DF4]
;		.BYTE	>[_wram_map_buf+$E40]
;		.BYTE	>[_wram_map_buf+$E8C]
;		.BYTE	>[_wram_map_buf+$ED8]
;		.BYTE	>[_wram_map_buf+$F24]
;		.BYTE	>[_wram_map_buf+$F70]
;		.BYTE	>[_wram_map_buf+$FBC]
;		.BYTE	>[_wram_map_buf+$1008]
;		.BYTE	>[_wram_map_buf+$1054]
;		.BYTE	>[_wram_map_buf+$10A0]
;		.BYTE	>[_wram_map_buf+$10EC]
;		.BYTE	>[_wram_map_buf+$1138]
;		.BYTE	>[_wram_map_buf+$1184]
;		.BYTE	>[_wram_map_buf+$11D0]
;		.BYTE	>[_wram_map_buf+$121C]
;		.BYTE	>[_wram_map_buf+$1268]
;		.BYTE	>[_wram_map_buf+$12B4]
;		.BYTE	>[_wram_map_buf+$1300]
;		.BYTE	>[_wram_map_buf+$134C]
;		.BYTE	>[_wram_map_buf+$1398]
;		.BYTE	>[_wram_map_buf+$13E4]
;		.BYTE	>[_wram_map_buf+$1430]
;		.BYTE	>[_wram_map_buf+$147C]
;		.BYTE	>[_wram_map_buf+$14C8]
;		.BYTE	>[_wram_map_buf+$1514]
;		.BYTE	>[_wram_map_buf+$1560]
;		.BYTE	>[_wram_map_buf+$15AC]
;		.BYTE	>[_wram_map_buf+$15F8]
;		.BYTE	>[_wram_map_buf+$1644]

_obj_spr_tbl_lib:
		.WORD	_obj0_spr_lib
		.WORD	_obj1_spr_lib
		.WORD	_obj2_spr_lib
		.WORD	_obj3_spr_lib
		.WORD	_obj4_spr_lib
		.WORD	_obj5_spr_lib
_obj0_spr_lib:
		.WORD	_spr_heliA0
		.WORD	_spr_heliB0
		.WORD	_spr_heliC0
		.WORD	_spr_heliD0
		.WORD	_spr_heliA1
		.WORD	_spr_heliB1
		.WORD	_spr_heliC1
		.WORD	_spr_heliD1
		.WORD	_spr_heliA2
		.WORD	_spr_heliB2
		.WORD	_spr_heliC2
		.WORD	_spr_heliD2
		.WORD	_spr_heliA3
		.WORD	_spr_heliB3
		.WORD	_spr_heliC3
		.WORD	_spr_heliD3
		.WORD	_spr_heliA4
		.WORD	_spr_heliB4
		.WORD	_spr_heliC4
		.WORD	_spr_heliD4
		.WORD	_spr_heliA5
		.WORD	_spr_heliB5
		.WORD	_spr_heliC5
		.WORD	_spr_heliD5
		.WORD	_spr_heliA6
		.WORD	_spr_heliB6
		.WORD	_spr_heliC6
		.WORD	_spr_heliD6
		.WORD	_spr_heliA7
		.WORD	_spr_heliB7
		.WORD	_spr_heliC7
		.WORD	_spr_heliD7
_obj3_spr_lib:
		.WORD	_spr_train0
		.WORD	_spr_train1
		.WORD	_spr_train2
		.WORD	_spr_train3
		.WORD	_spr_train0
		.WORD	_spr_train1
		.WORD	_spr_train2
		.WORD	_spr_train3
_obj2_spr_lib:
		.WORD	_spr_shipA0
		.WORD	_spr_shipA1
		.WORD	_spr_shipA2
		.WORD	_spr_shipA3
		.WORD	_spr_shipA4
		.WORD	_spr_shipA5
		.WORD	_spr_shipA6
		.WORD	_spr_shipA7
		.WORD	_spr_shipB0
		.WORD	_spr_shipB1
		.WORD	_spr_shipB2
		.WORD	_spr_shipB3
		.WORD	_spr_shipB4
		.WORD	_spr_shipB5
		.WORD	_spr_shipB6
		.WORD	_spr_shipB7
_obj1_spr_lib:
		.WORD	_spr_planeA0	; flight scprites
		.WORD	_spr_planeA1
		.WORD	_spr_planeA2
		.WORD	_spr_planeA3
		.WORD	_spr_planeA4
		.WORD	_spr_planeA5
		.WORD	_spr_planeA6
		.WORD	_spr_planeA7
		.WORD	_spr_planeA6	; right-to-left landing/take off
		.WORD	_spr_planeB2
		.WORD	_spr_planeB3
		.WORD	_spr_planeB4
		.WORD	_spr_planeA2	; right-t-left- landing/take off
		.WORD	_spr_planeC2
		.WORD	_spr_planeC3
		.WORD	_spr_planeC4
_obj5_spr_lib:
		.WORD	_spr_tornado0
		.WORD	_spr_tornado1
		.WORD	_spr_tornado2
		.WORD	_spr_tornado3
		.WORD	_spr_tornado4
		.WORD	_spr_tornado5
		.WORD	_spr_tornado6
		.WORD	_spr_tornado7
		.WORD	_spr_tornado8
_obj4_spr_lib:
		.WORD	_spr_monster00
		.WORD	_spr_monster01
		.WORD	_spr_monster02
		.WORD	_spr_monster03
		.WORD	_spr_monster04
		.WORD	_spr_monster05
		.WORD	_spr_monster06
		.WORD	_spr_monster07
		.WORD	_spr_monster08
		.WORD	_spr_monster09
		.WORD	_spr_monster0A
		.WORD	_spr_monster0B
		.WORD	_spr_monster0C
		.WORD	_spr_monster0D
		.WORD	_spr_monster0E
		.WORD	_spr_monster0F
		.WORD	_spr_monster10
		.WORD	_spr_monster11
		.WORD	_spr_monster12
		.WORD	_spr_monster13
		.WORD	_spr_monster14					; never happened before
		.WORD	_spr_monster15
		.WORD	_spr_monster16
		.WORD	_spr_monster17
		.WORD	_spr_monster18					; never happened before
		.WORD	_spr_monster19
		.WORD	_spr_monster1A
		.WORD	_spr_monster1B
		.WORD	_spr_monster1C
		.WORD	_spr_monster1D
		.WORD	_spr_monster1C					; unused indexes, spritest the same
		.WORD	_spr_monster1D					; unused indexes, spritest the same
		.WORD	_spr_monster20
		.WORD	_spr_monster21
		.WORD	_spr_monster20					; unused indexes, spritest the same
		.WORD	_spr_monster21					; unused indexes, spritest the same
		.WORD	_spr_monster24
		.WORD	_spr_monster25
		.WORD	_spr_monster24					; unused indexes, spritest the same
		.WORD	_spr_monster25					; unused indexes, spritest the same
		.WORD	_spr_monster28
		.WORD	_spr_monster29
		.WORD	_spr_monster28					; unused indexes, spritest the same
		.WORD	_spr_monster29					; unused indexes, spritest the same
_spr_monster17:
		.BYTE	$FA,$79,$01,$F8
		.BYTE	$F2,$78,$01,$F8
		.BYTE	$01,$7A,$01,$01
		.BYTE	$F9,$6B,$01,$09
		.BYTE	$F9,$6A,$01,$01
		.BYTE	$10,$68,$02,$F8
		.BYTE	$08,$67,$02,$FB
		.BYTE	$00,$66,$02,$00
		.BYTE	$00,$65,$02,$F8
		.BYTE	$F8,$56,$02,$00
		.BYTE	$F8,$55,$02,$F8
		.BYTE	$F0,$46,$02,$00
		.BYTE	$F0,$45,$02,$F8
		.BYTE	$80
_spr_monster16:
		.BYTE	$FF,$5A,$01,$FF
		.BYTE	$F7,$4A,$01,$01
		.BYTE	$FF,$79,$41,$0A
		.BYTE	$F7,$78,$41,$0A
		.BYTE	$FA,$69,$02,$10
		.BYTE	$12,$68,$02,$F8
		.BYTE	$0A,$67,$02,$FA
		.BYTE	$02,$59,$02,$08
		.BYTE	$02,$58,$02,$00
		.BYTE	$02,$57,$02,$F8
		.BYTE	$FA,$49,$02,$08
		.BYTE	$FA,$48,$02,$00
		.BYTE	$FA,$47,$02,$F8
		.BYTE	$80
_spr_monster1B:
		.BYTE	$FE,$73,$01,$03
		.BYTE	$F6,$72,$01,$03
		.BYTE	$E6,$62,$01,$04
		.BYTE	$E6,$62,$01,$FC
		.BYTE	$EE,$71,$01,$04
		.BYTE	$EE,$70,$01,$FC
		.BYTE	$E8,$52,$02,$02
		.BYTE	$FE,$43,$02,$12
		.BYTE	$FE,$42,$02,$0A
		.BYTE	$00,$61,$02,$02
		.BYTE	$00,$60,$02,$FA
		.BYTE	$F8,$51,$02,$02
		.BYTE	$F8,$50,$02,$FA
		.BYTE	$F0,$41,$02,$02
		.BYTE	$F0,$40,$02,$FA
		.BYTE	$80
_spr_monster19:
		.BYTE	$00,$75,$01,$07
		.BYTE	$00,$74,$01,$FF
		.BYTE	$F0,$76,$01,$02
		.BYTE	$F8,$77,$01,$02
		.BYTE	$FC,$43,$02,$13
		.BYTE	$FC,$42,$02,$0B
		.BYTE	$00,$64,$02,$03
		.BYTE	$00,$63,$02,$FB
		.BYTE	$F8,$54,$02,$03
		.BYTE	$F8,$53,$02,$FB
		.BYTE	$80
_spr_monster28:
		.BYTE	$EF,$76,$01,$00
		.BYTE	$F0,$5B,$02,$00
		.BYTE	$ED,$7E,$01,$0B
		.BYTE	$F8,$7E,$81,$08
		.BYTE	$E9,$7E,$01,$02
		.BYTE	$F7,$78,$01,$F8
		.BYTE	$F8,$4D,$02,$00
		.BYTE	$F8,$4C,$02,$F8
		.BYTE	$EF,$77,$01,$08
		.BYTE	$EF,$75,$01,$F8
		.BYTE	$F0,$44,$02,$F8
		.BYTE	$80
_spr_monster29:
		.BYTE	$EB,$7E,$01,$04
		.BYTE	$F9,$7F,$01,$09
		.BYTE	$F0,$4F,$01,$08
		.BYTE	$F8,$78,$01,$F8
		.BYTE	$F9,$4E,$02,$00
		.BYTE	$F0,$76,$01,$00
		.BYTE	$F0,$75,$01,$F8
		.BYTE	$F1,$45,$02,$00
		.BYTE	$F1,$44,$02,$F8
		.BYTE	$80
_spr_monster1C:
		.BYTE	$F5,$4E,$81,$05
		.BYTE	$02,$4E,$41,$F9
		.BYTE	$00,$7D,$01,$00
		.BYTE	$F8,$6D,$01,$00
		.BYTE	$F8,$6C,$01,$F8
		.BYTE	$F0,$5D,$01,$00
		.BYTE	$F0,$5C,$01,$F8
		.BYTE	$00,$72,$02,$FB
		.BYTE	$F8,$55,$02,$00
		.BYTE	$F8,$54,$02,$F8
		.BYTE	$F0,$45,$02,$00
		.BYTE	$F0,$44,$02,$F8
		.BYTE	$F8,$40,$02,$F0
		.BYTE	$80
_spr_monster1D:
		.BYTE	$FE,$4E,$41,$EF
		.BYTE	$F0,$5C,$01,$F7
		.BYTE	$F8,$4F,$C1,$05
		.BYTE	$00,$4F,$01,$F8
		.BYTE	$00,$7D,$01,$FF
		.BYTE	$F8,$6D,$01,$FF
		.BYTE	$F8,$6C,$01,$F7
		.BYTE	$F0,$5D,$01,$FF
		.BYTE	$F8,$4D,$02,$FF
		.BYTE	$F8,$4C,$02,$F7
		.BYTE	$F0,$4A,$02,$F7
		.BYTE	$F0,$45,$02,$FF
		.BYTE	$80
_spr_monster00:
		.BYTE	$11,$43,$42,$06
		.BYTE	$FE,$7B,$01,$08
		.BYTE	$FE,$7A,$01,$00
		.BYTE	$F6,$6B,$01,$08
		.BYTE	$F6,$6A,$01,$00
		.BYTE	$EE,$5B,$01,$08
		.BYTE	$EE,$5A,$01,$00
		.BYTE	$0F,$76,$02,$00
		.BYTE	$0F,$75,$02,$F8
		.BYTE	$07,$71,$02,$00
		.BYTE	$07,$70,$02,$F8
		.BYTE	$FF,$62,$02,$08
		.BYTE	$FF,$61,$02,$00
		.BYTE	$FF,$60,$02,$F9
		.BYTE	$F7,$52,$02,$08
		.BYTE	$F7,$51,$02,$00
		.BYTE	$F7,$50,$02,$F9
		.BYTE	$EF,$42,$02,$08
		.BYTE	$EF,$41,$02,$00
		.BYTE	$80
_spr_monster01:
		.BYTE	$12,$43,$42,$FE
		.BYTE	$00,$7D,$01,$08
		.BYTE	$00,$7C,$01,$00
		.BYTE	$F8,$6D,$01,$08
		.BYTE	$F8,$6C,$01,$00
		.BYTE	$F0,$5D,$01,$08
		.BYTE	$F0,$5C,$01,$00
		.BYTE	$08,$74,$02,$00
		.BYTE	$10,$56,$02,$F8
		.BYTE	$08,$73,$02,$F8
		.BYTE	$00,$63,$02,$F8
		.BYTE	$F8,$53,$02,$F8
		.BYTE	$00,$65,$02,$08
		.BYTE	$00,$64,$02,$00
		.BYTE	$F8,$55,$02,$08
		.BYTE	$F8,$54,$02,$00
		.BYTE	$F0,$45,$02,$08
		.BYTE	$F0,$44,$02,$00
		.BYTE	$80
_spr_monster02:
		.BYTE	$11,$43,$02,$F2
		.BYTE	$FE,$7F,$01,$08
		.BYTE	$FE,$7E,$01,$00
		.BYTE	$F6,$6F,$01,$08
		.BYTE	$F6,$6E,$01,$00
		.BYTE	$EE,$5F,$01,$08
		.BYTE	$EE,$5E,$01,$00
		.BYTE	$0F,$79,$02,$F7
		.BYTE	$07,$78,$02,$00
		.BYTE	$07,$77,$02,$F8
		.BYTE	$FF,$67,$02,$F8
		.BYTE	$F7,$57,$02,$F8
		.BYTE	$FF,$69,$02,$08
		.BYTE	$FF,$68,$02,$00
		.BYTE	$F7,$59,$02,$08
		.BYTE	$F7,$58,$02,$00
		.BYTE	$EF,$49,$02,$08
		.BYTE	$EF,$48,$02,$00
		.BYTE	$80
_spr_monster03:
		.BYTE	$11,$43,$02,$F8
		.BYTE	$00,$7D,$01,$08
		.BYTE	$00,$7C,$01,$00
		.BYTE	$F8,$6D,$01,$08
		.BYTE	$F8,$6C,$01,$00
		.BYTE	$F0,$5D,$01,$08
		.BYTE	$F0,$5C,$01,$00
		.BYTE	$10,$79,$02,$FC
		.BYTE	$08,$47,$02,$00
		.BYTE	$08,$56,$02,$F8
		.BYTE	$00,$63,$02,$F8
		.BYTE	$F8,$53,$02,$F8
		.BYTE	$00,$65,$02,$08
		.BYTE	$00,$64,$02,$00
		.BYTE	$F8,$55,$02,$08
		.BYTE	$F8,$54,$02,$00
		.BYTE	$F0,$45,$02,$08
		.BYTE	$F0,$44,$02,$00
		.BYTE	$80
_spr_monster0C:
		.BYTE	$F7,$74,$01,$FF
		.BYTE	$F7,$73,$01,$F7
		.BYTE	$EF,$72,$01,$07
		.BYTE	$EF,$71,$01,$FF
		.BYTE	$EF,$70,$01,$F7
		.BYTE	$F8,$53,$02,$10
		.BYTE	$F0,$43,$02,$10
		.BYTE	$00,$61,$02,$00
		.BYTE	$00,$60,$02,$F8
		.BYTE	$FC,$52,$02,$08
		.BYTE	$F8,$51,$02,$00
		.BYTE	$F8,$50,$02,$F8
		.BYTE	$F4,$42,$02,$08
		.BYTE	$F0,$41,$02,$00
		.BYTE	$F0,$40,$02,$F8
		.BYTE	$80
_spr_monster0D:
		.BYTE	$F7,$7A,$01,$07
		.BYTE	$F7,$79,$01,$00
		.BYTE	$F7,$78,$01,$F8
		.BYTE	$EF,$77,$01,$08
		.BYTE	$EF,$76,$01,$00
		.BYTE	$EF,$75,$01,$F8
		.BYTE	$FB,$62,$02,$10
		.BYTE	$00,$66,$02,$08
		.BYTE	$00,$65,$02,$00
		.BYTE	$00,$64,$02,$F8
		.BYTE	$F8,$56,$02,$08
		.BYTE	$F8,$55,$02,$00
		.BYTE	$F8,$54,$02,$F8
		.BYTE	$F0,$45,$02,$00
		.BYTE	$F0,$44,$02,$F8
		.BYTE	$80
_spr_monster0E:
		.BYTE	$F7,$7D,$01,$07
		.BYTE	$F7,$7C,$01,$FF
		.BYTE	$F7,$7B,$01,$F7
		.BYTE	$EF,$6C,$01,$07
		.BYTE	$EF,$6B,$01,$FF
		.BYTE	$EF,$6A,$01,$F7
		.BYTE	$FA,$46,$02,$10
		.BYTE	$02,$49,$02,$10
		.BYTE	$00,$69,$02,$08
		.BYTE	$00,$68,$02,$00
		.BYTE	$00,$67,$02,$F8
		.BYTE	$F8,$59,$02,$08
		.BYTE	$F8,$58,$02,$00
		.BYTE	$F8,$57,$02,$F8
		.BYTE	$F0,$48,$02,$00
		.BYTE	$F0,$47,$02,$F8
		.BYTE	$80
_spr_monster0F:
		.BYTE	$FA,$46,$02,$18
		.BYTE	$F7,$7A,$01,$07
		.BYTE	$F7,$79,$01,$00
		.BYTE	$F7,$78,$01,$F8
		.BYTE	$EF,$77,$01,$08
		.BYTE	$EF,$76,$01,$00
		.BYTE	$EF,$75,$01,$F8
		.BYTE	$F8,$4B,$02,$10
		.BYTE	$00,$5A,$02,$08
		.BYTE	$F8,$4A,$02,$08
		.BYTE	$00,$65,$02,$00
		.BYTE	$00,$64,$02,$F8
		.BYTE	$F8,$55,$02,$00
		.BYTE	$F8,$54,$02,$F8
		.BYTE	$F0,$45,$02,$00
		.BYTE	$F0,$44,$02,$F8
		.BYTE	$80
_spr_monster11:
		.BYTE	$EB,$5D,$01,$FE
		.BYTE	$F4,$4F,$01,$06
		.BYTE	$F3,$4E,$01,$FE
		.BYTE	$E8,$4D,$02,$F8
		.BYTE	$F8,$5C,$02,$00
		.BYTE	$F8,$5B,$02,$F8
		.BYTE	$F0,$4C,$02,$00
		.BYTE	$F0,$4B,$02,$F8
		.BYTE	$FB,$43,$02,$10
		.BYTE	$FB,$42,$02,$08
		.BYTE	$00,$61,$02,$02
		.BYTE	$00,$60,$02,$FA
		.BYTE	$80
_spr_monster13:
		.BYTE	$05,$7C,$01,$00
		.BYTE	$FD,$6C,$01,$03
		.BYTE	$00,$7F,$02,$08
		.BYTE	$F3,$7D,$01,$09
		.BYTE	$F3,$7B,$01,$01
		.BYTE	$12,$6D,$02,$00
		.BYTE	$F0,$6D,$02,$10
		.BYTE	$01,$57,$02,$F8
		.BYTE	$FA,$47,$02,$F8
		.BYTE	$00,$7E,$02,$00
		.BYTE	$F8,$6F,$02,$08
		.BYTE	$F8,$6E,$02,$00
		.BYTE	$F0,$5F,$02,$08
		.BYTE	$F0,$5E,$02,$00
		.BYTE	$10,$68,$02,$F9
		.BYTE	$08,$67,$02,$FB
		.BYTE	$80
_spr_monster08:
		.BYTE	$03,$59,$01,$00
		.BYTE	$FB,$49,$01,$00
		.BYTE	$03,$7B,$01,$08
		.BYTE	$FB,$6B,$01,$08
		.BYTE	$F3,$5B,$01,$08
		.BYTE	$EA,$42,$02,$09
		.BYTE	$EA,$41,$02,$01
		.BYTE	$02,$72,$02,$06
		.BYTE	$02,$71,$02,$FE
		.BYTE	$FA,$62,$02,$06
		.BYTE	$FA,$61,$02,$FE
		.BYTE	$F2,$52,$02,$06
		.BYTE	$F2,$51,$02,$FE
		.BYTE	$02,$70,$02,$F6
		.BYTE	$FA,$60,$02,$F6
		.BYTE	$80
_spr_monster09:
		.BYTE	$F1,$5C,$01,$01
		.BYTE	$00,$7D,$01,$09
		.BYTE	$00,$7C,$01,$01
		.BYTE	$F8,$6D,$01,$09
		.BYTE	$F8,$6C,$01,$01
		.BYTE	$F0,$5D,$01,$09
		.BYTE	$E0,$40,$02,$00
		.BYTE	$E8,$50,$02,$FD
		.BYTE	$F0,$54,$02,$00
		.BYTE	$F0,$53,$02,$F8
		.BYTE	$00,$75,$02,$08
		.BYTE	$00,$74,$02,$00
		.BYTE	$00,$73,$02,$F8
		.BYTE	$F8,$65,$02,$08
		.BYTE	$F8,$64,$02,$00
		.BYTE	$F8,$63,$02,$F8
		.BYTE	$80
_spr_monster0A:
		.BYTE	$03,$7F,$01,$09
		.BYTE	$03,$7E,$01,$01
		.BYTE	$FB,$6F,$01,$09
		.BYTE	$FB,$6E,$01,$01
		.BYTE	$F3,$5F,$01,$09
		.BYTE	$F3,$5E,$01,$01
		.BYTE	$E9,$58,$02,$F6
		.BYTE	$E9,$55,$02,$FE
		.BYTE	$F1,$57,$02,$00
		.BYTE	$F1,$56,$02,$F8
		.BYTE	$01,$78,$02,$08
		.BYTE	$01,$77,$02,$00
		.BYTE	$01,$76,$02,$F8
		.BYTE	$F9,$68,$02,$08
		.BYTE	$F9,$67,$02,$00
		.BYTE	$F9,$66,$02,$F8
		.BYTE	$80
_spr_monster0B:
		.BYTE	$F1,$5C,$01,$01
		.BYTE	$00,$7D,$01,$09
		.BYTE	$00,$7C,$01,$01
		.BYTE	$F8,$6D,$01,$09
		.BYTE	$F8,$6C,$01,$01
		.BYTE	$F0,$5D,$01,$09
		.BYTE	$E0,$69,$02,$FE
		.BYTE	$E8,$6A,$02,$00
		.BYTE	$F0,$7A,$02,$00
		.BYTE	$F0,$79,$02,$F8
		.BYTE	$00,$75,$02,$08
		.BYTE	$00,$74,$02,$00
		.BYTE	$00,$73,$02,$F8
		.BYTE	$F8,$65,$02,$08
		.BYTE	$F8,$64,$02,$00
		.BYTE	$F8,$63,$02,$F8
		.BYTE	$80
_spr_monster04:
		.BYTE	$F8,$75,$01,$09
		.BYTE	$F8,$74,$01,$01
		.BYTE	$F8,$73,$01,$F8
		.BYTE	$F0,$72,$01,$09
		.BYTE	$F0,$71,$01,$01
		.BYTE	$F0,$70,$01,$F9
		.BYTE	$00,$54,$02,$E8
		.BYTE	$F8,$44,$02,$E8
		.BYTE	$00,$60,$02,$F0
		.BYTE	$F8,$50,$02,$F0
		.BYTE	$00,$62,$02,$00
		.BYTE	$00,$61,$02,$F8
		.BYTE	$F8,$53,$02,$08
		.BYTE	$F8,$52,$02,$00
		.BYTE	$F8,$51,$02,$F8
		.BYTE	$F0,$43,$02,$08
		.BYTE	$F0,$42,$02,$00
		.BYTE	$F0,$41,$02,$F8
		.BYTE	$80
_spr_monster05:
		.BYTE	$F8,$7B,$01,$08
		.BYTE	$F8,$7A,$01,$00
		.BYTE	$F8,$79,$01,$F8
		.BYTE	$F0,$78,$01,$08
		.BYTE	$F0,$77,$01,$00
		.BYTE	$F0,$76,$01,$F8
		.BYTE	$00,$64,$02,$E8
		.BYTE	$00,$65,$02,$F0
		.BYTE	$F8,$55,$02,$F0
		.BYTE	$00,$67,$02,$00
		.BYTE	$00,$66,$02,$F8
		.BYTE	$F8,$58,$02,$08
		.BYTE	$F8,$57,$02,$00
		.BYTE	$F8,$56,$02,$F8
		.BYTE	$F0,$48,$02,$08
		.BYTE	$F0,$47,$02,$00
		.BYTE	$F0,$46,$02,$F8
		.BYTE	$80
_spr_monster06:
		.BYTE	$F8,$6F,$01,$09
		.BYTE	$F8,$6E,$01,$01
		.BYTE	$F8,$6D,$01,$F9
		.BYTE	$F0,$7E,$01,$09
		.BYTE	$F0,$7D,$01,$01
		.BYTE	$F0,$7C,$01,$F9
		.BYTE	$08,$68,$02,$E8
		.BYTE	$00,$69,$02,$E8
		.BYTE	$00,$6A,$02,$F0
		.BYTE	$F8,$5A,$02,$F0
		.BYTE	$00,$6C,$02,$00
		.BYTE	$00,$6B,$02,$F8
		.BYTE	$F8,$5D,$02,$08
		.BYTE	$F8,$5C,$02,$00
		.BYTE	$F8,$5B,$02,$F8
		.BYTE	$F0,$4D,$02,$08
		.BYTE	$F0,$4C,$02,$00
		.BYTE	$F0,$4B,$02,$F8
		.BYTE	$80
_spr_monster07:
		.BYTE	$F8,$7B,$01,$08
		.BYTE	$F8,$7A,$01,$00
		.BYTE	$F8,$79,$01,$F8
		.BYTE	$F0,$78,$01,$08
		.BYTE	$F0,$77,$01,$00
		.BYTE	$F0,$76,$01,$F8
		.BYTE	$00,$68,$02,$E7
		.BYTE	$FC,$4A,$02,$E8
		.BYTE	$00,$59,$02,$F0
		.BYTE	$F8,$49,$02,$F0
		.BYTE	$00,$67,$02,$00
		.BYTE	$00,$66,$02,$F8
		.BYTE	$F8,$58,$02,$08
		.BYTE	$F8,$57,$02,$00
		.BYTE	$F8,$56,$02,$F8
		.BYTE	$F0,$48,$02,$08
		.BYTE	$F0,$47,$02,$00
		.BYTE	$F0,$46,$02,$F8
		.BYTE	$80
_spr_monster15:
		.BYTE	$F9,$4A,$01,$09
		.BYTE	$F9,$49,$01,$01
		.BYTE	$FD,$6A,$41,$F9
		.BYTE	$F5,$5A,$41,$F9
		.BYTE	$FF,$47,$02,$01
		.BYTE	$FF,$46,$02,$F9
		.BYTE	$EF,$69,$02,$FC
		.BYTE	$E7,$59,$02,$FC
		.BYTE	$F7,$45,$02,$01
		.BYTE	$F7,$44,$02,$F9
		.BYTE	$80
_spr_monster14:
		.BYTE	$F6,$6C,$01,$02
		.BYTE	$F6,$6B,$01,$FA
		.BYTE	$FD,$6A,$01,$0A
		.BYTE	$F5,$5A,$01,$0A
		.BYTE	$F7,$48,$02,$10
		.BYTE	$EF,$69,$02,$F8
		.BYTE	$E7,$59,$02,$F8
		.BYTE	$FF,$68,$02,$08
		.BYTE	$FF,$67,$02,$00
		.BYTE	$FF,$66,$02,$F8
		.BYTE	$F7,$58,$02,$08
		.BYTE	$F7,$57,$02,$00
		.BYTE	$F7,$56,$02,$F8
		.BYTE	$80
_spr_monster1A:
		.BYTE	$F5,$79,$01,$00
		.BYTE	$F7,$78,$01,$F8
		.BYTE	$E5,$77,$01,$00
		.BYTE	$E5,$77,$01,$F8
		.BYTE	$ED,$76,$01,$00
		.BYTE	$ED,$75,$01,$F8
		.BYTE	$00,$50,$02,$E6
		.BYTE	$FE,$60,$02,$EE
		.BYTE	$E8,$40,$01,$FE
		.BYTE	$F0,$42,$02,$FE
		.BYTE	$F0,$41,$02,$F6
		.BYTE	$00,$62,$02,$FE
		.BYTE	$00,$61,$02,$F6
		.BYTE	$F8,$52,$02,$FE
		.BYTE	$F8,$51,$02,$F6
		.BYTE	$80
_spr_monster18:
		.BYTE	$00,$74,$01,$06
		.BYTE	$00,$73,$01,$FE
		.BYTE	$F8,$72,$01,$FE
		.BYTE	$F8,$71,$01,$F6
		.BYTE	$F0,$70,$01,$FE
		.BYTE	$00,$50,$02,$E8
		.BYTE	$FE,$60,$02,$F0
		.BYTE	$00,$65,$02,$08
		.BYTE	$00,$64,$02,$00
		.BYTE	$00,$63,$02,$F8
		.BYTE	$F8,$55,$02,$08
		.BYTE	$F8,$54,$02,$00
		.BYTE	$F8,$53,$02,$F8
		.BYTE	$80
_spr_monster20:
		.BYTE	$EB,$7F,$01,$FA
		.BYTE	$FF,$7F,$81,$F9
		.BYTE	$F0,$76,$01,$F8
		.BYTE	$00,$45,$02,$00
		.BYTE	$F5,$40,$02,$F8
		.BYTE	$F8,$7B,$01,$08
		.BYTE	$F8,$7A,$01,$00
		.BYTE	$F0,$78,$01,$08
		.BYTE	$F0,$77,$01,$00
		.BYTE	$F8,$58,$02,$08
		.BYTE	$F8,$57,$02,$00
		.BYTE	$F0,$48,$02,$08
		.BYTE	$F0,$47,$02,$00
		.BYTE	$80
_spr_monster21:
		.BYTE	$EC,$4F,$81,$F9
		.BYTE	$FF,$4F,$01,$F9
		.BYTE	$F1,$76,$01,$F8
		.BYTE	$F1,$77,$01,$00
		.BYTE	$F9,$7B,$01,$08
		.BYTE	$F9,$5E,$02,$00
		.BYTE	$F1,$4E,$02,$00
		.BYTE	$F9,$5F,$02,$08
		.BYTE	$F1,$78,$01,$08
		.BYTE	$F1,$48,$02,$08
		.BYTE	$80
_spr_monster24:
		.BYTE	$F3,$4F,$01,$0F
		.BYTE	$F2,$4F,$41,$FD
		.BYTE	$00,$7D,$01,$09
		.BYTE	$00,$7C,$01,$01
		.BYTE	$F8,$6D,$01,$09
		.BYTE	$F8,$6C,$01,$01
		.BYTE	$F0,$5D,$01,$09
		.BYTE	$00,$46,$02,$00
		.BYTE	$F8,$45,$02,$00
		.BYTE	$F8,$43,$02,$08
		.BYTE	$00,$75,$02,$08
		.BYTE	$80
_spr_monster25:
		.BYTE	$F1,$4E,$41,$0E
		.BYTE	$F1,$4E,$01,$FD
		.BYTE	$F0,$47,$01,$08
		.BYTE	$00,$7D,$01,$08
		.BYTE	$00,$7C,$01,$00
		.BYTE	$F8,$6D,$01,$08
		.BYTE	$F8,$6C,$01,$00
		.BYTE	$F8,$44,$02,$FF
		.BYTE	$00,$75,$02,$07
		.BYTE	$F8,$65,$02,$07
		.BYTE	$80
_spr_monster12:
		.BYTE	$F9,$4E,$01,$0C
		.BYTE	$F9,$5F,$01,$04
		.BYTE	$F1,$4F,$01,$03
		.BYTE	$FD,$6F,$02,$10
		.BYTE	$FE,$6E,$02,$08
		.BYTE	$FE,$6D,$02,$00
		.BYTE	$F6,$5E,$02,$08
		.BYTE	$F6,$5D,$02,$00
		.BYTE	$EE,$69,$02,$FC
		.BYTE	$E6,$59,$02,$FC
		.BYTE	$FE,$66,$02,$F8
		.BYTE	$F6,$56,$02,$F8
		.BYTE	$80
_spr_monster10:
		.BYTE	$EE,$4D,$01,$03
		.BYTE	$F8,$4B,$01,$F2
		.BYTE	$F5,$4C,$01,$F9
		.BYTE	$F7,$5C,$01,$03
		.BYTE	$F0,$7C,$02,$06
		.BYTE	$F0,$7B,$02,$FE
		.BYTE	$F0,$7A,$02,$F6
		.BYTE	$F8,$7F,$02,$06
		.BYTE	$F8,$7E,$02,$FE
		.BYTE	$F8,$7D,$02,$F6
		.BYTE	$FF,$50,$02,$E6
		.BYTE	$00,$62,$02,$FE
		.BYTE	$00,$61,$02,$F6
		.BYTE	$FE,$60,$02,$EE
		.BYTE	$80
_spr_heliA0:
		.BYTE	$FA,$18,$01,$F9
		.BYTE	$FA,$19,$01,$01
		.BYTE	$02,$28,$01,$F9
		.BYTE	$02,$29,$01,$01
		.BYTE	$0A,$2E,$01,$FD
		.BYTE	$80
_spr_heliB0:
		.BYTE	$FA,$30,$01,$F9
		.BYTE	$FA,$31,$01,$01
		.BYTE	$02,$08,$01,$F9
		.BYTE	$02,$09,$01,$01
		.BYTE	$0A,$1E,$01,$FD
		.BYTE	$80
_spr_heliC0:
		.BYTE	$FA,$1C,$01,$F9
		.BYTE	$FA,$1D,$01,$01
		.BYTE	$02,$2C,$01,$F9
		.BYTE	$02,$2D,$01,$01
		.BYTE	$0A,$2E,$01,$FD
		.BYTE	$80
_spr_heliD0:
		.BYTE	$FA,$1A,$01,$F9
		.BYTE	$FA,$1B,$01,$01
		.BYTE	$02,$2A,$01,$F9
		.BYTE	$02,$2B,$01,$01
		.BYTE	$0A,$1E,$01,$FD
		.BYTE	$80
_spr_heliA2:
		.BYTE	$FC,$3D,$41,$EF
		.BYTE	$F8,$33,$41,$F7
		.BYTE	$F8,$32,$41,$FF
		.BYTE	$00,$0B,$41,$F7
		.BYTE	$00,$0A,$41,$FF
		.BYTE	$80
_spr_heliB2:
		.BYTE	$FC,$3A,$41,$EF
		.BYTE	$F8,$39,$41,$F7
		.BYTE	$F8,$38,$41,$FF
		.BYTE	$00,$3C,$41,$F7
		.BYTE	$00,$3B,$41,$FF
		.BYTE	$80
_spr_heliC2:
		.BYTE	$FC,$3D,$41,$EF
		.BYTE	$F8,$37,$41,$F7
		.BYTE	$F8,$36,$41,$FF
		.BYTE	$00,$0F,$41,$F7
		.BYTE	$00,$0E,$41,$FF
		.BYTE	$80
_spr_heliD2:
		.BYTE	$FC,$3A,$41,$EF
		.BYTE	$F8,$35,$41,$F7
		.BYTE	$F8,$34,$41,$FF
		.BYTE	$00,$0D,$41,$F7
		.BYTE	$00,$0C,$41,$FF
		.BYTE	$80
_spr_heliA4:
		.BYTE	$EF,$2E,$81,$FD
		.BYTE	$F7,$28,$81,$F9
		.BYTE	$F7,$29,$81,$01
		.BYTE	$FF,$18,$81,$F9
		.BYTE	$FF,$19,$81,$01
		.BYTE	$80
_spr_heliB4:
		.BYTE	$EF,$1E,$81,$FD
		.BYTE	$F7,$08,$81,$F9
		.BYTE	$F7,$09,$81,$01
		.BYTE	$FF,$30,$81,$F9
		.BYTE	$FF,$31,$81,$01
		.BYTE	$80
_spr_heliC4:
		.BYTE	$EF,$2E,$81,$FD
		.BYTE	$F7,$2C,$81,$F9
		.BYTE	$F7,$2D,$81,$01
		.BYTE	$FF,$1C,$81,$F9
		.BYTE	$FF,$1D,$81,$01
		.BYTE	$80
_spr_heliD4:
		.BYTE	$EF,$1E,$81,$FD
		.BYTE	$F7,$2A,$81,$F9
		.BYTE	$F7,$2B,$81,$01
		.BYTE	$FF,$1A,$81,$F9
		.BYTE	$FF,$1B,$81,$01
		.BYTE	$80
_spr_heliA6:
		.BYTE	$F8,$32,$01,$FA
		.BYTE	$F8,$33,$01,$02
		.BYTE	$00,$0A,$01,$FA
		.BYTE	$00,$0B,$01,$02
		.BYTE	$FC,$3D,$01,$0A
		.BYTE	$80
_spr_heliB6:
		.BYTE	$F8,$38,$01,$FA
		.BYTE	$F8,$39,$01,$02
		.BYTE	$00,$3B,$01,$FA
		.BYTE	$00,$3C,$01,$02
		.BYTE	$FC,$3A,$01,$0A
		.BYTE	$80
_spr_heliC6:
		.BYTE	$F8,$36,$01,$FA
		.BYTE	$F8,$37,$01,$02
		.BYTE	$00,$0E,$01,$FA
		.BYTE	$00,$0F,$01,$02
		.BYTE	$FC,$3D,$01,$0A
		.BYTE	$80
_spr_heliD6:
		.BYTE	$F8,$34,$01,$FA
		.BYTE	$F8,$35,$01,$02
		.BYTE	$00,$0C,$01,$FA
		.BYTE	$00,$0D,$01,$02
		.BYTE	$FC,$3A,$01,$0A
		.BYTE	$80
_spr_heliA1:
		.BYTE	$FA,$11,$41,$F7
		.BYTE	$FA,$10,$41,$FF
		.BYTE	$02,$21,$41,$F7
		.BYTE	$02,$20,$41,$FF
		.BYTE	$80
_spr_heliB1:
		.BYTE	$FA,$13,$41,$F7
		.BYTE	$FA,$12,$41,$FF
		.BYTE	$02,$23,$41,$F7
		.BYTE	$02,$22,$41,$FF
		.BYTE	$80
_spr_heliC1:
		.BYTE	$FA,$15,$41,$F7
		.BYTE	$FA,$14,$41,$FF
		.BYTE	$02,$25,$41,$F7
		.BYTE	$02,$24,$41,$FF
		.BYTE	$80
_spr_heliD1:
		.BYTE	$FA,$17,$41,$F7
		.BYTE	$FA,$16,$41,$FF
		.BYTE	$02,$27,$41,$F7
		.BYTE	$02,$26,$41,$FF
		.BYTE	$80
_spr_heliA3:
		.BYTE	$F7,$21,$C1,$F7
		.BYTE	$F7,$20,$C1,$FF
		.BYTE	$FF,$11,$C1,$F7
		.BYTE	$FF,$10,$C1,$FF
		.BYTE	$80
_spr_heliB3:
		.BYTE	$F7,$23,$C1,$F7
		.BYTE	$F7,$22,$C1,$FF
		.BYTE	$FF,$13,$C1,$F7
		.BYTE	$FF,$12,$C1,$FF
		.BYTE	$80
_spr_heliC3:
		.BYTE	$F7,$25,$C1,$F7
		.BYTE	$F7,$24,$C1,$FF
		.BYTE	$FF,$15,$C1,$F7
		.BYTE	$FF,$14,$C1,$FF
		.BYTE	$80
_spr_heliD3:
		.BYTE	$F7,$27,$C1,$F7
		.BYTE	$F7,$26,$C1,$FF
		.BYTE	$FF,$17,$C1,$F7
		.BYTE	$FF,$16,$C1,$FF
		.BYTE	$80
_spr_heliA5:
		.BYTE	$F7,$20,$81,$FA
		.BYTE	$F7,$21,$81,$02
		.BYTE	$FF,$10,$81,$FA
		.BYTE	$FF,$11,$81,$02
		.BYTE	$80
_spr_heliB5:
		.BYTE	$F7,$22,$81,$FA
		.BYTE	$F7,$23,$81,$02
		.BYTE	$FF,$12,$81,$FA
		.BYTE	$FF,$13,$81,$02
		.BYTE	$80
_spr_heliC5:
		.BYTE	$F7,$24,$81,$FA
		.BYTE	$F7,$25,$81,$02
		.BYTE	$FF,$14,$81,$FA
		.BYTE	$FF,$15,$81,$02
		.BYTE	$80
_spr_heliD5:
		.BYTE	$F7,$26,$81,$FA
		.BYTE	$F7,$27,$81,$02
		.BYTE	$FF,$16,$81,$FA
		.BYTE	$FF,$17,$81,$02
		.BYTE	$80
_spr_heliA7:
		.BYTE	$FA,$10,$01,$FA
		.BYTE	$FA,$11,$01,$02
		.BYTE	$02,$20,$01,$FA
		.BYTE	$02,$21,$01,$02
		.BYTE	$80
_spr_heliB7:
		.BYTE	$FA,$12,$01,$FA
		.BYTE	$FA,$13,$01,$02
		.BYTE	$02,$22,$01,$FA
		.BYTE	$02,$23,$01,$02
		.BYTE	$80
_spr_heliC7:
		.BYTE	$FA,$14,$01,$FA
		.BYTE	$FA,$15,$01,$02
		.BYTE	$02,$24,$01,$FA
		.BYTE	$02,$25,$01,$02
		.BYTE	$80
_spr_heliD7:
		.BYTE	$FA,$16,$01,$FA
		.BYTE	$FA,$17,$01,$02
		.BYTE	$02,$26,$01,$FA
		.BYTE	$02,$27,$01,$02
		.BYTE	$80
_spr_train0:
		.BYTE	$F7,$82,$01,$02
		.BYTE	$FF,$83,$01,$02
		.BYTE	$80
_spr_train1:
		.BYTE	$F8,$85,$41,$FB
		.BYTE	$F8,$84,$41,$03
		.BYTE	$00,$87,$41,$FB
		.BYTE	$00,$86,$41,$03
		.BYTE	$80
_spr_train2:
		.BYTE	$FD,$80,$01,$FA
		.BYTE	$FD,$81,$01,$02
		.BYTE	$80
_spr_train3:
		.BYTE	$F8,$84,$01,$F8
		.BYTE	$F8,$85,$01,$00
		.BYTE	$00,$86,$01,$F8
		.BYTE	$00,$87,$01,$00
		.BYTE	$80
_spr_shipA0:
		.BYTE	$F0,$B4,$01,$FC
		.BYTE	$F8,$B5,$01,$FC
		.BYTE	$00,$B6,$01,$FC
		.BYTE	$08,$B7,$01,$FC
		.BYTE	$80
_spr_shipA1:
		.BYTE	$F4,$C8,$41,$FC
		.BYTE	$F4,$C7,$41,$04
		.BYTE	$FC,$CB,$41,$F4
		.BYTE	$FC,$CA,$41,$FC
		.BYTE	$FC,$C9,$41,$04
		.BYTE	$04,$CD,$41,$F4
		.BYTE	$04,$CC,$41,$FC
		.BYTE	$80
_spr_shipA2:
		.BYTE	$F8,$BA,$41,$F2
		.BYTE	$F8,$B9,$41,$FA
		.BYTE	$F8,$B8,$41,$02
		.BYTE	$00,$BE,$41,$F2
		.BYTE	$00,$BD,$41,$FA
		.BYTE	$00,$BC,$41,$02
		.BYTE	$00,$BB,$41,$0A
		.BYTE	$80
_spr_shipA3:
		.BYTE	$F6,$C1,$41,$EF
		.BYTE	$F6,$C0,$41,$F7
;		.BYTE	$F6,$CF,$41,$FF		; BUG
		.BYTE	$F6,$BF,$41,$FF		; FIX
		.BYTE	$FE,$C4,$41,$EF
		.BYTE	$FE,$C3,$41,$F7
		.BYTE	$FE,$C2,$41,$FF
		.BYTE	$06,$C6,$41,$F7
		.BYTE	$06,$C5,$41,$FF
		.BYTE	$80
_spr_shipA4:
		.BYTE	$F2,$B0,$01,$FC
		.BYTE	$FA,$B1,$01,$FC
		.BYTE	$02,$B2,$01,$FC
		.BYTE	$0A,$B3,$01,$FC
		.BYTE	$80
_spr_shipA5:
		.BYTE	$F6,$BF,$01,$F4
		.BYTE	$F6,$C0,$01,$FC
		.BYTE	$F6,$C1,$01,$04
		.BYTE	$FE,$C2,$01,$F4
		.BYTE	$FE,$C3,$01,$FC
		.BYTE	$FE,$C4,$01,$04
		.BYTE	$06,$C5,$01,$F4
		.BYTE	$06,$C6,$01,$FC
		.BYTE	$80
_spr_shipA6:
		.BYTE	$F8,$B8,$01,$F0
		.BYTE	$F8,$B9,$01,$F8
		.BYTE	$F8,$BA,$01,$00
		.BYTE	$00,$BB,$01,$E8
		.BYTE	$00,$BC,$01,$F0
		.BYTE	$00,$BD,$01,$F8
		.BYTE	$00,$BE,$01,$00
		.BYTE	$80
_spr_shipA7:
		.BYTE	$F7,$C7,$01,$F4
		.BYTE	$F7,$C8,$01,$FC
		.BYTE	$FF,$C9,$01,$F4
		.BYTE	$FF,$CA,$01,$FC
		.BYTE	$FF,$CB,$01,$04
		.BYTE	$07,$CC,$01,$FC
		.BYTE	$07,$CD,$01,$04
		.BYTE	$80
_spr_shipB0:
		.BYTE	$F0,$AF,$01,$FF
		.BYTE	$F8,$B4,$01,$F8
		.BYTE	$F8,$B5,$01,$00
		.BYTE	$00,$B6,$01,$F8
		.BYTE	$00,$B7,$01,$00
		.BYTE	$80
_spr_shipB1:
		.BYTE	$F1,$C5,$41,$F7
		.BYTE	$F1,$C4,$41,$FF
		.BYTE	$F9,$C7,$41,$F7
		.BYTE	$F9,$C6,$41,$FF
		.BYTE	$01,$C9,$41,$F7
		.BYTE	$01,$C8,$41,$FF
		.BYTE	$80
_spr_shipB2:
		.BYTE	$F0,$B8,$41,$FA
		.BYTE	$F8,$BA,$41,$FA
		.BYTE	$F8,$B9,$41,$02
		.BYTE	$00,$BC,$41,$FA
		.BYTE	$00,$BB,$41,$02
		.BYTE	$80
_spr_shipB3:
		.BYTE	$F3,$BE,$41,$F7
		.BYTE	$F3,$BD,$41,$FF
		.BYTE	$FB,$C0,$41,$F7
		.BYTE	$FB,$BF,$41,$FF
		.BYTE	$03,$C3,$41,$F7
		.BYTE	$03,$C2,$41,$FF
		.BYTE	$03,$C1,$41,$07
		.BYTE	$80
_spr_shipB4:
		.BYTE	$F2,$AF,$01,$FF
		.BYTE	$FA,$B0,$01,$F8
		.BYTE	$FA,$B1,$01,$00
		.BYTE	$02,$B2,$01,$F8
		.BYTE	$02,$B3,$01,$00
		.BYTE	$80
_spr_shipB5:
;		.BYTE	$F3,$BF,$01,$FA		; BUG
		.BYTE	$F3,$BD,$01,$FA		; FIX
		.BYTE	$F3,$BE,$01,$02
		.BYTE	$FB,$BF,$01,$FA
		.BYTE	$FB,$C0,$01,$02
		.BYTE	$03,$C1,$01,$F2
		.BYTE	$03,$C2,$01,$FA
		.BYTE	$03,$C3,$01,$02
		.BYTE	$80
_spr_shipB6:
		.BYTE	$F0,$B8,$01,$FD
		.BYTE	$F8,$B9,$01,$F5
		.BYTE	$F8,$BA,$01,$FD
		.BYTE	$00,$BB,$01,$F5
;		.BYTE	$00,$BB,$01,$FD		; BUG
		.BYTE	$00,$BC,$01,$FD		; FIX
		.BYTE	$80
_spr_shipB7:
		.BYTE	$F1,$C4,$01,$F8
		.BYTE	$F1,$C5,$01,$00
		.BYTE	$F9,$C6,$01,$F8
		.BYTE	$F9,$C7,$01,$00
		.BYTE	$01,$C8,$01,$F8
		.BYTE	$01,$C9,$01,$00
		.BYTE	$80
_spr_planeA0:
		.BYTE	$F4,$A6,$03,$FC
		.BYTE	$FC,$A7,$03,$F4
		.BYTE	$FC,$A8,$03,$FC
		.BYTE	$FC,$A9,$03,$04
		.BYTE	$04,$AA,$03,$F4
		.BYTE	$04,$AB,$03,$FC
		.BYTE	$04,$AC,$03,$04
		.BYTE	$0C,$AD,$00,$FC
		.BYTE	$80
_spr_planeA1:
		.BYTE	$F8,$9D,$03,$F8
		.BYTE	$F8,$9E,$03,$00
		.BYTE	$F8,$9F,$03,$08
		.BYTE	$00,$A0,$03,$F8
		.BYTE	$00,$A1,$03,$00
		.BYTE	$00,$A2,$03,$08
		.BYTE	$08,$A3,$03,$F8
		.BYTE	$08,$A4,$03,$00
		.BYTE	$08,$A5,$03,$08
		.BYTE	$80
_spr_planeA2:
		.BYTE	$F4,$96,$43,$F4
		.BYTE	$F4,$95,$43,$FC
		.BYTE	$FC,$9A,$43,$EC
		.BYTE	$FC,$99,$43,$F4
		.BYTE	$FC,$98,$43,$FC
		.BYTE	$FC,$97,$43,$04
		.BYTE	$04,$9C,$43,$F4
		.BYTE	$04,$9B,$43,$FC
		.BYTE	$80
_spr_planeA3:
		.BYTE	$F0,$A3,$83,$F0
		.BYTE	$F0,$A4,$83,$F8
		.BYTE	$F0,$A5,$83,$00
		.BYTE	$F8,$A0,$83,$F0
		.BYTE	$F8,$A1,$83,$F8
		.BYTE	$F8,$A2,$83,$00
		.BYTE	$00,$9D,$83,$F0
		.BYTE	$00,$9E,$83,$F8
		.BYTE	$00,$9F,$83,$00
		.BYTE	$80
_spr_planeA4:
		.BYTE	$EC,$AD,$83,$FC
		.BYTE	$F4,$AA,$83,$F4
		.BYTE	$F4,$AB,$83,$FC
		.BYTE	$F4,$AC,$83,$04
		.BYTE	$FC,$A7,$83,$F4
		.BYTE	$FC,$A8,$83,$FC
		.BYTE	$FC,$A9,$83,$04
		.BYTE	$04,$A6,$83,$FC
		.BYTE	$80
_spr_planeA5:
		.BYTE	$F0,$A5,$C3,$F8
		.BYTE	$F0,$A4,$C3,$00
		.BYTE	$F0,$A3,$C3,$08
		.BYTE	$F8,$A2,$C3,$F8
		.BYTE	$F8,$A1,$C3,$00
		.BYTE	$F8,$A0,$C3,$08
		.BYTE	$00,$9F,$C3,$F8
		.BYTE	$00,$9E,$C3,$00
		.BYTE	$00,$9D,$C3,$08
		.BYTE	$80
_spr_planeA6:
		.BYTE	$F4,$95,$03,$FC
		.BYTE	$F4,$96,$03,$04
		.BYTE	$FC,$97,$03,$F4
		.BYTE	$FC,$98,$03,$FC
		.BYTE	$FC,$99,$03,$04
		.BYTE	$FC,$9A,$03,$0C
		.BYTE	$04,$9B,$03,$FC
		.BYTE	$04,$9C,$03,$04
		.BYTE	$80
_spr_planeA7:
		.BYTE	$F8,$9F,$43,$F8
		.BYTE	$F8,$9E,$43,$00
		.BYTE	$F8,$9D,$43,$08
		.BYTE	$00,$A2,$43,$F8
		.BYTE	$00,$A1,$43,$00
		.BYTE	$00,$A0,$43,$08
		.BYTE	$08,$A5,$43,$F8
		.BYTE	$08,$A4,$43,$00
		.BYTE	$08,$A3,$43,$08
		.BYTE	$80
_spr_planeB2:
		.BYTE	$F4,$8E,$03,$FC
		.BYTE	$F4,$8F,$03,$04
		.BYTE	$FC,$90,$03,$F4
		.BYTE	$FC,$91,$03,$FC
		.BYTE	$FC,$92,$03,$04
		.BYTE	$04,$93,$03,$FC
		.BYTE	$04,$94,$03,$04
		.BYTE	$80
_spr_planeB3:
		.BYTE	$F8,$8A,$03,$F8
		.BYTE	$F8,$8B,$03,$00
		.BYTE	$00,$8C,$03,$F8
		.BYTE	$00,$8D,$03,$00
		.BYTE	$80
_spr_planeB4:
		.BYTE	$F8,$88,$03,$FC
		.BYTE	$FF,$88,$83,$FC
		.BYTE	$FC,$89,$03,$04
		.BYTE	$80
_spr_planeC2:
		.BYTE	$F4,$8E,$43,$FC
		.BYTE	$F4,$8F,$43,$F4
		.BYTE	$FC,$90,$43,$04
		.BYTE	$FC,$91,$43,$FC
		.BYTE	$FC,$92,$43,$F4
		.BYTE	$04,$93,$43,$FC
		.BYTE	$04,$94,$43,$F4
		.BYTE	$80
_spr_planeC3:
		.BYTE	$F8,$8A,$43,$00
		.BYTE	$F8,$8B,$43,$F8
		.BYTE	$00,$8C,$43,$00
		.BYTE	$00,$8D,$43,$F8
		.BYTE	$80
_spr_planeC4:
		.BYTE	$F8,$88,$43,$FC
		.BYTE	$FF,$88,$C3,$FC
		.BYTE	$FC,$89,$43,$F4
		.BYTE	$80
_spr_tornado0:
		.BYTE	$E8,$4B,$01,$F5
		.BYTE	$E8,$4C,$01,$FD
		.BYTE	$E8,$4D,$01,$05
		.BYTE	$F0,$4E,$01,$F5
		.BYTE	$F0,$4F,$01,$FD
		.BYTE	$F0,$50,$01,$05
		.BYTE	$F8,$51,$01,$FD
		.BYTE	$80
_spr_tornado1:
		.BYTE	$E8,$52,$01,$F5
		.BYTE	$E8,$53,$01,$FD
		.BYTE	$E8,$54,$01,$05
		.BYTE	$F0,$55,$01,$F5
		.BYTE	$F0,$56,$01,$FD
		.BYTE	$F0,$57,$01,$05
		.BYTE	$F8,$58,$01,$FD
		.BYTE	$80
_spr_tornado2:
		.BYTE	$E8,$59,$01,$F5
		.BYTE	$E8,$5A,$01,$FD
		.BYTE	$E8,$5B,$01,$05
		.BYTE	$F0,$5C,$01,$F5
		.BYTE	$F0,$5D,$01,$FD
		.BYTE	$F0,$5E,$01,$05
		.BYTE	$F8,$5F,$01,$FD
		.BYTE	$80
_spr_tornado3:
		.BYTE	$E8,$4B,$01,$F5
		.BYTE	$E8,$4C,$01,$FD
		.BYTE	$E8,$4D,$01,$05
		.BYTE	$F0,$60,$01,$F5
		.BYTE	$F0,$61,$01,$FD
		.BYTE	$F0,$62,$01,$05
		.BYTE	$F8,$63,$01,$FD
		.BYTE	$80
_spr_tornado4:
		.BYTE	$E8,$52,$01,$F5
		.BYTE	$E8,$53,$01,$FD
		.BYTE	$E8,$54,$01,$05
		.BYTE	$F0,$64,$01,$F5
		.BYTE	$F0,$65,$01,$FD
		.BYTE	$F0,$66,$01,$05
		.BYTE	$F8,$67,$01,$FD
		.BYTE	$80
_spr_tornado5:
		.BYTE	$E8,$59,$01,$F5
		.BYTE	$E8,$5A,$01,$FD
		.BYTE	$E8,$5B,$01,$05
		.BYTE	$F0,$68,$01,$F5
		.BYTE	$F0,$69,$01,$FD
		.BYTE	$F0,$6A,$01,$05
		.BYTE	$F8,$6B,$01,$FD
		.BYTE	$80
_spr_tornado6:
		.BYTE	$E8,$4B,$01,$F5
		.BYTE	$E8,$4C,$01,$FD
		.BYTE	$E8,$4D,$01,$05
		.BYTE	$F0,$6C,$01,$F5
		.BYTE	$F0,$6D,$01,$FD
		.BYTE	$F0,$6E,$01,$05
		.BYTE	$F8,$6F,$01,$FD
		.BYTE	$80
_spr_tornado7:
		.BYTE	$E8,$52,$01,$F5
		.BYTE	$E8,$53,$01,$FD
		.BYTE	$E8,$54,$01,$05
		.BYTE	$F0,$70,$01,$F5
		.BYTE	$F0,$71,$01,$FD
		.BYTE	$F0,$72,$01,$05
		.BYTE	$F8,$73,$01,$FD
		.BYTE	$80
_spr_tornado8:
		.BYTE	$E8,$59,$01,$F5
		.BYTE	$E8,$5A,$01,$FD
		.BYTE	$E8,$5B,$01,$05
		.BYTE	$F0,$74,$01,$F5
		.BYTE	$F0,$75,$01,$FD
		.BYTE	$F0,$76,$01,$05
		.BYTE	$F8,$77,$01,$FD
		.BYTE	$80
_tornado_spr_anim_list:
		.BYTE	$00,$01,$02
		.BYTE	$00,$01,$02
		.BYTE	$00,$01,$02
		.BYTE	$03,$04,$05
		.BYTE	$03,$04,$05
		.BYTE	$03,$04,$05
		.BYTE	$06,$07,$08
		.BYTE	$06,$07,$08
		.BYTE	$06,$07,$08

_obj_monster_chr_anim_list0:
		.BYTE	$44,$45,$46,$47
_obj_monster_chr_anim_list1:
		.BYTE	$4D,$43,$4D,$43
_obj_monster_chr_anim_list2:
		.BYTE	$4D,$4D,$43,$43,$4D,$43,$4D,$43

_obj_monster_anim_lib0:
		.WORD	byte_89D71
		.WORD	byte_89D71
		.WORD	byte_89D71
		.WORD	byte_89D71
		.WORD	byte_89D93
		.WORD	byte_89D93
		.WORD	byte_89D93
		.WORD	byte_89D93
byte_89D71:
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$04
		.BYTE	$06,$00
byte_89D93:
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$04
		.BYTE	$00

_obj_monster_anim_lib1:
		.WORD	byte_89DC4
		.WORD	byte_89DE5
		.WORD	byte_89E06
		.WORD	byte_89E27
		.WORD	byte_89E48
		.WORD	byte_89E68
		.WORD	byte_89E88
		.WORD	byte_89EA8
byte_89DC4:
		.BYTE	$04,$05,$06,$07,$04,$05,$06,$07,$04,$05,$06,$07,$04,$05,$06,$07
		.BYTE	$04,$05,$06,$07,$04,$05,$06,$07,$04,$05,$06,$07,$04,$05,$06,$07
		.BYTE	$10
byte_89DE5:
		.BYTE	$0C,$0D,$0E,$0F,$0C,$0D,$0E,$0F,$0C,$0D,$0E,$0F,$0C,$0D,$0E,$0F
		.BYTE	$0C,$0D,$0E,$0F,$0C,$0D,$0E,$0F,$0C,$0D,$0E,$0F,$0C,$0D,$0E,$0F
		.BYTE	$11
byte_89E06:
		.BYTE	$08,$09,$0A,$0B,$08,$09,$0A,$0B,$08,$09,$0A,$0B,$08,$09,$0A,$0B
		.BYTE	$08,$09,$0A,$0B,$08,$09,$0A,$0B,$08,$09,$0A,$0B,$08,$09,$0A,$0B
		.BYTE	$12
byte_89E27:
		.BYTE	$00,$01,$02,$03,$00,$01,$02,$03,$00,$01,$02,$03,$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03,$00,$01,$02,$03,$00,$01,$02,$03,$00,$01,$02,$03
		.BYTE	$13
byte_89E48:
		.BYTE	$20,$21,$20,$21,$20,$21,$20,$21,$20,$21,$20,$21,$20,$21,$20,$21
		.BYTE	$20,$21,$20,$21,$20,$21,$20,$21,$20,$21,$20,$21,$20,$21,$20,$21
byte_89E68:
		.BYTE	$28,$29,$28,$29,$28,$29,$28,$29,$28,$29,$28,$29,$28,$29,$28,$29
		.BYTE	$28,$29,$28,$29,$28,$29,$28,$29,$28,$29,$28,$29,$28,$29,$28,$29
byte_89E88:
		.BYTE	$24,$25,$24,$25,$24,$25,$24,$25,$24,$25,$24,$25,$24,$25,$24,$25
		.BYTE	$24,$25,$24,$25,$24,$25,$24,$25,$24,$25,$24,$25,$24,$25,$24,$25
byte_89EA8:
		.BYTE	$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D
		.BYTE	$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D,$1C,$1D

_constr_building_sizes_list:
		.BYTE	$01,$01,$01,$01,$01,$02,$02,$02,$02,$02,$03,$03,$03,$03

_obj_spr_rotate_delta_table:
		.BYTE	$00,$FF,$FF,$FF,$FF,$01,$01,$01
		.BYTE	$01,$00,$FF,$FF,$FF,$FF,$01,$01
		.BYTE	$01,$01,$00,$FF,$FF,$FF,$FF,$01
		.BYTE	$01,$01,$01,$00,$FF,$FF,$FF,$FF
		.BYTE	$FF,$01,$01,$01,$00,$FF,$FF,$FF
		.BYTE	$FF,$FF,$01,$01,$01,$00,$FF,$FF
		.BYTE	$FF,$FF,$FF,$01,$01,$01,$00,$FF
		.BYTE	$FF,$FF,$FF,$FF,$01,$01,$01,$00

		SECTION_START F04

; =============== S U B	R O U T	I N E =======================================
_spr_game_interface_cursor_draw:
		LDA		_game_msg_wnd_active_flag
		BNE		loc_88CD1
		LDA		_cur_game_cursor_area
		BNE		loc_88CE0
loc_88CD1:
		LDA		#$00
		STA		_spr_insert_raw_args._idx
		MOVW	_spr_insert_raw_args._pos,_cur_game_cursor_scr_pos
		JMP		_spr_lib_raw_insert
loc_88CE0:
		LDX		_const_palette_selected_idx
		LDA		_constr_building_sizes_list,X
		CPX		#$0C
		BNE		loc_88CEC
		CLC
		ADC		_cur_port_type_select_idx
loc_88CEC:
		STA		_spr_insert_raw_args._idx
		LDA		_cur_game_cursor_scr_pos._COL
		AND		#$F8
		STA		_spr_insert_raw_args._pos._COL
		LDA		_cur_game_cursor_scr_pos._ROW
		AND		#$F8
		STA		_spr_insert_raw_args._pos._ROW
;		JMP		_spr_lib_raw_insert				; REDUNDANT

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
; Simpliest routine to add raw sprites without attr control and clipping
; uses different type of tmp on purpose, because it's called from NMI, and
; _tmp0 may be used in other code as well as args are in different vars
;
_spr_lib_raw_insert:
		LDX		_spr_insert_raw_args._idx
		LDA		_spr_lib_lo,X
		STA		_tmp15
		LDA		_spr_lib_hi,X
		STA		_tmp15+1
		DEC		_spr_insert_raw_args._pos._ROW
		LDX		_spr_buf_pos
		LDY		#$00
loc_88C9C:
		LDA		(_tmp15),Y
		CMP		#$80
		BEQ		loc_88CC5
		INY
		CLC
		ADC		_spr_insert_raw_args._pos._ROW
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_tmp15),Y
		INY
		CLC
		ADC		_spr_insert_raw_args._pos._COL
		STA		_spr_buf,X
		INX
		JMP		loc_88C9C
loc_88CC5:
		STX		_spr_buf_pos
		RTS

		SECTION_END F04

		BANK_END F5,$A000
