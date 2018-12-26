
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
		LDA		_obj_pos._heli._X				; position currently, they CRAAASHHHHH!!
		CMP		_obj_pos._plane._X				; so we can crash airplane without request
		BNE		locret_88076					; NOTE, with this crash we destroy only plane, not heli!
		LDA		_obj_pos._heli._Y
		CMP		_obj_pos._plane._Y
		BNE		locret_88076
;		JMP		_airplane_crash_start			; OPTIMIZED
_airplane_crash_start:
		LDA		#$22
		STA		_game_msg_idx
		LDX		_obj_pos._plane._X				; cur plane obj pos set to goto pos
		STX		_goto_dst_tile._X				; the same time X,Y loaded with the same pos
		LDY		_obj_pos._plane._Y
;		STX		_goto_dst_tile._Y				; BUG! Y loaded, X stored instead!
		STY		_goto_dst_tile._Y				; FIX: correct opcode
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
		LDA		_obj_pos._plane._X
		CMP		_last_airport_pos._X
		BNE		locret_88202
		LDA		_obj_pos._plane._Y
		CMP		_last_airport_pos._Y
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
		STA		_obj_dst_pos._X,Y
		JSR		_obj_flying_get_rand_pos
		STA		_obj_dst_pos._Y,Y
		JSR		_obj_fetch
		JSR		_obj_flying_dst_test_adjust		; check if we already there
		CMP		#$FF							; repeat if yeas
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
		LDA		_cur_obj_pos._X
		CMP		_cur_dst_pos._X
		BEQ		loc_88B97
		BCS		loc_88BA7
		LDA		_cur_obj_pos._Y
		CMP		_cur_dst_pos._Y
		BEQ		loc_88B91
		BCC		loc_88B94
		LDA		#$01
		RTS
loc_88B91:
		LDA		#$02
		RTS
loc_88B94:
		LDA		#$03
		RTS
loc_88B97:
		LDA		_cur_obj_pos._Y
		CMP		_cur_dst_pos._Y
		BEQ		loc_88BBA
		BCC		loc_88BA4
		LDA		#$00
		RTS
loc_88BA4:
		LDA		#$04
		RTS
loc_88BA7:
		LDA		_cur_obj_pos._Y
		CMP		_cur_dst_pos._Y
		BEQ		loc_88BB4
		BCS		loc_88BB7
		LDA		#$05
		RTS
loc_88BB4:
		LDA		#$06
		RTS
loc_88BB7:
		LDA		#$07
		RTS
loc_88BBA:
		CPY		#$03
		BNE		loc_88BD9
		LDA		_obj_cur_spr_idx._train
		AND		#$02
		BEQ		loc_88BCE
		LDA		_obj_shift._train._X
		CMP		#$04
		BEQ		loc_88BD9
		BEQ		loc_88BD5
loc_88BCE:
		LDA		_obj_shift._train._Y
		CMP		#$04
		BEQ		loc_88BD9
loc_88BD5:
		LDA		_obj_cur_spr_idx._train
		RTS
loc_88BD9:
		LDA		#$FF
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
; BAD CODE: similar function to train object
; as against the train, ship can move diagonally
; but here we test if he moves always diagonally lol
; so if ship moving vertically or horizontally,
; we never arrive to the dest
; -
;		LDA		_obj_shift._ship._X
;		CMP		#$04
;		BNE		loc_8828E
;		LDA		_obj_shift._ship._Y
;		CMP		#$04
;		BNE		loc_8828E
; -
; FIX: some possible logic flaw still left. there is no
; test here for virtually possible not moving at all state
; but actually it handled with rotate routine
; -
		LDX		_obj_shift._ship._X
		BEQ		._ship_no_x_move
		LDA		_obj_shift._ship._X
		CMP		#$04
		BNE		loc_8828E
._ship_no_x_move:
		LDX		_obj_shift._ship._Y
		BEQ		._ship_no_y_move
		LDA		_obj_shift._ship._Y
		CMP		#$04
		BNE		loc_8828E
._ship_no_y_move:
; -
		LDA		_cur_obj_pos._X
		CMP		_obj_dst_pos._ship._X
		BNE		loc_8828E
		LDA		_cur_obj_pos._Y
		CMP		_obj_dst_pos._ship._Y
		BNE		loc_8828E
		LDA		#$FF
		RTS
loc_8828E:
;		LDA		_obj_cur_spr_idx._ship			; REDUNDANT, not used here, like for train
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_ship_retarget:
		JSR		_get_cur_obj_map_ptr			; _tmp15/_tmp15+1 = map_buf_pos ofs
;		LDA		#$00							; REDUNDANT, seems unused here
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
		LDA		_ptr18
		CLC
		ADC		_obj_spr_delta,Y
		AND		#$07
		STA		_obj_cur_spr_idx._ship			; do first roration
		PLA										; restore new direction
.ship_course_same:
		ASL										; set new target 1 tile ahead
		TAX										; in a desired direction
		LDA		_cur_obj_idx
		ASL
		TAY
		LDA		_obj_dst_pos._X,Y
		CLC
		ADC		_obj_dir_delta_list,X
		STA		_obj_dst_pos._X,Y
		LDA		_obj_dst_pos._Y,Y
		CLC
		ADC		_obj_dir_delta_list+1,X
		STA		_obj_dst_pos._Y,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_ship_look_for_new_dir:
; -
; NEW: let's make ship more interesting
; WARNING: this will change rand_seed as well!
; -
		JSR		_rand							; decide to change direct course
		CMP		#$D0							; probability of course change
		BCS		.nope
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
.nope:
; -
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
		BNE		_obj_ship_look_for_new_dir
		LDX		_ptr18							; default value - opposite one
		RTS
_obj_map_tile_dir_buf_ofs_list:
		.BYTE	$B4,$B5,$01,$4D,$4C,$4B,$FF,$B3

; =============== S U B	R O U T	I N E =======================================
_obj_ship_max_range_test:
		LDA		_cur_obj_pos._X					; here we should decide if we can
		BEQ		.ship_leftmost_pos				; go in the selected direction
		CMP		#$4B
		BEQ		.ship_rightmost_pos
		LDA		_cur_obj_pos._Y
		BEQ		.ship_top_pos
		CMP		#$4B
		BEQ		.ship_bottom_pos
.ship_can_go:
		SEC
		RTS
.ship_cant_go:
;		INX										; here we skip to the next direction one more time
;		TXA										; don't know why, actually. looks loke dupe code here
;		AND		#$07
;		TAX
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
		LDX		_obj_shift._train._X
		LDA		_obj_cur_spr_idx._train			; select movement direction
		AND		#$02							; may be only vert or hor
		BNE		loc_88357
		LDX		_obj_shift._train._Y
loc_88357:
		CPX		#$04
		BNE		locret_8836D					; if timers are max
		LDA		_cur_obj_pos._X					; test if we in dest point
		CMP		_obj_dst_pos._train._X
		BNE		locret_8836D
		LDA		_cur_obj_pos._Y
		CMP		_obj_dst_pos._train._Y
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
		LDA		(_tmp15),Y
loc_88386:
		CMP		_rail_tiles_allowed,X			; search for cur allowed tile
		BEQ		loc_883A1
		INX
		CPX		#$11
		BCC		loc_88386						; X has its index now in arrays
		LDA		byte_571						; no available tiles, special bounce check
		BNE		.building_bounce
		JMP		.remove_train
.building_bounce:
		LDA		_obj_cur_spr_idx._train
		AND		#$02
		BEQ		.building_bounce_vert
		BNE		.building_bounce_hor
loc_883A1:
		CPX		#$04
		BCS		loc_883AA
.building_bounce_hor:
		LDY		#$00
		JMP		loc_88402
loc_883AA:
		CPX		#$08
		BCS		loc_883B3
.building_bounce_vert:
		LDY		#$01
		JMP		loc_88402
loc_883B3:
		CPX		#$0C
		BCS		loc_883BA
		JMP		loc_88451
loc_883BA:
		CPX		#$10
		BCS		loc_883C1
		JMP		loc_884A3
loc_883C1:
		INC		byte_572
		LDA		_obj_cur_spr_idx._train
		ASL
		STA		_tmp118
		LDA		byte_572
		AND		#$03
		CLC
		ADC		_tmp118
		TAX
		LDA		byte_883E1,X
		BEQ		loc_883DE
		TAX
		JMP		loc_88451
loc_883DE:
		JMP		.train_dest_pos_load
byte_883E1:
		.BYTE	$00,$08,$00,$09,$00,$09,$00,$0B,$00,$0A,$00,$0B,$00,$08,$00,$0A

_rail_tiles_allowed:
		.BYTE	$9C,$E2,$BB,$B7
		.BYTE	$9D,$E3,$BC,$B8
		.BYTE	$9E,$9F,$A0,$A1
		.BYTE	$A2,$A3,$A4,$A5
		.BYTE	$A6

loc_88402:
		LDA		_cur_obj_pos,Y
		BEQ		loc_8840D
		CMP		#$4B
		BNE		loc_88425
		INY
		INY
loc_8840D:
		LDX		_obj_cur_spr_idx._train
		TXA
		CMP		byte_8844D,Y
		BEQ		loc_8841E
		LDX		_ptr18
		STX		_obj_cur_spr_idx._train
		STX		_obj_dst_spr_idx._train
loc_8841E:
		JSR		_obj_train_near_tile_test
		BCC		.remove_train
		BCS		loc_8844A
loc_88425:
		LDX		_obj_cur_spr_idx._train
		JSR		_obj_train_near_tile_test
		BCS		loc_8844A
		LDX		_ptr18
		JSR		_obj_train_near_tile_test
		BCS		loc_88442
.remove_train:
		LDA		_obj_active_flags
		AND		#$F7
		STA		_obj_active_flags
		LDA		#$00
		STA		byte_571
		RTS
loc_88442:
		LDA		_ptr18
		STA		_obj_cur_spr_idx._train
		STA		_obj_dst_spr_idx._train
loc_8844A:
		JMP		.train_dest_pos_load

byte_8844D:
		.BYTE	$02,$04,$06,$00

loc_88451:
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
		LDA		byte_88493,X
		STA		_obj_dst_spr_idx._train
		LDA		byte_88493+1,X
		STA		_obj_spr_delta._train
		LDX		_obj_shift._train._X
		LDA		_obj_shift._train._Y
		STA		_obj_shift._train._X
		STX		_obj_shift._train._Y
.train_dest_pos_load:
		LDA		_obj_dst_spr_idx._train
		ASL
		TAX
		LDA		_obj_dst_pos._train._X
		CLC
		ADC		_obj_dir_delta_list,X
		STA		_obj_dst_pos._train._X
		LDA		_obj_dst_pos._train._Y
		CLC
		ADC		_obj_dir_delta_list+1,X
		STA		_obj_dst_pos._train._Y
		RTS

byte_88493:
		.BYTE	$04,$FF
		.BYTE	$02,$01
		.BYTE	$04,$01
		.BYTE	$06,$FF
		.BYTE	$00,$01
		.BYTE	$02,$FF
		.BYTE	$00,$FF
		.BYTE	$06,$01

loc_884A3:
		INC		byte_572
		TXA
		SEC
		SBC		#$0C
		ASL
		ASL
		ASL
		CLC
		ADC		_obj_cur_spr_idx._train
		TAX
		LDA		byte_572
		AND		#$01
		BEQ		loc_884BA
		INX
loc_884BA:
		LDA		byte_884C6,X
		BEQ		loc_884C3
		TAX
		JMP		loc_88451
loc_884C3:
		JMP		.train_dest_pos_load

byte_884C6:
		.BYTE	$00,$08,$00,$00,$00,$0A,$08,$0A,$00,$09,$09,$0B,$00,$0B,$00,$00
		.BYTE	$08,$09,$00,$09,$00,$00,$00,$08,$00,$00,$00,$0B,$0A,$0B,$00,$0A

; =============== S U B	R O U T	I N E =======================================
; most of the following code is for determine if we run into the bus station
; or not. this is not trivial task because we won't know for sure wether it
; bus station or not, if just bounce into special building tile (80-8E) so we need
; here carefully calc the side you run into the building, then calc position of building
; index tile, read it and CHECK!
;
_obj_train_near_tile_test:
		LDY		_obj_map_tile_dir_buf_ofs_list,X
		BPL		loc_884F3
		DEC		_tmp15+1
		LDA		(_tmp15),Y
		INC		_tmp15+1
		BNE		loc_884F5
loc_884F3:
		LDA		(_tmp15),Y
loc_884F5:
		CMP		#$7D								; lucky! run into bus station already!
		BNE		loc_884FC
		JMP		.bus_station_ride
loc_884FC:
		CMP		#$80
		BCC		.stop_move
		CMP		#$88
		BCC		.bus_station_ride_test
		CMP		#$9C
		BCC		.stop_move
		CMP		#$A7
		BCC		.continue_move
		CMP		#$B7
		BCC		.stop_move
		CMP		#$B9
		BCC		.continue_move
		CMP		#$BB
		BCC		.stop_move
		CMP		#$BD
		BCC		.continue_move
		CMP		#$E2
		BCC		.stop_move
		CMP		#$E4
		BCS		.stop_move
		LDA		#$03
		BNE		.water_rails
.continue_move:
		LDA		#$00
.water_rails:
		STA		byte_571
		SEC
		RTS
.stop_move:
		CLC
		RTS
.bus_station_ride_test:
		LDA		byte_571
		BNE		loc_8858A
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
		SBC		byte_8858C,X
		STA		_ptr18
		LDA		_tmp15+1
		SBC		#$00
		STA		_ptr18+1
		INC		_tmp15+1
		LDA		(_ptr18),Y
		JMP		loc_88570
loc_8855B:
		LDA		(_tmp15),Y
		AND		#$0F
		TAX
		LDA		_tmp15
		SEC
		SBC		byte_8858C,X
		STA		_ptr18
		LDA		_tmp15+1
		SBC		#$00
		STA		_ptr18+1
		LDA		(_ptr18),Y
loc_88570:
		STA		_ptr18+1
		PLA
		TAX
		POPB	_ptr18
		LDA		_ptr18+1
		CMP		#$7D
		BEQ		.bus_station_ride
		CLC
		RTS
.bus_station_ride:
		LDA		_obj_cur_spr_idx._train
		AND		#$02
		LSR
		ADC		#$01
		STA		byte_571
loc_8858A:
		SEC
		RTS

byte_8858C:
		.BYTE	$4C,$4D,$01,$98,$99,$9A,$4E,$02

; =============== S U B	R O U T	I N E =======================================
; WARNING: DUMB DELAY HERE!
;
; also
;
; should've change var0 at some timer basis, but cmp test performed
; aganst cur_obj_idx, not monster_var0, which seems should instead
; probably makes difference on monster anim
;
_obj_hndl10_monster:
		LDX		#$00
loc_88596:
		DEX
		BNE		loc_88596
		LDA		#$04
		STA		_cur_obj_idx
		BIT		_obj_monster_anim_phase1
		BMI		loc_885A9
		JSR		_obj_monster_anim0
		JMP		locret_885D8
loc_885A9:
		JSR		_obj_fetch
		LDA		_frames_div8_counter
		CMP		_cur_obj_idx
		BNE		loc_885D5
		DEC		_obj_monster_delay
		BEQ		loc_885C3
		CMP		#$07
		BCC		loc_885C6
		LDA		#$02
		STA		_obj_monster_delay
		BNE		loc_885C6
loc_885C3:
		JSR		_obj_monster_anim1
loc_885C6:
		LDA		_obj_monster_anim_phase0
		CMP		#$20
		BEQ		loc_885D5
		JSR		_obj_monster_move
		BCS		locret_885D8
		JSR		_obj_fetch
loc_885D5:
		JMP		_obj_draw
locret_885D8:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_monster_chr01_select:
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
		LDA		byte_6CA
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
_obj_monster_chr2_select:
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
_obj_monster_move:
		LDA		_obj_monster_anim_phase1
		AND		#$01
		TAY
		LDA		_obj_monster_chr_idx
		AND		byte_8873F,Y
		BNE		loc_886ED
		LDA		_obj_shift._monster,Y
		CLC
		ADC		#$01
		AND		#$07
		STA		_obj_shift._monster,Y
		BNE		loc_8872E
		LDA		_obj_pos._monster,Y
		CLC
		ADC		#$01
		STA		_obj_pos._monster,Y
		JMP		loc_88705
loc_886ED:
		LDA		_obj_shift._monster,Y
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
		BNE		loc_8872E
		TYA
		EOR		#$01
		TAY
		LDA		_obj_pos._monster,Y
		CMP		_obj_dst_pos._monster,Y
		BEQ		loc_88730
		LDA		#$00
		STA		_obj_monster_delay
		LDA		#$1F
		STA		_obj_monster_anim_phase0
		LDA		_obj_monster_anim_phase1
		EOR		#$01
		STA		_obj_monster_anim_phase1
		JSR		_obj_monster_anim1
loc_8872E:
		CLC
		RTS
loc_88730:
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
_obj_monster_anim0:
		LDA		#$20
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
		STA		_obj_monster_anim_phase1
		LDA		_obj_dst_pos._monster._X
		CMP		_obj_pos._monster._X
		BCS		loc_8876A
		LDA		_obj_monster_chr_idx
		ORA		#$01
		STA		_obj_monster_chr_idx
loc_8876A:
		LDA		_obj_dst_pos._monster._Y
		CMP		_obj_pos._monster._Y
		BCS		locret_8877A
		LDA		_obj_monster_chr_idx
		ORA		#$30
		STA		_obj_monster_chr_idx
locret_8877A:
		RTS

;_unused_0:
;		.BYTE	$02,$04,$02,$00,$06,$04,$06,$00

; =============== S U B	R O U T	I N E =======================================
_obj_monster_anim1:
		INC		_obj_monster_anim_phase0
loc_88633:
		JSR		_obj_monster_chr01_select
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
		STA		_tmp17
		LDY		byte_6CA
		BEQ		loc_88657
		CLC
		ADC		#$08
		STA		_tmp17
loc_88657:
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
		LDA		byte_6CA
		BNE		loc_88633
		JSR		_obj_monster_chr2_select
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
		LDA		#$4C
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
		LDX		_cur_obj_pos._X
		LDY		_cur_obj_pos._Y
		JSR		_far_fire_or_destroy_map_tile_ex
loc_887B6:
		JMP		_obj_draw
locret_887B9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_tornado_move:
		LDX		#$00
		STX		_tmp15
		LDA		_cur_obj_pos._X
		CMP		_cur_dst_pos._X
		BEQ		loc_887D8
		BCC		loc_887CF
		LDX		#$FF
		SBC		_cur_dst_pos._X
		BCS		loc_887D8
loc_887CF:
		LDX		#$01
		LDA		_cur_dst_pos._X
		SEC
		SBC		_cur_obj_pos._X
loc_887D8:
		STA		_tmp15
		LDY		#$00
		LDA		_cur_obj_pos._Y
		CMP		_cur_dst_pos._Y
		BEQ		loc_88834
		BCC		loc_887F1
		SBC		_cur_dst_pos._Y
		CMP		_tmp15
		BCC		loc_88806
		LDY		#$FF
		BMI		loc_887FE
loc_887F1:
		LDA		_cur_dst_pos._Y
		SEC
		SBC		_cur_obj_pos._Y
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
_obj_draw:
		LDX		_game_wnd_spr_mode_flag
		BEQ		loc_88110
		JSR		_map_scr_obj_spr_insert
		JMP		_obj_flush
loc_88110:
		LDY		_cur_obj_idx
		BEQ		.obj_heli_draw
		CPY		#$02
		BEQ		.obj_ship_draw
		CPY		#$04
		BEQ		.obj_monster_draw
		CPY		#$05
		BEQ		.obj_tornado_draw
.obj_common_draw:
		LDA		_obj_cur_spr_idx,Y
.obj_common_draw_ex:
		STA		_spr_insert_args._idx
		JSR		_obj_spr_draw
		JMP		_obj_flush
.obj_heli_draw:
		LDA		_obj_cur_spr_idx._heli
		LDX		_frames_div4_counter
		JSR		_obj_spr_heli_draw
		JMP		_obj_flush
.obj_monster_draw:
		LDA		_obj_monster_spr_idx
		STA		_spr_insert_args._idx
		JSR		_obj_spr_draw
		LDX		_cur_obj_pos._X
		LDY		_cur_obj_pos._Y
		JMP		_far_fire_or_destroy_map_tile_ex
.obj_ship_draw:
		CMPWI	_city._cur_year,1950			; for years 1900-1949 used _spr_shipB set
		BCS		.obj_common_draw				; the rest sprites uses _spr_shipA set
		LDA		_obj_cur_spr_idx,Y
		CLC
		ADC		#$08
		BNE		.obj_common_draw_ex
.obj_tornado_draw:
		LDX		_obj_tornado_counter
		LDA		byte_89D36,X
		JMP		.obj_common_draw_ex

; =============== S U B	R O U T	I N E =======================================
_obj_spr_heli_draw:
		ASL										; fetch every 4th sprite idx
		ASL
		STA		_spr_insert_args._idx			; set +1 idx every 1/8 frames
		TXA
		LSR
;		AND		#$01							; BUG? two frames are unused with that
		AND		#$03							; FIX: use all 4
		CLC
		ADC		_spr_insert_args._idx
		STA		_spr_insert_args._idx

; =============== S U B	R O U T	I N E =======================================
_obj_spr_draw:
		LDA		_cur_obj_pos._X
		SEC
		SBC		_cur_map_wnd_pos._X
		BCS		loc_889F7
		CMP		#$FD
		BCS		loc_889FB
		BCC		locret_88A45
loc_889F7:
		CMP		#$1B
		BCS		locret_88A45
loc_889FB:
		LDA		_cur_obj_pos._Y
		SEC
		SBC		_cur_map_wnd_pos._Y
		BCS		loc_88A09
		CMP		#$FD
		BCS		loc_88A0D
		BCC		locret_88A45
loc_88A09:
		CMP		#$18
		BCS		locret_88A45
loc_88A0D:
		LDA		_cur_obj_idx
		ASL
		TAX
		LDA		_cur_obj_pos._X
		SEC
		SBC		_cur_map_wnd_pos._X
		CLC
		ADC		#$07
		ASL
		ASL
		ASL
		CLC
		ADC		_obj_shift,X
		STA		_spr_insert_args._pos._X
		LDA		_cur_obj_pos._Y
		SEC
		SBC		_cur_map_wnd_pos._Y
		CLC
		ADC		#$07
		ASL
		ASL
		ASL
		CLC
		ADC		_obj_shift._Y,X
		STA		_spr_insert_args._pos._Y
		LDA		_cur_obj_idx
		CMP		#$03
		BNE		loc_88A42
		LDA		byte_571
		BNE		locret_88A45
loc_88A42:
		JSR		_obj_spr_clip_draw
locret_88A45:
		RTS

; NOTE: clip value never used in sprite drawing routines,
; several sprite drawing routines has its own hardwritten
; clipping code and no one uses the common one.
; the _spr_insert_args._clip only zeroed at some point
; never change it's value
;
; =============== S U B	R O U T	I N E =======================================
;_obj_clip_select_unused:
;		TAX
;		LDA		#$00
;		STA		_spr_insert_args._clip
;		LDA		_spr_insert_args._pos._X
;		CLC
;		ADC		byte_89D36,X
;		CMP		#$38
;		BCS		loc_88A5B
;		LDA		_spr_insert_args._clip
;		ORA		#$01
;		STA		_spr_insert_args._clip
;loc_88A5B:
;		LDA		_spr_insert_args._pos._Y
;		CLC
;		ADC		byte_89D36,X
;		CMP		#$38
;		BCS		loc_88A6D
;		LDA		_spr_insert_args._clip
;		ORA		#$02
;		STA		_spr_insert_args._clip
;		BNE		locret_88A7D
;loc_88A6D:
;		LDA		_spr_insert_args._pos._Y
;		CLC
;		ADC		byte_89D36,X
;		CMP		#$D9
;		BCC		locret_88A7D
;		LDA		_spr_insert_args._clip
;		ORA		#$04
;		STA		_spr_insert_args._clip
;locret_88A7D:
;		RTS

; NOTE: clipping embedded in this routine, no need in additional
; clipping function or variable.
;
; =============== S U B	R O U T	I N E =======================================
_obj_spr_clip_draw:
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
		DEC		_spr_insert_args._pos._Y
		LDX		_spr_buf_pos
		LDY		#$00
loc_88AAC:
		STX		_tmp17
		LDA		(_tmp15),Y
		CMP		#$80
		BEQ		loc_88B09
		INY
		CLC
		ADC		_spr_insert_args._pos._Y
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
		ADC		_spr_insert_args._pos._X
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
		ADC		_spr_insert_args._pos._X
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
		ADC		_spr_insert_args._pos._Y
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
		ADC		_spr_insert_args._pos._X
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
		LDA		_obj_pos._Y,X
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
		LDA		_obj_pos._X,X
		STA		_cur_obj_pos._X
		LDA		_obj_pos._Y,X
		STA		_cur_obj_pos._Y
		LDA		_obj_dst_pos._X,X
		STA		_cur_dst_pos._X
		LDA		_obj_dst_pos._Y,X
		STA		_cur_dst_pos._Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_obj_flush:
		LDA		_cur_obj_idx
		ASL
		TAX
		LDA		_cur_obj_pos._X
		STA		_obj_pos._X,X
		LDA		_cur_obj_pos._Y
		STA		_obj_pos._Y,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_cur_obj_map_ptr:
;		LDY		_cur_obj_pos._Y
;		LDA		off_888F5,Y
;		CLC
;		ADC		_cur_obj_pos._X
;		STA		_tmp15
;		LDA		off_88941,Y
;		ADC		#$00
;		STA		_tmp15+1
; FIX: WRANING, make sure wram_map_buf is at the beginning of the page, instead
; use full 16-bit addition
;
		BUFPOS	_tmp15,_cur_obj_pos._X,_cur_obj_pos._Y,#$4C,#>[_wram_map_buf]
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
		.WORD	byte_898D7
		.WORD	byte_898E0
		.WORD	byte_898F1
		.WORD	byte_898FA
		.WORD	byte_898D7
		.WORD	byte_898E0
		.WORD	byte_898F1
		.WORD	byte_898FA
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
		.WORD	byte_89AA3
		.WORD	byte_89AC4
		.WORD	byte_89AE9
		.WORD	byte_89B0A
		.WORD	byte_89B2F
		.WORD	byte_89B50
		.WORD	byte_89B75
		.WORD	byte_89B96
		.WORD	byte_89B75
		.WORD	byte_89BBB
		.WORD	byte_89BD8
		.WORD	byte_89BE9
		.WORD	byte_89AE9
		.WORD	byte_89BF6
		.WORD	byte_89C13
		.WORD	byte_89C24
_obj5_spr_lib:
		.WORD	byte_89C31
		.WORD	byte_89C4E
		.WORD	byte_89C6B
		.WORD	byte_89C88
		.WORD	byte_89CA5
		.WORD	byte_89CC2
		.WORD	byte_89CDF
		.WORD	byte_89CFC
		.WORD	byte_89D19
_obj4_spr_lib:
		.WORD	byte_88FC3
		.WORD	byte_89010
		.WORD	byte_89059
		.WORD	byte_890A2
		.WORD	byte_89359
		.WORD	byte_893A2
		.WORD	byte_893E7
		.WORD	byte_89430
		.WORD	byte_89259
		.WORD	byte_89296
		.WORD	byte_892D7
		.WORD	byte_89318
		.WORD	byte_890EB
		.WORD	byte_89128
		.WORD	byte_89165
		.WORD	byte_891A6
		.WORD	byte_8962E
		.WORD	byte_891E7
		.WORD	byte_895FD
		.WORD	byte_89218
		.WORD	byte_894A2
		.WORD	byte_89479
		.WORD	byte_88E70
		.WORD	byte_88E3B
		.WORD	byte_89514
		.WORD	byte_88EE2
		.WORD	byte_894D7
		.WORD	byte_88EA5
		.WORD	byte_88F5D
		.WORD	byte_88F92
		.WORD	byte_88F5D
		.WORD	byte_88F92
		.WORD	byte_89549
		.WORD	byte_8957E
		.WORD	byte_89549
		.WORD	byte_8957E
		.WORD	byte_895A7
		.WORD	byte_895D4
		.WORD	byte_895A7
		.WORD	byte_895D4
		.WORD	byte_88F0B
		.WORD	byte_88F38
		.WORD	byte_88F0B
		.WORD	byte_88F38
byte_88E3B:
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
byte_88E70:
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
byte_88EA5:
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
byte_88EE2:
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
byte_88F0B:
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
byte_88F38:
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
byte_88F5D:
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
byte_88F92:
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
byte_88FC3:
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
byte_89010:
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
byte_89059:
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
byte_890A2:
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
byte_890EB:
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
byte_89128:
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
byte_89165:
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
byte_891A6:
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
byte_891E7:
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
byte_89218:
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
byte_89259:
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
byte_89296:
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
byte_892D7:
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
byte_89318:
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
byte_89359:
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
byte_893A2:
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
byte_893E7:
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
byte_89430:
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
byte_89479:
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
byte_894A2:
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
byte_894D7:
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
byte_89514:
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
byte_89549:
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
byte_8957E:
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
byte_895A7:
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
byte_895D4:
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
byte_895FD:
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
byte_8962E:
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
byte_898D7:
		.BYTE	$F7,$82,$01,$02
		.BYTE	$FF,$83,$01,$02
		.BYTE	$80
byte_898E0:
		.BYTE	$F8,$85,$41,$FB
		.BYTE	$F8,$84,$41,$03
		.BYTE	$00,$87,$41,$FB
		.BYTE	$00,$86,$41,$03
		.BYTE	$80
byte_898F1:
		.BYTE	$FD,$80,$01,$FA
		.BYTE	$FD,$81,$01,$02
		.BYTE	$80
byte_898FA:
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
		.BYTE	$F6,$BF,$41,$FF		; BUG
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
byte_89AA3:
		.BYTE	$F4,$A6,$03,$FC
		.BYTE	$FC,$A7,$03,$F4
		.BYTE	$FC,$A8,$03,$FC
		.BYTE	$FC,$A9,$03,$04
		.BYTE	$04,$AA,$03,$F4
		.BYTE	$04,$AB,$03,$FC
		.BYTE	$04,$AC,$03,$04
		.BYTE	$0C,$AD,$00,$FC
		.BYTE	$80
byte_89AC4:
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
byte_89AE9:
		.BYTE	$F4,$96,$43,$F4
		.BYTE	$F4,$95,$43,$FC
		.BYTE	$FC,$9A,$43,$EC
		.BYTE	$FC,$99,$43,$F4
		.BYTE	$FC,$98,$43,$FC
		.BYTE	$FC,$97,$43,$04
		.BYTE	$04,$9C,$43,$F4
		.BYTE	$04,$9B,$43,$FC
		.BYTE	$80
byte_89B0A:
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
byte_89B2F:
		.BYTE	$EC,$AD,$83,$FC
		.BYTE	$F4,$AA,$83,$F4
		.BYTE	$F4,$AB,$83,$FC
		.BYTE	$F4,$AC,$83,$04
		.BYTE	$FC,$A7,$83,$F4
		.BYTE	$FC,$A8,$83,$FC
		.BYTE	$FC,$A9,$83,$04
		.BYTE	$04,$A6,$83,$FC
		.BYTE	$80
byte_89B50:
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
byte_89B75:
		.BYTE	$F4,$95,$03,$FC
		.BYTE	$F4,$96,$03,$04
		.BYTE	$FC,$97,$03,$F4
		.BYTE	$FC,$98,$03,$FC
		.BYTE	$FC,$99,$03,$04
		.BYTE	$FC,$9A,$03,$0C
		.BYTE	$04,$9B,$03,$FC
		.BYTE	$04,$9C,$03,$04
		.BYTE	$80
byte_89B96:
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
byte_89BBB:
		.BYTE	$F4,$8E,$03,$FC
		.BYTE	$F4,$8F,$03,$04
		.BYTE	$FC,$90,$03,$F4
		.BYTE	$FC,$91,$03,$FC
		.BYTE	$FC,$92,$03,$04
		.BYTE	$04,$93,$03,$FC
		.BYTE	$04,$94,$03,$04
		.BYTE	$80
byte_89BD8:
		.BYTE	$F8,$8A,$03,$F8
		.BYTE	$F8,$8B,$03,$00
		.BYTE	$00,$8C,$03,$F8
		.BYTE	$00,$8D,$03,$00
		.BYTE	$80
byte_89BE9:
		.BYTE	$F8,$88,$03,$FC
		.BYTE	$FF,$88,$83,$FC
		.BYTE	$FC,$89,$03,$04
		.BYTE	$80
byte_89BF6:
		.BYTE	$F4,$8E,$43,$FC
		.BYTE	$F4,$8F,$43,$F4
		.BYTE	$FC,$90,$43,$04
		.BYTE	$FC,$91,$43,$FC
		.BYTE	$FC,$92,$43,$F4
		.BYTE	$04,$93,$43,$FC
		.BYTE	$04,$94,$43,$F4
		.BYTE	$80
byte_89C13:
		.BYTE	$F8,$8A,$43,$00
		.BYTE	$F8,$8B,$43,$F8
		.BYTE	$00,$8C,$43,$00
		.BYTE	$00,$8D,$43,$F8
		.BYTE	$80
byte_89C24:
		.BYTE	$F8,$88,$43,$FC
		.BYTE	$FF,$88,$C3,$FC
		.BYTE	$FC,$89,$43,$F4
		.BYTE	$80
byte_89C31:
		.BYTE	$E8,$4B,$01,$F5
		.BYTE	$E8,$4C,$01,$FD
		.BYTE	$E8,$4D,$01,$05
		.BYTE	$F0,$4E,$01,$F5
		.BYTE	$F0,$4F,$01,$FD
		.BYTE	$F0,$50,$01,$05
		.BYTE	$F8,$51,$01,$FD
		.BYTE	$80
byte_89C4E:
		.BYTE	$E8,$52,$01,$F5
		.BYTE	$E8,$53,$01,$FD
		.BYTE	$E8,$54,$01,$05
		.BYTE	$F0,$55,$01,$F5
		.BYTE	$F0,$56,$01,$FD
		.BYTE	$F0,$57,$01,$05
		.BYTE	$F8,$58,$01,$FD
		.BYTE	$80
byte_89C6B:
		.BYTE	$E8,$59,$01,$F5
		.BYTE	$E8,$5A,$01,$FD
		.BYTE	$E8,$5B,$01,$05
		.BYTE	$F0,$5C,$01,$F5
		.BYTE	$F0,$5D,$01,$FD
		.BYTE	$F0,$5E,$01,$05
		.BYTE	$F8,$5F,$01,$FD
		.BYTE	$80
byte_89C88:
		.BYTE	$E8,$4B,$01,$F5
		.BYTE	$E8,$4C,$01,$FD
		.BYTE	$E8,$4D,$01,$05
		.BYTE	$F0,$60,$01,$F5
		.BYTE	$F0,$61,$01,$FD
		.BYTE	$F0,$62,$01,$05
		.BYTE	$F8,$63,$01,$FD
		.BYTE	$80
byte_89CA5:
		.BYTE	$E8,$52,$01,$F5
		.BYTE	$E8,$53,$01,$FD
		.BYTE	$E8,$54,$01,$05
		.BYTE	$F0,$64,$01,$F5
		.BYTE	$F0,$65,$01,$FD
		.BYTE	$F0,$66,$01,$05
		.BYTE	$F8,$67,$01,$FD
		.BYTE	$80
byte_89CC2:
		.BYTE	$E8,$59,$01,$F5
		.BYTE	$E8,$5A,$01,$FD
		.BYTE	$E8,$5B,$01,$05
		.BYTE	$F0,$68,$01,$F5
		.BYTE	$F0,$69,$01,$FD
		.BYTE	$F0,$6A,$01,$05
		.BYTE	$F8,$6B,$01,$FD
		.BYTE	$80
byte_89CDF:
		.BYTE	$E8,$4B,$01,$F5
		.BYTE	$E8,$4C,$01,$FD
		.BYTE	$E8,$4D,$01,$05
		.BYTE	$F0,$6C,$01,$F5
		.BYTE	$F0,$6D,$01,$FD
		.BYTE	$F0,$6E,$01,$05
		.BYTE	$F8,$6F,$01,$FD
		.BYTE	$80
byte_89CFC:
		.BYTE	$E8,$52,$01,$F5
		.BYTE	$E8,$53,$01,$FD
		.BYTE	$E8,$54,$01,$05
		.BYTE	$F0,$70,$01,$F5
		.BYTE	$F0,$71,$01,$FD
		.BYTE	$F0,$72,$01,$05
		.BYTE	$F8,$73,$01,$FD
		.BYTE	$80
byte_89D19:
		.BYTE	$E8,$59,$01,$F5
		.BYTE	$E8,$5A,$01,$FD
		.BYTE	$E8,$5B,$01,$05
		.BYTE	$F0,$74,$01,$F5
		.BYTE	$F0,$75,$01,$FD
		.BYTE	$F0,$76,$01,$05
		.BYTE	$F8,$77,$01,$FD
		.BYTE	$80
byte_89D36:
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
		LDA		_cur_game_cursor_scr_pos._X
		STA		_spr_insert_raw_args._pos._X
		LDA		_cur_game_cursor_scr_pos._Y
		STA		_spr_insert_raw_args._pos._Y
		JMP		_spr_lib_raw_insert
loc_88CE0:
		LDX		_prev_constr_palette_press_idx
		LDA		_constr_building_sizes_list,X
		CPX		#$0C
		BNE		loc_88CEC
		CLC
		ADC		_cur_port_type_select_idx
loc_88CEC:
		STA		_spr_insert_raw_args._idx
		LDA		_cur_game_cursor_scr_pos._X
		AND		#$F8
		STA		_spr_insert_raw_args._pos._X
		LDA		_cur_game_cursor_scr_pos._Y
		AND		#$F8
		STA		_spr_insert_raw_args._pos._Y
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
		DEC		_spr_insert_raw_args._pos._Y
		LDX		_spr_buf_pos
		LDY		#$00
loc_88C9C:
		LDA		(_tmp15),Y
		CMP		#$80
		BEQ		loc_88CC5
		INY
		CLC
		ADC		_spr_insert_raw_args._pos._Y
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
		ADC		_spr_insert_raw_args._pos._X
		STA		_spr_buf,X
		INX
		JMP		loc_88C9C
loc_88CC5:
		STX		_spr_buf_pos
		RTS

		SECTION_END F04

		BANK_END F5,$A000
