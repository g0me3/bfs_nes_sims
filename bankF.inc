
		ORG $E000

		BANK_START $FF

		INCBIN "..\copyrights.txt"

; =============== S U B	R O U T	I N E =======================================
RESET:
		SEI
		CLD
		LDX		#$00
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
		LDX		#$04
		STX		_MMC5_PRG_PAGES
		LDX		#$00
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
		STY		_ptr0								; CAUTION! Y here = 0 because addr = 700
		TYA											; clear Y manually if address changes
loc_18A03C:
		STA		(_ptr0),Y
		DEY
		BNE		loc_18A03C
		DEC		_ptr0+1
		BPL		loc_18A03C
		LDX		#$00
		LDY		#$04
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
		LDA		#$03
		STA		_MMC5_PRG_SIZE
		LDA		#$04
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
		LDA		#$00
		STA		_tmp9
		LDA		#$C0
		STA		_PAD1_REG
		LDA		#$00
		STA		byte_4084+$F8C
		FJSRA	_apu_init,SRAM,PRG0
		LDA		#$80
		STA		_apu_mus_idx_req
		LDX		#$1F
		LDA		#$0F
loc_18A09D:
		STA		_pal_bg_buf,X
		DEX
		BPL		loc_18A09D
		LDA		#$03
		STA		_MMC5_CHR_SIZE
		LDA		#$00
		STA		_MMC5_CHR_BANKSA+7
		LDA		#$02
		STA		_MMC5_CHR_MODE
		STA		_mmc5_chr_mode_shadow
		LDA		#$00
		STA		_mmc5_sp_mode_shadow
		STA		_MMC5_SP_MODE
		LDA		#$00
		STA		_MMC5_SP_SCROLL
		STA		_mmc5_sp_scroll_shadow
		STA		_MMC5_SP_PAGE
		LDA		#$00
		STA		_MMC5_NT_MODE
		LDA		#$FF
		STA		_MMC5_NT_FILL
		LDA		#$00
		STA		_MMC5_AT_FILL
		LDA		#$00
		STA		_MMC5_IRQ_ENABLE
		STA		_title_sub_idx
		JSR		_spr_clear
		LDY		#$01
		STY		_rand_seed
		INY
		STY		_rand_seed+1
		INY
		STY		_rand_seed+2
		JSR		_thread1_init
		LDX		#$04
		LDA		#$64
loc_18A0F0:
		STA		_funding_level,X
		DEX
		BPL		loc_18A0F0
		JSR		_sram_on
		FJSRA	_sram_init,SRAM,PRG3
		JSR		_sram_off
		LDA		#$FF
		STA		_game_core_state
		STA		byte_37
		LDA		#$01
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
		LDA		#$00
		STA		_scr_res_idx
		JSR		_ppu_queues_reset
		FJSRA	_title_screen,PRG7,PRG9
		INC		_main_sub_idx
		RTS
_game_setup_mode:
		DEX
		BNE		_ingame_mode
		JSR		_ppu_queues_reset
		FJSRA	_game_setup,SRAM,PRG1
		RTS
_ingame_mode:
		JSR		_ppu_queues_reset
		FJSRA	_game_vars_init,SRAM,PRGA
		JSR		_thread1_init
		FJSRA	_game_start,PRGC,PRGD
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
		LDA		#$02
		STA		_PPU_OAM_DMA
		CLI
		JSR		_ppu_regs_reload
		JSR		_ppu_scrolls_reload
		JSR		_mmc5_scrolls_reload
loc_18A193:
		JMP		loc_18A359
loc_18A196:
		LDA		#$02
		STA		_PPU_OAM_DMA
		CLI
		LDA		_ppu_pal_dequeue_req
		BEQ		loc_18A1B8
		JSR		_pal_ppu_send
		LDA		#$00
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
		LDA		#$00
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
		MOVW	_MMC5_CHR_BANKSA,_mmc5_chr_banks_shadow
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
		AND		#$07
		STA		_frames_div8_counter
		INC		_frames_div4_counter
		LDA		_frames_div4_counter
		AND		#$0F
		STA		_frames_div4_counter
		LDA		#$00
		STA		_spr_buf_pos
		LDA		_scr_mode_idx
		BEQ		loc_18A258
		LDA		_scr_res_idx
		CMP		#$01
		BNE		loc_18A2A1
		LDA		_maps_info_mode_idx
		BNE		loc_18A2A1
loc_18A258:
		PUSHB	_prg_prg8000_shadow
		PUSHB	_prg_prgA000_shadow
		LDA		#$F5
		STA		_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		#$FD
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_scr_mode_idx
		CMP		#$01
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
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
loc_18A2A1:
		LDA		_game_wnd_spr_mode_flag
		BNE		loc_18A2D6
		LDA		_city._game_speed
		CMP		#$03
		BNE		loc_18A2B4
		LDA		#$00
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
		LDX		#$00
		CMP		#$08
		BCC		loc_18A2E5
		INX
loc_18A2E5:
		LDA		_bitmasks,Y
		AND		_thread0_flags,X
		BNE		_switch_to_thread1
		LDA		_thread_priority_req
		AND		#$7F
		STA		_thread_priority_req
		LDA		#$01
		STA		_game_core_state
		BNE		_switch_to_thread0
loc_18A2FC:
		LDA		_game_core_state
		ORA		_pad0_held
		BNE		_switch_to_thread0
		LDX		#$00
		LDY		_maps_info_mode_idx
		CPY		#$08
		BCC		loc_18A30D
		INX
loc_18A30D:
		LDA		_bitmasks,Y
		AND		_thread1_flags,X
		BEQ		_switch_to_thread1
		LDY		#$00
		STY		_thread1_flags
		STY		_thread1_flags+1
		INY
		STY		byte_107
		STY		_game_core_state
_switch_to_thread0:
		LDX		#$00
		BEQ		loc_18A32E
_switch_to_thread1:
		LDX		#$00
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
		LDA		#$00
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
		LDA		#$00
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
		LDX		#$00
		ASL
		BCS		loc_18A45A
		LDA		_scr_mode_idx
		CMP		#$03
		BNE		loc_18A41F
		LDA		#$00
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
		AND		#$03
		BNE		loc_18A438
		DEC		byte_674
loc_18A438:
		LDA		_frames_counter
		AND		#$0F
		CMP		#$08
		LDA		_tmp15
		BCS		loc_18A448
		SEC
		SBC		#$02
		JMP		loc_18A44B
loc_18A448:
		CLC
		ADC		#$02
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
		LDA		#$00
		STA		byte_14
		LDA		#$1E
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		RTS
loc_18A48B:
		CMP		#$03
		BEQ		loc_18A49E
		LDA		#$02
		STA		byte_14
		LDA		#$DE
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		RTS
loc_18A49E:
		LDA		#$00
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
		CMP		#$01
		BEQ		sub_18A4E4
		CMP		#$02
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
		LDX		#$00
		STA		_PPU_SCROLL
		STX		_PPU_SCROLL
		LDA		#$01
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
		LDA		#$02
		STA		byte_14
		LDA		#$DE
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		JMP		loc_18A4B8

; =============== S U B	R O U T	I N E =======================================
sub_18A4FB:
		LDA		#$00
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
		LDA		#$00
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
;		LDA		#$00
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
		LDA		#$00
		STA		_pad0_held
		STA		_pad0_autorep_press
		STA		_pad0_autorep_release
		RTS
loc_18A560:
		LDX		#$01
		JSR		_read_pad
		STA		_tmp15
		JSR		_read_pad
		CMP		_tmp15
		BNE		loc_18A560
		STA		_pad1_prev_held
		LDX		#$00
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
		LDX		#$08
		BNE		loc_18A590
loc_18A58E:
		LDX		#$04
loc_18A590:
		STX		_pad0_autorep_delay
		CMP		#$00
		BEQ		loc_18A5C2
		CMP		_pad0_held
		BEQ		loc_18A5AE
		EOR		_pad0_held
		AND		_tmp15
		STA		_pad0_autorep_press
		STA		_pad0_autorep_release
		CMP		#$00
		BEQ		loc_18A5C8
		LDA		#$08
		STA		_pad0_autorep_timer
		JMP		loc_18A5C8
loc_18A5AE:
		LDA		#$00
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
		LDA		#$00
		STA		_pad0_autorep_press
		STA		_pad0_autorep_release
loc_18A5C8:
		LDA		_tmp15
		STA		_pad0_held
		RTS

; =============== S U B	R O U T	I N E =======================================
_read_pad:
		LDY		#$08
		LDA		#$01
		STA		_PAD0_REG
		LDA		#$00
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
		LDA		#$02
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
		LDA		#$02
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
		LDY		#$00
		LDA		_extnt_queue_buf,X
		BPL		loc_18A739
		AND		#$7F
		LDY		#$FF
loc_18A739:
		INX
		STY		_word15_v_h_selector
		STA		_tmp18
		LDY		#$00
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
		LDA		#$00
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
		LDX		#$00
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
		LDA		#$02
		STA		_MMC5_WRAM0_ENABLE
		LDA		#$01
		STA		_MMC5_WRAM1_ENABLE
		LDA		#$FF
		STA		_sram_state_flag
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_off:
		PHA
		LDA		#$01
		STA		_MMC5_WRAM0_ENABLE
		LDA		#$02
		STA		_MMC5_WRAM1_ENABLE
		LDA		#$00
		STA		_sram_state_flag
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_farcall_argsAXY_safe:
		DEC		_mmc5_operation_in_progress_flag
		STA		_tmp3
		STX		_tmp4
		STY		_tmp5
		POPB	_ptr0
		CLC
		ADC		#$04
		STA		_tmp2
		POPB	_ptr0+1
		ADC		#$00
		PHA
		PUSHB	_tmp2
		PUSHB	_prg_prg8000_shadow
		PUSHB	_prg_prgA000_shadow
		LDY		#$01
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
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
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
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
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
		POPB	_ptrDC
		CLC
		ADC		#$04
		TAX
		POPB	_ptrDC+1
		ADC		#$00
		PHA
		TXA
		PHA
		PUSHB	_prg_prg8000_shadow
		PUSHB	_prg_prgA000_shadow
		CMP		#$FB
		LDY		#$01
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
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDX		word_EE
		LDY		word_EE+1
		JSR		_jmp_ptrDC
		DEC		_mmc5_operation_in_progress_flag
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
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
		INCW	_tmp15
		RTS
loc_18A866:
		LDA		_tmp15
		CLC
		ADC		#$20
		STA		_tmp15
		LDA		_tmp15+1
		ADC		#$00
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
		LDY		#$00
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
		LDA		#$00
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_load_lib:
		AND		#$FF
		BMI		loc_18A8CA
		LDX		#$00
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
		ADC		#$00
		STA		_ptr0+1
loc_18A8DD:
		LDA		#$10
		STA		_tmp2
		LDY		#$00
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
		PUSHB	_game_core_state
		LDA		#$01
		STA		_game_core_state
		LDA		_screen_pal_cur_bg_idx
		JSR		_pal_load_lib
		LDA		_screen_pal_cur_spr_idx
		ORA		#$80
		JSR		_pal_load_lib
		JSR		_pal_cut_off_half
		LDA		#$04
		STA		_ptr0
loc_18A90D:
		LDA		#$05
		STA		_ptr0+1
		LDA		#$03
		STA		_ppu_pal_dequeue_req
loc_18A916:
		JSR		_wait_for_nmi
		DEC		_ptr0+1
		BNE		loc_18A916
		JSR		_pal_buf_fade_in
		DEC		_ptr0
		BNE		loc_18A90D
		POPB	_game_core_state
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_cut_off_half:
		LDX		#$1F
loc_18A92B:
		LDA		_pal_bg_buf,X
		AND		#$0F
		STA		_pal_bg_buf,X
		DEX
		BPL		loc_18A92B
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_buf_fade_in:
		LDA		#$00
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
		MOVWO	_tmp4,_pal_bg_buf
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
		ADC		#$00
		STA		_tmp3
		MOVWO	_tmp4,_pal_spr_buf
		JMP		__pal_buf_fade_in

; =============== S U B	R O U T	I N E =======================================
; !FALLTHROUGH!
__pal_buf_fade_in:
		LDX		#$10
		LDY		#$00
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
		PUSHB	_game_core_state
		LDA		#$01
		STA		_game_core_state
		JSR		_pal_buf_fade_out
		LDA		#$04
		STA		_ptr0
loc_18A99B:
		LDA		#$05
		STA		_ptr0+1
		LDA		#$03
		STA		_ppu_pal_dequeue_req
loc_18A9A4:
		JSR		_wait_for_nmi
		DEC		_ptr0+1
		BNE		loc_18A9A4
		JSR		_pal_buf_fade_out
		DEC		_ptr0
		BNE		loc_18A99B
		POPB	_game_core_state
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
		LDA		#$0F
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
		ORA		#$04
		STA		_PPU_CTRL
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_buf_init:
		JSR		_sram_on
		LDX		#$00
		MOVWO	_ptr0,_wram_map_buf
		LDA		#$90
		LDX		#$16
		STA		_tmp2
		STX		_tmp3
		LDA		#$90
		LDY		#$00
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
		LDA		#$00
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
		LDA		#$00
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
		LDA		#$00
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
		LDA		#$02
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
		LDY		#$00
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
		INCW	word_9B
		DEC		byte_9F
		BNE		loc_18AB3D
		LDA		byte_A0
		BEQ		loc_18AB71
		STA		byte_9F
		LDA		#$00
		STA		byte_A0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$E0
		STA		word_9B
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#>[_MMC5_EXRAM]
		STA		word_9B+1
		BNE		loc_18AB3D
loc_18AB71:
		LDA		#$01
		STA		_MMC5_CHR_MODE
		JMP		_sram_off
loc_18AB79:
		LDY		#$00
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
		ADDW8	word_9B,#$20
		DEC		byte_9F
		BNE		loc_18AB7B
		LDA		byte_A0
		BEQ		loc_18AB71
		STA		byte_9F
		LDA		#$00
		STA		byte_A0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		STA		word_9B
		LDA		#>[_MMC5_EXRAM]
		STA		word_9B+1
		JMP		loc_18AB7B

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_pos_list_ilit:
		LDX		#$0B
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
_game_chr_common_reload:
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
		LDA		#<[_stack_upper+$49]			; offset to the top of the
		STA		_thread_stack+1					; thread1 stack
		LDA		#$FB							; pushing all stack data here
		STA		_thread_prgA000+1				; prg state is SRAM,PRGB
		LDA		#>[_THREAD1_MAIN]				; CAUTION! the lower nibble
		STA		_stack_upper+$4F				; assumed to be 00!, so won't
		LDA		#$00							; move thread1_main unless you
		STA		_thread_prg8000+1				; fix it
		STA		_stack_upper+$4E				; <- here
		STA		_stack_upper+$4D				; flags
		STA		_game_core_state				; threads core init
		LDA		#$80
		STA		_ppu_ctrl_shadow				; initial game CHR banks
		JSR		_game_chr_common_reload		; OPTIMIZE
;		LDA		#$34							; REDUNDANT, call routine above
;		STA		_MMC5_CHR_BANKSA				; instead.
;		STA		_mmc5_chr_banks_shadow
		LDA		#$35
		STA		_MMC5_CHR_BANKSA+1
		STA		_mmc5_chr_banks_shadow+1
;		LDA		#$3A							; REDUNDANT
;		STA		_MMC5_CHR_BANKSA+2
;		STA		_mmc5_chr_banks_shadow+2
;		LDA		#$37
;		STA		_MMC5_CHR_BANKSA+3
;		STA		_mmc5_chr_banks_shadow+3
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_music_off:
		LDA		_APU_STATUS
		CMP		_APU_STATUS
		BNE		_apu_music_off
		AND		#$0F
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
		LDY		#$00
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
		LDY		#$00
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
		AND		#$03
		STA		word_5A9+1
		LDA		(_tmp6),Y
		INY
		STA		word_5A9
		INCW	word_5A9
		BNE		loc_18ACBA
loc_18ACA9:
		PHA
		LDA		(_tmp6),Y
		INY
		AND		#$1F
		CLC
		ADC		#$01
		STA		word_5A9
		LDA		#$00
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
		ADC		#$01
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
		LDX		#$00
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
		LDY		#$08
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
		SBC		#$00
		STA		byte_5AC
		STA		_PPU_ADDR
		STY		_PPU_ADDR
		LDA		_PPU_DATA
		LDA		_PPU_DATA
		LDX		#$00
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
		LDA		#$00
		BEQ		loc_18AEB4
loc_18AEA8:
		LDA		#$02
		DEC		_goto_transition_counter
		BNE		loc_18AEB4
		LDA		#$00
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

; ---------------------------------------------------------------------------
; ---------------S-P-R-I-T-E--R-O-U-T-I-N-E-S--A-N-D--D-A-T-A----------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
; sprite insert routine with additional option to flip and overwrite both
; sprite flip and attributes. Used to draw the same sprites mirrored differently
; on screen with effects. cursors, rects, arrows, selection corners, etc..
;
;_spr_lib_attr_insert:
;		PUSHB	_prg_prgA000_shadow
;		LDA		#$F2
;		STA		_MMC5_PRG_PAGES+2
;		LDX		_spr_insert_args._idx
;		LDA		_spr_lib_lo,X
;		STA		_ptr0
;		LDA		_spr_lib_hi,X
;		STA		_ptr0+1
;		DEC		_spr_insert_args._pos._Y
;		LDX		_spr_buf_pos
;		LDY		#$00
;loc_18A603:
;		LDA		(_ptr0),Y
;		CMP		#$80
;		BEQ		loc_18A643
;		LDA		_spr_insert_args._attr
;		ASL
;		LDA		(_ptr0),Y
;		INY
;		BCC		loc_18A615
;		EOR		#$FF
;		SBC		#$06
;loc_18A615:
;		CLC
;		ADC		_spr_insert_args._pos._Y
;		STA		_spr_buf,X
;		INX
;		LDA		(_ptr0),Y
;		INY
;		STA		_spr_buf,X
;		INX
;		LDA		(_ptr0),Y
;		INY
;		ORA		_spr_insert_args._attr
;		STA		_spr_buf,X
;		INX
;		LDA		_spr_insert_args._attr
;		ASL
;		ASL
;		LDA		(_ptr0),Y
;		INY
;		BCC		loc_18A639
;		EOR		#$FF
;		SBC		#$06
;loc_18A639:
;		CLC
;		ADC		_spr_insert_args._pos._X
;		STA		_spr_buf,X
;		INX
;		JMP		loc_18A603
;loc_18A643:
;		STX		_spr_buf_pos
;		POPB	_prg_prgA000_shadow
;		STA		_MMC5_PRG_PAGES+2
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
; Display raw sprites in corresponding position on screen, no flips/attr
; overwrites not performed. Used only in drawing an ingame message windows
; with animated sprites. Simple version of _spr_lib_attr_insert, may be
; replaced with it, but with additional clear of the _attr parameter.
; However, has an additional code at the end to clip the sprites out of the
; boundary of the mesage window.
;
;_spr_lib_clip_insert:
;		PUSHB	_prg_prg8000_shadow
;		LDA		#$F8
;		STA		_MMC5_PRG_PAGES+1
;		LDX		_spr_insert_args._idx
;		LDA		_spr_lib_lo,X
;		STA		_ptr0
;		LDA		_spr_lib_hi,X
;		STA		_ptr0+1
;		DEC		_spr_insert_args._pos._Y
;		LDX		_spr_buf_pos
;		LDY		#$00
;loc_18A666:
;		LDA		(_ptr0),Y
;		CMP		#$80
;		BEQ		loc_18A6A4
;		LDA		(_ptr0),Y
;		INY
;		CLC
;		ADC		_spr_insert_args._pos._Y
;		STA		_spr_buf,X
;		INX
;		LDA		(_ptr0),Y
;		INY
;		STA		_spr_buf,X
;		INX
;		LDA		(_ptr0),Y
;		INY
;		STA		_spr_buf,X
;		INX
;		LDA		(_ptr0),Y
;		INY
;		CLC
;		ADC		_spr_insert_args._pos._X
;		CMP		#$38
;		BCC		loc_18A692
;		CMP		#$F8
;		BCC		loc_18A69D
;loc_18A692:
;		LDA		#$F4
;		DEX
;		DEX
;		DEX
;		STA		_spr_buf,X
;		JMP		loc_18A666
;loc_18A69D:
;		STA		_spr_buf,X
;		INX
;		JMP		loc_18A666
;loc_18A6A4:
;		STX		_spr_buf_pos
;		POPB	_prg_prg8000_shadow
;		STA		_MMC5_PRG_PAGES+1
;		RTS

; =============== S U B	R O U T	I N E =======================================
; Merged versions of sprite routines. Maybe little slower in common,
; but i'm sure will not much slower in general.
;
_spr_lib_attr_insert:
		PUSHB	_prg_prgA000_shadow
		LDA		#$F2
		STA		_MMC5_PRG_PAGES+2
		LDA		#$00							; clip no, attrs yes
		STA		_tmp17
		JSR		_spr_lib_insert_ex
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		RTS

; =============== S U B	R O U T	I N E =======================================
_spr_lib_clip_insert:
		PUSHB	_prg_prg8000_shadow
		LDA		#$F8
		STA		_MMC5_PRG_PAGES+1
		LDA		#$80							; clip yes
		STA		_tmp17
		LDA		#$00							; attrs no
		STA		_spr_insert_args._attr
		JSR		_spr_lib_insert_ex
		POPB	_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		RTS

; =============== S U B	R O U T	I N E =======================================
; Common sprite insert routine for all different kind of sprites.
;
_spr_lib_insert_ex:
		LDX		_spr_insert_args._idx
		LDA		_spr_lib_lo,X
		STA		_ptr0
		LDA		_spr_lib_hi,X
		STA		_ptr0+1
		DEC		_spr_insert_args._pos._Y
		LDX		_spr_buf_pos
		LDY		#$00
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
		SBC		#$06
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
		SBC		#$06
loc_18A639:
		CLC
		ADC		_spr_insert_args._pos._X
		BIT		_tmp17
		BPL		loc_18A69D						; no 80 flag, no clip
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
		JMP		loc_18A603
loc_18A69D:
		STA		_spr_buf,X
		INX
		JMP		loc_18A603
loc_18A643:
		STX		_spr_buf_pos
		RTS

_spr_lib_lo:
		.BYTE	<[_spr00]
		.BYTE	<[_spr01]
		.BYTE	<[_spr02]
		.BYTE	<[_spr03]
		.BYTE	<[_spr04]
		.BYTE	<[_spr05]
		.BYTE	<[_spr06]
		.BYTE	<[_spr07]
		.BYTE	<[_spr08]
		.BYTE	<[_spr09]
		.BYTE	<[_spr0A]
		.BYTE	<[_spr0B]
		.BYTE	<[_spr0C]
		.BYTE	<[_spr0D]
		.BYTE	<[_spr0E]
		.BYTE	0
		.BYTE	<[_spr10]
		.BYTE	<[_spr11]
		.BYTE	<[_spr12]
		.BYTE	<[_spr13]
		.BYTE	<[_spr14]
		.BYTE	<[_spr15]
		.BYTE	<[_spr16]
		.BYTE	<[_spr17]
		.BYTE	<[_spr18]
		.BYTE	<[_spr19]
		.BYTE	<[_spr1A]
		.BYTE	<[_spr1B]
		.BYTE	<[_spr1C]
		.BYTE	<[_spr1D]
		.BYTE	<[_spr1E]
		.BYTE	<[_spr1F]
		.BYTE	<[_spr20]
		.BYTE	<[_spr21]
		.BYTE	<[_spr22]
		.BYTE	<[_spr23]
		.BYTE	<[_spr24]
		.BYTE	<[_spr25]
		.BYTE	<[_spr26]
		.BYTE	<[_spr27]
		.BYTE	<[_spr28]
		.BYTE	<[_spr29]
		.BYTE	<[_spr2A]
		.BYTE	<[_spr2B]
		.BYTE	<[_spr2C]
		.BYTE	<[_spr2D]
		.BYTE	<[_spr2E]
		.BYTE	<[_spr2F]
		.BYTE	<[_spr30]
		.BYTE	<[_spr31]
		.BYTE	<[_spr32]
		.BYTE	<[_spr33]
		.BYTE	<[_spr34]
		.BYTE	<[_spr35]
		.BYTE	<[_spr36]
		.BYTE	<[_spr37]
		.BYTE	<[_spr38]
		.BYTE	<[_spr39]
		.BYTE	<[_spr3A]
		.BYTE	<[_spr3B]
		.BYTE	<[_spr3C]
		.BYTE	<[_spr3D]
		.BYTE	<[_spr3E]
		.BYTE	<[_spr3F]
		.BYTE	<[_spr40]
		.BYTE	<[_spr41]
		.BYTE	<[_spr42]
		.BYTE	<[_spr43]
		.BYTE	<[_spr44]
		.BYTE	<[_spr45]
		.BYTE	<[_spr46]
		.BYTE	<[_spr47]
		.BYTE	<[_spr48]
		.BYTE	<[_spr49]
		.BYTE	<[_spr4A]
		.BYTE	<[_spr4B]
		.BYTE	<[_spr4C]
		.BYTE	<[_spr4D]
		.BYTE	<[_spr4E]
		.BYTE	<[_spr4F]
		.BYTE	<[_spr50]
		.BYTE	<[_spr51]
		.BYTE	<[_spr52]
		.BYTE	<[_spr53]
		.BYTE	<[_spr54]
		.BYTE	<[_spr55]
		.BYTE	<[_spr56]
		.BYTE	<[_spr57]
		.BYTE	<[_spr58]
		.BYTE	<[_spr59]
		.BYTE	<[_spr5A]
		.BYTE	<[_spr5B]
		.BYTE	<[_spr5C]
		.BYTE	<[_spr5D]
		.BYTE	<[_spr5E]
		.BYTE	<[_spr5F]
		.BYTE	<[_spr60]
		.BYTE	<[_spr61]
		.BYTE	<[_spr62]
		.BYTE	<[_spr63]
		.BYTE	<[_spr64]
		.BYTE	<[_spr65]
		.BYTE	<[_spr66]
		.BYTE	<[_spr67]
		.BYTE	<[_spr68]
		.BYTE	<[_spr69]
		.BYTE	<[_spr6A]
		.BYTE	<[_spr6B]
		.BYTE	<[_spr6C]
		.BYTE	<[_spr6D]
		.BYTE	<[_spr6E]
		.BYTE	<[_spr6F]
		.BYTE	<[_spr70]
		.BYTE	<[_spr71]
		.BYTE	<[_spr72]
		.BYTE	<[_spr73]
		.BYTE	<[_spr74]
		.BYTE	<[_spr75]
		.BYTE	<[_spr76]
		.BYTE	<[_spr77]
		.BYTE	<[_spr78]
		.BYTE	<[_spr79]
		.BYTE	<[_spr7A]
		.BYTE	<[_spr7B]
		.BYTE	<[_spr7B]
		.BYTE	<[_spr7B]
		.BYTE	<[_spr7E]
		.BYTE	<[_spr7F]
		.BYTE	<[_spr80]
		.BYTE	<[_spr81]
		.BYTE	<[_spr82]
		.BYTE	<[_spr83]
		.BYTE	<[_spr84]
		.BYTE	<[_spr85]
		.BYTE	<[_spr86]
		.BYTE	<[_spr87]
		.BYTE	<[_spr88]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[_spr90]
		.BYTE	<[_spr91]
		.BYTE	<[_spr92]
		.BYTE	<[_spr93]
		.BYTE	<[_spr94]
		.BYTE	<[_spr95]
		.BYTE	<[_spr96]
		.BYTE	<[_spr97]
		.BYTE	<[_spr98]
		.BYTE	<[_spr99]
		.BYTE	<[_spr9A]
		.BYTE	<[_spr9B]
		.BYTE	<[_spr9C]
		.BYTE	<[_spr9D]
		.BYTE	<[_spr9E]
		.BYTE	<[_spr9F]
		.BYTE	<[_sprA0]
		.BYTE	<[_sprA1]
		.BYTE	<[_sprA2]
		.BYTE	<[_sprA3]
		.BYTE	<[_sprA4]
		.BYTE	<[_sprA5]
		.BYTE	<[_sprA6]
		.BYTE	<[_sprA7]
		.BYTE	<[_sprA8]
_spr_lib_hi:
		.BYTE	>[_spr00]
		.BYTE	>[_spr01]
		.BYTE	>[_spr02]
		.BYTE	>[_spr03]
		.BYTE	>[_spr04]
		.BYTE	>[_spr05]
		.BYTE	>[_spr06]
		.BYTE	>[_spr07]
		.BYTE	>[_spr08]
		.BYTE	>[_spr09]
		.BYTE	>[_spr0A]
		.BYTE	>[_spr0B]
		.BYTE	>[_spr0C]
		.BYTE	>[_spr0D]
		.BYTE	>[_spr0E]
		.BYTE	0
		.BYTE	>[_spr10]
		.BYTE	>[_spr11]
		.BYTE	>[_spr12]
		.BYTE	>[_spr13]
		.BYTE	>[_spr14]
		.BYTE	>[_spr15]
		.BYTE	>[_spr16]
		.BYTE	>[_spr17]
		.BYTE	>[_spr18]
		.BYTE	>[_spr19]
		.BYTE	>[_spr1A]
		.BYTE	>[_spr1B]
		.BYTE	>[_spr1C]
		.BYTE	>[_spr1D]
		.BYTE	>[_spr1E]
		.BYTE	>[_spr1F]
		.BYTE	>[_spr20]
		.BYTE	>[_spr21]
		.BYTE	>[_spr22]
		.BYTE	>[_spr23]
		.BYTE	>[_spr24]
		.BYTE	>[_spr25]
		.BYTE	>[_spr26]
		.BYTE	>[_spr27]
		.BYTE	>[_spr28]
		.BYTE	>[_spr29]
		.BYTE	>[_spr2A]
		.BYTE	>[_spr2B]
		.BYTE	>[_spr2C]
		.BYTE	>[_spr2D]
		.BYTE	>[_spr2E]
		.BYTE	>[_spr2F]
		.BYTE	>[_spr30]
		.BYTE	>[_spr31]
		.BYTE	>[_spr32]
		.BYTE	>[_spr33]
		.BYTE	>[_spr34]
		.BYTE	>[_spr35]
		.BYTE	>[_spr36]
		.BYTE	>[_spr37]
		.BYTE	>[_spr38]
		.BYTE	>[_spr39]
		.BYTE	>[_spr3A]
		.BYTE	>[_spr3B]
		.BYTE	>[_spr3C]
		.BYTE	>[_spr3D]
		.BYTE	>[_spr3E]
		.BYTE	>[_spr3F]
		.BYTE	>[_spr40]
		.BYTE	>[_spr41]
		.BYTE	>[_spr42]
		.BYTE	>[_spr43]
		.BYTE	>[_spr44]
		.BYTE	>[_spr45]
		.BYTE	>[_spr46]
		.BYTE	>[_spr47]
		.BYTE	>[_spr48]
		.BYTE	>[_spr49]
		.BYTE	>[_spr4A]
		.BYTE	>[_spr4B]
		.BYTE	>[_spr4C]
		.BYTE	>[_spr4D]
		.BYTE	>[_spr4E]
		.BYTE	>[_spr4F]
		.BYTE	>[_spr50]
		.BYTE	>[_spr51]
		.BYTE	>[_spr52]
		.BYTE	>[_spr53]
		.BYTE	>[_spr54]
		.BYTE	>[_spr55]
		.BYTE	>[_spr56]
		.BYTE	>[_spr57]
		.BYTE	>[_spr58]
		.BYTE	>[_spr59]
		.BYTE	>[_spr5A]
		.BYTE	>[_spr5B]
		.BYTE	>[_spr5C]
		.BYTE	>[_spr5D]
		.BYTE	>[_spr5E]
		.BYTE	>[_spr5F]
		.BYTE	>[_spr60]
		.BYTE	>[_spr61]
		.BYTE	>[_spr62]
		.BYTE	>[_spr63]
		.BYTE	>[_spr64]
		.BYTE	>[_spr65]
		.BYTE	>[_spr66]
		.BYTE	>[_spr67]
		.BYTE	>[_spr68]
		.BYTE	>[_spr69]
		.BYTE	>[_spr6A]
		.BYTE	>[_spr6B]
		.BYTE	>[_spr6C]
		.BYTE	>[_spr6D]
		.BYTE	>[_spr6E]
		.BYTE	>[_spr6F]
		.BYTE	>[_spr70]
		.BYTE	>[_spr71]
		.BYTE	>[_spr72]
		.BYTE	>[_spr73]
		.BYTE	>[_spr74]
		.BYTE	>[_spr75]
		.BYTE	>[_spr76]
		.BYTE	>[_spr77]
		.BYTE	>[_spr78]
		.BYTE	>[_spr79]
		.BYTE	>[_spr7A]
		.BYTE	>[_spr7B]
		.BYTE	>[_spr7B]
		.BYTE	>[_spr7B]
		.BYTE	>[_spr7E]
		.BYTE	>[_spr7F]
		.BYTE	>[_spr80]
		.BYTE	>[_spr81]
		.BYTE	>[_spr82]
		.BYTE	>[_spr83]
		.BYTE	>[_spr84]
		.BYTE	>[_spr85]
		.BYTE	>[_spr86]
		.BYTE	>[_spr87]
		.BYTE	>[_spr88]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[_spr90]
		.BYTE	>[_spr91]
		.BYTE	>[_spr92]
		.BYTE	>[_spr93]
		.BYTE	>[_spr94]
		.BYTE	>[_spr95]
		.BYTE	>[_spr96]
		.BYTE	>[_spr97]
		.BYTE	>[_spr98]
		.BYTE	>[_spr99]
		.BYTE	>[_spr9A]
		.BYTE	>[_spr9B]
		.BYTE	>[_spr9C]
		.BYTE	>[_spr9D]
		.BYTE	>[_spr9E]
		.BYTE	>[_spr9F]
		.BYTE	>[_sprA0]
		.BYTE	>[_sprA1]
		.BYTE	>[_sprA2]
		.BYTE	>[_sprA3]
		.BYTE	>[_sprA4]
		.BYTE	>[_sprA5]
		.BYTE	>[_sprA6]
		.BYTE	>[_sprA7]
		.BYTE	>[_sprA8]

		SECTION_START 10F

_spr00:
		.BYTE	$00,$00,$01,$00
		.BYTE	$00,$01,$01,$08
		.BYTE	$08,$02,$01,$00
		.BYTE	$08,$03,$01,$08
		.BYTE	$80
_spr01:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$04
		.BYTE	$03,$06,$00,$FD
		.BYTE	$03,$07,$00,$04
		.BYTE	$80
_spr02:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$0C
		.BYTE	$0B,$06,$00,$FD
		.BYTE	$0B,$07,$00,$0C
		.BYTE	$80
_spr03:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$14
		.BYTE	$13,$06,$00,$FD
		.BYTE	$13,$07,$00,$14
		.BYTE	$80
_spr04:
		.BYTE	$FC,$04,$00,$FD
		.BYTE	$FC,$05,$00,$1C
		.BYTE	$1B,$06,$00,$FD
		.BYTE	$1B,$07,$00,$1C
		.BYTE	$80
_spr05:
		.BYTE	$00,$E5,$00,$00
		.BYTE	$00,$E6,$00,$08
		.BYTE	$80
_spr06:
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
_spr07:
		.BYTE	$FE,$9E,$01,$00
		.BYTE	$FE,$9E,$41,$07
		.BYTE	$06,$9E,$81,$00
		.BYTE	$06,$9E,$C1,$07
		.BYTE	$80
_spr08:
		.BYTE	$00,$8B,$03,$00
		.BYTE	$00,$8C,$03,$08
		.BYTE	$08,$8D,$03,$04
		.BYTE	$80
_spr09:
		.BYTE	$00,$8D,$83,$04
		.BYTE	$08,$8B,$83,$00
		.BYTE	$08,$8C,$83,$08
		.BYTE	$80
_spr0A:
		.BYTE	$00,$8F,$43,$00
		.BYTE	$08,$9F,$43,$00
		.BYTE	$04,$8E,$43,$08
		.BYTE	$80
_spr0B:
		.BYTE	$04,$8E,$03,$00
		.BYTE	$00,$8F,$03,$08
		.BYTE	$08,$9F,$03,$08
		.BYTE	$80
_spr10:
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
_spr11:
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
_spr12:
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
_spr13:
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
_spr14:
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
_spr15:
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
_spr16:
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
_spr17:
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
_spr18:
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
_spr19:
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
_spr1A:
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
_spr1B:
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
_spr1C:
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
_spr1D:
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
_spr1E:
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
_spr1F:
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
_spr20:
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
_spr21:
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
_spr22:
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
_spr23:
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
_spr24:
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
_spr25:
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
_spr26:
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
_spr27:
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
_spr28:
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
_sprA0:
		.BYTE	$F8,$DD,$00,$FF
		.BYTE	$00,$DC,$00,$00
		.BYTE	$00,$00,$00,$00
		.BYTE	$00,$C0,$00,$FF
		.BYTE	$80

		SECTION_END	10F

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
		LDX		#$00
		STX		_PPU_ADDR
		LDX		#$04
		LDY		#$00
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
		ORA		#$04
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
		LDY		#$00
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
; input:
; dividend fixed point 16.16 (integer E8/E9, fraction E6/E7)
; divider  fixed point 8.8   (integer EB, fraction EA)
; result:
; fixed point 8.8 (integer E7, fraction E6)
;
_div_32to16:
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
		ROL		_tmpE9
		LDA		_tmpE8
		SEC
		SBC		_tmpEA
		STA		_tmpE8
		LDA		_tmpE9
		SBC		_tmpEB
		STA		_tmpE9
		BCS		loc_18BE32
		LDA		_tmpE8
		CLC
		ADC		_tmpEA
		STA		_tmpE8
		LDA		_tmpE9
		ADC		_tmpEB
		STA		_tmpE9
		CLC
loc_18BE32:
		DEY
		BNE		loc_18BE0D
		ROL		_tmpE6
		ROL		_tmpE7
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
; integer multiply 8bit args, result 16 bit
;
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
		STA		_mmc5_tmp_mul_result
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_result
		STA		$104,X
		LDA		_MMC5_MUL1
		ADC		#$00
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
		STA		_mmc5_tmp_mul_result
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_result
		STA		$104,X
		LDA		_MMC5_MUL1
		ADC		#$00
		STA		_mmc5_tmp_mul_result
		LDA		$106,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_result
		STA		$105,X
		LDA		_MMC5_MUL1
		ADC		#$00
		STA		_mmc5_tmp_mul_result
		LDA		$107,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_result
		STA		$106,X
		LDA		_MMC5_MUL1
		ADC		#$00
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
		STA		_mmc5_tmp_mul_result
		LDA		$106,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_result
		STA		_mmc5_tmp_mul_result
		LDA		_MMC5_MUL1
		ADC		#$00
		STA		_mmc5_tmp_mul_data
		LDA		$104,X
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		$105,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_result
		STA		$104,X
		LDA		_MMC5_MUL1
		ADC		_mmc5_tmp_mul_data
		STA		_mmc5_tmp_mul_data
		LDA		#$00
		ADC		#$00
		STA		_mmc5_tmp_mul_data+1
		LDA		$106,X
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		CLC
		ADC		_mmc5_tmp_mul_data
		STA		$105,X
		LDA		_MMC5_MUL1
		ADC		_mmc5_tmp_mul_data+1
		STA		$106,X
		LDX		_backup_var
		RTS

_bg_music_list:
		.BYTE	$08,$20,$41,$42,$44,$48

_get_dec_digit_tbl0:
		.BYTE	[[1 >> 0] & $FF]		; $01
_hex_to_dec_tbl2:
		.BYTE	[[10 >> 0] & $FF]		; $0A
		.BYTE	[[100 >> 0] & $FF]		; $64
		.BYTE	[[1000 >> 0] & $FF]		; $E8
		.BYTE	[[10000 >> 0] & $FF]	; $10
		.BYTE	[[100000 >> 0] & $FF]	; $A0
		.BYTE	[[1000000 >> 0] & $FF]	; $40
		.BYTE	[[10000000 >> 0] & $FF] ; $80
_get_dec_digit_tbl1:
		.BYTE	[[1 >> 8] & $FF]		; $00
_hex_to_dec_tbl1:
		.BYTE	[[10 >> 8] & $FF]		; $00
		.BYTE	[[100 >> 8] & $FF]		; $00
		.BYTE	[[1000 >> 8] & $FF]		; $03
		.BYTE	[[10000 >> 8] & $FF]	; $27
		.BYTE	[[100000 >> 8] & $FF]	; $86
		.BYTE	[[1000000 >> 8] & $FF]	; $42
		.BYTE	[[10000000 >> 8] & $FF] ; $96
_get_dec_digit_tbl2:
		.BYTE	[[1 >> 16] & $FF]		; $00
_hex_to_dec_tbl0:
		.BYTE	[[10 >> 16] & $FF]		; $00
		.BYTE	[[100 >> 16] & $FF]		; $00
		.BYTE	[[1000 >> 16] & $FF]	; $00
		.BYTE	[[10000 >> 16] & $FF]	; $00
		.BYTE	[[100000 >> 16] & $FF]	; $01
		.BYTE	[[1000000 >> 16] & $FF] ; $0F
		.BYTE	[[10000000 >> 16] & $FF]; $98

		LAST_BANK_END FF,$10000

total_banks	= 16
total_size	= 16*8192
