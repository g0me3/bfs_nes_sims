
		ORG $E000

		BANK_START $F

		INCBIN "..\copyrights.txt"

; =============== S U B	R O U T	I N E =======================================
RESET:
		SEI
		CLD
		LDX		#0
		STX		_PPU_CTRL
		STX		_PPU_MASK
		INX
loc_18A00B:
		INC		_frames_counter
		LDA		_PPU_STATUS
		BPL		loc_18A00B
		DEX
		BNE		loc_18A00B
		DEX
		TXS
		LDX		#4
		STX		_MMC5_PRG_PAGES
		LDX		#0
		STX		_APU_STATUS
		STX		_ppu_mask_shadow
loc_18A023:
		LDA		aIshi,X
		BMI		loc_18A033
		CMP		_ram_signature,X
		BNE		loc_18A030
		INX
		BNE		loc_18A023
loc_18A030:
		JSR		_map_buf_init
loc_18A033:
		LDY		#>[_pre_ppu_buf]
		STY		_ptr0+1
		LDY		#<[_pre_ppu_buf]
		STY		_ptr0
		TYA
loc_18A03C:
		STA		(_ptr0),Y
		DEY
		BNE		loc_18A03C
		DEC		_ptr0+1
		BPL		loc_18A03C
		LDX		#0
		LDY		#4
loc_18A049:
		LDA		aIshi,X
		STA		_ram_signature,X
		INX
		DEY
		BNE		loc_18A049
loc_18A053:
		JSR		_get_input
		LDA		_pad0_held
		AND		#$F0
		CMP		#$F0
		BEQ		loc_18A053
		JSR		_gui_ctrl_menu_pos_list_ilit
		LDA		#3
		STA		_MMC5_PRG_SIZE
		LDA		#4
		STA		_MMC5_PRG_PAGES
		LDY		#$FC
		STY		_prg_prg8000_shadow
		STY		_MMC5_PRG_PAGES+1
		INY
		STY		_prg_prgA000_shadow
		STY		_MMC5_PRG_PAGES+2
		INY
		STY		_MMC5_PRG_PAGES+3
		INY
		STY		_MMC5_PRG_PAGES+4
		LDA		#0
		STA		byte_9
		LDA		#$C0
		STA		_PAD1_REG
		LDA		#0
		STA		byte_4084+$F8C
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F0
		.WORD	_apu_init
		LDA		#$80
		STA		_apu_mus_idx_req
		LDX		#$1F
		LDA		#$F
loc_18A09D:
		STA		_pal_bg_buf,X
		DEX
		BPL		loc_18A09D
		LDA		#3
		STA		_MMC5_CHR_SIZE
		LDA		#0
		STA		_MMC5_CHR_BANKSA+7
		LDA		#2
		STA		_MMC5_CHR_MODE
		STA		_mmc5_chr_mode_shadow
		LDA		#0
		STA		_mmc5_sp_mode_shadow
		STA		_MMC5_SP_MODE
		LDA		#0
		STA		_MMC5_SP_SCROLL
		STA		_mmc5_sp_scroll_shadow
		STA		_MMC5_SP_PAGE
		LDA		#0
		STA		_MMC5_NT_MODE
		LDA		#$FF
		STA		_MMC5_NT_FILL
		LDA		#0
		STA		_MMC5_AT_FILL
		LDA		#0
		STA		_MMC5_IRQ_ENABLE
		STA		_title_sub_idx
		JSR		_spr_clear
		LDY		#1
		STY		_rand_seed
		INY
		STY		_rand_seed+1
		INY
		STY		_rand_seed+2
		JSR		_thread1_init
		LDX		#4
		LDA		#$64
loc_18A0F0:
		STA		_funding_level,X
		DEX
		BPL		loc_18A0F0
		JSR		_sram_on
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_sram_init
		JSR		_sram_off
		LDA		#$FF
		STA		_game_core_state
		STA		byte_37
		LDA		#1
		STA		_MMC5_CHR_MODE
		STA		_mmc5_chr_mode_shadow
		LDA		#$80
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		CLI

; =============== S U B	R O U T	I N E =======================================
_THREAD0_LOOP:
		JSR		_THREAD0_MAIN
		JSR		_wait_for_nmi
		JMP		_THREAD0_LOOP

; =============== S U B	R O U T	I N E =======================================
_THREAD0_MAIN:
		LDX		_main_sub_idx
		BNE		_game_setup_mode
		LDA		#0
		STA		_scr_res_idx
		JSR		_ppu_queues_reset
		JSR		_farcall_argsAXY_safe
		.BYTE	$F7,$F9
		.WORD	_title_screen
		INC		_main_sub_idx
		RTS
_game_setup_mode:
		DEX
		BNE		_ingame_mode
		JSR		_ppu_queues_reset
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F1
		.WORD	_game_setup
		RTS
_ingame_mode:
		JSR		_ppu_queues_reset
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$FA
		.WORD	_game_vars_init
		JSR		_thread1_init
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_game_start
		RTS
aIshi:
		.BYTE	"ISHI",$FF

; =============== S U B	R O U T	I N E =======================================
NMI:
		PHA
		TXA
		PHA
		TYA
		PHA
		LDA		_nmi_busy_flag
		BEQ		loc_18A178
		CLI
		JSR		_ppu_regs_reload
		JSR		_ppu_scrolls_reload
		JSR		_mmc5_scrolls_reload
		JMP		loc_18A35B
loc_18A178:
		INC		_nmi_busy_flag
		LDA		_mmc5_operation_in_progress_flag
		BEQ		loc_18A196
		LDA		_ppu_mask_shadow
		AND		#$18
		BEQ		loc_18A193
		LDA		#2
		STA		_PPU_OAM_DMA
		CLI
		JSR		_ppu_regs_reload
		JSR		_ppu_scrolls_reload
		JSR		_mmc5_scrolls_reload
loc_18A193:
		JMP		loc_18A359
loc_18A196:
		LDA		#2
		STA		_PPU_OAM_DMA
		CLI
		LDA		_ppu_pal_dequeue_req
		BEQ		loc_18A1B8
		JSR		_pal_ppu_send
		LDA		#0
		STA		_ppu_pal_dequeue_req
		JSR		_ppu_regs_reload
		LDA		_wnd_ppu_data_backup_req
		BEQ		loc_18A1F3
		JSR		_ppu_nt_data_backup
		INC		_wnd_cur_line_num
		JMP		loc_18A1F3
loc_18A1B8:
		LDA		_scr_mode_idx
		BNE		loc_18A1DE
		LDX		_pal_anim_counter
		LDA		_pal_anim_table,X
		LDY		#$11
		LDX		#$3F
		STX		_PPU_ADDR
		STY		_PPU_ADDR
		STA		_PPU_DATA
		LDA		#$3F
		STA		_PPU_ADDR
		LDA		#0
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		STA		_PPU_ADDR
loc_18A1DE:
		JSR		_ppu_regs_reload
		LDA		_wnd_ppu_data_backup_req
		BEQ		loc_18A1EA
		JSR		_ppu_nt_data_backup
		INC		_wnd_cur_line_num
loc_18A1EA:
		LDA		_ppu_queue_pos_tail
		CMP		_ppu_queue_pos_head
		BEQ		loc_18A1F3
		JSR		_ppu_dequeue
loc_18A1F3:
		JSR		_ppu_scrolls_reload
		LDA		_main_sub_idx
		BNE		loc_18A213
		LDA		_mmc5_chr_banks_shadow
		STA		_MMC5_CHR_BANKSA
		LDA		_mmc5_chr_banks_shadow+1
		STA		_MMC5_CHR_BANKSA+1
		LDA		_mmc5_chr_banks_shadow+2
		STA		_MMC5_CHR_BANKSA+2
		LDA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
loc_18A213:
		JSR		_mmc5_scrolls_reload
		TSX
		LDY		_cur_thread_idx
		TXA
		STA		_thread_stack,Y
		LDA		_prg_prg8000_shadow
		STA		_thread_prg8000,Y
		LDA		_prg_prgA000_shadow
		STA		_thread_prgA000,Y
		LDA		_sram_state_flag
		STA		_thread_sram_state,Y
		JSR		_get_input
		INC		_frames_counter
		INC		_frames_div8_counter
		LDA		_frames_div8_counter
		AND		#7
		STA		_frames_div8_counter
		INC		_frames_div4_counter
		LDA		_frames_div4_counter
		AND		#$F
		STA		_frames_div4_counter
		LDA		#0
		STA		_spr_buf_pos
		LDA		_scr_mode_idx
		BEQ		loc_18A258
		LDA		_scr_res_idx
		CMP		#1
		BNE		loc_18A2A1
		LDA		_maps_info_mode_idx
		BNE		loc_18A2A1
loc_18A258:
		LDA		_prg_prg8000_shadow
		PHA
		LDA		_prg_prgA000_shadow
		PHA
		LDA		#$F5
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		#$FD
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_scr_mode_idx
		CMP		#1
		BEQ		loc_18A284
		PHA
		JSR		_spr_game_interface_cursor_draw
		PLA
		BNE		loc_18A27C
		JSR		_goto_alert_spr_draw
loc_18A27C:
		LDA		_obj_active_flags
		BEQ		loc_18A284
		JSR		_obj_handlers
loc_18A284:
		LDA		_scr_mode_idx
		BEQ		loc_18A28D
		LDA		_obj_active_flags
		BNE		loc_18A28D
loc_18A28D:
		LDA		_scr_res_window_opened_flag
		BNE		loc_18A295
		JSR		_spr_finish
loc_18A295:
		PLA
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		PLA
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
loc_18A2A1:
		LDA		_game_wnd_spr_mode_flag
		BNE		loc_18A2D6
		LDA		_city._game_speed
		CMP		#3
		BNE		loc_18A2B4
		LDA		#0
		STA		_game_core_stop_flag
		BEQ		loc_18A2D6
loc_18A2B4:
		LDA		_game_core_stop_flag
		BNE		loc_18A2BC
		INC		_game_core_stop_timer
loc_18A2BC:
		LDA		_game_core_stop_timer
		LDX		_city._game_speed
		BNE		loc_18A2CA
		AND		#$7F
		BNE		loc_18A2D6
		BEQ		loc_18A2CE
loc_18A2CA:
		AND		#$1F
		BNE		loc_18A2D6
loc_18A2CE:
		LDA		#$FF
		STA		_game_core_stop_flag
		STA		_game_core_state
loc_18A2D6:
		LDA		_thread_priority_req
		BPL		loc_18A2FC
		AND		#$7F
		TAY
		LDX		#0
		CMP		#8
		BCC		loc_18A2E5
		INX
loc_18A2E5:
		LDA		_bitmasks,Y
		AND		_thread0_flags,X
		BNE		_switch_to_thread1
		LDA		_thread_priority_req
		AND		#$7F
		STA		_thread_priority_req
		LDA		#1
		STA		_game_core_state
		BNE		_switch_to_thread0
loc_18A2FC:
		LDA		_game_core_state
		ORA		_pad0_held
		BNE		_switch_to_thread0
		LDX		#0
		LDY		_maps_info_mode_idx
		CPY		#8
		BCC		loc_18A30D
		INX
loc_18A30D:
		LDA		_bitmasks,Y
		AND		_thread1_flags,X
		BEQ		_switch_to_thread1
		LDY		#0
		STY		_thread1_flags
		STY		_thread1_flags+1
		INY
		STY		byte_107
		STY		_game_core_state
_switch_to_thread0:
		LDX		#0
		BEQ		loc_18A32E
_switch_to_thread1:
		LDX		#0
		STX		_game_core_state
		INX
loc_18A32E:
		STX		_cur_thread_idx
		LDY		_cur_thread_idx
		LDA		_thread_sram_state,Y
		STA		_sram_state_flag
		JSR		_sram_enable_restore
		LDA		_thread_prg8000,Y
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		_thread_prgA000,Y
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDX		_thread_stack,Y
		TXS
		LDA		_ppu_mask_shadow
		AND		#$18
		BEQ		loc_18A356
		JSR		_wait_for_mmc5_irq
loc_18A356:
		JSR		_extnt_dequeue
loc_18A359:
		DEC		_nmi_busy_flag
loc_18A35B:
		LDA		#$FA
		STA		_MMC5_PRG_PAGES+1
		LDA		#$F0
		STA		_MMC5_PRG_PAGES+2
		JSR		_apu_play
		LDA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_cur_thread_idx
		BEQ		loc_18A382
		LDA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
loc_18A382:
		LDA		byte_674
		ORA		_input_forbid_delay
		BNE		loc_18A3A6
		LDA		_scroll_msg_active_flag
		BEQ		loc_18A3A6
		LDA		#$F3
		STA		_MMC5_PRG_PAGES+2
		LDA		#$F7
		STA		_MMC5_PRG_PAGES+1
		JSR		_scroll_msg_char_output
		LDA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
loc_18A3A6:
		INC		_pal_anim_counter
		LDA		_pal_anim_counter
		CMP		#$28
		BCC		loc_18A3B2
		LDA		#0
		STA		_pal_anim_counter
loc_18A3B2:
		LDA		_pad0_held
		AND		#$F0
		CMP		#$F0
		BNE		loc_18A3BD
		JMP		RESET
loc_18A3BD:
		INC		byte_525
		LDA		byte_525
		CMP		#$3C
		BCC		loc_18A3CF
		INC		byte_524
		LDA		#0
		STA		byte_525
loc_18A3CF:
		LDA		#$FF
		STA		_nmi_flag
		PLA
		TAY
		PLA
		TAX
		PLA
		RTI
_pal_anim_table:
		.BYTE	$36,$36,$36,$36,$36,$36
		.BYTE	$26,$26,$26,$26,$26,$26
		.BYTE	$16,$16,$16,$16,$16,$16,$16,$16
		.BYTE	$06,$06,$06,$06,$06,$06
		.BYTE	$16,$16,$16,$16,$16,$16,$16,$16
		.BYTE	$26,$26,$26,$26,$26,$26
_bitmasks:
		.BYTE	$01,$02,$04,$08,$10,$20,$40,$80,$01,$02,$04
_inv_bitmasks:
		.BYTE	$80,$40,$20,$10,$08,$04,$02,$01

; =============== S U B	R O U T	I N E =======================================
_ppu_scrolls_reload:
		LDA		_game_wnd_spr_mode_flag
		LDX		#0
		ASL
		BCS		loc_18A45A
		LDA		_scr_mode_idx
		CMP		#3
		BNE		loc_18A41F
		LDA		#0
		TAX
		BEQ		loc_18A45A
loc_18A41F:
		LDA		_cur_screen_tile._X
		ASL
		ASL
		LDX		_game_msg_wnd_active_flag
		BNE		loc_18A44B
		LDX		byte_674
		BEQ		loc_18A44B
		STA		_tmp15
		LDA		_frames_counter
		AND		#3
		BNE		loc_18A438
		DEC		byte_674
loc_18A438:
		LDA		_frames_counter
		AND		#$F
		CMP		#8
		LDA		_tmp15
		BCS		loc_18A448
		SEC
		SBC		#2
		JMP		loc_18A44B
loc_18A448:
		CLC
		ADC		#2
loc_18A44B:
		TAX
		LDA		_cur_screen_tile._Y
		ASL
		ASL
		SEC
		SBC		#$38
		BCS		loc_18A457
		ADC		#$F0
loc_18A457:
		LDY		_PPU_STATUS
loc_18A45A:
		STX		_PPU_SCROLL
		STA		_PPU_SCROLL
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_regs_reload:
		LDA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		LDA		_ppu_mask_shadow
		STA		_PPU_MASK
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_scrolls_reload:
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_18A478
		LDA		_scr_res_idx
		AND		#$FE
		BNE		loc_18A49E
loc_18A478:
		LDA		_scr_mode_idx
		BNE		loc_18A48B
		LDA		#0
		STA		byte_14
		LDA		#$1E
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		RTS
loc_18A48B:
		CMP		#3
		BEQ		loc_18A49E
		LDA		#2
		STA		byte_14
		LDA		#$DE
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		RTS
loc_18A49E:
		LDA		#0
		STA		_MMC5_IRQ_ENABLE
		RTS

; =============== S U B	R O U T	I N E =======================================
IRQ:
		PHA
		TXA
		PHA
		TYA
		PHA
		LDA		_MMC5_IRQ_ENABLE
		LDA		byte_14
		BEQ		loc_18A4BE
		CMP		#1
		BEQ		sub_18A4E4
		CMP		#2
		BEQ		sub_18A4FB
loc_18A4B8:
		PLA
		TAY
		PLA
		TAX
		PLA
		RTI
loc_18A4BE:
		JSR		_delay
		JSR		_delay
		LDA		_scroll_msg_shift_pos
		LDX		#0
		STA		_PPU_SCROLL
		STX		_PPU_SCROLL
		LDA		#1
		STA		byte_14
		LDA		#$26
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		JMP		loc_18A4B8

; =============== S U B	R O U T	I N E =======================================
; WTF?
_delay:
		JSR		locret_18A4E3
locret_18A4E3:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_18A4E4:
		JSR		_delay
		JSR		_ppu_scrolls_reload
		LDA		#2
		STA		byte_14
		LDA		#$DE
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		JMP		loc_18A4B8

; =============== S U B	R O U T	I N E =======================================
sub_18A4FB:
		LDA		#0
		STA		_MMC5_IRQ_ENABLE
		LDA		_APU_STATUS
		AND		#$10
		BNE		loc_18A50A
		NOP
		CMP		_ppu_ctrl_shadow
loc_18A50A:
		LDX		#$15
loc_18A50C:
		DEX
		NOP
		BNE		loc_18A50C
		LDA		_ppu_mask_shadow
		AND		#$F7
		STA		_PPU_MASK
		LDA		_wnd_ppu_data_backup_req
		BEQ		loc_18A51E
		JSR		_ppu_extnt_data_backup
loc_18A51E:
		JMP		loc_18A4B8

; =============== S U B	R O U T	I N E =======================================
_wait_for_nmi:
		LDA		#0
		STA		_nmi_flag
loc_18A525:
		LDA		_nmi_flag
		BEQ		loc_18A525
		RTS

; =============== S U B	R O U T	I N E =======================================
_wait_for_mmc5_irq:
		LDA		_MMC5_IRQ_ENABLE
		AND		#$40
		BEQ		_wait_for_mmc5_irq
		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref_3:
;		LDA		byte_13
;		BEQ		_unref_3
;		LDA		#0
;		STA		byte_13
;		RTS

; =============== S U B	R O U T	I N E =======================================
_get_input:
		LDA		_scr_mode_idx
		BNE		loc_18A54E
		LDA		_game_wnd_spr_mode_flag
		BMI		loc_18A54E
		LDA		_game_msg_wnd_active_flag
		BNE		loc_18A54E
		LDA		byte_674
		BNE		loc_18A557
loc_18A54E:
		LDX		_input_forbid_delay
		BEQ		loc_18A560
		DEX
		STX		_input_forbid_delay
loc_18A557:
		LDA		#0
		STA		_pad0_held
		STA		_pad0_autorep_press
		STA		_pad0_autorep_release
		RTS
loc_18A560:
		LDX		#1
		JSR		_read_pad
		STA		_tmp15
		JSR		_read_pad
		CMP		_tmp15
		BNE		loc_18A560
		STA		_pad1_prev_held
		LDX		#0
		LDA		_pad0_held
		STA		_pad0_prev_held
		JSR		_read_pad
		STA		_tmp15
loc_18A57B:
		JSR		_read_pad
		CMP		_tmp15
		BEQ		loc_18A587
		STA		_tmp15
		JMP		loc_18A57B
loc_18A587:
		TAX
		BPL		loc_18A58E
		LDX		#8
		BNE		loc_18A590
loc_18A58E:
		LDX		#4
loc_18A590:
		STX		_pad0_autorep_delay
		CMP		#0
		BEQ		loc_18A5C2
		CMP		_pad0_held
		BEQ		loc_18A5AE
		EOR		_pad0_held
		AND		_tmp15
		STA		_pad0_autorep_press
		STA		_pad0_autorep_release
		CMP		#0
		BEQ		loc_18A5C8
		LDA		#8
		STA		_pad0_autorep_timer
		JMP		loc_18A5C8
loc_18A5AE:
		LDA		#0
		STA		_pad0_autorep_press
		DEC		_pad0_autorep_timer
		BNE		loc_18A5BD
		LDA		_pad0_autorep_delay
		STA		_pad0_autorep_timer
		LDA		_tmp15
loc_18A5BD:
		STA		_pad0_autorep_release
		JMP		loc_18A5C8
loc_18A5C2:
		LDA		#0
		STA		_pad0_autorep_press
		STA		_pad0_autorep_release
loc_18A5C8:
		LDA		_tmp15
		STA		_pad0_held
		RTS

; =============== S U B	R O U T	I N E =======================================
_read_pad:
		LDY		#8
		LDA		#1
		STA		_PAD0_REG
		LDA		#0
		STA		_PAD0_REG
loc_18A5D9:
		PHA
		LDA		_PAD0_REG,X
		STA		_pad_tmp
		LSR
		ORA		_pad_tmp
		LSR
		PLA
		ROL
		DEY
		BNE		loc_18A5D9
		RTS

; =============== S U B	R O U T	I N E =======================================
_spr_lib_prg2_insert:
		LDA		_prg_prgA000_shadow
		PHA
		LDA		#$F2
		STA		_MMC5_PRG_PAGES+2
		LDX		_spr_insert_args._idx
		LDA		_spr_lib_lo,X
		STA		_ptr0
		LDA		_spr_lib_hi,X
		STA		_ptr0+1
		DEC		_spr_insert_args._pos._Y
		LDX		_spr_buf_pos
		LDY		#0
loc_18A603:
		LDA		(_ptr0),Y
		CMP		#$80
		BEQ		loc_18A643
		LDA		_spr_insert_args._attr
		ASL
		LDA		(_ptr0),Y
		INY
		BCC		loc_18A615
		EOR		#$FF
		SBC		#6
loc_18A615:
		CLC
		ADC		_spr_insert_args._pos._Y
		STA		_spr_buf,X
		INX
		LDA		(_ptr0),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_ptr0),Y
		INY
		ORA		_spr_insert_args._attr
		STA		_spr_buf,X
		INX
		LDA		_spr_insert_args._attr
		ASL
		ASL
		LDA		(_ptr0),Y
		INY
		BCC		loc_18A639
		EOR		#$FF
		SBC		#6
loc_18A639:
		CLC
		ADC		_spr_insert_args._pos._X
		STA		_spr_buf,X
		INX
		JMP		loc_18A603
loc_18A643:
		STX		_spr_buf_pos
		PLA
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		RTS

; =============== S U B	R O U T	I N E =======================================
_spr_lib_prg8_insert:
		LDA		_prg_prg8000_shadow
		PHA
		LDA		#$F8
		STA		_MMC5_PRG_PAGES+1
		LDX		_spr_insert_args._idx
		LDA		_spr_lib_lo,X
		STA		_ptr0
		LDA		_spr_lib_hi,X
		STA		_ptr0+1
		DEC		_spr_insert_args._pos._Y
		LDX		_spr_buf_pos
		LDY		#0
loc_18A666:
		LDA		(_ptr0),Y
		CMP		#$80
		BEQ		loc_18A6A4
		LDA		(_ptr0),Y
		INY
		CLC
		ADC		_spr_insert_args._pos._Y
		STA		_spr_buf,X
		INX
		LDA		(_ptr0),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_ptr0),Y
		INY
		STA		_spr_buf,X
		INX
		LDA		(_ptr0),Y
		INY
		CLC
		ADC		_spr_insert_args._pos._X
		CMP		#$38
		BCC		loc_18A692
		CMP		#$F8
		BCC		loc_18A69D
loc_18A692:
		LDA		#$F4
		DEX
		DEX
		DEX
		STA		_spr_buf,X
		JMP		loc_18A666
loc_18A69D:
		STA		_spr_buf,X
		INX
		JMP		loc_18A666
loc_18A6A4:
		STX		_spr_buf_pos
		PLA
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_dequeue:
		LDA		_scr_mode_idx
		BNE		loc_18A6C1
		LDA		_game_wnd_spr_mode_flag
		BMI		loc_18A6C1
		LDA		_game_msg_wnd_active_flag
		BNE		loc_18A6C1
		LDA		byte_674
		BEQ		loc_18A6C1
		RTS
loc_18A6C1:
		LDX		_ppu_queue_pos_tail
		LDA		#2
		STA		_tmp15
loc_18A6C7:
		CPX		_ppu_queue_pos_head
		BEQ		loc_18A6FB
		DEC		_tmp15
		BMI		loc_18A6FB
		LDA		_ppu_queue_buf,X
		INX
		LDY		_ppu_queue_buf,X
		INX
		STY		_PPU_ADDR
		STA		_PPU_ADDR
		LDA		_ppu_queue_buf,X
		BMI		loc_18A6E8
		JSR		_ppu_set_hor_increment
		JMP		loc_18A6ED
loc_18A6E8:
		JSR		_ppu_set_vert_increment
		AND		#$7F
loc_18A6ED:
		TAY
		INX
loc_18A6EF:
		LDA		_ppu_queue_buf,X
		INX
		STA		_PPU_DATA
		DEY
		BNE		loc_18A6EF
		BEQ		loc_18A6C7
loc_18A6FB:
		STX		_ppu_queue_pos_tail
		JMP		_ppu_reset

; =============== S U B	R O U T	I N E =======================================
_extnt_dequeue:
		LDA		_scr_mode_idx
		BNE		loc_18A714
		LDA		_game_wnd_spr_mode_flag
		BMI		loc_18A714
		LDA		_game_msg_wnd_active_flag
		BNE		loc_18A714
		LDA		byte_674
		BEQ		loc_18A714
		RTS
loc_18A714:
		LDX		_extnt_queue_pos_tail
		LDA		#2
		STA		_tmp17
loc_18A71A:
		CPX		_extnt_queue_pos_head
		BEQ		loc_18A74F
		DEC		_tmp17
		BMI		loc_18A74F
		LDA		_extnt_queue_buf,X
		INX
		LDY		_extnt_queue_buf,X
		INX
		STA		_tmp15
		STY		_tmp15+1
		LDY		#0
		LDA		_extnt_queue_buf,X
		BPL		loc_18A739
		AND		#$7F
		LDY		#$FF
loc_18A739:
		INX
		STY		_word15_v_h_selector
		STA		_tmp18
		LDY		#0
loc_18A740:
		LDA		_extnt_queue_buf,X
		INX
		STA		(_tmp15),Y
		JSR		_inc_word15_v_h
		DEC		_tmp18
		BNE		loc_18A740
		BEQ		loc_18A71A
loc_18A74F:
		STX		_extnt_queue_pos_tail
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_queues_reset:
		LDA		#0
		STA		_ppu_queue_pos_tail
		STA		_ppu_queue_pos_head
		STA		_extnt_queue_pos_tail
		STA		_extnt_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_spr_finish:
		LDX		_spr_buf_pos
		BNE		loc_18A763

; =============== S U B	R O U T	I N E =======================================
_spr_clear:
		LDX		#0
loc_18A763:
		LDA		#$F4
loc_18A765:
		STA		_spr_buf,X
		INX
		INX
		INX
		INX
		BNE		loc_18A765
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_enable_restore:
		LDA		_sram_state_flag
		BNE		_sram_on
		BEQ		_sram_off

; =============== S U B	R O U T	I N E =======================================
_sram_on:
		PHA
		LDA		#2
		STA		_MMC5_WRAM0_ENABLE
		LDA		#1
		STA		_MMC5_WRAM1_ENABLE
		LDA		#$FF
		STA		_sram_state_flag
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_off:
		PHA
		LDA		#1
		STA		_MMC5_WRAM0_ENABLE
		LDA		#2
		STA		_MMC5_WRAM1_ENABLE
		LDA		#0
		STA		_sram_state_flag
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_farcall_argsAXY_safe:
		DEC		_mmc5_operation_in_progress_flag
		STA		_tmp3
		STX		_tmp4
		STY		_tmp5
		PLA
		STA		_ptr0
		CLC
		ADC		#4
		STA		_tmp2
		PLA
		STA		_ptr0+1
		ADC		#0
		PHA
		LDA		_tmp2
		PHA
		LDA		_prg_prg8000_shadow
		PHA
		LDA		_prg_prgA000_shadow
		PHA
		LDY		#1
		LDA		(_ptr0),Y
		INY
		PHA
		LDA		(_ptr0),Y
		INY
		PHA
		LDA		(_ptr0),Y
		INY
		TAX
		LDA		(_ptr0),Y
		STA		_ptr0+1
		STX		_ptr0
		PLA
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		PLA
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		_tmp3
		LDX		_tmp4
		LDY		_tmp5
		INC		_mmc5_operation_in_progress_flag
		JSR		_jmp_ptr0
		DEC		_mmc5_operation_in_progress_flag
		STA		_tmp3
		STX		_tmp4
		STY		_tmp5
		PLA
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		PLA
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		_tmp3
		LDX		_tmp4
		LDY		_tmp5
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_jmp_ptr0:
		JMP		(_ptr0)

; =============== S U B	R O U T	I N E =======================================
_farcall_argsXY_safe:
		STX		word_EE
		STY		word_EE+1
		DEC		_mmc5_operation_in_progress_flag
		PLA
		STA		_ptrDC
		CLC
		ADC		#4
		TAX
		PLA
		STA		_ptrDC+1
		ADC		#0
		PHA
		TXA
		PHA
		LDA		_prg_prg8000_shadow
		PHA
		LDA		_prg_prgA000_shadow
		PHA
		CMP		#$FB
		LDY		#1
		LDA		(_ptrDC),Y
		INY
		PHA
		LDA		(_ptrDC),Y
		INY
		PHA
		LDA		(_ptrDC),Y
		INY
		TAX
		LDA		(_ptrDC),Y
		STA		_ptrDC+1
		STX		_ptrDC
		PLA
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		PLA
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDX		word_EE
		LDY		word_EE+1
		JSR		_jmp_ptrDC
		DEC		_mmc5_operation_in_progress_flag
		PLA
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		PLA
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_jmp_ptrDC:
		INC		_mmc5_operation_in_progress_flag
		JMP		(_ptrDC)

; =============== S U B	R O U T	I N E =======================================
_inc_word15_v_h:
		LDA		_word15_v_h_selector
		BNE		loc_18A866
		INC		_tmp15
		BNE		locret_18A865
		INC		_tmp15+1
locret_18A865:
		RTS
loc_18A866:
		LDA		_tmp15
		CLC
		ADC		#$20
		STA		_tmp15
		LDA		_tmp15+1
		ADC		#0
		STA		_tmp15+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_ppu_send:
		JSR		_ppu_set_hor_increment
		LSR
		BCC		loc_18A881
		PHA
		LDY		#$10
		JSR		_pal_ppu_buf_send
		PLA
loc_18A881:
		LSR
		BCC		locret_18A889
		LDY		#0
		JSR		_pal_ppu_buf_send
locret_18A889:
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_ppu_buf_send:
		LDX		#$3F
		STX		_PPU_ADDR
		STY		_PPU_ADDR
		LDX		#$10
loc_18A894:
		LDA		_pal_bg_buf,Y
		STA		_PPU_DATA
		INY
		DEX
		BNE		loc_18A894
		LDA		#$3F
		STA		_PPU_ADDR
		LDA		#0
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_load_lib:
		AND		#$FF
		BMI		loc_18A8CA
		LDX		#0
		STX		_ptr0+1
		ASL
		ASL
		ASL
		ASL
		ROL		_ptr0+1
		CLC
		ADC		#<[_pal_bg_lib]
		STA		_ptr0
		LDA		#>[_pal_bg_lib]
		ADC		_ptr0+1
		STA		_ptr0+1
		BNE		loc_18A8DD
loc_18A8CA:
		LDX		#$10
		AND		#$7F
		ASL
		ASL
		ASL
		ASL
		CLC
		ADC		#<[_pal_spr_lib]
		STA		_ptr0
		LDA		#>[_pal_spr_lib]
		ADC		#0
		STA		_ptr0+1
loc_18A8DD:
		LDA		#$10
		STA		_tmp2
		LDY		#0
loc_18A8E3:
		LDA		(_ptr0),Y
		INY
		STA		_pal_bg_buf,X
		INX
		DEC		_tmp2
		BNE		loc_18A8E3
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_fade_in:
		LDA		_game_core_state
		PHA
		LDA		#1
		STA		_game_core_state
		LDA		_screen_pal_cur_bg_idx
		JSR		_pal_load_lib
		LDA		_screen_pal_cur_spr_idx
		ORA		#$80
		JSR		_pal_load_lib
		JSR		_pal_cut_off_half
		LDA		#4
		STA		_ptr0
loc_18A90D:
		LDA		#5
		STA		_ptr0+1
		LDA		#3
		STA		_ppu_pal_dequeue_req
loc_18A916:
		JSR		_wait_for_nmi
		DEC		_ptr0+1
		BNE		loc_18A916
		JSR		_pal_buf_fade_in
		DEC		_ptr0
		BNE		loc_18A90D
		PLA
		STA		_game_core_state
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_cut_off_half:
		LDX		#$1F
loc_18A92B:
		LDA		_pal_bg_buf,X
		AND		#$F
		STA		_pal_bg_buf,X
		DEX
		BPL		loc_18A92B
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_buf_fade_in:
		LDA		#0
		STA		_tmp3
		LDA		_screen_pal_cur_bg_idx
		ASL
		ASL
		ASL
		ASL
		ROL		_tmp3
		CLC
		ADC		#<[_pal_bg_lib]
		STA		_tmp2
		LDA		#>[_pal_bg_lib]
		ADC		_tmp3
		STA		_tmp3
		LDA		#<[_pal_bg_buf]
		STA		_tmp4
		LDA		#>[_pal_bg_buf]
		STA		_tmp5
		JSR		__pal_buf_fade_in
		LDA		_screen_pal_cur_spr_idx
		ASL
		ASL
		ASL
		ASL
		CLC
		ADC		#<[_pal_spr_lib]
		STA		_tmp2
		LDA		#>[_pal_spr_lib]
		ADC		#0
		STA		_tmp3
		LDA		#<[_pal_spr_buf]
		STA		_tmp4
		LDA		#>[_pal_spr_buf]
		STA		_tmp5
		JMP		__pal_buf_fade_in

; =============== S U B	R O U T	I N E =======================================
; !FALLTHROUGH!
__pal_buf_fade_in:
		LDX		#$10
		LDY		#0
loc_18A97B:
		LDA		(_tmp4),Y
		CMP		(_tmp2),Y
		BEQ		loc_18A986
		CLC
		ADC		#$10
		STA		(_tmp4),Y
loc_18A986:
		INY
		DEX
		BNE		loc_18A97B
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_fade_out:
		LDA		_game_core_state
		PHA
		LDA		#1
		STA		_game_core_state
		JSR		_pal_buf_fade_out
		LDA		#4
		STA		_ptr0
loc_18A99B:
		LDA		#5
		STA		_ptr0+1
		LDA		#3
		STA		_ppu_pal_dequeue_req
loc_18A9A4:
		JSR		_wait_for_nmi
		DEC		_ptr0+1
		BNE		loc_18A9A4
		JSR		_pal_buf_fade_out
		DEC		_ptr0
		BNE		loc_18A99B
		PLA
		STA		_game_core_state
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_buf_fade_out:
		LDX		#$1F
loc_18A9B9:
		LDA		_pal_bg_buf,X
		SEC
		SBC		#$10
		BCC		loc_18A9C6
		STA		_pal_bg_buf,X
		BCS		loc_18A9CB
loc_18A9C6:
		LDA		#$F
		STA		_pal_bg_buf,X
loc_18A9CB:
		DEX
		BPL		loc_18A9B9
		RTS

; =============== S U B	R O U T	I N E =======================================
_render_off:
		PHA
		LDA		_ppu_mask_shadow
		AND		#$E1
		STA		_ppu_mask_shadow
		JSR		_wait_for_nmi
		LDA		_ppu_mask_shadow
		STA		_PPU_MASK
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_render_on:
		PHA
		LDA		_ppu_mask_shadow
		ORA		#$1E
		STA		_ppu_mask_shadow
		JSR		_wait_for_nmi
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_set_hor_increment:
		PHA
		LDA		_ppu_ctrl_shadow
		AND		#$FB
		STA		_PPU_CTRL
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_set_vert_increment:
		PHA
		LDA		_ppu_ctrl_shadow
		ORA		#4
		STA		_PPU_CTRL
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_buf_init:
		JSR		_sram_on
		LDX		#0
		LDA		#<[_wram_map_buf]
		STA		_ptr0
		LDA		#>[_wram_map_buf]
		STA		_ptr0+1
		LDA		#$90
		LDX		#$16
		STA		_tmp2
		STX		_tmp3
		LDA		#$90
		LDY		#0
loc_18AA19:
		STA		(_ptr0),Y
		INY
		BNE		loc_18AA20
		INC		_ptr0+1
loc_18AA20:
		DEC		_tmp2
		BNE		loc_18AA19
		DEC		_tmp3
		BPL		loc_18AA19
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_ppu_reset:
		LDA		_PPU_STATUS
		LDA		#0
		STA		_PPU_SCROLL
		STA		_PPU_SCROLL
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_nt_data_backup:
		JSR		_sram_on
		LDA		_wnd_line_tmp_width
		STA		byte_9F
		LDA		_wnd_line_tmp_height
		STA		byte_A0
		LDA		_wnd_cur_line_num
		ASL
		ASL
		ASL
		ASL
		TAX
		LDY		_wnd_lvl_idx
		LDA		_wnd_type,Y
		BNE		loc_18AAB5
		JSR		_ppu_set_hor_increment
		LDY		_wnd_line_tmp_ppu_ofs
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_18AA60
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$24
		BNE		loc_18AA64
loc_18AA60:
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$20
loc_18AA64:
		STA		_PPU_ADDR
		STY		_PPU_ADDR
		LDA		_PPU_DATA
loc_18AA6D:
		LDA		_wnd_lvl_idx
		BEQ		loc_18AA7A
		LDA		_PPU_DATA
		STA		_nt_backup_buf1,X
		JMP		loc_18AA80
loc_18AA7A:
		LDA		_PPU_DATA
		STA		_nt_backup_buf0,X
loc_18AA80:
		INX
		DEC		byte_9F
		BNE		loc_18AA6D
		LDA		byte_A0
		BEQ		loc_18AAAF
		STA		byte_9F
		LDA		#0
		STA		byte_A0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$E0
		TAY
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_18AA9F
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$24
		BNE		loc_18AAA3
loc_18AA9F:
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$20
loc_18AAA3:
		STA		_PPU_ADDR
		STY		_PPU_ADDR
		LDA		_PPU_DATA
		JMP		loc_18AA6D
loc_18AAAF:
		JSR		_sram_off
		JMP		_ppu_scrolls_reload
loc_18AAB5:
		JSR		_ppu_set_vert_increment
		LDY		_wnd_line_tmp_ppu_ofs
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_18AAC5
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$24
		BNE		loc_18AAC9
loc_18AAC5:
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$20
loc_18AAC9:
		STA		_PPU_ADDR
		STY		_PPU_ADDR
		LDA		_PPU_DATA
loc_18AAD2:
		LDA		_wnd_lvl_idx
		BEQ		loc_18AADF
		LDA		_PPU_DATA
		STA		_nt_backup_buf1,X
		JMP		loc_18AAE5
loc_18AADF:
		LDA		_PPU_DATA
		STA		_nt_backup_buf0,X
loc_18AAE5:
		INX
		DEC		byte_9F
		BNE		loc_18AAD2
		LDA		byte_A0
		BEQ		loc_18AAAF
		STA		byte_9F
		LDA		#0
		STA		byte_A0
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_18AB01
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		LDY		#$24
		BNE		loc_18AB07
loc_18AB01:
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		LDY		#$20
loc_18AB07:
		STY		_PPU_ADDR
		STA		_PPU_ADDR
		LDA		_PPU_DATA
		JMP		loc_18AAD2

; =============== S U B	R O U T	I N E =======================================
_ppu_extnt_data_backup:
		JSR		_sram_on
		LDA		#2
		STA		_MMC5_CHR_MODE
		LDA		_wnd_line_tmp_width
		STA		byte_9F
		LDA		_wnd_line_tmp_height
		STA		byte_A0
		LDA		_wnd_line_tmp_ppu_ofs
		STA		word_9B
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#>[_MMC5_EXRAM]
		STA		word_9B+1
		LDA		_wnd_cur_line_num
		ASL
		ASL
		ASL
		ASL
		TAX
		LDY		_wnd_lvl_idx
		LDA		_wnd_type,Y
		BNE		loc_18AB79
		LDY		#0
loc_18AB3D:
		LDA		_wnd_lvl_idx
		BEQ		loc_18AB49
		LDA		(word_9B),Y
		STA		_extnt_backup1,X
		JMP		loc_18AB4E
loc_18AB49:
		LDA		(word_9B),Y
		STA		_extnt_backup0,X
loc_18AB4E:
		INX
		INC		word_9B
		BNE		loc_18AB55
		INC		word_9B+1
loc_18AB55:
		DEC		byte_9F
		BNE		loc_18AB3D
		LDA		byte_A0
		BEQ		loc_18AB71
		STA		byte_9F
		LDA		#0
		STA		byte_A0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$E0
		STA		word_9B
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#>[_MMC5_EXRAM]
		STA		word_9B+1
		BNE		loc_18AB3D
loc_18AB71:
		LDA		#1
		STA		_MMC5_CHR_MODE
		JMP		_sram_off
loc_18AB79:
		LDY		#0
loc_18AB7B:
		LDA		_wnd_lvl_idx
		BEQ		loc_18AB87
		LDA		(word_9B),Y
		STA		_extnt_backup1,X
		JMP		loc_18AB8C
loc_18AB87:
		LDA		(word_9B),Y
		STA		_extnt_backup0,X
loc_18AB8C:
		INX
		LDA		word_9B
		CLC
		ADC		#$20
		STA		word_9B
		BCC		loc_18AB98
		INC		word_9B+1
loc_18AB98:
		DEC		byte_9F
		BNE		loc_18AB7B
		LDA		byte_A0
		BEQ		loc_18AB71
		STA		byte_9F
		LDA		#0
		STA		byte_A0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		STA		word_9B
		LDA		#>[_MMC5_EXRAM]
		STA		word_9B+1
		JMP		loc_18AB7B

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_pos_list_ilit:
		LDX		#$B
loc_18ABB5:
		LDA		_gui_ctrl_menu_pos_data,X
		STA		_gui_ctrl_menu_pos_list,X
		DEX
		BPL		loc_18ABB5
		RTS
_gui_ctrl_menu_pos_data:
		.BYTE	$02,$07
		.BYTE	$04,$07
		.BYTE	$06,$07
		.BYTE	$08,$07
		.BYTE	$0A,$07
		.BYTE	$0C,$07

; =============== S U B	R O U T	I N E =======================================
_game_msg_chr_common_load:
		LDA		#$34
		STA		_mmc5_chr_banks_shadow
		STA		_MMC5_CHR_BANKSA
		LDA		#$3A
		STA		_mmc5_chr_banks_shadow+2
		STA		_MMC5_CHR_BANKSA+2
		LDA		#$37
		STA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
		RTS

; =============== S U B	R O U T	I N E =======================================
_thread1_init:
		LDA		#<[_stack_upper+$49]
		STA		_thread_stack+1
		LDA		#$FB
		STA		_thread_prgA000+1
		LDA		#>[_THREAD1_MAIN]
		STA		_stack_upper+$4F
		LDA		#0
		STA		_thread_prg8000+1
		STA		_stack_upper+$4E
		STA		_stack_upper+$4D
		STA		_game_core_state
		LDA		#$80
		STA		_ppu_ctrl_shadow
		LDA		#$34
		STA		_MMC5_CHR_BANKSA
		STA		_mmc5_chr_banks_shadow
		LDA		#$35
		STA		_MMC5_CHR_BANKSA+1
		STA		_mmc5_chr_banks_shadow+1
		LDA		#$3A
		STA		_MMC5_CHR_BANKSA+2
		STA		_mmc5_chr_banks_shadow+2
		LDA		#$37
		STA		_MMC5_CHR_BANKSA+3
		STA		_mmc5_chr_banks_shadow+3
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_music_off:
		LDA		_APU_STATUS
		CMP		_APU_STATUS
		BNE		_apu_music_off
		AND		#$F
		STA		_APU_STATUS
		LDA		_apu_idx
		ORA		#$80
		STA		_apu_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_music_on:
		LDA		_apu_idx
		AND		#$7F
		STA		_apu_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_res_unpack:
		JSR		_apu_music_off
		LDA		_PPU_STATUS
		STX		_ptr0
		STY		_ptr0+1
		LDY		#0
		LDA		(_ptr0),Y
		STA		_tmp2
		INY
		LDA		(_ptr0),Y
		STA		_tmp3
		INY
		LDA		(_ptr0),Y
		STA		byte_5AE
loc_18AC59:
		INY
		LDA		(_ptr0),Y
		STA		_tmp6
		INY
		LDA		(_ptr0),Y
		STA		_tmp6+1
		TYA
		PHA
		JSR		_tlm_unpack
		PLA
		TAY
		DEC		byte_5AE
		BNE		loc_18AC59
		JMP		_apu_music_on

; =============== S U B	R O U T	I N E =======================================
_tlm_unpack:
		LDA		_tmp2
		STA		_tmp4
		LDA		_tmp3
		STA		_tmp5
loc_18AC7A:
		LDY		#0
		LDA		(_tmp6),Y
		CMP		#$FF
		BNE		loc_18AC83
		RTS
loc_18AC83:
		AND		#$E0
		CMP		#$E0
		BNE		loc_18ACA9
		LDA		(_tmp6),Y
		ASL
		ASL
		ASL
		AND		#$E0
		PHA
		LDA		(_tmp6),Y
		INY
		AND		#3
		STA		word_5A9+1
		LDA		(_tmp6),Y
		INY
		STA		word_5A9
		INC		word_5A9
		BNE		loc_18ACA7
		INC		word_5A9+1
loc_18ACA7:
		BNE		loc_18ACBA
loc_18ACA9:
		PHA
		LDA		(_tmp6),Y
		INY
		AND		#$1F
		CLC
		ADC		#1
		STA		word_5A9
		LDA		#0
		STA		word_5A9+1
loc_18ACBA:
		PLA
		BPL		loc_18ACC0
		JMP		loc_18AD56
loc_18ACC0:
		LDX		_tmp3
		STX		_PPU_ADDR
		LDX		_tmp2
		STX		_PPU_ADDR
		LDX		word_5A9+1
		STX		byte_5AD
		LDX		word_5A9
		BNE		loc_18ACD8
		DEC		byte_5AD
loc_18ACD8:
		CMP		#$20
		BEQ		loc_18AD12
		CMP		#$40
		BEQ		loc_18AD23
		CMP		#$60
		BEQ		loc_18AD42
loc_18ACE4:
		LDA		(_tmp6),Y
		INY
		BNE		loc_18ACEB
		INC		_tmp6+1
loc_18ACEB:
		STA		_PPU_DATA
		DEX
		BNE		loc_18ACE4
		DEC		byte_5AD
		BPL		loc_18ACE4
loc_18ACF6:
		TYA
		CLC
		ADC		_tmp6
		STA		_tmp6
		BCC		loc_18AD00
		INC		_tmp6+1
loc_18AD00:
		LDA		_tmp2
		CLC
		ADC		word_5A9
		STA		_tmp2
		LDA		_tmp3
		ADC		word_5A9+1
		STA		_tmp3
		JMP		loc_18AC7A
loc_18AD12:
		LDA		(_tmp6),Y
		INY
loc_18AD15:
		STA		_PPU_DATA
		DEX
		BNE		loc_18AD15
		DEC		byte_5AD
		BPL		loc_18AD15
		JMP		loc_18ACF6
loc_18AD23:
		LDA		(_tmp6),Y
		INY
		STA		_PPU_DATA
		LDA		(_tmp6),Y
		DEY
		STA		_PPU_DATA
		DEX
		BNE		loc_18AD23
		DEC		byte_5AD
		BPL		loc_18AD23
		INY
		INY
		ASL		word_5A9
		ROL		word_5A9+1
		JMP		loc_18ACF6
loc_18AD42:
		LDA		(_tmp6),Y
		INY
loc_18AD45:
		STA		_PPU_DATA
		CLC
		ADC		#1
		DEX
		BNE		loc_18AD45
		DEC		byte_5AD
		BPL		loc_18AD45
		JMP		loc_18ACF6
loc_18AD56:
		PHA
		LDA		(_tmp6),Y
		INY
		STA		byte_5AC
		LDA		(_tmp6),Y
		INY
		CLC
		ADC		_tmp4
		STA		byte_5AB
		LDA		_tmp5
		ADC		byte_5AC
		STA		byte_5AC
		TYA
		CLC
		ADC		_tmp6
		STA		_tmp6
		BCC		loc_18AD78
		INC		_tmp6+1
loc_18AD78:
		LDA		word_5A9
		BNE		loc_18AD80
		DEC		word_5A9+1
loc_18AD80:
		PLA
		CMP		#$80
		BEQ		loc_18AD8D
		CMP		#$A0
		BEQ		loc_18ADA1
		CMP		#$C0
		BEQ		loc_18ADB8
loc_18AD8D:
		JSR		sub_18ADC9
		JSR		_tlm_get_reverse_ppu_data
		JSR		_tlm_send_ppu_data
		INC		byte_5AC
		DEC		word_5A9+1
		BPL		loc_18AD8D
		JMP		loc_18AC7A
loc_18ADA1:
		JSR		sub_18ADC9
		JSR		_tlm_get_reverse_ppu_data
		JSR		_tlm_rotate_data
		JSR		_tlm_send_ppu_data
		INC		byte_5AC
		DEC		word_5A9+1
		BPL		loc_18ADA1
		JMP		loc_18AC7A
loc_18ADB8:
		JSR		sub_18ADC9
		JSR		_tlm_get_ppu_data
		JSR		_tlm_send_ppu_data
		DEC		word_5A9+1
		BPL		loc_18ADB8
		JMP		loc_18AC7A

; =============== S U B	R O U T	I N E =======================================
sub_18ADC9:
		LDX		word_5A9
		LDA		word_5A9+1
		BEQ		loc_18ADD3
		LDX		#0
loc_18ADD3:
		STX		byte_5AF
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_get_reverse_ppu_data:
		LDA		byte_5AC
		STA		_PPU_ADDR
		LDA		byte_5AB
		STA		_PPU_ADDR
		LDA		_PPU_DATA
loc_18ADE6:
		LDA		_PPU_DATA
		STA		_ppu_queue_buf,X
		DEX
		BNE		loc_18ADE6
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_send_ppu_data:
		LDX		byte_5AF
		LDA		_tmp3
		STA		_PPU_ADDR
		LDA		_tmp2
		STA		_PPU_ADDR
loc_18ADFD:
		LDA		_ppu_queue_buf,X
		STA		_PPU_DATA
		DEX
		BNE		loc_18ADFD
		DEC		byte_5AF
		LDA		byte_5AF
		SEC
		ADC		_tmp2
		STA		_tmp2
		BCC		locret_18AE15
		INC		_tmp3
locret_18AE15:
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_rotate_data:
		LDX		byte_5AF
loc_18AE19:
		LDA		_ppu_queue_buf,X
		STA		byte_5AD
		LDY		#8
loc_18AE21:
		ASL		byte_5AD
		ROR
		DEY
		BNE		loc_18AE21
		STA		_ppu_queue_buf,X
		DEX
		BNE		loc_18AE19
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_get_ppu_data:
		STX		byte_5AD
		DEC		byte_5AD
		LDA		byte_5AB
		CLC
		SBC		byte_5AD
		STA		byte_5AB
		TAY
		LDA		byte_5AC
		SBC		#0
		STA		byte_5AC
		STA		_PPU_ADDR
		STY		_PPU_ADDR
		LDA		_PPU_DATA
		LDA		_PPU_DATA
		LDX		#0
loc_18AE56:
		INX
		LDA		_PPU_DATA
		STA		_ppu_queue_buf,X
		CPX		byte_5AF
		BNE		loc_18AE56
		RTS

; =============== S U B	R O U T	I N E =======================================
_goto_alert_spr_draw:
		LDA		_goto_alert_req
		BNE		loc_18AE69
		RTS
loc_18AE69:
		LDX		_spr_buf_pos
		LDA		#$27
		STA		_spr_buf,X
		STA		_spr_buf+4,X
		LDA		#$2F
		STA		_spr_buf+8,X
		STA		_spr_buf+$C,X
		LDA		#$80
		STA		_spr_buf+3,X
		STA		_spr_buf+$B,X
		LDA		#$88
		STA		_spr_buf+7,X
		STA		_spr_buf+$F,X
		LDA		#$1F
		STA		_spr_buf+1,X
		LDA		#$2F
		STA		_spr_buf+5,X
		LDA		#$3E
		STA		_spr_buf+9,X
		LDA		#$3F
		STA		_spr_buf+$D,X
		LDA		_goto_transition_counter
		BNE		loc_18AEA8
		LDA		#0
		BEQ		loc_18AEB4
loc_18AEA8:
		LDA		#2
		DEC		_goto_transition_counter
		BNE		loc_18AEB4
		LDA		#0
		STA		_goto_alert_req
loc_18AEB4:
		STA		_spr_buf+2,X
		STA		_spr_buf+6,X
		STA		_spr_buf+$A,X
		STA		_spr_buf+$E,X
		TXA
		CLC
		ADC		#$10
		STA		_spr_buf_pos
		RTS
_spr_lib_lo:
		.BYTE	<[byte_18B019]
		.BYTE	<[byte_18B02A]
		.BYTE	<[byte_18B03B]
		.BYTE	<[byte_18B04C]
		.BYTE	<[byte_18B05D]
		.BYTE	<[byte_18B06E]
		.BYTE	<[byte_18B077]
		.BYTE	<[byte_18B118]
		.BYTE	<[byte_18B129]
		.BYTE	<[byte_18B136]
		.BYTE	<[byte_18B143]
		.BYTE	<[byte_18B150]
		.BYTE	<[byte_3FD1E]
		.BYTE	<[byte_3FD3F]
		.BYTE	<[byte_3FD60]
		.BYTE	0
		.BYTE	<[byte_18B15D]
		.BYTE	<[byte_18B1A2]
		.BYTE	<[byte_18B1EB]
		.BYTE	<[byte_18B258]
		.BYTE	<[byte_18B2C5]
		.BYTE	<[byte_18B332]
		.BYTE	<[byte_18B3AF]
		.BYTE	<[byte_18B41C]
		.BYTE	<[byte_18B4A1]
		.BYTE	<[byte_18B50E]
		.BYTE	<[byte_18B57B]
		.BYTE	<[byte_18B5E8]
		.BYTE	<[byte_18B655]
		.BYTE	<[byte_18B6C2]
		.BYTE	<[byte_18B747]
		.BYTE	<[byte_18B7B4]
		.BYTE	<[byte_18B82D]
		.BYTE	<[byte_18B8A6]
		.BYTE	<[byte_18B913]
		.BYTE	<[byte_18B96C]
		.BYTE	<[byte_18B9F9]
		.BYTE	<[byte_18BA52]
		.BYTE	<[byte_18BA83]
		.BYTE	<[byte_18BADC]
		.BYTE	<[byte_18BB21]
		.BYTE	<[byte_D0000]
		.BYTE	<[byte_D0055]
		.BYTE	<[byte_D00BE]
		.BYTE	<[byte_D0143]
		.BYTE	<[byte_D01B0]
		.BYTE	<[byte_D021D]
		.BYTE	<[byte_D0296]
		.BYTE	<[byte_D031B]
		.BYTE	<[byte_D03A8]
		.BYTE	<[byte_D0415]
		.BYTE	<[byte_D0472]
		.BYTE	<[byte_D04DF]
		.BYTE	<[byte_D054C]
		.BYTE	<[byte_D05C5]
		.BYTE	<[byte_D061E]
		.BYTE	<[byte_D068B]
		.BYTE	<[byte_D06BC]
		.BYTE	<[byte_D06FD]
		.BYTE	<[byte_D073E]
		.BYTE	<[byte_D078F]
		.BYTE	<[byte_D07FC]
		.BYTE	<[byte_D0869]
		.BYTE	<[byte_D08D2]
		.BYTE	<[byte_D093B]
		.BYTE	<[byte_D09B0]
		.BYTE	<[byte_D0A19]
		.BYTE	<[byte_D0A72]
		.BYTE	<[byte_D0AB7]
		.BYTE	<[byte_D0B00]
		.BYTE	<[byte_D0B55]
		.BYTE	<[byte_D0B8E]
		.BYTE	<[byte_D0BC7]
		.BYTE	<[byte_D0C20]
		.BYTE	<[byte_D0C65]
		.BYTE	<[byte_D0CAE]
		.BYTE	<[byte_D0D03]
		.BYTE	<[byte_D0D4C]
		.BYTE	<[byte_D0D95]
		.BYTE	<[byte_D0DFE]
		.BYTE	<[byte_D0E6B]
		.BYTE	<[byte_D0EE4]
		.BYTE	<[byte_D0F61]
		.BYTE	<[byte_D0F9A]
		.BYTE	<[byte_D0FD7]
		.BYTE	<[byte_D1054]
		.BYTE	<[byte_D10BD]
		.BYTE	<[byte_D1126]
		.BYTE	<[byte_D1197]
		.BYTE	<[byte_D11F0]
		.BYTE	<[byte_D1249]
		.BYTE	<[byte_D12A2]
		.BYTE	<[byte_D12E7]
		.BYTE	<[byte_D1330]
		.BYTE	<[byte_D1385]
		.BYTE	<[byte_D13EE]
		.BYTE	<[byte_D1457]
		.BYTE	<[byte_D14B0]
		.BYTE	<[byte_D14F5]
		.BYTE	<[byte_D153E]
		.BYTE	<[byte_D1593]
		.BYTE	<[byte_D15FC]
		.BYTE	<[byte_D1669]
		.BYTE	<[byte_D16D6]
		.BYTE	<[byte_D1743]
		.BYTE	<[byte_D17B8]
		.BYTE	<[byte_D17E5]
		.BYTE	<[byte_D1812]
		.BYTE	<[byte_D1847]
		.BYTE	<[byte_D187C]
		.BYTE	<[byte_D18E9]
		.BYTE	<[byte_D1946]
		.BYTE	<[byte_D197B]
		.BYTE	<[byte_D19B0]
		.BYTE	<[byte_D19E1]
		.BYTE	<[byte_D1A12]
		.BYTE	<[byte_D1A43]
		.BYTE	<[byte_D1A74]
		.BYTE	<[byte_D1AA1]
		.BYTE	<[byte_D1ACE]
		.BYTE	<[byte_D1B1F]
		.BYTE	<[byte_D1B64]
		.BYTE	<[byte_D1BAD]
		.BYTE	<[byte_D1C02]
		.BYTE	<[byte_D1C02]
		.BYTE	<[byte_D1C02]
		.BYTE	<[byte_D1C53]
		.BYTE	<[byte_D1C98]
		.BYTE	<[byte_D1CE1]
		.BYTE	<[byte_D1D36]
		.BYTE	<[byte_D1D7B]
		.BYTE	<[byte_D1DB8]
		.BYTE	<[byte_D1DFD]
		.BYTE	<[byte_D1E46]
		.BYTE	<[byte_D1E8B]
		.BYTE	<[byte_D1EC8]
		.BYTE	<[byte_D1F0D]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_3FB61]
		.BYTE	<[byte_3FB82]
		.BYTE	<[byte_3FBA3]
		.BYTE	<[byte_3FBC4]
		.BYTE	<[byte_3FBE5]
		.BYTE	<[byte_3FC06]
		.BYTE	<[byte_3FC27]
		.BYTE	<[byte_3FC48]
		.BYTE	<[byte_3FC69]
		.BYTE	<[byte_3FC8E]
		.BYTE	<[byte_3FCB3]
		.BYTE	<[byte_3FCC4]
		.BYTE	<[byte_3FCD5]
		.BYTE	<[byte_3FCE6]
		.BYTE	<[byte_3FCEB]
		.BYTE	<[byte_3FCFC]
		.BYTE	<[byte_18BB6A]
		.BYTE	<[byte_3FD0D]
		.BYTE	<[byte_3FDA1]
		.BYTE	<[byte_3FDC2]
		.BYTE	<[byte_3FDC7]
		.BYTE	<[byte_3FDD0]
		.BYTE	<[byte_3FDD9]
		.BYTE	<[byte_3FDE2]
		.BYTE	<[byte_3FDE7]
_spr_lib_hi:
		.BYTE	>[byte_18B019]
		.BYTE	>[byte_18B02A]
		.BYTE	>[byte_18B03B]
		.BYTE	>[byte_18B04C]
		.BYTE	>[byte_18B05D]
		.BYTE	>[byte_18B06E]
		.BYTE	>[byte_18B077]
		.BYTE	>[byte_18B118]
		.BYTE	>[byte_18B129]
		.BYTE	>[byte_18B136]
		.BYTE	>[byte_18B143]
		.BYTE	>[byte_18B150]
		.BYTE	>[byte_3FD1E]
		.BYTE	>[byte_3FD3F]
		.BYTE	>[byte_3FD60]
		.BYTE	0
		.BYTE	>[byte_18B15D]
		.BYTE	>[byte_18B1A2]
		.BYTE	>[byte_18B1EB]
		.BYTE	>[byte_18B258]
		.BYTE	>[byte_18B2C5]
		.BYTE	>[byte_18B332]
		.BYTE	>[byte_18B3AF]
		.BYTE	>[byte_18B41C]
		.BYTE	>[byte_18B4A1]
		.BYTE	>[byte_18B50E]
		.BYTE	>[byte_18B57B]
		.BYTE	>[byte_18B5E8]
		.BYTE	>[byte_18B655]
		.BYTE	>[byte_18B6C2]
		.BYTE	>[byte_18B747]
		.BYTE	>[byte_18B7B4]
		.BYTE	>[byte_18B82D]
		.BYTE	>[byte_18B8A6]
		.BYTE	>[byte_18B913]
		.BYTE	>[byte_18B96C]
		.BYTE	>[byte_18B9F9]
		.BYTE	>[byte_18BA52]
		.BYTE	>[byte_18BA83]
		.BYTE	>[byte_18BADC]
		.BYTE	>[byte_18BB21]
		.BYTE	>[byte_D0000]
		.BYTE	>[byte_D0055]
		.BYTE	>[byte_D00BE]
		.BYTE	>[byte_D0143]
		.BYTE	>[byte_D01B0]
		.BYTE	>[byte_D021D]
		.BYTE	>[byte_D0296]
		.BYTE	>[byte_D031B]
		.BYTE	>[byte_D03A8]
		.BYTE	>[byte_D0415]
		.BYTE	>[byte_D0472]
		.BYTE	>[byte_D04DF]
		.BYTE	>[byte_D054C]
		.BYTE	>[byte_D05C5]
		.BYTE	>[byte_D061E]
		.BYTE	>[byte_D068B]
		.BYTE	>[byte_D06BC]
		.BYTE	>[byte_D06FD]
		.BYTE	>[byte_D073E]
		.BYTE	>[byte_D078F]
		.BYTE	>[byte_D07FC]
		.BYTE	>[byte_D0869]
		.BYTE	>[byte_D08D2]
		.BYTE	>[byte_D093B]
		.BYTE	>[byte_D09B0]
		.BYTE	>[byte_D0A19]
		.BYTE	>[byte_D0A72]
		.BYTE	>[byte_D0AB7]
		.BYTE	>[byte_D0B00]
		.BYTE	>[byte_D0B55]
		.BYTE	>[byte_D0B8E]
		.BYTE	>[byte_D0BC7]
		.BYTE	>[byte_D0C20]
		.BYTE	>[byte_D0C65]
		.BYTE	>[byte_D0CAE]
		.BYTE	>[byte_D0D03]
		.BYTE	>[byte_D0D4C]
		.BYTE	>[byte_D0D95]
		.BYTE	>[byte_D0DFE]
		.BYTE	>[byte_D0E6B]
		.BYTE	>[byte_D0EE4]
		.BYTE	>[byte_D0F61]
		.BYTE	>[byte_D0F9A]
		.BYTE	>[byte_D0FD7]
		.BYTE	>[byte_D1054]
		.BYTE	>[byte_D10BD]
		.BYTE	>[byte_D1126]
		.BYTE	>[byte_D1197]
		.BYTE	>[byte_D11F0]
		.BYTE	>[byte_D1249]
		.BYTE	>[byte_D12A2]
		.BYTE	>[byte_D12E7]
		.BYTE	>[byte_D1330]
		.BYTE	>[byte_D1385]
		.BYTE	>[byte_D13EE]
		.BYTE	>[byte_D1457]
		.BYTE	>[byte_D14B0]
		.BYTE	>[byte_D14F5]
		.BYTE	>[byte_D153E]
		.BYTE	>[byte_D1593]
		.BYTE	>[byte_D15FC]
		.BYTE	>[byte_D1669]
		.BYTE	>[byte_D16D6]
		.BYTE	>[byte_D1743]
		.BYTE	>[byte_D17B8]
		.BYTE	>[byte_D17E5]
		.BYTE	>[byte_D1812]
		.BYTE	>[byte_D1847]
		.BYTE	>[byte_D187C]
		.BYTE	>[byte_D18E9]
		.BYTE	>[byte_D1946]
		.BYTE	>[byte_D197B]
		.BYTE	>[byte_D19B0]
		.BYTE	>[byte_D19E1]
		.BYTE	>[byte_D1A12]
		.BYTE	>[byte_D1A43]
		.BYTE	>[byte_D1A74]
		.BYTE	>[byte_D1AA1]
		.BYTE	>[byte_D1ACE]
		.BYTE	>[byte_D1B1F]
		.BYTE	>[byte_D1B64]
		.BYTE	>[byte_D1BAD]
		.BYTE	>[byte_D1C02]
		.BYTE	>[byte_D1C02]
		.BYTE	>[byte_D1C02]
		.BYTE	>[byte_D1C53]
		.BYTE	>[byte_D1C98]
		.BYTE	>[byte_D1CE1]
		.BYTE	>[byte_D1D36]
		.BYTE	>[byte_D1D7B]
		.BYTE	>[byte_D1DB8]
		.BYTE	>[byte_D1DFD]
		.BYTE	>[byte_D1E46]
		.BYTE	>[byte_D1E8B]
		.BYTE	>[byte_D1EC8]
		.BYTE	>[byte_D1F0D]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_3FB61]
		.BYTE	>[byte_3FB82]
		.BYTE	>[byte_3FBA3]
		.BYTE	>[byte_3FBC4]
		.BYTE	>[byte_3FBE5]
		.BYTE	>[byte_3FC06]
		.BYTE	>[byte_3FC27]
		.BYTE	>[byte_3FC48]
		.BYTE	>[byte_3FC69]
		.BYTE	>[byte_3FC8E]
		.BYTE	>[byte_3FCB3]
		.BYTE	>[byte_3FCC4]
		.BYTE	>[byte_3FCD5]
		.BYTE	>[byte_3FCE6]
		.BYTE	>[byte_3FCEB]
		.BYTE	>[byte_3FCFC]
		.BYTE	>[byte_18BB6A]
		.BYTE	>[byte_3FD0D]
		.BYTE	>[byte_3FDA1]
		.BYTE	>[byte_3FDC2]
		.BYTE	>[byte_3FDC7]
		.BYTE	>[byte_3FDD0]
		.BYTE	>[byte_3FDD9]
		.BYTE	>[byte_3FDE2]
		.BYTE	>[byte_3FDE7]
byte_18B019:
		.BYTE	$00,$00,$01,$00
		.BYTE	$00,$01,$01,$08
		.BYTE	$08,$02,$01,$00
		.BYTE	$08,$03,$01,$08
		.BYTE	$80
byte_18B02A:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$04
		.BYTE	$03,$06,$00,$FD
		.BYTE	$03,$07,$00,$04
		.BYTE	$80
byte_18B03B:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$0C
		.BYTE	$0B,$06,$00,$FD
		.BYTE	$0B,$07,$00,$0C
		.BYTE	$80
byte_18B04C:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$14
		.BYTE	$13,$06,$00,$FD
		.BYTE	$13,$07,$00,$14
		.BYTE	$80
byte_18B05D:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$1C
		.BYTE	$1B,$06,$00,$FD
		.BYTE	$1B,$07,$00,$1C
		.BYTE	$80
byte_18B06E:
		.BYTE	$00,$E5,$00,$00
		.BYTE	$00,$E6,$00,$08
		.BYTE	$80
byte_18B077:
		.BYTE	$00,$90,$01,$08
		.BYTE	$00,$91,$01,$10
		.BYTE	$00,$92,$01,$18
		.BYTE	$00,$93,$01,$20
		.BYTE	$08,$94,$01,$00
		.BYTE	$08,$95,$01,$08
		.BYTE	$08,$96,$01,$10
		.BYTE	$08,$97,$01,$18
		.BYTE	$08,$98,$01,$20
		.BYTE	$08,$99,$01,$28
		.BYTE	$10,$9A,$01,$00
		.BYTE	$10,$9B,$01,$08
		.BYTE	$10,$9B,$01,$10
		.BYTE	$10,$9B,$01,$18
		.BYTE	$10,$9B,$01,$20
		.BYTE	$10,$9C,$01,$28
		.BYTE	$18,$9A,$01,$00
		.BYTE	$18,$9B,$01,$08
		.BYTE	$18,$9B,$01,$10
		.BYTE	$18,$9B,$01,$18
		.BYTE	$18,$9B,$01,$20
		.BYTE	$18,$9C,$01,$28
		.BYTE	$20,$9A,$01,$00
		.BYTE	$20,$9B,$01,$08
		.BYTE	$20,$9B,$01,$10
		.BYTE	$20,$9B,$01,$18
		.BYTE	$20,$9B,$01,$20
		.BYTE	$20,$9C,$01,$28
		.BYTE	$28,$9A,$01,$00
		.BYTE	$28,$9B,$01,$08
		.BYTE	$28,$9B,$01,$10
		.BYTE	$28,$9B,$01,$18
		.BYTE	$28,$9B,$01,$20
		.BYTE	$28,$9C,$01,$28
		.BYTE	$30,$9D,$01,$00
		.BYTE	$30,$A4,$01,$08
		.BYTE	$30,$A4,$01,$10
		.BYTE	$30,$A4,$01,$18
		.BYTE	$30,$A4,$01,$20
		.BYTE	$30,$A7,$01,$28
		.BYTE	$80
byte_18B118:
		.BYTE	$FE,$9E,$01,$00
		.BYTE	$FE,$9E,$41,$07
		.BYTE	$06,$9E,$81,$00
		.BYTE	$06,$9E,$C1,$07
		.BYTE	$80
byte_18B129:
		.BYTE	$00,$8B,$03,$00
		.BYTE	$00,$8C,$03,$08
		.BYTE	$08,$8D,$03,$04
		.BYTE	$80
byte_18B136:
		.BYTE	$00,$8D,$83,$04
		.BYTE	$08,$8B,$83,$00
		.BYTE	$08,$8C,$83,$08
		.BYTE	$80
byte_18B143:
		.BYTE	$00,$8F,$43,$00
		.BYTE	$08,$9F,$43,$00
		.BYTE	$04,$8E,$43,$08
		.BYTE	$80
byte_18B150:
		.BYTE	$04,$8E,$03,$00
		.BYTE	$00,$8F,$03,$08
		.BYTE	$08,$9F,$03,$08
		.BYTE	$80
byte_18B15D:
		.BYTE	$F8,$1B,$01,$08
		.BYTE	$F8,$1A,$01,$00
		.BYTE	$F8,$19,$01,$F8
		.BYTE	$F0,$18,$01,$08
		.BYTE	$F0,$17,$01,$00
		.BYTE	$F0,$16,$01,$F8
		.BYTE	$10,$24,$00,$06
		.BYTE	$10,$23,$00,$FE
		.BYTE	$10,$22,$00,$F6
		.BYTE	$08,$21,$00,$06
		.BYTE	$08,$20,$00,$FE
		.BYTE	$08,$1F,$00,$F6
		.BYTE	$00,$1E,$01,$06
		.BYTE	$00,$1D,$01,$FE
		.BYTE	$00,$1C,$01,$F6
		.BYTE	$18,$26,$02,$FE
		.BYTE	$18,$25,$02,$F6
		.BYTE	$80
byte_18B1A2:
		.BYTE	$E8,$00,$01,$FE
		.BYTE	$00,$0A,$01,$09
		.BYTE	$00,$09,$01,$01
		.BYTE	$00,$08,$01,$F9
		.BYTE	$00,$07,$01,$F1
		.BYTE	$F8,$06,$01,$09
		.BYTE	$F8,$05,$01,$01
		.BYTE	$F8,$04,$01,$F9
		.BYTE	$F0,$03,$01,$09
		.BYTE	$F0,$02,$01,$01
		.BYTE	$F0,$01,$01,$F9
		.BYTE	$10,$2A,$00,$00
		.BYTE	$10,$29,$00,$F8
		.BYTE	$08,$28,$00,$00
		.BYTE	$08,$27,$00,$F8
		.BYTE	$18,$2D,$02,$01
		.BYTE	$18,$2C,$02,$F9
		.BYTE	$18,$2B,$02,$F1
		.BYTE	$80
byte_18B1EB:
		.BYTE	$10,$96,$40,$08
		.BYTE	$10,$96,$00,$F0
		.BYTE	$08,$95,$40,$08
		.BYTE	$08,$95,$00,$F0
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$45,$01,$00
		.BYTE	$00,$44,$01,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$40,$00
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B258:
		.BYTE	$00,$6C,$00,$00
		.BYTE	$00,$6B,$00,$F8
		.BYTE	$10,$6F,$40,$08
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6D,$40,$08
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$40,$00
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B2C5:
		.BYTE	$00,$45,$01,$00
		.BYTE	$00,$44,$01,$F8
		.BYTE	$10,$6F,$40,$08
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6D,$40,$08
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$40,$00
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B332:
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$57,$40,$F0
		.BYTE	$08,$58,$40,$E8
		.BYTE	$10,$5C,$40,$E8
		.BYTE	$10,$5B,$40,$F0
		.BYTE	$08,$6E,$40,$00
		.BYTE	$08,$58,$00,$10
		.BYTE	$08,$57,$00,$08
		.BYTE	$10,$5C,$00,$10
		.BYTE	$10,$5B,$00,$08
		.BYTE	$00,$6C,$00,$00
		.BYTE	$00,$6B,$00,$F8
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B3AF:
		.BYTE	$F8,$68,$01,$00
		.BYTE	$F8,$67,$01,$F8
		.BYTE	$00,$45,$01,$00
		.BYTE	$00,$44,$01,$F8
		.BYTE	$10,$6F,$40,$08
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6D,$40,$08
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$40,$00
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B41C:
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$08,$48,$40,$00
		.BYTE	$08,$48,$00,$F8
		.BYTE	$00,$6C,$00,$00
		.BYTE	$00,$6B,$00,$F8
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$10,$49,$40,$10
		.BYTE	$10,$49,$00,$E8
		.BYTE	$10,$4A,$00,$F0
		.BYTE	$08,$46,$40,$10
		.BYTE	$00,$42,$41,$10
		.BYTE	$08,$47,$40,$08
		.BYTE	$08,$47,$00,$F0
		.BYTE	$08,$46,$00,$E8
		.BYTE	$00,$42,$01,$E8
		.BYTE	$10,$4A,$40,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$80
byte_18B4A1:
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$10,$4A,$40,$08
		.BYTE	$08,$9C,$00,$08
		.BYTE	$08,$9B,$00,$00
		.BYTE	$00,$6B,$00,$F8
		.BYTE	$00,$9A,$00,$08
		.BYTE	$00,$99,$00,$00
		.BYTE	$F8,$98,$01,$08
		.BYTE	$F8,$97,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B50E:
		.BYTE	$10,$4B,$42,$00
		.BYTE	$F8,$BF,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$08,$A2,$00,$08
		.BYTE	$08,$A1,$00,$00
		.BYTE	$00,$A0,$00,$08
		.BYTE	$00,$9F,$01,$00
		.BYTE	$F8,$9E,$01,$08
		.BYTE	$00,$44,$01,$F8
		.BYTE	$10,$4A,$40,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B57B:
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$F8,$67,$01,$F8
		.BYTE	$08,$A2,$00,$08
		.BYTE	$08,$A1,$00,$00
		.BYTE	$00,$A0,$00,$08
		.BYTE	$00,$9F,$01,$00
		.BYTE	$F8,$9E,$01,$08
		.BYTE	$F8,$9D,$01,$00
		.BYTE	$00,$44,$01,$F8
		.BYTE	$10,$4A,$40,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B5E8:
		.BYTE	$10,$6F,$40,$08
		.BYTE	$08,$6D,$40,$08
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$08,$6E,$40,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$45,$01,$00
		.BYTE	$00,$44,$01,$F8
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B655:
		.BYTE	$00,$6C,$00,$00
		.BYTE	$00,$6B,$00,$F8
		.BYTE	$F8,$68,$01,$00
		.BYTE	$F8,$67,$01,$F8
		.BYTE	$10,$6F,$40,$08
		.BYTE	$08,$6D,$40,$08
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$08,$6E,$40,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B6C2:
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$45,$01,$00
		.BYTE	$00,$44,$01,$F8
		.BYTE	$10,$49,$40,$10
		.BYTE	$10,$49,$00,$E8
		.BYTE	$10,$4A,$40,$08
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$10,$4A,$00,$F0
		.BYTE	$00,$42,$41,$10
		.BYTE	$08,$48,$40,$00
		.BYTE	$08,$47,$40,$08
		.BYTE	$08,$46,$40,$10
		.BYTE	$00,$6A,$41,$08
		.BYTE	$00,$42,$01,$E8
		.BYTE	$08,$48,$00,$F8
		.BYTE	$08,$47,$00,$F0
		.BYTE	$08,$46,$00,$E8
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B747:
		.BYTE	$F8,$40,$01,$00
		.BYTE	$F8,$3F,$01,$F8
		.BYTE	$00,$6C,$00,$00
		.BYTE	$00,$6B,$00,$F8
		.BYTE	$10,$6F,$40,$08
		.BYTE	$08,$6D,$40,$08
		.BYTE	$10,$6F,$00,$F0
		.BYTE	$08,$6E,$00,$F8
		.BYTE	$08,$6D,$00,$F0
		.BYTE	$08,$6E,$40,$00
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$00,$6A,$41,$08
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$10,$4B,$42,$00
		.BYTE	$00,$6A,$01,$F0
		.BYTE	$F8,$69,$01,$08
		.BYTE	$F8,$66,$01,$F0
		.BYTE	$F0,$65,$01,$08
		.BYTE	$F0,$64,$01,$00
		.BYTE	$F0,$63,$01,$F8
		.BYTE	$F0,$62,$01,$F0
		.BYTE	$E8,$61,$01,$08
		.BYTE	$E8,$60,$01,$00
		.BYTE	$E8,$5F,$01,$F8
		.BYTE	$80
byte_18B7B4:
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$18,$5E,$02,$07
		.BYTE	$18,$5D,$02,$FF
		.BYTE	$10,$5C,$00,$0F
		.BYTE	$10,$5B,$00,$07
		.BYTE	$10,$4B,$42,$FF
		.BYTE	$10,$5A,$02,$F7
		.BYTE	$10,$59,$00,$EF
		.BYTE	$08,$48,$40,$FF
		.BYTE	$08,$58,$00,$0F
		.BYTE	$08,$57,$00,$07
		.BYTE	$08,$56,$00,$F7
		.BYTE	$08,$55,$00,$EF
		.BYTE	$F8,$4E,$00,$E7
		.BYTE	$F8,$50,$01,$FF
		.BYTE	$F8,$4F,$01,$F7
		.BYTE	$00,$54,$00,$FF
		.BYTE	$00,$53,$00,$F7
		.BYTE	$00,$52,$00,$EF
		.BYTE	$00,$51,$01,$E7
		.BYTE	$00,$43,$41,$07
		.BYTE	$F8,$41,$01,$07
		.BYTE	$F8,$3E,$01,$EF
		.BYTE	$F0,$3D,$01,$07
		.BYTE	$F0,$3C,$01,$FF
		.BYTE	$F0,$3B,$01,$F7
		.BYTE	$F0,$3A,$01,$EF
		.BYTE	$E8,$39,$01,$FF
		.BYTE	$E8,$38,$01,$F7
		.BYTE	$80
byte_18B82D:
		.BYTE	$00,$72,$00,$FF
		.BYTE	$00,$71,$00,$F7
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$18,$5E,$02,$07
		.BYTE	$18,$5D,$02,$FF
		.BYTE	$10,$5C,$00,$0F
		.BYTE	$10,$5B,$00,$07
		.BYTE	$10,$4B,$42,$FF
		.BYTE	$10,$5A,$02,$F7
		.BYTE	$10,$59,$00,$EF
		.BYTE	$08,$48,$40,$FF
		.BYTE	$08,$58,$00,$0F
		.BYTE	$08,$57,$00,$07
		.BYTE	$08,$56,$00,$F7
		.BYTE	$08,$55,$00,$EF
		.BYTE	$F8,$4E,$00,$E7
		.BYTE	$F8,$50,$01,$FF
		.BYTE	$F8,$4F,$01,$F7
		.BYTE	$00,$52,$00,$EF
		.BYTE	$00,$51,$01,$E7
		.BYTE	$00,$43,$41,$07
		.BYTE	$F8,$41,$01,$07
		.BYTE	$F8,$3E,$01,$EF
		.BYTE	$F0,$3D,$01,$07
		.BYTE	$F0,$3C,$01,$FF
		.BYTE	$F0,$3B,$01,$F7
		.BYTE	$F0,$3A,$01,$EF
		.BYTE	$E8,$39,$01,$FF
		.BYTE	$E8,$38,$01,$F7
		.BYTE	$80
byte_18B8A6:
		.BYTE	$08,$74,$00,$00
		.BYTE	$08,$73,$00,$F8
		.BYTE	$E8,$00,$01,$FD
		.BYTE	$F8,$06,$01,$08
		.BYTE	$F8,$05,$01,$00
		.BYTE	$F8,$04,$01,$F8
		.BYTE	$F0,$03,$01,$08
		.BYTE	$F0,$02,$01,$00
		.BYTE	$F0,$01,$01,$F8
		.BYTE	$00,$0A,$01,$08
		.BYTE	$00,$09,$01,$00
		.BYTE	$00,$08,$01,$F8
		.BYTE	$00,$07,$01,$F0
		.BYTE	$10,$4B,$02,$F8
		.BYTE	$10,$5B,$40,$F0
		.BYTE	$10,$5C,$40,$E8
		.BYTE	$08,$58,$40,$E8
		.BYTE	$08,$57,$40,$F0
		.BYTE	$08,$58,$00,$10
		.BYTE	$08,$57,$00,$08
		.BYTE	$10,$5C,$00,$10
		.BYTE	$10,$5B,$00,$08
		.BYTE	$10,$4B,$42,$00
		.BYTE	$18,$4C,$42,$08
		.BYTE	$18,$4D,$42,$00
		.BYTE	$18,$4D,$02,$F8
		.BYTE	$18,$4C,$02,$F0
		.BYTE	$80
byte_18B913:
		.BYTE	$18,$84,$42,$08
		.BYTE	$18,$85,$42,$00
		.BYTE	$18,$85,$02,$F8
		.BYTE	$18,$84,$02,$F0
		.BYTE	$08,$7F,$40,$08
		.BYTE	$10,$83,$40,$00
		.BYTE	$10,$82,$40,$08
		.BYTE	$08,$81,$00,$00
		.BYTE	$08,$80,$00,$F8
		.BYTE	$10,$83,$00,$F8
		.BYTE	$10,$82,$00,$F0
		.BYTE	$08,$7F,$00,$F0
		.BYTE	$00,$7E,$01,$08
		.BYTE	$00,$7D,$01,$00
		.BYTE	$00,$7C,$01,$F8
		.BYTE	$00,$7B,$01,$F0
		.BYTE	$F8,$7A,$01,$08
		.BYTE	$F8,$79,$01,$00
		.BYTE	$F8,$78,$01,$F8
		.BYTE	$F0,$77,$01,$08
		.BYTE	$F0,$76,$01,$00
		.BYTE	$F0,$75,$01,$F8
		.BYTE	$80
byte_18B96C:
		.BYTE	$F0,$87,$01,$00
		.BYTE	$F0,$86,$01,$F8
		.BYTE	$10,$94,$42,$00
		.BYTE	$10,$93,$42,$08
		.BYTE	$10,$92,$42,$10
		.BYTE	$08,$8F,$42,$10
		.BYTE	$08,$90,$42,$08
		.BYTE	$08,$91,$42,$00
		.BYTE	$08,$91,$02,$F8
		.BYTE	$08,$90,$02,$F0
		.BYTE	$10,$94,$02,$F8
		.BYTE	$10,$93,$02,$F0
		.BYTE	$10,$92,$02,$E8
		.BYTE	$08,$8F,$02,$E8
		.BYTE	$00,$8E,$40,$00
		.BYTE	$00,$8D,$40,$08
		.BYTE	$00,$8C,$40,$10
		.BYTE	$F8,$89,$41,$08
		.BYTE	$F8,$88,$41,$10
		.BYTE	$00,$8E,$00,$F8
		.BYTE	$00,$8D,$00,$F0
		.BYTE	$00,$8C,$00,$E8
		.BYTE	$F8,$8B,$00,$00
		.BYTE	$F8,$8A,$00,$F8
		.BYTE	$F8,$89,$01,$F0
		.BYTE	$F8,$88,$01,$E8
		.BYTE	$F0,$69,$01,$08
		.BYTE	$F0,$66,$01,$F0
		.BYTE	$E8,$65,$01,$08
		.BYTE	$E8,$64,$01,$00
		.BYTE	$E8,$63,$01,$F8
		.BYTE	$E8,$62,$01,$F0
		.BYTE	$E0,$61,$01,$08
		.BYTE	$E0,$60,$01,$00
		.BYTE	$E0,$5F,$01,$F8
		.BYTE	$80
byte_18B9F9:
		.BYTE	$18,$84,$02,$F0
		.BYTE	$18,$85,$02,$F8
		.BYTE	$18,$85,$42,$00
		.BYTE	$18,$84,$42,$08
		.BYTE	$08,$7F,$00,$F0
		.BYTE	$10,$83,$00,$F8
		.BYTE	$10,$82,$00,$F0
		.BYTE	$08,$81,$40,$F8
		.BYTE	$08,$80,$40,$00
		.BYTE	$10,$83,$40,$00
		.BYTE	$10,$82,$40,$08
		.BYTE	$08,$7F,$40,$08
		.BYTE	$00,$7E,$41,$F0
		.BYTE	$00,$7D,$41,$F8
		.BYTE	$00,$7C,$41,$00
		.BYTE	$00,$7B,$41,$08
		.BYTE	$F8,$7A,$41,$F0
		.BYTE	$F8,$79,$41,$F8
		.BYTE	$F8,$78,$41,$00
		.BYTE	$F0,$77,$41,$F0
		.BYTE	$F0,$76,$41,$F8
		.BYTE	$F0,$75,$41,$00
		.BYTE	$80
byte_18BA52:
		.BYTE	$10,$B9,$03,$18
		.BYTE	$10,$B8,$03,$10
		.BYTE	$08,$B7,$03,$18
		.BYTE	$08,$B6,$03,$10
		.BYTE	$10,$B9,$00,$00
		.BYTE	$10,$B8,$00,$F8
		.BYTE	$08,$B7,$00,$00
		.BYTE	$08,$B6,$00,$F8
		.BYTE	$10,$B9,$01,$E8
		.BYTE	$10,$B8,$01,$E0
		.BYTE	$08,$B7,$01,$E8
		.BYTE	$08,$B6,$01,$E0
		.BYTE	$80
byte_18BA83:
		.BYTE	$E8,$00,$01,$FD
		.BYTE	$10,$12,$02,$08
		.BYTE	$10,$11,$00,$00
		.BYTE	$10,$10,$02,$F8
		.BYTE	$10,$0F,$00,$F0
		.BYTE	$08,$0E,$00,$08
		.BYTE	$08,$0D,$00,$00
		.BYTE	$08,$0C,$00,$F8
		.BYTE	$08,$0B,$00,$F0
		.BYTE	$00,$0A,$01,$08
		.BYTE	$00,$09,$01,$00
		.BYTE	$00,$08,$01,$F8
		.BYTE	$00,$07,$01,$F0
		.BYTE	$F8,$06,$01,$08
		.BYTE	$F8,$05,$01,$00
		.BYTE	$F8,$04,$01,$F8
		.BYTE	$F0,$03,$01,$08
		.BYTE	$F0,$02,$01,$00
		.BYTE	$F0,$01,$01,$F8
		.BYTE	$18,$15,$02,$04
		.BYTE	$18,$14,$02,$FC
		.BYTE	$18,$13,$02,$F4
		.BYTE	$80
byte_18BADC:
		.BYTE	$F8,$1B,$01,$08
		.BYTE	$F8,$1A,$01,$00
		.BYTE	$F8,$19,$01,$F8
		.BYTE	$F0,$18,$01,$08
		.BYTE	$F0,$17,$01,$00
		.BYTE	$F0,$16,$01,$F8
		.BYTE	$10,$24,$00,$06
		.BYTE	$10,$23,$00,$FE
		.BYTE	$10,$22,$00,$F6
		.BYTE	$08,$21,$00,$06
		.BYTE	$08,$20,$00,$FE
		.BYTE	$08,$1F,$00,$F6
		.BYTE	$00,$1E,$01,$06
		.BYTE	$00,$1D,$01,$FE
		.BYTE	$00,$1C,$01,$F6
		.BYTE	$18,$26,$02,$FE
		.BYTE	$18,$25,$02,$F6
		.BYTE	$80
byte_18BB21:
		.BYTE	$E8,$00,$01,$FE
		.BYTE	$00,$0A,$01,$09
		.BYTE	$00,$09,$01,$01
		.BYTE	$00,$08,$01,$F9
		.BYTE	$00,$07,$01,$F1
		.BYTE	$F8,$06,$01,$09
		.BYTE	$F8,$05,$01,$01
		.BYTE	$F8,$04,$01,$F9
		.BYTE	$F0,$03,$01,$09
		.BYTE	$F0,$02,$01,$01
		.BYTE	$F0,$01,$01,$F9
		.BYTE	$10,$2A,$00,$00
		.BYTE	$10,$29,$00,$F8
		.BYTE	$08,$28,$00,$00
		.BYTE	$08,$27,$00,$F8
		.BYTE	$18,$2D,$02,$01
		.BYTE	$18,$2C,$02,$F9
		.BYTE	$18,$2B,$02,$F1
		.BYTE	$80
byte_18BB6A:
		.BYTE	$F8,$DD,$00,$FF
		.BYTE	$00,$DC,$00,$00
		.BYTE	$00,$00,$00,$00
		.BYTE	$00,$C0,$00,$FF
		.BYTE	$80

; =============== S U B	R O U T	I N E =======================================
_rand:
		LDA		_rand_seed
		STA		_rand_seed+2
		ADC		_rand_seed+1
		STA		_rand_seed
		ADC		_rand_seed+2
		STA		_rand_seed+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_area_fill:
		JSR		_ppu_set_hor_increment
		STX		_PPU_ADDR
		LDX		#0
		STX		_PPU_ADDR
		LDX		#4
		LDY		#0
loc_18BB97:
		STA		_PPU_DATA
		DEY
		BNE		loc_18BB97
		DEX
		BNE		loc_18BB97
		JMP		_ppu_reset

; =============== S U B	R O U T	I N E =======================================
_ppu_increment_select:
		PHA
		LDA		_ppu_ctrl_shadow
		ORA		#4
		BCS		loc_18BBAC
		AND		#$FB
loc_18BBAC:
		STA		_PPU_CTRL
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_send:
		STA		_PPU_ADDR
		INY
		LDA		(_ptr0),Y
		STA		_PPU_ADDR
		INY
		LDA		(_ptr0),Y
		ASL
		JSR		_ppu_increment_select
		ASL
		LDA		(_ptr0),Y
		AND		#$3F
		TAX
		BCC		loc_18BBCA
		INY
loc_18BBCA:
		BCS		loc_18BBCD
		INY
loc_18BBCD:
		LDA		(_ptr0),Y
		STA		_PPU_DATA
		DEX
		BNE		loc_18BBCA
		INY
		JSR		_adc_ptr0

; =============== S U B	R O U T	I N E =======================================
_tlm_transfer:
		LDY		#0
		LDA		(_ptr0),Y
		BNE		_tlm_send
		JMP		_ppu_reset

; =============== S U B	R O U T	I N E =======================================
_adc_ptr0:
		TYA
		CLC
		ADC		_ptr0
		STA		_ptr0
		BCC		locret_18BBEC
		INC		_ptr0+1
locret_18BBEC:
		RTS

; =============== S U B	R O U T	I N E =======================================
;_adc_tmp2_unref:
;		TYA
;		CLC
;		ADC		_tmp2
;		STA		_tmp2
;		BCC		locret_18BBF7
;		INC		_tmp3
;locret_18BBF7:
;		RTS

_pal_bg_lib:
		.BYTE	$0F,$36,$10,$12,$0F,$30,$10,$27,$0F,$36,$00,$16,$0F,$12,$28,$2A
		.BYTE	$0F,$3C,$21,$12,$0F,$30,$10,$37,$0F,$36,$10,$16,$0F,$26,$39,$2B
		.BYTE	$0F,$30,$14,$12,$0F,$39,$19,$29,$0F,$30,$28,$17,$0F,$30,$27,$08
		.BYTE	$30,$3C,$25,$0F,$30,$3C,$11,$12,$30,$34,$24,$14,$30,$3C,$27,$06
		.BYTE	$0F,$35,$30,$13,$0F,$35,$30,$28,$0F,$35,$30,$16,$0F,$35,$30,$1A
		.BYTE	$0F,$3C,$10,$12,$0F,$30,$10,$37,$0F,$36,$10,$16,$0F,$12,$37,$29
		.BYTE	$0F,$38,$27,$17,$0F,$0F,$27,$30,$0F,$2C,$19,$2A,$0F,$30,$16,$06
		.BYTE	$0F,$30,$16,$34,$0F,$30,$11,$34,$0F,$30,$35,$14,$0F,$38,$28,$02
		.BYTE	$0F,$30,$26,$17,$0F,$37,$2A,$12,$0F,$30,$1B,$16,$0F,$30,$15,$05
		.BYTE	$0F,$30,$26,$17,$0F,$26,$0F,$15,$0F,$30,$2B,$12,$0F,$35,$23,$03
		.BYTE	$0F,$30,$25,$0F,$0F,$30,$22,$26,$0F,$30,$23,$15,$0F,$12,$37,$29
		.BYTE	$30,$0F,$37,$0A,$30,$35,$25,$15,$30,$24,$38,$0F,$30,$0F,$02,$12
		.BYTE	$0F,$36,$10,$12,$0F,$30,$10,$27,$0F,$36,$00,$16,$0F,$12,$27,$0A
		.BYTE	$0F,$36,$10,$12,$0F,$30,$10,$27,$0F,$36,$00,$16,$0F,$12,$27,$17
		.BYTE	$0F,$36,$10,$12,$0F,$30,$10,$27,$0F,$36,$00,$16,$0F,$12,$28,$3C
		.BYTE	$0F,$30,$26,$13,$0F,$30,$24,$0F,$0F,$30,$26,$28,$0F,$24,$16,$30
		.BYTE	$0F,$30,$26,$17,$0F,$37,$2A,$12,$0F,$30,$2B,$16,$0F,$35,$23,$03
		.BYTE	$0F,$30,$30,$08,$0F,$0F,$0F,$0F,$0F,$30,$1B,$16,$0F,$36,$26,$17
		.BYTE	$0F,$30,$2A,$14,$0F,$32,$22,$02,$0F,$30,$26,$0F,$0F,$30,$16,$2C
		.BYTE	$0F,$30,$30,$30,$0F,$30,$30,$30,$0F,$30,$30,$30,$0F,$30,$30,$30
_pal_spr_lib:
		.BYTE	$0F,$16,$00,$0F,$0F,$30,$35,$0F,$0F,$26,$18,$0F,$0F,$3C,$12,$0F
		.BYTE	$0F,$15,$35,$0F,$0F,$1A,$35,$0F,$0F,$15,$27,$0F,$0F,$36,$16,$0F
		.BYTE	$0F,$30,$15,$02,$0F,$30,$36,$0F,$0F,$26,$37,$05,$0F,$30,$30,$0F
		.BYTE	$0F,$16,$10,$0F,$0F,$30,$35,$0F,$0F,$3C,$12,$0F,$0F,$35,$15,$0F
		.BYTE	$0F,$30,$27,$15,$0F,$30,$27,$0F,$0F,$30,$0F,$22,$0F,$30,$22,$26
		.BYTE	$0F,$30,$16,$34,$0F,$30,$36,$0F,$0F,$32,$17,$0F,$0F,$25,$36,$17
		.BYTE	$0F,$0F,$16,$26,$0F,$30,$35,$0F,$0F,$26,$16,$06,$0F,$30,$16,$06
		.BYTE	$0F,$30,$0F,$17,$0F,$30,$35,$0F,$0F,$30,$0F,$12,$0F,$0F,$0F,$0F
		.BYTE	$0F,$0F,$30,$0F,$0F,$30,$35,$0F,$0F,$30,$0F,$12,$0F,$0F,$0F,$0F
		.BYTE	$0F,$15,$35,$0F,$0F,$1A,$35,$0F,$0F,$15,$27,$0F,$0F,$1A,$23,$0F
		.BYTE	$0F,$15,$35,$0F,$0F,$1A,$35,$0F,$0F,$15,$27,$0F,$0F,$23,$39,$0F
		.BYTE	$0F,$16,$30,$28,$0F,$16,$30,$28,$0F,$16,$30,$28,$0F,$16,$30,$28

; =============== S U B	R O U T	I N E =======================================
_rand_clamp:
		PHA
		JSR		_rand
		PHA
		JSR		_mmc5_mul8to8
		PLA
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_div_16to16:
		LDA		_tmpEB
		ORA		_tmpEA
		BNE		loc_18BE0B
		SEC
		RTS
loc_18BE0B:
		LDY		#$10
loc_18BE0D:
		ROL		_tmpE6
		ROL		_tmpE7
		ROL		_tmpE8
		ROL		_tmpE8+1
		LDA		_tmpE8
		SEC
		SBC		_tmpEA
		STA		_tmpE8
		LDA		_tmpE8+1
		SBC		_tmpEB
		STA		_tmpE8+1
		BCS		loc_18BE32
		LDA		_tmpE8
		CLC
		ADC		_tmpEA
		STA		_tmpE8
		LDA		_tmpE8+1
		ADC		_tmpEB
		STA		_tmpE8+1
		CLC
loc_18BE32:
		DEY
		BNE		loc_18BE0D
		ROL		_tmpE6
		ROL		_tmpE7
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_mul8to8:
		STX		_backup_var
		TSX
		LDA		$103,X
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		$104,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		STA		$103,X
		LDA		_MMC5_MUL1
		STA		$104,X
		LDX		_backup_var
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_mul16to8:
		STX		_backup_var
		TSX
		LDA		$103,X
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		$104,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		STA		$103,X
		LDA		_MMC5_MUL1
		STA		_mmc5_mtmp_mul_result
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_mtmp_mul_result
		STA		$104,X
		LDA		_MMC5_MUL1
		ADC		#0
		STA		$105,X
		LDX		_backup_var
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_mul32to8:
		STX		_backup_var
		TSX
		LDA		$103,X
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		$104,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		STA		$103,X
		LDA		_MMC5_MUL1
		STA		_mmc5_mtmp_mul_result
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_mtmp_mul_result
		STA		$104,X
		LDA		_MMC5_MUL1
		ADC		#0
		STA		_mmc5_mtmp_mul_result
		LDA		$106,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_mtmp_mul_result
		STA		$105,X
		LDA		_MMC5_MUL1
		ADC		#0
		STA		_mmc5_mtmp_mul_result
		LDA		$107,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_mtmp_mul_result
		STA		$106,X
		LDA		_MMC5_MUL1
		ADC		#0
		STA		$107,X
		LDX		_backup_var
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_mul16to16:
		STX		_backup_var
		TSX
		LDA		$103,X
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		STA		$103,X
		LDA		_MMC5_MUL1
		STA		_mmc5_mtmp_mul_result
		LDA		$106,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_mtmp_mul_result
		STA		_mmc5_mtmp_mul_result
		LDA		_MMC5_MUL1
		ADC		#0
		STA		byte_5A6
		LDA		$104,X
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_mtmp_mul_result
		STA		$104,X
		LDA		_MMC5_MUL1
		ADC		byte_5A6
		STA		byte_5A6
		LDA		#0
		ADC		#0
		STA		byte_5A7
		LDA		$106,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		byte_5A6
		STA		$105,X
		LDA		_MMC5_MUL1
		ADC		byte_5A7
		STA		$106,X
		LDX		_backup_var
		RTS

_bg_music_list:
		.BYTE	$08,$20,$41,$42,$44,$48

		LAST_BANK_END F,$10000

total_banks	= 16
total_size	= 16*8192
