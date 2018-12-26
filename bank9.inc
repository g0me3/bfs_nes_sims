
		ORG $A000

		BANK_START $F9

; ---------------------------------------------------------------------------
; -----M-A-I-B--S-C-R-E-E-N--R-E-S-O-U-R-C-E-S--C-O-D-E--A-N-D--D-A-T-A------
; ---------------------------------------------------------------------------

		SECTION_START	C10

; FIX, moved from PRG7 to the local place
; -
; =============== S U B	R O U T	I N E =======================================
_scr_chr_vars_init:
		LDA		_ppu_ctrl_shadow				; set scroll high bits to nt 2400
		ORA		#$01
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		LDA		#$84							; switch mmc5 to one-screen mode with
		STA		_mmc5_nt_mode_shadow			; exnt mapped to nt 2C00
		STA		_MMC5_NT_MODE
		LDA		_scr_res_idx					; _scr_res_idx now = _scr_res_idx*2  be careful!
		ASL
		STA		_scr_res_idx
		ASL
		TAX
		LDY		#$00
loc_A001C:
		LDA		_screen_chr_banks_lib,X			; load CHR banks for sprites
		BEQ		loc_A0024						; $00 means - no change bank
		STA		_MMC5_CHR_BANKSA,Y
loc_A0024:
		INX
		INY
		CPY		#$04
		BNE		loc_A001C
		LDA		#$00							; reset screen specific vars
		STA		_mini_map_cur_row
		STA		_mini_map_cur_col
		STA		_max_tax_rate+1
		STA		_game_msg_wnd_active_flag
		LDA		#$14
		STA		_max_tax_rate					; hardcoded tax maximum = 20
		LDA		#$80
		STA		_game_wnd_spr_mode_flag
		STA		_game_core_state
		STA		_scr_res_control_flags
;		JMP		_screen_pal_spr_init			; REDUNDANT
;
; =============== S U B	R O U T	I N E =======================================
;_screen_pal_spr_init:
		LDA		_scr_res_idx
		LSR
		TAX
		LDA		_screen_pal_spr_list,X			; load spr pal idx
		STA		_screen_pal_cur_spr_idx
		RTS

_screen_pal_spr_list:
		.BYTE	$03,$08,$02,$02,$03,$03,$06,$04,$00
_screen_chr_banks_lib:
		.BYTE	$34,$00,$36,$00
		.BYTE	$34,$00,$39,$00
		.BYTE	$34,$5D,$36,$37
		.BYTE	$34,$00,$00,$00
		.BYTE	$34,$00,$00,$00
		.BYTE	$34,$00,$00,$00
		.BYTE	$34,$00,$00,$37
		.BYTE	$34,$00,$1A,$37

; REDUNDANT, unreferenced copy of the routine below
;
; =============== S U B	R O U T	I N E =======================================
;_screen_pal_bg_init_unref:
;		LDA		_scr_res_idx
;		AND		#$FE
;		BNE		loc_A00A7
;		LDY		_cur_info_maps_button_idx
;		LDA		_screen_pal_extra_list,Y
;		JMP		loc_A00AC
;loc_A00A7:
;		LSR
;		TAY
;		LDA		_screen_pal_list,Y
;loc_A00AC:
;		STA		_screen_pal_cur_bg_idx
;		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_pal_bg_load:
		LDA		#$02
		STA		_ppu_pal_dequeue_req			; only bg queue flag
		LDA		_scr_res_idx
		AND		#$FE
		BNE		loc_A0080
		LDY		_cur_info_maps_button_idx		; for infom maps pal differs by info type
		LDA		_screen_pal_extra_list,Y
		JMP		loc_A0085
loc_A0080:
		LSR										; the rest is always the same
		TAY
		LDA		_screen_pal_list,Y
loc_A0085:
		STA		_screen_pal_cur_bg_idx
		JMP		_pal_load_lib					; load pal immediately

_screen_pal_list:
		.BYTE	$00,$04,$03,$02,$0B,$12,$06,$0A
_screen_pal_extra_list:
		.BYTE	$05,$05,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01

; =============== S U B	R O U T	I N E =======================================
_scr_res_wnd_close:
		LDX		#$FF
		STX		_constr_area_update_req			; map area redraw request
		INX
		STX		_gui_active_controls_flags		; clear gui flags, request to redraw
;		STX		_update_rect_left				; REDUNDANT, redraw top-leftmost area of map
;		STX		_update_rect_top				; which is not correct here
		STX		_scr_res_control_flags
		STX		_game_core_stop_flag
		STX		_scr_res_idx					; FIX BUGS#19
;		INX										; REDUNDANT, one more leftover
;		LDA		#$18							; REDUNDANT, continue, these paremerers
;		STA		_update_rect_right				; are set in another routine
;		LDA		#$14							; and calculated from cur screen pos
;		STA		_update_rect_bottom
		LDA		#$44							; NOTE: always the same cursor pos
		STA		_cur_game_cursor_scr_pos._COL	; after exit any screen resourse
		LDA		#$30
		STA		_cur_game_cursor_scr_pos._ROW
		JSR		_buffers_wait_flush				; wait for ppu queues if any
		JSR		_pal_fade_out					; fade out
		LDA		_ppu_ctrl_shadow				; restore regular game ppo modes
		AND		#$FC
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		LDA		#$00
		STA		_MMC5_NT_MODE
		STA		_game_wnd_spr_mode_flag
		STA		_scr_res_window_opened_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_common:
		JSR		_thread_lock_flag_set			; enter THREAD1-safe critical section
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		JSR		_spr_clear
;		FJSRA	_scr_chr_vars_init,PRG6,PRG9	; REDUNDANT
		JSR		_scr_chr_vars_init				; FIX, now local
		JSR		_wait_for_nmi
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		JSR		_wait_for_nmi
		FJSRA	_scr_common_draw,PRG6,PRG9		; static tlm draw
; NOTE
; for some reason attempting to use a regular handler system with no extra checks
; lead to screen garbling... all functions although uses ppu queue buffers,
; flushes them manually to ppu, so all these functions should be called
; when render is off. neither of any code here does its own render off stuff
;
; in order to remove these functions from here and inserting them into main
; sub list you need to remove all force flush code and make the nmi code do its
; work instead
;
		JSR		_scr_res_hndl2_A_budget_draw
		JSR		_scr_res_hndl3_A_city_eval_draw
;		JSR		_far_scr_res_hndl5_A_history_draw		; REDUNDANT
		FJSRA	_scr_res_hndl5_A_history_draw,PRG7,PRG9	; OPTIMIZED
;		JSR		_far_scr_res_hndl6_A_bank_draw			; REDUNDANT
		FJSRA	_scr_res_hndl6_A_bank_draw,PRG7,PRG9	; OPTIMIZED
;-
		LDA		_scr_res_idx					; currently _scr_res_idx = _scr_res_idx*2
		LSR
		TAY
		LDA		_scr_res_ctlr_flag_list,Y		; preload control flags
		STA		_scr_res_control_flags
		JSR		_ppu_reset
		JSR		_wait_for_nmi
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#$01
		STA		_scr_res_window_opened_flag
		JSR		_pal_fade_in					; show the actual screen
.scr_res_loop:
		JSR		_wait_for_nmi
;		FJSRA	_wnd_common_cursor_spr_clear,PRG7,PRG9	; REDUNDANT
		JSR		_wnd_common_cursor_spr_clear	; OPTIMIZED, now local
		JSR		_spr_finish
;		FJSRA	_scr_res_cursor_move,PRG6,PRG9	; REDUNDANT, common cursor handlers
		JSR		_scr_res_cursor_move			; OPTIMIZED, now local
;		FJSRA	_scr_res_cursor_draw,PRG6,PRG9	; REDUNDANT, common cursor draw
		JSR		_scr_res_cursor_draw			; OPTIMIZED, now local
		JSR		_scr_res_hndl_common_exec		; execure screen handler
		BIT		_scr_res_control_flags
		BVC		.scr_res_quick_warp_test		; cancel command test
		APUA_SE	_SE_IDX_CLICK					; exit from screen
;		FJSRA	_scr_res_wnd_close,PRG6,PRG9	; REDUNDANT
;		RTS
		JMP		_scr_res_wnd_close				; OPTIMIZED, now local
.scr_res_quick_warp_test:
		LDA		_scr_res_control_flags			; for warp from one screen to another
		CMP		#$10							; used in budget to banks screen jump
		BNE		.scr_res_loop
		JMP		_scr_res_common

_scr_res_ctlr_flag_list:
		.BYTE	$00,$00,$20,$08,$08,$08,$08,$A8

; OPTIMIZED, local now
; =============== S U B	R O U T	I N E =======================================
_scr_res_cursor_move:
		LDA		_scr_res_control_flags			; for resources with a free cursor
		AND		#$20							; like graphs and info maps
		BNE		locret_A0323
		LDA		_pad0_held						; test for D-Pad movements only
		AND		#$0F
		BNE		loc_A0324
locret_A0323:
		RTS
loc_A0324:
		LDX		#$00
		LDY		#$03
		LDA		_pad0_held						; fetch held buttons, keep in A
loc_A032A:
		LSR
		BCC		loc_A0341						; shift d-pad bits in order R, L, U, D
		PHA										; save d-pad bits for now
		LDA		_cur_game_cursor_scr_pos,X		; get cur pos
		CLC
		ADC		_scr_res_cursor_speed,Y			; increment position for 4 directions
		CMP		_scr_res_cursor_min_pos_list,X	; test for limits
		BCC		loc_A0340
		CMP		_scr_res_cursor_max_pos_list,X
		BCS		loc_A0340
		STA		_cur_game_cursor_scr_pos,X		; if not at the side of the screen, store
loc_A0340:
		PLA										; restore d-pad bits
loc_A0341:
		CPY		#$02							; Y is dpad pos idx, 0,1 - hor, 2,3 - vert
		BNE		loc_A0346						; so test if we should switch X to Y
		INX										; switch once at Y 02
loc_A0346:
		DEY
		BPL		loc_A032A						; loop until all 4 dpad tested
		LDA		#$00
		STA		_cur_game_cursor_area			; not used the same way as ingame
		RTS

; all directions has it's own speed just for convinience to correspond
; to the d-pad index counter
;
_scr_res_cursor_speed:
		.BYTE	$FE,$02,$FE,$02
_scr_res_cursor_min_pos_list:
		.BYTE	$08,$18
_scr_res_cursor_max_pos_list:
		.BYTE	$E9,$D9

; =============== S U B	R O U T	I N E =======================================
_scr_res_cursor_draw:
		LDA		_scr_res_control_flags
		AND		#$08
		BNE		locret_A0360
		LDA		_spr_buf_pos					; draw new cursor always from 0
		BEQ		_scr_res_cursor_spr_insert_ex	; if already 0, jump directly
		PHA										; if no, save old position
		JSR		_scr_res_cursor_spr_insert_ex_0	; draw and return here
		POPB	_spr_buf_pos					; then restore
locret_A0360:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_cursor_spr_insert_ex_0:
		LDA		#$00
		STA		_spr_buf_pos					; FIX if already 0, no need to store

; =============== S U B	R O U T	I N E =======================================
_scr_res_cursor_spr_insert_ex:
		STA		_spr_insert_args._idx			; in any cases A here is 0
		STA		_spr_insert_args._attr
		LDA		_cur_game_cursor_scr_pos._COL
		STA		_spr_insert_args._pos._COL
		LDA		_cur_game_cursor_scr_pos._ROW
		STA		_spr_insert_args._pos._ROW
		JMP		_spr_lib_attr_insert
; -

; =============== S U B	R O U T	I N E =======================================
_wnd_common_cursor_spr_clear:
		LDX		#$0F
		LDA		#$FA
loc_B97FC:
		STA		_spr_buf,X
		DEX
		BPL		loc_B97FC
		RTS

; =============== S U B	R O U T	I N E =======================================
; there are two handlers for every event. one is for init/draw, the second is
; for input and interact. some init/draw routines doesn't call from this array
; they call from the main code instead, because they can draw only when render is
; off and flushes its buffers directly to PPU.
; this probably were done in order to draw the screen at once without waiting
; but some other screens don't do that, like overall info screen. unless other
; screens where all the numbers should be drawn before the screen shows, this
; one draw numbers one by one after the screen shown...
;
_scr_res_hndl_common_exec:
		LDA		_scr_res_idx
		JSR		_switch
		.WORD	_scr_res_hndl0_A_info_maps_draw, 	_scr_res_hndl0_B_info_maps_input
		.WORD	_far_scr_res_hndl1_A_graphs_draw,	_far_scr_res_hndl1_B_graphs_input
		.WORD	0,									_far_scr_res_hndl2_B_budget_input
		.WORD	0,									_scr_res_hndl3_B_city_eval_input
		.WORD	_scr_res_hndl4_A_city_overview_init,_scr_res_hndl4_B_city_overview_input
		.WORD	0,									_scr_res_hndl5_B_history_input
		.WORD	0,									_far_scr_res_hndl6_B_bank_input
		.WORD	_scr_res_hndl7_A_new_city_gen_init,	_scr_res_hndl7_B_new_city_gen_input

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl7_A_new_city_gen_init:
		LDA		#$00
		STA		_mini_map_cur_col				; this special screen called from
		STA		_mini_map_cur_row				; _game_setup functions, not from game itself
		STA		_new_city_screen_cur_row_idx
		STA		_new_city_screen_cur_col_idx
		STA		_cur_info_maps_button_idx
		STA		_info_maps_menu_extra_idx
		STA		_tmp6D0+1
		INC		_scr_res_idx
		LDA		#$10
		STA		_tmp6D0
;		JMP		_new_city_map_generate			; REDUNDANT

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_new_city_map_generate:
		JSR		_new_city_screen_spr_draw		; put regular sprites
		JSR		_mini_map_clear_draw			; clear map line by line
;		FJSRA	_new_city_please_wait_spr_draw,PRG7,PRG9	; REDUNDANT
		JSR		_new_city_please_wait_spr_draw	; OPTIMIZED, local now
		JSR		_wait_for_nmi
		JSR		_sram_write_enable
		FJSRA	_generate_new_map,SRAM,PRG3		; do actual generation, lock other activity
		LDA		#$80
		STA		_scr_res_control_flags			; done, go to the input part
		LDA		#$00
		STA		_new_city_map_regen_req
;		JMP		_new_city_screen_spr_draw		; REDUNDANT, already drawn above
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl7_B_new_city_gen_input:
		LDA		#$80
		STA		_game_core_state
		JSR		_new_city_screen_cursor_draw
		BIT		_scr_res_control_flags			; map update request flag test
		BPL		.no_mimi_map_redraw
		JSR		_new_city_screen_spr_draw		; draw cur sprite while we busy
loc_EA0EC:
;		JSR		_wait_for_nmi					; actually there is already such test inside
		JSR		_mini_map_fill_block			; draw mini-map block by block
		BIT		_scr_res_control_flags			; while flag is set
		BMI		loc_EA0EC
		LDA		#$20
		STA		_scr_res_control_flags			; exit, when done
		RTS
.no_mimi_map_redraw:
		BIT		_pad0_autorep_press				; press B test
		BVC		.no_b_pressed
		LDA		#$40							; exit to title
		STA		_scr_res_control_flags
		LDA		#$02
		STA		_title_sub_idx
		RTS
.no_b_pressed:
		BMI		.button_a_pressed				; test if A pressed instead
		BIT		_pad0_autorep_release			; or autorep falling edge
		BMI		.button_a_pressed
		JSR		_new_city_screen_cursor_move	; move cursor then, C=1 if moved
		BCS		.new_city_defalut_darw
		LDA		_new_city_map_regen_req			; test if any map number controls changed
		BEQ		.new_city_defalut_darw
		DEC		_new_city_map_regen_delay		; no regen immediately, wait
		BNE		.new_city_defalut_darw
		JMP		_new_city_map_generate			; if no activyty for a long time, do regen
.button_a_pressed:
		LDA		_new_city_screen_cur_row_idx
		BNE		loc_EA12C						; test if next map button ressed
		JMP		_new_city_next_map_push			; draw push button
loc_EA12C:
		CMP		#$01
		BNE		loc_EA138						; test if start game button pressed
		LDA		#$00							; this is hacky way to tell _game_setup
		STA		_title_sub_idx					; routine that we need to play game or back to title
		JMP		_new_city_start_new_game_push	; draw push button
loc_EA138:
		JMP		_new_city_map_num_change_push	; draw push map change buttons
.new_city_defalut_darw:
		JMP		_new_city_screen_spr_draw

; =============== S U B	R O U T	I N E =======================================
_new_city_screen_cursor_draw:					; draws a cursor at a fixed
		LDA		_new_city_screen_cur_row_idx	; positions on the screen controls
		CMP		#$02
		BCC		.get_spr_pos					; upper buttons, fixed pos
		BEQ		.get_up_arrow_pos				; idx=A=2 already
		CLC										; A>2, then idx = A+2
		ADC		#$02
.get_up_arrow_pos:
		CLC
		ADC		_new_city_screen_cur_col_idx	; get col idx
.get_spr_pos:
		ASL
		TAY
		LDA		_new_city_cur_pos_list,Y		; fixed cursor positions from the list
		STA		_tmp6CE
		LDA		_new_city_cur_pos_list+1,Y
		STA		_tmp6CC
		FJSRA	_common_select_cursor_control,SRAM,PRG1
		RTS

_new_city_cur_pos_list:
		.BYTE	$48,$E0		; new city gen pos
		.BYTE	$68,$E0		; start game pos
		.BYTE	$D2,$E2		; up arrow 0
		.BYTE	$D2,$DA		; up arrow 1
		.BYTE	$D2,$D2		; up arrow 2
		.BYTE	$DA,$E2		; down arrow 0
		.BYTE	$DA,$DA		; down arrow 1
		.BYTE	$DA,$D2		; down arrow 2

; =============== S U B	R O U T	I N E =======================================
_new_city_screen_cursor_move:
		LDA		_pad0_autorep_press
		AND		#$0F
		BEQ		.no_cursor_moved				; if any dpad pressed
		APUX_SE	_SE_IDX_SELECT					; play sound
		AND		#$03
		BNE		.hor_movements
		LDA		_pad0_autorep_release			; vert movements here
		CMP		#$08
		BNE		.up_movements
		DEC		_new_city_screen_cur_row_idx	; dec twice to not to use jmp instead
		DEC		_new_city_screen_cur_row_idx	; dummy dec, because we inc immediately
.up_movements:
		INC		_new_city_screen_cur_row_idx	; inc if needed
		LDA		_new_city_screen_cur_row_idx	; wrap around
		AND		#$03
		STA		_new_city_screen_cur_row_idx
		BPL		.cursor_moved					; always S=0 here, unconditional jump
.hor_movements:
		LDA		_new_city_screen_cur_row_idx
		CMP		#$02
		BCC		.no_cursor_moved				; no left-right movements for upper buttons
		LDA		_pad0_autorep_release
		LSR
		BCC		.left_movements
		DEC		_new_city_screen_cur_col_idx	; the same for hor idxes
		DEC		_new_city_screen_cur_col_idx
.left_movements:
		INC		_new_city_screen_cur_col_idx
		LDY		_new_city_screen_cur_col_idx	; wrap around 3 using table
		BPL		loc_EA181						; if S=1, then wrap to 2
		LDY		#$02
loc_EA181:
		LDA		_wrap_3_table,Y
		STA		_new_city_screen_cur_col_idx
.cursor_moved:
		SEC
		RTS
.no_cursor_moved:
		CLC
		RTS

_wrap_3_table:
		.BYTE	$00,$01,$02,$00

; =============== S U B	R O U T	I N E =======================================
_new_city_start_new_game_push:
		APUA_SE	_SE_IDX_SELECT					; play se
		LDA		_new_city_map_regen_req			; wait for map fully redrawn
		BNE		locret_EA1F3					; actually dows not need here
;		MOVWO	_ptr0,_tlm_nt_new_game_push		; REDUNDANT, merged in lib routine
;		MOVWO	_tmp2,_tlm_extnt_new_game_push
;		LDA		#$FF
;		JSR		_tlm_queue						; queue push button tilemaps
		LDX		#$14							; OPTIMIZED
		JSR		_tlm_lib_queue					; -
		JSR		_new_city_screen_spr_draw
		LDX		#$08							; delay for 8 frames
loc_EA1E8:
		JSR		_wait_for_nmi
		DEX
		BNE		loc_EA1E8
		LDA		#$40
		STA		_scr_res_control_flags			; set exit flag
locret_EA1F3:
		RTS

; =============== S U B	R O U T	I N E =======================================
_new_city_map_num_change_push:
		APUA_SE	_SE_IDX_SBUILD					; the same sound as for small buildings
		LDX		#$00
		LDA		_new_city_screen_cur_row_idx
		CMP		#$02
		BEQ		loc_EA203						; change what, up or...
		LDX		#$80							; change down
loc_EA203:
		STX		_new_city_button_to_change_flags
		LDY		_new_city_screen_cur_col_idx
		LDA		_new_city_map_change_idx_list,Y
		ORA		_new_city_button_to_change_flags; generate mask of button to change
		STA		_new_city_button_to_change_flags
		LDA		#$80
		STA		_new_city_map_regen_delay		; reload delay, map number is changed
		STA		_new_city_map_regen_req			; set flag to redraw map
;		FJSRA	_new_city_map_num_button_update,PRG7,PRG9	; REDUNDANT, draws dummy sprite, no need here
		MOVW	_ptr0,_new_map_number			; hex to dec
		JSR		_hex_to_dec9999
		LDY		_new_city_screen_cur_col_idx	; do map number change decimally
		BIT		_new_city_button_to_change_flags; by decimal digit, Y=digit idx
		BPL		loc_EA242						; _tmp0-_tmp2 contains a decimal number
		LDA		_ptr0,Y
		BNE		loc_EA23D
		LDA		#$0A
loc_EA23D:
		SEC
		SBC		#$01
		BPL		loc_EA24E
loc_EA242:
		LDA		_ptr0,Y
		CMP		#$09
		BNE		loc_EA24B
		LDA		#$FF
loc_EA24B:
		CLC
		ADC		#$01
loc_EA24E:
		STA		_ptr0,Y
		LDA		_tmp2							; convert decomal to hex back
		ASL
		TAY
		LDA		_num_hundreds,Y
		CLC
		ADC		_ptr0
		STA		_ptr0
		LDA		_num_hundreds+1,Y
		ADC		#$00
		STA		_tmp3							; temporary higher hex nubble of map num
		LDY		_ptr0+1
		LDA		_num_decals,Y
		CLC
		ADC		_ptr0
		STA		_ptr0							; _tmo0-lower hex nubble
		STA		_new_map_number					; update hex map number
		LDA		_tmp3							; in case there are overflow bits,
		ADC		#$00							; increment it to hi nibble
		STA		_ptr0+1							; now _tmp1 full hight nibble, _ptr0 - 16bit hex
		STA		_new_map_number+1				; update hex map number hi nibble
		JSR		_hex_to_dec9999					; now CONVERT TO DECIMAL ONE MORE TIME LOL
		JSR		_new_city_map_num_spr_insert	; because we need to draw decimal but we erase it already
		LDA		#$F2							; BUG, see, this is tiles for pressed
		BIT		_new_city_button_to_change_flags; number selection arrows, but
		BPL		loc_EA289						; ...
		LDA		#$F7							; here is NO button update code!
loc_EA289:
; FIX. now arrow buttons are animated! they probably haven't enabled it here
; because of previously it was an FJSRA call (see below) which is changes
; content of ram vars _tmp0 to _tmp6 (saves the temporary vars there)
; since we moved this routine here and made it local, there is no more
; var changes, so we can use it now and all working as intended
; -
		JSR		_new_city_map_num_button_update
; -
		JSR		_new_city_map_num_roll_anim		; after press we do rolling number anim here
		LDA		#$F1							; and restore button back, unpush!
		BIT		_new_city_button_to_change_flags
		BPL		loc_EA295
		LDA		#$F6
loc_EA295:
;		FJSRA	_new_city_map_num_button_update,PRG7,PRG9	; here is the proper routine called!
		JSR		_new_city_map_num_button_update	; FIX, now local, draw unpressed button back
		RTS

_new_city_map_change_idx_list:
		.BYTE	$01,$02,$04
_num_hundreds:
		.WORD	0,100,200,300,400,500,600,700,800,900
_num_decals:
		.BYTE	0, 10, 20, 30, 40, 50, 60, 70, 80, 90

; =============== S U B	R O U T	I N E =======================================
; updates one particular button tile with a custom one. seems planned to animate
; pushes of the buttons, but writes always the same values.
; moved from PRG7, now local here
;
_new_city_map_num_button_update:
		PHA
		LDY		#$FF							; valid flags 04 02 01
		BIT		_new_city_button_to_change_flags;             84 82 81
		BPL		loc_B96AB
		LDY		#$02
loc_B96AB:
		LDA		_new_city_button_to_change_flags
loc_B96AE:
		LSR										; calc spr buf pos idx
		INY
		BCC		loc_B96AE
		LDA		_new_city_ctrl_spr_pos_list,Y
		TAX
		PLA
		STA		_spr_buf,X						; set the map num select button tile
		RTS

_new_city_ctrl_spr_pos_list:
		.BYTE	$1D,$21,$25
		.BYTE	$11,$15,$19

; =============== S U B	R O U T	I N E =======================================
_new_city_next_map_push:
		APUA_SE	_SE_IDX_SELECT					; play se
		LDA		_new_city_map_regen_req			; if we already pending a new map
		BNE		.next_skip						; skip this part
;		MOVWO	_ptr0,_tlm_nt_next_map_push
;		MOVWO	_tmp2,_tlm_extnt_next_map_push
;		LDA		#$FF
;		JSR		_tlm_queue						; draw push button
		LDX		#$18
		JSR		_tlm_lib_queue
; BUG, here we have a problem, after push we wait for nmi and
; at least for one frame we have no sprites shown here. so
; when you press the button, the bottom controls will blink
; FIX, if we remove this frame, then the buffers will flush on the
; number roll animations below without blinking the sprites itself
;
;		JSR		_wait_for_nmi					; FIX, removed
		LDA		_new_map_number					; now increment map number,
		CLC										; store it to tmp0
		ADC		#$01
		STA		_ptr0
		LDA		_new_map_number+1
		ADC		#$00
		STA		_ptr0+1
		CMP		#$03
		BCC		.do_next_map					; check rough if less than 999
		BNE		loc_EA321						; REDUNDANT, more than 1024, never possible here actually
		LDA		_ptr0							; more than exactly 999
		CMP		#$E8
		BCC		.do_next_map
loc_EA321:
		LDA		#$00							; clamp back to 0
		STA		_ptr0+1
		STA		_ptr0
.do_next_map:
		MOVW	_new_map_number,_ptr0			; store map number
		JSR		_hex_to_dec9999					; also hex to dec it
		LDA		#$01							; masks for buttons to roll
		LDX		_ptr0							; all positive because increment only
		BNE		loc_EA342
		LDA		#$03
		LDX		_ptr0+1
		BNE		loc_EA342
		LDA		#$07
loc_EA342:
		STA		_new_city_button_to_change_flags
		JSR		_new_city_map_num_spr_insert
		JSR		_new_city_map_num_roll_anim		; animate change
		JSR		_new_city_map_generate
;		MOVWO	_ptr0,_tlm_nt_next_map_release	; REDUNDANT, merged
;		MOVWO	_tmp2,_tlm_extnt_netx_map_release
;		LDA		#$FF
;		JMP		_tlm_queue
		LDX		#$1C							; OPTIMIZED
		JMP		_tlm_lib_queue					; draw release button again
.next_skip:
		JMP		_new_city_screen_spr_draw

_tlm_nt_next_map_push:
		.BYTE	$F9,$24,$04,$AF,$AF,$AF,$AF
		.BYTE	$19,$25,$04,$ED,$DE,$EE,$FD
		.BYTE	$39,$25,$03,$FE,$AD,$B9
		.BYTE	$FF
_tlm_extnt_next_map_push:
		.BYTE	$19,$5D,$04,$0A,$0A,$0A,$0A
		.BYTE	$39,$5D,$03,$0A,$0A,$0A
		.BYTE	$FF
_tlm_nt_next_map_release:
		.BYTE	$F9,$24,$04,$A9,$A9,$A9,$A9
		.BYTE	$19,$25,$04,$4E,$45,$58,$54
		.BYTE	$39,$25,$03,$4D,$41,$50
		.BYTE	$FF
_tlm_extnt_netx_map_release:
		.BYTE	$19,$5D,$04,$8B,$8B,$8B,$8B
		.BYTE	$39,$5D,$03,$8B,$8B,$8B
		.BYTE	$FF
_tlm_nt_new_game_push:
		.BYTE	$79,$25,$04,$AF,$AF,$AF,$AF
		.BYTE	$99,$25,$04,$AB,$AC,$AD,$AE
		.BYTE	$B9,$25,$04,$9E,$AD,$9F,$DE
		.BYTE	$FF
_tlm_extnt_new_game_push:
		.BYTE	$99,$5D,$04,$0A,$0A,$0A,$0A
		.BYTE	$B9,$5D,$04,$0A,$0A,$0A,$0A
		.BYTE	$FF

; REDUNDANT, merged with similar routine in PRG7, then moved it to the system bank
; and converted all calls to the library calls by its indexes
; now we have much shorter calls and all tlm are in one table in system bank
;
; =============== S U B	R O U T	I N E =======================================
; args: _ptr0 - nt data
;       _tmp2 - extnt data
;       A - stop byte for data
;_tlm_queue:
;		STA		byte_15F						; REDUNDANT, custom stop byte stored here
;		PPUQSTART
;		LDY		#$00
;loc_EA36D:
;		LDA		(_ptr0),Y						; first queue nt data
;		CMP		byte_15F						; REDUNDANT, now all data uses the same stop-byte
;		CMP		#$FF
;		BEQ		loc_EA37B
;		PPUQMOVA
;		INY
;		BNE		loc_EA36D
;loc_EA37B:
;		PPUQEND
;		EXTQSTART
;		LDY		#$00
;loc_EA381:
;		LDA		(_tmp2),Y						; then queue extnt data
;		CMP		byte_15F
;		CMP		#$FF
;		BEQ		loc_EA38F
;		EXTQMOVA
;		INY
;		BNE		loc_EA381
;loc_EA38F:
;		EXTQEND
;		RTS

; REDUNDANT, unused
;		.BYTE	$01,$00,$0A,$00,$64,$00,$64,$00

; =============== S U B	R O U T	I N E =======================================
_new_city_screen_spr_draw:
		JSR		_new_city_map_num_spr_insert	; insert default num frame
		MOVW	_ptr0,_new_map_number			; insert actual number
		JSR		_hex_to_dec9999
		LDX		#$50
		LDY		#$02
loc_EA419:
		LDA		#$BE
		JSR		_new_city_map_num_digit_spr_draw
		DEY
		BPL		loc_EA419
		RTS

; =============== S U B	R O U T	I N E =======================================
_new_city_map_num_spr_insert:
		LDX		#$10							; draws map number control
		LDY		#$3B
loc_EA426:
		LDA		_spr_new_city_map_def,Y			; default map number frame sprites
		STA		_spr_buf,X
		INX
		DEY
		BPL		loc_EA426
		STX		_spr_buf_pos
		RTS

; NOTE, sprites drawn here in reverse order, from latest to earliest
; here we may use the regular _spr_raw_insert if move it to the
; system bank
;
_spr_new_city_map_def:
		.BYTE	$D0,$02,$EA,$AE
		.BYTE	$D8,$02,$EB,$AE
		.BYTE	$E0,$02,$EC,$AE
		.BYTE	$D0,$01,$ED,$B6
		.BYTE	$D8,$01,$ED,$B6
		.BYTE	$E0,$01,$EE,$B6
		.BYTE	$D0,$01,$EF,$C6
		.BYTE	$D8,$01,$EF,$C6
		.BYTE	$E0,$01,$F0,$C6
		.BYTE	$D0,$03,$F1,$CE
		.BYTE	$D8,$03,$F1,$CE
		.BYTE	$E0,$03,$F1,$CE
		.BYTE	$D0,$03,$F6,$D6
		.BYTE	$D8,$03,$F6,$D6
		.BYTE	$E0,$03,$F6,$D6

; =============== S U B	R O U T	I N E =======================================
_new_city_map_num_roll_anim:
		LDA		#$BE
		STA		_new_city_digit_spr_y_pos		; default Y pos for map num digit sprites
		LDY		#$02
		LDX		#$50
.num_draw_loop:
		LDA		_new_city_button_to_change_flags; check if button need to be drawn
		AND		_new_city_map_num_bitmasks,Y	; by masking its bit
		BNE		.num_to_animate
		LDA		_new_city_digit_spr_y_pos		; if masked, just draw at the same position
		JSR		_new_city_map_num_digit_spr_draw; draw dummy sprite for not moved digits
		LDA		_new_city_digit_spr_y_pos		; to have two sprites for every digit all the time
		BNE		.num_draw_next					; non-zero value, unconditional jump here
.num_to_animate:
		LDA		_ptr0,Y							; read digit
		PHA										; save current value
		JSR		_new_city_map_old_digit_draw	; calc previous value and draw it as starting digit
		PLA
		STA		_ptr0,Y							; restore new digit value, calc position of new sprite
		BIT		_new_city_button_to_change_flags; roll direction
		BPL		loc_EA467
		LDA		_new_city_digit_spr_y_pos
		SEC
		SBC		#$0C
		BNE		.num_draw_next
loc_EA467:
		LDA		_new_city_digit_spr_y_pos
		CLC
		ADC		#$0C
.num_draw_next:
		JSR		_new_city_map_num_digit_spr_draw; draw new sprite below or above the centered one
		DEY
		BPL		.num_draw_loop
		STX		_spr_buf_pos					; store _spr_buf_pos
		BIT		_new_city_button_to_change_flags
		BPL		.do_scroll_up
.do_scroll_down:
		JSR		_wait_for_nmi
		INC		_new_city_digit_spr_y_pos		; animate position for sprites
		LDA		_new_city_digit_spr_y_pos
		CMP		#$CB
		BEQ		.stop_scroll
		BNE		.do_scrolls
.do_scroll_up:
		JSR		_wait_for_nmi
		DEC		_new_city_digit_spr_y_pos
		LDA		_new_city_digit_spr_y_pos
		CMP		#$B1
		BEQ		.stop_scroll
.do_scrolls:
		LDA		_new_city_button_to_change_flags; extract sprite idx to scroll
		LDY		#$02
		LDX		#$10
.roll_digits:
		LSR
		BCC		loc_EA4B3
		BIT		_new_city_button_to_change_flags; dec/inc direct sprite data
		BMI		loc_EA4AD
		DEC		_spr_buf+$50,X
		DEC		_spr_buf+$54,X
		BNE		loc_EA4B3
loc_EA4AD:
		INC		_spr_buf+$50,X
		INC		_spr_buf+$54,X
loc_EA4B3:
		PHA
		LDA		_num_spr_pos_adjust_list,Y		; switch digit to scroll
		TAX
		PLA
		DEY
		BPL		.roll_digits
		BIT		_new_city_button_to_change_flags
		BPL		.do_scroll_up
		BMI		.do_scroll_down
.stop_scroll:
		LDX		#$68
		STA		_spr_buf_pos					; fixed _spr_buf_pos here
		RTS

_num_spr_pos_adjust_list:
		.BYTE	$00,$00,$08
_new_city_map_num_bitmasks:
		.BYTE	$01,$02,$04

; =============== S U B	R O U T	I N E =======================================
_new_city_map_num_digit_spr_draw:
		STA		_spr_buf,X
		INX
		LDA		_ptr0,Y
		CLC
		ADC		#$E0
		STA		_spr_buf,X
		INX
		LDA		#$00
		STA		_spr_buf,X
		INX
		LDA		_new_city_map_num_digit_row_list,Y
		STA		_spr_buf,X
		INX
		RTS

_new_city_map_num_digit_row_list:
		.BYTE	$E0,$D8,$D0

; =============== S U B	R O U T	I N E =======================================
_new_city_map_old_digit_draw:
		BIT		_new_city_button_to_change_flags; check the direction of change
		BPL		.decrement_digit				; push up
		LDA		_ptr0,Y							; push down
		CMP		#$09
		BNE		loc_EA4F8						; increment or wrap to 0
		LDA		#$FF
loc_EA4F8:
		CLC
		ADC		#$01
		BPL		loc_EA507
.decrement_digit:
		LDA		_ptr0,Y
		SEC
		SBC		#$01
		BPL		loc_EA507						; decrement or wrap to 9
		LDA		#$09
loc_EA507:
		STA		_ptr0,Y							; store new digit
		LDA		_new_city_digit_spr_y_pos
		JMP		_new_city_map_num_digit_spr_draw

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl0_A_info_maps_draw:
		LDA		_cur_map_wnd_pos._ROW			; copy current screen map position
		ASL										; to info map selection rect sprite pos
		CLC
		ADC		#$3C
		STA		_info_maps_rect_pos._ROW
		LDA		_cur_map_wnd_pos._COL
		ASL
		CLC
		ADC		#$45
		STA		_info_maps_rect_pos._COL
		JSR		_info_maps_city_name_hud_draw	; force draw hud names, same as in-game, but
		JSR		_info_maps_date_hud_draw_ex		; without need to recalc ppu offsets
		JSR		_info_maps_popul_hud_draw
		LDA		#$00
		STA		_info_maps_button_pending
		INC		_scr_res_idx
		LDA		#$80
		STA		_scr_res_control_flags
;		LDA		#$0B							; REDUNDANT
		LDA		#$FF							; FIX, another special value on entry
		STA		_prev_info_maps_button_idx		; to prevent button release draw at start
		JMP		_scr_res_hndl0_AB_info_maps_redraw

; NOTE, for some reasons, original logic for this screen allows the cursor
; movements and even the button pushes while current map is drawn, in this
; case the new button number is pending and will be redrawn after the map draw
; process is complete.
; however, when THREAD1 set flag when map is changes, the info map lock
; all controls until it redraw the map again.
; that's weird somehow.
;
; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl0_B_info_maps_input:
		JSR		_info_maps_date_hud_draw		; date update if needed (when requested)
		JSR		_info_maps_popul_hud_draw		; update popul all the time
;		JSR		sub_EADC9						; REDUNDANT
; OPTIMIZED
		LDX		#$10
		STX		_spr_buf_pos					; draw mini-map frame
		FJSRA	_info_maps_spr_rect_draw,PRG7,PRG9
; -
		JSR		_info_maps_spr_arrows_hide		; hide mini-map arrows
		BIT		_scr_res_control_flags			; button A pressed and mini-map redraw in progress
		BPL		.no_mini_map_redraw				; skip if masked
		JMP		_info_maps_mini_map_update		; pending buttons press test here
.no_mini_map_redraw:
		BIT		_info_maps_button_pending
		BPL		.no_buttons_pending
;		BVC		loc_EA563						; REDUNDANT, there is no B button exit here
;		LDA		#$40							; nor skip exit from _info_maps_mini_map_update
;		STA		_scr_res_control_flags			; function, so never executed.
;		JMP		_info_maps_exit
;loc_EA563:
		LDA		_info_maps_button_pending		; if previously pending, read new button idx
		AND		#$0F
		TAX
		LDA		_scr_res_ctrl_flag_list,X		; read busy flag for particular screen
		STA		_scr_res_control_flags			; because some buttons arent allowed to be "pended"
		LDA		#$00
		STA		_info_maps_button_pending		; clear request,
		LDA		_cur_info_maps_button_idx		; store new/prev button idxes
		STA		_prev_info_maps_button_idx
		STX		_cur_info_maps_button_idx		; redraw without buttons, they're already drawn
		JMP		_scr_res_hndl0_AB_info_maps_redraw_ex
.no_buttons_pending:
		LDA		_budget_scr_draw_req			; city engine still active, so fiscal year over
		BEQ		loc_EA588						; may occurs at info screen, do special jump to it
		JMP		_info_maps_budget_screen_warp
loc_EA588:
		LDA		_mini_map_need_redraw_flag		; flag from THREAD1 when values are recalculated
		BNE		loc_EA5BA
		BIT		_pad0_held						; test for B held
		BVC		.idle_test
		JMP		_info_maps_arrows_move			; in this case draw arrows and move them
.idle_test:
		LDA		#$00							; idle state for info maps screen
		STA		_scr_res_control_flags
		STA		_game_core_state
		BIT		_pad0_autorep_press				; regular button press test performed here
		BPL		_info_maps_idle
		APUA_SE	_SE_IDX_BCLICK
		LDA		#$80
		STA		_game_core_state
		JSR		_info_maps_get_hover_button_idx	; check hovered button
		BCC		_info_maps_exit
		LDA		_cur_info_maps_button_idx		; button detected
		STA		_prev_info_maps_button_idx
		STX		_cur_info_maps_button_idx
		JMP		_scr_res_hndl0_AB_info_maps_redraw
loc_EA5BA:
		LDA		#$A0
		STA		_scr_res_control_flags			; set recalc mode flags

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl0_AB_info_maps_redraw:
		FJSRA	_info_maps_button_redraw,PRG7,PRG9	; FIX, now in PRG7

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl0_AB_info_maps_redraw_ex:
		JSR		_thread_lock_flag_set
		JSR		_info_maps_menu_handler			; shows and control menu if needed
		LDA		_scr_res_control_flags			; control flags changes if some action
		BEQ		_info_maps_idle					; on the map were made, so it need to
		JSR		_mini_map_clear_draw_request	; be updated
		JSR		_wait_for_nmi
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		FJSRA	_info_maps_header_and_legend_redraw,PRG7,PRG9	; FIX, now in PRG7
		JMP		_mini_map_fill_block

; =============== S U B	R O U T	I N E =======================================
_info_maps_idle:
		LDA		_pad0_held						; idle routine to take priority
		BEQ		locret_EA5F1					; to the THREAD0 every time button
		LDA		#$80							; is pressed
		STA		_game_core_state
locret_EA5F1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_exit:
		LDA		_info_maps_rect_pos._ROW		; recalc the info map selection frame pos
		SEC										; into the new game map pos
		SBC		#$3C
		LSR
		STA		_cur_map_wnd_pos._ROW
		LDA		_info_maps_rect_pos._COL
		SEC
		SBC		#$45
		LSR
		STA		_cur_map_wnd_pos._COL
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_budget_screen_warp:
		LDA		#$80							; called from info maps mode at the end of year
		STA		_budget_screen_return_to_info_maps_flag
		LDA		#$02
		STA		_scr_res_idx
		LDA		#$10
		STA		_scr_res_control_flags
		JMP		_pal_fade_out

; =============== S U B	R O U T	I N E =======================================
_info_maps_mini_map_update:
		BIT		_pad0_autorep_press				; when map is already drawn, you
		BPL		.no_buttons_redraw				; still able to press other buttons
		LDA		_info_maps_button_pending		; they set pending request and immediately
		BNE		.no_buttons_redraw				; applied after redraw proccess is over
		APUA_SE	_SE_IDX_BCLICK
		PUSHB	_scr_res_control_flags
		JSR		_info_maps_get_hover_button_idx	; calc button hovered currently
		PLA
		BCC		.no_buttons_redraw				; skip if no button pressed
		STA		_scr_res_control_flags			; change cur button
		STX		_ptr0
		BCS		loc_EA637
;		LDX		#$C0							; REDUNDANT, never gets here, C=1 always.
												; seems intended to interrupt map draw or
												; for quick exit, but does not work at all
loc_EA637:
		TXA
		ORA		#$80
		STA		_info_maps_button_pending		; so, if pending buttons pressed, store its idx
		PUSHB	_cur_info_maps_button_idx		; backup current buttons for now
		PUSHB	_prev_info_maps_button_idx
		LDA		_cur_info_maps_button_idx		; redraw pressed/released buttons
		STA		_prev_info_maps_button_idx		; while current map still drawn
		LDA		_ptr0
		STA		_cur_info_maps_button_idx
		FJSRA	_info_maps_button_redraw,PRG7,PRG9	; FIX, now in PRG7
		POPB	_prev_info_maps_button_idx		; restore previous parameters
		POPB	_cur_info_maps_button_idx
.no_buttons_redraw:
		JMP		_mini_map_fill_block			; redraw mini-map

; =============== S U B	R O U T	I N E =======================================
_info_maps_get_hover_button_idx:
		LDY		_cur_game_cursor_scr_pos._ROW	; +---+---+ 40
		CPY		#$40							; |   |0  |
		BCC		.no_button_hover				; |EXT|10-+
		CPY		#$C0							; | B |1  |
		BCS		.no_button_hover				; +---+20-+
		LDX		_cur_game_cursor_scr_pos._COL	; |   |2/3|
		CPX		#$30							; |   +30-+
		BCS		.no_button_hover				; |   |4  |
		CPX		#$10							; |   +40-+
		BCC		.no_button_hover				; |   |5  |
		TYA										; | NO+50-+
		SEC										; |   |6  |
		SBC		#$40							; |   +60-+
		CPX		#$20							; |   |7  |
		LDX		#$FF							; |   +70-+
		BCS		.buttons_area					; |   |9/A|
		CMP		#$20							; +---+80-+ C0
		BCS		.no_button_hover				;10  20  30
		LDX		#$07							; X=08 - exit button pressed
		SEC
.buttons_area:
		INX
		SBC		#$10
		BCS		.buttons_area					; calc button idx (row-$40)/$10
		CPX		#$03
		BCC		loc_EA696						; skip for special case, for multiple
		INX										; menu buttons
		CPX		#$09
		BCC		loc_EA696						; increment above 9
		INX
loc_EA696:
		LDA		_scr_res_ctrl_flag_list,X		; test for press allow flags
		STA		_scr_res_control_flags
		CMP		#$40
		BEQ		.no_button_hover
		SEC										; C=1 button hovered
		RTS
.no_button_hover:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_ppu_pos_queue:
		PHA
		LDA		_mini_map_cur_row
		ASL
		TAY
		LDA		_mini_map_ppu_col_ofs_list,Y
		LDX		_scr_res_idx					; adjust mini-map pos for new city gen screen
		CPX		#$0A
		BCC		loc_EA702
		SBC		#$06
		CLC
loc_EA702:
		ADC		_mini_map_cur_col				; adjust column position of minimap to draw
		STA		_tmp2
		LDA		_mini_map_ppu_col_ofs_list+1,Y
		STA		_tmp3
		PPUQSTART								; push offsets into NT/EXTNT bufs at the same time
		LDY		_extnt_queue_pos_head
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		STA		_extnt_queue_buf,Y
		INX
		INY
		LDA		_tmp3
		STA		_ppu_queue_buf,X
		CLC
		ADC		#$38
		STA		_extnt_queue_buf,Y
		INX
		INY
		PLA
		STA		_ppu_queue_buf,X				; push count of bytes, as arg in A
		STA		_extnt_queue_buf,Y
		STA		_tmp4							; save counter
		INX
		INY
		STX		_tmp2							; save queue pos by now in tmp vars for further use
		STY		_tmp3
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_fill_tile:
		LDA		_cur_info_maps_button_idx		; every type of maps will get its own type of blocks
		JSR		_switch
		.WORD	_mini_map0_draw
		.WORD	_mini_map1_draw
		.WORD	_mini_map2356_draw
		.WORD	_mini_map2356_draw
		.WORD	_mini_map4_draw
		.WORD	_mini_map2356_draw
		.WORD	_mini_map2356_draw
		.WORD	_mini_map2356_draw
		.WORD	_mini_map89_draw
		.WORD	_mini_map89_draw

; =============== S U B	R O U T	I N E =======================================
_mini_map0_draw:
		LDA		_mini_map_cur_row				; calc the wram map offset for
		ASL										; calculated block 4x4
		ASL										; additional mul to have right value below
		TAY
		LDA		_mini_map_cur_col
		ASL
		ASL
;		ADC		_city_mini_map_row_pos_list,Y	; REDUNDANT, table calc for multiply
		ADC		_city_map_row_pos_list_lo,Y		; let's reuse global list, Y already
		STA		_ptr0							; multiplied by 4, so we get the same result
;		LDA		_city_mini_map_row_pos_list+1,Y
		LDA		_city_map_row_pos_list_hi,Y
		ADC		#$00
		STA		_ptr0+1							; now we have ofs to left-top corner of 4x4 area
		JSR		_mini_map_area_info_collect
;		JMP		sub_EA94A						; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;sub_EA94A:
		LDY		#$00
		JSR		sub_EAA5E
		INY
		JSR		sub_EAA5E
		LDA		_tmp118
		BEQ		loc_EA964
		CMP		#$02
		BCC		loc_EA964
		LDA		#$00
		STA		byte_12A
		JMP		loc_EA992
loc_EA964:
		LDA		byte_128
		CMP		#$02
		BCC		loc_EA9A1
		LDA		byte_127
		CMP		byte_128
		BEQ		loc_EA98A
		LSR
		STA		_ptr0
		LDA		byte_127
		SEC
		SBC		byte_128
		CMP		_ptr0
		BCC		loc_EA9A7
		LSR		_ptr0
		LDA		byte_128
		CMP		_ptr0
		BCC		loc_EA992
loc_EA98A:
		LDA		#<[byte_EB5E3]
		STA		_ptr0
		LDA		#>[byte_EB5E3]
		BNE		loc_EA9AD
loc_EA992:
		LDA		byte_129
		LDX		byte_12A
		STX		byte_129
		STA		byte_12A
		JMP		loc_EA9A7
loc_EA9A1:
		LDX		byte_129
		STX		byte_12A
loc_EA9A7:
		LDA		#<[byte_EB5C3]
		STA		_ptr0
		LDA		#>[byte_EB5C3]
loc_EA9AD:
		JSR		sub_EAA49
		CMP		#$FF
		BNE		loc_EA9CA
		JSR		sub_EA9D9
		LDX		#$D0							; default bg tiles indexes
		LDA		_ptr0
		LDY		_cur_info_maps_button_idx
		BEQ		loc_EA9C5
		LDX		#$50							; for power-like maps
		CLC
		ADC		#$60							; additional checker bg tile
loc_EA9C5:
		STA		_tmp4
		STX		_tmp5
		RTS
loc_EA9CA:
		STA		_tmp4
		MOVWO	_ptr0,byte_EB603
		LDA		(_ptr0),Y
		STA		_tmp5
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map1_draw:
		JSR		_mini_map0_draw
		LDA		_tmp4
		CMP		#$5C
		BCC		locret_EA7B6
		CMP		#$5F
		BCS		locret_EA7B6
		LDX		byte_129
		CPX		byte_12A
		BNE		loc_EA786
		LDA		#$58
		BNE		loc_EA789
loc_EA786:
		SEC
		SBC		#$03
loc_EA789:
		STA		_tmp4
		LDA		_mini_map_cur_row
		ASL
		TAY
		CLC
		LDA		#<[_power_map]
		ADC		_pow_mul38_table,Y
		STA		_ptr0
		LDA		#>[_power_map]
		ADC		_pow_mul38_table+1,Y
		STA		_ptr0+1
		LDA		_mini_map_cur_col
		LSR
		JSR		_adc_ptr0
		JSR		sub_EA8DF
		LDY		#$90
		LDA		byte_131
		CMP		_tmp5
		BEQ		loc_EA7B4
		LDY		#$10
loc_EA7B4:
		STY		_tmp5
locret_EA7B6:
		RTS

;_unused_3:
;		.BYTE	$66,$67,$68,$70,$71,$72,$76,$77,$78,$AC,$DC,$D9,$DA,$DB,$D9,$DA
;		.BYTE	$DB,$D9,$DA,$DB,$D8,$D8

; =============== S U B	R O U T	I N E =======================================
_mini_map2356_draw:
		LDY		#$00
		LDA		_cur_info_maps_button_idx
		ASL
		TAY
		LDX		_mini_map_cur_row
		CLC
		LDA		off_EB5A0,Y
		ADC		byte_EB66F,X
		STA		_ptr0
		LDA		off_EB5A0+1,Y
		ADC		#$00
		STA		_ptr0+1
		LDX		_mini_map_cur_col
		LDA		_mini_map_cur_row
		AND		#$01
		STA		_tmp6
		BEQ		loc_EA7F4
		INX
loc_EA7F4:
		TXA
		LSR
		JSR		_adc_ptr0
		LDY		#$00
		LDA		_mini_map_cur_col
		AND		#$01
		CMP		_tmp6
		BNE		loc_EA80D
		LDA		(_ptr0),Y
		LSR
		LSR
		LSR
		LSR
		JMP		loc_EA811
loc_EA80D:
		LDA		(_ptr0),Y
		AND		#$0F
loc_EA811:
		STA		_tmp6
		LDX		_cur_info_maps_button_idx
		CPX		#$03
		BNE		loc_EA82B
		LSR
		CMP		#$04
		BEQ		loc_EA82E
		TAX
		LDA		byte_EA84D,X
		STA		_tmp4
		LDA		byte_EA855,X
		STA		_tmp5
		RTS
loc_EA82B:
		LSR
		BNE		loc_EA831
loc_EA82E:
		JMP		_mini_map0_draw
loc_EA831:
		TAX
		LDA		byte_EA83D,X
		STA		_tmp4
		LDA		byte_EA845,X
		STA		_tmp5
		RTS
byte_EA83D:
		.BYTE	$5F,$28,$00,$50,$28,$50,$00,$50
byte_EA845:
		.BYTE	$10,$10,$10,$D0,$D0,$50,$D0,$90
byte_EA84D:
		.BYTE	$50,$50,$28,$28,$00,$00,$50,$50
byte_EA855:
		.BYTE	$D0,$D0,$D0,$D0,$D0,$D0,$90,$90

; =============== S U B	R O U T	I N E =======================================
_mini_map4_draw:
		LDA		_mini_map_cur_row
		ASL
		TAY
		CLC
		LDA		#<[_traffic_map]
		ADC		_pow_mul38_table,Y
		STA		_ptr0
		LDA		#>[_traffic_map]
		ADC		_pow_mul38_table+1,Y
		STA		_ptr0+1
		LDA		_mini_map_cur_col
		JSR		_adc_ptr0
		JSR		_get_info_map_block_sum
		LSR		_tmp5
		LSR		_tmp5
		LDA		_tmp5
		STA		_tmp6
		LSR
		BNE		loc_EA888
		JMP		_mini_map0_draw
loc_EA888:
		TAY
		LDA		byte_EA83D,Y
		STA		_tmp4
		LDA		byte_EA845,Y
		STA		_tmp5
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map89_draw:
		LDY		#$00
		LDA		_cur_info_maps_button_idx
		ASL
		TAY
		LDX		_mini_map_cur_row
		CLC
		LDA		off_EB5A0,Y
		ADC		byte_EB682,X
		STA		_ptr0
		LDA		off_EB5A0+1,Y
		ADC		#$00
		STA		_ptr0+1
		LDA		_mini_map_cur_col
		LSR
		LSR
		JSR		_adc_ptr0
		LDY		#$00
		LDA		_mini_map_cur_col
		AND		#$02
		BNE		loc_EA8C8
		LDA		(_ptr0),Y
		LSR
		LSR
		LSR
		LSR
		JMP		loc_EA8CC
loc_EA8C8:
		LDA		(_ptr0),Y
		AND		#$0F
loc_EA8CC:
		STA		_tmp6
		TAX
		BNE		loc_EA8D4
		JMP		_mini_map0_draw
loc_EA8D4:
		LDA		byte_EB55A,X
		STA		_tmp4
		LDA		byte_EB56A,X
		STA		_tmp5
		RTS

byte_EB55A:
		.BYTE	$5F,$5F,$28,$28,$00,$00,$50,$50,$28,$28,$50,$50,$00,$00,$50,$50
byte_EB56A:
		.BYTE	$10,$10,$10,$10,$10,$10,$D0,$D0,$D0,$D0,$50,$50,$D0,$D0,$90,$90

; =============== S U B	R O U T	I N E =======================================
sub_EA8DF:
		LDA		#$00
		STA		_tmp5
		LDA		_mini_map_cur_col
		AND		#$01
		BNE		sub_EA909
		LDY		#$00
		JSR		sub_EA91A
		LDY		#$09
		JSR		sub_EA8FB
		LDY		#$13
		JSR		sub_EA91A
		LDY		#$1C

; =============== S U B	R O U T	I N E =======================================
; !FALLTHROUGH!
sub_EA8FB:
		LDA		(_ptr0),Y
		AND		#$0F
		TAX
		LDA		byte_EB733,X
		CLC
		ADC		_tmp5
		STA		_tmp5
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA909:
		LDY		#$00
		JSR		sub_EA8FB
		LDY		#$0A
		JSR		sub_EA91A
		LDY		#$13
		JSR		sub_EA8FB
		LDY		#$1D

; =============== S U B	R O U T	I N E =======================================
sub_EA91A:
		LDA		(_ptr0),Y
		LSR
		LSR
		LSR
		LSR
		TAX
		LDA		byte_EB733,X
		CLC
		ADC		_tmp5
		STA		_tmp5
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_info_map_block_sum:
		LDA		#$00
		STA		_tmp5
		LDY		#$00
		JSR		_get_two_nibbles_sum
		LDY		#$13

; =============== S U B	R O U T	I N E =======================================
_get_two_nibbles_sum:
		LDA		(_ptr0),Y
		AND		#$0F
		CLC
		ADC		_tmp5
		STA		_tmp5
		LDA		(_ptr0),Y
		LSR
		LSR
		LSR
		LSR
		CLC
		ADC		_tmp5
		STA		_tmp5
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA9D9:
		LDA		#$02
		STA		_tmp118
		MOVWI	_ptr0,$0000
loc_EA9E4:
		LDX		_ptr0+1
		CPX		#$08
		BCS		locret_EAA40
		LDY		#$00
		LDA		_tmp118+8,X
		AND		#$0F
		CMP		_tmp118
		BCS		loc_EAA28
		LDY		#$02
		LDA		_tmp118+8,X
		LSR
		LSR
		LSR
		LSR
		CMP		_tmp118
		BCS		loc_EAA28
loc_EAA04:
		LDY		#$01
		LDA		_tmp118+7,X
		AND		#$0F
		CMP		_tmp118
		BCS		loc_EAA28
		DEC		_tmp118
		BNE		loc_EAA04
loc_EAA15:
		LDA		_tmp118+7,X
		LSR
		LSR
		LSR
		LSR
		BEQ		loc_EAA15
		STA		_tmp118+7,X
		LDA		#$02
		STA		_tmp118
		BNE		loc_EAA04
loc_EAA28:
		CPY		#$00
		BEQ		loc_EAA3A
		DEY
		TYA
		CLC
		ADC		_ptr0+1
		TAY
		LDA		byte_EAA41,Y
		CLC
		ADC		_ptr0
		STA		_ptr0
loc_EAA3A:
		INC		_ptr0+1
		INC		_ptr0+1
		BNE		loc_EA9E4
locret_EAA40:
		RTS
byte_EAA41:
		.BYTE	$1B,$36,$09,$12,$03,$06,$01,$02

; =============== S U B	R O U T	I N E =======================================
sub_EAA49:
		STA		_ptr0+1
		LDA		byte_129
		ASL
		ASL
		ADC		byte_12A
		LDY		_cur_info_maps_button_idx
		BEQ		loc_EAA5A
		ADC		#$10
loc_EAA5A:
		TAY
		LDA		(_ptr0),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EAA5E:
		LDA		_tmp118+3
		STA		byte_127,Y
		LDX		#$03
		TXA
		STA		byte_129,Y
		DEX
loc_EAA6B:
		LDA		byte_127,Y
		BEQ		loc_EAA75
		CMP		_tmp118,X
		BCS		loc_EAA7F
loc_EAA75:
		LDA		_tmp118,X
		STA		byte_127,Y
		TXA
		STA		byte_129,Y
loc_EAA7F:
		DEX
		BPL		loc_EAA6B
		LDA		#$00
		LDX		byte_129,Y
		STA		_tmp118,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_area_info_collect:
		LDA		#$00
		LDY		#$0E
loc_EAA8F:
		STA		_tmp118,Y						; prepare tmp buffer for stats
		DEY
		BPL		loc_EAA8F
		STA		byte_131
		TAY
.area_collect_loop:
		LDA		(_ptr0),Y						; collect map tiles
.area_lookup:
		LDX		#$00
.area_type_search_loop:
		CMP		_mini_map_building_types_list,X	; detect its types
		BCC		loc_EAAA9
		INX
		CPX		#$12
		BCC		.area_type_search_loop			; up to 18 block
.debug_assert:
		BCS		.debug_assert					; here we have debug assert, shouldnt be executed
loc_EAAA9:
		LDA		_mini_map_building_colors_list,X; found an area, get the tile color
		CMP		#$0E							; all tiles except special building tiles goes
		BNE		loc_EAAB6						; directly to buf calc
		JSR		_map_get_building_type			; or else we need to calc an actual building idx here
		JMP		.area_lookup					; make an actual area search again
loc_EAAB6:
		CMP		#$80
		BCC		loc_EAABF						; buildings has 80 bit set in list, skip if other obstacles
		INC		byte_131						; or increment building tiles counter
		AND		#$0F							; and extract type of building color
loc_EAABF:
		TAX
		LDA		_tmp118,X						; increment stat counter
		CLC
		ADC		#$01
		STA		_tmp118,X
		TXA
		AND		#$03
		TAX
		LDA		_mini_map_weights_list,X		; calc weight value for current tile
		PHA										;
		TXA
		PHA
		TYA
		AND		#$0F
		LSR
		TAX
		LDA		byte_EAE8A,X					; as well as stat sum value
		TAX
		PLA
		LSR
		BEQ		loc_EAAE1
		INX
loc_EAAE1:
		PLA
		CLC
		ADC		_tmp118+7,X						; adding stat sum
		STA		_tmp118+7,X
		INY
		CPY		#$E8							; end conditions for 4 lines below, E8 in Y
		BEQ		.area_collect_done				; A loads with new Y value for ptr0 indexed load
		LDA		#$E4
		CPY		#$9C
		BEQ		.area_next_row
		LDA		#$98
		CPY		#$50
		BEQ		.area_next_row					; line 4C-4F, start 98
		LDA		#$4C
		CPY		#$04
		BNE		.area_collect_loop				; line 0-3, start 4C, or continue for a line
.area_next_row:
		TAY
		JMP		.area_collect_loop
.area_collect_done:
		LDA		_cur_info_maps_button_idx		; now we have stats for kind of ground tile, its
		BNE		.area_simple_sum				; nubers, weighted sum as well as number of
		LDX		_info_maps_menu_extra_idx		; buildings inside area
		BEQ		.area_simple_sum				; for all kinds of maps except overall map, use simple summing
		DEX
		BEQ		loc_EAB3E						; for overall maps use three different types of summing
		DEX										; according to current overall map type
		BEQ		loc_EAB29
		CLC
		LDA		_tmp118
		ADC		_tmp118+1
		ADC		_tmp118+4
		ADC		_tmp118+6
		STA		_tmp118+1
		LDA		_tmp118+5
		BPL		loc_EAB60
loc_EAB29:
		CLC
		LDA		_tmp118
		ADC		_tmp118+1
		ADC		_tmp118+4
		ADC		_tmp118+5
		STA		_tmp118+1
		LDA		_tmp118+6
		BPL		loc_EAB60
loc_EAB3E:
		CLC
		LDA		_tmp118
		ADC		_tmp118+1
		ADC		_tmp118+5
		ADC		_tmp118+6
		STA		_tmp118+1
		LDA		_tmp118+4
		BPL		loc_EAB60
.area_simple_sum:
		LDA		_tmp118
		CLC
		ADC		_tmp118+4
		ADC		_tmp118+5
		ADC		_tmp118+6
loc_EAB60:
		STA		_tmp118
		RTS

_mini_map_weights_list:
		.BYTE	$10,$01,$10,$01
byte_EAE8A:
		.BYTE	$00,$02,$04,$06,$04,$06,$00,$02
_mini_map_building_types_list:
		.BYTE	_MAP_C_AREA_DEF					; range for R areas
		.BYTE	_MAP_I_AREA_DEF					; ...C
		.BYTE	_MAP_POLICE_STATION				; ...I
		.BYTE	_MAP_INTERNAL_BUILD0			; for all other buildings
		.BYTE	_MAP_GROUND
		.BYTE	_MAP_ROAD_LR
		.BYTE	_MAP_ELECTRO_LR
		.BYTE	_MAP_GRASS
		.BYTE	_MAP_CORRUPTED
		.BYTE	_MAP_FLOOD
		.BYTE	_MAP_POLLUTION
		.BYTE	_MAP_ELECTRO_ROAD_LR
		.BYTE	_MAP_WATER
		.BYTE	_MAP_WOODS0
		.BYTE	_MAP_WATER_ROAD_LR
		.BYTE	_MAP_WATER_RAIL_LR
		.BYTE	_MAP_FIRE
		.BYTE	$FF
_mini_map_building_colors_list:
		.BYTE	$84,$86,$85,$80,$0E,$01,$00,$80,$02,$01,$03,$00,$80,$03,$02,$00,$03,$01

; =============== S U B	R O U T	I N E =======================================
_map_get_building_type:
		LDA		(_ptr0),Y						; we read special building tle number
		AND		#$0F							; which determines the actual building idx
		TAX										; tile position on the map
		LDA		_ptr0							; relative to current offset
		SEC
		SBC		_special_tiles_ofs_list,X		; adjust ptr for this value
		STA		_tmp4
		LDA		_ptr0+1
		SBC		#$00
		STA		_tmp5							; store ptr to temp var
		LDA		(_tmp4),Y						; and now we could read the tile
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_fill_block:
		JSR		_buffers_test_wait_flush		; wait for flush
		DEC		_mmc5_operation_in_progress_flag; enter critical section
		LDA		_mini_map_cur_col				; the mini-map width is 19 tiles
		LSR										; so we can draw 4 blocks by 4
		LSR										; 4 tiles and one last block
		TAY										; 3 tiles, here table calc for it
		LDA		_mini_map_block_size_list,Y
		JSR		_mini_map_ppu_pos_queue
loc_EAB8B:
		JSR		_mini_map_fill_tile				; fills single mini-map tile with corresponding value
		LDX		_tmp2							; put them to queue buf
		LDA		_tmp4
		STA		_ppu_queue_buf,X				; nt and extnt as well
		INC		_tmp2
		LDX		_tmp3
		LDA		_tmp5
		STA		_extnt_queue_buf,X
		INC		_tmp3							; do this for all 4 tiles in block
		INC		_mini_map_cur_col				; here we have a list of columns where
		LDY		_mini_map_cur_col				; tile fill is over for current block
		TYA
		CMP		_mini_map_stop_col_list,Y		; block end test
		BCC		loc_EAB8B
		CMP		#$13
		BNE		loc_EABB8						; line end test
		LDA		#$00
		STA		_mini_map_cur_col				; new line init here
		JSR		_mini_map_set_new_line
loc_EABB8:
		LDX		_tmp2							; backup queue counters
		PPUQEND
		LDX		_tmp3
		EXTQEND
		INC		_mmc5_operation_in_progress_flag; exit critical section
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_set_new_line:
		INC		_mini_map_cur_row				; while draw, just increment counters
		LDA		_mini_map_cur_row
		CMP		#$13
		BNE		locret_EA6EE					; as soon all lines are drawn
		LDA		#$00							; do finishing setup
		STA		_scr_res_control_flags			; reset counters
		STA		_mini_map_cur_row
		STA		_mini_map_cur_col
		LDX		_scr_res_idx
		CPX		#$0A
		BCS		loc_EA6C3
		STA		_game_core_state				; reset core state for all except new city map gen
loc_EA6C3:
;		LDA		_thread_priority_req			; REDUNDANT, this code does nothing, all values are
;		AND		#$FF							; the same, no flags test here as well
;		STA		_thread_priority_req
		LDA		_mini_map_need_redraw_flag
		BNE		locret_EA6EE
		LDA		_info_maps_button_pending
		BEQ		locret_EA6EE
		LDA		#$00
		STA		_scr_res_control_flags
		LDA		#$80
		STA		_game_core_state
;		BNE		locret_EA6EE					; REDUNDANT, here we have some remnants of old code
;		LDA		#$00							; never executed, BNE here is unconditional
;		STA		_mini_map_need_redraw_flag
;		LDA		#$80
;		STA		_scr_res_control_flags
;		STA		_game_core_state
locret_EA6EE:
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_menu_handler:
		LDA		_scr_res_control_flags			; menu if for buttons with flag 0
		BEQ		.do_info_menu
		RTS
.do_info_menu:
		JSR		_info_maps_menu_draw			; draws an actual menu
		JMP		.info_menu_spr_update
.info_menu_loop:
		JSR		_wait_for_nmi					; menu idle loop
		BIT		_pad0_autorep_press
		BMI		.info_menu_A_press				; test for A/B buttons
		BVS		.info_menu_B_press
		LDA		_pad0_autorep_press
		AND		#$0F
		BEQ		.info_menu_loop					; cursor movement test
		TAX										; X = d-pad buttons pressed value
		APUA_SE	_SE_IDX_CLICK					; jump to next option
		LDY		_cur_info_maps_button_idx		; according to current selected menu type
		LDA		_info_maps_menu_var_ofs_list,Y	; idxes of extra menu pos vars
		TAY
		LDA		_info_maps_menu_extra_idx,Y		; load var
		CLC										; good optimization here! instead of using a lot of
		ADC		_info_maps_menu_var_delta_list,X; conditions, just read the delta to adjust value
		AND		#$03
		STA		_info_maps_menu_extra_idx,Y		; adjust, wrap around 3 - max value here
.info_menu_spr_update:
;		FJSRA	_get_info_maps_wnd_menu_cursor_pos,PRG7,PRG9	; REDUNDANT
		JSR		_get_info_maps_wnd_menu_cursor_pos	; OPTIMIZED, now local
;		FJSRA	_scr_res_cursor_draw,PRG6,PRG9	; REDUNDANT
		JSR		_scr_res_cursor_draw			; OPTIMIZED, now local
		JSR		_spr_finish
		JMP		.info_menu_loop
.info_menu_B_press:
		LDY		_cur_info_maps_button_idx		; B button - cancel current selection
		LDA		_info_maps_menu_var_ofs_list,Y
		TAY
		LDA		_info_maps_menu_extra_value_backup
		STA		_info_maps_menu_extra_idx,Y		; restore previously saved extra map idx value
		JSR		_info_maps_menu_erase			; close window
		LDY		_prev_info_maps_button_idx		; restore previously pressed button numbers
		LDA		_cur_info_maps_button_idx		; draw back to previous state all buttons and headers
		STA		_prev_info_maps_button_idx
		STY		_cur_info_maps_button_idx
		FJSRA	_info_maps_button_redraw,PRG7,PRG9	; FIX, now in PRG7
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		FJSRA	_info_maps_header_and_legend_redraw,PRG7,PRG9	; FIX, now in PRG7
		RTS
.info_menu_A_press:
		LDY		_cur_info_maps_button_idx		; press A, selection made, update vars
		BEQ		loc_EAC51						; overall map is 4-items and will be calc separately
		LDA		_info_maps_menu_var_ofs_list,Y	; adjust map button idx accordingly
		TAY
		LDA		_info_maps_menu_extra_idx,Y		; the rest of menus are 2-items only
		AND		#$01
		CLC
		ADC		_cur_info_maps_button_idx
		STA		_cur_info_maps_button_idx
loc_EAC51:
		LDA		#$80
		STA		_scr_res_control_flags

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_info_maps_menu_erase:
		APUA_SE	_SE_IDX_ROLLOUT
		FJSRA	_wnd_erase,PRGC,PRG9			; restore nt and extnt line by line, erasing wnd
		LDA		_cur_info_maps_button_idx		; set cursor position for corresponding button
		AND		#$FE
		TAY
		LDA		_info_maps_def_return_cursor_pos_list,Y
		STA		_cur_game_cursor_scr_pos._COL
		LDA		_info_maps_def_return_cursor_pos_list+1,Y
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		JMP		_wait_for_nmi					; exit

_info_maps_def_return_cursor_pos_list:
		.BYTE	$24,$48
		.BYTE	$24,$68
		.BYTE	$00,$00
		.BYTE	$00,$00
		.BYTE	$24,$B8
_info_maps_menu_var_delta_list:
		.BYTE	$00,$01,$FF,$00,$02,$00,$00,$00,$FE

; OPTIMIZED, now local
; =============== S U B	R O U T	I N E =======================================
; set fixed menu positions for extra menu windows for selecting the
; detailed map information type for city overall, population and police/fire
; map modes
;
_get_info_maps_wnd_menu_cursor_pos:
		LDY		_cur_info_maps_button_idx		; for info maps with multiple
		LDA		_info_maps_options_list,Y		; options, get the ram ofs of
		TAY										; option idx var
		LDA		_info_maps_menu_extra_idx,Y	; now get the actual value
		STA		_ptr0
		TYA
		ASL										; we may have 4 options for all menus
		ASL										; but two of three uses only 2
		CLC										; so we have some redundancy
		ADC		_ptr0							; in data here
		TAX										; X = map idx * 4 + sub menu idx
		LDA		_info_maps_sub_menu_pos_row_list,X
		STA		_cur_game_cursor_scr_pos._ROW	; set cursor pos to corresponding
		LDA		_info_maps_sub_menu_pos_col_list,X	; position
		STA		_cur_game_cursor_scr_pos._COL
		RTS

_info_maps_sub_menu_pos_col_list:
		.BYTE	$60,$90,$60,$90	; 4 menu options
		.BYTE	$60,$98,$60,$98	; 2 menu options, NOTE, last two bytes *unused* as well
		.BYTE	$60,$90			; 2 menu options ,$60,$90 REDUNDANT, *unused*
_info_maps_sub_menu_pos_row_list:
		.BYTE	$50,$50,$78,$78	; 4 menu options
		.BYTE	$80,$80,$80,$80 ; 2 menu options NOTE, last two bytes *unused* as well
		.BYTE	$B8,$B8			; 2 menu options ,$B8,$B8 REDUNDANT, *unused*
_info_maps_options_list:
		.BYTE	$00,$FF,$01,$FF,$FF,$FF,$FF,$FF,$02

; =============== S U B	R O U T	I N E =======================================
_info_maps_menu_draw:
		APUA_SE	_SE_IDX_ROLLIN
		LDA		#$40							; FIX, since we have idxes changed
		STA		_game_msg_wnd_active_flag		; may interfer with something in future
		LDA		#$10
		STA		_spr_buf_pos					; reserve the cursor buffer, not sure why
		JSR		_spr_finish
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		PUSHB	_cur_info_maps_button_idx
		LDA		#$0D							; idx for dummy header and legend data
		STA		_cur_info_maps_button_idx		; erase current headers, prepare to redraw it
		FJSRA	_info_maps_header_and_legend_redraw,PRG7,PRG9	; FIX, now in PRG7
		POPB	_cur_info_maps_button_idx
		LDY		_cur_info_maps_button_idx
		LDA		_info_maps_menu_var_ofs_list,Y
		TAY
		LDA		_info_maps_menu_extra_idx,Y		; backup current value in case we cancel selection
		STA		_info_maps_menu_extra_value_backup
		LDA		#$00
		STA		_wnd_lvl_idx
		LDY		_cur_info_maps_button_idx
		LDA		info_maps_menu_wnd_idx_list,Y	; get the wnd resource values
		STA		_wnd_idx
		LDX		info_maps_menu_wnd_idx_list+1,Y
		LDA		info_maps_menu_wnd_pos_list,X
		STA		_wnd_left_tile
		LDA		info_maps_menu_wnd_pos_list+1,X
		STA		_wnd_top_tile
		FJSRA	_wnd_draw,PRGC,PRG9				; draw window
		RTS

info_maps_menu_wnd_idx_list:
		.BYTE	$09,$00
		.BYTE	$07,$02
		.BYTE	$00,$00
		.BYTE	$00,$00
		.BYTE	$08,$04
info_maps_menu_wnd_pos_list:
		.BYTE	$07,$04
		.BYTE	$07,$0A
		.BYTE	$07,$11

; NOTE, this prevents the mini-map clear at the info screens when you not change
; the info map mode, but the THREAD1 signaling it changes, then code here
; should redraw the info map while doing it yor cursor is locked and all you
; see if the map is changed somehow, but most of the time it not!
; so it looks like you get some hang up for a couple of seconds
; so, why they doesn't wanted to clear map in this case? that's odd
;
; =============== S U B	R O U T	I N E =======================================
_mini_map_clear_draw_request:
; FIX, lets clear it all the time
;		LDA		_mini_map_need_redraw_flag
;		BEQ		_mini_map_clear_draw
		LDA		#$00							; reset request flag
		STA		_mini_map_need_redraw_flag
;		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_clear_draw:
		LDA		#$00
		STA		_mini_map_cur_row
loc_EACE9:
		PPUQSTART
		LDA		_mini_map_cur_row
		ASL
		PHA
		TAY
		LDA		_mini_map_ppu_col_ofs_list,Y	; get ppu ofs low nibble
		LDY		_scr_res_idx					; check the screen we draw
		CPY		#$0A							; new citi map shifter 6 tiles
		BCC		loc_EACFD						; to the left, the rest are
		SBC		#$06							; default
loc_EACFD:
		PPUQMOVA
		STA		_ptr0							; store ptr temporary
		PLA
		TAY
		LDA		_mini_map_ppu_col_ofs_list+1,Y	; put ppu ofs hi nibble
		STA		_ptr0+1							; full ptr stored now
		PPUQMOVA
		PPUQMOVB	#$13,OP_NOP,0				; put counter of bytes ($13)
		TAY
		LDA		#$DF							; send spaces
loc_EAD17:
		PPUQMOVA
		DEY
		BNE		loc_EAD17
		PPUQEND
		EXTQSTART
		EXTQMOVB	_ptr0,OP_NOP,0				; the same for extnt using the same ofs
		EXTQMOVB	_ptr0+1,OP_ADD,#$38			; but with correction to extnt ram ofs (24+38=5C)
		EXTQMOVB	#$13,OP_NOP,0				; same $13 tile
		TAY
		LDA		#$01							; default extnt value
loc_EAD3A:
		EXTQMOVA
		DEY
		BNE		loc_EAD3A
		EXTQEND
		LDA		_scr_res_idx					; only for new city map gen mode
		CMP		#$0D
		BCC		loc_EAD58
		LDA		_mini_map_cur_row				; after 8th row of drawing
		CMP		#$08
		BCC		loc_EAD58						; put "please wait" in the middle
;		FJSRA	_new_city_please_wait_spr_draw,PRG7,PRG9
		JSR		_new_city_please_wait_spr_draw	; OPTIMIZED, local now
loc_EAD58:
		JSR		_wait_for_nmi					; wait for nmi to flush buffers
		INC		_mini_map_cur_row				; draw black lines line by line
		LDA		_mini_map_cur_row
		CMP		#$13
		BCC		loc_EACE9
		LDA		#$00
		STA		_mini_map_cur_row
		RTS

; =============== S U B	R O U T	I N E =======================================
_new_city_please_wait_spr_draw:
		LDY		#$00
		LDX		#$60
loc_B9672:
		LDA		_spr_please_wait,Y
		STA		_spr_buf,X
		INX
		INY
		CPY		#$20
		BCC		loc_B9672
		STX		_spr_buf_pos
		RTS

_spr_please_wait:
		.BYTE	$80,$90,$00,$40
		.BYTE	$80,$91,$00,$48
		.BYTE	$80,$92,$00,$50
		.BYTE	$80,$93,$00,$58
		.BYTE	$80,$94,$00,$60
		.BYTE	$80,$A0,$00,$68
		.BYTE	$80,$A1,$00,$70
		.BYTE	$80,$A2,$00,$78

; =============== S U B	R O U T	I N E =======================================
_info_maps_city_name_hud_draw:
		LDY		_city._name						; as against the ni-game routines
		DEY										; this is single screen displays, no
		BEQ		locret_EAE85					; need to recalc the starting positions,
		STY		_ptr0							; first byte of city name is its length
		STY		_ptr0+1							; copy for extnt
		LDY		#$01							; point to begin of the scring
		PPUQSTART
		PPUQMOVB	#$62,OP_NOP,0				; put fixed pos and len already read
		PPUQMOVB	#$24,OP_NOP,0
		PPUQMOVB	_ptr0,OP_NOP,0
loc_EAE57:
		LDA		_city._name,Y					; just copy the text
		PPUQMOVA
		INY
		DEC		_ptr0
		BNE		loc_EAE57
		PPUQEND
		EXTQSTART								; same for extnt
		EXTQMOVB	#$62,OP_NOP,0
		EXTQMOVB	#>[_MMC5_EXRAM],OP_NOP,0
		EXTQMOVB	_ptr0+1,OP_NOP,0
		LDA		#$59							; extnt fixed
loc_EAE7B:
		EXTQMOVA
		DEC		_ptr0+1
		BNE		loc_EAE7B
		EXTQEND
locret_EAE85:
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_date_hud_draw:
		LDA		_hud_redraw_req					; wait for external redraw request
		BEQ		locret_EADB1
		LDA		#$00
		STA		_hud_redraw_req

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_info_maps_date_hud_draw_ex:
		MOVW	_ptr0,_city._cur_year			; convert city year to ppu data
		JSR		_hex_to_dec9999					; similar to regular hud, but
		LDA		#$15							; without need to recalc ppu offset
		JSR		_printf_num_lib					; use fixed position instead
		PPUQSTART
		PPUQMOVB	#$7B,OP_NOP,0				; the same with month
		PPUQMOVB	#$24,OP_NOP,0
		PPUQMOVB	#$03,OP_NOP,0
		LDA		_city._cur_month
		ASL
		ASL
		TAY
loc_EADA1:
		LDA		_months_lib,Y
		CMP		#$80
		BEQ		loc_EADAF
		PPUQMOVA
		INY
		BNE		loc_EADA1
loc_EADAF:
		PPUQEND
locret_EADB1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_popul_hud_draw:
		MOVD	_ptr0,_popul_cur				; fixed popul count draw
		JSR		_hex_to_dec9999999
		LDA		#$16
		JMP		_printf_num_lib

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;sub_EADC9:
;		LDX		#$10
;		STX		_spr_buf_pos
;		FJSRA	_info_maps_spr_rect_draw,PRG7,PRG9
;		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_spr_arrows_hide:
		LDX		#$2F
loc_EADD7:
;		LDA		#$00							; ORIGINAL: hides sprites to upper left corner
		LDA		#$FA							; FIX: hides sprites out of screen
		STA		_spr_buf+$20,X
		DEX
		BPL		loc_EADD7
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_arrows_move:
		LDA		#$28
		STA		_scr_res_control_flags			; control flags are set to frame move mode
		STA		_game_core_state				; THREAD0 takes control for now, THREAD1 suspended
		LDA		_pad0_autorep_press				; d-pad autorep test
		AND		#$0F
		BEQ		.dpad_released
		LDX		#$04
		STX		_pad0_dpad_autorep_delay		; when press, reload dpad autorep counter
		BNE		.arrows_move
.dpad_released:
		LDA		_pad0_held
		AND		#$0F
		BEQ		.arrows_draw					; now test an actual input
		LDX		_pad0_dpad_autorep_delay
		BEQ		.arrows_move
		DEC		_pad0_dpad_autorep_delay
		BNE		.arrows_draw
.arrows_move:
		JSR		_info_maps_rect_pos_adjust
.arrows_draw:
		FJSRA	_info_maps_spr_arrows_draw,PRG7,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
; A = d-pad button on start
;
_info_maps_rect_pos_adjust:
;		LDX		#$01							; REDUNDANT
		LDX		#$00							; FIX, when using the _tpos macro
		LSR										; col/row addresses here are swapped!
		BCS		.arrows_move_right				; hor movements
		LSR
		BCS		.arrows_move_left
		INX										; switch to vert movements var
		LSR
		BCC		.arrows_move_left				; now this is not right, but up
.arrows_move_right:
		LDA		_info_maps_rect_pos,X
		CMP		_info_maps_rect_limits_right_bottom,X	; check limits not exceeded mini-map window
		BCS		locret_EAE36
		CLC
		ADC		#$02
		STA		_info_maps_rect_pos,X
		RTS
.arrows_move_left:
		LDA		_info_maps_rect_pos,X
		CMP		_info_maps_rect_limits_left_top,X
		BEQ		locret_EAE36
		SEC
		SBC		#$02
		STA		_info_maps_rect_pos,X			; adjust pos with clapm
locret_EAE36:
		RTS

; FIX, also swapped because of changing pos vars order
_info_maps_rect_limits_left_top:
		.BYTE	$45,$3C
_info_maps_rect_limits_right_bottom:
		.BYTE	$AB,$AA

; =============== S U B	R O U T	I N E =======================================
_far_scr_res_hndl1_A_graphs_draw:
		LDA		#$00
		STA		_MMC5_PRG_PAGES					; switch WRAM bank mode
		INC		_scr_res_idx
;		FJSRA	_scr_res_hndl1_A_graphs_draw_init,PRG6,PRG9	; REDUNDANT SO MUCH!
		LDY		#$FF							; OPTIMIZED, this is what
		STY		_cur_graphs_button_press_idx	; to be far called lol
		STY		_prev_graphs_button_press_idx	; -
		FJSRA	_scr_res_hndl1_A_graphs_draw,PRG7,PRG9	; graph draw is in external bank
		LDA		#$04
		STA		_MMC5_PRG_PAGES					; restore WRAM bank mode
		RTS

; =============== S U B	R O U T	I N E =======================================
_far_scr_res_hndl1_B_graphs_input:
		LDA		#$00
		STA		_MMC5_PRG_PAGES
		FJSRA	_scr_res_hndl1_B_graphs_input,PRG7,PRG9	; external again
		LDA		#$04
		STA		_MMC5_PRG_PAGES
		RTS

; NOTE, budget screen draws at the end of the fiscal year. The draw request
; flag is set in _fiscal_year_results_calc when the funding values are calculated
; then _fiscal_year_results_calc will wait for budget screen to fully drawn
; because of the portion of code which calculates the amount of taxes needed
; see notes below.
;
; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl2_A_budget_draw:
		LDA		_scr_res_idx					; draw only if budget screen selected
		CMP		#$04							; since called from the common code for all resources
		BEQ		loc_EAEEC
		RTS
loc_EAEEC:
		DEC		_mmc5_operation_in_progress_flag; busy flag
		LDA		#$02
		STA		_MMC5_CHR_MODE					; set chr mapping mode
		LDA		#$80
		STA		_budget_scr_draw_req			; in case we call budget manually, set flag manualy here
		STA		_game_core_state				; and suspend THREAD1
		LDA		#$AC
		STA		_cur_game_cursor_scr_pos._COL	; set default pos of the cursor
		LDA		#$34
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$00
		STA		_budget_tax_cur_pos
		STA		_pad0_autorep_press
		STA		_tmp118+2
		LDA		#$A0
		STA		_scr_res_control_flags			; draw static values for budget screen calculated
		MOVD	_ptr0,_city_taxes_collected		; in _fiscal_year_results_calc before calling this
		JSR		_hex_to_dec99999				; draw function
		LDA		#$01
		JSR		_printf_num_lib
		MOVD	_ptr0,_city._money
		JSR		_hex_to_dec9999999
		LDA		#$12
		JSR		_printf_num_lib
		MOVW	_ptr0,_fund_request_fire		; amount needed to 100% funding of all services
		JSR		_hex_to_dec9999
		LDA		#$02
		JSR		_printf_num_lib
		MOVW	_ptr0,_fund_request_police
		JSR		_hex_to_dec9999
		LDA		#$03
		JSR		_printf_num_lib
		MOVW	_ptr0,_fund_request_trans
		JSR		_hex_to_dec9999
		LDA		#$04
		JSR		_printf_num_lib
		MOVW	_ptr0,_fund_request_education
		JSR		_hex_to_dec9999
		LDA		#$05
		JSR		_printf_num_lib
		BIT		_city._bank_flags				; draw bank picture if available
		BPL		loc_EAFD0
;		MOVWO	_ptr0,_tlm_nt_bank_icon
;		MOVWO	_tmp2,_tlm_extnt_bank_icon
;		LDA		#$FF
;		JSR		_tlm_queue
		LDX		#$20
		JSR		_tlm_lib_queue
		JSR		_buffers_force_flush
		BIT		_city._bank_flags
		BVC		loc_EAFBB
;		MOVWO	_ptr0,_tlm_nt_bank_loan			; also draw additional LOAN text if needed
;		MOVWO	_tmp2,_tlm_extnt_bank_loan
		LDX		#$24
		BNE		loc_EAFCB
loc_EAFBB:
;		MOVWO	_ptr0,_tlm_nt_bank_no_loan
;		MOVWO	_tmp2,_tlm_extnt_bank_no_loan
		LDX		#$28
loc_EAFCB:
;		LDA		#$FF
;		JSR		_tlm_queue
		JSR		_tlm_lib_queue
; -
; NOTE, this code what we exactly need to make use of
; AUTO-BUDGET function in game options without need to call
; the whole budget screen.
;
loc_EAFD0:
		LDY		#$00							; now we has to recalculate money amount needed
		LDX		#$03							; to fund all objects
		CLC
loc_EAFD5:
		LDA		_city_taxes_collected,Y			; first we calc the overall money colledcted by this year
		ADC		_city._money,Y
		STA		_budget_money,Y					; temporary value for planning the budget
		INY
		DEX
		BNE		loc_EAFD5
		JSR		_budget_funds_alloc_calc		; calc allocated value by previously calculated fund rates
		JSR		_budget_funds_alloc_sum_calc	; sum of all funds minus loaned money
		JSR		_budget_funds_alloc_sum_test	; test if you can afford full funding
		BCS		loc_EAFF8						; if you haven't enough money for funding all on 100 percent
		LDA		#$08							; start to decrease values from edu to police continuosly
		JSR		_budget_funds_alloc_adjust		; unless money are enough finally.
		JSR		_budget_funds_alloc_sum_calc	; recalculate sum one more time as we changed allocations here
		JSR		_budget_funds_rate_recalc		; also recalculate a new rates for funds
loc_EAFF8:
; -
		LDY		#$00
		STY		_budget_tax_cur_pos
loc_EAFFD:
		JSR		_budget_cur_fund_draw			; now draw calculated values
		LDY		_budget_tax_cur_pos
		INY
		INY
		STY		_budget_tax_cur_pos
		CPY		#$0A
		BCC		loc_EAFFD
		JSR		_buffers_force_flush
		LDA		#$01
		STA		_MMC5_CHR_MODE
		FJSRA	_wnd_header_year_draw,PRG7,PRG9	; finally draw current year in header
		INC		_scr_res_idx
		LDA		#$00
		STA		_budget_display_timer			; sprites and animation init
		STA		_budget_display_timer+1
		STA		_budget_tax_cur_pos
		FJSRA	_budget_screen_cursor_pos_set,PRG7,PRG9
		FJSRA	_budget_screen_clock_spr_animate,PRG7,PRG9
		INC		_mmc5_operation_in_progress_flag
		RTS

_tlm_nt_bank_icon:
		.BYTE	$84,$26,$04,$7B,$7C,$7D,$7E
		.BYTE	$90,$26,$01,$24
		.BYTE	$79,$26,$03,$10,$11,$12
		.BYTE	$99,$26,$03,$20,$21,$22
		.BYTE	$B9,$26,$03,$30,$31,$32
		.BYTE	$D8,$26,$06,$13,$14,$15,$16,$17,$18
		.BYTE	$F8,$26,$06,$23,$24,$25,$26,$27,$28
		.BYTE	$18,$27,$06,$33,$34,$35,$36,$37,$38
		.BYTE	$FF
_tlm_nt_bank_no_loan:
		.BYTE	$96,$26,$01,$00
		.BYTE	$FF
_tlm_nt_bank_loan:
		.BYTE	$8F,$26,$01,$7F
		.BYTE	$94,$26,$03,$05,$00,$00
		.BYTE	$FF
_tlm_extnt_bank_icon:
		.BYTE	$84,$5E,$04,$45,$45,$45,$45
		.BYTE	$90,$5E,$01,$0B
		.BYTE	$79,$5E,$03,$D7,$D7,$D7
		.BYTE	$99,$5E,$03,$D7,$D7,$D7
		.BYTE	$B9,$5E,$03,$D7,$D7,$D7
		.BYTE	$D8,$5E,$06,$17,$17,$17,$17,$17,$17
		.BYTE	$F8,$5E,$06,$17,$17,$17,$17,$17,$17
		.BYTE	$18,$5F,$06,$17,$17,$17,$17,$17,$17
		.BYTE	$FF
_tlm_extnt_bank_no_loan:
		.BYTE	$96,$5E,$01,$05
		.BYTE	$FF
_tlm_extnt_bank_loan:
		.BYTE	$8F,$5E,$01,$05
		.BYTE	$94,$5E,$03,$05,$05,$05
		.BYTE	$FF

; =============== S U B	R O U T	I N E =======================================
_far_scr_res_hndl2_B_budget_input:
		FJSRA	_scr_res_hndl2_B_budget_input,PRG7,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
_budget_cur_fund_update:
;		FJSRA	_budget_cur_fund_alloc_update,PRG6,PRG9; REDUNDANT, now local
		JSR		_budget_cur_fund_alloc_update	; apply allocated funds value changes for cur column
		JSR		_budget_funds_alloc_sum_calc	; same as at the budget init, summing
		JSR		_budget_funds_alloc_sum_test	; testing
		BCS		_budget_cur_fund_draw			; skip to draw if all is ok
		LDA		_budget_tax_cur_pos				; or recalculate if money aren't enough
		JSR		_budget_funds_alloc_adjust		; if overall money aren't enough, decrease allocations automatically
		JSR		_budget_funds_alloc_sum_calc	; this will clamp the last value to it's max always
		JSR		_budget_cur_fund_rate_recalc

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_budget_cur_fund_draw:
;		JSR		_taxes_alloc_hex_to_dec9999		; REDUNDANT
; OPTIMIZED, unufication
		LDA		_budget_tax_cur_pos				; get currently selected budget screen row
		AND		#$FE
		TAY
		LDA		_budget_fund_alloc,Y			; get the current allocated funding amount
		STA		_ptr0
		LDA		_budget_fund_alloc+1,Y
		STA		_ptr0+1
		JSR		_hex_to_dec9999
; -
		LDY		_budget_tax_cur_pos				; draw decimal converted value
		LDA		_budget_fund_print_idx_list,Y	; common for tax value threated as alloc amount here
		JSR		_printf_num_lib
		LDA		_budget_tax_cur_pos				; now, for all other than tax positions need to
		AND		#$FE							; draw percents as well
		BEQ		loc_EB328
		JSR		_budget_cur_fund_rate_recalc	; calc current allocation percentage
		LDA		#$00
		STA		_ptr0+1
		LDA		_budget_tax_cur_pos				; get the rate value (0-100)
		LSR
		TAY
		DEY
		LDA		_budget_fund_rate,Y
		STA		_ptr0
		JSR		_hex_to_dec9999					; to decimal
		LDY		_budget_tax_cur_pos				; also library draw
		LDA		_budget_tax_print_idx_list,Y
		JSR		_printf_num_lib
		JSR		_budget_overall_money_draw		; calc and draw cash flow and current funds fields
loc_EB328:
;		FJSRA	_save_taxes_data,PRG7,PRG9
; OPTIMIZED
		LDA		_budget_fund_alloc._tax			; update ingame values
		STA		_city._tax_rate
		MOVW	_fund_alloc_fire,_budget_fund_alloc._fire
		MOVW	_fund_alloc_police,_budget_fund_alloc._police
		MOVW	_fund_alloc_trans,_budget_fund_alloc._trans
		MOVW	_fund_alloc_education,_budget_fund_alloc._education
; -
		RTS

_budget_fund_print_idx_list:
		.BYTE	$00,$00,$07,$07,$08,$08,$09,$09,$0A,$0A,$0B,$0B
_budget_tax_print_idx_list:
		.BYTE	$00,$00,$0C,$0C,$0D,$0D,$0E,$0E,$0F,$0F,$10,$10

; FIX, now local
; -
; =============== S U B	R O U T	I N E =======================================
_budget_cur_fund_alloc_update:
		LDA		_budget_tax_cur_pos				; cur pos corresponds to particular
		AND		#$FE							; tax on screen, bits 1-7, so already
; REDUNDANT, optimized
;		TAX										; multiplied by 2, just mask lower bit
;		LDA		_fund_reqest_vars_list,X		; get the var ofs by idx
;		STA		_ptr0
;		LDA		_fund_reqest_vars_list+1,X
;		STA		_ptr0+1
;		LDY		#$00
;		LDA		(_ptr0),Y						; read requested fund value if any
;		STA		_tmp2							; this will be the max value of funding
;		INY
;		LDA		(_ptr0),Y
; OPTIMIZED
		JSR		_budget_get_fund_reqest_by_idx
		STX		_tmp2
; -
		STA		_tmp3
		BNE		loc_A0118
		LDA		_tmp2
		BNE		loc_A0118
		RTS										; zero, nothing to fund, nothing to increase
loc_A0118:
		LDA		_budget_tax_adjust_value		; this is actually a counter, when you press
		LSR										; buttons on budget screen, if non-zero, then
		LSR										; value need adjustment, if button is held
		LSR										; higher nibbles reaches $3x, so we may change
		LSR										; higher digits of the desired value up to 1000
;		ASL										; REDUNDANT
		TAX										; extract digit idx for adjust, mul2, because of words
		LDA		_budget_tax_cur_pos				; now, lower bit is exactly what arrow is pressed
		LSR										; up or down, so we need to increase or decrease
		BCC		.do_decrease
		ASL										; do increase here
		TAY
		LDA		_budget_fund_alloc,Y			; Y = tax idx * 2
		CLC
		ADC		_dec_digit_tbl0,X
		STA		_ptr0
		LDA		_budget_fund_alloc+1,Y
		ADC		_dec_digit_tbl1,X
		STA		_ptr0+1							; store result in tmp var
		CMP		_tmp3							; compare with max value needed
		BCC		.apply_adjust
		BNE		.clamp_to_max
		LDA		_ptr0
		CMP		_tmp2
		BCC		.apply_adjust
		BEQ		.apply_adjust
.clamp_to_max:
		LDA		_tmp2							; more than need, fix to max value
		STA		_budget_fund_alloc,Y
		LDA		_tmp3
		STA		_budget_fund_alloc+1,Y
		RTS
.apply_adjust:
		LDA		_ptr0+1							; update increased value
.apply_adjust_ex:
		STA		_budget_fund_alloc+1,Y
		LDA		_ptr0
		STA		_budget_fund_alloc,Y
		RTS
.do_decrease:
		ASL
		TAY
		LDA		_budget_fund_alloc,Y			; the same for decrease
		SEC
		SBC		_dec_digit_tbl0,X
		STA		_ptr0
		LDA		_budget_fund_alloc+1,Y
		SBC		_dec_digit_tbl1,X				; test against 0 instead
		BCS		.apply_adjust_ex				; OPTIMIZE a little
;		BCS		loc_A017A
		LDA		#$00
		STA		_budget_fund_alloc,Y			; store minimal value
		STA		_budget_fund_alloc+1,Y
		RTS
;loc_A017A:										; REDUNDANT
;		STA		_budget_fund_alloc+1,Y
;		LDA		_ptr0
;		STA		_budget_fund_alloc,Y
;		RTS

;_inc_table:									; REDUNDANT, merged with common decimal array
;		.WORD	1
;		.WORD	10
;		.WORD	100
;		.WORD	1000

; =============== S U B	R O U T	I N E =======================================
_budget_get_fund_reqest_by_idx:
		TAY										; already multiplied by 2 here
		LDA		_fund_reqest_vars_list,Y		; get the var ofs
		STA		_ptr0
		LDA		_fund_reqest_vars_list+1,Y
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y						; read requested fund value if any
		TAX										; lower to X
		INY
		LDA		(_ptr0),Y						; higher to A
		RTS

_fund_reqest_vars_list:
		.WORD	_max_tax_rate					; used for calculate maximum for overall tax rate
		.WORD	_fund_request_fire
		.WORD	_fund_request_police
		.WORD	_fund_request_trans
		.WORD	_fund_request_education
;		.WORD	0								; unused missing fund parameter of the earlier versions (medicine?)
; -

; =============== S U B	R O U T	I N E =======================================
_budget_funds_alloc_sum_test:
		LDY		#$00							; subtract all money including taxes
		LDX		#$03							; with sum of allocated funds
		SEC
loc_EB335:
		LDA		_budget_money,Y
		SBC		_budget_funds_alloc_sum,Y		; if loan get, result may be negative
		INY
		DEX
		BNE		loc_EB335
		RTS

; =============== S U B	R O U T	I N E =======================================
_budget_funds_alloc_adjust:
		AND		#$FE							; prepare counters
		PHA
		LDX		#$03
		LDY		#$00
		SEC
loc_EB348:
		LDA		_budget_funds_alloc_sum,Y		; get the delta of money left after funding
		SBC		_budget_money,Y					; will be negative if money are enough, and positive
		STA		_ptr0,Y							; if not.
		INY
		DEX
		BNE		loc_EB348
		PLA
		TAY
loc_EB357:
		LDA		_budget_fund_alloc,Y			; so subtract delta from cur funding value if positive
		SEC										; thus we decrease it for amount we missing
		SBC		_ptr0
		LDA		_budget_fund_alloc+1,Y			; if value is negative, we just add it and never exceed the 0 here
		SBC		_ptr0+1
		LDA		#$00
		SBC		_tmp2
		BCS		loc_EB38B						; if we add, no carry set. if we subtract, carry set on positive result
		LDA		_ptr0							; negative result means we have not cover the deficit with current funding only
		SEC										; so subtract full value of it,..
		SBC		_budget_fund_alloc,Y
		STA		_ptr0
		LDA		_ptr0+1
		SBC		_budget_fund_alloc+1,Y
		STA		_ptr0+1
		LDA		_tmp2
		SBC		#$00
		STA		_tmp2
		LDA		#$00
		STA		_budget_fund_alloc,Y			; then zero it. drop that fund at all
		STA		_budget_fund_alloc+1,Y
		DEY										; can't be zero here, because last valid value here is 2
		DEY
		BNE		loc_EB357						; and go to the upper funding value to readjust
;loc_EB389:
;		BEQ		loc_EB389						; REDUNDANT, debug assert should be the case if no money at all
loc_EB38B:
		LDA		_budget_fund_alloc,Y			; when we reach positive value, adjust alloc to it so we will have
		SEC										; allocation for exactly money we have not loess no more
		SBC		_ptr0
		STA		_budget_fund_alloc,Y
		LDA		_budget_fund_alloc+1,Y
		SBC		_ptr0+1
		STA		_budget_fund_alloc+1,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_budget_funds_alloc_sum_calc:
		LDA		#$00							; clear tmp sum
		STA		_tmp0
		STA		_tmp1
		STA		_tmp2
		LDY		#$02
loc_EB3A7:
		LDA		_budget_fund_alloc,Y			; all 4 allocated funds summing
		CLC
		ADC		_tmp0
		STA		_tmp0
		LDA		_budget_fund_alloc+1,Y
		ADC		_tmp1
		STA		_tmp1
		LDA		#$00
		ADC		_tmp2
		STA		_tmp2
		INY
		INY
		CPY		#$0A
		BCC		loc_EB3A7
		LDY		#$02
loc_EB3C4:
		LDA		_tmp0,Y							; store sum and
		STA		_budget_funds_alloc_sum,Y
		STA		_budget_funds_alloc_sum_loan,Y	; a sum including loan
		DEY
		BPL		loc_EB3C4
		BIT		_city._bank_flags				; special case if you have loaned
		BVC		locret_EB3E7					; recalculate loaned sum
		LDX		#$03
		LDY		#$00
		CLC
loc_EB3DA:
		LDA		_tmp0,Y							; adjust alloc sum with loan
		ADC		_bank_loan_amount,Y
		STA		_budget_funds_alloc_sum_loan,Y
		INY
		DEX
		BNE		loc_EB3DA
locret_EB3E7:
		RTS
_bank_loan_amount:
		.BYTE	$F4,$01,$00						; FIXME, one more hardcoded loan amount, 500 in 3-bytes value here!

; =============== S U B	R O U T	I N E =======================================
_budget_funds_rate_recalc:
		LDA		#$02
		STA		_budget_tax_cur_pos
loc_EB3F0:
		JSR		_budget_cur_fund_rate_recalc
		JSR		_wait_for_nmi
		INC		_budget_tax_cur_pos
		INC		_budget_tax_cur_pos
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCC		loc_EB3F0
		RTS

; =============== S U B	R O U T	I N E =======================================
_budget_overall_money_draw:
		JSR		_budget_cash_flow_calc			; got amount calculated and sign already in queue
		JSR		_hex_to_dec9999999				; now convert to decimal string
		LDA		#$11
		JSR		_printf_num_lib					; and display
; ADD, fix negative money
		LDA		#$DC							; sign "+" by default
		STA		_tmp6
; -
		LDX		#$03
		LDY		#$00
		SEC
loc_EB414:
		LDA		_budget_money,Y					; calc current funds amount
		SBC		_budget_funds_alloc_sum_loan,Y
		STA		_tmp0,Y
		INY
		DEX
		BNE		loc_EB414
		LDA		_tmp2
; ADD, fix negative values
		BPL		loc_EB427
		INC		_city_gone_broke_flag
		LDA		#$7F
		STA		_tmp6							; negative money, set sign "-"
		LDX		#$03
		LDY		#$00
		SEC
loc_EB426:
		LDA		_budget_funds_alloc_sum_loan,Y	; calc negative value
		SBC		_budget_money,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EB426
loc_EB427:
		PPUQSTART								; send sign char into ppu queue
		PPUQMOVB	#$0F,OP_NOP,0
		PPUQMOVB	#$27,OP_NOP,0
		PPUQMOVB	#$01,OP_NOP,0
		PPUQMOVB	_tmp6,OP_NOP,0
		PPUQEND
; -
loc_EB428:
		JSR		_hex_to_dec9999999				; BUG: can be negative if loan set
		LDA		#$13
		JMP		_printf_num_lib					; and display

; =============== S U B	R O U T	I N E =======================================
; this function calculated allocated amount for all fundings by previously calculated
; and stored funding rates in percent by formula request * rate / 100
;
_budget_funds_alloc_calc:
		LDA		#$02							; cur_pos = 0 is a special case for city tax value
		STA		_budget_tax_cur_pos
loc_EB450:
		AND		#$FE
; REDUNDANT, merged
;		TAY
;		LDA		_fund_reqest_vars_list,Y		; get the fund request var ptr
;		STA		_ptr0
;		LDA		_fund_reqest_vars_list+1,Y
;		STA		_ptr0+1
;		LDY		#$00
;		LDA		(_ptr0),Y						; read actual value
;		TAX
;		INY
;		LDA		(_ptr0),Y
; OPTIMIZED
		JSR		_budget_get_fund_reqest_by_idx
; -
		STX		_ptr0
		STA		_ptr0+1
		LDA		_budget_tax_cur_pos				; get corresponding fund rate
		LSR
		TAY
		DEY
		LDA		_budget_fund_rate,Y
		TAX
		JSR		_mmc5_mul16to8_fast				; request * rate
;		LDY		#$02							; REDUNDANT, moved to mul
;loc_EB478:
;		LDA		_tmp0,Y							; store result
;		STA		_tmp3,Y
;		DEY
;		BPL		loc_EB478
		LDA		#$64							; request * rate / 100
		STA		_tmp0
		LDA		#$00
		STA		_tmp1
		STA		_tmp2
		JSR		_div24_to_24
		LDA		_budget_tax_cur_pos				; store fund alloc values
		AND		#$FE
		TAY
		TXA
		STA		_budget_fund_alloc,Y
		LDA		_tmp6
		STA		_budget_fund_alloc+1,Y
;		JSR		_wait_for_nmi					; REDUNDANT, wait what?! here?!
		INC		_budget_tax_cur_pos				; calc for all 4 funding positions
		INC		_budget_tax_cur_pos
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCC		loc_EB450
		RTS

; =============== S U B	R O U T	I N E =======================================
; as against the previous one function, this one calculates the funding rate
; for allocated / requested values since we change exactly allocated value
; at the budget screen by formula alloc * 100 / request
;
_budget_cur_fund_rate_recalc:
		LDA		_budget_tax_cur_pos
		AND		#$FE
		TAY
		PHA
		LDA		_budget_fund_alloc,Y
		STA		_ptr0
		LDA		_budget_fund_alloc+1,Y
		STA		_ptr0+1
		LDX		#100
		JSR		_mmc5_mul16to8_fast				; allocated funds * 100
;		LDY		#$02							; REDUNDANT, move to mul
;loc_EB4C6:
;		LDA		_tmp0,Y							; backup result
;		STA		_tmp3,Y
;		DEY
;		BPL		loc_EB4C6
		PLA
; REDUNDANT
;		TAY
;		LDA		_fund_reqest_vars_list,Y		; get fund requested amount
;		STA		_ptr0
;		LDA		_fund_reqest_vars_list+1,Y
;		STA		_ptr0+1
;		LDY		#$00
;		LDA		(_ptr0),Y						; read fund request amount
;		TAX
;		INY
;		LDA		(_ptr0),Y
; -
; OPTIMIZED
		JSR		_budget_get_fund_reqest_by_idx
; -
		STA		_ptr0+1
		BNE		loc_EB4EF						; division by zero test
		CPX		#$00
		BNE		loc_EB4EF
		LDX		#$64							; default value for div by zero case 100
		BNE		loc_EB4F8
loc_EB4EF:
		STX		_ptr0
		LDA		#$00
		STA		_tmp2
		JSR		_div24_to_24					; X=alloc*100/request = allocation percent
loc_EB4F8:
		LDA		_budget_tax_cur_pos				; adjust cur tax pos to _budget_fund_rate structure
		LSR										; Y = (_budget_tax_cur_pos>>1) - 1
		TAY
		DEY
		TXA
		STA		_budget_fund_rate,Y				; store result
		RTS

; =============== S U B	R O U T	I N E =======================================
_budget_cash_flow_calc:
		LDA		#$DC							; sign "+" by default
		STA		_tmp6
		LDY		#$00
		LDX		#$03
		SEC
loc_EBAB4:
		LDA		_city_taxes_collected,Y			; subtract funds
		SBC		_budget_funds_alloc_sum,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EBAB4
		BCS		loc_EBAD9						; test if not exceeded
		LDA		#$7F
		STA		_tmp6							; negative money, set sign "-"
		LDY		#$00
		LDX		#$03
		SEC
loc_EBACC:
		LDA		_budget_funds_alloc_sum,Y		; calc negative value
		SBC		_city_taxes_collected,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EBACC
loc_EBAD9:
		PPUQSTART								; send sign char into ppu queue
		PPUQMOVB	#$AF,OP_NOP,0
		PPUQMOVB	#$26,OP_NOP,0
		PPUQMOVB	#$01,OP_NOP,0
		PPUQMOVB	_tmp6,OP_NOP,0
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl3_A_city_eval_draw:
		LDA		_scr_res_idx					; same as budget, draws in common code
		CMP		#$06
		BEQ		loc_EB05B
		RTS
loc_EB05B:
		LDA		#$02
		STA		_MMC5_CHR_MODE
		FJSRA	_wnd_header_year_draw,PRG7,PRG9	; draw year in header first here
		LDA		#$00
		STA		_city_eval_string_counter
		LDA		#$80
		STA		_scr_res_control_flags
		INC		_scr_res_idx
		LDX		_city._city_type				; draw city type string
		LDA		#$04
		JSR		_printf_txt_lib
		LDX		_city._difficult				; difficult string
		LDA		#$05
		JSR		_printf_txt_lib
loc_EB084:
		LDA		#$00							; draw other numeric values
		STA		_ptr0+1
		STA		_tmp2
		LDA		_city_eval_string_counter
		ASL
		TAX
		LDA		_city_eval_vars_list,X
		STA		_tmp4
		LDA		_city_eval_vars_list+1,X
		STA		_tmp5
		LDY		_city_eval_string_counter
		LDX		_city_eval_len_list,Y
		LDY		#$00
		CPX		#$01
		BNE		loc_EB0A9
		LDA		(_tmp4),Y						; read one-byte value
		BMI		.city_problem_txt_draw			; draw number if non-zero, or else draw only text
loc_EB0A9:
		LDA		(_tmp4),Y						; read the value for the rest parameters
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EB0A9
		DEY
		LDA		(_tmp4),Y						; continue if large value is positive
		BPL		loc_EB0D4						; test if values threated as negative
		LDY		_city_eval_string_counter
		CPY		#$03
		BEQ		loc_EB0C2						; values 3 and 6 are signed
		CPY		#$06
		BNE		loc_EB0D4						; the rest is not
loc_EB0C2:
		LDX		_city_eval_len_list,Y			; invert value
		LDY		#$00
		SEC
loc_EB0C8:
		LDA		#$00
		SBC		_ptr0,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EB0C8
loc_EB0D4:
		JSR		_hex_to_dec999999				; convert to a string
		LDA		_city_eval_string_counter
		CLC
		ADC		#$17
		JSR		_printf_num_lib					; draw it
		INC		_city_eval_string_counter
		LDA		_city_eval_string_counter
		CMP		#$0B
		BCC		loc_EB084						; continue for all 10 values
.city_problem_txt_draw:
		LDA		#$0B							; draw city problem list
		STA		_city_eval_string_counter
loc_EB0EF:
		LDA		_city_eval_string_counter
		SEC
		SBC		#$0B
		STA		_tmp7
		TAX
		LDA		_worst_problem_list,X			; skip if no value
		BMI		loc_EB10D
		TAX
		LDA		_tmp7
		JSR		_printf_txt_lib
		INC		_city_eval_string_counter
		LDA		_city_eval_string_counter
		CMP		#$0F
		BCC		loc_EB0EF
loc_EB10D:
		JSR		_buffers_force_flush
		LDA		#$01
		STA		_MMC5_CHR_MODE
		RTS

_city_eval_len_list:
		.BYTE	$01,$01,$03,$03,$03,$02,$02,$01,$01,$01,$01,$01
_city_eval_vars_list:
		.WORD	_doing_a_good_job_votes
		.WORD	_doing_a_bad_job_votes
		.WORD	_popul_cur
		.WORD	_popul_net_migration
		.WORD	_assessed_value
		.WORD	_city._score
		.WORD	_annual_score_change
		.WORD	_worst_problem_percents
		.WORD	_worst_problem_percents+1
		.WORD	_worst_problem_percents+2
		.WORD	_worst_problem_percents+3

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl3_B_city_eval_input:
		BIT		_pad0_autorep_press				; simple skip test, simpliest of all here
		BVC		locret_EB11F
		LDA		#$40
		STA		_scr_res_control_flags
locret_EB11F:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl4_A_city_overview_init:
		PUSHB	_cur_info_maps_button_idx		; use the same critical section enter
		LDA		#$0B							; as for info screens, but here, so we
		STA		_cur_info_maps_button_idx		; need to hack it somehow
		JSR		_thread_lock_flag_set
		POPB	_cur_info_maps_button_idx		; restore hacked value
		INC		_game_core_state				; suspend THREAD1
		LDA		#$00
		STA		_tmp118+4						; init counter
		INC		_scr_res_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl4_B_city_overview_input:
		LDA		_tmp118+4						; values are $00 to $20, 33 in total
		CMP		#$21
		BCS		.overwiew_skip_test
		JSR		_city_param_overview_draw		; draw current
		INC		_tmp118+4						; to the next value
		LDA		_tmp118+4
		CMP		#$21
		BCC		locret_EB161					; unless draw all can't skip
.overwiew_skip_test:
		BIT		_pad0_autorep_press				; finally we could skip
		BVC		locret_EB161
		LDA		#$40
		STA		_scr_res_control_flags			; set control flags to signal main loop
		LDA		#$00							; we need exit
		STA		_game_core_state				; unlock THREAD1
locret_EB161:
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_param_overview_draw:
		CMP		#$0E							; different types of numbers should be displayed here
		BCC		.overview00_0D
		BEQ		.overview_0E
		CMP		#$0F
		BEQ		.overview_0F
		CMP		#$13
		BCC		.overview10_12
		CMP		#$1B
		BCC		.overview13_1A					; 2-byte var, parks/ground/shores stats
		JMP		.overview1B_20
.overview_0F:									; dummy
		RTS
.overview13_1A:
		LDA		_tmp118+4						; these values are written twice in two modes
		SEC
		SBC		#$13
		AND		#$FE
		TAX
		LDA		_parks_count,X				; read 2-bytes at once
		STA		_ptr0
		LDA		_parks_count+1,X
		STA		_ptr0+1
		LDA		_tmp118+4						; then check if odd or even counters here
		LSR
		BCC		.ground_count					; of odd, then print percent of overall map square
		LDA		#<[5776]						; 76*76 = 5776 a total number of tiles on map
		LDY		#>[5776]
		JMP		.overwiew_percent_printf		; printf percent of value out of 5776
.ground_count:
		LSR		_ptr0+1							; if even, then write overall area size in square km probationally
		ROR		_ptr0							; we have only 3 digits to display there, so div8
		LSR		_ptr0+1							; 5776/8 = 722 max 3 digits voila!
		ROR		_ptr0
		LSR		_ptr0+1
		ROR		_ptr0
		JMP		.overwiev_dec999_printf			; printf regular decimal value
.overview00_0D:
		LDA		_tmp118+4						; all regular counters
		JSR		_overview_get_var_by_idx		; read 2-bytes value in any case, even for 1-byte vars!
		LDX		_tmp118+4
		LDA		_overview_value_size_list,X		; then check if var is 1-byte
		BNE		.overwiev_dec999_printf			; if not (1), skip to printf
.overwiev_dec99_printf:
		LDA		#$00							; if yes (0), zero the higher nibble for convinience
		STA		_ptr0+1
.overwiev_dec999_printf:
		JSR		_hex_to_dec9999					; printf regular number
		LDX		_tmp118+4
		LDA		_overview_printf_lib_list,X
		JMP		_printf_num_lib
.overview_0E:
		LDA		_coal_power_station_count		; power stations here summed, only 36 value is used (E)
		CLC										; value F skipped, not used printf idx 3F then. so probably
		ADC		_nuke_power_station_count		; there were separate display of coal/power stations before
		STA		_ptr0
		LDA		#$00
		ADC		#$00
		STA		_ptr0+1							; coal+nukes may be 128+128 max, so we can have 256 here or $100 in hex
;		JSR		_hex_to_dec9999					; OPTIMIZED
;		LDA		#$36
;		JMP		_printf_num_lib
		JMP		.overwiev_dec999_printf
.overview10_12:
		LDA		_tmp118+4						; here we have 2-byte values
		JSR		_overview_get_var_by_idx
		JSR		_hex_to_dec9999					; to decimal first (4-digits max)
		PUSHB	_ptr0							; backup lower digit
		LDY		#$03
		LDX		#$00
loc_EB1EC:
		LDA		_ptr0+1,X						; shift one digit ahead (div 10)
		STA		_ptr0,X
		INX
		DEY
		BNE		loc_EB1EC
		LDX		_tmp118+4						; print higher digits up to decimal dot
		LDA		_overview_printf_lib_list,X
		JSR		_printf_num_lib
		POPB	_ptr0							; pop decimal after dot
		LDA		#$00
		STA		_ptr0+1
		STA		_tmp2
		STA		_tmp3
		LDX		_tmp118+4
		LDA		_overview_printf_lib_list,X		; printf as well
		CLC
		ADC		#$03
		JMP		_printf_num_lib					; next 3 idenxes are for this
.overview1B_20:
		LDA		_tmp118+4						; for developement states of map
		SEC										; 6 parameters
		SBC		#$1B
		JSR		_overview_get_var_by_idx
		LDA		_stats._total
		LDY		_stats._total+1
;		JMP		.overwiew_percent_printf		; REDUNDANT
.overwiew_percent_printf:
		PHA										; store divider
		TYA
		PHA
		LDX		#100							; multiply _tmp0/tmp1 * 100
		JSR		_mmc5_mul16to8_fast
;		LDX		#$02							; REDUNDANT, moved to mul, since used everywhere
;loc_EB230:
;		LDA		_tmp0,X
;		STA		_tmp3,X
;		DEX
;		BPL		loc_EB230
		POPB	_ptr0+1							; restore divider
		POPB	_ptr0
; NOTE, here we have copy-paste error, this div-by-zero test tests the higher
; nibble of divider first in other place here, but we have lower nibble to
; test in this place, then must test higher obviously. this code does calc
; for lover only, so any values like 256, 512 will be displayed as 0
;
		BNE		loc_EB247						; test for div by zero
;		LDA		_ptr0							; BUG
		LDA		_ptr0+1							; FIX, easiest way to fix this without code rearrangement
		BNE		loc_EB247
		LDX		#$00							; default value then 0
		BEQ		loc_EB24E
loc_EB247:
		LDA		#$00
		STA		_tmp2
		JSR		_div24_to_24
loc_EB24E:
		STX		_ptr0
		JMP		.overwiev_dec99_printf

_overview_value_size_list:
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$00,$00,$00,$00
_overview_printf_lib_list:
		.BYTE	$22,$24,$26,$28,$2A,$2C,$2E,$2F,$30,$31,$32,$33,$34,$35,$36,$3F	; note F aren't used here!
		.BYTE	$40,$41,$42,$37,$38,$39,$3A,$3B,$3C,$3D,$3E,$23,$25,$27,$29,$2B
		.BYTE	$2D

; =============== S U B	R O U T	I N E =======================================
_overview_get_var_by_idx:
		ASL
		TAX
		LDA		_overview_var_ptr_list,X
		STA		_ptr0
		LDA		_overview_var_ptr_list+1,X
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		TAX
		INY
		LDA		(_ptr0),Y
		STA		_ptr0+1
		STX		_ptr0
		RTS

_overview_var_ptr_list:
		.WORD	_RCI_counts._R				; 00
		.WORD	_RCI_counts._I				; 01
		.WORD	_RCI_counts._C				; 02
		.WORD	_stats._developed			; 03
		.WORD	_stats._undeveloped			; 04
		.WORD	_stats._other				; 05
		.WORD	_stats._total				; 06
		.WORD	_schools_count				; 07
		.WORD	_hospitals_count			; 08
		.WORD	_stadiums_count				; 09
		.WORD	_fire_stations_count		; 0A
		.WORD	_police_stations_count		; 0B
		.WORD	_airports_count				; 0C
		.WORD	_sea_ports_count			; 0D
		.WORD	_roads_count				; 0E	; *unused* REDUNDANT
		.WORD	_roads_count				; 0F	; *unused* REDUNDANT
		.WORD	_roads_count				; 10
		.WORD	_rails_count				; 11
		.WORD	_electrics_count			; 12

; =============== S U B	R O U T	I N E =======================================
;_far_scr_res_hndl5_A_history_draw:
;		FJSRA	_scr_res_hndl5_A_history_draw,PRG7,PRG9
;		RTS

; =============== S U B	R O U T	I N E =======================================
_printf_history_year:
		LDX		_tmp118+8
		LDA		_city._history_list,X
		INX
		STA		_ptr0
		LDA		_city._history_list,X
		INX
		STX		_tmp118+8
		STA		_ptr0+1
		JSR		_hex_to_dec9999
		LDA		_tmp118+7
		JMP		_printf_num_lib

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl5_B_history_input:
		BIT		_pad0_autorep_press
		BVC		locret_EB2BE
		LDA		#$40
		STA		_scr_res_control_flags
locret_EB2BE:
		RTS

; =============== S U B	R O U T	I N E =======================================
;_far_scr_res_hndl6_A_bank_draw:
;		FJSRA	_scr_res_hndl6_A_bank_draw,PRG7,PRG9
;		RTS

; =============== S U B	R O U T	I N E =======================================
_far_scr_res_hndl6_B_bank_input:
		FJSRA	_scr_res_hndl6_B_bank_input,PRG7,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
_thread_lock_flag_set:
		LDA		_cur_info_maps_button_idx
		ORA		#$80
		STA		_thread_priority_req
		JMP		_wait_for_nmi

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;_switch:
;		ASL
;		STA		_ptr0
;		SEC
;		PLA
;		ADC		_ptr0
;		STA		_ptr0
;		PLA
;		ADC		#$00
;		STA		_ptr0+1
;		LDY		#$00
;		LDA		(_ptr0),Y
;		PHA
;		INY
;		LDA		(_ptr0),Y
;		STA		_ptr0
;		POPB	_ptr0+1
;		JMP		(_ptr0)

; RE-REDUNDANT, moved to system bank, merged
; =============== S U B	R O U T	I N E =======================================
;_adc_ptr0:
;		CLC
;		ADC		_ptr0
;		STA		_ptr0
;		BCC		locret_EB52A
;		INC		_ptr0+1
;locret_EB52A:
;		RTS

_pow_mul38_table:
		.WORD	$000
		.WORD	$026
		.WORD	$04C
		.WORD	$072
		.WORD	$098
		.WORD	$0BE
		.WORD	$0E4
		.WORD	$10A
		.WORD	$130
		.WORD	$156
		.WORD	$17C
		.WORD	$1A2
		.WORD	$1C8
		.WORD	$1EE
		.WORD	$214
		.WORD	$23A
		.WORD	$260
		.WORD	$286
		.WORD	$2AC
off_EB5A0:
		.WORD	_tmp_map
		.WORD	_power_map
		.WORD	_pop_dencity_map
		.WORD	_pop_growth_map
		.WORD	_traffic_map
		.WORD	_pollution_map
		.WORD	_crime_map
		.WORD	_land_val_map
		.WORD	_fire_map
		.WORD	_police_map

; REDUNDANT, merged, moved to system bank
;_special_tiles_ofs_list:
;		.BYTE	$4C,$4D,$01,$98,$99,$9A,$4E,$02,$E4,$E5,$E6,$E7,$9B,$4F,$03

byte_EB5C3:
		.BYTE	$51,$52,$55,$B1,$54,$FF,$FF,$FF,$57,$FF,$FF,$FF,$B3,$FF,$FF,$FF
		.BYTE	$5C,$5C,$5C,$5C,$5E,$FF,$FF,$FF,$5E,$FF,$FF,$FF,$5E,$FF,$FF,$FF
byte_EB5E3:
		.BYTE	$00,$53,$56,$B2,$53,$FF,$FF,$FF,$56,$FF,$FF,$FF,$B2,$FF,$FF,$FF
		.BYTE	$00,$5D,$5D,$5D,$5D,$FF,$FF,$FF,$5D,$FF,$FF,$FF,$5D,$FF,$FF,$FF
byte_EB603:
		.BYTE	$50,$50,$D0,$10,$50,$FF,$FF,$FF,$D0,$FF,$FF,$FF,$10,$FF,$FF,$FF
		.BYTE	$50,$50,$50,$50,$50,$FF,$FF,$FF,$50,$FF,$FF,$FF,$50,$FF,$FF,$FF
;_city_mini_map_row_pos_list:
;		.WORD	_city_map
;		.WORD	_city_map+$130
;		.WORD	_city_map+$260
;		.WORD	_city_map+$390
;		.WORD	_city_map+$4C0
;		.WORD	_city_map+$5F0
;		.WORD	_city_map+$720
;		.WORD	_city_map+$850
;		.WORD	_city_map+$980
;		.WORD	_city_map+$AB0
;		.WORD	_city_map+$BE0
;		.WORD	_city_map+$D10
;		.WORD	_city_map+$E40
;		.WORD	_city_map+$F70
;		.WORD	_city_map+$10A0
;		.WORD	_city_map+$11D0
;		.WORD	_city_map+$1300
;		.WORD	_city_map+$1430
;		.WORD	_city_map+$1560
_mini_map_ppu_col_ofs_list:
		.BYTE	$09,$25
		.BYTE	$29,$25
		.BYTE	$49,$25
		.BYTE	$69,$25
		.BYTE	$89,$25
		.BYTE	$A9,$25
		.BYTE	$C9,$25
		.BYTE	$E9,$25
		.BYTE	$09,$26
		.BYTE	$29,$26
		.BYTE	$49,$26
		.BYTE	$69,$26
		.BYTE	$89,$26
		.BYTE	$A9,$26
		.BYTE	$C9,$26
		.BYTE	$E9,$26
		.BYTE	$09,$27
		.BYTE	$29,$27
		.BYTE	$49,$27
byte_EB66F:
		.BYTE	$00,$09,$13,$1C,$26,$2F,$39,$42,$4C,$55,$5F,$68,$72,$7B,$85,$8E
		.BYTE	$98,$A1,$AB
byte_EB682:
		.BYTE	$00,$00,$05,$05,$0A,$0A,$0F,$0F,$14,$14,$19,$19,$1E,$1E,$23,$23
		.BYTE	$28,$28,$2D
_info_maps_menu_var_ofs_list:
		.BYTE	$00,$FF,$01,$FF,$FF,$FF,$FF,$FF,$02

; REDUNDANT, this is an early copy of nt/extnt for Dr.Willy in Bank window with
; cash in its hand. the position values are shifted bottom-left
; so it would be displayed under the left window. this may indicate that
; there were another BANK screen layout
;
;		.BYTE	$00,$07,$0E
;		.BYTE	$26,$25,$06,$35,$DF,$58,$AC,$AD,$3A
;		.BYTE	$46,$25,$06,$DF,$A8,$A9,$AA,$AB,$DF
;		.BYTE	$66,$25,$06,$DF,$B8,$B9,$BA,$BB,$DF
;		.BYTE	$86,$25,$06,$DF,$C8,$C9,$CA,$CB,$32
;		.BYTE	$26,$5D,$06,$08,$01,$08,$C8,$C8,$08
;		.BYTE	$46,$5D,$06,$01,$C8,$C8,$C8,$C8,$01
;		.BYTE	$66,$5D,$06,$01,$C8,$C8,$C8,$C8,$01
;		.BYTE	$86,$5D,$06,$01,$C8,$C8,$C8,$C8,$08

byte_EB733:
		.BYTE	$00,$01,$01,$02,$01,$02,$02,$03,$01,$02,$02,$03,$02,$03,$03,$04
_scr_res_ctrl_flag_list:
		.BYTE	$00,$80,$00,$00,$80,$80,$80,$80,$00,$00,$40,$40
_mini_map_block_size_list:
		.BYTE	$04,$04,$04,$04,$03
_mini_map_stop_col_list:
		.BYTE	$04,$04,$04,$04,$04,$08,$08,$08,$08,$0C,$0C,$0C,$0C,$10,$10,$10
		.BYTE	$10,$13,$13,$13

; =============== S U B	R O U T	I N E =======================================
_printf_txt_lib:
		STA		_tmp7							; lib selector
		STX		_tmp6							; string selector
		PPUQSTART
;		ASL										; REDUNDANT, optimized
		TAY
		LDA		_txt_ppu_ofs_lib_lo,Y			; get the corresponding offset
		PPUQMOVA								; (same pos for all strings in one
		LDA		_txt_ppu_ofs_lib_hi,Y			; library block)
		PPUQMOVA
;		LDY		_tmp7							; REDUNDANT, optimized
		LDA		_txt_ppu_len_list,Y				; put string length (same length for all strings
		PPUQMOVA								; in one library block)
		STA		_tmp5
;		LDA		_tmp7							; REDUNDANT, optimized
;		ASL										; REDUNDANT, optimized
;		TAY										; REDUNDANT, optimized
		LDA		_txt_libs_lo,Y					; finally, read the library offset
		STA		_ptr0
		LDA		_txt_libs_hi,Y
		STA		_ptr0+1
;		LDY		_tmp7
		LDA		_tmp6
		BEQ		loc_EB896
		STA		_tmp4
		LDA		#$00
		CLC
loc_EB88F:
		ADC		_txt_ppu_len_list,Y				; now calculate offset of the actual string
		DEC		_tmp4							; ofs = string_selector * sting_length
		BNE		loc_EB88F
loc_EB896:
		TAY										; Y = ofs
loc_EB897:
		LDA		(_ptr0),Y						; queue actual string
		PPUQMOVA
		INY
		DEC		_tmp5
		BNE		loc_EB897
		PPUQEND
		EXTQSTART
		LDY		_tmp7							; now we has to adjust extnt as well
		CPY		#$06							; but only not for history strings, they are fine already
		BCS		locret_EB8F3
;		ASL										; REDUNDANT, optimized
;		TAY										; REDUNDANT, optimized
		LDA		_txt_ppu_ofs_lib_lo,Y			; get the same ppu offsets, but adjust it to extnt buffer
		EXTQMOVA
		LDA		_txt_ppu_ofs_lib_hi,Y
		CLC
		ADC		#$38
		EXTQMOVA
;		LDY		_tmp7							; REDUNDANT, optimized
		LDA		_txt_ppu_len_list,Y				; also length
		EXTQMOVA
		STA		_tmp5
;		LDA		_tmp7
;		ASL
;		TAY
		LDA		_txt_extnt_lib_lo,Y				; load corresponding extnt library ofs
		STA		_ptr0
		LDA		_txt_extnt_lib_hi,Y
		STA		_ptr0+1
		LDY		_tmp6
		LDA		_city_eval_string_counter		; check if we has to draw city problems list here
		CMP		#$0B
		BNE		loc_EB8E7						; if not, read extnt as is
		LDA		(_ptr0),Y						; for city probles extnt need to be adjusted
		ORA		#$80
		BNE		loc_EB8E9
loc_EB8E7:
		LDA		(_ptr0),Y						; if not, then exactly the same
loc_EB8E9:
		EXTQMOVA								; fill same extnt for length of sting
		DEC		_tmp5
		BNE		loc_EB8E9
		EXTQEND
locret_EB8F3:
		RTS

; =============== S U B	R O U T	I N E =======================================
_printf_num_lib:
		STA		_tmp118							; tmp index of library string
;		CMP		#$06							; REDUNDANT: never used, should disable
;		BEQ		locret_EB903					; some screen positions in the table
;		CMP		#$0B							; but why you need to call them if
;		BEQ		locret_EB903					; they aren't displayed anyway?
;		CMP		#$10
;		BNE		loc_EB904
;locret_EB903:
;		RTS
;loc_EB904:
		TAY										; convert decimal buffer into string
		LDX		_printf_num_lib_spaces_list,Y	; fill leading zeroes with spaces
		JSR		_printf_num_nt_convert
		LDX		_tmp118
loc_EB90E:
		LDA		_ptr0,Y							; now shift tile indexes to corresponding
		CLC										; tileset in CHR ROM
		ADC		_printf_num_tile_ofs_list,X		; individual for all numbers
		STA		_ptr0,Y
		DEY
		BPL		loc_EB90E
		PPUQSTART
		LDY		_tmp118							; FIX: optimized
;		ASL										; REDUNDANT, optimized
;		TAY										; REDUNDANT, optimized
		LDA		_printf_num_screen_pos_list_lo,Y; push ppu offset to queue
		PPUQMOVA
		LDA		_printf_num_screen_pos_list_hi,Y
		PPUQMOVA
;		LDY		_tmp118							; REDUNDANT, optimized
		LDA		_printf_num_len_list,Y			; queue length of number
		PPUQMOVA
		TAY
		DEY
loc_EB93C:
		LDA		_ptr0,Y							; queue actual string
		PPUQMOVA
		DEY
		BPL		loc_EB93C
		PPUQEND
		JSR		_printf_num_extnt_convert		; now fill extnt data for number
		EXTQSTART
		LDY		_tmp118
		CPY		#$22
		BCS		locret_EB97F					; also we not need extnt for some numbers
;		ASL										; REDUNDANT, optimized
;		TAY										; REDUNDANT, optimized
		LDA		_printf_num_screen_pos_list_lo,Y; queue the rest of data
		EXTQMOVA
		LDA		_printf_num_screen_pos_list_hi,Y
		CLC
		ADC		#$38							; adjust ppu pos to extnt buffer
		EXTQMOVA
;		LDY		_tmp118							; REDUNDANT, optimized
		LDA		_printf_num_len_list,Y
		EXTQMOVA
		TAY
		DEY
loc_EB973:
		LDA		_ptr0,Y							; store.
		EXTQMOVA
		DEY
		BPL		loc_EB973
		EXTQEND
locret_EB97F:
		RTS

; =============== S U B	R O U T	I N E =======================================
_printf_num_extnt_convert:
		LDA		#$00							; the same way as for nt data
		STA		_tmp118+3
		LDX		_tmp118
		LDA		_printf_num_len_list,X
		TAY
		DEY
loc_EB98D:
		LDA		_ptr0,Y							; find a special symbols that
		BMI		loc_EB9B2						; need special extnt values
		LDX		_tmp118+3
		BNE		loc_EB9A9
		CPY		#$00
		BEQ		loc_EB9A9
		INC		_tmp118+3
		TAX
		LDA		#$0B
		CPX		#$24
		BEQ		loc_EB9B8
		CPX		#$2D
		BEQ		loc_EB9B8
loc_EB9A9:
		LDX		_tmp118
		LDA		_number_extnt_table0,X
		JMP		loc_EB9B8
loc_EB9B2:
		LDX		_tmp118
		LDA		_number_extnt_table1,X
loc_EB9B8:
		STA		_ptr0,Y							; replace and continue for the whole buffer
		DEY
		BPL		loc_EB98D
		RTS

; =============== S U B	R O U T	I N E =======================================
_printf_num_nt_convert:
		LDA		#$00
		STA		_tmp7
		LDY		_tmp118
		LDA		_printf_num_len_list,Y			; read length of desires number
		TAY
		DEY
loc_EB9CB:
		LDA		_ptr0,Y							; look for first non-zero digit
		BNE		loc_EB9DA
		STX		_ptr0,Y							; fill zero with corresponding space char (in X)
		DEY
		BPL		loc_EB9CB
		INY										; finish digit with zero-terminate code
		LDX		#$00
		STX		_ptr0,Y
loc_EB9DA:
		LDX		_tmp118							; finish with a special symbol ("-" or "$"
		LDA		#$24							; "$"
		CPX		#$1B
		BEQ		loc_EB9F9
		LDA		#$2D							; "-"
		CPX		#$1A
		BEQ		loc_EB9F4
		CPX		#$1D
		BNE		locret_EB9FE					;
		LDX		_annual_score_change+1			; signed values also
		BPL		locret_EB9FE
		BMI		loc_EB9F9
loc_EB9F4:
		LDX		_popul_net_migration+2
		BPL		locret_EB9FE
loc_EB9F9:
		INY
		STA		_ptr0,Y							; put special character here
		DEY
locret_EB9FE:									; the rest not need any other symbols
		RTS

; =============== S U B	R O U T	I N E =======================================
; input args: 16bit _tmp0 * reg X, store 24bit _tmp0-tmp2, _tmp3_tmp5
;
_mmc5_mul16to8_fast:
		LDA		_tmp1							; thread unsafe non-stack version of mul routine
		STA		_MMC5_MUL0						; THREAD1 won't restore mul registers
		STX		_MMC5_MUL1						; however, most of calc using this routine
		LDA		_MMC5_MUL0						; work when THREAD1 suspended, so does not
		STA		_tmp2							; be interrupted...
		LDA		_MMC5_MUL1
		STA		_tmp5
		LDA		_tmp0
		STA		_MMC5_MUL0
;		LDA		#$64							; REDUNDANT: WTF LDA $64 redundant
		LDA		_MMC5_MUL0
		STA		_tmp0
		STA		_tmp3
		LDA		_MMC5_MUL1
		CLC
		ADC		_tmp2
		STA		_tmp1
		STA		_tmp4
		LDA		_tmp5
		ADC		#$00
		STA		_tmp2
		STA		_tmp5
;		LDX		#$02							; OPTIMIZED, store results in _tmp3 for div operations
;loc_EB230:										; stored directly to vars in code
;		LDA		_tmp0,X
;		STA		_tmp3,X
;		DEX
;		BPL		loc_EB230
		RTS

; =============== S U B	R O U T	I N E =======================================
_div24_to_24:
		JSR		loc_EBB02						; another local only division routine
		ASL		_tmp3
		ROL		_tmp4
		ROL		_tmp5
		JMP		loc_EBB06
loc_EBB02:
		LDX		#$00
		STX		_tmp6
loc_EBB06:
		LDA		_tmp3
		CMP		_ptr0
		LDA		_tmp4
		SBC		_ptr0+1
		LDA		_tmp5
		SBC		_tmp2
		BCC		locret_EBB2E
		INX
		BNE		loc_EBB19
		INC		_tmp6
loc_EBB19:
		SEC
		LDA		_tmp3
		SBC		_ptr0
		STA		_tmp3
		LDA		_tmp4
		SBC		_ptr0+1
		STA		_tmp4
		LDA		_tmp5
		SBC		_tmp2
		STA		_tmp5
		BCS		loc_EBB06
locret_EBB2E:
		RTS

_txt_ppu_ofs_lib_lo:
		.BYTE	<[$2687]
		.BYTE	<[$26C7]
		.BYTE	<[$2707]
		.BYTE	<[$2747]
		.BYTE	<[$2636]
		.BYTE	<[$2678]
		.BYTE	<[$24CA]
		.BYTE	<[$252A]
		.BYTE	<[$258A]
		.BYTE	<[$25EA]
		.BYTE	<[$264A]
		.BYTE	<[$26AA]
		.BYTE	<[$270A]
_txt_ppu_ofs_lib_hi:
		.BYTE	>[$2687]
		.BYTE	>[$26C7]
		.BYTE	>[$2707]
		.BYTE	>[$2747]
		.BYTE	>[$2636]
		.BYTE	>[$2678]
		.BYTE	>[$24CA]
		.BYTE	>[$252A]
		.BYTE	>[$258A]
		.BYTE	>[$25EA]
		.BYTE	>[$264A]
		.BYTE	>[$26AA]
		.BYTE	>[$270A]
_txt_ppu_len_list:
		.BYTE	$09,$09,$09,$09,$08,$04,$12,$12,$12,$12,$12,$12,$12

; REDUNDANT, the same array as previous one
;byte_EBB56:
;		.BYTE	$09,$09,$09,$09,$08,$04,$12,$12,$12,$12,$12,$12,$12

_txt_libs_lo:
		.BYTE	<[_txt_problems_lib]
		.BYTE	<[_txt_problems_lib]
		.BYTE	<[_txt_problems_lib]
		.BYTE	<[_txt_problems_lib]
		.BYTE	<[_txt_town_names_lib]
		.BYTE	<[_txt_difficult_lib]
		.BYTE	<[_txt_history_lib]
		.BYTE	<[_txt_history_lib]
		.BYTE	<[_txt_history_lib]
		.BYTE	<[_txt_history_lib]
		.BYTE	<[_txt_history_lib]
		.BYTE	<[_txt_history_lib]
		.BYTE	<[_txt_history_lib]
_txt_libs_hi:
		.BYTE	>[_txt_problems_lib]
		.BYTE	>[_txt_problems_lib]
		.BYTE	>[_txt_problems_lib]
		.BYTE	>[_txt_problems_lib]
		.BYTE	>[_txt_town_names_lib]
		.BYTE	>[_txt_difficult_lib]
		.BYTE	>[_txt_history_lib]
		.BYTE	>[_txt_history_lib]
		.BYTE	>[_txt_history_lib]
		.BYTE	>[_txt_history_lib]
		.BYTE	>[_txt_history_lib]
		.BYTE	>[_txt_history_lib]
		.BYTE	>[_txt_history_lib]

; NOTE, history strings aren't needed the extnt so they are not in this
; lib at all.
;
_txt_extnt_lib_lo:
		.BYTE	<[_txt_problems_extnt_list]
		.BYTE	<[_txt_problems_extnt_list]
		.BYTE	<[_txt_problems_extnt_list]
		.BYTE	<[_txt_problems_extnt_list]
		.BYTE	<[_txt_town_names_extnt_list]
		.BYTE	<[_txt_difficult_extnt_list]
_txt_extnt_lib_hi:
		.BYTE	>[_txt_problems_extnt_list]
		.BYTE	>[_txt_problems_extnt_list]
		.BYTE	>[_txt_problems_extnt_list]
		.BYTE	>[_txt_problems_extnt_list]
		.BYTE	>[_txt_town_names_extnt_list]
		.BYTE	>[_txt_difficult_extnt_list]
_txt_problems_extnt_list:
		.BYTE	$05,$05,$04,$05,$05,$06,$05
_txt_town_names_extnt_list:
		.BYTE	$05,$05,$05,$05,$06,$06
_txt_difficult_extnt_list:
		.BYTE	$04,$05,$05
_txt_town_names_lib:
		.BYTE	$1F,$12,$15,$15,$0A,$10,$0E,$DC		; "Village"
		.BYTE	$1D,$18,$20,$17,$DC,$DC,$DC,$DC		; "Town"
		.BYTE	$0C,$12,$1D,$22,$DC,$DC,$DC,$DC		; "City"
		.BYTE	$0C,$0A,$19,$12,$1D,$0A,$15,$DC		; "Capital"
		.BYTE	$09,$0A,$0B,$0C,$0D,$0E,$10,$11		; "Metropolis"
		.BYTE	$09,$12,$13,$14,$15,$16,$17,$18		; "Megapolis"
_txt_difficult_lib:
		.BYTE	$AD,$AE,$AF,$DC						; "Easy"
		.BYTE	$27,$28,$29,$2A						; "Normal"
		.BYTE	$24,$25,$26,$DC						; "Hard"
_txt_problems_lib:
		.BYTE	$0C,$1B,$12,$16,$0E,$DC,$DC,$DC,$DC	; "Crime"
		.BYTE	$19,$18,$15,$15,$1E,$1D,$12,$18,$17	; "Pollution"
		.BYTE	$D0,$D1,$D2,$D3,$D4,$D5,$D6,$D7,$D8	; "Housing cost"
		.BYTE	$1D,$0A,$21,$0E,$1C,$DC,$DC,$DC,$DC ; "Taxes"
		.BYTE	$1D,$1B,$0A,$0F,$0F,$12,$0C,$DC,$DC ; "Traffic"
		.BYTE	$00,$01,$02,$03,$04,$05,$06,$07,$08 ; "Unemployment"
		.BYTE	$0F,$12,$1B,$0E,$1C,$DC,$DC,$DC,$DC ; "Fires"
_txt_history_lib:
		.BYTE	"Become Town       "
		.BYTE	"Become City       "
		.BYTE	"Become Capital    "
		.BYTE	"Become Metropolis "
		.BYTE	"Become Megaropolis"
		.BYTE	"Fire              "
		.BYTE	"Flood             "
		.BYTE	"Air Crash         "
		.BYTE	"Tornado           "
		.BYTE	"Earthquake        "
		.BYTE	"Monster Attack    "
		.BYTE	"Melt Down         "
_printf_num_lib_spaces_list:
		.BYTE	$DF,$DC,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DC,$DC,$DC,$20,$DF,$DF,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC
		.BYTE	$DC,$DC,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C
		.BYTE	$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C
		.BYTE	$7C,$7C,$7C,$7C,$7C,$7C,$7C,$7C,$20,$20,$20,$20,$20,$20,$20
_printf_num_tile_ofs_list:
		.BYTE	$30,$00,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30,$30
		.BYTE	$30,$00,$00,$00,$30,$00,$00,$00,$00,$30,$00,$00,$00,$00,$00,$00
		.BYTE	$00,$00,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50
		.BYTE	$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50,$50
		.BYTE	$50,$50,$50,$50,$50,$50,$50,$50,$30,$30,$30,$30,$30,$30,$30

; FIX: optimized by split nibbles as any other arrays here.
;
_printf_num_screen_pos_list_lo:
		.BYTE	<[$24F1]
		.BYTE	<[$2535]
		.BYTE	<[$25AA]
		.BYTE	<[$25CA]
		.BYTE	<[$25EA]
		.BYTE	<[$260A]
		.BYTE	<[$0000]	; 06
		.BYTE	<[$25B0]
		.BYTE	<[$25D0]
		.BYTE	<[$25F0]
		.BYTE	<[$2610]
		.BYTE	<[$0000]	; 0B
		.BYTE	<[$25B8]
		.BYTE	<[$25D8]
		.BYTE	<[$25F8]
		.BYTE	<[$2618]
		.BYTE	<[$0000]	; 10
		.BYTE	<[$26B1]
		.BYTE	<[$26D1]
		.BYTE	<[$2711]
		.BYTE	<[$2498]
		.BYTE	<[$2476]
		.BYTE	<[$246E]
		.BYTE	<[$2564]
		.BYTE	<[$25A4]
		.BYTE	<[$2538]
		.BYTE	<[$2578]
		.BYTE	<[$25F6]
		.BYTE	<[$271A]
		.BYTE	<[$275A]
		.BYTE	<[$2682]
		.BYTE	<[$26C2]
		.BYTE	<[$2702]
		.BYTE	<[$2742]
		.BYTE	<[$2509]
		.BYTE	<[$250D]
		.BYTE	<[$2529]
		.BYTE	<[$252D]
		.BYTE	<[$2549]
		.BYTE	<[$254D]
		.BYTE	<[$25A9]
		.BYTE	<[$25AD]
		.BYTE	<[$25C9]
		.BYTE	<[$25CD]
		.BYTE	<[$2609]
		.BYTE	<[$260D]
		.BYTE	<[$2649]
		.BYTE	<[$24FA]
		.BYTE	<[$251A]
		.BYTE	<[$253A]
		.BYTE	<[$255A]
		.BYTE	<[$257A]
		.BYTE	<[$259A]
		.BYTE	<[$25BA]
		.BYTE	<[$25DA]
		.BYTE	<[$265A]
		.BYTE	<[$2638]
		.BYTE	<[$269A]
		.BYTE	<[$2678]
		.BYTE	<[$26DA]
		.BYTE	<[$26B8]
		.BYTE	<[$271A]
		.BYTE	<[$26F8]
		.BYTE	<[$2738]
		.BYTE	<[$26A9]
		.BYTE	<[$26C9]
		.BYTE	<[$26E9]
		.BYTE	<[$26AD]
		.BYTE	<[$26CD]
		.BYTE	<[$26ED]
		.BYTE	<[$26CA]
		.BYTE	<[$26AD]
		.BYTE	<[$24C4]
		.BYTE	<[$2524]
		.BYTE	<[$2584]
		.BYTE	<[$25E4]
		.BYTE	<[$2644]
		.BYTE	<[$26A4]
		.BYTE	<[$2704]
_printf_num_screen_pos_list_hi:
		.BYTE	>[$24F1]
		.BYTE	>[$2535]
		.BYTE	>[$25AA]
		.BYTE	>[$25CA]
		.BYTE	>[$25EA]
		.BYTE	>[$260A]
		.BYTE	>[$0000]
		.BYTE	>[$25B0]
		.BYTE	>[$25D0]
		.BYTE	>[$25F0]
		.BYTE	>[$2610]
		.BYTE	>[$0000]
		.BYTE	>[$25B8]
		.BYTE	>[$25D8]
		.BYTE	>[$25F8]
		.BYTE	>[$2618]
		.BYTE	>[$0000]
		.BYTE	>[$26B1]
		.BYTE	>[$26D1]
		.BYTE	>[$2711]
		.BYTE	>[$2498]
		.BYTE	>[$2476]
		.BYTE	>[$246E]
		.BYTE	>[$2564]
		.BYTE	>[$25A4]
		.BYTE	>[$2538]
		.BYTE	>[$2578]
		.BYTE	>[$25F6]
		.BYTE	>[$271A]
		.BYTE	>[$275A]
		.BYTE	>[$2682]
		.BYTE	>[$26C2]
		.BYTE	>[$2702]
		.BYTE	>[$2742]
		.BYTE	>[$2509]
		.BYTE	>[$250D]
		.BYTE	>[$2529]
		.BYTE	>[$252D]
		.BYTE	>[$2549]
		.BYTE	>[$254D]
		.BYTE	>[$25A9]
		.BYTE	>[$25AD]
		.BYTE	>[$25C9]
		.BYTE	>[$25CD]
		.BYTE	>[$2609]
		.BYTE	>[$260D]
		.BYTE	>[$2649]
		.BYTE	>[$24FA]
		.BYTE	>[$251A]
		.BYTE	>[$253A]
		.BYTE	>[$255A]
		.BYTE	>[$257A]
		.BYTE	>[$259A]
		.BYTE	>[$25BA]
		.BYTE	>[$25DA]
		.BYTE	>[$265A]
		.BYTE	>[$2638]
		.BYTE	>[$269A]
		.BYTE	>[$2678]
		.BYTE	>[$26DA]
		.BYTE	>[$26B8]
		.BYTE	>[$271A]
		.BYTE	>[$26F8]
		.BYTE	>[$2738]
		.BYTE	>[$26A9]
		.BYTE	>[$26C9]
		.BYTE	>[$26E9]
		.BYTE	>[$26AD]
		.BYTE	>[$26CD]
		.BYTE	>[$26ED]
		.BYTE	>[$26CA]
		.BYTE	>[$26AD]
		.BYTE	>[$24C4]
		.BYTE	>[$2524]
		.BYTE	>[$2584]
		.BYTE	>[$25E4]
		.BYTE	>[$2644]
		.BYTE	>[$26A4]
		.BYTE	>[$2704]
_printf_num_len_list:
		.BYTE	$02,$05,$04,$04,$04,$04,$04,$04,$04,$04,$04,$04,$03,$03,$03,$03
		.BYTE	$03,$06,$06,$06,$03,$04,$06,$03,$03,$06,$06,$08,$04,$04,$03,$03
		.BYTE	$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03
		.BYTE	$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03,$03
		.BYTE	$03,$03,$03,$01,$01,$01,$01,$01,$04,$04,$04,$04,$04,$04,$04
_number_extnt_table0:
		.BYTE	$59,$05,$19,$19,$19,$19,$00,$59
		.BYTE	$59,$59,$59,$00,$19,$19,$19,$19
		.BYTE	$00,$05,$05,$05,$8B,$42,$42,$85
		.BYTE	$05,$8B,$05,$05,$05,$05,$85,$05
		.BYTE	$05,$05,$15,$15,$15,$15,$15,$15
_number_extnt_table1:
		.BYTE	$05,$05,$05,$05,$05,$05,$05,$05
		.BYTE	$05,$05,$05,$05,$05,$05,$05,$05
		.BYTE	$05,$05,$05,$05,$8B,$01,$01,$05
		.BYTE	$05,$05,$05,$05,$05,$05,$02,$02
		.BYTE	$02,$02,$14,$14,$14,$14,$14,$14

; REDUNDANT, merged with _fund_reqest_vars_list
;_fund_reqest_ofs_list:
;		.WORD	0
;		.WORD	_fund_request_fire
;		.WORD	_fund_request_police
;		.WORD	_fund_request_trans
;		.WORD	_fund_request_education

; =============== S U B	R O U T	I N E =======================================
;_taxes_alloc_hex_to_dec9999:					; REDUNDANT
;		LDA		_budget_tax_cur_pos				; -
;		AND		#$FE							; -
;		TAY										; -
;		LDA		_budget_fund_alloc,Y			; -
;		STA		_ptr0							; -
;		LDA		_budget_fund_alloc+1,Y			; -
;		STA		_ptr0+1							; -
;
; !FALLTHROUGH!

; ---------------------------------------------------------------------------
; -------------H-E-X--T-O--D-E-C-I-M-A-L--C-O-N-V-E-R-T-E-R-S----------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
; input args, common:	_tmp0 (24-bit word)
; output            :   _tmp0 (4 to 7 bytes array)
;
_hex_to_dec9999:
		LDY		#$00
		STY		_tmp2
		LDX		#$03
		BNE		_hex_to_dec_ex

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec99999:
		LDX		#$04
		BNE		_hex_to_dec_ex

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec999999:
		LDX		#$05
		BNE		_hex_to_dec_ex

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec9999999:
		LDX		#$06

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec_ex:
		LDY		#$00
		STY		_tmp3							; prepare buffer
		STY		_tmp4
		STY		_tmp5
		STY		_tmp6
		STY		_tmp7
loc_EBA2D:
		LDY		#$00
loc_EBA2F:
		LDA		_tmp0							; from 10m to 10 back count
		CMP		_hex_to_dec_tbl0,X				; compare 24-bit value with table
		LDA		_tmp1
		SBC		_hex_to_dec_tbl1,X
		LDA		_tmp2
		SBC		_hex_to_dec_tbl2,X
		BCC		loc_EBA58
		LDA		_tmp0							; less than or equals, subtract
		SBC		_hex_to_dec_tbl0,X
		STA		_tmp0
		LDA		_tmp1
		SBC		_hex_to_dec_tbl1,X
		STA		_tmp1
		LDA		_tmp2
		SBC		_hex_to_dec_tbl2,X
		STA		_tmp2
		INY
		BNE		loc_EBA2F
loc_EBA58:
		STY		_tmp2,X
		DEX
		BPL		loc_EBA2D
		LDX		#$00
loc_EBA5F:
		LDA		_tmp2,X
		STA		_tmp1,X
		INX
		CPX		#$06
		BNE		loc_EBA5F
		RTS

; REDUNDANT, merged to one common tables set
; in system bank
;
;_hex_to_dec_tbl2:
;		.BYTE	[[10 >> 16] & $FF]		; $00
;		.BYTE	[[100 >> 16] & $FF]		; $00
;		.BYTE	[[1000 >> 16] & $FF]	; $00
;		.BYTE	[[10000 >> 16] & $FF]	; $00
;		.BYTE	[[100000 >> 16] & $FF]	; $01
;		.BYTE	[[1000000 >> 16] & $FF] ; $0F
;		.BYTE	[[10000000 >> 16] & $FF]; $98
;_hex_to_dec_tbl1:
;		.BYTE	[[10 >> 8] & $FF]		; $00
;		.BYTE	[[100 >> 8] & $FF]		; $00
;		.BYTE	[[1000 >> 8] & $FF]		; $03
;		.BYTE	[[10000 >> 8] & $FF]	; $27
;		.BYTE	[[100000 >> 8] & $FF]	; $86
;		.BYTE	[[1000000 >> 8] & $FF]  ; $42
;		.BYTE	[[10000000 >> 8] & $FF] ; $96
;_hex_to_dec_tbl0:
;		.BYTE	[[10 >> 0] & $FF]		; $0A
;		.BYTE	[[100 >> 0] & $FF]		; $64
;		.BYTE	[[1000 >> 0] & $FF]		; $E8
;		.BYTE	[[10000 >> 0] & $FF]	; $10
;		.BYTE	[[100000 >> 0] & $FF]	; $A0
;		.BYTE	[[1000000 >> 0] & $FF]  ; $40
;		.BYTE	[[10000000 >> 0] & $FF] ; $80

		SECTION_ENDS	C10, "PRGC and PRG1 MAIN SCREEN RESOURCES MANAGER"

		BANK_END F9,$C000
