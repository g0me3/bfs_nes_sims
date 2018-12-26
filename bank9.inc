
		ORG $A000

		BANK_START $F9

; FIX, moved from bank 7 to the local place
; -
; =============== S U B	R O U T	I N E =======================================
_scr_chr_vars_init:
		LDA		_ppu_ctrl_shadow
		ORA		#$01
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		LDA		#$84
		STA		_mmc5_nt_mode_shadow
		STA		_MMC5_NT_MODE
		LDA		_scr_res_idx
		ASL
		STA		_scr_res_idx
		ASL
		TAX
		LDY		#$00
loc_A001C:
		LDA		_screen_chr_banks_lib,X
		BEQ		loc_A0024
		STA		_MMC5_CHR_BANKSA,Y
loc_A0024:
		INX
		INY
		CPY		#$04
		BNE		loc_A001C
		LDA		#$00
		STA		_mini_map_cur_row
		STA		_mini_map_cur_col
		STA		_fund_request_tax+1
		STA		_game_msg_wnd_active_flag
		LDA		#$14
		STA		_fund_request_tax
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
		LDA		_screen_pal_spr_list,X
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
		STA		_ppu_pal_dequeue_req
		LDA		_scr_res_idx
		AND		#$FE
		BNE		loc_A0080
		LDY		_cur_info_maps_button_idx
		LDA		_screen_pal_extra_list,Y
		JMP		loc_A0085
loc_A0080:
		LSR
		TAY
		LDA		_screen_pal_list,Y
loc_A0085:
		STA		_screen_pal_cur_bg_idx
		JMP		_pal_load_lib

_screen_pal_list:
		.BYTE	$00,$04,$03,$02,$0B,$12,$06,$0A
_screen_pal_extra_list:
		.BYTE	$05,$05,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01

; =============== S U B	R O U T	I N E =======================================
_scr_res_wnd_close:
		LDX		#$FF
		STX		byte_53
		INX
		STX		byte_3C
		STX		byte_54
		STX		byte_56
		STX		_scr_res_control_flags
		STX		_game_core_stop_flag
		STX		_scr_res_idx					; FIX BUGS#19
		INX
		LDA		#$18
		STA		byte_55
		LDA		#$14
		STA		byte_57
		LDA		#$44							; NOTE: always the same cursor pos
		STA		_cur_game_cursor_scr_pos._COL	; after exit any screen resourse
		LDA		#$30
		STA		_cur_game_cursor_scr_pos._ROW
		JSR		_flush_wait_0
		JSR		_pal_fade_out
		LDA		_ppu_ctrl_shadow
		AND		#$FC
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		LDA		#$00
		STA		_MMC5_NT_MODE
		STA		_game_wnd_spr_mode_flag
		STA		_scr_res_window_opened_flag
		RTS
;-

; =============== S U B	R O U T	I N E =======================================
_scr_res_common:
		JSR		_thread_lock_flag_set
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		JSR		_spr_clear
;		FJSRA	_scr_chr_vars_init,PRG6,PRG9	; REDUNDANT
		JSR		_scr_chr_vars_init				; FIX, now local
		JSR		_wait_for_nmi
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		JSR		_wait_for_nmi
		FJSRA	_scr_common_draw,PRG6,PRG9
; -
; for some reason attempting to use a regular handler system with no extra checks
; lead to screen garbling... probably there is no proper sync somewhere
; putting these draw routines here prevents from interrupting while in draw?
; all there routines need render to be off, while other init functions aren't
;
		JSR		_scr_res_hndl2_A_budget_draw
		JSR		_scr_res_hndl3_A_city_eval_draw
;		JSR		_far_scr_res_hndl5_A_history_draw		; REDUNDANT
		FJSRA	_scr_res_hndl5_A_history_draw,PRG7,PRG9	; OPTIMIZED
;		JSR		_far_scr_res_hndl6_A_bank_draw			; REDUNDANT
		FJSRA	_scr_res_hndl6_A_bank_draw,PRG7,PRG9	; OPTIMIZED
;-
		LDA		_scr_res_idx
		LSR
		TAY
		LDA		byte_EA08C,Y
		STA		_scr_res_control_flags
		JSR		_ppu_reset
		JSR		_wait_for_nmi
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#$01
		STA		_scr_res_window_opened_flag
		JSR		_pal_fade_in
.scr_res_loop:
		JSR		_wait_for_nmi
		FJSRA	_wnd_common_cursor_spr_clear,PRG7,PRG9
		JSR		_spr_finish
		FJSRA	_scr_res_cursor_move,PRG6,PRG9
		FJSRA	_scr_res_cursor_draw,PRG6,PRG9
		JSR		_scr_res_hndl_common_exec
		BIT		_scr_res_control_flags
		BVC		.scr_res_quick_warp_test
		LDA		#$01
		STA		_apu_se_flags
;		FJSRA	_scr_res_wnd_close,PRG6,PRG9	; REDUNDANT
;		RTS
		JMP		_scr_res_wnd_close				; FIX, now local
.scr_res_quick_warp_test:
		LDA		_scr_res_control_flags
		CMP		#$10
		BNE		.scr_res_loop
		JMP		_scr_res_common

byte_EA08C:
		.BYTE	$00,$00,$20,$08,$08,$08,$08,$A8

; =============== S U B	R O U T	I N E =======================================
; handles 2, 3, 5, 6 are called once before tha main loop
; once, handles 2, 3, 5 are unrefrenced here, 6 refrenced anyway
; BUT WHY?!
;
_scr_res_hndl_common_exec:
		LDA		_scr_res_idx
		JSR		_switch
		.WORD	_scr_res_hndl0_A_info_maps_draw
		.WORD	_scr_res_hndl0_B_info_maps_input
		.WORD	_far_scr_res_hndl1_A_graphs_draw
		.WORD	_far_scr_res_hndl1_B_graphs_input
		.WORD	0	; _far_scr_res_hndl2_B_budget_input	; *unused* here
		.WORD	_far_scr_res_hndl2_B_budget_input
		.WORD	0	; _scr_res_hndl3_B_city_eval_input	; *unused* here
		.WORD	_scr_res_hndl3_B_city_eval_input
		.WORD	_scr_res_hndl4_A_city_overview_init
		.WORD	_scr_res_hndl4_B_city_overview_input
		.WORD	0	;_scr_res_hndl5_B_history_input		; *unused* here
		.WORD	_scr_res_hndl5_B_history_input
		.WORD	0	;_far_scr_res_hndl6_A_bank_draw		; *unused* here
		.WORD	_far_scr_res_hndl6_B_bank_input
		.WORD	_scr_res_hndl7_A_new_city_gen_init
		.WORD	_scr_res_hndl7_B_new_city_gen_input

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl7_A_new_city_gen_init:
		LDA		#$00
		STA		_mini_map_cur_col
		STA		_mini_map_cur_row
		STA		byte_140
		STA		byte_141
		STA		_cur_info_maps_button_idx
		STA		_info_maps_city_map_modes_idx
		STA		_tmp6D0+1
		INC		_scr_res_idx
		LDA		#$10
		STA		_tmp6D0
		JMP		sub_EA29D

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl7_B_new_city_gen_input:
		LDA		#$80
		STA		_game_core_state
		JSR		sub_EA18F
		BIT		_scr_res_control_flags
		BPL		loc_EA0FD
		JSR		sub_EA405
loc_EA0EC:
		JSR		_wait_for_nmi
		JSR		_mini_map_draw
		BIT		_scr_res_control_flags
		BMI		loc_EA0EC
		LDA		#$20
		STA		_scr_res_control_flags
		RTS
loc_EA0FD:
		BIT		_pad0_autorep_press
		BVC		loc_EA10C
		LDA		#$40
		STA		_scr_res_control_flags
		LDA		#$02
		STA		_title_sub_idx
		RTS
loc_EA10C:
		BMI		loc_EA124
		BIT		_pad0_autorep_release
		BMI		loc_EA124
		JSR		sub_EA13E
		BCS		loc_EA13B
		LDA		byte_15B
		BEQ		loc_EA13B
		DEC		byte_15C
		BNE		loc_EA13B
		JMP		sub_EA29D
loc_EA124:
		LDA		byte_140
		BNE		loc_EA12C
		JMP		sub_EA2E5
loc_EA12C:
		CMP		#$01
		BNE		loc_EA138
		LDA		#$00
		STA		_title_sub_idx
		JMP		sub_EA1C5
loc_EA138:
		JMP		sub_EA1F4
loc_EA13B:
		JMP		sub_EA405

; =============== S U B	R O U T	I N E =======================================
sub_EA13E:
		LDA		_pad0_autorep_press
		AND		#$0F
		BEQ		loc_EA189
		LDX		#$40
		STX		_apu_se_flags+1
		AND		#$03
		BNE		loc_EA165
		LDA		_pad0_autorep_release
		CMP		#$08
		BNE		loc_EA158
		DEC		byte_140
		DEC		byte_140
loc_EA158:
		INC		byte_140
		LDA		byte_140
		AND		#$03
		STA		byte_140
		BPL		loc_EA187
loc_EA165:
		LDA		byte_140
		CMP		#$02
		BCC		loc_EA189
		LDA		_pad0_autorep_release
		LSR
		BCC		loc_EA177
		DEC		byte_141
		DEC		byte_141
loc_EA177:
		INC		byte_141
		LDY		byte_141
		BPL		loc_EA181
		LDY		#$02
loc_EA181:
		LDA		byte_EA18B,Y
		STA		byte_141
loc_EA187:
		SEC
		RTS
loc_EA189:
		CLC
		RTS
byte_EA18B:
		.BYTE	$00,$01,$02,$00

; =============== S U B	R O U T	I N E =======================================
sub_EA18F:
		LDA		byte_140
		CMP		#$02
		BCC		loc_EA19F
		BEQ		loc_EA19B
		CLC
		ADC		#$02
loc_EA19B:
		CLC
		ADC		byte_141
loc_EA19F:
		ASL
		TAY
		LDA		byte_EA1B5,Y
		STA		_tmp6CE
		LDA		byte_EA1B5+1,Y
		STA		_tmp6CC
		FJSRA	_common_select_cursor_draw,SRAM,PRG1
		RTS
byte_EA1B5:
		.BYTE	$48,$E0
		.BYTE	$68,$E0
		.BYTE	$D2,$E2
		.BYTE	$D2,$DA
		.BYTE	$D2,$D2
		.BYTE	$DA,$E2
		.BYTE	$DA,$DA
		.BYTE	$DA,$D2

; =============== S U B	R O U T	I N E =======================================
sub_EA1C5:
		LDA		#$40
		STA		_apu_se_flags+1
		LDA		byte_15B
		BNE		locret_EA1F3
		MOVWO	_ptr0,byte_EA3D8
		MOVWO	_tmp2,byte_EA3EE
		LDA		#$00
		JSR		sub_EA366
		JSR		sub_EA405
		LDX		#$08
loc_EA1E8:
		JSR		_wait_for_nmi
		DEX
		BNE		loc_EA1E8
		LDA		#$40
		STA		_scr_res_control_flags
locret_EA1F3:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA1F4:
		LDA		#$04
		STA		_apu_se_flags
		LDX		#$00
		LDA		byte_140
		CMP		#$02
		BEQ		loc_EA203
		LDX		#$80
loc_EA203:
		STX		byte_15E
		LDY		byte_141
		LDA		byte_EA2E2,Y
		ORA		byte_15E
		STA		byte_15E
		LDA		#$80
		STA		byte_15C
		STA		byte_15B
		FJSRA	sub_B96A1,PRG7,PRG9
		MOVW	_ptr0,_new_map_number
		JSR		_hex_to_dec9999
		LDY		byte_141
		BIT		byte_15E
		BPL		loc_EA242
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
		LDA		_tmp2
		ASL
		TAY
		LDA		byte_EA2C4,Y
		CLC
		ADC		_ptr0
		STA		_ptr0
		LDA		byte_EA2C4+1,Y
		ADC		#$00
		STA		_tmp3
		LDY		_ptr0+1
		LDA		byte_EA2D8,Y
		CLC
		ADC		_ptr0
		STA		_ptr0
		STA		_new_map_number
		LDA		_tmp3
		ADC		#$00
		STA		_ptr0+1
		STA		_new_map_number+1
		JSR		_hex_to_dec9999
		JSR		sub_EA422
		LDA		#$F2
		BIT		byte_15E
		BPL		loc_EA289
		LDA		#$F7
loc_EA289:
		JSR		sub_EA433
		LDA		#$F1
		BIT		byte_15E
		BPL		loc_EA295
		LDA		#$F6
loc_EA295:
		FJSRA	sub_B96A1,PRG7,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA29D:
		JSR		sub_EA405
		JSR		sub_EACE4
		FJSRA	sub_B966E,PRG7,PRG9
		JSR		_wait_for_nmi
		JSR		_sram_on
		FJSRA	_generate_new_map,SRAM,PRG3
		LDA		#$80
		STA		_scr_res_control_flags
		LDA		#$00
		STA		byte_15B
		JMP		sub_EA405
byte_EA2C4:
		.BYTE	$00,$00
		.BYTE	$64,$00
		.BYTE	$C8,$00
		.BYTE	$2C,$01
		.BYTE	$90,$01
		.BYTE	$F4,$01
		.BYTE	$58,$02
		.BYTE	$BC,$02
		.BYTE	$20,$03
		.BYTE	$84,$03
byte_EA2D8:
		.BYTE	$00,$0A,$14,$1E,$28,$32,$3C,$46,$50,$5A
byte_EA2E2:
		.BYTE	$01,$02,$04

; =============== S U B	R O U T	I N E =======================================
sub_EA2E5:
		LDA		#$40
		STA		_apu_se_flags+1
		LDA		byte_15B
		BNE		loc_EA363
		MOVWO	_ptr0,byte_EA392
		MOVWO	_tmp2,byte_EA3A7
		LDA		#$00
		JSR		sub_EA366
		JSR		_wait_for_nmi
		LDA		_new_map_number
		CLC
		ADC		#$01
		STA		_ptr0
		LDA		_new_map_number+1
		ADC		#$00
		STA		_ptr0+1
		CMP		#$03
		BCC		loc_EA327
		BNE		loc_EA321
		LDA		_ptr0
		CMP		#$E8
		BCC		loc_EA327
loc_EA321:
		LDA		#$00
		STA		_ptr0+1
		STA		_ptr0
loc_EA327:
		MOVW	_new_map_number,_ptr0
		JSR		_hex_to_dec9999
		LDA		#$01
		LDX		_ptr0
		BNE		loc_EA342
		LDA		#$03
		LDX		_ptr0+1
		BNE		loc_EA342
		LDA		#$07
loc_EA342:
		STA		byte_15E
		JSR		sub_EA422
		JSR		sub_EA433
		JSR		sub_EA29D
		MOVWO	_ptr0,byte_EA3B5
		MOVWO	_tmp2,byte_EA3CA
		LDA		#$00
		JMP		sub_EA366
loc_EA363:
		JMP		sub_EA405

; =============== S U B	R O U T	I N E =======================================
sub_EA366:
		STA		_stack_upper
		PPUQSTART
		LDY		#$00
loc_EA36D:
		LDA		(_ptr0),Y
		CMP		_stack_upper
		BEQ		loc_EA37B
		PPUQMOVA
		INY
		BNE		loc_EA36D
loc_EA37B:
		PPUQEND
		EXTQSTART
		LDY		#$00
loc_EA381:
		LDA		(_tmp2),Y
		CMP		_stack_upper
		BEQ		loc_EA38F
		EXTQMOVA
		INY
		BNE		loc_EA381
loc_EA38F:
		EXTQEND
		RTS
byte_EA392:
		.BYTE	$F9,$24,$04,$AF,$AF,$AF,$AF,$19,$25,$04,$ED,$DE,$EE,$FD,$39,$25
		.BYTE	$03,$FE,$AD,$B9,$00
byte_EA3A7:
		.BYTE	$19,$5D,$04,$0A,$0A,$0A,$0A,$39,$5D,$03,$0A,$0A,$0A,$00
byte_EA3B5:
		.BYTE	$F9,$24,$04,$A9,$A9,$A9,$A9,$19,$25,$04,$4E,$45,$58,$54,$39,$25
		.BYTE	$03,$4D,$41,$50,$00
byte_EA3CA:
		.BYTE	$19,$5D,$04,$8B,$8B,$8B,$8B,$39,$5D,$03,$8B,$8B,$8B,$00
byte_EA3D8:
		.BYTE	$79,$25,$04,$AF,$AF,$AF,$AF,$99,$25,$04,$AB,$AC,$AD,$AE,$B9,$25
		.BYTE	$04,$9E,$AD,$9F,$DE,$00
byte_EA3EE:
		.BYTE	$99,$5D,$04,$0A,$0A,$0A,$0A
		.BYTE	$B9,$5D,$04,$0A,$0A,$0A,$0A,$00,$01,$00,$0A,$00,$64,$00,$64,$00

; =============== S U B	R O U T	I N E =======================================
sub_EA405:
		JSR		sub_EA422
		MOVW	_ptr0,_new_map_number
		JSR		_hex_to_dec9999
		LDX		#$50
		LDY		#$02
loc_EA419:
		LDA		#$BE
		JSR		sub_EA4CE
		DEY
		BPL		loc_EA419
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA422:
		LDX		#$10
		LDY		#$3B
loc_EA426:
		LDA		byte_EB787,Y
		STA		_spr_buf,X
		INX
		DEY
		BPL		loc_EA426
		STX		_spr_buf_pos
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA433:
		LDA		#$BE
		STA		byte_15D
		LDY		#$02
		LDX		#$50
loc_EA43C:
		LDA		byte_15E
		AND		byte_EA4CB,Y
		BNE		loc_EA44F
		LDA		byte_15D
		JSR		sub_EA4CE
		LDA		byte_15D
		BNE		loc_EA46D
loc_EA44F:
		LDA		_ptr0,Y
		PHA
		JSR		sub_EA4EA
		PLA
		STA		_ptr0,Y
		BIT		byte_15E
		BPL		loc_EA467
		LDA		byte_15D
		SEC
		SBC		#$0C
		BNE		loc_EA46D
loc_EA467:
		LDA		byte_15D
		CLC
		ADC		#$0C
loc_EA46D:
		JSR		sub_EA4CE
		DEY
		BPL		loc_EA43C
		STX		_spr_buf_pos
		BIT		byte_15E
		BPL		loc_EA489
loc_EA47A:
		JSR		_wait_for_nmi
		INC		byte_15D
		LDA		byte_15D
		CMP		#$CB
		BEQ		loc_EA4C3
		BNE		loc_EA496
loc_EA489:
		JSR		_wait_for_nmi
		DEC		byte_15D
		LDA		byte_15D
		CMP		#$B1
		BEQ		loc_EA4C3
loc_EA496:
		LDA		byte_15E
		LDY		#$02
		LDX		#$10
loc_EA49D:
		LSR
		BCC		loc_EA4B3
		BIT		byte_15E
		BMI		loc_EA4AD
		DEC		_spr_buf+$50,X
		DEC		_spr_buf+$54,X
		BNE		loc_EA4B3
loc_EA4AD:
		INC		_spr_buf+$50,X
		INC		_spr_buf+$54,X
loc_EA4B3:
		PHA
		LDA		byte_EA4C8,Y
		TAX
		PLA
		DEY
		BPL		loc_EA49D
		BIT		byte_15E
		BPL		loc_EA489
		BMI		loc_EA47A
loc_EA4C3:
		LDX		#$68
		STA		_spr_buf_pos
		RTS
byte_EA4C8:
		.BYTE	$00,$00,$08
byte_EA4CB:
		.BYTE	$01,$02,$04

; =============== S U B	R O U T	I N E =======================================
sub_EA4CE:
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
		LDA		byte_EB784,Y
		STA		_spr_buf,X
		INX
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA4EA:
		BIT		byte_15E
		BPL		loc_EA4FD
		LDA		_ptr0,Y
		CMP		#$09
		BNE		loc_EA4F8
		LDA		#$FF
loc_EA4F8:
		CLC
		ADC		#$01
		BPL		loc_EA507
loc_EA4FD:
		LDA		_ptr0,Y
		SEC
		SBC		#$01
		BPL		loc_EA507
		LDA		#$09
loc_EA507:
		STA		_ptr0,Y
		LDA		byte_15D
		JMP		sub_EA4CE

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
		JSR		_info_maps_city_name_HUD_draw
		JSR		_info_maps_date_HUD_draw_ex
		JSR		_info_maps_popul_HUD_draw
		LDA		#$00
		STA		byte_13C
		INC		_scr_res_idx
		LDA		#$80
		STA		_scr_res_control_flags
;		LDA		#$0B							; REDUNDANT
		LDA		#$FF							; FIX, another special value on entry
		STA		_prev_info_maps_button_idx		; to prevent button release draw at start
		JMP		_scr_res_hndl0_AB_info_maps_redraw

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl0_B_info_maps_input:
		JSR		_info_maps_date_HUD_draw
		JSR		_info_maps_popul_HUD_draw
;		JSR		sub_EADC9						; REDUNDANT
; OPTIMIZED
		LDX		#$10
		STX		_spr_buf_pos
		FJSRA	_info_maps_spr_rect_draw,PRG7,PRG9
; -
		JSR		_info_maps_spr_arrows_hide
		BIT		_scr_res_control_flags
		BPL		loc_EA554
		JMP		_info_maps_push_A_button_test
loc_EA554:
		BIT		byte_13C
		BPL		loc_EA580
		BVC		loc_EA563
		LDA		#$40
		STA		_scr_res_control_flags
		JMP		_info_maps_exit
loc_EA563:
		LDA		byte_13C
		AND		#$0F
		TAX
		LDA		byte_EB743,X
		STA		_scr_res_control_flags
		LDA		#$00
		STA		byte_13C
		LDA		_cur_info_maps_button_idx
		STA		_prev_info_maps_button_idx
		STX		_cur_info_maps_button_idx
		JMP		_scr_res_hndl0_AB_info_maps_redraw_ex
loc_EA580:
		LDA		_fiscal_year_results_draw_req
		BEQ		loc_EA588
		JMP		_budget_screen_force
loc_EA588:
		LDA		byte_107
		BNE		loc_EA5BA
		BIT		_pad0_held
		BVC		loc_EA594
		JMP		_info_maps_arrows_move
loc_EA594:
		LDA		#$00
		STA		_scr_res_control_flags
		STA		_game_core_state
		BIT		_pad0_autorep_press
		BPL		_wait_for_key
		LDA		#$20
		STA		_apu_se_flags+1
		LDA		#$80
		STA		_game_core_state
		JSR		_info_maps_get_hover_button_idx
		BCC		_info_maps_exit
		LDA		_cur_info_maps_button_idx
		STA		_prev_info_maps_button_idx
		STX		_cur_info_maps_button_idx
		JMP		_scr_res_hndl0_AB_info_maps_redraw
loc_EA5BA:
		LDA		#$A0
		STA		_scr_res_control_flags

; =============== S U B	R O U T	I N E =======================================
; !FALLTHROUGH!
_scr_res_hndl0_AB_info_maps_redraw:
		FJSRA	_info_maps_button_redraw,PRG6,PRG9

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl0_AB_info_maps_redraw_ex:
		JSR		_thread_lock_flag_set
		JSR		sub_EABC3
		LDA		_scr_res_control_flags
		BEQ		_wait_for_key
		JSR		sub_EACD9
		JSR		_wait_for_nmi
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		FJSRA	_info_maps_header_and_legend_redraw,PRG6,PRG9
		JMP		_mini_map_draw

; =============== S U B	R O U T	I N E =======================================
_wait_for_key:
		LDA		_pad0_held
		BEQ		locret_EA5F1
		LDA		#$80
		STA		_game_core_state
locret_EA5F1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_exit:
		LDA		_info_maps_rect_pos._ROW
		SEC
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
_budget_screen_force:
		LDA		#$80
		STA		_budget_screen_return_to_info_maps_flag
		LDA		#$02
		STA		_scr_res_idx
		LDA		#$10
		STA		_scr_res_control_flags
		JMP		_pal_fade_out

; =============== S U B	R O U T	I N E =======================================
_info_maps_push_A_button_test:
		BIT		_pad0_autorep_press
		BPL		.no_buttons_redraw
		LDA		byte_13C
		BNE		.no_buttons_redraw
		LDA		#$20
		STA		_apu_se_flags+1
		PUSHB	_scr_res_control_flags
		JSR		_info_maps_get_hover_button_idx
		PLA
		BCC		.no_buttons_redraw
		STA		_scr_res_control_flags
		STX		_ptr0
		BCS		loc_EA637
		LDX		#$C0
loc_EA637:
		TXA
		ORA		#$80
		STA		byte_13C
		PUSHB	_cur_info_maps_button_idx
		PUSHB	_prev_info_maps_button_idx
		LDA		_cur_info_maps_button_idx
		STA		_prev_info_maps_button_idx
		LDA		_ptr0
		STA		_cur_info_maps_button_idx
		FJSRA	_info_maps_button_redraw,PRG6,PRG9
		POPB	_prev_info_maps_button_idx
		POPB	_cur_info_maps_button_idx
.no_buttons_redraw:
		JMP		_mini_map_draw

; =============== S U B	R O U T	I N E =======================================
_info_maps_get_hover_button_idx:
		LDY		_cur_game_cursor_scr_pos._ROW
		CPY		#$40
		BCC		loc_EA6A2
		CPY		#$C0
		BCS		loc_EA6A2
		LDX		_cur_game_cursor_scr_pos._COL
		CPX		#$30
		BCS		loc_EA6A2
		CPX		#$10
		BCC		loc_EA6A2
		TYA
		SEC
		SBC		#$40
		CPX		#$20
		LDX		#$FF
		BCS		loc_EA687
		CMP		#$20
		BCS		loc_EA6A2
		LDX		#$07
		SEC
loc_EA687:
		INX
		SBC		#$10
		BCS		loc_EA687
		CPX		#$03
		BCC		loc_EA696
		INX
		CPX		#$09
		BCC		loc_EA696
		INX
loc_EA696:
		LDA		byte_EB743,X
		STA		_scr_res_control_flags
		CMP		#$40
		BEQ		loc_EA6A2
		SEC
		RTS
loc_EA6A2:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EA6A4:
		INC		_mini_map_cur_row
		LDA		_mini_map_cur_row
		CMP		#$13
		BNE		locret_EA6EE
		LDA		#$00
		STA		_scr_res_control_flags
		STA		_mini_map_cur_row
		STA		_mini_map_cur_col
		LDX		_scr_res_idx
		CPX		#$0A
		BCS		loc_EA6C3
		STA		_game_core_state
loc_EA6C3:
		LDA		_thread_priority_req
		AND		#$FF
		STA		_thread_priority_req
		LDA		byte_107
		BNE		locret_EA6EE
		LDA		byte_13C
		BEQ		locret_EA6EE
		LDA		#$00
		STA		_scr_res_control_flags
		LDA		#$80
		STA		_game_core_state
		BNE		locret_EA6EE
		LDA		#$00
		STA		byte_107
		LDA		#$80
		STA		_scr_res_control_flags
		STA		_game_core_state
locret_EA6EE:
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_ppu_pos_queue:
		PHA
		LDA		_mini_map_cur_row
		ASL
		TAY
		LDA		_mini_map_ppu_col_ofs_list,Y
		LDX		_scr_res_idx
		CPX		#$0A
		BCC		loc_EA702
		SBC		#$06
		CLC
loc_EA702:
		ADC		_mini_map_cur_col
		STA		_tmp2
		LDA		_mini_map_ppu_col_ofs_list+1,Y
		STA		_tmp3
		PPUQSTART
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
		STA		_ppu_queue_buf,X
		STA		_extnt_queue_buf,Y
		STA		_tmp4
		INX
		INY
		STX		_tmp2
		STY		_tmp3
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_line_draw:
		LDA		_cur_info_maps_button_idx
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
		LDA		_mini_map_cur_row
		ASL
		TAY
		LDA		_mini_map_cur_col
		ASL
		ASL
		ADC		_mini_map_wram_ofs_list,Y
		STA		_ptr0
		LDA		_mini_map_wram_ofs_list+1,Y
		ADC		#$00
		STA		_ptr0+1
		JSR		sub_EAA8B
		JMP		sub_EA94A

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
		LDA		#<[_power_grid_map_buf]
		ADC		word_EB57A,Y
		STA		_ptr0
		LDA		#>[_power_grid_map_buf]
		ADC		word_EB57A+1,Y
		STA		_ptr0+1
		LDA		_mini_map_cur_col
		LSR
		JSR		_adc_ptr0_0
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
		JSR		_adc_ptr0_0
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
		LDA		#<[_traffic_density_map_buf]
		ADC		word_EB57A,Y
		STA		_ptr0
		LDA		#>[_traffic_density_map_buf]
		ADC		word_EB57A+1,Y
		STA		_ptr0+1
		LDA		_mini_map_cur_col
		JSR		_adc_ptr0_0
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
		JSR		_adc_ptr0_0
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
sub_EA94A:
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
		LDX		#<[_wram_map_buf+$D0]
		LDA		_ptr0
		LDY		_cur_info_maps_button_idx
		BEQ		loc_EA9C5
		LDX		#<[_wram_map_buf+$50]
		CLC
		ADC		#>[_wram_map_buf+$50]
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
sub_EAA8B:
		LDA		#$00
		LDY		#$0E
loc_EAA8F:
		STA		_tmp118,Y
		DEY
		BPL		loc_EAA8F
		STA		byte_131
		TAY
loc_EAA99:
		LDA		(_ptr0),Y
loc_EAA9B:
		LDX		#$00
loc_EAA9D:
		CMP		_mini_map_building_types_list0,X
		BCC		loc_EAAA9
		INX
		CPX		#$12
		BCC		loc_EAA9D
loc_EAAA7:
		BCS		loc_EAAA7
loc_EAAA9:
		LDA		_mini_map_building_types_list1,X
		CMP		#$0E
		BNE		loc_EAAB6
		JSR		sub_EAB64
		JMP		loc_EAA9B
loc_EAAB6:
		CMP		#$80
		BCC		loc_EAABF
		INC		byte_131
		AND		#$0F
loc_EAABF:
		TAX
		LDA		_tmp118,X
		CLC
		ADC		#$01
		STA		_tmp118,X
		TXA
		AND		#$03
		TAX
		LDA		byte_EAE86,X
		PHA
		TXA
		PHA
		TYA
		AND		#$0F
		LSR
		TAX
		LDA		byte_EAE8A,X
		TAX
		PLA
		LSR
		BEQ		loc_EAAE1
		INX
loc_EAAE1:
		PLA
		CLC
		ADC		_tmp118+7,X
		STA		_tmp118+7,X
		INY
		CPY		#$E8
		BEQ		loc_EAB04
		LDA		#$E4
		CPY		#$9C
		BEQ		loc_EAB00
		LDA		#$98
		CPY		#$50
		BEQ		loc_EAB00
		LDA		#$4C
		CPY		#$04
		BNE		loc_EAA99
loc_EAB00:
		TAY
		JMP		loc_EAA99
loc_EAB04:
		LDA		_cur_info_maps_button_idx
		BNE		loc_EAB53
		LDX		_info_maps_city_map_modes_idx
		BEQ		loc_EAB53
		DEX
		BEQ		loc_EAB3E
		DEX
		BEQ		loc_EAB29
		CLC
		LDA		_tmp118+4
		ADC		_tmp118
		ADC		_tmp118+6
		ADC		_tmp118+1
		STA		_tmp118+1
		LDA		_tmp118+5
		BPL		loc_EAB60
loc_EAB29:
		CLC
		LDA		_tmp118+5
		ADC		_tmp118
		ADC		_tmp118+4
		ADC		_tmp118+1
		STA		_tmp118+1
		LDA		_tmp118+6
		BPL		loc_EAB60
loc_EAB3E:
		CLC
		LDA		_tmp118+5
		ADC		_tmp118
		ADC		_tmp118+6
		ADC		_tmp118+1
		STA		_tmp118+1
		LDA		_tmp118+4
		BPL		loc_EAB60
loc_EAB53:
		LDA		_tmp118
		CLC
		ADC		_tmp118+4
		ADC		_tmp118+5
		ADC		_tmp118+6
loc_EAB60:
		STA		_tmp118
		RTS

byte_EAE86:
		.BYTE	$10,$01,$10,$01
byte_EAE8A:
		.BYTE	$00,$02,$04,$06,$04,$06,$00,$02
_mini_map_building_types_list0:
		.BYTE	$20,$40,$50,$80,$90,$91,$A7,$B2,$B4,$B5,$B6,$B9,$BD,$CE,$E0,$E2,$E6,$FF
_mini_map_building_types_list1:
		.BYTE	$84,$86,$85,$80,$0E,$01,$00,$80,$02,$01,$03,$00,$80,$03,$02,$00,$03,$01

; =============== S U B	R O U T	I N E =======================================
sub_EAB64:
		LDA		(_ptr0),Y
		AND		#$0F
		TAX
		LDA		_ptr0
		SEC
		SBC		_special_tiles_ofs_list,X
		STA		_tmp4
		LDA		_ptr0+1
		SBC		#$00
		STA		_tmp5
		LDA		(_tmp4),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_mini_map_draw:
		JSR		_flush_test_wait_0
		DEC		_mmc5_operation_in_progress_flag
		LDA		_mini_map_cur_col
		LSR
		LSR
		TAY
		LDA		byte_EB753,Y
		JSR		_mini_map_ppu_pos_queue
loc_EAB8B:
		JSR		_mini_map_line_draw
		LDX		_tmp2
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INC		_tmp2
		LDX		_tmp3
		LDA		_tmp5
		STA		_extnt_queue_buf,X
		INC		_tmp3
		INC		_mini_map_cur_col
		LDY		_mini_map_cur_col
		TYA
		CMP		byte_EB758,Y
		BCC		loc_EAB8B
		CMP		#$13
		BNE		loc_EABB8
		LDA		#$00
		STA		_mini_map_cur_col
		JSR		sub_EA6A4
loc_EABB8:
		LDX		_tmp2
		PPUQEND
		LDX		_tmp3
		EXTQEND
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EABC3:
		LDA		_scr_res_control_flags
		BEQ		loc_EABC9
		RTS
loc_EABC9:
		JSR		sub_EAC82
		JMP		loc_EABF6
loc_EABCF:
		JSR		_wait_for_nmi
		BIT		_pad0_autorep_press
		BMI		loc_EAC3C
		BVS		loc_EAC0A
		LDA		_pad0_autorep_press
		AND		#$0F
		BEQ		loc_EABCF
		TAX
		LDA		#$01
		STA		_apu_se_flags
		LDY		_cur_info_maps_button_idx
		LDA		byte_EB6A9,Y
		TAY
		LDA		_info_maps_city_map_modes_idx,Y
		CLC
		ADC		byte_EAC79,X
		AND		#$03
		STA		_info_maps_city_map_modes_idx,Y
loc_EABF6:
		FJSRA	sub_B962F,PRG7,PRG9
		FJSRA	_scr_res_cursor_draw,PRG6,PRG9
		JSR		_spr_finish
		JMP		loc_EABCF
loc_EAC0A:
		LDY		_cur_info_maps_button_idx
		LDA		byte_EB6A9,Y
		TAY
		LDA		byte_13A
		STA		_info_maps_city_map_modes_idx,Y
		JSR		loc_EAC56
		LDY		_prev_info_maps_button_idx
		LDA		_cur_info_maps_button_idx
		STA		_prev_info_maps_button_idx
		STY		_cur_info_maps_button_idx
		FJSRA	_info_maps_button_redraw,PRG6,PRG9
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		FJSRA	_info_maps_header_and_legend_redraw,PRG6,PRG9
		RTS
loc_EAC3C:
		LDY		_cur_info_maps_button_idx
		BEQ		loc_EAC51
		LDA		byte_EB6A9,Y
		TAY
		LDA		_info_maps_city_map_modes_idx,Y
		AND		#$01
		CLC
		ADC		_cur_info_maps_button_idx
		STA		_cur_info_maps_button_idx
loc_EAC51:
		LDA		#$80
		STA		_scr_res_control_flags
loc_EAC56:
		LDA		#$20
		STA		_apu_se_flags
		FJSRA	_wnd_erase,PRGC,PRG9
		LDA		_cur_info_maps_button_idx
		AND		#$FE
		TAY
		LDA		byte_EB69F,Y
		STA		_cur_game_cursor_scr_pos._COL
		LDA		byte_EB69F+1,Y
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		JMP		_wait_for_nmi
byte_EAC79:
		.BYTE	$00,$01,$FF,$00,$02,$00,$00,$00,$FE

; =============== S U B	R O U T	I N E =======================================
sub_EAC82:
		LDA		#$40
		STA		_apu_se_flags
		STA		_game_msg_wnd_active_flag
		LDA		#$10
		STA		_spr_buf_pos
		JSR		_spr_finish
;		FJSRA	_scr_pal_bg_load,PRG6,PRG9
		JSR		_scr_pal_bg_load				; FIX, now local
		PUSHB	_cur_info_maps_button_idx
		LDA		#$0D
		STA		_cur_info_maps_button_idx
		FJSRA	_info_maps_header_and_legend_redraw,PRG6,PRG9
		POPB	_cur_info_maps_button_idx
		LDY		_cur_info_maps_button_idx
		LDA		byte_EB6A9,Y
		TAY
		LDA		_info_maps_city_map_modes_idx,Y
		STA		byte_13A
		LDA		#$00
		STA		_wnd_lvl_idx
		LDY		_cur_info_maps_button_idx
		LDA		byte_EB695,Y
		STA		_wnd_idx
		LDX		byte_EB695+1,Y
		LDA		byte_EB6B2,X
		STA		_wnd_left_tile
		LDA		byte_EB6B2+1,X
		STA		_wnd_top_tile
		FJSRA	_wnd_draw,PRGC,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EACD9:
		LDA		byte_107
		BEQ		sub_EACE4
		LDA		#$00
		STA		byte_107
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EACE4:
		LDA		#$00
		STA		_mini_map_cur_row
loc_EACE9:
		PPUQSTART
		LDA		_mini_map_cur_row
		ASL
		PHA
		TAY
		LDA		_mini_map_ppu_col_ofs_list,Y
		LDY		_scr_res_idx
		CPY		#$0A
		BCC		loc_EACFD
		SBC		#$06
loc_EACFD:
		STA		_ppu_queue_buf,X
		STA		_ptr0
		PLA
		TAY
		INX
		LDA		_mini_map_ppu_col_ofs_list+1,Y
		STA		_ppu_queue_buf,X
		STA		_ptr0+1
		INX
		PPUQMOVB	#$13,OP_NOP,0
		TAY
		LDA		#$DF
loc_EAD17:
		PPUQMOVA
		DEY
		BNE		loc_EAD17
		PPUQEND
		EXTQSTART
		EXTQMOVB	_ptr0,OP_NOP,0
		EXTQMOVB	_ptr0+1,OP_ADD,#$38
		EXTQMOVB	#$13,OP_NOP,0
		TAY
		LDA		#$01
loc_EAD3A:
		EXTQMOVA
		DEY
		BNE		loc_EAD3A
		EXTQEND
		LDA		_scr_res_idx
		CMP		#$0D
		BCC		loc_EAD58
		LDA		_mini_map_cur_row
		CMP		#$08
		BCC		loc_EAD58
		FJSRA	sub_B966E,PRG7,PRG9
loc_EAD58:
		JSR		_wait_for_nmi
		INC		_mini_map_cur_row
		LDA		_mini_map_cur_row
		CMP		#$13
		BCC		loc_EACE9
		LDA		#$00
		STA		_mini_map_cur_row
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_date_HUD_draw:
		LDA		_hud_redraw_req
		BEQ		locret_EADB1
		LDA		#$00
		STA		_hud_redraw_req

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_info_maps_date_HUD_draw_ex:
		MOVW	_ptr0,_city._cur_year
		JSR		_hex_to_dec9999
		LDA		#$15
		JSR		_printf_num_lib
		PPUQSTART
		PPUQMOVB	#$7B,OP_NOP,0
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
_info_maps_popul_HUD_draw:
		MOVD	_ptr0,_popul_cur
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
;		LDA		#$00						; ORIGINAL: hides sprites to upper left corner
		LDA		#$FA						; FIX: hides sprites out of screen
		STA		_spr_buf+$20,X
		DEX
		BPL		loc_EADD7
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_arrows_move:
		LDA		#$28
		STA		_scr_res_control_flags
		STA		_game_core_state
		LDA		_pad0_autorep_press
		AND		#$0F
		BEQ		loc_EADF4
		LDX		#$04
		STX		_pad0_dpad_autorep_delay
		BNE		loc_EAE02
loc_EADF4:
		LDA		_pad0_held
		AND		#$0F
		BEQ		loc_EAE05
		LDX		_pad0_dpad_autorep_delay
		BEQ		loc_EAE02
		DEC		_pad0_dpad_autorep_delay
		BNE		loc_EAE05
loc_EAE02:
		JSR		sub_EAE0D
loc_EAE05:
		FJSRA	_info_maps_spr_arrows_draw,PRG7,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EAE0D:
		LDX		#$01
		LSR
		BCS		loc_EAE19
		LSR
		BCS		loc_EAE28
		DEX
		LSR
		BCC		loc_EAE28
loc_EAE19:
		LDA		_info_maps_rect_pos,X
		CMP		byte_EB751,X
		BCS		locret_EAE36
		CLC
		ADC		#$02
		STA		_info_maps_rect_pos,X
		RTS
loc_EAE28:
		LDA		_info_maps_rect_pos,X
		CMP		byte_EB74F,X
		BEQ		locret_EAE36
		SEC
		SBC		#$02
		STA		_info_maps_rect_pos,X
locret_EAE36:
		RTS

; =============== S U B	R O U T	I N E =======================================
_info_maps_city_name_HUD_draw:
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
_far_scr_res_hndl1_A_graphs_draw:
		LDA		#$00
		STA		_MMC5_PRG_PAGES
		INC		_scr_res_idx
;		FJSRA	_scr_res_hndl1_A_graphs_draw_init,PRG6,PRG9		; REDUNDANT SO MUCH!
		LDY		#$FF											; OPTIMIZED, this is what
		STY		_cur_graphs_button_press_idx										; to be far called lol
		STY		_prev_graphs_button_press_idx										; -
		FJSRA	_scr_res_hndl1_A_graphs_draw,PRG7,PRG9
		LDA		#$04
		STA		_MMC5_PRG_PAGES
		RTS

; =============== S U B	R O U T	I N E =======================================
_far_scr_res_hndl1_B_graphs_input:
		LDA		#$00
		STA		_MMC5_PRG_PAGES
		FJSRA	_scr_res_hndl1_B_graphs_input,PRG7,PRG9
		LDA		#$04
		STA		_MMC5_PRG_PAGES
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl2_A_budget_draw:
		LDA		_scr_res_idx
		CMP		#$04
		BEQ		loc_EAEEC
		RTS
loc_EAEEC:
		DEC		_mmc5_operation_in_progress_flag
		LDA		#$02
		STA		_MMC5_CHR_MODE
		LDA		#$80
		STA		_fiscal_year_results_draw_req
		STA		_game_core_state
		LDA		#$AC
		STA		_cur_game_cursor_scr_pos._COL
		LDA		#$34
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$00
		STA		_budget_tax_cur_pos
		STA		_pad0_autorep_press
		STA		_tmp118+2
		LDA		#$A0
		STA		_scr_res_control_flags
		MOVD	_ptr0,_taxes_collected
		JSR		_hex_to_dec99999
		LDA		#$01
		JSR		_printf_num_lib
		MOVD	_ptr0,_city._money
		JSR		_hex_to_dec9999999
		LDA		#$12
		JSR		_printf_num_lib
		MOVW	_ptr0,_fund_request_fire
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
		BIT		_city._bank_flags
		BPL		loc_EAFD0
		MOVWO	_ptr0,byte_EB7C3
		MOVWO	_tmp2,word_EB80C
		LDA		#$FF
		JSR		sub_EA366
		JSR		_buffers_force_flush
		BIT		_city._bank_flags
		BVC		loc_EAFBB
		MOVWO	_ptr0,word_EB801
		MOVWO	_tmp2,word_EB84A
		JMP		loc_EAFCB
loc_EAFBB:
		MOVWO	_ptr0,word_EB7FC
		MOVWO	_tmp2,word_EB845
loc_EAFCB:
		LDA		#$FF
		JSR		sub_EA366
loc_EAFD0:
		LDY		#$00
		LDX		#$03
		CLC
loc_EAFD5:
		LDA		_taxes_collected,Y
		ADC		_city._money,Y
		STA		_money_plus_taxes_collected,Y
		INY
		DEX
		BNE		loc_EAFD5
		JSR		_tax_alloc_calc
		JSR		_total_funds_calc
		JSR		_test_if_money_for_funds_are_enough
		BCS		loc_EAFF8
		LDA		#$08
		JSR		_tax_alloc_clamp
		JSR		_total_funds_calc
		JSR		_funding_rates_recalc
loc_EAFF8:
		LDY		#$00
		STY		_budget_tax_cur_pos
loc_EAFFD:
		JSR		_taxes_alloc_draw
		LDY		_budget_tax_cur_pos
		INY
		INY
		STY		_budget_tax_cur_pos
		CPY		#$0A
		BCC		loc_EAFFD
		JSR		_buffers_force_flush
		LDA		#$01
		STA		_MMC5_CHR_MODE
		FJSRA	_wnd_header_year_draw,PRG7,PRG9
		INC		_scr_res_idx
		LDA		#$00
		STA		_budget_display_timer
		STA		_budget_display_timer+1
		STA		_budget_tax_cur_pos
		FJSRA	_budget_screen_cursor_pos_set,PRG7,PRG9
		FJSRA	_budget_screen_clock_spr_animate,PRG7,PRG9
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_far_scr_res_hndl2_B_budget_input:
		FJSRA	_scr_res_hndl2_B_budget_input,PRG7,PRG9
		RTS

; =============== S U B	R O U T	I N E =======================================
_buffers_force_flush:
		JSR		_ppu_dequeue					; do dequeue of nt data
loc_EB043:
		JSR		_extnt_dequeue					; then extnt
		LDA		_ppu_queue_pos_tail				; if nt not flushed, repeat both
		CMP		_ppu_queue_pos_head
		BNE		_buffers_force_flush			; if flushed, here only extnt may
		LDA		_extnt_queue_pos_tail			; need to flush, so let's optimize
		CMP		_extnt_queue_pos_head			; it and jump only to extnt dequeue
		BNE		loc_EB043
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl3_A_city_eval_draw:
		LDA		_scr_res_idx
		CMP		#$06
		BEQ		loc_EB05B
		RTS
loc_EB05B:
		LDA		#$02
		STA		_MMC5_CHR_MODE
		FJSRA	_wnd_header_year_draw,PRG7,PRG9
		LDA		#$00
		STA		_tmp13D
		LDA		#$80
		STA		_scr_res_control_flags
		INC		_scr_res_idx
		LDX		_city._city_type
		LDA		#$04
		JSR		_printf_txt_lib
		LDX		_city._difficult
		LDA		#$05
		JSR		_printf_txt_lib
loc_EB084:
		LDA		#$00
		STA		_ptr0+1
		STA		_tmp2
		LDA		_tmp13D
		ASL
		TAX
		LDA		_city_eval_vars_list,X
		STA		_tmp4
		LDA		_city_eval_vars_list+1,X
		STA		_tmp5
		LDY		_tmp13D
		LDX		_scr06_city_eval_len_list,Y
		LDY		#$00
		CPX		#$01
		BNE		loc_EB0A9
		LDA		(_tmp4),Y
		BMI		loc_EB0EA
loc_EB0A9:
		LDA		(_tmp4),Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EB0A9
		DEY
		LDA		(_tmp4),Y
		BPL		loc_EB0D4
		LDY		_tmp13D
		CPY		#$03
		BEQ		loc_EB0C2
		CPY		#$06
		BNE		loc_EB0D4
loc_EB0C2:
		LDX		_scr06_city_eval_len_list,Y
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
		JSR		_hex_to_dec999999
		LDA		_tmp13D
		CLC
		ADC		#$17
		JSR		_printf_num_lib
		INC		_tmp13D
		LDA		_tmp13D
		CMP		#$0B
		BCC		loc_EB084
loc_EB0EA:
		LDA		#$0B
		STA		_tmp13D
loc_EB0EF:
		LDA		_tmp13D
		SEC
		SBC		#$0B
		STA		_tmp7
		TAX
		LDA		_worst_problem_list,X
		BMI		loc_EB10D
		TAX
		LDA		_tmp7
		JSR		_printf_txt_lib
		INC		_tmp13D
		LDA		_tmp13D
		CMP		#$0F
		BCC		loc_EB0EF
loc_EB10D:
		JSR		_buffers_force_flush
		LDA		#$01
		STA		_MMC5_CHR_MODE
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl3_B_city_eval_input:
		BIT		_pad0_autorep_press
		BVC		locret_EB11F
		LDA		#$40
		STA		_scr_res_control_flags
locret_EB11F:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl4_A_city_overview_init:
		PUSHB	_cur_info_maps_button_idx
		LDA		#$0B
		STA		_cur_info_maps_button_idx
		JSR		_thread_lock_flag_set
		POPB	_cur_info_maps_button_idx
		INC		_game_core_state
		LDA		#$00
		STA		_tmp118+4
		INC		_scr_res_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl4_B_city_overview_input:
		LDA		_tmp118+4
		CMP		#$21
		BCS		loc_EB153
		JSR		sub_EB162
		LDA		_tmp118+4
		CLC
		ADC		#$01
		STA		_tmp118+4
		CMP		#$21
		BCC		locret_EB161
loc_EB153:
		BIT		_pad0_autorep_press
		BVC		locret_EB161
		LDA		#$40
		STA		_scr_res_control_flags
		LDA		#$00
		STA		_game_core_state
locret_EB161:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EB162:
		CMP		#$0E
		BCC		loc_EB1A7
		BEQ		loc_EB1C5
		CMP		#$0F
		BEQ		locret_EB177
		CMP		#$13
		BCC		loc_EB1DC
		CMP		#$1B
		BCC		loc_EB178
		JMP		loc_EB214
locret_EB177:
		RTS
loc_EB178:
		LDA		_tmp118+4
		SEC
		SBC		#$13
		AND		#$FE
		TAX
		LDA		_parks_counter,X
		STA		_ptr0
		LDA		_parks_counter+1,X
		STA		_ptr0+1
		LDA		_tmp118+4
		LSR
		BCC		loc_EB198
		LDA		#$90
		LDY		#$16
		JMP		loc_EB226
loc_EB198:
		LSR		_ptr0+1
		ROR		_ptr0
		LSR		_ptr0+1
		ROR		_ptr0
		LSR		_ptr0+1
		ROR		_ptr0
		JMP		loc_EB1B9
loc_EB1A7:
		LDA		_tmp118+4
		JSR		_get_constr_count_by_idx
		LDX		_tmp118+4
		LDA		byte_EB52B,X
		BNE		loc_EB1B9
loc_EB1B5:
		LDA		#$00
		STA		_ptr0+1
loc_EB1B9:
		JSR		_hex_to_dec9999
		LDX		_tmp118+4
		LDA		byte_EB539,X
		JMP		_printf_num_lib
loc_EB1C5:
		LDA		_coal_power_station_count
		CLC
		ADC		_nuke_power_station_count
		STA		_ptr0
		LDA		#$00
		ADC		#$00
		STA		_ptr0+1
		JSR		_hex_to_dec9999
		LDA		#$36
		JMP		_printf_num_lib
loc_EB1DC:
		LDA		_tmp118+4
		JSR		_get_constr_count_by_idx
		JSR		_hex_to_dec9999
		PUSHB	_ptr0
		LDY		#$03
		LDX		#$00
loc_EB1EC:
		LDA		_ptr0+1,X
		STA		_ptr0,X
		INX
		DEY
		BNE		loc_EB1EC
		LDX		_tmp118+4
		LDA		byte_EB539,X
		JSR		_printf_num_lib
		POPB	_ptr0
		LDA		#$00
		STA		_ptr0+1
		STA		_tmp2
		STA		_tmp3
		LDX		_tmp118+4
		LDA		byte_EB539,X
		CLC
		ADC		#$03
		JMP		_printf_num_lib
loc_EB214:
		LDA		_tmp118+4
		SEC
		SBC		#$1B
		JSR		_get_constr_count_by_idx
		LDA		_stats._total
		LDY		_stats._total+1
		JMP		loc_EB226
loc_EB226:
		PHA
		TYA
		PHA
		LDX		#$64
		JSR		_mmc5_mul16to8_fast
		LDX		#$02
loc_EB230:
		LDA		_ptr0,X
		STA		_tmp3,X
		DEX
		BPL		loc_EB230
		POPB	_ptr0+1
		POPB	_ptr0
		BNE		loc_EB247
		LDA		_ptr0
		BNE		loc_EB247
		LDX		#$00
		BEQ		loc_EB24E
loc_EB247:
		LDA		#$00
		STA		_tmp2
		JSR		_div24
loc_EB24E:
		STX		_ptr0
		JMP		loc_EB1B5

; =============== S U B	R O U T	I N E =======================================
_get_constr_count_by_idx:
		ASL
		TAX
		LDA		off_EB26C,X
		STA		_ptr0
		LDA		off_EB26C+1,X
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		TAX
		INY
		LDA		(_ptr0),Y
		STA		_ptr0+1
		STX		_ptr0
		RTS
off_EB26C:
		.WORD	_RCI_counts._R
		.WORD	_RCI_counts._I
		.WORD	_RCI_counts._C
		.WORD	_stats._developed
		.WORD	_stats._undeveloped
		.WORD	_stats._other
		.WORD	_stats._total
		.WORD	_R_schools_count
		.WORD	_R_hospitals_count
		.WORD	_stadiums_count
		.WORD	_fire_stations_count
		.WORD	_police_stations_count
		.WORD	_airports_count
		.WORD	_sea_ports_count
		.WORD	_roads_count
		.WORD	_roads_count
		.WORD	_roads_count
		.WORD	_rails_count
		.WORD	_electrics_count

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
_budget_funds_recalc:
;		FJSRA	_budget_scr_tax_adjust,PRG6,PRG9
		JSR		_budget_scr_tax_adjust			; FIX, now local
		JSR		_total_funds_calc
		JSR		_test_if_money_for_funds_are_enough
		BCS		_taxes_alloc_draw
		LDA		_budget_tax_cur_pos
		JSR		_tax_alloc_clamp
		JSR		_total_funds_calc
		JSR		_funding_rate_calc

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_taxes_alloc_draw:
;		JSR		_taxes_alloc_hex_to_dec9999		; REDUNDANT
		LDA		_budget_tax_cur_pos				; OPTIMIZED, unufication
		AND		#$FE							; -
		TAY										; -
		LDA		_tax_alloc,Y					; -
		STA		_ptr0							; -
		LDA		_tax_alloc+1,Y					; -
		STA		_ptr0+1							; -
		JSR		_hex_to_dec9999					; -
		LDY		_budget_tax_cur_pos
		LDA		byte_EB76C,Y
		JSR		_printf_num_lib
		LDA		_budget_tax_cur_pos
		AND		#$FE
		BEQ		loc_EB328
		JSR		_funding_rate_calc
		LDA		#$00
		STA		_ptr0+1
		LDA		_budget_tax_cur_pos
		LSR
		TAY
		DEY
		LDA		_funding_level,Y
		STA		_ptr0
		JSR		_hex_to_dec9999
		LDY		_budget_tax_cur_pos
		LDA		byte_EB778,Y
		JSR		_printf_num_lib
		JSR		_cash_flow_draw
loc_EB328:
		FJSRA	_save_taxes_data,PRG7,PRG9
		RTS

; FIX, now local
; -
; =============== S U B	R O U T	I N E =======================================
_budget_scr_tax_adjust:
		LDA		_budget_tax_cur_pos				; cur pos corresponds to particular
		AND		#$FE							; tax on screen, bits 1-7, so already
		TAX										; multiplied by 2, just mask lower bit
		LDA		_fund_reqest_vars_list,X		; get the var ofs by idx
		STA		_ptr0
		LDA		_fund_reqest_vars_list+1,X
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y						; read requested fund value if any
		STA		_tmp2							; this will be the max value of funding
		INY
		LDA		(_ptr0),Y
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
		ASL
		TAX										; extract digit idx for adjust, mul2, because of words
		LDA		_budget_tax_cur_pos				; now, lower bit is exactly what arrow is pressed
		LSR										; up or down, so we need to increase or decrease
		BCC		.do_decrease
		ASL										; do increase here
		TAY
		LDA		_tax_alloc,Y					; Y = tax idx * 2
		CLC
		ADC		_inc_table,X					;
		STA		_ptr0
		LDA		_tax_alloc+1,Y
		ADC		_inc_table+1,X
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
		STA		_tax_alloc,Y
		LDA		_tmp3
		STA		_tax_alloc+1,Y
		RTS
.apply_adjust:
		LDA		_ptr0+1							; update increased value
.apply_adjust_ex:
		STA		_tax_alloc+1,Y
		LDA		_ptr0
		STA		_tax_alloc,Y
		RTS
.do_decrease:
		ASL
		TAY
		LDA		_tax_alloc,Y					; the same for decrease
		SEC
		SBC		_inc_table,X
		STA		_ptr0
		LDA		_tax_alloc+1,Y
		SBC		_inc_table+1,X					; test against 0 instead
		BCS		.apply_adjust_ex				; OPTIMIZE a little
;		BCS		loc_A017A
		LDA		#$00
		STA		_tax_alloc,Y					; store minimal value
		STA		_tax_alloc+1,Y
		RTS
;loc_A017A:										; REDUNDANT
;		STA		_tax_alloc+1,Y
;		LDA		_ptr0
;		STA		_tax_alloc,Y
;		RTS

_fund_reqest_vars_list:
		.WORD	_fund_request_tax
		.WORD	_fund_request_fire
		.WORD	_fund_request_police
		.WORD	_fund_request_trans
		.WORD	_fund_request_education
		.WORD	0
_inc_table:
		.WORD	1
		.WORD	10
		.WORD	100
		.WORD	1000
; -

; =============== S U B	R O U T	I N E =======================================
_test_if_money_for_funds_are_enough:
		LDY		#$00
		LDX		#$03
		SEC
loc_EB335:
		LDA		_money_plus_taxes_collected,Y
		SBC		_total_funds,Y
		INY
		DEX
		BNE		loc_EB335
		RTS

; =============== S U B	R O U T	I N E =======================================
_tax_alloc_clamp:
		AND		#$FE
		PHA
		LDX		#$03
		LDY		#$00
		SEC
loc_EB348:
		LDA		_total_funds,Y
		SBC		_money_plus_taxes_collected,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EB348
		PLA
		TAY
loc_EB357:
		LDA		_tax_alloc,Y
		SEC
		SBC		_ptr0
		LDA		_tax_alloc+1,Y
		SBC		_ptr0+1
		LDA		#$00
		SBC		_tmp2
		BCS		loc_EB38B
		LDA		_ptr0
		SEC
		SBC		_tax_alloc,Y
		STA		_ptr0
		LDA		_ptr0+1
		SBC		_tax_alloc+1,Y
		STA		_ptr0+1
		LDA		_tmp2
		SBC		#$00
		STA		_tmp2
		LDA		#$00
		STA		_tax_alloc,Y
		STA		_tax_alloc+1,Y
		DEY
		DEY
		BNE		loc_EB357
loc_EB389:
		BEQ		loc_EB389
loc_EB38B:
		LDA		_tax_alloc,Y
		SEC
		SBC		_ptr0
		STA		_tax_alloc,Y
		LDA		_tax_alloc+1,Y
		SBC		_ptr0+1
		STA		_tax_alloc+1,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_total_funds_calc:
		LDA		#$00
		STA		_ptr0
		STA		_ptr0+1
		STA		_tmp2
		LDY		#$02
loc_EB3A7:
		LDA		_tax_alloc,Y
		CLC
		ADC		_ptr0
		STA		_ptr0
		LDA		_tax_alloc+1,Y
		ADC		_ptr0+1
		STA		_ptr0+1
		LDA		#$00
		ADC		_tmp2
		STA		_tmp2
		INY
		INY
		CPY		#$0A
		BCC		loc_EB3A7
		LDY		#$02
loc_EB3C4:
		LDA		_ptr0,Y
		STA		_total_funds,Y
		STA		_total_funds_tmp,Y
		DEY
		BPL		loc_EB3C4
		BIT		_city._bank_flags
		BVC		locret_EB3E7
		LDX		#$03
		LDY		#$00
		CLC
loc_EB3DA:
		LDA		_ptr0,Y
		ADC		_bank_loan_amount,Y
		STA		_total_funds_tmp,Y
		INY
		DEX
		BNE		loc_EB3DA
locret_EB3E7:
		RTS
_bank_loan_amount:
		.BYTE	$F4,$01,$00

; =============== S U B	R O U T	I N E =======================================
_funding_rates_recalc:
		LDA		#$02
		STA		_budget_tax_cur_pos
loc_EB3F0:
		JSR		_funding_rate_calc
		JSR		_wait_for_nmi
		INC		_budget_tax_cur_pos
		INC		_budget_tax_cur_pos
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCC		loc_EB3F0
		RTS

; =============== S U B	R O U T	I N E =======================================
_cash_flow_draw:
		JSR		_cash_flow_calc
		JSR		_hex_to_dec9999999
		LDA		#$11
		JSR		_printf_num_lib
		LDX		#$03
		LDY		#$00
		SEC
loc_EB414:
		LDA		_money_plus_taxes_collected,Y
		SBC		_total_funds_tmp,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EB414
		LDA		_tmp2
		BPL		loc_EB428
		INC		byte_6CB
loc_EB428:
		JSR		_hex_to_dec9999999
		LDA		#$13
		JMP		_printf_num_lib

; =============== S U B	R O U T	I N E =======================================
_flush_wait_0:
		JSR		_wait_for_nmi

; =============== S U B	R O U T	I N E =======================================
_flush_test_wait_0:
		LDA		_extnt_queue_pos_head
		CMP		_extnt_queue_pos_tail
		BNE		_flush_wait_0
		LDA		_ppu_queue_pos_head
		CMP		_ppu_queue_pos_tail
		BNE		_flush_wait_0
		RTS

; =============== S U B	R O U T	I N E =======================================
_thread_lock_flag_set:
		LDA		_cur_info_maps_button_idx
		ORA		#$80
		STA		_thread_priority_req
		JMP		_wait_for_nmi

; =============== S U B	R O U T	I N E =======================================
_tax_alloc_calc:
		LDA		#$02
		STA		_budget_tax_cur_pos
loc_EB450:
		AND		#$FE
		TAY
		LDA		_fund_reqest_ofs_list,Y
		STA		_ptr0
		LDA		_fund_reqest_ofs_list+1,Y
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		TAX
		INY
		LDA		(_ptr0),Y
		STA		_ptr0+1
		STX		_ptr0
		LDA		_budget_tax_cur_pos
		LSR
		TAY
		DEY
		LDA		_funding_level,Y
		TAX
		JSR		_mmc5_mul16to8_fast
		LDY		#$02
loc_EB478:
		LDA		_ptr0,Y
		STA		_tmp3,Y
		DEY
		BPL		loc_EB478
		LDA		#$00
		STA		_tmp2
		STA		_ptr0+1
		LDA		#$64
		STA		_ptr0
		JSR		_div24
		LDA		_budget_tax_cur_pos
		AND		#$FE
		TAY
		TXA
		STA		_tax_alloc,Y
		LDA		_tmp6
		STA		_tax_alloc+1,Y
		JSR		_wait_for_nmi
		INC		_budget_tax_cur_pos
		INC		_budget_tax_cur_pos
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCC		loc_EB450
		RTS

; =============== S U B	R O U T	I N E =======================================
_funding_rate_calc:
		LDA		_budget_tax_cur_pos
		AND		#$FE
		TAY
		PHA
		LDA		_tax_alloc,Y
		STA		_ptr0
		LDA		_tax_alloc+1,Y
		STA		_ptr0+1
		LDX		#byte_64
		JSR		_mmc5_mul16to8_fast
		LDY		#$02
loc_EB4C6:
		LDA		_ptr0,Y
		STA		_tmp3,Y
		DEY
		BPL		loc_EB4C6
		PLA
		TAY
		LDA		_fund_reqest_ofs_list,Y
		STA		_ptr0
		LDA		_fund_reqest_ofs_list+1,Y
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		TAX
		INY
		LDA		(_ptr0),Y
		STA		_ptr0+1
		BNE		loc_EB4EF
		CPX		#$00
		BNE		loc_EB4EF
		LDX		#byte_64
		BNE		loc_EB4F8
loc_EB4EF:
		STX		_ptr0
		LDA		#$00
		STA		_tmp2
		JSR		_div24
loc_EB4F8:
		LDA		_budget_tax_cur_pos
		LSR
		TAY
		DEY
		TXA
		STA		_funding_level,Y
		RTS

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

; =============== S U B	R O U T	I N E =======================================
_adc_ptr0_0:
		CLC
		ADC		_ptr0
		STA		_ptr0
		BCC		locret_EB52A
		INC		_ptr0+1
locret_EB52A:
		RTS

byte_EB52B:
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$00,$00,$00,$00,$00
byte_EB539:
		.BYTE	$22,$24,$26,$28,$2A,$2C,$2E,$2F,$30,$31,$32,$33,$34,$35,$36,$3F
		.BYTE	$40,$41,$42,$37,$38,$39,$3A,$3B,$3C,$3D,$3E,$23,$25,$27,$29,$2B
		.BYTE	$2D
byte_EB55A:
		.BYTE	$5F,$5F,$28,$28,$00,$00,$50,$50,$28,$28,$50,$50,$00,$00,$50,$50
byte_EB56A:
		.BYTE	$10,$10,$10,$10,$10,$10,$D0,$D0,$D0,$D0,$50,$50,$D0,$D0,$90,$90
word_EB57A:
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
		.WORD	_tmp_recalc_bufs
		.WORD	_power_grid_map_buf
		.WORD	_pop_dencity_map_buf
		.WORD	_pop_growth_map_buf
		.WORD	_traffic_density_map_buf
		.WORD	_pollution_map_buf
		.WORD	_crime_level_map_buf
		.WORD	_land_value_map_buf
		.WORD	_fire_radius_map_buf
		.WORD	_police_radius_map_buf
; REDUNDANT
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
_mini_map_wram_ofs_list:
		.WORD	_wram_map_buf
		.WORD	_wram_map_buf+$130
		.WORD	_wram_map_buf+$260
		.WORD	_wram_map_buf+$390
		.WORD	_wram_map_buf+$4C0
		.WORD	_wram_map_buf+$5F0
		.WORD	_wram_map_buf+$720
		.WORD	_wram_map_buf+$850
		.WORD	_wram_map_buf+$980
		.WORD	_wram_map_buf+$AB0
		.WORD	_wram_map_buf+$BE0
		.WORD	_wram_map_buf+$D10
		.WORD	_wram_map_buf+$E40
		.WORD	_wram_map_buf+$F70
		.WORD	_wram_map_buf+$10A0
		.WORD	_wram_map_buf+$11D0
		.WORD	_wram_map_buf+$1300
		.WORD	_wram_map_buf+$1430
		.WORD	_wram_map_buf+$1560
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
byte_EB695:
		.BYTE	$09,$00
		.BYTE	$07,$02
		.BYTE	$00,$00
		.BYTE	$00,$00
		.BYTE	$08,$04
byte_EB69F:
		.BYTE	$24,$48
		.BYTE	$24,$68
		.BYTE	$00,$00
		.BYTE	$00,$00
		.BYTE	$24,$B8
byte_EB6A9:
		.BYTE	$00,$FF,$01,$FF,$FF,$FF,$FF,$FF,$02
byte_EB6B2:
		.BYTE	$07,$04
		.BYTE	$07,$0A
		.BYTE	$07,$11

; unused
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
byte_EB743:
		.BYTE	$00,$80,$00,$00,$80,$80,$80,$80,$00,$00,$40,$40
byte_EB74F:
		.BYTE	$3C,$45
byte_EB751:
		.BYTE	$AA,$AB
byte_EB753:
		.BYTE	$04,$04,$04,$04,$03
byte_EB758:
		.BYTE	$04,$04,$04,$04,$04,$08,$08,$08,$08,$0C,$0C,$0C,$0C,$10,$10,$10
		.BYTE	$10,$13,$13,$13
byte_EB76C:
		.BYTE	$00,$00,$07,$07,$08,$08,$09,$09,$0A,$0A,$0B,$0B
byte_EB778:
		.BYTE	$00,$00,$0C,$0C,$0D,$0D,$0E,$0E,$0F,$0F,$10,$10
byte_EB784:
		.BYTE	$E0,$D8,$D0
byte_EB787:
		.BYTE	$D0,$02,$EA,$AE,$D8,$02,$EB,$AE,$E0,$02,$EC,$AE,$D0,$01,$ED,$B6
		.BYTE	$D8,$01,$ED,$B6,$E0,$01,$EE,$B6,$D0,$01,$EF,$C6,$D8,$01,$EF,$C6
		.BYTE	$E0,$01,$F0,$C6,$D0,$03,$F1,$CE,$D8,$03,$F1,$CE,$E0,$03,$F1,$CE
		.BYTE	$D0,$03,$F6,$D6,$D8,$03,$F6,$D6,$E0,$03,$F6,$D6
byte_EB7C3:
		.BYTE	$84,$26,$04,$7B,$7C,$7D,$7E,$90,$26,$01,$24,$79,$26,$03,$10,$11
		.BYTE	$12,$99,$26,$03,$20,$21,$22,$B9,$26,$03,$30,$31,$32,$D8,$26,$06
		.BYTE	$13,$14,$15,$16,$17,$18,$F8,$26,$06,$23,$24,$25,$26,$27,$28,$18
		.BYTE	$27,$06,$33,$34,$35,$36,$37,$38,$FF
word_EB7FC:
		.WORD	$2696
		.BYTE	$01,$00
		.BYTE	$FF
word_EB801:
		.WORD	$268F
		.BYTE	$01,$7F
		.WORD	$2694
		.BYTE	$03,$05,$00,$00
		.BYTE	$FF
word_EB80C:
		.WORD	$5E84
		.BYTE	$04,$45,$45,$45,$45
		.WORD	$5E90
		.BYTE	$01,$0B
		.WORD	$5E79
		.BYTE	$03,$D7,$D7,$D7
		.WORD	$5E99
		.BYTE	$03,$D7,$D7,$D7
		.WORD	$5EB9
		.BYTE	$03,$D7,$D7,$D7
		.WORD	$5ED8
		.BYTE	$06,$17,$17,$17,$17,$17,$17
		.WORD	$5EF8
		.BYTE	$06,$17,$17,$17,$17,$17,$17
		.WORD	$5F18
		.BYTE	$06,$17,$17,$17,$17,$17,$17
		.BYTE	$FF
word_EB845:
		.WORD	$5E96
		.BYTE	$01,$05
		.BYTE	$FF
word_EB84A:
		.WORD	$5E8F
		.BYTE	$01,$05
		.WORD	$5E94
		.BYTE	$03,$05,$05,$05
		.BYTE	$FF

; =============== S U B	R O U T	I N E =======================================
_printf_txt_lib:
		STA		_tmp7
		STX		_tmp6
		PPUQSTART
;		ASL										; REDUNDANT, optimized
		TAY
		LDA		_txt_ppu_ofs_lib_lo,Y
		PPUQMOVA
		LDA		_txt_ppu_ofs_lib_hi,Y
		PPUQMOVA
;		LDY		_tmp7							; REDUNDANT, optimized
		LDA		byte_EBB49,Y
		STA		_ppu_queue_buf,X
		STA		_tmp5
		INX
;		LDA		_tmp7							; REDUNDANT, optimized
;		ASL										; REDUNDANT, optimized
;		TAY										; REDUNDANT, optimized
		LDA		_txt_libs_lo,Y
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
		ADC		byte_EBB56,Y
		DEC		_tmp4
		BNE		loc_EB88F
loc_EB896:
		TAY
loc_EB897:
		LDA		(_ptr0),Y
		PPUQMOVA
		INY
		DEC		_tmp5
		BNE		loc_EB897
		PPUQEND
		EXTQSTART
		LDY		_tmp7
		CPY		#$06
		BCS		locret_EB8F3
;		ASL									; REDUNDANT, optimized
;		TAY									; REDUNDANT, optimized
		LDA		_txt_ppu_ofs_lib_lo,Y
		EXTQMOVA
		LDA		_txt_ppu_ofs_lib_hi,Y
		CLC
		ADC		#$38
		EXTQMOVA
;		LDY		_tmp7						; REDUNDANT, optimized
		LDA		byte_EBB49,Y
		STA		_extnt_queue_buf,X
		STA		_tmp5
		INX
;		LDA		_tmp7
;		ASL
;		TAY
		LDA		_txt_len_lib_lo,Y
		STA		_ptr0
		LDA		_txt_len_lib_hi,Y
		STA		_ptr0+1
		LDY		_tmp6
		LDA		_tmp13D
		CMP		#$0B
		BNE		loc_EB8E7
		LDA		(_ptr0),Y
		ORA		#$80
		BNE		loc_EB8E9
loc_EB8E7:
		LDA		(_ptr0),Y
loc_EB8E9:
		EXTQMOVA
		DEC		_tmp5
		BNE		loc_EB8E9
		EXTQEND
locret_EB8F3:
		RTS

; =============== S U B	R O U T	I N E =======================================
_printf_num_lib:
		STA		_tmp118
;		CMP		#$06					; REDUNDANT: never used, should disable
;		BEQ		locret_EB903			; some screen positions in the table
;		CMP		#$0B					; but why you need to call them if
;		BEQ		locret_EB903			; they aren't displayed anyway?
;		CMP		#$10
;		BNE		loc_EB904
;locret_EB903:
;		RTS
;loc_EB904:
		TAY
		LDX		_printf_num_lib_spaces_list,Y
		JSR		_printf_num_lib_number_trim
		LDX		_tmp118
loc_EB90E:
		LDA		_ptr0,Y
		CLC
		ADC		_printf_num_tile_ofs_list,X
		STA		_ptr0,Y
		DEY
		BPL		loc_EB90E
		PPUQSTART
		LDY		_tmp118					; FIX: optimized
;		ASL								; REDUNDANT, optimized
;		TAY								; REDUNDANT, optimized
		LDA		_printf_num_screen_pos_list_lo,Y
		PPUQMOVA
		LDA		_printf_num_screen_pos_list_hi,Y
		PPUQMOVA
;		LDY		_tmp118					; REDUNDANT, optimized
		LDA		_printf_num_len_list,Y
		PPUQMOVA
		TAY
		DEY
loc_EB93C:
		LDA		_ptr0,Y
		PPUQMOVA
		DEY
		BPL		loc_EB93C
		PPUQEND
		JSR		sub_EB980
		EXTQSTART
		LDY		_tmp118
		CPY		#$22
		BCS		locret_EB97F
;		ASL								; REDUNDANT, optimized
;		TAY								; REDUNDANT, optimized
		LDA		_printf_num_screen_pos_list_lo,Y
		EXTQMOVA
		LDA		_printf_num_screen_pos_list_hi,Y
		CLC
		ADC		#$38
		EXTQMOVA
;		LDY		_tmp118					; REDUNDANT, optimized
		LDA		_printf_num_len_list,Y
		EXTQMOVA
		TAY
		DEY
loc_EB973:
		LDA		_ptr0,Y
		EXTQMOVA
		DEY
		BPL		loc_EB973
		EXTQEND
locret_EB97F:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_EB980:
		LDA		#$00
		STA		_tmp118+3
		LDX		_tmp118
		LDA		_printf_num_len_list,X
		TAY
		DEY
loc_EB98D:
		LDA		_ptr0,Y
		BMI		loc_EB9B2
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
		LDA		byte_EBE77,X
		JMP		loc_EB9B8
loc_EB9B2:
		LDX		_tmp118
		LDA		byte_EBE9F,X
loc_EB9B8:
		STA		_ptr0,Y
		DEY
		BPL		loc_EB98D
		RTS

; =============== S U B	R O U T	I N E =======================================
_printf_num_lib_number_trim:
		LDA		#$00
		STA		_tmp7
		LDY		_tmp118
		LDA		_printf_num_len_list,Y
		TAY
		DEY
loc_EB9CB:
		LDA		_ptr0,Y
		BNE		loc_EB9DA
		STX		_ptr0,Y
		DEY
		BPL		loc_EB9CB
		INY
		LDX		#$00
		STX		_ptr0,Y
loc_EB9DA:
		LDX		_tmp118
		LDA		#$24
		CPX		#$1B
		BEQ		loc_EB9F9
		LDA		#$2D
		CPX		#$1A
		BEQ		loc_EB9F4
		CPX		#$1D
		BNE		locret_EB9FE
		LDX		_annual_score_change+1
		BPL		locret_EB9FE
		BMI		loc_EB9F9
loc_EB9F4:
		LDX		_popul_net_migration+2
		BPL		locret_EB9FE
loc_EB9F9:
		INY
		STA		_ptr0,Y
		DEY
locret_EB9FE:
		RTS

; =============== S U B	R O U T	I N E =======================================
; input args: 16bit _ptr0 * reg X, store 24bit _ptr0/tmp2
;
_mmc5_mul16to8_fast:
		LDA		_ptr0+1
		STA		_MMC5_MUL0
		STX		_MMC5_MUL1
		LDA		_MMC5_MUL0
		STA		_tmp2
		LDA		_MMC5_MUL1
		STA		_tmp3
		LDA		_ptr0
		STA		_MMC5_MUL0
;		LDA		#$64					; FIX: WTF LDA $64 redundant
		LDA		_MMC5_MUL0
		STA		_ptr0
		LDA		_MMC5_MUL1
		CLC
		ADC		_tmp2
		STA		_ptr0+1
		LDA		_tmp3
		ADC		#$00
		STA		_tmp2
		RTS

; =============== S U B	R O U T	I N E =======================================
_cash_flow_calc:
		LDA		#$DC
		STA		_tmp6
		LDY		#$00
		LDX		#$03
		SEC
loc_EBAB4:
		LDA		_taxes_collected,Y
		SBC		_total_funds,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EBAB4
		BCS		loc_EBAD9
		LDA		#$7F
		STA		_tmp6
		LDY		#$00
		LDX		#$03
		SEC
loc_EBACC:
		LDA		_total_funds,Y
		SBC		_taxes_collected,Y
		STA		_ptr0,Y
		INY
		DEX
		BNE		loc_EBACC
loc_EBAD9:
		PPUQSTART
		PPUQMOVB	#$AF,OP_NOP,0
		PPUQMOVB	#$26,OP_NOP,0
		PPUQMOVB	#$01,OP_NOP,0
		PPUQMOVB	_tmp6,OP_NOP,0
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_div24:
		JSR		loc_EBB02
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
byte_EBB49:
		.BYTE	$09,$09,$09,$09,$08,$04,$12,$12,$12,$12,$12,$12,$12
byte_EBB56:
		.BYTE	$09,$09,$09,$09,$08,$04,$12,$12,$12,$12,$12,$12,$12
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
_txt_len_lib_lo:
		.BYTE	<[_txt_problems_len_list]
		.BYTE	<[_txt_problems_len_list]
		.BYTE	<[_txt_problems_len_list]
		.BYTE	<[_txt_problems_len_list]
		.BYTE	<[_txt_town_names_len_list]
		.BYTE	<[_txt_difficult_len_list]
_txt_len_lib_hi:
		.BYTE	>[_txt_problems_len_list]
		.BYTE	>[_txt_problems_len_list]
		.BYTE	>[_txt_problems_len_list]
		.BYTE	>[_txt_problems_len_list]
		.BYTE	>[_txt_town_names_len_list]
		.BYTE	>[_txt_difficult_len_list]
_txt_town_names_lib:
		.BYTE	$1F,$12,$15,$15,$0A,$10,$0E,$DC
		.BYTE	$1D,$18,$20,$17,$DC,$DC,$DC,$DC
		.BYTE	$0C,$12,$1D,$22,$DC,$DC,$DC,$DC
		.BYTE	$0C,$0A,$19,$12,$1D,$0A,$15,$DC
		.BYTE	$09,$0A,$0B,$0C,$0D,$0E,$10,$11
		.BYTE	$09,$12,$13,$14,$15,$16,$17,$18
_txt_town_names_len_list:
		.BYTE	 $05,$05,$05,$05,$06,$06
_txt_difficult_lib:
		.BYTE	$AD,$AE,$AF,$DC
		.BYTE	$27,$28,$29,$2A
		.BYTE	$24,$25,$26,$DC
_txt_difficult_len_list:
		.BYTE	$04,$05,$05
_txt_problems_lib:
		.BYTE	$0C,$1B,$12,$16,$0E,$DC,$DC,$DC,$DC
		.BYTE	$19,$18,$15,$15,$1E,$1D,$12,$18,$17
		.BYTE	$D0,$D1,$D2,$D3,$D4,$D5,$D6,$D7,$D8
		.BYTE	$1D,$0A,$21,$0E,$1C,$DC,$DC,$DC,$DC
		.BYTE	$1D,$1B,$0A,$0F,$0F,$12,$0C,$DC,$DC
		.BYTE	$00,$01,$02,$03,$04,$05,$06,$07,$08
		.BYTE	$0F,$12,$1B,$0E,$1C,$DC,$DC,$DC,$DC
_txt_problems_len_list:
		.BYTE	$05,$05,$04,$05,$05,$06,$05
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
		.BYTE	<[$0000]
		.BYTE	<[$25B0]
		.BYTE	<[$25D0]
		.BYTE	<[$25F0]
		.BYTE	<[$2610]
		.BYTE	<[$0000]
		.BYTE	<[$25B8]
		.BYTE	<[$25D8]
		.BYTE	<[$25F8]
		.BYTE	<[$2618]
		.BYTE	<[$0000]
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
byte_EBE77:
		.BYTE	$59,$05,$19,$19,$19,$19,$00,$59
		.BYTE	$59,$59,$59,$00,$19,$19,$19,$19
		.BYTE	$00,$05,$05,$05,$8B,$42,$42,$85
		.BYTE	$05,$8B,$05,$05,$05,$05,$85,$05
		.BYTE	$05,$05,$15,$15,$15,$15,$15,$15
byte_EBE9F:
		.BYTE	$05,$05,$05,$05,$05,$05,$05,$05
		.BYTE	$05,$05,$05,$05,$05,$05,$05,$05
		.BYTE	$05,$05,$05,$05,$8B,$01,$01,$05
		.BYTE	$05,$05,$05,$05,$05,$05,$02,$02
		.BYTE	$02,$02,$14,$14,$14,$14,$14,$14
_scr06_city_eval_len_list:
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
_fund_reqest_ofs_list:
		.WORD	0
		.WORD	_fund_request_fire
		.WORD	_fund_request_police
		.WORD	_fund_request_trans
		.WORD	_fund_request_education

; =============== S U B	R O U T	I N E =======================================
;_taxes_alloc_hex_to_dec9999:					; REDUNDANT
;		LDA		_budget_tax_cur_pos				; -
;		AND		#$FE							; -
;		TAY										; -
;		LDA		_tax_alloc,Y					; -
;		STA		_ptr0							; -
;		LDA		_tax_alloc+1,Y					; -
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
		STY		_tmp3						; prepare buffer
		STY		_tmp4
		STY		_tmp5
		STY		_tmp6
		STY		_tmp7
loc_EBA2D:
		LDY		#$00
loc_EBA2F:
		LDA		_tmp0						; from 10m to 10 back count
		CMP		_hex_to_dec_tbl0,X			; compare 24-bit value with table
		LDA		_tmp1
		SBC		_hex_to_dec_tbl1,X
		LDA		_tmp2
		SBC		_hex_to_dec_tbl2,X
		BCC		loc_EBA58
		LDA		_tmp0						; less than or equals, subtract
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

		BANK_END F9,$C000
