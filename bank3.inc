
		ORG $A000

		BANK_START $F3

		SECTION_START C01

; ---------------------------------------------------------------------------
; --G-A-M-E--M-E-S-S-A-G-E--W-I-N-D-O-W-S--E-X-T-E-R-N-A-L--H-A-N-D-L-E-R-S--
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
_game_msg_handler:
		LDX		#$03							; these arrays are 2-byte long
		LDA		#$00							; but they clear them as 4-bytes
loc_58144:
		STA		_anim_pos_is_load,X				; also some of these vars are used
		STA		byte_76,X						; as tmp values for other parts
		STA		byte_78,X						; of the game.
		STA		byte_7A,X
		STA		byte_7C,X						; currently, they used as sprite
		STA		byte_7E,X						; animation state parameters
		STA		byte_80,X						; like speed, direction, script ptr, etc
		DEX
		BPL		loc_58144
		STA		_tmp523
		STA		_tmp528
		STA		_tmp529
		STA		_tmp52A
		STA		_tmp52B
		STA		_tmp52C
		STA		_tmp52D
		STA		_tmp52E
		STA		_tmp52F
		STA		_tmp530
		LDA		#$05
		STA		_scr_mode_idx					; setup CHR banks and music
		JSR		_spr_clear
		JSR		_game_msg_pal_select
		LDA		#$01
		STA		_ppu_pal_dequeue_req
		JSR		_game_msg_apu_select
		JSR		_game_msg_wnd_draw				; draw window
		JSR		_spr_clear
		JSR		_wait_for_nmi
;		FJSRA	_game_msg_chr_setup,PRG2,PRG2	; REDUNDANT
		JSR		_game_msg_chr_setup				; OPTIMIZED: local sub now
		LDA		#$00
		STA		_ptr0
		JSR		_game_msg_txt_draw				; draw text fast line-based
		LDA		#$00
		STA		_nmi_frames_timer
		STA		_nmi_seconds_timer				; init timer for auto skip
.game_msg_loop:
		PUSHB	_ptr0							; here we backup for now a counter
		JSR		_wait_for_nmi					; of pages for current message
		LDA		#$00
		STA		_spr_buf_pos
		JSR		_game_msg_wnd_animA				; animation work
		JSR		_game_msg_wnd_animB
		JSR		_game_msg_wnd_animC
		JSR		_spr_finish
		JSR		_game_msg_presents_clear		; clear area 3x3 with present icon if any
		JSR		_msg_wnd_cheat_test				; cheat test here!
		POPB	_ptr0							; restore pages number
		LDA		_nmi_seconds_timer				; timeout for message window is 30 seconds
		CMP		#$1E							; force quit when exceeded
		BCS		loc_581EC
		LDA		_pad0_press				; test for manual skip/quit buttons
		AND		#$C0
		BEQ		.game_msg_loop
		LDA		_ptr0							; increment pages counter
		CLC
		ADC		#$01
		LDX		_game_msg_idx					; if message have more than one page,
		CMP		_game_msg_screens_count_list,X	; display it.
		BCS		loc_581EC						; of finish and close the window.
		STA		_ptr0							; save pages counter for next loop
		JSR		_game_msg_txt_draw
		LDA		#$00
		STA		_nmi_frames_timer				; reset nmi counters
		STA		_nmi_seconds_timer
		JMP		.game_msg_loop
loc_581EC:
		LDA		#$20							; here we start to quit message
		STA		_input_forbid_delay
		JSR		_spr_clear
		JSR		_wait_for_nmi					; restore CHR banks and reinit vars
		LDA		#$00
		STA		_scr_mode_idx
		LDA		#$80
		JSR		_pal_load_lib
		LDA		#$FF
		STA		_ppu_pal_dequeue_req
		JSR		_game_chr_common_reload
;		JSR		_game_msg_chr1_load
		LDA		_mmc5_chr_banks_shadow+1		; OPTIMIZED, restore CHR 1 from here
		STA		_MMC5_CHR_BANKSA+1
;		LDA		_mmc5_chr_banks_shadow+3		; REDUNDANT, already loaded in
;		STA		_MMC5_CHR_BANKSA+3				; _game_chr_common_reload routine!
		LDA		#$FF
		STA		_full_game_field_redraw_req		; set request for game area update
		LDA		#$00
		STA		_update_rect_left				; special counters for update area
		STA		_update_rect_top				; choses the whole screen, the display
		LDA		#$4D							; code will trim it to the current window
		STA		_update_rect_right				; automatically
		STA		_update_rect_bottom
		JSR		_game_msg_music_restore
		JSR		_present_put_in_list			; finally insert the selected present to the
		JSR		_init_special_year_date			; menu
		JSR		_debug_flag_toggle_test			; and check if we have performed the cheat
		LDA		#$00
		STA		_game_msg_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
; assuming all these are the same here, we'll hardly change it someday
; now, reducing the size, we can return it back to the original bank here
;
_game_msg_chr_setup:
		LDX		_game_msg_idx
;		LDA		_game_msg_chr0_list,X			; OPTIMIZED
		LDA		#$30
		STA		_MMC5_CHR_BANKSA
;		LDA		_game_msg_chr1_list,X			; OPTIMIZED
		LDA		#$31
		STA		_MMC5_CHR_BANKSA+1
;		LDA		_game_msg_chr2_list,X			; OPTIMIZED
		LDA		#$32
		STA		_MMC5_CHR_BANKSA+2
		LDA		_game_msg_chr3_list,X
		STA		_MMC5_CHR_BANKSA+3
		RTS

; OPTIMIZED
;_game_msg_chr0_list:
;		.BYTE	$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
;		.BYTE	$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
;		.BYTE	$30,$30,$30,$30
;_game_msg_chr1_list:
;		.BYTE	$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31
;		.BYTE	$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31,$31
;		.BYTE	$31,$31,$31,$31
;_game_msg_chr2_list:
;		.BYTE	$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32
;		.BYTE	$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32,$32
;		.BYTE	$32,$32,$32,$32
_game_msg_chr3_list:
		.BYTE	$33,$33,$57,$33,$3B,$33,$33,$33,$35,$3D,$33,$57,$33,$57,$57,$57
		.BYTE	$57,$57,$57,$57,$57,$33,$57,$57,$57,$57,$57,$33,$33,$33,$33,$33
		.BYTE	$33,$33,$33,$33

; =============== S U B	R O U T	I N E =======================================
_game_msg_apu_select:
		LDX		_game_msg_idx					; game msg uses one of two
		LDA		_game_msg_apu_types_list,X		; jingles for alert and regular messages
		BEQ		locret_5824B
		CMP		#$01
		BNE		loc_58243
		LDA		#_MUS_IDX_NOTICE0
		STA		_apu_mus_idx_req
		BNE		locret_5824B
loc_58243:
		APUA_SE	_SE_IDX_ALERT
		LDA		#_MUS_IDX_ALERT0
		STA		_apu_mus_idx_req
locret_5824B:
		RTS

_game_msg_apu_types_list:
		.BYTE	$01,$01,$01,$01,$01,$01,$02,$02,$02,$02,$02,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$02,$01
		.BYTE	$02,$02,$02,$02

; =============== S U B	R O U T	I N E =======================================
_game_msg_music_restore:
		LDA		#_MUS_IDX_STOP					; when exit, need to restore music
		STA		_apu_mus_idx_req				; for a corresponding city type
		JSR		_wait_for_nmi
		LDX		_city._city_type
		LDY		_bg_music_list,X				; list in the system bank now
		LDA		_city._game_flags				; for setting with disabled BGM
		AND		#$08							; restore silence
		BNE		loc_58262
		LDY		#_MUS_IDX_STOP
loc_58262:
		STY		_apu_mus_idx_req
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_txt_draw:
		PUSHB	_ptr0
;		JSR		_far_wnd_header_txt_draw		; REDUNDANT: direct call here
;		FJSRA	_wnd_header_txt_draw,PRG0,PRG0	; REDUNDANT: moved to local bank
		JSR		_wnd_header_txt_draw			; -
		POPB	_ptr0
		LDA		#$00
		STA		_ptr0+1
loc_5829C:
		PUSHB	_ptr0							; bug boring line-by-line displaying
		PUSHB	_ptr0+1							; message text.
		JSR		_wait_for_nmi					; all
		POPB	_ptr0+1
		POPB	_ptr0
		JSR		_game_msg_line_pos_calc			; data resides in PRG4, locally selected
		JSR		_game_msg_line_display			; with PRG3 here now
		INC		_ptr0+1
		LDY		_game_msg_idx
; REDUNDANT
;		LDA		_game_msg_lines_cnt_lib_lo,X
;		STA		_tmp2
;		LDA		_game_msg_lines_cnt_lib_hi,X
;		STA		_tmp3
;		LDY		_ptr0
;		LDA		_ptr0+1
;		CMP		(_tmp2),Y
; OPTIMIZED
		LDA		_game_msg_lines_cnt_lib,Y		; for every message and every page data
		CLC										; keep a number of lines to display.
		ADC		_ptr0							; so we either may select another line to draw
		TAY										; or stop here
		LDA		_ptr0+1
		CMP		_game_msg_lines_cnt_lib,Y
; -
		BCC		loc_5829C
		JSR		_game_msg_hints_extra_icons_draw; when text is done, draw extra overlay icons
		JMP		_game_msg_presents_icon_draw	; if any


; =============== S U B	R O U T	I N E =======================================
; REDUNDANT now
;
;_far_wnd_header_txt_draw:
;		FJSRA	_wnd_header_txt_draw,PRG0,PRG0
;		RTS

; OPTIMIZED: moved from external bank to the place it belongs
;
; =============== S U B	R O U T	I N E =======================================
_wnd_header_txt_draw:
		LDA		#$02							; draw 3-lines font on the message window
		STA		_tmp1							; header
loc_BB84:
		LDX		_game_msg_idx
		LDA		_wnd_header_txt_len,X
		STA		_tmp3							; OPTIMIZED: replace table with auto calc
		LDA		#23
		SEC
		SBC		_tmp3
		LSR
		CLC
		ADC		#8
		STA		_tmp2
;		LDA		_wnd_header_txt_pos,X			; OPTIMIZED
;		STA		_tmp2
		JSR		_wnd_header_txt_pos_calc
;		LDA		_game_msg_idx					; REDUNDANT no need to calculate
;		ASL										; idx * 3 value, there is only single
;		CLC										; byte array left
;		ADC		_tmp1							; -
;		ADC		_game_msg_idx					; -
;		TAX										; -
		LDX		_game_msg_idx
		LDA		_wnd_header_tlm_lo,X
		STA		_tmp2
		LDA		_wnd_header_tlm_hi,X
		STA		_tmp3
		LDX		_tmp1							; OPTIMIZED read special symbols
		LDA		_wnd_header_char_delta,X		; table and space symbol for a line
		STA		_ptr0							; -
		LDA		_wnd_header_space_table,X		; -
		STA		_tmp8							; -
		JSR		_wnd_header_queue
		DEC		_tmp1
		BPL		loc_BB84
		RTS

; OPTIMIZED, common position recalc function for all routines here
; =============== S U B	R O U T	I N E =======================================
; args:
;		_tmp1 - Y offset,
;		_tmp2 - X offset,
;		_tmp3 - width
; return:
;		_ptr4 - ppu offset
;		_tmp6 - line len
;		_tmp7 - wrap len
;
_wnd_header_txt_pos_calc:
		CIADDB	_tmp4,_cur_screen_tile._ROW,  #1,_tmp1,#30
		CIADDB	_tmp5,_cur_screen_tile._COL,NONE,_tmp2,#32
		WRAPB	_tmp6,_tmp7,_tmp5,_tmp3,#32
		BUFPOS	_ptr4,_tmp5,_tmp4,#$20,#$00	; DST=(_tmp4*#$20+_tmp5)+#$0000
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_header_queue:
		PPUQSTART
		LDY		#$00
		PPUQMOVB	_tmp4,OP_NOP,0
loc_BC1A:										; OPTIMIZED
		PPUQMOVB	_tmp5,OP_ORA,#$20
		PPUQMOVB	_tmp6,OP_NOP,0
loc_BC1B:
		LDA		(_tmp2),Y						; read header text
		INY
		CMP		#$20							; ADDED, if not space,
		BEQ		loc_BC1C						; adjust character idx
		CLC										; for a corresponding line
		ADC		_ptr0							; -
		.BYTE	$2C								; eats next LDA opcode for sure
loc_BC1C:										; -
		LDA		_tmp8							; or replace with space symbol
		PPUQMOVA
		DEC		_tmp6
		BNE		loc_BC1B
		DO_WRAP	_tmp6,_tmp7,loc_BC49
		PPUQMOVB	_tmp4,OP_AND,#$E0
		JMP		loc_BC1A						; OPTIMIZED
loc_BC49:
		PPUQEND
		RTS

; REDUNDANT, the autocalculated lengths are the same as in the original
; list, but except the very first line, which is moved one tile
; forward for some reasons... now it fixed
;
;_wnd_header_txt_pos:
;		.BYTE	$10,$11,$11,$10,$0E,$0E,$0E,$0E,$11,$0C,$0E,$10,$0E,$10,$10,$10
;		.BYTE	$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$10,$0F,$0F,$0F,$0F,$0E
;		.BYTE	$11,$11,$0F,$10

	MAC	LEN
		.BYTE	{1}_len
	ENDM

_wnd_header_txt_len:
		LEN		aGoodjob
		LEN		aTown
		LEN		aCity
		LEN		aCapital
		LEN		aMetropolis
		LEN		aMegaropolis
		LEN		aTrafficjam
		LEN		aTrafficjam
		LEN		aCrime
		LEN		aMonsterattack
		LEN		aEarthquake
		LEN		aPresent
		LEN		aSistercity
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPresent
		LEN		aPractice
		LEN		aPractice
		LEN		aPractice
		LEN		aPractice
		LEN		aHowtoloan
		LEN		aFire
		LEN		aFlood
		LEN		aAircrash
		LEN		aTornade

; OPTIMIZED: instead of keep all dupes for three different encodings of one
; header name line, we can just keep the difference between characters
; and a special symbol (space)
;
_wnd_header_char_delta:
		.BYTE	[-$40]&$FF,$00,$40
_wnd_header_space_table:
		.BYTE	$00,$00,$81

; REDUNDANT: here is three rows of the same message in 24x8 letters,
; middle row indexes are the same as corresponding ASCII codes
; but upper and lower are shifted, but it is the same letters (except spaces)
; unneeded data replaced with on-the-fly calcs, obviously cannot be used
; with anything except this particular kind of fonts.
; size of routine reduced by half

;		DELTA	-0x40			0				+0x40
;_wnd_header_tlm_lo:
;		.BYTE	<[byte_BD6C],	<[aGoodjob],	<[byte_BE90]
;		.BYTE	<[byte_BD74],	<[aTown],		<[byte_BE98]
;		.BYTE	<[byte_BD78],	<[aCity],		<[byte_BE9C]
;		.BYTE	<[byte_BD7C],	<[aCapital],	<[byte_BEA0]
;		.BYTE	<[byte_BD83],	<[aMetropolis],	<[byte_BEA7]
;		.BYTE	<[byte_BD8D],	<[aMegaropolis],<[byte_BEB1]
;		.BYTE	<[byte_BD98],	<[aTrafficjam],	<[byte_BEBC]
;		.BYTE	<[byte_BD98],	<[aTrafficjam],	<[byte_BEBC]
;		.BYTE	<[byte_BDA3],	<[aCrime],		<[byte_BEC7]
;		.BYTE	<[byte_BDA8],	<[aMonsterattack],<[byte_BECC]
;		.BYTE	<[byte_BDB6],	<[aEarthquake],	<[byte_BEDA]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC7],	<[aSistercity],	<[byte_BEEB]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDC0],	<[aPresent],	<[byte_BEE4]
;		.BYTE	<[byte_BDD2],	<[aPractice],	<[byte_BEF6]
;		.BYTE	<[byte_BDD2],	<[aPractice],	<[byte_BEF6]
;		.BYTE	<[byte_BDD2],	<[aPractice],	<[byte_BEF6]
;		.BYTE	<[byte_BDD2],	<[aPractice],	<[byte_BEF6]
;		.BYTE	<[byte_BDDA],	<[aHowtoloan],	<[byte_BEFE]
;		.BYTE	<[byte_BDE5],	<[aFire],		<[byte_BF09]
;		.BYTE	<[byte_BDE9],	<[aFlood],		<[byte_BF0D]
;		.BYTE	<[byte_BDEE],	<[aAircrash],	<[byte_BF12]
;		.BYTE	<[byte_BDF7],	<[aTornade],	<[byte_BF1B]
;_wnd_header_tlm_hi:
;		.BYTE	>[byte_BD6C],	>[aGoodjob],	>[byte_BE90]
;		.BYTE	>[byte_BD74],	>[aTown],		>[byte_BE98]
;		.BYTE	>[byte_BD78],	>[aCity],		>[byte_BE9C]
;		.BYTE	>[byte_BD7C],	>[aCapital],	>[byte_BEA0]
;		.BYTE	>[byte_BD83],	>[aMetropolis],	>[byte_BEA7]
;		.BYTE	>[byte_BD8D],	>[aMegaropolis],>[byte_BEB1]
;		.BYTE	>[byte_BD98],	>[aTrafficjam],	>[byte_BEBC]
;		.BYTE	>[byte_BD98],	>[aTrafficjam],	>[byte_BEBC]
;		.BYTE	>[byte_BDA3],	>[aCrime],		>[byte_BEC7]
;		.BYTE	>[byte_BDA8],	>[aMonsterattack],>[byte_BECC]
;		.BYTE	>[byte_BDB6],	>[aEarthquake],	>[byte_BEDA]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC7],	>[aSistercity],	>[byte_BEEB]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDC0],	>[aPresent],	>[byte_BEE4]
;		.BYTE	>[byte_BDD2],	>[aPractice],	>[byte_BEF6]
;		.BYTE	>[byte_BDD2],	>[aPractice],	>[byte_BEF6]
;		.BYTE	>[byte_BDD2],	>[aPractice],	>[byte_BEF6]
;		.BYTE	>[byte_BDD2],	>[aPractice],	>[byte_BEF6]
;		.BYTE	>[byte_BDDA],	>[aHowtoloan],	>[byte_BEFE]
;		.BYTE	>[byte_BDE5],	>[aFire],		>[byte_BF09]
;		.BYTE	>[byte_BDE9],	>[aFlood],		>[byte_BF0D]
;		.BYTE	>[byte_BDEE],	>[aAircrash],	>[byte_BF12]
;		.BYTE	>[byte_BDF7],	>[aTornade],	>[byte_BF1B]

_wnd_header_tlm_lo:
		.BYTE	<[aGoodjob]
		.BYTE	<[aTown]
		.BYTE	<[aCity]
		.BYTE	<[aCapital]
		.BYTE	<[aMetropolis]
		.BYTE	<[aMegaropolis]
		.BYTE	<[aTrafficjam]
		.BYTE	<[aTrafficjam]
		.BYTE	<[aCrime]
		.BYTE	<[aMonsterattack]
		.BYTE	<[aEarthquake]
		.BYTE	<[aPresent]
		.BYTE	<[aSistercity]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPresent]
		.BYTE	<[aPractice]
		.BYTE	<[aPractice]
		.BYTE	<[aPractice]
		.BYTE	<[aPractice]
		.BYTE	<[aHowtoloan]
		.BYTE	<[aFire]
		.BYTE	<[aFlood]
		.BYTE	<[aAircrash]
		.BYTE	<[aTornade]
_wnd_header_tlm_hi:
		.BYTE	>[aGoodjob]
		.BYTE	>[aTown]
		.BYTE	>[aCity]
		.BYTE	>[aCapital]
		.BYTE	>[aMetropolis]
		.BYTE	>[aMegaropolis]
		.BYTE	>[aTrafficjam]
		.BYTE	>[aTrafficjam]
		.BYTE	>[aCrime]
		.BYTE	>[aMonsterattack]
		.BYTE	>[aEarthquake]
		.BYTE	>[aPresent]
		.BYTE	>[aSistercity]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPresent]
		.BYTE	>[aPractice]
		.BYTE	>[aPractice]
		.BYTE	>[aPractice]
		.BYTE	>[aPractice]
		.BYTE	>[aHowtoloan]
		.BYTE	>[aFire]
		.BYTE	>[aFlood]
		.BYTE	>[aAircrash]
		.BYTE	>[aTornade]

;byte_BD6C:
;		.BYTE	$07,$2F,$2F,$24,$00,$2A,$2F,$22
;byte_BD74:
;		.BYTE	$14,$2F,$37,$2E
;byte_BD78:
;		.BYTE	$03,$29,$34,$39
;byte_BD7C:
;		.BYTE	$03,$21,$30,$29,$34,$21,$2C
;byte_BD83:
;		.BYTE	$0D,$25,$34,$32,$2F,$30,$2F,$2C,$29,$33
;byte_BD8D:
;		.BYTE	$0D,$25,$27,$21,$32,$2F,$30,$2F,$2C,$29,$33
;byte_BD98:
;		.BYTE	$14,$32,$21,$26,$26,$29,$23,$00,$2A,$21,$2D
;byte_BDA3:
;		.BYTE	$03,$32,$29,$2D,$25
;byte_BDA8:
;		.BYTE	$0D,$2F,$2E,$33,$34,$25,$32,$00,$21,$34,$34,$21,$23,$2B
;byte_BDB6:
;		.BYTE	$05,$21,$32,$34,$28,$31,$35,$21,$2B,$25
;byte_BDC0:
;		.BYTE	$10,$32,$25,$33,$25,$2E,$34
;byte_BDC7:
;		.BYTE	$13,$29,$33,$34,$25,$32,$00,$03,$29,$34,$39
;byte_BDD2:
;		.BYTE	$10,$32,$21,$23,$34,$29,$23,$25
;byte_BDDA:
;		.BYTE	$08,$2F,$37,$00,$14,$2F,$00,$0C,$2F,$21,$2E
;byte_BDE5:
;		.BYTE	$06,$29,$32,$25
;byte_BDE9:
;		.BYTE	$06,$2C,$2F,$2F,$24
;byte_BDEE:
;		.BYTE	$01,$29,$32,$00,$23,$32,$21,$33,$28
;byte_BDF7:
;		.BYTE	$14,$2F,$32,$2E,$21,$24,$25

	MAC	STR
{1}		EQU	.
		.BYTE	{2}
{1}_len	EQU	.-{1}
	ENDM

		STR		aGoodjob,		"Good job"
		STR		aTown,			"Town"
		STR		aCity,			"City"
		STR		aCapital,		"Capital"
		STR		aMetropolis,	"Metropolis"
		STR		aMegaropolis,	"Megaropolis"
		STR		aTrafficjam,	"Traffic jam"
		STR		aCrime,			"Crime"
		STR		aMonsterattack,	"Monster attack"
		STR		aEarthquake,	"Earthquake"
		STR		aPresent,		"Present"
		STR		aSistercity,	"Sister City"
		STR		aPractice,		"Practice"
		STR		aHowtoloan,		"How To Loan"
		STR		aFire,			"Fire"
		STR		aFlood,			"Flood"
		STR		aAircrash,		"Air crash"
;		STR		aTornade,		"Tornade"
		STR		aTornade,		"Tornado"			; HUH?

;byte_BE90:
;		.BYTE	$87,$AF,$AF,$A4,$81,$AA,$AF,$A2
;byte_BE98:
;		.BYTE	$94,$AF,$B7,$AE
;byte_BE9C:
;		.BYTE	$83,$A9,$B4,$B9
;byte_BEA0:
;		.BYTE	$83,$A1,$B0,$A9,$B4,$A1,$AC
;byte_BEA7:
;		.BYTE	$8D,$A5,$B4,$B2,$AF,$B0,$AF,$AC,$A9,$B3
;byte_BEB1:
;		.BYTE	$8D,$A5,$A7,$A1,$B2,$AF,$B0,$AF,$AC,$A9,$B3
;byte_BEBC:
;		.BYTE	$94,$B2,$A1,$A6,$A6,$A9,$A3,$81,$AA,$A1,$AD
;byte_BEC7:
;		.BYTE	$83,$B2,$A9,$AD,$A5
;byte_BECC:
;		.BYTE	$8D,$AF,$AE,$B3,$B4,$A5,$B2,$81,$A1,$B4,$B4,$A1,$A3,$AB
;byte_BEDA:
;		.BYTE	$85,$A1,$B2,$B4,$A8,$B1,$B5,$A1,$AB,$A5
;byte_BEE4:
;		.BYTE	$90,$B2,$A5,$B3,$A5,$AE,$B4
;byte_BEEB:
;		.BYTE	$93,$A9,$B3,$B4,$A5,$B2,$81,$83,$A9,$B4,$B9
;byte_BEF6:
;		.BYTE	$90,$B2,$A1,$A3,$B4,$A9,$A3,$A5
;byte_BEFE:
;		.BYTE	$88,$AF,$B7,$81,$94,$AF,$81,$8C,$AF,$A1,$AE
;byte_BF09:
;		.BYTE	$86,$A9,$B2,$A5
;byte_BF0D:
;		.BYTE	$86,$AC,$AF,$AF,$A4
;byte_BF12:
;		.BYTE	$81,$A9,$B2,$81,$A3,$B2,$A1,$B3,$A8
;byte_BF1B:
;		.BYTE	$94,$AF,$B2,$AE,$A1,$A4,$A5

; OPTIMIZED, another common routine for position calc, they
; are short in sources, but very long in code, so less
; routines, less code generated.
;
; =============== S U B	R O U T	I N E =======================================
_game_msg_line_pos_calc:
		CIADDB	_tmp2,_cur_screen_tile._ROW,#$04,_tmp1,#$1E
		CIADDB	_tmp3,_cur_screen_tile._COL,#$08, NONE,#$20
		WRAPB	_tmp4,_tmp5,_tmp3,#$17,#$20
		BUFPOS	_ptr2,_tmp3,_tmp2,#$20,#$00	; DST=(_tmp2*#$20+_tmp3)+#$0000
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_line_display:
		LDX		_game_msg_idx
		LDA		_game_msg_lisb_lo,X
		LDY		_game_msg_lisb_hi,X
		STA		_tmp6
		STY		_tmp6+1
		LDA		_ptr0
		ASL
		TAY
		LDA		(_tmp6),Y
		INY
		PHA
		LDA		(_tmp6),Y
		STA		_tmp6+1
		POPB	_tmp6
		LDA		_ptr0+1
		ASL
		TAY
		LDA		(_tmp6),Y
		INY
		PHA
		LDA		(_tmp6),Y
		STA		_tmp6+1
		POPB	_tmp6
; NOTE: here is the code for extnt data queue for message lines
; as you can see, it's always 0B, the same as drawn by the windows
; drawing routine, so they already 0B why we do need to reinit it?
; because of a special icon at the second practice hint screen!
; it changes the color of word "start" if extnt isn't cleared
;
		PUSHB	_tmp4
		PUSHB	_tmp5
		EXTQSTART
		EXTQMOVB	_tmp2,OP_NOP,0
loc_58370:
		EXTQMOVB	_tmp3,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_tmp4,OP_NOP,0
loc_58371:
		EXTQMOVB	#$0B,OP_NOP,0
		DEC		_tmp4
		BNE		loc_58371
		DO_WRAP	_tmp4,_tmp5,loc_5839E
		EXTQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_58370
loc_5839E:
		EXTQEND
		POPB	_tmp5
		POPB	_tmp4
; -
		PPUQSTART
		LDY		#$00
		PPUQMOVB	_tmp2,OP_NOP,0
loc_583BD:
		PPUQMOVB	_tmp3,OP_ORA,#$20
		PPUQMOVB	_tmp4,OP_NOP,0
loc_583BE:
		LDA		(_tmp6),Y
		INY
		CMP		#$40
		BEQ		loc_583FA
		CMP		#$0F
		BNE		loc_583CC
		DEY
		LDA		#$20
loc_583CC:
		PPUQMOVA
		DEC		_tmp4
		BNE		loc_583BE
		DO_WRAP	_tmp4,_tmp5,loc_583F7
		PPUQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_583BD
loc_583F7:
		PPUQEND
		RTS
loc_583FA:
		LDA		(_tmp6),Y
		INY
		JSR		_cmd_data_insert
		BCS		loc_583F7
		JMP		loc_583BE

; =============== S U B	R O U T	I N E =======================================
;_game_msg_chr1_load:							; REDUNDANT
;		LDA		_mmc5_chr_banks_shadow+1
;		STA		_MMC5_CHR_BANKSA+1
;		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_presents_clear:
		LDA		_tmp528							; here we test a special flag
		CMP		#$02							; to decide to clear this area
		BEQ		loc_58476						; or not
		RTS
loc_58476:
		LDA		#$02							; clear 3 lines here
		STA		_ptr0+1
loc_5847A:
		JSR		_game_msg_presents_pos_calc
		JSR		_game_msg_presents_line_clear	; clear line length 3
		DEC		_ptr0+1
		BPL		loc_5847A
		INC		_tmp528							; make sure we won't clear it again
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_presents_pos_calc:
		CIADDB	_tmp2,_cur_screen_tile._ROW,#$0C,_tmp1,#$1E
		CIADDB	_tmp3,_cur_screen_tile._COL,#$11, NONE,#$20
		WRAPB	_tmp4,_tmp5,_tmp3,#$03,#$20
		BUFPOS	_ptr2,_tmp3,_tmp2,#$20,#$00	; DST=(_tmp2*#$20+_tmp3)+#$0000
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_presents_line_clear:
		EXTQSTART
		PUSHB		_tmp4						; save offset for now
		PUSHB		_tmp5
		EXTQMOVB	_tmp2,OP_NOP,0				; setup and clear exnt line
loc_584FA:
		EXTQMOVB	_tmp3,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_tmp4,OP_NOP,0
loc_584FB:
		EXTQMOVB	#$0B,OP_NOP,0
		DEC		_tmp4
		BNE		loc_584FB
		DO_WRAP	_tmp4,_tmp5,loc_58527
		EXTQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_584FA
loc_58527:
		EXTQEND
		POPB	_tmp5							; reuse offset to nt data as well
		POPB	_tmp4
		PPUQSTART
		PPUQMOVB	_tmp2,OP_NOP,0
loc_58540:
		PPUQMOVB	_tmp3,OP_ORA,#$20			; fill with space.
		PPUQMOVB	_tmp4,OP_NOP,0
loc_58541:
		PPUQMOVB	#$20,OP_NOP,0
		DEC		_tmp4
		BNE		loc_58541
		DO_WRAP	_tmp4,_tmp5,loc_5856D
		PPUQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_58540
loc_5856D:
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_animA:
		LDA		_tmp523
		BNE		loc_58419
		LDX		_game_msg_idx
		LDA		_anim_seq_cmd_list,X
		STA		_tmp523
loc_58419:
		CMP		#$FA
		BEQ		locret_5846D
		LDY		#$CF
		LDX		_spr_buf_pos
		LDA		#$07
		STA		_ptr0
loc_58425:
		LDA		#$F4
		STA		_spr_buf,X
		STA		_spr_buf+4,X
		LDA		_tmp523
		CMP		#$FC
		BEQ		loc_58438
		TYA
		STA		_spr_buf,X
loc_58438:
		LDA		_tmp523
		CMP		#$FD
		BEQ		loc_58443
		TYA
		STA		_spr_buf+4,X
loc_58443:
		LDA		#$BA
		STA		_spr_buf+1,X
		STA		_spr_buf+5,X
		LDA		#$20
		STA		_spr_buf+2,X
		STA		_spr_buf+6,X
		LDA		#$38
		STA		_spr_buf+3,X
		LDA		#$F8
		STA		_spr_buf+7,X
		TXA
		CLC
		ADC		#$08
		TAX
		TYA
		SEC
		SBC		#$08
		TAY
		DEC		_ptr0
		BNE		loc_58425
		STX		_spr_buf_pos
locret_5846D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_animB:
		LDX		_game_msg_idx
		LDA		_animB_data_table0,X
		BNE		loc_58578
		RTS
loc_58578:
		LDA		#$FF
		STA		_hud_build_name_draw_req
		LDA		_tmp528
		BEQ		loc_58586
		BPL		loc_585A5
		JMP		loc_58614
loc_58586:
		LDA		#$98
		STA		_tmp529
		LDA		#$A8
		STA		_tmp52A
		INC		_tmp528
		LDA		_animB_data_list1
		STA		_tmp52B
		LDA		_animB_data_list4
		STA		_tmp52D
		LDA		_animB_data_list6
		STA		_tmp52F
loc_585A5:
		LDY		_tmp52C
		LDX		_tmp52B
		CPX		#$00
		BNE		loc_585CA
		INY
		LDA		_animB_data_list1,Y
		TAX
		CMP		#$FF
		BNE		loc_585BE
		STA		_tmp528
		JMP		loc_58614
loc_585BE:
		CMP		#$FE
		BNE		loc_585CA
		INY
		LDA		_animB_data_list1,Y
		TAX
		INC		_tmp528
loc_585CA:
		LDA		_animB_data_list0,Y
		CMP		#$FD
		BNE		loc_585DB
		DEX
		STX		_tmp52B
		STY		_tmp52C
		JMP		loc_58614
loc_585DB:
		STA		_spr_insert_args._idx
		DEX
		STX		_tmp52B
		STY		_tmp52C
		LDY		_tmp52E
		LDX		_tmp52D
		CPX		#$00
		BNE		loc_585F2
		INY
		LDX		_animB_data_list4,Y
loc_585F2:
		LDA		_animB_data_list2,Y
		CLC
		ADC		_tmp529
		STA		_tmp529
		STA		_spr_insert_args._pos._COL
		LDA		_animB_data_list3,Y
		CLC
		ADC		_tmp52A
		STA		_tmp52A
		STA		_spr_insert_args._pos._ROW
		DEX
		STX		_tmp52D
		STY		_tmp52E
		JSR		_spr_lib_attr_insert
loc_58614:
		LDY		_tmp530
		LDX		_tmp52F
		CPX		#$00
		BNE		loc_58622
		INY
		LDX		_animB_data_list6,Y
loc_58622:
		LDA		_animB_data_list5,Y
		CMP		#$FF
		BEQ		locret_58641
		CMP		#$FE
		BEQ		loc_58642
		STA		_spr_insert_args._idx
		DEX
		STX		_tmp52F
		STY		_tmp530
		LDA		#$28
		STA		_spr_insert_args._pos._COL
		LDA		#$A8
		STA		_spr_insert_args._pos._ROW
		JMP		_spr_lib_attr_insert
locret_58641:
		RTS
loc_58642:
		DEX
		STX		_tmp52F
		STY		_tmp530
		RTS

_animB_data_list0:
		.BYTE	$FD,$FD,$90,$91,$92,$93,$94,$95,$96,$97,$98,$00,$99,$9D,$FF
_animB_data_list1:
		.BYTE	$C0,$E0,$02,$02,$02,$02,$02,$02,$02,$02,$02,$FE,$04,$6E,$FF
_animB_data_list2:
		.BYTE	$00,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FF
_animB_data_list3:
		.BYTE	$00,$00,$01,$00,$01,$00,$01,$00,$01,$00,$01,$00,$01,$FF
_animB_data_list4:
		.BYTE	$60,$05,$01,$05,$01,$05,$01,$05,$01,$05,$01,$05,$01,$FF
_animB_data_list5:
		.BYTE	$FE,$FE,$FE,$9A,$9B,$9C,$9E,$9F,$9A,$A1,$A1,$A1,$A1,$A1,$A1,$FF
_animB_data_list6:
		.BYTE	$C0,$94,$AE,$08,$08,$14,$40,$08,$08,$E0,$E0,$E0,$E0,$E0,$E0,$FF
_animB_data_table0:
		.BYTE	$00,$00,$01,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$00,$01
		.BYTE	$00,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$00,$01,$01
		.BYTE	$01,$01,$01,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$00,$00

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_animC:
		LDX		_game_msg_idx
		LDA		_anim_seq_obj_count_list,X
		STA		_tmp522							; tmp anim obj counter
.anim_load_loop:
		LDX		_game_msg_idx
		LDA		_anim_seq_lib_lo,X
		STA		_ptr0
		LDA		_anim_seq_lib_hi,X
		STA		_ptr0+1
		LDY		_tmp522
		DEY
		LDA		(_ptr0),Y
		TAX
		LDA		_anim_pos_is_load,Y
		BEQ		loc_586FB
		BPL		loc_58707
		JMP		.anim_load_next_obj
loc_586FB:
		LDA		_anim_pos_col_list,X
		STA		_anim_pos_col,Y
		LDA		_anim_pos_row_list,X
		STA		_anim_pos_row,Y
loc_58707:
		LDA		_anim_pos_col,Y
		STA		_spr_insert_args._pos._COL
		LDA		_anim_pos_row,Y
		STA		_spr_insert_args._pos._ROW
		LDA		#$01
		STA		_anim_pos_is_load,Y
		LDA		_anim_spr_lib_lo,X
		STA		_ptr0
		LDA		_anim_spr_lib_hi,X
		STA		_ptr0+1
		LDA		_anim_data_lib_lo,X
		STA		_tmp2
		LDA		_anim_data_lib_hi,X
		STA		_tmp3
		LDA		_anim_dataA_lib_lo,X
		STA		_tmp4
		LDA		_anim_dataA_lib_hi,X
		STA		_tmp5
		LDA		_anim_dataB_lib_lo,X
		STA		_tmp6
		LDA		_anim_dataB_lib_hi,X
		STA		_tmp6+1
		LDX		byte_76,Y
		LDA		byte_78,Y
		TAY
		CPX		#$00
		BNE		loc_58751
loc_58748:
		LDA		(_tmp2),Y
		BEQ		loc_58760
		CMP		#$F8
		BCS		loc_5876C
		TAX
loc_58751:
		LDA		(_ptr0),Y
		STA		_spr_insert_args._idx
		DEX
		BNE		loc_587C5
		INY
		JMP		loc_587C5
		LDA		(_tmp2),Y
		BNE		loc_5876C
loc_58760:
		LDY		_tmp522
		DEY
		LDA		#$FF
		STA		_anim_pos_is_load,Y
		JMP		.anim_load_next_obj
loc_5876C:
		CMP		#$FE
		BNE		loc_5877E
		INY
		LDA		(_tmp2),Y
		INY
		STA		byte_82
		TYA
		SEC
		SBC		byte_82
		TAY
		JMP		loc_58748
loc_5877E:
		CMP		#$FF
		BNE		loc_587BE
		INY
		LDA		(_tmp2),Y
		INY
		STA		byte_82
		STY		byte_83
		LDY		_tmp522
		DEY
		LDA		byte_7E,Y
		BEQ		loc_587AA
		SEC
		SBC		#$01
		STA		byte_7E,Y
		BEQ		loc_587A4
		LDA		byte_83
		SEC
		SBC		byte_82
		TAY
		JMP		loc_58748
loc_587A4:
		LDY		byte_83
		INY
		JMP		loc_58748
loc_587AA:
		LDY		byte_83
		LDA		(_tmp2),Y
		LDY		_tmp522
		DEY
		STA		byte_7E,Y
		LDA		byte_83
		SEC
		SBC		byte_82
		TAY
		JMP		loc_58748
loc_587BE:
		INY
		STA		_tmp523
		JMP		loc_58748
loc_587C5:
		TYA
		LDY		_tmp522
		DEY
		STA		byte_78,Y
		TXA
		STA		byte_76,Y
		LDX		byte_7A,Y
		LDA		byte_7C,Y
		TAY
		CPX		#$00
		BNE		loc_587DE
		LDA		(_tmp6),Y
		TAX
loc_587DE:
		DEX
		BNE		loc_58837
		INY
loc_587E2:
		LDA		(_tmp6),Y
		CMP		#$FE
		BNE		loc_587F6
		INY
		LDA		(_tmp6),Y
		INY
		STA		byte_82
		TYA
		SEC
		SBC		byte_82
		TAY
		JMP		loc_587E2
loc_587F6:
		CMP		#$FF
		BNE		loc_58836
		INY
		LDA		(_tmp6),Y
		INY
		STA		byte_82
		STY		byte_83
		LDY		_tmp522
		DEY
		LDA		byte_80,Y
		BEQ		loc_58822
		SEC
		SBC		#$01
		STA		byte_80,Y
		BEQ		loc_5881C
		LDA		byte_83
		SEC
		SBC		byte_82
		TAY
		JMP		loc_587E2
loc_5881C:
		LDY		byte_83
		INY
		JMP		loc_587E2
loc_58822:
		LDY		byte_83
		LDA		(_tmp6),Y
		LDY		_tmp522
		DEY
		STA		byte_80,Y
		LDA		byte_83
		SEC
		SBC		byte_82
		TAY
		JMP		loc_587E2
loc_58836:
		TAX
loc_58837:
		LDA		(_tmp4),Y
		PHA
		TYA
		LDY		_tmp522
		DEY
		STA		byte_7C,Y
		TXA
		STA		byte_7A,Y
		PLA
		TAX
		AND		#$0F
		CMP		#$08
		BCC		loc_58850
		ORA		#$F0
loc_58850:
		CLC
		ADC		_anim_pos_col,Y
		STA		_anim_pos_col,Y
		TXA
		AND		#$F0
		LSR
		LSR
		LSR
		LSR
		CMP		#$08
		BCC		loc_58864
		ORA		#$F0
loc_58864:
		CLC
		ADC		_anim_pos_row,Y
		STA		_anim_pos_row,Y
		JSR		_spr_lib_clip_insert
.anim_load_next_obj:
		DEC		_tmp522
		BEQ		locret_58876
		JMP		.anim_load_loop
locret_58876:
		RTS

; =============== S U B	R O U T	I N E =======================================
_cmd_data_insert:
		CMP		#$23
		BNE		loc_5887E
		JMP		_cmd40_23_popul
loc_5887E:
		CMP		#$24
		BNE		locret_58885
		JMP		_cmd40_24_name
locret_58885:
		RTS

; =============== S U B	R O U T	I N E =======================================
_cmd40_24_name:
		LDA		(_tmp6),Y
		INY
		SEC
		SBC		#$30
		JSR		_get_select_name
		LDA		_tmp51A
		BNE		loc_58896
		CLC
		RTS
loc_58896:
		TYA
		PHA
		LDY		#$00
loc_5889A:
		LDA		(word_DA),Y
		INY
		PPUQMOVA
		DEC		_tmp4
		BNE		loc_588C5
		DO_WRAP	_tmp4,_tmp5,loc_588CE
		PPUQMOVB	_tmp2,OP_AND,#$E0
		PPUQMOVB	_tmp3,OP_ORA,#$20
		PPUQMOVB	_tmp4,OP_NOP,0
loc_588C5:
		DEC		_tmp51A
		BNE		loc_5889A
		PLA
		TAY
		CLC
		RTS
loc_588CE:
		PLA
		TAY
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_select_name:
		CMP		#$00
		BEQ		loc_588E4
		CMP		#$01
		BEQ		loc_588F6
		CMP		#$02
		BEQ		loc_58908
		RTS
;		CMP		#$03							; REDUNDANT, unused by now
;		BEQ		loc_5891A
;		BNE		loc_5892C
loc_588E4:
		LDA		_city._name
		SEC
		SBC		#$01
		STA		_tmp51A
		MOVWO	word_DA,_city._name+1
		RTS
loc_588F6:
		LDA		_city._sister_name
		SEC
		SBC		#$01
		STA		_tmp51A
		MOVWO	word_DA,_city._sister_name+1	; empty in practice, scenarios does not need it
		RTS
loc_58908:
		LDA		_city._bank_name
		SEC
		SBC		#$01
		STA		_tmp51A
		MOVWO	word_DA,_city._bank_name+1		; always empty, never initializes
		RTS										; so displaying always just as "BANK"
; REDUNDANT, unused by now
;loc_5891A:
;		LDA		_city._unused_name3
;		SEC
;		SBC		#$01
;		STA		_tmp51A
;		MOVWO	word_DA,_city._unused_name3+1	; unused buffers/name
;		RTS										; there is unknown what they
;loc_5892C:										; were intended for
;		LDA		_city._unused_name4
;		SEC
;		SBC		#$01
;		STA		_tmp51A
;		MOVWO	word_DA,_city._unused_name4+1
;		RTS

; =============== S U B	R O U T	I N E =======================================
_cmd40_23_popul:
		LDA		(_tmp6),Y
		INY
		SEC
		SBC		#$30
		JSR		_get_select_popul_num
		LDA		#$01
		STA		_tmp51B
		TYA
		PHA
		LDY		#$05
loc_58950:
		LDA		_tmp_popul_decimal_buf,Y
		DEY
		CMP		#$30
		BNE		loc_58963
		LDA		_tmp51B
		BEQ		loc_58961
		CPY		#$FF
		BNE		loc_58950
loc_58961:
		LDA		#$30
loc_58963:
		PPUQMOVA
		LDA		#$00
		STA		_tmp51B
		DEC		_tmp4
		BNE		loc_58990
		DO_WRAP	_tmp4,_tmp5,loc_58998
		PPUQMOVB	_tmp2,OP_AND,#$E0
		PPUQMOVB	_tmp3,OP_ORA,#$20
		PPUQMOVB	_tmp4,OP_NOP,0
loc_58990:
		CPY		#$FF
		BNE		loc_58950
		PLA
		TAY
		CLC
		RTS
loc_58998:
		PLA
		TAY
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_select_popul_num:
;		CMP		#$00							; REDUNDANT
;		BEQ		loc_589A4						; -
;		CMP		#$01							; -
;		BEQ		loc_589B9						; -
;loc_589A4:
		MOVD	_tmp_popul_hex,_popul_cur
;		JMP		loc_589C8						; -
;loc_589B9:										; REDUNDANT
;		MOVDI	_tmp_popul_hex,10000			; request for fixed amount of
;loc_589C8:										; citizens, not used anywhere
		TYA
		PHA
		LDY		#$05
loc_589CC:
		LDA		#$00
		STA		_tmp_popul_decimal_buf,Y
loc_589D1:
;		SEC										; REDUNDANT
;		LDA		_tmp_popul_hex
;		SBC		_dec_digit_tbl0,Y
;		STA		_tmp_popul_hex
;		LDA		_tmp_popul_hex+1
;		SBC		_dec_digit_tbl1,Y
;		STA		_tmp_popul_hex+1
;		LDA		_tmp_popul_hex+2
;		SBC		_dec_digit_tbl2,Y
;		STA		_tmp_popul_hex+2
		LDA		_tmp_popul_hex					; OPTIMIZED
		CMP		_dec_digit_tbl0,Y				; -
		LDA		_tmp_popul_hex+1				; -
		SBC		_dec_digit_tbl1,Y				; -
		LDA		_tmp_popul_hex+2				; -
		SBC		_dec_digit_tbl2,Y				; -
		BCC		loc_589FA
		LDA		_tmp_popul_hex					; OPTIMIZED, we has to sub here
		SBC		_dec_digit_tbl0,Y				; also C already set as well
		STA		_tmp_popul_hex
		LDA		_tmp_popul_hex+1
		SBC		_dec_digit_tbl1,Y
		STA		_tmp_popul_hex+1
		LDA		_tmp_popul_hex+2
		SBC		_dec_digit_tbl2,Y
		STA		_tmp_popul_hex+2
		LDA		_tmp_popul_decimal_buf,Y
		CLC
		ADC		#$01
		STA		_tmp_popul_decimal_buf,Y
		BNE		loc_589D1
loc_589FA:
;		CLC										; REDUNDANT
;		LDA		_tmp_popul_hex
;		ADC		_dec_digit_tbl0,Y
;		STA		_tmp_popul_hex
;		LDA		_tmp_popul_hex+1
;		ADC		_dec_digit_tbl1,Y
;		STA		_tmp_popul_hex+1
;		LDA		_tmp_popul_hex+2
;		ADC		_dec_digit_tbl2,Y
;		STA		_tmp_popul_hex+2
		LDA		_tmp_popul_decimal_buf,Y
		CLC
		ADC		#$30
		STA		_tmp_popul_decimal_buf,Y
		DEY
		BPL		loc_589CC
		PLA
		TAY
		RTS

; =============== S U B	R O U T	I N E =======================================
;       0
; event	1 x
; event	2 item F	White House
; event	3 x
; event	4 x
; event	5 x
; event	6 x
; event	7 x
; event	8 x
; event	9 x
; event	A x
; event	B item 1	Bank
; event	C x
; event	D item 2	Amusement Park
; event	E item 3	Zoo
; event	F item 4	Casino
; event	10 item	5	Water Reclaim
; event	11 item	6	Police Station
; event	12 item	7	Fire Station
; event	13 item	8	10th Monument
; event	14 item	9	100th Monument
; event	15 x
; event	16 item	A	Expo
; event	17 item	B	Monument
; event	18 item	C	Library
; event	19 item	D	Park
; event	1A item	E	Station
;
; REDUNDANT: too complex, replace with table.
;
_present_put_in_list:
;		LDX		#$0F
;		LDA		_game_msg_idx
;		CMP		#$02
;		BEQ		loc_58A5A
;		LDX		#$01
;		CMP		#$0B
;		BEQ		loc_58A5A
;		CMP		#$15
;		BEQ		locret_58A6D
;		CMP		#$0D
;		BCC		locret_58A6D
;		CMP		#$1B
;		BCS		locret_58A6D
;		SEC
;		SBC		#$0B
;		TAX
;		CPX		#$0B
;		BCC		loc_58A5A
;		DEX
;loc_58A5A:
;		TXA
;		PHA
		LDX		#$FF
		LDY		#$04
loc_58A60:
		INX
		LDA		_city._presents_list,X
		BEQ		loc_58A69
		DEY
		BNE		loc_58A60
loc_58A69:
;		PLA
		LDY		_game_msg_idx					; OPTIMIZED
		LDA		_presents_list,Y
		BMI		locret_58A6D
		STA		_city._presents_list,X
locret_58A6D:
		RTS

_presents_list:
		.BYTE	$FF,$FF,$0F,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$01,$FF,$02,$03,$04
		.BYTE	$05,$06,$07,$08,$09,$FF,$0A,$0B,$0C,$0D,$0E,$FF,$FF,$FF,$FF,$FF
		.BYTE	$FF,$FF,$FF,$FF

; =============== S U B	R O U T	I N E =======================================
_init_special_year_date:
		LDA		_game_msg_idx
		CMP		#$0C
		BEQ		loc_58A79
		CMP		#$15
		BEQ		loc_58A85
		RTS
loc_58A79:
		LDA		#$0A
		JSR		_adc_cur_year
		STA		_city._year_to_liberty
		STX		_city._year_to_liberty+1
		RTS
loc_58A85:
		LDA		#$01
		JSR		_adc_cur_year
		STA		_city._year_to_expo
		STX		_city._year_to_expo+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_adc_cur_year:
		LDX		_city._cur_year+1
		CLC
		ADC		_city._cur_year
;		LDA		_city._cur_year					; BUG wait, WHAT?
		BCC		locret_58A9E
		INX
locret_58A9E:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_draw:
;		JSR		_buffers_test_flush				; REDUNDANT, merged
;		BCS		_game_msg_wnd_draw
		JSR		_buffers_test_flush				; OPTIMIZED, the same function
		BNE		_game_msg_wnd_draw				; just test Z instead of C
		LDA		#$00
		STA		_tmp0
loc_58AA8:
		PUSHB	_tmp0
		JSR		_game_msg_wnd_line_pos_calc
		JSR		_game_msg_wnd_line_draw
		JSR		_wait_for_nmi
		POPB	_tmp0
		INC		_tmp0
		LDA		_tmp0
		CMP		#$15
		BCC		loc_58AA8
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_line_pos_calc:
		CIADDB	_tmp1,_cur_screen_tile._ROW,NONE,_tmp0,#$1E
		CIADDB	_tmp2,_cur_screen_tile._COL,#$07, NONE,#$20
		WRAPB	_tmp3,_tmp4,_tmp2,#$19,#$20
		BUFPOS	_tmp1,_tmp2,_tmp1,#$20,#$00	; DST=(_tmp1*#$20+_tmp2)+#$0000
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_line_draw:
		LDX		_tmp0
		LDY		_game_msg_wnd_nt_ofs_list,X
		STY		_tmp6+1
		PUSHB	_tmp3
		PUSHB	_tmp4
		PPUQSTART
		PPUQMOVB	_ptr0+1,OP_NOP,0
loc_58B37:
		PPUQMOVB	_tmp2,OP_ORA,#$20
		PPUQMOVB	_tmp3,OP_NOP,0
loc_58B38:
		LDA		_game_msg_wnd_extnt_frame_data,Y
		INY
		PPUQMOVA
		DEC		_tmp3
		BNE		loc_58B38
		DO_WRAP	_tmp3,_tmp4,loc_58B67
		PPUQMOVB	_ptr0+1,OP_AND,#$E0
		JMP		loc_58B37
loc_58B67:
		LDY		_tmp6+1
		PPUQEND
		POPB	_tmp4
		POPB	_tmp3
		EXTQSTART
		EXTQMOVB	_ptr0+1,OP_NOP,0
loc_58B80:
		EXTQMOVB	_tmp2,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_tmp3,OP_NOP,0
loc_58B81:
		LDA		_game_msg_wnd_nt_frame_data,Y
		INY
		EXTQMOVA
		DEC		_tmp3
		BNE		loc_58B81
		DO_WRAP	_tmp3,_tmp4,loc_58BB0
		EXTQMOVB	_ptr0+1,OP_AND,#$E0
		JMP		loc_58B80
loc_58BB0:
		EXTQEND
		RTS

_game_msg_wnd_nt_ofs_list:
		.BYTE	$00,$19,$19,$32,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$64
_game_msg_wnd_nt_frame_data:
		.BYTE	$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58
		.BYTE	$58,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$58
		.BYTE	$58,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$58
		.BYTE	$58,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$58
		.BYTE	$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58
_game_msg_wnd_extnt_frame_data:
		.BYTE	$C0,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C2
		.BYTE	$D0,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$D2
		.BYTE	$D0,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$D2
		.BYTE	$D0,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$D2
		.BYTE	$E0,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E2

; =============== S U B	R O U T	I N E =======================================
_msg_wnd_cheat_test:
		LDA		_pad0_held						; on the cheat screen, hold
		ORA		_pad1_prev_held					; A+B+U+D+L+R combination
		AND		#$CF							; using any buttons on any pads
		CMP		#$CF							; since we can't press all D-Pad
		BNE		loc_59BA9						; on single pad at once
		LDA		_debug_scroll_msg_flags			; when done, first flag is set
		ORA		#$01
		STA		_debug_scroll_msg_flags
		RTS
loc_59BA9:
		LDA		_pad0_held						; here we get if no buttons are
		AND		_pad1_prev_held					; pressed or its wrong combination
		BEQ		locret_59BBF					; skip idle case, test only if
		LDA		_pad0_held						; pressed something
		CMP		_pad1_prev_held					; now compare pads held on c1 and c2
		BEQ		locret_59BBF					; if they are the same, skip
		BCC		locret_59BBF					; if c1 bumber lower, skip
		LDA		_debug_scroll_msg_flags			; or set another flag
		ORA		#$02
		STA		_debug_scroll_msg_flags
locret_59BBF:
		RTS

; =============== S U B	R O U T	I N E =======================================
_debug_flag_toggle_test:
		LDA		_debug_scroll_msg_flags
		CMP		#$03
		BNE		locret_59BCF
		LDA		_city._game_flags
		EOR		#$10
		STA		_city._game_flags
locret_59BCF:
		RTS

		SECTION_ENDS C01, "PRGC EXT GAME MSG HANDLERS"

		SECTION_START 301

; ---------------------------------------------------------------------------
; ---------------------S-R-A-M--I-N-I-T--R-O-U-T-I-N-E-S---------------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
_sram_reset:
		JSR		_sram_write_enable
		INC		_sram_data_crc
		JSR		_sram_write_disable
		JMP		RESET

; =============== S U B	R O U T	I N E =======================================
_sram_sum_reinit:
		JSR		_sram_sum_calc
		MOVW	_sram_data_crc,_tmp2
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_init:
		JSR		_sram_write_enable
		JSR		_sram_sum_calc
		LDA		_sram_data_crc
		CMP		_tmp2
		BNE		loc_59C0E
		LDA		_sram_data_crc+1
		CMP		_tmp3
		BNE		loc_59C0E
		LDA		_sram_vars._scenarios_clear_flags
		STA		_city._scenarios_clear_flags
		LDA		_sram_vars._all_scenarios_are_clear_ram_flag
		STA		_city._all_scenarios_are_clear_ram_flag
		LDA		_sram_has_saved_city_data_flag
		STA		_has_saved_city_data_flag
		JMP		_sram_write_disable
loc_59C0E:
		LDA		#>[_land_val_map]			; CAUTION: low nibble is zero
		STA		_ptr0+1							; here
		LDA		#<[_land_val_map]
		STA		_ptr0
		STA		_has_saved_city_data_flag		; zero goes here as well
		TAY
loc_59C1A:
		STA		(_ptr0),Y
		INY
		BNE		loc_59C1A
		INC		_ptr0+1
		LDX		_ptr0+1
		CPX		#$A0
		BNE		loc_59C1A
		INC		_sram_data_crc
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_sram_sum_calc:
		LDY		#$00
		STY		_tmp4
		STY		_tmp5
		STY		_tmp6
		STY		_tmp2
		STY		_tmp3
		STY		_ptr0
		LDA		#>[_sram_graphs]				; for sram data only excluding crc
		STA		_ptr0+1
		LDY		#<[_sram_graphs]
loc_59C3F:
		LDA		(_ptr0),Y
		JSR		_sum_calc
		INY
		BNE		loc_59C3F
		INC		_ptr0+1
		LDA		_ptr0+1
		CMP		#$A0							; end of sram buffer, start of A000 ROM area
		BNE		loc_59C3F
		LDA		#$00
		JSR		_sum_calc
		LDA		#$00

; =============== S U B	R O U T	I N E =======================================
_sum_calc:
		STA		_tmp6
		LDA		_tmp2
		STA		_tmp4
		LDA		_tmp3
		STA		_tmp5
		LDX		#$00
loc_59C62:
		LSR		_tmp6
		ROR		_tmp5
		ROR		_tmp4
		BCC		loc_59C76
		LDA		_tmp4
		EOR		#$05
		STA		_tmp4
		LDA		_tmp5
		EOR		#$C0
		STA		_tmp5
loc_59C76:
		INX
		CPX		#$08
		BNE		loc_59C62
		LDA		_tmp4
		STA		_tmp2
		LDA		_tmp5
		STA		_tmp3
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_data_load:
		DEC		_mmc5_operation_in_progress_flag
		JSR		_sram_write_enable
		MOVWO	_ptr0,_sram_city_map
		MOVWO	_ptr2,_city_map
		LDY		#$00
loc_5915A:
		LDA		(_ptr0),Y
		STA		(_ptr2),Y
		INCW	_ptr0
		INCW	_ptr2
		LDA		_tmp2
		CMP		#<[_city_map_end]
		BNE		loc_5915A
		LDA		_tmp3
		CMP		#>[_city_map_end]
		BNE		loc_5915A
		LDX		#$00
loc_59178:
		LDA		_sram_graphs,X
		STA		_graphs,X
		INX
		CPX		#_graphs_size
		BNE		loc_59178
		LDX		#$00
loc_59185:
		LDA		_sram_vars,X
		STA		_city,X
		INX
		CPX		#_city_size
		BNE		loc_59185
		JSR		_sram_write_disable
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_data_save:
		DEC		_mmc5_operation_in_progress_flag
		JSR		_sram_write_enable
		MOVWO	_ptr0,_city_map
		MOVWO	_ptr2,_sram_city_map
		LDY		#$00
loc_58FD3:
		LDA		(_ptr0),Y
		STA		(_ptr2),Y
		INCW	_ptr0
		INCW	_ptr2
		LDA		_ptr0
		CMP		#<[_city_map_end]
		BNE		loc_58FD3
		LDA		_ptr0+1
		CMP		#>[_city_map_end]
		BNE		loc_58FD3
		LDX		#$00
loc_58FF1:
		LDA		_graphs,X
		STA		_sram_graphs,X
		INX
		CPX		#_graphs_size
		BNE		loc_58FF1
; FIX, store current wnd position, can't use _city member directly
; because _cur_map_wnd_pos used frequently and has to be in zero page
		MOVW	_city._wnd_pos,_cur_map_wnd_pos
;
		LDX		#$00
loc_58FFE:
		LDA		_city,X
		STA		_sram_vars,X
		INX
		CPX		#_city_size
		BNE		loc_58FFE
		LDA		#$01
		STA		_has_saved_city_data_flag
		STA		_sram_has_saved_city_data_flag
		JSR		_sram_sum_reinit
		JSR		_sram_write_disable
		INC		_mmc5_operation_in_progress_flag
		RTS

		SECTION_ENDS	301, "SRAM LIBRARY ROUTINES"

; ---------------------------------------------------------------------------
; -----------------H-U-D--S-C-R-O-L-L-E-R--H-A-N-D-L-E-R-S-------------------
; ---------------------------------------------------------------------------

		SECTION_START F03

; =============== S U B	R O U T	I N E =======================================
_scroll_msg_char_output:
		LDA		_scr_mode_idx
		BEQ		loc_58CDB
		RTS
loc_58CDB:
		LDA		_scroll_msg_start_pos
		BNE		loc_58CE6
		STA		_scroll_msg_end_char
		STA		_scroll_msg_shift_pos
loc_58CE6:
		LDA		_scroll_msg_shift_pos
		AND		#$07
		BEQ		loc_58D0D
		ADDB	_scroll_msg_shift_pos,#$02
		LDA		_scroll_msg_start_pos
		LDX		_scroll_msg_idx
		CMP		_scroll_msg_end_pos,X
		BCC		locret_58D0C
		LDA		#$00
		STA		_scroll_msg_active_flag
		STA		_scroll_msg_start_pos
		STA		_debug_scroll_msg_flags
locret_58D0C:
		RTS
loc_58D0D:
		LDA		_debug_scroll_msg_flags			; hacky-hacky
		CMP		#$03
		BNE		loc_58D30
		LDA		_city._game_flags
		AND		#$10
		BEQ		loc_58D21
;		LDA		#<[_scroll_txt_i_am_god]		; REDUNDANT, why won't we have these message
;		LDX		#>[_scroll_txt_i_am_god]		; in a common library as well?
		LDA		#$19
		BNE		loc_58D25
loc_58D21:
;		LDA		#<[_scroll_txt_debug_off]
;		LDX		#>[_scroll_txt_debug_off]
		LDA		#$1A
loc_58D25:
;		STA		_ptr15							; REDUNDANT actualy, the message size here is not
;		STX		_ptr15+1						; identical to scroll_msg_idx == 1, but
;		LDA		#$01							; it's not really important, since the rest
		STA		_scroll_msg_idx					; of line filled with spaces in any case
;		BNE		loc_58D3D
loc_58D30:
		LDX		_scroll_msg_idx
		LDA		_scroll_msg_lib_lo,X
		STA		_ptr15
		LDA		_scroll_msg_lib_hi,X
		STA		_ptr15+1
loc_58D3D:
		LDA		_cur_screen_tile._ROW			; TODO: CIADDB without store!
		LSR
		CLC
		ADC		#$1B
		CMP		#$1E
		BCC		loc_58D49
		SBC		#$1E
loc_58D49:
		LDX		#$20
		MULXA
		STA		_tmp17
		LDA		_scroll_msg_start_pos
		AND		#$1F
		CLC
		ADC		_tmp17
		INC		_scroll_msg_start_pos
		STA		_tmp17
		TXA
		ADC		#$20
		STA		_ptr18
		LDX		_ppu_queue_pos_tail
		DEX
		DEX
		DEX
		DEX
		STX		_ppu_queue_pos_tail
		LDY		_scroll_msg_end_char
		PPUQMOVB	_tmp17,OP_NOP,0				; draw one char at a time,
		PPUQMOVB	_ptr18,OP_NOP,0				; the rest is up to scroll
		PPUQMOVB	#$01,OP_NOP,0
		LDA		(_ptr15),Y
		CMP		#$FF
		BNE		loc_58D93
		LDA		#$DF
		BNE		loc_58D930
loc_58D93:
		INC		_scroll_msg_end_char
loc_58D930:
		STA		_ppu_queue_buf,X
loc_58D94:
		LDA		_scroll_msg_shift_pos
		CLC
		ADC		#$02
		STA		_scroll_msg_shift_pos
		RTS

; NOTE: the length of lines not actually so much inportant here,
; we may assume all messages are the same max size and the only
; noticeable effect of this is that short and long messages will
; have the same duration on the screen.
;
_scroll_msg_end_pos:
		.BYTE	$3D,$3C,$3C,$33,$36,$33,$3A,$3C,$3C,$41,$43,$43,$48,$49,$3D,$3F
		.BYTE	$33,$32,$2F,$34,$39,$38,$32,$44,$36,$3B,$34

_scroll_msg_lib_lo:
		.BYTE	<[_scroll_txt00_more_residental]
		.BYTE	<[_scroll_txt01_more_commercial]
		.BYTE	<[_scroll_txt02_more_industrial]
		.BYTE	<[_scroll_txt03_more_roads]
		.BYTE	<[_scroll_txt04_inadeqate_rail_system]
		.BYTE	<[_scroll_txt05_build_a_power_plant]
		.BYTE	<[_scroll_txt06_demand_a_stadium]
		.BYTE	<[_scroll_txt07_requires_a_sea_port]
		.BYTE	<[_scroll_txt08_requires_airport]
		.BYTE	<[_scroll_txt09_demand_a_fire_department]
		.BYTE	<[_scroll_txt0A_demand_a_police_department]
		.BYTE	<[_scroll_txt0B_blackouts_reported]
		.BYTE	<[_scroll_txt0C_tax_rate_is_too_high]
		.BYTE	<[_scroll_txt0D_roads_deteriorating]
		.BYTE	<[_scroll_txt0E_fire_department_need_funding]
		.BYTE	<[_scroll_txt0F_police_need_funding]
		.BYTE	<[_scroll_txt10_plane_crashed]
		.BYTE	<[_scroll_txt11_shipwreck_reported]
		.BYTE	<[_scroll_txt12_train_crashed]
		.BYTE	<[_scroll_txt13_heli_crashed]
		.BYTE	<[_scroll_txt14_hi_unemployment_rate]
		.BYTE	<[_scroll_txt15_city_gone_broke]
		.BYTE	<[_scroll_txt16_explosion_detected]
		.BYTE	<[_scroll_txt17_brownouts]
		.BYTE	<[_scroll_txt18_heavy_traffic_repotrted]
		.BYTE	<[_scroll_txt_i_am_god]
		.BYTE	<[_scroll_txt_debug_off]
_scroll_msg_lib_hi:
		.BYTE	>[_scroll_txt00_more_residental]
		.BYTE	>[_scroll_txt01_more_commercial]
		.BYTE	>[_scroll_txt02_more_industrial]
		.BYTE	>[_scroll_txt03_more_roads]
		.BYTE	>[_scroll_txt04_inadeqate_rail_system]
		.BYTE	>[_scroll_txt05_build_a_power_plant]
		.BYTE	>[_scroll_txt06_demand_a_stadium]
		.BYTE	>[_scroll_txt07_requires_a_sea_port]
		.BYTE	>[_scroll_txt08_requires_airport]
		.BYTE	>[_scroll_txt09_demand_a_fire_department]
		.BYTE	>[_scroll_txt0A_demand_a_police_department]
		.BYTE	>[_scroll_txt0B_blackouts_reported]
		.BYTE	>[_scroll_txt0C_tax_rate_is_too_high]
		.BYTE	>[_scroll_txt0D_roads_deteriorating]
		.BYTE	>[_scroll_txt0E_fire_department_need_funding]
		.BYTE	>[_scroll_txt0F_police_need_funding]
		.BYTE	>[_scroll_txt10_plane_crashed]
		.BYTE	>[_scroll_txt11_shipwreck_reported]
		.BYTE	>[_scroll_txt12_train_crashed]
		.BYTE	>[_scroll_txt13_heli_crashed]
		.BYTE	>[_scroll_txt14_hi_unemployment_rate]
		.BYTE	>[_scroll_txt15_city_gone_broke]
		.BYTE	>[_scroll_txt16_explosion_detected]
		.BYTE	>[_scroll_txt17_brownouts]
		.BYTE	>[_scroll_txt18_heavy_traffic_repotrted]
		.BYTE	>[_scroll_txt_i_am_god]
		.BYTE	>[_scroll_txt_debug_off]

_scroll_txt00_more_residental:
		.BYTE	_M,_O,_R,_E,__,_R,_E,_S,_I,_D,_E,_N,_T,_I,_A,_L,__,_Z,_O,_N,_E,_S,__,_N,_E,_E,_D,_E,_D,$FF
_scroll_txt01_more_commercial:
		.BYTE	_M,_O,_R,_E,__,_C,_O,_M,_M,_E,_R,_C,_I,_A,_L,__,_Z,_O,_N,_E,_S,__,_N,_E,_E,_D,_E,_D,$FF
_scroll_txt02_more_industrial:
		.BYTE	_M,_O,_R,_E,__,_I,_N,_D,_U,_S,_T,_R,_I,_A,_L,__,_Z,_O,_N,_E,_S,__,_N,_E,_E,_D,_E,_D,$FF
_scroll_txt03_more_roads:
		.BYTE	_M,_O,_R,_E,__,_R,_O,_A,_D,_S,__,_R,_E,_Q,_U,_I,_R,_E,_D,$FF
_scroll_txt04_inadeqate_rail_system:
		.BYTE	_I,_N,_A,_D,_E,_Q,_U,_A,_T,_E,__,_R,_A,_I,_L,__,_S,_Y,_S,_T,_E,_M,$FF
_scroll_txt05_build_a_power_plant:
		.BYTE	_B,_U,_I,_L,_D,__,_A,__,_P,_O,_W,_E,_R,__,_P,_L,_A,_N,_T,$FF
_scroll_txt06_demand_a_stadium:
		.BYTE	_R,_E,_S,_I,_D,_E,_N,_T,_S,__,_D,_E,_M,_A,_N,_D,__,_A,__,_S,_T,_A,_D,_I,_U,_M,$FF
_scroll_txt07_requires_a_sea_port:
		.BYTE	_I,_N,_D,_U,_S,_T,_R,_Y,__,_R,_E,_Q,_U,_I,_R,_E,_S,__,_A,__,_S,_E,_A,__,_P,_O,_R,_T,$FF
_scroll_txt08_requires_airport:
		.BYTE	_C,_O,_M,_M,_E,_R,_C,_E,__,_R,_E,_Q,_U,_I,_R,_E,_S,__,_A,_N,__,_A,_I,_R,_P,_O,_R,_T,$FF
_scroll_txt09_demand_a_fire_department:
		.BYTE	_C,_I,_T,_I,_Z,_E,_N,_S,__,_D,_E,_M,_A,_N,_D,__,_A,__,_F,_I,_R,_E,__,_D,_E,_P,_A,_R,_T,_M,_E,_N,_T,$FF
_scroll_txt0A_demand_a_police_department:
		.BYTE	_C,_I,_T,_I,_Z,_E,_N,_S,__,_D,_E,_M,_A,_N,_D,__,_A,__,_P,_O,_L,_I,_C,_E,__,_D,_E,_P,_A,_R,_T,_M,_E,_N,_T,$FF
_scroll_txt0B_blackouts_reported:
		.BYTE	_B,_L,_A,_C,_K,_O,_U,_T,_S,__,_R,_E,_P,_O,_R,_T,_E,_D,__,__,_C,_H,_E,_C,_K,__,_P,_O,_W,_E,_R,__,_M,_A,_P,$FF
_scroll_txt0C_tax_rate_is_too_high:
		.BYTE	_C,_I,_T,_I,_Z,_E,_N,_S,__,_U,_P,_S,_E,_T,__,__,_T,_H,_E,__,_T,_A,_X,__,_R,_A,_T,_E,__,_I,_S,__,_T,_O,_O,__,_H,_I,_G,_H,$FF
_scroll_txt0D_roads_deteriorating:
		.BYTE	_R,_O,_A,_D,_S,__,_D,_E,_T,_E,_R,_I,_O,_R,_A,_T,_I,_N,_G,__,__,_D,_U,_E,__,_T,_O,__,_L,_A,_C,_K,__,_O,_F,__,_F,_U,_N,_D,_S,$FF
_scroll_txt0E_fire_department_need_funding:
		.BYTE	_F,_I,_R,_E,__,_D,_E,_P,_A,_R,_T,_M,_E,_N,_T,_S,__,_N,_E,_E,_D,__,_F,_U,_N,_D,_I,_N,_G,$FF
_scroll_txt0F_police_need_funding:
		.BYTE	_P,_O,_L,_I,_C,_E,__,_D,_E,_P,_A,_R,_T,_M,_E,_N,_T,_S,__,_N,_E,_E,_D,__,_F,_U,_N,_D,_I,_N,_G,$FF
_scroll_txt10_plane_crashed:
		.BYTE	_A,__,_P,_L,_A,_N,_E,__,_H,_A,_S,__,_C,_R,_A,_S,_H,_E,_D,$FF
_scroll_txt11_shipwreck_reported:
		.BYTE	_S,_H,_I,_P,_W,_R,_E,_C,_K,__,_R,_E,_P,_O,_R,_T,_E,_D,$FF
_scroll_txt12_train_crashed:
		.BYTE	_A,__,_T,_R,_A,_I,_N,__,_C,_R,_A,_S,_H,_E,_D,$FF
_scroll_txt13_heli_crashed:
		.BYTE	_A,__,_H,_E,_L,_I,_C,_O,_P,_T,_E,_R,__,_C,_R,_A,_S,_H,_E,_D,$FF
_scroll_txt14_hi_unemployment_rate:
		.BYTE	_U,_N,_E,_M,_P,_L,_O,_Y,_M,_E,_N,_T,__,_R,_A,_T,_E,__,_I,_S,__,_H,_I,_G,_H,$FF
_scroll_txt15_city_gone_broke:
		.BYTE	_Y,_O,_U,_R,__,_C,_I,_T,_Y,__,_H,_A,_S,__,_G,_O,_N,_E,__,_B,_R,_O,_K,_E,$FF
_scroll_txt16_explosion_detected:
		.BYTE	_E,_X,_P,_L,_O,_S,_I,_O,_N,__,_D,_E,_T,_E,_C,_T,_E,_D,$FF
_scroll_txt17_brownouts:
		.BYTE	_B,_R,_O,_W,_N,_O,_U,_T,_S,__,__,_B,_U,_I,_L,_D,__,_A,_N,_O,_T,_H,_E,_R,__,_P,_O,_W,_E,_R,__,_P,_L,_A,_N,_T,$FF
_scroll_txt18_heavy_traffic_repotrted:
		.BYTE	_H,_E,_A,_V,_Y,__,_T,_R,_A,_F,_F,_I,_C,__,_R,_E,_P,_O,_R,_T,_E,_D,$FF
_scroll_txt_i_am_god:
		.BYTE	_I,__,_A,_M,__,_G,_O,_D,__,__,__,_I,__,_W,_I,_S,_H,__,_Y,_O,_U,__,_H,_A,_P,_P,_Y,$FF
_scroll_txt_debug_off:
		.BYTE	_I,__,_A,_M,__,_G,_O,_D,__,__,__,_D,_E,_B,_U,_G,__,_O,_F,_F,$FF

		SECTION_ENDS F03, "PRGF EXT HUD SCROLLER"

; ---------------------------------------------------------------------------
; -----------------G-A-M-E--M-E-N-U-S--H-A-N-D-L-E-R-S-----------------------
; ---------------------------------------------------------------------------

		SECTION_START C02

; =============== S U B	R O U T	I N E =======================================
_game_menu_exec:
		TXA
		PHA
		LDA		#$01
		STA		_scr_mode_idx
		JSR		_spr_clear
		JSR		_wait_for_nmi
		PLA
		TAX
		CPX		#$00
		BNE		loc_58F72
;		APUA_SE	_SE_IDX_CLICK					; move into function
;		FJSRA	_game_save_menu,SRAM,PRG3		; REDUNDANT: already enabled sram in FJSR for _game_menu_exec
		JSR		_game_save_menu
		JMP		_game_menu_break
loc_58F72:
		DEX
		BNE		loc_58F7F
;		FJSRA	_game_load_menu,SRAM,PRG3		; REDUNDANT: already enabled sram in FJSR for _game_menu_exec
		JSR		_game_load_menu					; the only sub that can use two different se
		JMP		_game_menu_break
loc_58F7F:
		DEX
		BNE		loc_58F90
;		APUA_SE	_SE_IDX_CLICK					; move into function
;		FJSRA	_game_title_return,SRAM,PRG3	; REDUNDANT: already enabled sram in FJSR for _game_menu_exec
		JSR		_game_title_return
		JMP		_game_menu_break
loc_58F90:
;		APUA_SE	_SE_IDX_CLICK					; move into function
;		FJSRA	_game_quit,SRAM,PRG3			; REDUNDANT: already enabled sram in FJSR for _game_menu_exec
		JSR		_game_quit
_game_menu_break:
		LDA		#$00
		STA		_scr_mode_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_save_menu:
		APUA_SE	_SE_IDX_CLICK
		JSR		_save_menu_exec
		BCS		loc_58FBA
		JSR		_sram_data_save
		JSR		_spr_clear
		JSR		_wait_for_nmi
		FJSRA	_wnd_erase,PRGC,PRGD
		JSR		_game_chr_common_reload
		SEC
		RTS
loc_58FBA:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
;_load_menu_exec:
;		LDA		#$10
;		BNE		_common_yes_no_question_menu

; =============== S U B	R O U T	I N E =======================================
_save_menu_exec:
		LDA		#$0B

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_question_menu:
		STA		_wnd_idx+1
		LDA		#$01
		STA		_wnd_lvl_idx
		LDA		#$09
		STA		_wnd_left_tile+1
		LDA		#$0C
		STA		_wnd_top_tile+1
		LDA		#$80
		STA		_pal_load_lib
		LDA		#$01
		STA		_ppu_pal_dequeue_req
		JSR		_wait_for_nmi
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		LDA		#$00
		STA		_common_yes_no_menu_cursor_pos
		FJSRA	_wnd_draw,PRGC,PRGD
loc_59047:
		LDA		#$00
		STA		_spr_buf_pos
		JSR		_wait_for_nmi
		LDA		#$37
		STA		_MMC5_CHR_BANKSA+3
		JSR		_common_yes_no_menu_cursor_draw
		LDA		_pad0_press
		AND		#$80
		BNE		loc_5907B
		LDA		_pad0_press
		AND		#$40
		BNE		loc_5909C
		LDA		_pad0_press
		AND		#$0F
		BEQ		loc_59047
		APUA_SE	_SE_IDX_SELECT
		LDA		_common_yes_no_menu_cursor_pos
		BEQ		loc_59076
		DEC		_common_yes_no_menu_cursor_pos
		BEQ		loc_59047
loc_59076:
		INC		_common_yes_no_menu_cursor_pos
		BNE		loc_59047
loc_5907B:
		LDA		_common_yes_no_menu_cursor_pos
		BNE		loc_59097
		JSR		_spr_clear
		JSR		_common_yes_no_menu_yes_push_draw
		APUA_SE	_SE_IDX_BCLICK
		LDA		#$18
		STA		_ptr0
loc_5908E:
		JSR		_wait_for_nmi
		DEC		_ptr0
		BNE		loc_5908E
		CLC
		RTS
loc_59097:
		PUSHB	#$00
		BEQ		loc_5909F
loc_5909C:
		PUSHB	#$FF
loc_5909F:
		JSR		_spr_clear
		JSR		_common_yes_no_menu_no_push_draw
		APUA_SE	_SE_IDX_BCLICK
		JSR		_spr_clear
		LDA		#$00
		STA		_mmc5_operation_in_progress_flag
		JSR		_wait_for_nmi
		FJSRA	_wnd_erase,PRGC,PRGD
		JSR		_game_chr_common_reload
		PLA
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_cursor_draw:
		LDA		#$00
		LDX		_common_yes_no_menu_cursor_pos
		BEQ		loc_590C9
		LDA		#$30
loc_590C9:
		CLC
		ADC		#$78
		STA		_spr_insert_args._pos._COL
		LDA		#$90
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._idx
		LDA		#$00
		STA		_spr_insert_args._attr
		JMP		_spr_lib_attr_insert

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_yes_push_draw:
		LDA		#$0C
		STA		_spr_insert_args._idx
		LDA		#$60
		BNE		_common_yes_no_menu_button_push_draw

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_no_push_draw:
		LDA		#$0D
		STA		_spr_insert_args._idx
		LDA		#$90

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_button_push_draw:
		STA		_spr_insert_args._pos._COL
		LDA		#$88
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._attr
		JMP		_spr_lib_attr_insert

; =============== S U B	R O U T	I N E =======================================
_game_load_menu:
		LDA		_has_saved_city_data_flag
		BNE		loc_59103
		APUA_SE	_SE_IDX_ERROR
		BNE		loc_5912E
loc_59103:
		LDA		#$00							; set continue game mode
		STA		_cur_game_mode
		APUA_SE	_SE_IDX_CLICK
;		JSR		_load_menu_exec					; REDUNDANT
		LDA		#$10							; OPTIMIZED: load menu
		JSR		_common_yes_no_question_menu
		BCS		loc_5912E
;		JSR		_sram_data_load					; REDUNDANT, we will load it in a usual
		JSR		_spr_clear						; way in _game_vars_init after restarting
		JSR		_wait_for_nmi
		FJSRA	_wnd_erase,PRGC,PRGD
		JSR		_game_chr_common_reload
		JSR		_pal_fade_out
		TSX										; trick to jump 5 levels up
		TXA										; to exit back from _start_game routine
		CLC										; and do the game loop again starting
		ADC		#$10							; from vars reinitialization
		TAX
		TXS
loc_5912E:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_quit:
		APUA_SE	_SE_IDX_CLICK
		JSR		_save_menu_exec
		BCC		loc_591A1
		CMP		#$FF
		BNE		loc_591B7
		CLC
		RTS
loc_591A1:
		JSR		_sram_data_save
		JSR		_spr_clear
		JSR		_wait_for_nmi
		FJSRA	_wnd_erase,PRGC,PRGD
		JSR		_game_chr_common_reload
		JSR		_pal_fade_out
loc_591B7:
		JMP		_turn_off_power_screen

; =============== S U B	R O U T	I N E =======================================
_game_title_return:
		APUA_SE	_SE_IDX_CLICK
		JSR		_save_menu_exec
		BCC		loc_591C5
		CMP		#$FF
		BNE		loc_591D8
		CLC
		RTS
loc_591C5:
		JSR		_sram_data_save
		JSR		_spr_clear
		JSR		_wait_for_nmi
		FJSRA	_wnd_erase,PRGC,PRGD
		JSR		_game_chr_common_reload
loc_591D8:
		LDA		#$00
		STA		_main_sub_idx					; exit to main sub0, start title
		LDA		#$02							; option called "GOTO MENU"
		STA		_title_sub_idx					; so let's start from menu then!
		JSR		_pal_fade_out
		TSX
		TXA
		CLC
		ADC		#$10
		TAX
		TXS
		RTS

; =============== S U B	R O U T	I N E =======================================
_turn_off_power_screen:
		LDA		#_MUS_IDX_STOP
		STA		_apu_mus_idx_req
		LDA		#$00
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		STA		_ppu_mask_shadow
		STA		_PPU_MASK
		STA		_tmp582
		LDA		#$80
		STA		_game_wnd_spr_mode_flag
		LDA		#$53
		STA		_MMC5_CHR_BANKSA+3
		JSR		_ppu_scr_clear
		JSR		_turn_off_power_draw
		LDA		#$13
		STA		_screen_pal_cur_bg_idx
		LDA		#$0B
		STA		_screen_pal_cur_spr_idx
		LDA		#$80
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		LDA		#$1E
		STA		_ppu_mask_shadow
		STA		_PPU_MASK
		JSR		_pal_fade_in
		JSR		_turn_off_power_animate
loc_59229:
		LDA		#$FF
		STA		_game_core_state
		JSR		_wait_for_nmi
		JSR		_turn_off_power_animate
		JSR		_spr_finish
		JMP		loc_59229

; =============== S U B	R O U T	I N E =======================================
_ppu_scr_clear:
		LDA		#$20
		LDX		#$00
		STA		_PPU_ADDR
		STX		_PPU_ADDR
		LDY		#$1E
		LDA		#$20
loc_59248:
		LDX		#$20
loc_5924A:
		STA		_PPU_DATA
		DEX
		BNE		loc_5924A
		DEY
		BNE		loc_59248
		LDA		#$02
		STA		_MMC5_CHR_MODE
		MOVWO	_ptr0,_MMC5_EXRAM
		LDY		#$00
		LDX		#$04
		LDA		#$59
loc_59266:
		STA		(_ptr0),Y
		INY
		BNE		loc_59266
		DEX
		BEQ		loc_59272
		INC		_ptr0+1
		BNE		loc_59266
loc_59272:
		LDA		#$01
		STA		_MMC5_CHR_MODE
		RTS

; =============== S U B	R O U T	I N E =======================================
_turn_off_power_draw:
		MOVWO	_ptr0,aTurnOffPowerSwitch
		LDY		#$00
		LDA		#$21
		LDX		#$E6
		STA		_PPU_ADDR
		STX		_PPU_ADDR
loc_5928C:
		LDA		(_ptr0),Y
		BMI		loc_59296
		STA		_PPU_DATA
		INY
		BNE		loc_5928C
loc_59296:
		INY
		LDA		#$22
		LDX		#$26
		STA		_PPU_ADDR
		STX		_PPU_ADDR
loc_592A1:
		LDA		(_ptr0),Y
		BMI		locret_592AB
		STA		_PPU_DATA
		INY
		BNE		loc_592A1
locret_592AB:
		RTS

aTurnOffPowerSwitch:
		.BYTE	"Turn off power switch"
		.BYTE	$FF
		.BYTE	"with reset..."
		.BYTE	$FF

; =============== S U B	R O U T	I N E =======================================
_turn_off_power_animate:
		LDA		#$78
		STA		_spr_insert_args._pos._COL
		LDA		#$40
		STA		_spr_insert_args._pos._ROW
		LDA		#$A2
		STA		_spr_insert_args._idx
		JSR		_spr_lib_attr_insert
		LDA		_tmp582
		LDX		#$00
loc_592E4:
		CMP		_off_power_anim_data,X
		BCC		loc_592ED
		INX
		INX
		BCS		loc_592E4
loc_592ED:
		LDA		_off_power_anim_data+1,X
		CMP		#$FF
		BEQ		loc_59301
		STA		_spr_insert_args._idx
		LDA		#$78
		STA		_spr_insert_args._pos._COL
		LDA		#$48
		STA		_spr_insert_args._pos._ROW
		JSR		_spr_lib_attr_insert
loc_59301:
		LDX		_tmp582
		INX
		CPX		#$52
		BCC		loc_5930B
		LDX		#$00
loc_5930B:
		STX		_tmp582
		RTS

_off_power_anim_data:
		.BYTE	$0A,$A3
		.BYTE	$16,$A4
		.BYTE	$24,$A5
		.BYTE	$34,$A6
		.BYTE	$3F,$A7
		.BYTE	$42,$A8
		.BYTE	$52,$FF

		SECTION_ENDS C02, "PRGC EXT GAME MENU ROUTINES"

; ---------------------------------------------------------------------------
; ------N-E-W--C-I-T-Y--M-A-P--A-U-T-O-G-E-N-E-R-A-T-E--R-O-U-T-I-N-E-S------
; ---------------------------------------------------------------------------

		SECTION_START 901

; =============== S U B	R O U T	I N E =======================================
; NOTE, all following code and algos are exactly the same as in Micropolis
; sources, so we may quickly merge these sources with this code
;
_generate_new_map:
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		MOVW	_rand_seed,_new_map_number		; initialize RNG to a specific value
		ROL		_rand_seed						; for every map, so all maps will be
		ROL		_rand_seed+1					; random but every single map generates
		ROL										; exactly the same way
		ROL		_rand_seed
		ROL		_rand_seed+1
		ROL
		ROL		_rand_seed
		ROL		_rand_seed+1
		ROL
		ADC		_new_map_number
		STA		_rand_seed+2
		JSR		_rand
		JSR		_rand
		JSR		_rand
		CMP		#$2B							; 43 from 256 to decide
		BCS		.generate_land					; to generate island map
		JSR		_generate_island
		LDA		#$00
		STA		_mmc5_operation_in_progress_flag
		RTS
.generate_land:
		JSR		_city_map_clear
		JSR		_rand_map_start_pos				; base point for river generation
		JSR		_generate_rivers				; draw rivers with two brushes
		JSR		_generate_lakes					; draw small lakes
		JSR		_smooth_water_shores				; fill shores pixels
		JSR		_fill_water_tiles				; fill actual map tiles for water and shores
		JSR		_generate_woods
		JSR		_fill_wood_tiles
		LDA		#$00
		STA		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_island:
		LDA		#$4B							; not random start
		STA		_map_start_pos._COL
		STA		_map_start_pos._ROW
loc_59379:
		LDA		#$02							; select water tiles if we at the edges
		LDX		_map_start_pos._COL
		CPX		#$02
		BCC		loc_59393
		CPX		#$4A
		BCS		loc_59393
		LDX		_map_start_pos._ROW
		CPX		#$02
		BCC		loc_59393
		CPX		#$4A
		BCS		loc_59393
		LDA		#$00							; and ground tiles in the middle of the map
loc_59393:
		JSR		_map_start_pos_write			; write
		DEC		_map_start_pos._ROW
		BPL		loc_59379
		DEC		_map_start_pos._COL
		BMI		loc_593A8
		LDA		#$4B
		STA		_map_start_pos._ROW
		BNE		loc_59379
loc_593A8:
		LDA		#$00
		STA		_map_start_pos._COL
loc_593AD:
		LDX		#$09
		JSR		_rand_clamp_X					; now distort the island edges with river brush
		STA		_map_pos._ROW
		LDA		_map_start_pos._COL
		STA		_map_pos._COL
		JSR		_put_small_brush				; for the top side rendomly
		LDX		#$09
		JSR		_rand_clamp_X
		STA		_ptr0
		LDA		#$47
		SEC
		SBC		_ptr0
		STA		_map_pos._ROW
		JSR		_put_small_brush				; then for the bottom side randomly
		LDA		#$00
		STA		_map_pos._ROW
		JSR		_put_small_brush				; then completely clear the top line
		LDA		#$48
		STA		_map_pos._ROW					; and bottom line
		JSR		_put_small_brush
		LDX		_map_start_pos._COL
		INX
		INX
		STX		_map_start_pos._COL
		CPX		#$4C
		BCC		loc_593AD
		LDA		#$00
		STA		_map_start_pos._ROW
loc_593F1:
		LDX		#$09
		JSR		_rand_clamp_X					; now do the same for the horizontal sides
		STA		_map_pos._COL
		LDA		_map_start_pos._ROW
		STA		_map_pos._ROW
		JSR		_put_small_brush				; for the left side randomly
		LDX		#$09
		JSR		_rand_clamp_X
		STA		_ptr0
		LDA		#$47
		SEC
		SBC		_ptr0
		STA		_map_pos._COL
		JSR		_put_small_brush				; for the right side randomly
		LDA		#$00
		STA		_map_pos._COL
		JSR		_put_small_brush				; and remove left-right side of the edges
		LDA		#$48
		STA		_map_pos._COL
		JSR		_put_small_brush
		LDX		_map_start_pos._ROW
		INX
		INX
		STX		_map_start_pos._ROW
		CPX		#$4C
		BCC		loc_593F1
		JSR		_smooth_water_shores			; same water smooth, woods, then woods smooth
		JSR		_fill_water_tiles				; procedures
		JSR		_generate_woods
		JMP		_fill_wood_tiles				; and fill finally ann actual tiles

; =============== S U B	R O U T	I N E =======================================
_map_start_pos_write:
		PHA
		BUFPOS	_ptr0,_map_start_pos._COL,_map_start_pos._ROW,#$4C,#>[_city_map]	; DST=(_map_start_pos._ROW*#$4C+_map_start_pos._COL)+#>[_city_map]00
		LDY		#$00
		PLA
		STA		(_ptr0),Y
		RTS

; OPTIMIZED: optimized function used more than once here
;
; =============== S U B	R O U T	I N E =======================================
_map_tmp_pos_read_ptr0:
		BUFPOS	_ptr0,_map_tmp_pos._COL,_map_tmp_pos._ROW,#$4C,#>[_city_map]	; DST=(_map_tmp_pos._ROW*#$4C+_map_tmp_pos._COL)+#>[_city_map]00
		LDY		#$00
		LDA		(_ptr0),Y
		RTS

; OPTIMIZED: optimized function used more than once here
;
; =============== S U B	R O U T	I N E =======================================
_map_tmp_pos_read_ptr2:
		BUFPOS	_ptr2,_map_tmp_pos._COL,_map_tmp_pos._ROW,#$4C,#>[_city_map]	; DST=(_map_tmp_pos._ROW*#$4C+_map_tmp_pos._COL)+#>[_city_map]00
		LDY		#$00
		LDA		(_ptr2),Y
		RTS

; OPTIMIZED: optimized function used more than once here
;
; =============== S U B	R O U T	I N E =======================================
_map_pos_read:
		BUFPOS	_ptr0,_map_pos._COL,_map_pos._ROW,#$4C,#>[_city_map]	; DST=(_map_pos._ROW*#$4C+_map_pos._COL)+#>[_city_map]00
		LDY		#$00
		LDA		(_ptr0),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_map_clear:
		MOVWO	_ptr0,_city_map
		LDY		#$00
loc_5946A:
		LDA		#$00
		STA		(_ptr0),Y
		INCW	_ptr0
		LDA		_ptr0+1
		CMP		#>[_city_map_end]
		BCC		loc_5946A
		LDA		_ptr0
		CMP		#<[_city_map_end]
		BCC		loc_5946A
		RTS

; =============== S U B	R O U T	I N E =======================================
_rand_map_start_pos:
		LDX		#$19
		JSR		_rand_clamp_X
		CLC
		ADC		#$1A
		STA		_map_start_pos._COL
		STA		_map_pos._COL
		LDX		#$19
		JSR		_rand_clamp_X
		CLC
		ADC		#$1A
		STA		_map_start_pos._ROW
		STA		_map_pos._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_rivers:
		LDX		#$04
		JSR		_rand_clamp_X					; select main direction
		STA		_map_last_dir
		STA		_map_dir
		JSR		_generate_b_river				; draw flow to the edge of the screen
		MOVW	_map_pos,_map_start_pos			; return to start pos
		LDA		_map_last_dir					; insert direction
		EOR		#$04
		STA		_map_last_dir
		STA		_map_dir
		JSR		_generate_b_river				; draw opposite flow
		MOVW	_map_pos,_map_start_pos
		LDX		#$04
		JSR		_rand_clamp_X
		STA		_map_last_dir
		JMP		_generate_s_river				; generate small branch


; =============== S U B	R O U T	I N E =======================================
_generate_b_river:
		JSR		_map_pos_test_bounds			; stop if we reach the map bounds
		BCS		locret_5950D
		JSR		_put_big_brush					; draw round brush of river
; NOTE, the original code does increment or decrement _map_dir only if this
; test fails, and if passed does _map_dir=_map_last_dir only instead.
; here we see the same result, but redundant call of all three _rand
; test one by one..
; However, if we change the order, we change all maps here, because of
; differnt RNG flow from now...
		JSR		_rand							; test for change direction
		AND		#$01
		BEQ		loc_594F0
		INC		_map_dir
loc_594F0:
		JSR		_rand
		AND		#$01
		BEQ		loc_594FA
		DEC		_map_dir
loc_594FA:
		JSR		_rand
		CMP		#$18
		BCS		loc_59507						; test for follow direction
		LDA		_map_last_dir
		STA		_map_dir
loc_59507:
		JSR		_map_pos_move					; move current pos to selected direction
		JMP		_generate_b_river
locret_5950D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_s_river:
		JSR		_map_pos_test_bounds			; the same method as for b river
		BCS		locret_5953D
		JSR		_put_small_brush				; but woth small brush
		JSR		_rand
		AND		#$01
		BEQ		loc_59520
		INC		_map_dir
loc_59520:
		JSR		_rand
		AND		#$01
		BEQ		loc_5952A
		DEC		_map_dir
loc_5952A:
		JSR		_rand
		CMP		#$14
		BCS		loc_59537
		LDA		_map_last_dir
		STA		_map_dir
loc_59537:
		JSR		_map_pos_move
		JMP		_generate_s_river
locret_5953D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_woods:
		LDX		#$32							; random select of amount of woods
		JSR		_rand_clamp_X					; counter = rand(50) + 23 (half of the
		CLC										; Micropolis version)
		ADC		#$17
		STA		_tmp57C
loc_59549:
		LDX		#$4C							; choose random position on the map for
		JSR		_rand_clamp_X					; every wood
		STA		_map_pos._COL
		LDX		#$4C
		JSR		_rand_clamp_X
		STA		_map_pos._ROW
		JSR		_put_woods
		DEC		_tmp57C
		BNE		loc_59549
		JSR		_smooth_woods_edges				; smooth edges two times in row, to
		JMP		_smooth_woods_edges				; make it smoother


; =============== S U B	R O U T	I N E =======================================
_put_woods:
		LDX		#$3C							; select size for current woods splash
		JSR		_rand_clamp_X
		CLC
		ADC		#$17
		STA		_tmp57D
		LDA		_map_pos._COL					; set the starting point
		STA		_map_tmp_pos._COL
		LDA		_map_pos._ROW
		STA		_map_tmp_pos._ROW
loc_5957F:
		LDX		#$08
		JSR		_rand_clamp_X					; randomly select the direction
		STA		_map_dir
		JSR		_map_tmp_pos_move				; then move pen to it
		JSR		_map_tmp_pos_bounds_test		; test bounds
		BCS		locret_595BA
		JSR		_map_tmp_pos_read_ptr0			; OPTIMIZED: read current tile
		BNE		loc_595B5						; put only on zero pixels, ie ground tiles
		LDA		#$05
		STA		(_ptr0),Y						; put default square wood pixel
loc_595B5:
		DEC		_tmp57D							; repeat
		BNE		loc_5957F
locret_595BA:
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_lakes:
		LDX		#$05							; rand position and type of lakes
		JSR		_rand_clamp_X
		TAX
		INX
		STX		_tmp580							; nimber of lakes
loc_595C5:
		LDX		#$3E
		JSR		_rand_clamp_X					; position on the map
		CLC										; inside map except edges
		ADC		#$03
		STA		_map_pos._COL
		LDX		#$3E
		JSR		_rand_clamp_X
		CLC
		ADC		#$03
		STA		_map_pos._ROW
		LDX		#$06
		JSR		_rand_clamp_X
		ADC		#$01
		STA		_tmp581							; number of brushes to draw
loc_595E5:
		LDX		#$07
		JSR		_rand_clamp_X					; now randomly select relative
		SEC										; position of brush to put
		SBC		#$03
		CLC
		ADC		_map_pos._COL
		STA		_map_tmp_pos._COL
		LDX		#$07
		JSR		_rand_clamp_X
		SEC
		SBC		#$03
		CLC
		ADC		_map_pos._ROW
		STA		_map_tmp_pos._ROW
		JSR		_rand
		CMP		#$34
		BCC		loc_59610						; and finally select what brush
		JSR		_put_small_brush				; to put
		JMP		loc_59613
loc_59610:
		JSR		_put_big_brush
loc_59613:
		DEC		_tmp581							; do brushes loop
		BNE		loc_595E5
		DEC		_tmp580
		BNE		loc_595C5						; do lakes loop
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_tmp_pos_move:
		LDA		_map_dir
		AND		#$07
		STA		_map_dir
		LDX		_map_dir
		LDA		_map_tmp_pos._COL
		CLC
		ADC		_move_dir_list0,X
		STA		_map_tmp_pos._COL
		LDA		_map_tmp_pos._ROW
		CLC
		ADC		_move_dir_list1,X
		STA		_map_tmp_pos._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_pos_move:
		LDA		_map_dir
		AND		#$07
		STA		_map_dir
		LDX		_map_dir
		LDA		_map_pos._COL
		CLC
		ADC		_move_dir_list0,X
		STA		_map_pos._COL
		LDA		_map_pos._ROW
		CLC
		ADC		_move_dir_list1,X
		STA		_map_pos._ROW
		RTS

_move_dir_list0:
		.BYTE	$00,$01,$01,$01,$00,$FF,$FF,$FF
_move_dir_list1:
		.BYTE	$FF,$FF,$00,$01,$01,$01,$00,$FF

; =============== S U B	R O U T	I N E =======================================
_put_big_brush:
		LDX		#$00							; fast single counter
		STX		_tmp579							; instead of using i,j for matix
loc_59673:
;		LDX		_tmp579							; REDUNDANT
		LDA		_map_pos._COL					; fast matrix positions calculate
		CLC
		ADC		_big_brush_matrix_col_tbl,X		; table of columns
		STA		_map_tmp_pos._COL
		LDA		_map_pos._ROW
		CLC
		ADC		_big_brush_matrix_row_tbl,X		; table of rows
		STA		_map_tmp_pos._ROW
		LDA		_big_brush_matrix,X				; then fetch matrix value
		BEQ		loc_59692
		JSR		_put_map_pixel					; draw only non-zero pixels
loc_59692:
		INC		_tmp579
;		INX										; REDUNDANT
		LDX		_tmp579
		CPX		#$31
		BCC		loc_59673
		RTS

_big_brush_matrix:
		.BYTE	$00,$00,$03,$03,$03,$00,$00
		.BYTE	$00,$03,$02,$02,$02,$03,$00
		.BYTE	$03,$02,$02,$02,$02,$02,$03
		.BYTE	$03,$02,$02,$04,$02,$02,$03
		.BYTE	$03,$02,$02,$02,$02,$02,$03
		.BYTE	$00,$03,$02,$02,$02,$03,$00
		.BYTE	$00,$00,$03,$03,$03,$00,$00
_big_brush_matrix_col_tbl:
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
_big_brush_matrix_row_tbl:
		.BYTE	$00,$00,$00,$00,$00,$00,$00
		.BYTE	$01,$01,$01,$01,$01,$01,$01
		.BYTE	$02,$02,$02,$02,$02,$02,$02
		.BYTE	$03,$03,$03,$03,$03,$03,$03
		.BYTE	$04,$04,$04,$04,$04,$04,$04
		.BYTE	$05,$05,$05,$05,$05,$05,$05
		.BYTE	$06,$06,$06,$06,$06,$06,$06

; =============== S U B	R O U T	I N E =======================================
_put_small_brush:
		LDA		#$00							; the same as for bbig brush
		STA		_tmp57A							; but less pixels in brush
loc_59736:
		LDX		_tmp57A
		LDA		_map_pos._COL
		CLC
		ADC		_small_brush_matrix_col_tbl,X
		STA		_map_tmp_pos._COL
		LDA		_map_pos._ROW
		CLC
		ADC		_small_brush_matrix_row_tbl,X
		STA		_map_tmp_pos._ROW
		LDA		_small_brush_matrix,X			; NOTE, there is no ship routes
		BEQ		loc_59755
		JSR		_put_map_pixel
loc_59755:
		LDX		_tmp57A
		INX
		STX		_tmp57A
		CPX		#$10
		BCC		loc_59736
		RTS

_small_brush_matrix:
		.BYTE	$00,$03,$03,$00
		.BYTE	$03,$02,$02,$03
		.BYTE	$03,$02,$02,$03
		.BYTE	$00,$03,$03,$00
_small_brush_matrix_col_tbl:
		.BYTE	$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03
_small_brush_matrix_row_tbl:
		.BYTE	$00,$00,$00,$00
		.BYTE	$01,$01,$01,$01
		.BYTE	$02,$02,$02,$02
		.BYTE	$03,$03,$03,$03

; =============== S U B	R O U T	I N E =======================================
_put_map_pixel:
		PHA										; store brush pixel for now
		JSR		_map_tmp_pos_bounds_test		; test if we not run out of map
		BCS		.map_tile_skip
		JSR		_map_tmp_pos_read_ptr0			; OPTIMIZED: size
		BEQ		.put_map_tile					; read current tile from map
		CMP		#$04
		BEQ		.map_tile_skip					; if 4 (ship route), don't touch it
		CMP		#$02
		BNE		.put_map_tile					; if 3 (ground), replace it
		PLA
		CMP		#$04							; if 2 (river), then write only ship
		BEQ		.put_ship_route_pixel			; routes and skip the rest
		RTS
.map_tile_skip:
		PLA
		RTS
.put_map_tile:
		PLA
.put_ship_route_pixel:
		STA		(_ptr0),Y						; _ptr0 already points to current tile, refresh
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_tmp_pos_bounds_test:
		LDA		_map_tmp_pos._COL
		CMP		#$4C
		BCS		loc_597DD
		LDA		_map_tmp_pos._ROW
		CMP		#$4C
		BCS		loc_597DD
		CLC
		RTS
loc_597DD:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_pos_test_bounds:
		LDA		_map_pos._COL
		BPL		loc_597EA
		CMP		#$FC							; test map bounds -4 to the left
		BCC		loc_59800
		BCS		loc_597EE
loc_597EA:
		CMP		#$4A							; test map bounds -2 to the right
		BCS		loc_59800
loc_597EE:
		LDA		_map_pos._ROW
		BPL		loc_597F9
		CMP		#$FC							; test map bounds -4 to the top
		BCC		loc_59800
		CLC
		RTS
loc_597F9:
		CMP		#$4A							; test map bounds -2 to the bottom
		BCS		loc_59800
		BCS		loc_59800
		RTS
loc_59800:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_smooth_woods_edges:
		LDA		#$00							; mostly same as shores smooth
		STA		_map_pos._COL
		STA		_map_pos._ROW
.woods_edges_loop:
		JSR		_map_pos_read					; OPTIMIZED size
		BMI		.woods_next_pixel				; skip already filled water tiles
		BEQ		.woods_next_pixel				; and empty grounds, the rest are woods
		LDY		#$00							; FIX, the same copypaste code as for
		STY		_tmp57B							; sea shores
;		STY		_tmp4
.woods_area_loop:
;		LDY		_tmp4							; end loop test comes first correct here
;		CPY		#$04							; then read current nearest tile
;		BCS		.put_woods_pixel				; let do the same as for other sub
		STY		_tmp4
		LDA		_map_pos._COL
		CLC
		ADC		_shores_delta_col,Y
		STA		_map_tmp_pos._COL
		LDA		_map_pos._ROW
		CLC
		ADC		_shores_delta_row,Y
		STA		_map_tmp_pos._ROW
		JSR		_map_tmp_pos_bounds_test		; test for bounds and nothing more extra actions!
		BCS		loc_5987B						; as against the sea shores, they don't revert
		JSR		_map_tmp_pos_read_ptr2			; the direction here and just skip the pixel
		BMI		loc_5987B						; as in the original Micropolis code.
		BEQ		loc_5987B
		SEC
		BCS		loc_5987C
loc_5987B:
		CLC
loc_5987C:
		ROL		_tmp57B
		LDY		_tmp4							; FIX, the same code as for rivers
		INY
		CPY		#$04
		BNE		.woods_area_loop
.put_woods_pixel:
		LDX		_tmp57B
		LDA		_woods_edges_matrix,X
		LDY		#$00
		STA		(_ptr0),Y
.woods_next_pixel:
		INC		_map_pos._COL
		LDA		_map_pos._COL
		CMP		#$4C
		BCC		.woods_edges_loop
		INC		_map_pos._ROW
		LDA		_map_pos._ROW
		CMP		#$4C
		BCS		locret_598AC
		LDA		#$00
		STA		_map_pos._COL
		BEQ		.woods_edges_loop
locret_598AC:
		RTS

_woods_edges_matrix:
		.BYTE	$00,$00,$00,$00					; same table as for shores, but with woods
		.BYTE	$00,$16,$10,$13					; tile set pixels $10-$18
		.BYTE	$00,$18,$12,$15
		.BYTE	$00,$17,$11,$14

; =============== S U B	R O U T	I N E =======================================
_smooth_water_shores:
		LDA		#$00
		STA		_map_pos._COL					; scan full map
		STA		_map_pos._ROW
.shores_map_loop:
		JSR		_map_pos_read					; OPTIMIZED size
		CMP		#$03							; here we read the map pixel, _tmp0
;		BEQ		.water_shore_found				; points to current pixel
;		JMP		.shores_next_pixel
		BNE		.shores_next_pixel				; FIX, the short is enough
;.water_shore_found:
		LDY		#$00							; FIX, now 0 for sure
		STY		_tmp57B
.shores_area_loop:
		STY		_tmp4							; FIX, here we always have current Y to store
;		CPY		#$04							; REDUNDANT, moved to the loop end
;		BCC		.do_shores_area
;		JMP		.put_shores_pixel
;.do_shores_area:
		LDA		_map_pos._COL					; select area position
		CLC
		ADC		_shores_delta_col,Y
		STA		_map_tmp_pos._COL
		LDA		_map_pos._ROW
		CLC
		ADC		_shores_delta_row,Y
		STA		_map_tmp_pos._ROW
		JSR		_map_tmp_pos_bounds_test		; test for bounds
		BCC		loc_59957						; if ok then read the tile
		LDY		_tmp4							; otherwize, use altrnate direction
		LDA		_shores_delta_col,Y				; NOTE, this is a different behaviour
		NEGA									; from the Micropolis code. they don't
		CLC										; bounce in negative direction, but
		ADC		_map_pos._COL					; just skip the tile
		STA		_map_tmp_pos._COL				; if not use this, sometimes the shores
		LDA		_shores_delta_row,Y				; calculated incorrectly!
		NEGA
		CLC
		ADC		_map_pos._ROW
		STA		_map_tmp_pos._ROW
		JSR		_map_tmp_pos_read_ptr2			; OPTIMIZED size
		BNE		loc_5997C						; now read nearest tile and
		SEC										; and check is it ground or not
		BCS		loc_5997D
loc_59957:
		JSR		_map_tmp_pos_read_ptr2			; OPTIMIZED size
		BEQ		loc_5997C
		SEC
		BCS		loc_5997D
loc_5997C:
		CLC
loc_5997D:
		ROL		_tmp57B							; put bit into smother tile selection
; FIX,
		LDY		_tmp4
		INY
		CPY		#$04
; -
; BUG, this fix alone were redundant here seems they put counter test to the loop start
; but forgot to remove this jump... now we have test here, and remove it from top of
; the loop, and let this branch stay here
;		BEQ		.put_shores_pixel
;		JMP		.shores_area_loop
		BNE		.shores_area_loop				; FIX, short enough too
.put_shores_pixel:
		LDX		_tmp57B
		LDA		_shores_matrix,X				; now read smooth table by index we calculated
		LDY		#$00
		STA		(_ptr0),Y
.shores_next_pixel:
		INC		_map_pos._COL
		LDA		_map_pos._COL
		CMP		#$4C
		BCS		loc_5999E
		JMP		.shores_map_loop
loc_5999E:
		INC		_map_pos._ROW
		LDA		_map_pos._ROW
		CMP		#$4C
		BCS		locret_599B0
		LDA		#$00
		STA		_map_pos._COL
		JMP		.shores_map_loop
locret_599B0:
		RTS
; 3
;0*1
; 2
_shores_delta_col:					; 0   1   2   3   4   5   6   7
		.BYTE	$FF,$01,$00,$00		; G   G   G   G   G   G   G   G
_shores_delta_row:					;G*G N*G G*N N*N G*G N*G G*N N*N
		.BYTE	$00,$00,$01,$FF		; G   G   G   G   N   N   N   N
_shores_matrix:						;00  00  00  07  00  15  10  13
		.BYTE	$00,$00,$00,$07		; 0   1   2   3   4   5   6   7
		.BYTE	$00,$15,$10,$13		; N   N   N   N   N   N   N   N
		.BYTE	$00,$17,$12,$14		;G*G N*G G*N N*N G*G N*G G*N N*N
		.BYTE	$07,$16,$11,$07		; G   G   G   G   N   N   N   N
									;00  17  12  14  07  17  11  07

; =============== S U B	R O U T	I N E =======================================
_fill_wood_tiles:
		MOVWO	_ptr0,_city_map					; same as for rivers
		LDY		#$00
loc_599D3:
		LDA		(_ptr0),Y						; skip already filled rivers tiles
		BMI		loc_599E8
		BEQ		loc_599FB						; for ground set the default ground tile
		CLC
		ADC		#[_MAP_WOODS0-$10]				; the same two ranges for woods $BE + $10 / $19
		PHA										; _MAP_WOODS0 - _MAP_WOODS8
		JSR		_rand							; _MAP_WOODS9 - _MAP_WOODS11
		LSR
		PLA
		BCS		loc_599E6
		ADC		#$09
loc_599E6:
		STA		(_ptr0),Y
loc_599E8:
		INCW	_ptr0
		LDA		_ptr0+1
		CMP		#>[_city_map_end]
		BCC		loc_599D3
		LDA		_ptr0
		CMP		#<[_city_map_end]
		BCC		loc_599D3
		RTS
loc_599FB:
		LDA		#_MAP_GROUND
		STA		(_ptr0),Y
		BNE		loc_599E8
; REDUNDANT, never executes, the previous BNE is unconditional branch
; there is unknown where should be this default tile set
;
;		LDA		#_MAP_WOODS4
;		STA		(_ptr0),Y
;		BNE		loc_599E8

; =============== S U B	R O U T	I N E =======================================
_fill_water_tiles:
		MOVWO	_ptr0,_city_map					; for all pixels, replace them with actual tiles
		LDY		#$00
loc_59A11:
		LDA		(_ptr0),Y
;		BEQ		.put_clean_ground				; REDUNDANT
		BEQ		.end_of_fill_test				; OPTIMIZED
		CMP		#$02
		BEQ		.put_water
		CMP		#$07
		BEQ		.put_water
		CMP		#$04
		BEQ		.put_ship_route_tile
		CLC
		ADC		#[_MAP_SHORE0-$10]				; for the sea shores, select one of
		PHA										; two different types randomly
		JSR		_rand							; $AE + $10 to $17 -> $BE to $C5 (_MAP_SHORE0-_MAP_SHORE7)
		LSR										; $AE + $18 to $1F -> $C6 to $CD (_MAP_SHORE8-_MAP_SHOREF)
		PLA
		BCS		loc_59A2E						; if fails, put tile as is
		ADC		#$08
loc_59A2E:
		STA		(_ptr0),Y
.end_of_fill_test:
		INCW	_ptr0
		LDA		_ptr0+1
		CMP		#>[_city_map_end]
		BCC		loc_59A11
		LDA		_ptr0
		CMP		#<[_city_map_end]
		BCC		loc_59A11
		RTS
;.put_clean_ground:
;		LDA		#_MAP_GROUND					; REDUNDANT, zero pixels does not fill here!
;		BNE		.end_of_fill_test				; this load is redundant.
.put_water:
		LDA		#_MAP_WATER
		STA		(_ptr0),Y
		BNE		.end_of_fill_test
.put_ship_route_tile:
		LDA		#_MAP_WATER_ROUTE
		STA		(_ptr0),Y
		BNE		.end_of_fill_test

; =============== S U B	R O U T	I N E =======================================
_rand_clamp_X:
		JSR		_rand
		MULAX
		RTS

		SECTION_ENDS 901, "PRG9 EXT MAP GENERATOR"

		BANK_END F3,$C000
