
		ORG $A000

		BANK_START $01

; =============== S U B	R O U T	I N E =======================================
_game_setup:
		LDA		#$80
		STA		_apu_mus_idx_req
		JSR		_wait_for_nmi
		LDA		#4
		STA		_apu_mus_idx_req
		LDA		#0
		STA		_tmp6D5
		STA		_tmp6D2
		STA		_tmp6D3
		LDA		_scenario_result_idx
		BNE		loc_2402A
		LDY		_cur_game_mode
		BNE		loc_24024
		LDA		#$80
		STA		_apu_mus_idx_req
loc_24024:
		LDA		_game_setup_subs_list,Y
		STA		_game_setup_sub_idx
loc_2402A:
		JSR		_wait_for_nmi
		JSR		_game_setup_sub_exec
		LDA		_main_sub_idx
		CMP		#1
		BEQ		loc_2402A
		RTS
_game_setup_subs_list:
		.BYTE	$03,$00,$05,$09

; =============== S U B	R O U T	I N E =======================================
_game_setup_sub_exec:
		LDY		_game_setup_sub_idx
		LDA		_game_setup_lib_lo,Y
		STA		_ptr0
		LDA		_game_setup_lib_hi,Y
		STA		_ptr0+1
		JMP		(_ptr0)
_game_setup_lib_lo:
		.BYTE	<[_game_setup00_new_city_gen]
		.BYTE	<[_game_setup01_name_input_init]
		.BYTE	<[_game_setup02_name_input_input]
		.BYTE	<[_game_setup03_dummy]
		.BYTE	<[_game_setup04_break]
		.BYTE	<[_game_setup05_scenario_init]
		.BYTE	<[_game_setup06_scenario_select_input]
		.BYTE	<[_game_setup07_scenario_intro_init]
		.BYTE	<[_game_setup08_scenario_intro_draw]
		.BYTE	<[_game_setup09_practice_msg_init]
		.BYTE	<[_game_setup0A_practice_msg_draw]
		.BYTE	<[_game_setup0B_scenario_over_init]
		.BYTE	<[_game_setup0C_scenario_over_input]
		.BYTE	<[_game_setup0D_difficult_init]
		.BYTE	<[_game_setup0E_difficult_input]
		.BYTE	<[_game_setup0F_all_complete_init]
		.BYTE	<[_game_setup10_all_complete_input]
_game_setup_lib_hi:
		.BYTE	>[_game_setup00_new_city_gen]
		.BYTE	>[_game_setup01_name_input_init]
		.BYTE	>[_game_setup02_name_input_input]
		.BYTE	>[_game_setup03_dummy]
		.BYTE	>[_game_setup04_break]
		.BYTE	>[_game_setup05_scenario_init]
		.BYTE	>[_game_setup06_scenario_select_input]
		.BYTE	>[_game_setup07_scenario_intro_init]
		.BYTE	>[_game_setup08_scenario_intro_draw]
		.BYTE	>[_game_setup09_practice_msg_init]
		.BYTE	>[_game_setup0A_practice_msg_draw]
		.BYTE	>[_game_setup0B_scenario_over_init]
		.BYTE	>[_game_setup0C_scenario_over_input]
		.BYTE	>[_game_setup0D_difficult_init]
		.BYTE	>[_game_setup0E_difficult_input]
		.BYTE	>[_game_setup0F_all_complete_init]
		.BYTE	>[_game_setup10_all_complete_input]

; =============== S U B	R O U T	I N E =======================================
_game_setup05_scenario_init:
		LDA		#1
		STA		_game_core_state
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		LDY		#$80
		STY		_game_wnd_spr_mode_flag
		STY		_ppu_ctrl_shadow
		LDY		#1
		STY		_scr_mode_idx
		INY
		STY		_scr_res_idx
		DEC		_mmc5_operation_in_progress_flag
		LDA		#8
		STA		_MMC5_NT_MODE
		STA		_mmc5_nt_mode_shadow
		LDA		_city._all_scenarios_are_clear_ram_flag
		BNE		loc_240A5
		LDX		#<[_tlm_res_scenario_normal]
		LDY		#>[_tlm_res_scenario_normal]
		JSR		_tlm_res_unpack
		LDY		#3
		BNE		loc_240B2
loc_240A5:
		LDX		#<[_tlm_res_scenario_complete]
		LDY		#>[_tlm_res_scenario_complete]
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$FA
		.WORD	_tlm_res_unpack
		LDY		#4
loc_240B2:
		STY		_tmp6D4
		JSR		_ppu_queues_reset
		LDA		#2
		STA		_MMC5_CHR_MODE
		JSR		_scenario_closed_screen_update
		LDA		#1
		STA		_MMC5_CHR_MODE
		LDA		#0
		STA		_tmp6D0+1
		STA		_tmp6D0
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#2
		STA		_screen_pal_cur_spr_idx
		LDA		#8
		STA		_screen_pal_cur_bg_idx
		INC		_game_setup_sub_idx
		JMP		_pal_fade_in

; =============== S U B	R O U T	I N E =======================================
_scenario_closed_screen_update:
		LDY		#0
		LDX		#6
		LDA		_city._all_scenarios_are_clear_ram_flag
		BEQ		loc_240FA
		LDY		#6
		LDX		#8
		LDA		_city._scenarios_clear_flags
		LSR
		LSR
		LSR
		LSR
		LSR
		LSR
		BPL		loc_240FD
loc_240FA:
		LDA		_city._scenarios_clear_flags
loc_240FD:
		STX		_tmp3
loc_240FF:
		STY		_tmp2
		LSR
		BCC		loc_2411A
		PHA
		LDA		_tmp2
		PHA
		JSR		_scenario_closed_icon_draw
loc_2410B:
		JSR		_ppu_dequeue
		JSR		_extnt_dequeue
		JSR		_ppu_queue_empty_compare
		BNE		loc_2410B
		PLA
		STA		_tmp2
		PLA
loc_2411A:
		LDY		_tmp2
		INY
		CPY		_tmp3
		BNE		loc_240FF
		RTS

; =============== S U B	R O U T	I N E =======================================
_scenario_closed_icon_draw:
		LDX		_ppu_queue_pos_head
		LDA		_scenario_closed_ppu_ofs_hi,Y
		STA		_ptr0+1
		LDA		_scenario_closed_ppu_ofs_lo,Y
		STA		_ptr0
		LDY		#0
loc_24130:
		LDA		_ptr0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		STA		_ppu_queue_buf,X
		INX
		LDA		#6
		STA		_ppu_queue_buf,X
		STA		_tmp3
		INX
loc_24144:
		LDA		_tlm_scenario_closed_nt,Y
		STA		_ppu_queue_buf,X
		INY
		INX
		DEC		_tmp3
		BNE		loc_24144
		LDA		_ptr0
		CLC
		ADC		#$20
		STA		_ptr0
		BCC		loc_2415B
		INC		_ptr0+1
loc_2415B:
		CPY		#$24
		BNE		loc_24130
		STX		_ppu_queue_pos_head
		LDY		_tmp2
		LDX		_extnt_queue_pos_head
		LDA		_scenario_closed_ppu_ofs_hi,Y
		CLC
		ADC		#$3C
		STA		_ptr0+1
		LDA		_scenario_closed_ppu_ofs_lo,Y
		STA		_ptr0
		LDY		#0
loc_24174:
		LDA		_ptr0
		STA		_extnt_queue_buf,X
		INX
		LDA		_ptr0+1
		STA		_extnt_queue_buf,X
		INX
		LDA		#6
		STA		_extnt_queue_buf,X
		STA		_tmp3
		INX
loc_24188:
		LDA		_tlm_scenario_closed_extnt,Y
		STA		_extnt_queue_buf,X
		INY
		INX
		DEC		_tmp3
		BNE		loc_24188
		LDA		_ptr0
		CLC
		ADC		#$20
		STA		_ptr0
		BCC		loc_2419F
		INC		_ptr0+1
loc_2419F:
		CPY		#$24
		BNE		loc_24174
		STX		_extnt_queue_pos_head
		RTS
_scenario_closed_ppu_ofs_lo:
		.BYTE	$E5,$ED,$F5,$05,$0D,$15
_scenario_closed_ppu_ofs_hi:
		.BYTE	$20,$20,$20,$22,$22,$22
_tlm_scenario_closed_nt:
		.BYTE	$E0,$E1,$A1,$A1,$E4,$E5
		.BYTE	$F0,$F1,$F2,$F3,$F4,$F5
		.BYTE	$B0,$E6,$E7,$E8,$E9,$B2
		.BYTE	$B0,$F6,$F7,$F8,$F9,$B2
		.BYTE	$EA,$EB,$EC,$ED,$EE,$EF
		.BYTE	$FA,$FB,$C1,$C1,$FE,$FF
_tlm_scenario_closed_extnt:
		.BYTE	$0B,$0B,$01,$01,$0B,$0B
		.BYTE	$0B,$0B,$0B,$0B,$0B,$0B
		.BYTE	$01,$0B,$0B,$0B,$0B,$01
		.BYTE	$01,$0B,$0B,$0B,$0B,$01
		.BYTE	$0B,$0B,$0B,$0B,$0B,$0B
		.BYTE	$0B,$0B,$01,$01,$0B,$0B

; =============== S U B	R O U T	I N E =======================================
_game_setup06_scenario_select_input:
		JSR		_scenario_select_input
		BIT		_pad0_autorep_press
		BVC		loc_2420C
		LDA		#2
		STA		_title_sub_idx
		DEC		_main_sub_idx
		JMP		_pal_fade_out
loc_2420C:
		JMP		_common_select_cursor_draw

; =============== S U B	R O U T	I N E =======================================
_scenario_select_input:
		LDA		_pad0_autorep_release
		AND		#$F
		BEQ		loc_24219
		LDA		#$40
		STA		_apu_se_flags+1
loc_24219:
		LDX		_tmp6D2
		LDA		_pad0_autorep_release
		LSR
		BCC		loc_24229
		INX
		CPX		_tmp6D4
		BNE		loc_24229
		LDX		#0
loc_24229:
		LSR
		BCC		loc_24233
		DEX
		BPL		loc_24233
		LDX		_tmp6D4
		DEX
loc_24233:
		AND		#3
		BEQ		loc_2423F
		LDA		_tmp6D3
		EOR		#1
		STA		_tmp6D3
loc_2423F:
		STX		_tmp6D2
		LDY		_tmp6D3
		LDA		byte_2427B,Y
		STA		_tmp6CE
		LDA		byte_24274,X
		LDY		_city._all_scenarios_are_clear_ram_flag
		BEQ		loc_24256
		LDA		byte_24277,X
loc_24256:
		STA		_tmp6CC
		LDA		_pad0_autorep_press
		BPL		locret_24273
		LDA		_tmp6D3
		ASL
		ASL
		ADC		_tmp6D2
		TAY
		LDA		byte_2427D,Y
		STA		_scenario_idx
		LDA		#1
		STA		_apu_se_flags
		INC		_game_setup_sub_idx
locret_24273:
		RTS
byte_24274:
		.BYTE	$4D,$8D,$CD
byte_24277:
		.BYTE	$45,$75,$A5,$DD
byte_2427B:
		.BYTE	$5D,$A5
byte_2427D:
		.BYTE	$00,$01,$02,$06,$03,$04,$05,$07

; =============== S U B	R O U T	I N E =======================================
_game_setup07_scenario_intro_init:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		JSR		_spr_clear
		LDX		#<[_tlm_res_fullscreen_wnd]
		LDY		#>[_tlm_res_fullscreen_wnd]
		JSR		_tlm_res_unpack
		LDY		_scenario_idx
		LDA		_scenario_intro_header_lib_lo,Y
		STA		_ptr0
		LDA		_scenario_intro_header_lib_hi,Y
		STA		_ptr0+1
		JSR		_tlm_transfer
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#$11
		STA		_screen_pal_cur_bg_idx
		JSR		_pal_fade_in
		LDY		_scenario_idx
		LDA		_scenario_msg_lib_lo,Y
		STA		_tmp6D6
		LDA		_scenario_msg_lib_hi,Y
		STA		_tmp6D6+1
		LDY		#0
		STY		_tmp6DA
		STY		_tmp6D9
		DEY
		STY		_tmp6D8
		INC		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup08_scenario_intro_draw:
		JSR		_msg_print_out_handler
		BIT		_pad0_autorep_press
		BPL		loc_242EC
		LDA		#1
		STA		_apu_se_flags
		JSR		_scenario_map_load
		INC		_main_sub_idx
		LDA		#$80
		STA		_apu_mus_idx_req
		RTS
loc_242EC:
		BVC		locret_242F7
		DEC		_game_setup_sub_idx
		DEC		_game_setup_sub_idx
		DEC		_game_setup_sub_idx
locret_242F7:
		RTS

; =============== S U B	R O U T	I N E =======================================
_msg_print_out_handler:
		JSR		_msg_print_out

; =============== S U B	R O U T	I N E =======================================
_msg_print_out:
		LDY		_tmp6D8
		CPY		#$FF
		BEQ		loc_2430D
		INC		_tmp6D8
		LDA		_msg_dictionary,Y
		BPL		loc_24347
		STA		_tmp6D8
loc_2430D:
		LDA		_tmp6D6+1
		CMP		#$FF
		BEQ		locret_2436F
		STA		_ptr0+1
		LDA		_tmp6D6
		STA		_ptr0
		INC		_tmp6D6
		BNE		loc_24323
		INC		_tmp6D6+1
loc_24323:
		LDY		#0
		LDA		(_ptr0),Y
		BPL		loc_24330
		AND		#$7F
		STA		_tmp6D8
		BPL		_msg_print_out
loc_24330:
		CMP		#$A
		BNE		loc_2433D
		LDA		#0
		STA		_tmp6D9
		INC		_tmp6DA
		RTS
loc_2433D:
		CMP		#$5B
		BNE		loc_24347
		LDA		#$FF
		STA		_tmp6D6+1
		RTS
loc_24347:
		PHA
		LDX		_ppu_queue_pos_head
		LDY		_tmp6DA
		LDA		_msg_print_out_ppu_pos_list_lo,Y
		CLC
		ADC		_tmp6D9
		STA		_ppu_queue_buf,X
		INX
		LDA		_msg_print_out_ppu_pos_list_hi,Y
		STA		_ppu_queue_buf,X
		INX
		LDA		#1
		STA		_ppu_queue_buf,X
		INX
		PLA
		STA		_ppu_queue_buf,X
		INX
		STX		_ppu_queue_pos_head
		INC		_tmp6D9
locret_2436F:
		RTS
_msg_print_out_ppu_pos_list_lo:
		.BYTE	$04,$24,$44,$64,$84,$A4,$C4,$E4,$04,$24,$44,$64,$84,$A4,$C4,$E4,$04
_msg_print_out_ppu_pos_list_hi:
		.BYTE	$21,$21,$21,$21,$21,$21,$21,$21,$22,$22,$22,$22,$22,$22,$22,$22,$23

; =============== S U B	R O U T	I N E =======================================
_scenario_map_load:
		JSR		_pal_fade_out
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		JSR		_map_buf_init
		LDA		#1
		STA		_MMC5_CHR_SIZE
		JSR		_apu_music_off
		LDY		_scenario_idx
		LDX		_scenario_data_chr_bank,Y
		STX		_MMC5_CHR_BANKSA+3
		INX
		STX		_MMC5_CHR_BANKSA+7
		LDA		_PPU_STATUS
		LDA		_scenario_data_ppu_hi,Y
		AND		#$F
		STA		_PPU_ADDR
		LDA		_scenario_data_ppu_lo,Y
		STA		_PPU_ADDR
		LDA		_PPU_DATA
		LDA		off_251A3,Y
		STA		_tmp2
		LDA		off_251AC,Y
		STA		_tmp3
		LDA		#<[_wram_map_buf]
		STA		_tmp4
		LDA		#>[_wram_map_buf]
		STA		_tmp5
		JSR		_sram_on
		JSR		_scenario_map_unpack
		JSR		_sram_off
		LDA		#3
		STA		_MMC5_CHR_SIZE
		JSR		_apu_music_on
		LDY		_scenario_idx
		LDX		_scenario_titles_ofs_lib,Y
		LDY		#1
loc_243F1:
		LDA		_scenario_town_names_list,X
		BMI		loc_243FD
		STA		_city,Y
		INX
		INY
		BNE		loc_243F1
loc_243FD:
		STY		_city._name
		INC		_mmc5_operation_in_progress_flag
		LDA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
		JMP		_render_on

; =============== S U B	R O U T	I N E =======================================
_scenario_map_unpack:
		LDY		#0
		LDA		_PPU_DATA
		STA		_tmp6
		AND		#$F0
		CMP		#$80
		BNE		loc_24449
		LDA		_tmp6
		CMP		#$8F
		BNE		loc_2442D
		LDX		_PPU_DATA
		LDA		_PPU_DATA
loc_24424:
		JSR		_scenario_map_block_unpack
		DEX
		BNE		loc_24424
		JMP		_scenario_map_unpack
loc_2442D:
		LDA		_tmp6
		SEC
		SBC		#$80
		ASL
		TAY
		LDA		(_tmp2),Y
		JSR		_scenario_map_block_unpack
		LDA		_tmp6
		SEC
		SBC		#$80
		ASL
		TAY
		INY
		LDA		(_tmp2),Y
		JSR		_scenario_map_block_unpack
		JMP		_scenario_map_unpack
loc_24449:
		LDA		_tmp6
		JSR		_scenario_map_block_unpack
		JMP		_scenario_map_unpack

; =============== S U B	R O U T	I N E =======================================
_scenario_map_block_unpack:
		STA		_tmp6+1
loc_24453:
		LDY		#0
		LDA		(_tmp4),Y
		CMP		#$90
		BEQ		loc_2446F
		INC		_tmp4
		BNE		loc_24461
		INC		_tmp5
loc_24461:
		LDA		_tmp4
		CMP		#<[_power_grid_map_buf]
		BNE		loc_24453
		LDA		_tmp5
		CMP		#>[_power_grid_map_buf]
		BNE		loc_24453
		BEQ		loc_244EC
loc_2446F:
		LDA		_tmp6+1
		BMI		loc_244D6
		CMP		#$64
		BNE		loc_2449E
		LDY		#3
		LDA		#$8E
		STA		(_tmp4),Y
		LDY		#$4F
		LDA		#$8D
		STA		(_tmp4),Y
		LDY		#$9B
		LDA		#$8C
		STA		(_tmp4),Y
		LDY		#$E4
		LDA		#$88
		STA		(_tmp4),Y
		INY
		LDA		#$89
		STA		(_tmp4),Y
		INY
		LDA		#$8A
		STA		(_tmp4),Y
		INY
		LDA		#$8B
		STA		(_tmp4),Y
loc_2449E:
		CMP		#$60
		BCC		loc_244BE
		LDY		#2
		LDA		#$87
		STA		(_tmp4),Y
		LDY		#$4E
		LDA		#$86
		STA		(_tmp4),Y
		LDY		#$98
		LDA		#$83
		STA		(_tmp4),Y
		INY
		LDA		#$84
		STA		(_tmp4),Y
		INY
		LDA		#$85
		STA		(_tmp4),Y
loc_244BE:
		LDY		#0
		LDA		_tmp6+1
		STA		(_tmp4),Y
		INY
		LDA		#$82
		STA		(_tmp4),Y
		LDA		#$80
		LDY		#$4C
		STA		(_tmp4),Y
		INY
		LDA		#$81
		STA		(_tmp4),Y
		BNE		loc_244DA
loc_244D6:
		LDY		#0
		STA		(_tmp4),Y
loc_244DA:
		INC		_tmp4
		BNE		loc_244E0
		INC		_tmp5
loc_244E0:
		LDA		_tmp4
		CMP		#<[_power_grid_map_buf]
		BNE		loc_244EE
		LDA		_tmp5
		CMP		#>[_power_grid_map_buf]
		BNE		loc_244EE
loc_244EC:
		PLA
		PLA
loc_244EE:
		LDA		_tmp6+1
		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref_19:
;		INC		_ptr0
;		BNE		locret_244F7
;		INC		_ptr0+1
;locret_244F7:
;		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup01_name_input_init:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		LDA		#8
		STA		_MMC5_NT_MODE
		STA		_mmc5_nt_mode_shadow
		LDY		#$80
		STY		_game_wnd_spr_mode_flag
		STY		_ppu_ctrl_shadow
		LDY		#1
		STY		_scr_mode_idx
		STY		_game_core_state
		INY
		STY		_scr_res_idx
		DEC		_mmc5_operation_in_progress_flag
		JSR		_spr_clear
		LDX		#<[_tlm_res_name_init]
		LDY		#>[_tlm_res_name_init]
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$FA
		.WORD	_tlm_res_unpack
		LDA		#0
		STA		byte_6DC+2
		LDA		#2
		STA		_MMC5_CHR_MODE
		LDX		#<[_rle_tlm_nt_input_name]
		LDY		#>[_rle_tlm_nt_input_name]
		JSR		_rle_nt_res_queue
		LDX		#<[_rle_tlm_extnt_input_name]
		LDY		#>[_rle_tlm_extnt_input_name]
		JSR		_rle_extnt_res_queue
		JSR		_name_input_char_nt_queue
		JSR		_name_input_char_extnt_queue
loc_24549:
		JSR		_ppu_dequeue
		JSR		_extnt_dequeue
		JSR		_ppu_queue_empty_compare
		BNE		loc_24549
		LDA		#1
		STA		_MMC5_CHR_MODE
		LDA		#$2C
		STA		_MMC5_CHR_BANKSA+2
		LDA		#$2D
		STA		_MMC5_CHR_BANKSA+3
		INC		_mmc5_operation_in_progress_flag
		JSR		_render_on
		LDA		#$CD
		STA		_tmp6CC
		LDA		#$A6
		STA		_tmp6CE
		LDA		#0
		STA		_tmp6D0+1
		STA		_tmp6D0
		INC		_game_setup_sub_idx
		LDA		#9
		STA		_screen_pal_cur_bg_idx
		LDA		#7
		STA		_screen_pal_cur_spr_idx
		JMP		_pal_fade_in

; =============== S U B	R O U T	I N E =======================================
_game_setup02_name_input_input:
		JSR		_name_input_cursor_move_control
		JSR		_common_select_cursor_draw
		JSR		_name_input_button_push_control
		JSR		_name_input_button_push_draw
		JSR		_name_input_char_store
		JSR		_name_input_char_nt_queue
		JSR		_name_input_char_extnt_queue
		JSR		_spr_finish
		BIT		_pad0_autorep_press
		BVC		locret_245A9
		JSR		_name_input_previous_scr
locret_245A9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_next_scr:
		LDA		byte_6DC+2
		BNE		loc_245C9
		INC		byte_6DC+2
		JSR		_ppu_queues_reset
		JSR		_name_input_clear_header_area
		LDX		#<[_rle_tlm_nt_input_word]
		LDY		#>[_rle_tlm_nt_input_word]
		JSR		_rle_nt_res_queue
		LDX		#<[_rle_tlm_extnt_input_word]
		LDY		#>[_rle_tlm_extnt_input_word]
		JSR		_rle_extnt_res_queue
		JMP		_wait_for_flush
loc_245C9:
		LDA		#$D
		STA		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_previous_scr:
		LDA		byte_6DC+2
		BEQ		loc_245EE
		DEC		byte_6DC+2
		JSR		_ppu_queues_reset
		JSR		_name_input_clear_header_area
		LDX		#<[_rle_tlm_nt_input_name]
		LDY		#>[_rle_tlm_nt_input_name]
		JSR		_rle_nt_res_queue
		LDX		#<[_rle_tlm_extnt_input_name]
		LDY		#>[_rle_tlm_extnt_input_name]
		JSR		_rle_extnt_res_queue
		JMP		_wait_for_flush
loc_245EE:
		LDA		#0
		STA		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_clear_header_area:
		LDX		#<[_rle_tlm_nt_clear_area]
		LDY		#>[_rle_tlm_nt_clear_area]
		JSR		_rle_nt_res_queue
		LDX		#<[_rle_tlm_extnt_clear_area]
		LDY		#>[_rle_tlm_extnt_clear_area]
		JSR		_rle_extnt_res_queue

; =============== S U B	R O U T	I N E =======================================
_wait_for_flush:
		JSR		_wait_for_nmi
		JSR		_ppu_queue_empty_compare
		BNE		_wait_for_flush
locret_2460A:
		RTS

; =============== S U B	R O U T	I N E =======================================
_common_select_cursor_draw:
		LDA		_pad0_autorep_press
		BEQ		loc_24614
		LDA		#$FF
		STA		_tmp6D0+1
loc_24614:
		LDA		_tmp6D0+1
		BMI		loc_24632
		LDA		_tmp6D0
		BNE		loc_2462F
		LDY		#8
		LDA		_tmp6D0+1
		EOR		#1
		STA		_tmp6D0+1
		BEQ		loc_2462C
		LDY		#$10
loc_2462C:
		STY		_tmp6D0
loc_2462F:
		DEC		_tmp6D0
loc_24632:
		LDA		#0
		STA		_spr_insert_args._idx
		STA		_spr_buf_pos
		LDA		_tmp6D0+1
		BEQ		loc_2464A
		LDA		_tmp6CC
		STA		_spr_insert_args._pos._X
		LDA		_tmp6CE
		STA		_spr_insert_args._pos._Y
		JMP		_spr_lib_prg2_insert
loc_2464A:
		JMP		_spr_finish

; =============== S U B	R O U T	I N E =======================================
_name_input_button_push_draw:
		LDA		byte_6DC+3
		BEQ		locret_2460A
		DEC		byte_6DC+3
		LDY		byte_6DC+6
		CPY		#$21
		BCS		loc_2469A
		CPY		#8
		BEQ		loc_2469A
		CMP		#$10
		BNE		loc_24688
		CPY		#$20
		BNE		loc_24676
		LDX		#<[byte_24C5D]
		LDY		#>[byte_24C5D]
		JSR		_rle_nt_res_queue
		LDX		#<[byte_24C6A]
		LDY		#>[byte_24C6A]
		JMP		_rle_extnt_res_queue
loc_24676:
		CPY		#$C
		BNE		locret_246DB
		LDX		#<[byte_24C45]
		LDY		#>[byte_24C45]
		JSR		_rle_nt_res_queue
		LDX		#<[byte_24C51]
		LDY		#>[byte_24C51]
		JMP		_rle_extnt_res_queue
loc_24688:
		CMP		#1
		BNE		locret_246DB
		LDX		#<[byte_24CA5]
		LDY		#>[byte_24CA5]
		JSR		_rle_nt_res_queue
		LDX		#<[byte_24CD1]
		LDY		#>[byte_24CD1]
		JMP		_rle_extnt_res_queue
loc_2469A:
		CMP		#$10
		BNE		loc_246AC
		LDX		#<[byte_24CA5]
		LDY		#>[byte_24CA5]
		JSR		_rle_nt_res_queue
		LDX		#<[byte_24CD1]
		LDY		#>[byte_24CD1]
		JSR		_rle_extnt_res_queue
loc_246AC:
		LDA		#$A0
		STA		_spr_insert_args._idx
		LDA		byte_6DC+4
		STA		_spr_insert_args._pos._X
		LDA		byte_6DC+5
		STA		_spr_insert_args._pos._Y
		JSR		_spr_lib_prg2_insert
		LDX		#$FD
		LDA		byte_6DC+6
		CMP		#8
		BEQ		loc_246C9
		ORA		#$80
		TAX
loc_246C9:
		STX		_spr_buf+$19
		LDA		byte_6DC+6
		SEC
		SBC		#$30
		CMP		#$A
		BCS		locret_246DB
		LDA		#2
		STA		_spr_buf+$1A
locret_246DB:
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_cursor_move_control:
		LDA		_pad0_autorep_release
		AND		#$F
		BEQ		loc_246E6
		LDA		#$40
		STA		_apu_se_flags+1
loc_246E6:
		LDX		_tmp6D2
		LDY		_tmp6D3
		LDA		_pad0_autorep_release
		LSR
		BCC		loc_24706
		INX
		CPX		#$A
		BNE		loc_24700
		CPY		#0
		BEQ		loc_24700
		CPY		#2
		BNE		loc_246FF
		INX
loc_246FF:
		INX
loc_24700:
		CPX		#$C
		BNE		loc_24706
		LDX		#0
loc_24706:
		LSR
		BCC		loc_2471C
		DEX
		BPL		loc_24713
		LDX		#$B
		CPY		#2
		BNE		loc_24713
		DEX
loc_24713:
		CPX		#$A
		BNE		loc_2471C
		CPY		#0
		BEQ		loc_2471C
		DEX
loc_2471C:
		LSR
		BCC		loc_24732
		INY
		CPX		#$A
		BCC		loc_2472C
		BNE		loc_24727
		INX
loc_24727:
		CPY		#2
		BNE		loc_2472C
		INY
loc_2472C:
		CPY		#4
		BNE		loc_24732
		LDY		#0
loc_24732:
		LSR
		BCC		loc_24746
		DEY
		BPL		loc_2473A
		LDY		#3
loc_2473A:
		CPX		#$A
		BCC		loc_24746
		BNE		loc_24741
		INX
loc_24741:
		CPY		#2
		BNE		loc_24746
		DEY
loc_24746:
		STX		_tmp6D2
		STY		_tmp6D3
		TXA
		ASL
		ASL
		ASL
		ASL
		ADC		#$25
		STA		_tmp6CC
		TYA
		ASL
		ASL
		ASL
		ASL
		ADC		#$8D
		STA		_tmp6CE
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_button_push_control:
		LDA		_pad0_autorep_press
		BPL		loc_247BD
		LDA		_tmp6D3
		ASL
		ADC		_tmp6D3
		ASL
		ASL
		ADC		_tmp6D2
		TAY
		LDA		_input_chars_tbl,Y
		STA		byte_6DC+1
		STA		byte_6DC+6
		BPL		loc_247A1
		LDY		#0
		STY		byte_6DC+1
		INY
		STY		byte_6DC+6
		STY		_apu_se_flags
		LDA		#8
		STA		byte_6DC+3
		LDX		#<[byte_24C77]
		LDY		#>[byte_24C77]
		JSR		_rle_nt_res_queue
		LDX		#<[byte_24C8E]
		LDY		#>[byte_24C8E]
		JSR		_rle_extnt_res_queue
		JSR		_wait_for_flush
		JMP		_name_input_next_scr
loc_247A1:
		LDA		_tmp6CC
		SEC
		SBC		#4
		STA		byte_6DC+4
		LDA		_tmp6CE
		SEC
		SBC		#4
		STA		byte_6DC+5
		LDA		#$10
		STA		byte_6DC+3
		LDA		#1
		STA		_apu_se_flags
		RTS
loc_247BD:
		LDA		#0
		STA		byte_6DC+1
		RTS
_input_chars_tbl:
		.BYTE	$31,$32,$33,$34,$35,$36,$37,$38,$39,$30, $08,$0C
		.BYTE	$51,$57,$45,$52,$54,$59,$55,$49,$4F,$50,$00,$20
		.BYTE	$41,$53,$44,$46,$47,$48,$4A,$4B,$4C,$3A,$00,$00
		.BYTE	$5A,$58,$43,$56,$42,$4E,$4D,$2C,$2E,$2D,$00,$FF

; =============== S U B	R O U T	I N E =======================================
_name_input_char_store:
		LDA		byte_6DC+1
		BEQ		locret_24842
		LDX		byte_6DC+2
		LDA		off_248BD,X
		STA		_ptr0
		LDA		#>[_city]
		STA		_ptr0+1
		LDY		#0
		LDA		(_ptr0),Y
		STA		_tmp2
		LDA		byte_6DC+1
		CMP		#8
		BNE		loc_24821
		LDY		_tmp2
		DEY
		BEQ		locret_24842
		LDA		#$7C
		STA		(_ptr0),Y
		TYA
		LDY		#0
		STA		(_ptr0),Y
		BEQ		locret_24842
loc_24821:
		CMP		#$C
		BNE		loc_24834
		LDA		#$7C
		LDY		#$A
loc_24829:
		STA		(_ptr0),Y
		DEY
		BNE		loc_24829
		LDA		#1
		STA		(_ptr0),Y
		BNE		locret_24842
loc_24834:
		LDY		_tmp2
		CPY		#$B
		BEQ		locret_24842
		STA		(_ptr0),Y
		INY
		TYA
		LDY		#0
		STA		(_ptr0),Y
locret_24842:
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_char_nt_queue:
		LDX		byte_6DC+2
		LDA		off_248BD,X
		STA		_ptr0
		LDA		#>[_city]
		STA		_ptr0+1
		LDX		_ppu_queue_pos_head
		LDY		byte_6DC+2
		LDA		byte_248BF,Y
		STA		_ppu_queue_buf,X
		INX
		LDA		byte_248C1,Y
		STA		_ppu_queue_buf,X
		INX
		LDA		#$A
		STA		_ppu_queue_buf,X
		INX
		LDY		#1
loc_2486A:
		LDA		(_ptr0),Y
		STA		_ppu_queue_buf,X
		INX
		INY
		CPY		#$B
		BNE		loc_2486A
		STX		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_char_extnt_queue:
		LDX		byte_6DC+2
		LDA		off_248BD,X
		STA		_ptr0
		LDA		#>[_city]
		STA		_ptr0+1
		LDX		_extnt_queue_pos_head
		LDY		byte_6DC+2
		LDA		byte_248BF,Y
		STA		_extnt_queue_buf,X
		INX
		LDA		byte_248C1,Y
		CLC
		ADC		#$3C
		STA		_extnt_queue_buf,X
		INX
		LDA		#$A
		STA		_extnt_queue_buf,X
		INX
		LDY		#1
loc_248A2:
		STX		_tmp3
		LDX		#$8B
		LDA		(_ptr0),Y
		CMP		#$7C
		BEQ		loc_248AE
		LDX		#$B
loc_248AE:
		TXA
		LDX		_tmp3
		STA		_extnt_queue_buf,X
		INX
		INY
		CPY		#$B
		BNE		loc_248A2
		STX		_extnt_queue_pos_head
		RTS
off_248BD:
		.BYTE	<[_city]
		.BYTE	<[_city._sister_name]
byte_248BF:
		.BYTE	$0B,$0B
byte_248C1:
		.BYTE	$21,$21

; =============== S U B	R O U T	I N E =======================================
_game_setup0D_difficult_init:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		LDX		#<[_tlm_res_game_play_level]
		LDY		#>[_tlm_res_game_play_level]
		JSR		_tlm_res_unpack
		JSR		_difficult_scr_cursor_draw
		LDA		#0
		STA		_tmp6D0+1
		STA		_tmp6D0
		INC		_mmc5_operation_in_progress_flag
		JSR		_render_on
		INC		_game_setup_sub_idx
		JMP		_pal_fade_in

; =============== S U B	R O U T	I N E =======================================
_game_setup0E_difficult_input:
		JSR		_difficult_scr_cursor_move
		JSR		_common_select_cursor_draw
		BIT		_pad0_autorep_press
		BPL		loc_2493D
		LDA		#1
		STA		_apu_se_flags
		LDA		_city._name
		CMP		#1
		BNE		loc_24916
		LDY		#8
		LDX		_scenario_titles_ofs_lib,Y
		LDY		#1
loc_24907:
		LDA		_scenario_town_names_list,X
		BMI		loc_24913
		STA		_city,Y
		INX
		INY
		BNE		loc_24907
loc_24913:
		STY		_city._name
loc_24916:
		LDA		_city._sister_name
		CMP		#1
		BNE		loc_24933
		LDY		#8
		LDX		_scenario_titles_ofs_lib,Y
		LDY		#1
loc_24924:
		LDA		_scenario_town_names_list,X
		BMI		loc_24930
		STA		_city._sister_name,Y
		INX
		INY
		BNE		loc_24924
loc_24930:
		STY		_city._sister_name
loc_24933:
		INC		_main_sub_idx
		LDA		#$80
		STA		_apu_mus_idx_req
		JMP		_pal_fade_out
loc_2493D:
		BVC		locret_24944
		LDA		#1
		STA		_game_setup_sub_idx
locret_24944:
		RTS

; =============== S U B	R O U T	I N E =======================================
_difficult_scr_cursor_move:
		LDA		_pad0_autorep_press
		AND		#$C
		BEQ		locret_2496C
		LDY		_city._difficult
		CMP		#4
		BNE		loc_2495D
		CPY		#2
		BEQ		locret_2496C
		LDA		#1
		STA		_apu_se_flags
		INY
		BNE		loc_24966
loc_2495D:
		CPY		#0
		BEQ		locret_2496C
		LDA		#1
		STA		_apu_se_flags
		DEY
loc_24966:
		STY		_city._difficult
		JSR		_difficult_scr_cursor_draw
locret_2496C:
		RTS

; =============== S U B	R O U T	I N E =======================================
_difficult_scr_cursor_draw:
		LDX		_extnt_queue_pos_head
		LDY		#2
loc_24971:
		LDA		off_249B0,Y
		STA		_extnt_queue_buf,X
		INX
		LDA		off_249B3,Y
		STA		_extnt_queue_buf,X
		INX
		LDA		byte_249B6,Y
		STA		_extnt_queue_buf,X
		STA		_tmp2
		INX
		LDA		#$8B
		CPY		_city._difficult
		BNE		loc_24991
		LDA		#$4B
loc_24991:
		STA		_extnt_queue_buf,X
		INX
		DEC		_tmp2
		BNE		loc_24991
		DEY
		BPL		loc_24971
		STX		_extnt_queue_pos_head
		LDY		_city._difficult
		LDA		byte_249B9,Y
		STA		_tmp6CE
		LDA		#$50
		STA		_tmp6CC
		JSR		_common_select_cursor_draw
		RTS
off_249B0:
		.BYTE	<[_MMC5_EXRAM+$1A6]
		.BYTE	<[_MMC5_EXRAM+$1E6]
		.BYTE	<[_MMC5_EXRAM+$226]
off_249B3:
		.BYTE	>[_MMC5_EXRAM+$1A6]
		.BYTE	>[_MMC5_EXRAM+$1E6]
		.BYTE	>[_MMC5_EXRAM+$226]
byte_249B6:
		.BYTE	$04,$06,$04
byte_249B9:
		.BYTE	$6E,$7E,$8E

; =============== S U B	R O U T	I N E =======================================
_game_setup03_dummy:
		INC		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup04_break:
		INC		_main_sub_idx
		LDA		#$80
		STA		_apu_mus_idx_req
		JMP		_pal_fade_out

; =============== S U B	R O U T	I N E =======================================
_game_setup00_new_city_gen:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		LDA		#$24
		STA		_MMC5_NT_MODE
		STA		_mmc5_nt_mode_shadow
		LDA		#7
		STA		_scr_res_idx
		LDA		#0
		STA		_scr_res_control_flags
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F9
		.WORD	_scr_res_common
		LDA		_title_sub_idx
		BEQ		loc_249F4
		DEC		_main_sub_idx
		JSR		_pal_fade_out
loc_249F4:
		INC		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_rle_nt_res_queue:
		STY		_ptr0+1
		STX		_ptr0
		LDY		#0
		LDX		_ppu_queue_pos_head
loc_24A00:
		LDA		(_ptr0),Y
		BEQ		loc_24A28
		BMI		loc_24A16
		INY
		STA		_tmp2
loc_24A09:
		LDA		(_ptr0),Y
		STA		_ppu_queue_buf,X
		INX
		INY
		DEC		_tmp2
		BNE		loc_24A09
		BEQ		loc_24A00
loc_24A16:
		INY
		AND		#$7F
		STA		_tmp2
		LDA		(_ptr0),Y
loc_24A1D:
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp2
		BNE		loc_24A1D
		INY
		BNE		loc_24A00
loc_24A28:
		STA		_ppu_queue_buf,X
		STX		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_rle_extnt_res_queue:
		STY		_ptr0+1
		STX		_ptr0
		LDY		#0
		LDX		_extnt_queue_pos_head
loc_24A36:
		LDA		(_ptr0),Y
		BEQ		loc_24A5E
		BMI		loc_24A4C
		INY
		STA		_tmp2
loc_24A3F:
		LDA		(_ptr0),Y
		STA		_extnt_queue_buf,X
		INX
		INY
		DEC		_tmp2
		BNE		loc_24A3F
		BEQ		loc_24A36
loc_24A4C:
		INY
		AND		#$7F
		STA		_tmp2
		LDA		(_ptr0),Y
loc_24A53:
		STA		_extnt_queue_buf,X
		INX
		DEC		_tmp2
		BNE		loc_24A53
		INY
		BNE		loc_24A36
loc_24A5E:
		STA		_extnt_queue_buf,X
		STX		_extnt_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
; WHAT?!
_ppu_queue_empty_compare:
		LDA		_ppu_queue_pos_tail
		CMP		_ppu_queue_pos_head
		BNE		locret_24A6E
		LDA		_extnt_queue_pos_tail
		CMP		_extnt_queue_pos_head
locret_24A6E:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup09_practice_msg_init:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		LDY		#$80
		STY		_game_wnd_spr_mode_flag
		STY		_ppu_ctrl_shadow
		LDY		#1
		STY		_scr_mode_idx
		INY
		STY		_scr_res_idx
		DEC		_mmc5_operation_in_progress_flag
		JSR		_spr_clear
		LDA		#8
		STA		_MMC5_NT_MODE
		LDX		#<[_tlm_res_fullscreen_wnd]
		LDY		#>[_tlm_res_fullscreen_wnd]
		JSR		_tlm_res_unpack
		LDA		#<[_tlm_practice_wnd_header]
		STA		_ptr0
		LDA		#>[_tlm_practice_wnd_header]
		STA		_ptr0+1
		JSR		_tlm_transfer
		LDY		#<[_practice_msg0]
		LDX		#>[_practice_msg0]
		LDA		_tmp6D5
		BEQ		loc_24AB0
		LDY		#<[_practice_msg1]
		LDX		#>[_practice_msg1]
loc_24AB0:
		STY		_tmp6D6
		STX		_tmp6D6+1
		LDY		#0
		STY		_tmp6DA
		STY		_tmp6D9
		DEY
		STY		_tmp6D8
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#8
		STA		_screen_pal_cur_bg_idx
		INC		_game_setup_sub_idx
		JMP		_pal_fade_in

; =============== S U B	R O U T	I N E =======================================
_game_setup0A_practice_msg_draw:
		JSR		_msg_print_out_handler
		BIT		_pad0_autorep_press
		BVC		loc_24AE4
		LDA		#2
		STA		_title_sub_idx
		DEC		_main_sub_idx
		JMP		_pal_fade_out
loc_24AE4:
		BPL		locret_24B0D
		LDA		#1
		STA		_apu_se_flags
		LDA		_tmp6D5
		BNE		loc_24AF6
		INC		_tmp6D5
		DEC		_game_setup_sub_idx
		RTS
loc_24AF6:
		LDA		#8
		STA		_scenario_idx
		JSR		_scenario_map_load
		LDA		#0
		STA		_scenario_idx
		INC		_main_sub_idx
		LDA		#$80
		STA		_apu_mus_idx_req
		JSR		_pal_fade_out
locret_24B0D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup0B_scenario_over_init:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		LDY		#$80
		STY		_game_wnd_spr_mode_flag
		STY		_ppu_ctrl_shadow
		LDY		#1
		STY		_scr_mode_idx
		INY
		STY		_scr_res_idx
		DEC		_mmc5_operation_in_progress_flag
		LDA		#8
		STA		_MMC5_NT_MODE
		JSR		_spr_clear
		LDX		#<[_tlm_res_fullscreen_wnd]
		LDY		#>[_tlm_res_fullscreen_wnd]
		JSR		_tlm_res_unpack
		LDA		_scenario_result_idx
		CMP		#1
		BEQ		loc_24B49
		LDA		#<[_tlm_scenario_loose_header]
		STA		_ptr0
		LDA		#>[_tlm_scenario_loose_header]
		STA		_ptr0+1
		JSR		_tlm_transfer
loc_24B49:
		LDA		_scenario_result_idx
		CMP		#1
		BNE		loc_24B6C
		JSR		_sram_on
		LDY		_scenario_idx
		LDA		_city._scenarios_clear_flags
		ORA		_bit_flags,Y
		STA		_city._scenarios_clear_flags
		STA		_sram_vars._scenarios_clear_flags
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_sram_sum_reinit
		JSR		_sram_off
loc_24B6C:
		LDY		_scenario_result_idx
		DEY
		LDA		_final_msg_lib_lo,Y
		STA		_tmp6D6
		LDA		_final_msg_lib_hi,Y
		STA		_tmp6D6+1
		LDY		#0
		STY		_tmp6DA
		STY		_tmp6D9
		DEY
		STY		_tmp6D8
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#$10
		STA		_screen_pal_cur_bg_idx
		INC		_game_setup_sub_idx
		JMP		_pal_fade_in
_bit_flags:
		.BYTE	$01,$02,$04,$08,$10,$20,$40,$80

; =============== S U B	R O U T	I N E =======================================
_game_setup0C_scenario_over_input:
		JSR		_msg_print_out_handler
		LDA		_pad0_autorep_press
		AND		#$C0
		BEQ		locret_24BC1
		LDA		#0
		STA		_scenario_result_idx
		LDX		#5
		LDA		_city._all_scenarios_are_clear_ram_flag
		BNE		loc_24BBE
		LDA		_city._scenarios_clear_flags
		CMP		#$3F
		BNE		loc_24BBE
		LDX		#$F
loc_24BBE:
		STX		_game_setup_sub_idx
locret_24BC1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup0F_all_complete_init:
		JSR		_ppu_queues_reset
		JSR		_pal_fade_out
		JSR		_render_off
		LDY		#$80
		STY		_game_wnd_spr_mode_flag
		STY		_ppu_ctrl_shadow
		LDY		#1
		STY		_scr_mode_idx
		INY
		STY		_scr_res_idx
		DEC		_mmc5_operation_in_progress_flag
		LDA		#8
		STA		_MMC5_NT_MODE
		JSR		_spr_clear
		LDX		#<[_tlm_res_fullscreen_wnd]
		LDY		#>[_tlm_res_fullscreen_wnd]
		JSR		_tlm_res_unpack
		LDA		#<[_tlm_all_complete_]
		STA		_ptr0
		LDA		#>[_tlm_all_complete_]
		STA		_ptr0+1
		JSR		_tlm_transfer
		LDA		#<[_all_scenarios_clear_msg]
		STA		_tmp6D6
		LDA		#>[_all_scenarios_clear_msg]
		STA		_tmp6D6+1
		LDY		#0
		STY		_tmp6DA
		STY		_tmp6D9
		DEY
		STY		_tmp6D8
		JSR		_sram_on
		LDA		#1
		STA		_city._all_scenarios_are_clear_ram_flag
		STA		_sram_vars._all_scenarios_are_clear_ram_flag
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_sram_sum_reinit
		JSR		_sram_off
		JSR		_render_on
		INC		_mmc5_operation_in_progress_flag
		LDA		#8
		STA		_screen_pal_cur_bg_idx
		INC		_game_setup_sub_idx
		JMP		_pal_fade_in

; =============== S U B	R O U T	I N E =======================================
_game_setup10_all_complete_input:
		JSR		_msg_print_out_handler
		LDA		_pad0_autorep_press
		AND		#$C0
		BEQ		locret_24C44
		LDA		#0
		STA		_scenario_result_idx
		LDA		#5
		STA		_game_setup_sub_idx
locret_24C44:
		RTS
byte_24C45:
		.BYTE	$0A,$1A,$22,$02,$C0,$C0,$3A,$22,$02,$C4,$DA,$00
byte_24C51:
		.BYTE	$0A,$1A,$5E,$02,$0A,$0A,$3A,$5E,$02,$0A,$0A,$00
byte_24C5D:
		.BYTE	$0B,$58,$22,$04,$C2,$C3,$C2,$C2,$78,$22,$01,$C1,$00
byte_24C6A:
		.BYTE	$0B,$58,$5E,$04,$0A,$0A,$0A,$0A,$78,$5E,$01,$0A,$00
byte_24C77:
		.BYTE	$15,$B8,$22,$04,$C0,$C0,$C0,$C0,$D8,$22,$04,$B0,$B1,$B2,$B3,$F8
		.BYTE	$22,$04,$C0,$C1,$C2,$C3,$00
byte_24C8E:
		.BYTE	$15,$B8,$5E,$04,$0A,$0A,$0A,$0A,$D8,$5E,$04,$C8,$C8,$C8,$C8,$F8
		.BYTE	$5E,$04,$C8,$C8,$C8,$C8,$00
byte_24CA5:
		.BYTE	$2A,$1A,$22,$02,$C1,$C1,$3A,$22,$02,$43,$4C,$58,$22,$04,$2A,$4E
		.BYTE	$2A,$2A,$78,$22,$01,$DD,$B8,$22,$04,$C1,$C1,$C1,$C1,$D8,$22,$04
		.BYTE	$A1,$A2,$A3,$A4,$F8,$22,$04,$B1,$B2,$B3,$B4,$00
byte_24CD1:
		.BYTE	$2A,$1A,$5E,$02,$06,$06,$3A,$5E,$02,$8B,$8B,$58,$5E,$04,$07,$07
		.BYTE	$07,$07,$78,$5E,$01,$89,$B8,$5E,$04,$06,$06,$06,$06,$D8,$5E,$04
		.BYTE	$CA,$CA,$CA,$CA,$F8,$5E,$04,$CA,$CA,$CA,$CA,$00
_rle_tlm_nt_clear_area:
		.BYTE	$03,$42,$21,$1C,$9C,$DD,$03,$22,$21,$1C,$9C,$DD,$03,$02,$21,$1C
		.BYTE	$9C,$DD,$03,$E2,$20,$1C,$9C,$DD,$03,$C2,$20,$1C,$9C,$DD,$03,$A2
		.BYTE	$20,$1C,$9C,$DD,$03,$82,$20,$1C,$9C,$DD,$00
_rle_tlm_extnt_clear_area:
		.BYTE	$03,$42,$5D,$1C,$9C,$86,$03,$22,$5D,$1C,$9C,$86,$03,$02,$5D,$1C
		.BYTE	$9C,$86,$03,$E2,$5C,$1C,$9C,$86,$03,$C2,$5C,$1C,$9C,$86,$03,$A2
		.BYTE	$5C,$1C,$9C,$86,$03,$82,$5C,$1C,$9C,$86,$00
_rle_tlm_nt_input_name:
		.BYTE	$04,$83,$20,$1A,$A5,$98,$A6,$43,$A7,$A3,$20,$1A,$B5,$DF,$86,$8C
		.BYTE	$8C,$C5,$AB,$DF,$88,$8D,$C5,$AD,$DF,$8A,$C2,$AB,$7B,$79,$C4,$DF
		.BYTE	$AE,$AC,$C6,$87,$DF,$B7,$C3,$20,$1A,$B5,$DF,$96,$9C,$89,$D5,$BB
		.BYTE	$DF,$98,$9D,$D5,$BD,$DF,$9A,$D2,$BB,$8B,$DF,$D4,$DF,$BE,$BC,$D6
		.BYTE	$97,$DF,$B7,$E3,$20,$1A,$B5,$F9,$F9,$F9,$99,$8B,$F9,$01,$7A,$88
		.BYTE	$F9,$05,$B7,$03,$21,$1A,$B5,$98,$DC,$05,$B7,$23,$21,$1A,$B5,$98
		.BYTE	$DC,$05,$B7,$43,$21,$1A,$C5,$98,$C6,$01,$C7,$00
_rle_tlm_extnt_input_name:
		.BYTE	$03,$83,$5C,$1A,$9A,$CA,$42,$A3,$5C,$1A,$CA,$01,$01,$01,$01,$00
		.BYTE	$00,$01,$01,$01,$00,$00,$01,$01,$00,$00,$01,$01,$00,$01,$00,$00
		.BYTE	$00,$01,$01,$CA,$C3,$5C,$1A,$CA,$01,$01,$01,$01,$00,$00,$01,$01
		.BYTE	$01,$00,$00,$01,$01,$00,$00,$01,$01,$00,$01,$00,$00,$00,$01,$01
		.BYTE	$CA,$E3,$5C,$1A,$CA,$80,$80,$80,$01,$8B,$80,$0E,$01,$80,$80,$80
		.BYTE	$80,$80,$80,$80,$80,$CA,$03,$5D,$1A,$CA,$98,$06,$05,$CA,$23,$5D
		.BYTE	$1A,$CA,$98,$06,$04,$CA,$43,$5D,$1A,$9A,$CA,$00
_rle_tlm_nt_input_word:
		.BYTE	$04,$82,$20,$1C,$A5,$9A,$A6,$47,$A7,$A2,$20,$1C,$B5,$DF,$86,$8C
		.BYTE	$8C,$C5,$AB,$DF,$88,$8D,$C5,$AD,$DF,$A6,$AC,$C7,$8D,$AD,$C2,$AB
		.BYTE	$AF,$76,$77,$8D,$AD,$C8,$DF,$B7,$C2,$20,$1C,$B5,$DF,$96,$9C,$89
		.BYTE	$D5,$BB,$DF,$98,$9D,$D5,$BD,$DF,$B6,$BC,$D7,$9D,$BD,$D2,$BB,$BF
		.BYTE	$A7,$78,$9D,$BD,$D8,$DF,$B7,$E2,$20,$1C,$B5,$F9,$F9,$F9,$99,$96
		.BYTE	$F9,$05,$B7,$02,$21,$1C,$B5,$9A,$DC,$05,$B7,$22,$21,$1C,$B5,$9A
		.BYTE	$DC,$05,$B7,$42,$21,$1C,$C5,$9A,$C6,$01,$C7,$00
_rle_tlm_extnt_input_word:
		.BYTE	$03,$82,$5C,$1C,$9C,$CA,$46,$A2,$5C,$1C,$CA,$01,$01,$01,$01,$00
		.BYTE	$00,$01,$01,$01,$00,$00,$01,$01,$00,$00,$01,$00,$00,$00,$00,$01
		.BYTE	$01,$01,$00,$00,$01,$CA,$C2,$5C,$1C,$CA,$01,$01,$01,$01,$00,$00
		.BYTE	$01,$01,$01,$00,$00,$01,$01,$00,$00,$01,$00,$00,$00,$00,$01,$01
		.BYTE	$01,$00,$00,$01,$CA,$E2,$5C,$1C,$CA,$80,$80,$80,$01,$96,$80,$05
		.BYTE	$CA,$02,$5D,$1C,$CA,$9A,$06,$05,$CA,$22,$5D,$1C,$CA,$9A,$06,$04
		.BYTE	$CA,$42,$5D,$1C,$9C,$CA,$00
_scenario_titles_ofs_lib:
		.BYTE	 $00,$06, $0B,$13,$19,$20,$24,$2E,$36
_scenario_town_names_list:
		.BYTE	"CISCO",$FF
		.BYTE	"BERN",$FF
		.BYTE	"DETROIT",$FF
		.BYTE	"TOKYO",$FF
		.BYTE	"BOSTON",$FF
		.BYTE	"RIO",$FF
		.BYTE	"LAS VEGAS",$FF
		.BYTE	"FREEDOM",$FF
		.BYTE	"SIMCITY",$FF
_scenario_intro_header_lib_lo:
		.BYTE	<[_tlm_scenario_header0]
		.BYTE	<[_tlm_scenario_header1]
		.BYTE	<[_tlm_scenario_header2]
		.BYTE	<[_tlm_scenario_header3]
		.BYTE	<[_tlm_scenario_header4]
		.BYTE	<[_tlm_scenario_header5]
		.BYTE	<[_tlm_scenario_header6]
		.BYTE	<[_tlm_scenario_header7]
_scenario_intro_header_lib_hi:
		.BYTE	>[_tlm_scenario_header0]
		.BYTE	>[_tlm_scenario_header1]
		.BYTE	>[_tlm_scenario_header2]
		.BYTE	>[_tlm_scenario_header3]
		.BYTE	>[_tlm_scenario_header4]
		.BYTE	>[_tlm_scenario_header5]
		.BYTE	>[_tlm_scenario_header6]
		.BYTE	>[_tlm_scenario_header7]
_scenario_msg_lib_lo:
		.BYTE	<[_scenario_msg0]
		.BYTE	<[_scenario_msg1]
		.BYTE	<[_scenario_msg2]
		.BYTE	<[_scenario_msg3]
		.BYTE	<[_scenario_msg4]
		.BYTE	<[_scenario_msg5]
		.BYTE	<[_scenario_msg6]
		.BYTE	<[_scenario_msg7]
_scenario_msg_lib_hi:
		.BYTE	>[_scenario_msg0]
		.BYTE	>[_scenario_msg1]
		.BYTE	>[_scenario_msg2]
		.BYTE	>[_scenario_msg3]
		.BYTE	>[_scenario_msg4]
		.BYTE	>[_scenario_msg5]
		.BYTE	>[_scenario_msg6]
		.BYTE	>[_scenario_msg7]
_final_msg_lib_lo:
		.BYTE	<[_game_won_msg]
		.BYTE	<[_game_lose_msg]
_final_msg_lib_hi:
		.BYTE	>[_game_won_msg]
		.BYTE	>[_game_lose_msg]
_tlm_scenario_header6:
		.WORD	$8520
		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.WORD	$A520
		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.BYTE	0
_tlm_scenario_header7:
		.WORD	$8520
		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.WORD	$A520
		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.BYTE	0
_tlm_scenario_header0:
		.WORD	$8520
		.BYTE	$16,$13,$21,$2E,$00,$06,$32,$21,$2E,$23,$29,$33,$23,$2F,$00,$03,$01,$00,$00,$1C,$3F,$1B,$3C
		.WORD	$A520
		.BYTE	$16,$53,$61,$6E,$00,$46,$72,$61,$6E,$63,$69,$73,$63,$6F,$00,$43,$41,$BC,$00,$5C,$7F,$7C,$7C
		.BYTE	0
_tlm_scenario_header1:
		.WORD	$8620
		.BYTE	$15,$02,$25,$32,$2E,$9B,$13,$37,$29,$34,$3A,$25,$32,$2C,$21,$2E,$24,$00,$1C,$3F,$3C,$3B
		.WORD	$A620
		.BYTE	$15,$42,$65,$72,$6E,$BB,$53,$77,$69,$74,$7A,$65,$72,$6C,$61,$6E,$64,$00,$5C,$7F,$7C,$7B
		.BYTE	0
_tlm_scenario_header2:
		.WORD	$8820
		.BYTE	$10,$04,$25,$34,$32,$2F,$29,$34,$9B,$0D,$09,$9C,$1C,$3F,$3D,$1D,$00
		.WORD	$A820
		.BYTE	$10,$44,$65,$74,$72,$6F,$69,$74,$BB,$6D,$49,$BC,$5C,$7F,$7D,$5D,$00
		.BYTE	0
_tlm_scenario_header3:
		.WORD	$8820
		.BYTE	$10,$14,$2F,$2B,$39,$2F,$00,$0A,$21,$30,$21,$2E,$00,$1C,$3F,$3C,$1C
		.WORD	$A820
		.BYTE	$10,$54,$6F,$6B,$79,$6F,$BB,$4A,$61,$70,$61,$6E,$00,$5C,$7F,$7C,$5C
		.BYTE	0
_tlm_scenario_header4:
		.WORD	$8820
		.BYTE	$10,$02,$2F,$23,$34,$2F,$2E,$9B,$0D,$01,$00,$00,$1D,$1B,$1C,$1B,$00
		.WORD	$A820
		.BYTE	$10,$42,$6F,$73,$74,$6F,$6E,$BB,$6D,$41,$BC,$00,$5D,$5B,$5C,$5B,$00
		.BYTE	0
_tlm_scenario_header5:
		.WORD	$8320
		.BYTE	$1A,$12,$29,$2F,$00,$24,$25,$00,$0A,$21,$2E,$25,$29,$32,$2F,$9B,$02,$32,$21,$3A,$29,$2C,$00,$1D,$1B,$1F,$3D
		.WORD	$A320
		.BYTE	$1A,$52,$69,$6F,$00,$64,$65,$00,$4A,$61,$6E,$65,$69,$72,$6F,$BB,$42,$72,$61,$7A,$69,$6C,$00,$5D,$5B,$5F,$7D
		.BYTE	0
_tlm_scenario_loose_header:
		.WORD	$8720
		.BYTE	$12,$09,$2D,$30,$25,$21,$23,$28,$2D,$25,$2E,$34,$00,$2E,$2F,$34,$29,$23,$25
		.WORD	$A720
		.BYTE	$12,$49,$6D,$70,$65,$61,$63,$68,$6D,$65,$6E,$74,$00,$6E,$6F,$74,$69,$63,$65
		.BYTE	0
_tlm_practice_wnd_header:
		.WORD	$8320
		.BYTE	$5A,$DF
		.WORD	$A320
		.BYTE	$5A,$DF
		.WORD	$C320
		.BYTE	$5A,$F9
		.WORD	$8C20
		.BYTE	$08,$8E,$AD,$AC,$C1,$AB,$C2,$C1,$AF
		.WORD	$AC20
		.BYTE	$08,$9E,$BD,$BC,$D1,$BB,$D2,$D1,$BF
		.WORD	$8324
		.BYTE	$5A,$01
		.WORD	$A324
		.BYTE	$5A,$01
		.WORD	$C324
		.BYTE	$5A,$00
		.WORD	$8D24
		.BYTE	$47,$00
		.WORD	$AD24
		.BYTE	$47,$00
		.BYTE	0
_tlm_all_complete_:
		.WORD	$8320
		.BYTE	$5A,$DF
		.WORD	$A320
		.BYTE	$5A,$DF
		.WORD	$C320
		.BYTE	$5A,$F9
		.WORD	$8920
		.BYTE	$0E,$8A,$8D,$8C,$8F,$AD,$AC,$AB,$C5,$C9,$AC,$AB,$C2,$8D,$8C
		.WORD	$A920
		.BYTE	$0E,$9A,$9D,$9C,$8B,$BD,$BC,$BB,$D5,$D9,$BC,$BB,$D2,$9D,$9C
		.WORD	$8324
		.BYTE	$5A,$01
		.WORD	$A324
		.BYTE	$5A,$01
		.WORD	$C324
		.BYTE	$5A,$00
		.WORD	$8D24
		.BYTE	$48,$00
		.WORD	$AD24
		.BYTE	$48,$00
		.BYTE	0
_scenario_data_chr_bank:
		.BYTE	$1B,$1A,$1C,$1A,$1D,$1D,$1E,$1F,$1F
_scenario_data_ppu_lo:
		.BYTE	$A2,$00,$36,$D3,$36,$C2,$12,$9E,$E3
_scenario_data_ppu_hi:
		.BYTE	$18,$00,$24,$0D,$31,$3D,$4A,$56,$56
off_251A3:
		.BYTE	<[byte_251B5]
		.BYTE	<[byte_251D3]
		.BYTE	<[byte_251F1]
		.BYTE	<[byte_2520F]
		.BYTE	<[byte_2522D]
		.BYTE	<[byte_2524B]
		.BYTE	<[byte_251F1]
		.BYTE	<[byte_251B5]
		.BYTE	<[byte_2522D]
off_251AC:
		.BYTE	>[byte_251B5]
		.BYTE	>[byte_251D3]
		.BYTE	>[byte_251F1]
		.BYTE	>[byte_2520F]
		.BYTE	>[byte_2522D]
		.BYTE	>[byte_2524B]
		.BYTE	>[byte_251F1]
		.BYTE	>[byte_251B5]
		.BYTE	>[byte_2522D]
byte_251B5:
		.BYTE	$90,$90,$90,$92,$92,$90,$91,$91,$91,$98,$97,$91,$92,$9D,$99,$91
		.BYTE	$91,$99,$9D,$90,$9D,$92,$90,$9D,$DB,$D1,$95,$91,$93,$91
byte_251D3:
		.BYTE	$9B,$91,$91,$9B,$91,$91,$90,$90,$DA,$DB,$92,$90,$DE,$DB,$90,$92
		.BYTE	$DB,$DE,$DD,$DE,$DE,$DF,$9A,$91,$91,$99,$D7,$D8,$97,$91
byte_251F1:
		.BYTE	$9B,$91,$91,$9B,$91,$91,$BD,$BD,$90,$90,$91,$BA,$9D,$92,$BD,$AE
		.BYTE	$92,$9D,$90,$92,$AE,$BD,$B8,$9C,$B7,$9B,$92,$90,$91,$B7
byte_2520F:
		.BYTE	$91,$BA,$9B,$91,$BA,$9B,$90,$90,$9C,$BC,$91,$91,$9D,$92,$BD,$BD
		.BYTE	$DA,$DB,$90,$92,$92,$92,$DB,$DC,$D9,$90,$9A,$91,$DB,$DE
byte_2522D:
		.BYTE	$90,$90,$CC,$C4,$C4,$CC,$C7,$BF,$D1,$D2,$BF,$C7,$BD,$BD,$DD,$DE
		.BYTE	$DA,$DB,$DE,$DF,$D8,$D9,$DB,$DC,$CD,$90,$C9,$BD,$D7,$D8
byte_2524B:
		.BYTE	$DD,$DE,$DA,$DB,$91,$91,$92,$90,$92,$92,$9A,$91,$DE,$DF,$BA,$91
		.BYTE	$B2,$92,$9D,$92,$90,$92,$DE,$DB,$B2,$B2,$99,$91,$91,$99

_eof	EQU		$5B
_quo	EQU		$22

		MAC		MSG_DIC
{1}		EQU		[[.-_msg_dictionary]|$80]
		.BYTE	{2},{3}
		ENDM

_msg_dictionary:
		MSG_DIC	_the,	"the",$FF
		MSG_DIC	_to,	"to",$FF
		MSG_DIC	_and,	"and",$FF
		MSG_DIC	_of,	"of",$FF
		MSG_DIC	_have,	"have",$FF
		MSG_DIC	_city_,	"city",$FF
		MSG_DIC	_You,	"You",$FF
		MSG_DIC	_years,	"years",$FF
		MSG_DIC	_in,	"in",$FF
		MSG_DIC	_your,	"your",$FF
		MSG_DIC	_you,	"you",$FF
		MSG_DIC	_con,	"con",$FF
_scenario_msg6:
		.BYTE	_eof
_scenario_msg7:
		.BYTE	_eof
_scenario_msg0:
		.BYTE	" Damage from ",_the," earth-",$A
		.BYTE	"quake was m",_in,"or compared",$A
		.BYTE	_to," that ",_of," ",_the," ensu",_in,"g",$A
		.BYTE	"fires,which ",_to,"ok days ",_to,$A
		.BYTE	_con,"trol.1500people died.",$A
		.BYTE	" Controll",_in,"g ",_the," fires",$A
		.BYTE	"should be ",_your," ",_in,"itial",$A
		.BYTE	_con,"cern.Then clear ",_the,$A
		.BYTE	"rubble ",_and," start rebuild-",$A
		.BYTE	_in,"g.",_You," ",_have," 5 ",_years,".",$A
		.BYTE	_eof
_scenario_msg1:
		.BYTE	"The roads here are becom-",$A
		.BYTE	_in,"g more ",_con,"gested every",$A
		.BYTE	"day,",_and," ",_the," residents",$A
		.BYTE	"are upset.They dem",_and,$A
		.BYTE	"that ",_you," do someth",_in,"g",$A
		.BYTE	"about it.",$A
		.BYTE	" Some ",_have," suggested a",$A
		.BYTE	"mass transit system as",$A
		.BYTE	_the," answer,but this",$A
		.BYTE	"would require major re-",$A
		.BYTE	"zon",_in,"g ",_in," ",_the," down",_to,"wn",$A
		.BYTE	"area.",_You," ",_have," 10 ",_years,".",$A
		.BYTE	_eof
_scenario_msg2:
		.BYTE	" By 1970,competition from",$A
		.BYTE	"overseas ",_and," o",_the,"r e",_con,"o-",$A
		.BYTE	"mic fac",_to,"rs pushed ",_the,$A
		.BYTE	"once",_quo,"au",_to,"mobil capital",$A
		.BYTE	_of," ",_the," world",_quo,_in,_to," reces-",$A
		.BYTE	"sion.Plummet",_in,"g l",_and," val-",$A
		.BYTE	"ues ",_and," unemployment ",_the,"n",$A
		.BYTE	_in,"creased crime ",_in," ",_the,$A
		.BYTE	_in,"ner-",_city_," ",_to," chronic",$A
		.BYTE	"levels.",_You," ",_have," 10 ",_years,$A
		.BYTE	_to," reduce crime ",_and," re-",$A
		.BYTE	"build ",_the," ",_in,"dustrial base",$A
		.BYTE	_of," ",_the," ",_city_,".",$A
		.BYTE	_eof
_scenario_msg3:
		.BYTE	" A large reptilian crea-",$A
		.BYTE	"ture has been spooted",$A
		.BYTE	"head",_in,"g for Tokyo. It",$A
		.BYTE	"seems ",_to," be attracted ",_to,$A
		.BYTE	_the," heavy levels ",_of," ",_in,"-",$A
		.BYTE	"dustrial pollution ",_the,"re.",$A
		.BYTE	" Try ",_to," ",_con,"trol ",_the,$A
		.BYTE	"fires,",_the,"n rebuild ",_the,$A
		.BYTE	_in,"dustrial center.",_You,$A
		.BYTE	_have," 5 ",_years,".",$A
		.BYTE	_eof
_scenario_msg4:
		.BYTE	" A major meltdown ",_to,$A
		.BYTE	"occur at one ",_of," ",_the," new",$A
		.BYTE	"down",_to,"wn nuclear reac-",$A
		.BYTE	_to,"rs.The area ",_in," ",_the,$A
		.BYTE	"vic",_in,"ity ",_of," ",_the," reac",_to,"r",$A
		.BYTE	"will be severely ",_con,"tam-",$A
		.BYTE	_in,"ated by rediation,forc-",$A
		.BYTE	_in,"g ",_you," ",_to," restructure",$A
		.BYTE	_the," ",_city_," around it.",_You,$A
		.BYTE	_have," 5 ",_years," ",_to," get ",_the,$A
		.BYTE	"situation under ",_con,"trol.",$A
		.BYTE	_eof
_scenario_msg5:
		.BYTE	" By ",_the," mid-21st century,",$A
		.BYTE	_the," greenhouse effect",$A
		.BYTE	_in,"creased global tempera-",$A
		.BYTE	"tures 4",$27,"F.Polar icecaps",$A
		.BYTE	"melted ",_and," raised sea",$A
		.BYTE	"levels worldwide.Coastal",$A
		.BYTE	"areas were devastated by",$A
		.BYTE	"flood ",_and," erosion.",$A
		.BYTE	" ",_You," ",_have," 10 ",_years," ",_to,$A
		.BYTE	"turn this swamp back ",_in,_to,$A
		.BYTE	"a ",_city_," aga",_in,".",$A
		.BYTE	_eof
_game_won_msg:
		.BYTE	" ",_You,"r mayorial skill ",_and,$A
		.BYTE	_city_," plann",_in,"g expertise",$A
		.BYTE	_have," earned ",_you,$A
		.BYTE	_the," KEY TO THE CITY.",$A
		.BYTE	" Local residents will",$A
		.BYTE	"erect monuments ",_to," ",_your,$A
		.BYTE	"glory ",_and," name ",_the,"ir",$A
		.BYTE	"first-born children after",$A
		.BYTE	_you,". Why not run for",$A
		.BYTE	"governor?",$A
		.BYTE	_eof
_game_lose_msg:
		.BYTE	" The entire population ",_of,$A
		.BYTE	"this ",_city_," has f",_in,"ally had",$A
		.BYTE	"enough ",_of," ",_your," ",_in,"ept",$A
		.BYTE	"plann",_in,"g ",_and," ",_in,"competant",$A
		.BYTE	"management. An angry mob",$A
		.BYTE	"- lead by ",_your," mo",_the,"r -",$A
		.BYTE	"has been spotted ",_in," ",_the,$A
		.BYTE	"vic",_in,"ity ",_of," ",_city_," hall.",$A
		.BYTE	_You," should seriously",$A
		.BYTE	_con,"sider tak",_in,"g an ",$A
		.BYTE	"extended vacation - NOW.",$A
		.BYTE	"(Or read ",_the," manual",$A
		.BYTE	_and," try aga",_in,").",$A
		.BYTE	_eof
_practice_msg0:
		.BYTE	"Welcome ",_to," ",_the," world ",_of,$A
		.BYTE	"SIM CITY!  SIM CITY is a",$A
		.BYTE	_city_," build",_in,"g game,",$A
		.BYTE	"where ",_you," are ",_the," mayor.",$A
		.BYTE	_You," do ",_the," l",_and," zon",_in,"g,",$A
		.BYTE	"adjust ",_the," budget,",$A
		.BYTE	"lower skyrocket",_in,"g  crime",$A
		.BYTE	"rates, iron out traffic",$A
		.BYTE	"snarls",_con,"trol enviro-",$A
		.BYTE	"nmental pollution, ",_and,$A
		.BYTE	"many o",_the,"r elements that",$A
		.BYTE	"require ",_your," attention.",$A
		.BYTE	"To beg",_in," with,  lets aim",$A
		.BYTE	"at build",_in,"g a ",_city_," with",$A
		.BYTE	"a population ",_of," 30,000",$A
		.BYTE	"people.",$A
		.BYTE	_eof
_practice_msg1:
		.BYTE	_You," can see ",_the," entier",$A
		.BYTE	"map by press",_in,"g ",_the," B",$A
		.BYTE	"but",_to,"n ",_and," ",_the," ",_con,"trol",$A
		.BYTE	"pad simultaneously.",$A
		.BYTE	_eof
_all_scenarios_clear_msg:
		.BYTE	" ",_You," are great.",$A
		.BYTE	"All scenarios are cleared",$A
		.BYTE	"But ",_the,"re is more",$A
		.BYTE	"scenarios ",_and," ",_the,"y are",$A
		.BYTE	"very difficult.",$A
		.BYTE	_eof
_tlm_res_fullscreen_wnd:
		.WORD	$2000
		.BYTE	1
		.WORD	_pak_tlm_fullscreen_wnd0
_pak_tlm_fullscreen_wnd0:
		.BYTE	$E4,$61,$DD,$00,$A5,$39,$A6,$00,$A7,$23,$DD,$00,$B5,$24, $00,$0F
		.BYTE	$19,$2F,$35,$00,$21,$32,$25,$00,$21,$00,$37,$29,$2E,$2E,$25,$32
		.BYTE	$24,$00,$00,$B7,$89,$00,$7E,$0F,$59,$6F,$75,$00,$61,$72,$65,$00
		.BYTE	$61,$00,$77,$69,$6E,$6E,$65,$72,$8A,$00,$98,$24,$81,$0F,$99,$AF
		.BYTE	$B5,$81,$A1,$A3,$85,$81,$A1,$81,$B7,$A9,$AE,$AE,$85,$A3,$24,$81
		.BYTE	$85,$00,$9D,$39,$20,$9F,$00,$DD,$F0,$3F,$00,$DD,$F0,$7F,$00,$DD
		.BYTE	$F0,$FF,$00,$DD,$F0,$64,$00,$DD,$00,$C5,$39,$C6,$00,$C7,$E4,$61
		.BYTE	$DD,$E4,$3F,$FF,$E4,$61,$81,$3B,$CA,$84,$04,$5E,$39,$98,$9F,$04
		.BYTE	$7D,$F0,$25,$04,$7D,$39,$0B,$9F,$04,$DD,$F0,$3F,$04,$DD,$F0,$7F
		.BYTE	$04,$DD,$F0,$FF,$04,$DD,$F0,$65,$04,$DD,$3A,$CA,$E4,$61,$81,$E4
		.BYTE	$3F,$00,$FF
_tlm_res_scenario_normal:
		.WORD	$2000
		.BYTE	1
		.WORD	byte_25C2F
byte_25C2F:
		.BYTE	$E4,$62,$DD,$00,$A5,$37,$A6,$00,$A7,$25,$DD,$00,$B5,$23,$DF,$0F
		.BYTE	$AA,$AF,$C9,$AF,$C1,$AB,$DF,$DF,$AA,$C1,$AF,$8C,$AC,$AD,$C2,$8D
		.BYTE	$23,$DF,$00,$B7,$8A,$00,$7D,$0F,$BA,$BF,$D9,$BF,$D1,$BB,$DF,$DF
		.BYTE	$BA,$D1,$BF,$9C,$BC,$BD,$D2,$9D,$8B,$00,$98,$37,$F9,$87,$00,$9C
		.BYTE	$01,$DC,$A0,$23,$A1,$01,$A2,$DC,$87,$00,$E4,$87,$00,$E4,$88,$00
		.BYTE	$DC,$00,$B0,$63,$84,$03,$B2,$DC,$DC,$B0,$63,$8C,$83,$01,$0A,$63
		.BYTE	$C4,$00,$B2,$8A,$00,$FB,$63,$94,$83,$01,$0A,$63,$9C,$83,$01,$0A
		.BYTE	$63,$D4,$8B,$01,$1A,$63,$A4,$83,$01,$0A,$63,$AC,$83,$01,$0A,$63
		.BYTE	$E4,$8B,$01,$1A,$63,$B4,$83,$01,$0A,$63,$BC,$83,$01,$0A,$63,$F4
		.BYTE	$8A,$01,$1A,$00,$C0,$23,$C1,$01,$C2,$DC,$87,$01,$84,$06,$DC,$C0
		.BYTE	$C1,$C1,$C1,$C1,$C2,$89,$00,$FB,$62,$CD,$62,$D3,$23,$DC,$01,$28
		.BYTE	$F3,$24,$DC,$63,$FC,$00,$DC,$89,$00,$FB,$03,$DC,$DC,$C7,$D6,$25
		.BYTE	$DC,$01,$C7,$9B,$86,$01,$C9,$01,$85,$DC,$8A,$01,$BA,$64,$D7,$00
		.BYTE	$D1,$22,$DC,$63,$F4,$23,$DC,$63,$F8,$01,$DC,$DC,$F0,$29,$00,$DC
		.BYTE	$63,$C0,$83,$01,$0A,$63,$C8,$83,$01,$0A,$63,$CC,$8B,$01,$1A,$63
		.BYTE	$D0,$83,$01,$0A,$63,$D8,$83,$01,$0A,$63,$DC,$8B,$01,$1A,$63,$E0
		.BYTE	$83,$01,$0A,$63,$E8,$83,$01,$0A,$63,$EC,$8B,$01,$1A,$63,$F0,$83
		.BYTE	$01,$0A,$63,$F8,$83,$01,$0A,$63,$FC,$F0,$2A,$01,$7A,$00,$DC,$63
		.BYTE	$8C,$23,$DC,$62,$AD,$83,$01,$C8,$63,$2B,$01,$60,$61,$8C,$01,$BB
		.BYTE	$00,$AB,$25,$DC,$01,$0F,$7C,$86,$02,$E9,$00,$6E,$8B,$01,$D9,$00
		.BYTE	$DC,$63,$D6,$23,$DC,$03,$85,$86,$88,$89,$23,$DC,$63,$F0,$8B,$01
		.BYTE	$BA,$03,$DA,$DB,$CE,$CF,$23,$DC,$00,$8A,$62,$9D,$00,$DC,$88,$01
		.BYTE	$EB,$86,$00,$9C,$00,$C5,$37,$C6,$00,$C7,$E4,$62,$DD,$27,$00,$2F
		.BYTE	$FF,$8F,$03,$C0,$8F,$03,$C0,$27,$00,$E4,$62,$81,$39,$CA,$86,$04
		.BYTE	$5D,$23,$01,$25,$00,$84,$04,$86,$00,$01,$84,$04,$8B,$22,$01,$9F
		.BYTE	$04,$7C,$87,$04,$7C,$37,$80,$07,$CA,$81,$81,$81,$81,$81,$81,$CA
		.BYTE	$37,$01,$89,$04,$7C,$23,$53,$01,$01,$01,$87,$05,$04,$05,$01,$01
		.BYTE	$53,$53,$53,$53,$9F,$04,$FA,$F0,$3F,$04,$FA,$01,$01,$01,$F0,$28
		.BYTE	$04,$DC,$25,$06,$23,$01,$85,$05,$A9,$00,$01,$23,$03,$8C,$04,$9A
		.BYTE	$03,$06,$06,$01,$01,$8A,$05,$AB,$02,$01,$06,$06,$94,$05,$99,$03
		.BYTE	$06,$06,$06,$06,$94,$05,$B2,$34,$01,$F0,$A8,$04,$FC,$00,$01,$23
		.BYTE	$02,$89,$05,$B2,$00,$01,$25,$05,$F0,$29,$05,$BB,$0C,$01,$02,$02
		.BYTE	$02,$02,$01,$01,$01,$01,$02,$02,$02,$02,$93,$05,$B2,$9D,$07,$06
		.BYTE	$38,$CA,$E4,$62,$81,$E4,$3F,$00,$FF
_tlm_res_game_play_level:
		.WORD	$2000
		.BYTE	1
		.WORD	_pak_tlm_game_play_level0
_pak_tlm_game_play_level0:
		.BYTE	 $0E5,$03,$DD,$00,$A5,$35,$A6,$00,$A7,$27,$DD,$00,$B5,$23,$DF,$0E
		.BYTE	$8A,$AC,$C6,$87,$DF,$8E,$C9,$AC,$7B,$DF,$FA,$AF,$C7,$AF,$C9,$22
		.BYTE	$DF,$00,$B7,$8C,$01,$1C,$0E,$9F,$BC,$D6,$97,$DF,$9E,$D9,$BC,$8B
		.BYTE	$DF,$FB,$BF,$D7,$BF,$D9,$8C,$01,$38,$2B,$F9,$00,$7A,$28,$F9,$89
		.BYTE	$01,$3B,$35,$DD,$8A,$01,$7B,$03,$45,$61,$73,$79,$22,$DD,$64,$C4
		.BYTE	$08,$B7,$7B,$24,$32,$30,$9C,$9D,$9D,$DD,$F0,$2A,$01,$7B,$05,$4D
		.BYTE	$65,$64,$69,$75,$6D,$88,$01,$AC,$00,$31,$F0,$2F,$01,$B6,$03,$48
		.BYTE	$61,$72,$64,$8A,$01,$AA,$01,$DD,$35,$F0,$2C,$01,$B7,$00,$C5,$35
		.BYTE	$C6,$00,$C7,$E5,$43,$DD,$27,$FF,$01,$00,$00,$83,$03,$C6,$01,$00
		.BYTE	$00,$27,$FF,$23,$C3,$23,$3F,$0F,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
		.BYTE	$00,$00,$00,$00,$FF,$FF,$FF,$FF,$8F,$03,$E0,$E5,$03,$86,$37,$CA
		.BYTE	$88,$04,$FC,$24,$01,$01,$00,$00,$84,$05,$27,$01,$01,$01,$24,$00
		.BYTE	$22,$01,$9F,$05,$1B,$89,$05,$1B,$2B,$80,$00,$01,$28,$80,$89,$05
		.BYTE	$1B,$35,$89,$8A,$05,$7B,$23,$8B,$22,$89,$25,$88,$23,$8B,$22,$8A
		.BYTE	$00,$89,$F0,$2E,$05,$7B,$01,$8B,$8B,$F0,$3D,$05,$AC,$8A,$05,$AA
		.BYTE	$00,$89,$F0,$2E,$05,$B6,$36,$CA,$E5,$43,$86,$28,$00,$02,$FF,$FF
		.BYTE	$FF,$2B,$00,$87,$03,$D8,$2B,$00,$03,$FF,$FF,$FF,$FF,$8F,$07,$E0
		.BYTE	$FF

		BANK_END 1,$C000
