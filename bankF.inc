
		ORG $E000

		BANK_START $FF

aIshi:
		.BYTE	"ISHI",$FF

		INCBIN "../copyrights.txt"

; =============== S U B	R O U T	I N E =======================================
RESET:
		SEI
		CLD
		LDX		#$00
		STX		_PPU_CTRL						; regular startup code here
		STX		_PPU_MASK
		INX
loc_18A00B:
		INC		_frames_counter					; skip 2 frames of ppu dead state
		LDA		_PPU_STATUS
		BPL		loc_18A00B
		DEX
		BNE		loc_18A00B
		DEX
		TXS
		LDX		#$04
		STX		_MMC5_PRG_PAGES					; set wram bank
		LDX		#$00
		STX		_APU_STATUS
		STX		_ppu_mask_shadow
; REDUNDANT
;loc_18A023:
;		LDA		aIshi,X							; check ram signature
;		BMI		loc_18A033						; when ram isn't clear, just reset map buffer
;		CMP		_ram_signature,X
;		BNE		loc_18A030
;		INX
;		BNE		loc_18A023
;loc_18A030:
;		JSR		_city_map_init					; so actually this check does nothing here
;loc_18A033:
; -
		LDY		#$07							; now clar ram area $0000-$07FF
		STY		_ptr0+1							; because we load 700, then decrease Y
		LDY		#$00							; we will get 7FF for next offset and so on
		STY		_ptr0							; so we clear here all ram
		TYA
loc_18A03C:
		STA		(_ptr0),Y
		DEY
		BNE		loc_18A03C
		DEC		_ptr0+1
		BPL		loc_18A03C
; REDUNDANT
;		LDX		#$00
;		LDY		#$04
;loc_18A049:
;		LDA		aIshi,X							; rewrite signature even if it already there
;		STA		_ram_signature,X
;		INX
;		DEY
;		BNE		loc_18A049
; -
loc_18A053:
		JSR		_get_input						; read pad to check if we rebooted with
		LDA		_pad0_held						; soft-reset buttons, then
		AND		#$F0							; we need wait they released
		CMP		#$F0
		BEQ		loc_18A053
; REDUNDANT, moved to common init code
;		JSR		_city._menu_pos_init	; initialize window positions
		LDA		#$03
		STA		_MMC5_PRG_SIZE
		LDA		#$04
		STA		_MMC5_PRG_PAGES
		LDY		#$FC							; initial state of two switcheable banks FC, FD
		STY		_prg_prg8000_shadow
		STY		_MMC5_PRG_PAGES+1
		INY
		STY		_prg_prgA000_shadow
		STY		_MMC5_PRG_PAGES+2
		INY										; two non-switcheable banks are FE, FF always
		STY		_MMC5_PRG_PAGES+3
		INY
		STY		_MMC5_PRG_PAGES+4
;		LDA		#$00							; never used, reinit twice...
;		STA		_tmp9
		LDA		#$C0
		STA		_PAD1_REG
;		LDA		#$00							; unknown register, probably debug leftover
;		STA		byte_4084+$F8C
		FJSRA	_apu_init,SRAM,PRG0
		LDA		#_MUS_IDX_STOP
		STA		_apu_mus_idx_req
		LDX		#$1F
		LDA		#$0F
loc_18A09D:
		STA		_pal_bg_buf,X
		DEX
		BPL		loc_18A09D
; OPTIMIZED
; -
		STX		_MMC5_NT_FILL					; X here = $FF
		INX
		STX		_MMC5_IRQ_ENABLE				; now X = $00
		STX		_mmc5_sp_mode_shadow			; reset all MMC5 registers
		STX		_MMC5_SP_MODE
		STX		_mmc5_sp_scroll_shadow
		STX		_MMC5_SP_SCROLL
		STX		_MMC5_SP_PAGE
		STX		_MMC5_NT_MODE
		STX		_MMC5_AT_FILL
		STX		_MMC5_CHR_BANKSA+7
;		STX		_title_sub_idx					; REDUNDANT, all ram vars here are 0
		INX
		INX
		STX		_mmc5_chr_mode_shadow			; we need only to reinit vars that shouldn't be 0
		STX		_MMC5_CHR_MODE
		INX
		STX		_MMC5_CHR_SIZE
; -
; REDUNDANT, need more space
; -
;		LDA		#$03
;		STA		_MMC5_CHR_SIZE
;		LDA		#$00
;		STA		_MMC5_CHR_BANKSA+7
;		LDA		#$02
;		STA		_mmc5_chr_mode_shadow
;		STA		_MMC5_CHR_MODE
;		LDA		#$00
;		STA		_mmc5_sp_mode_shadow
;		STA		_MMC5_SP_MODE
;		LDA		#$00
;		STA		_mmc5_sp_scroll_shadow
;		STA		_MMC5_SP_SCROLL
;		STA		_MMC5_SP_PAGE
;		LDA		#$00
;		STA		_MMC5_NT_MODE
;		LDA		#$FF
;		STA		_MMC5_NT_FILL
;		LDA		#$00
;		STA		_MMC5_AT_FILL
;		LDA		#$00
;		STA		_MMC5_IRQ_ENABLE
;		STA		_title_sub_idx
; -
		JSR		_spr_clear
		LDY		#$01
		STY		_rand_seed						; initial state of rand $030201
		INY
		STY		_rand_seed+1
		INY
		STY		_rand_seed+2
;		JSR		_thread1_init					; REDUNDANT, initialized before game
		JSR		_game_chr_common_reload			; OPTIMIZED, we only need CHR setup here
		LDA		#$35
		STA		_mmc5_chr_banks_shadow+1
		STA		_MMC5_CHR_BANKSA+1
		LDX		#$04
		LDA		#100
loc_18A0F0:
		STA		_budget_fund_rate,X
		DEX
		BPL		loc_18A0F0
;		JSR		_sram_write_enable				; REDUNDANT
		FJSRA	_sram_init,SRAM,PRG3
;		JSR		_sram_write_disable				; REDUNDANT
		LDA		#$FF
		STA		_game_core_state
;		STA		byte_37							; REDUNDANT, never read!
		LDA		#$01
		STA		_mmc5_chr_mode_shadow
		STA		_MMC5_CHR_MODE
		LDA		#$80
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL						; enable nmi, start game
		CLI

; =============== S U B	R O U T	I N E =======================================
_THREAD0_LOOP:
		JSR		_THREAD0_MAIN					; main loop for thread0, gui
		JSR		_wait_for_nmi					; draw and other user stuff
		JMP		_THREAD0_LOOP

; =============== S U B	R O U T	I N E =======================================
_THREAD0_MAIN:
		LDX		_main_sub_idx					; main game state idx, title is 0
		BNE		_game_setup_mode
		LDA		#$00
		STA		_scr_res_idx
		JSR		_ppu_queues_reset
		FJSRA	_title_screen,PRG7,PRG9
		INC		_main_sub_idx
		RTS
_game_setup_mode:								; title menu is 1
		DEX
		BNE		_ingame_mode
		JSR		_ppu_queues_reset
		FJSRA	_game_setup,SRAM,PRG1
		RTS
_ingame_mode:
		JSR		_ppu_queues_reset				; ingame is 2
		FJSRA	_game_vars_init,SRAM,PRG1		; moved to PRG1 instead of PRGA
		JSR		_thread1_init
		FJSRA	_game_start,PRGC,PRGD
		RTS

; moved to system bank from PRGA
;
; =============== S U B	R O U T	I N E =======================================
_city_stats_reset:
		LDA		#$00
		STA		_doing_a_good_job_votes
		STA		_doing_a_bad_job_votes
		STA		_popul_cur
		STA		_popul_cur+1
		STA		_popul_cur+2
		STA		_popul_net_migration
		STA		_popul_net_migration+1
		STA		_popul_net_migration+2
		STA		_popul_last_year
		STA		_popul_last_year+1
		STA		_popul_last_year+2
		STA		_assessed_value
		STA		_assessed_value+1				; FIX, missing nibbles
		STA		_assessed_value+2				; FIX, missing nibbles
		STA		_annual_score_change
		STA		_annual_score_change+1
		MOVWI	_city._score,500
		RTS

; =============== S U B	R O U T	I N E =======================================
; clear RAM area from _tmpE6 ptr X/Y size
;
_clear_buf:
		STY		_tmp9
		LDY		#$00
		TYA
loc_1044D0:
		STA		(_tmpE6),Y
		INY
		BNE		loc_1044D0
		INC		_tmpE7
		DEX
		BNE		loc_1044D0
loc_1044DA:
		STA		(_tmpE6),Y
		INY
		CPY		_tmp9
		BNE		loc_1044DA
		RTS

; =============== S U B	R O U T	I N E =======================================
NMI:
		PHA
		TXA
		PHA
		TYA
		PHA
		LDA		_nmi_busy_flag
		BEQ		.nmi_normal
		CLI
		JSR		_ppu_regs_reload				; dummy nmi cycle when busy,
		JSR		_ppu_scrolls_reload				; loose one full frame here
		JSR		_mmc5_scrolls_reload
		JMP		.nmi_busy
.nmi_normal:
		INC		_nmi_busy_flag					; raise nmi busy flag, test
		LDA		_mmc5_operation_in_progress_flag; if MMC5 operations in progress
		BEQ		.nmi_start						; perform default ppu operations
		LDA		_ppu_mask_shadow				; if render is on or just skip
		AND		#$18							; if nothing to display here
		BEQ		loc_18A193
		LDA		#$02
		STA		_PPU_OAM_DMA
		CLI
		JSR		_ppu_regs_reload
		JSR		_ppu_scrolls_reload
		JSR		_mmc5_scrolls_reload
loc_18A193:
		JMP		.nmi_finish						; finish nmi either way
.nmi_start:
		LDA		#$02
		STA		_PPU_OAM_DMA					; here is a normal nmi operation
		CLI
		LDA		_ppu_pal_dequeue_req			; check if we have ppu buffers to flush
		BEQ		loc_18A1B8						; then send them, palette buffers first
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
		LDA		_scr_mode_idx					; do special additional palette animation
		BNE		loc_18A1DE						; if in a corresponding screen mode
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
		JSR		_ppu_regs_reload				; also check for screen data backup request
		LDA		_wnd_ppu_data_backup_req		; used by the window draw routines to save
		BEQ		loc_18A1EA						; background data when window displayed
		JSR		_ppu_nt_data_backup
		INC		_wnd_cur_line_num				; one line per nmi, so we see roll on/out effect
loc_18A1EA:
		LDA		_ppu_queue_pos_tail				; the rest of the ppu buffers flushes here
		CMP		_ppu_queue_pos_head
		BEQ		loc_18A1F3
		JSR		_ppu_dequeue
loc_18A1F3:
		JSR		_ppu_scrolls_reload				; also reload CHR banks at the title screen
		LDA		_main_sub_idx					; the game setup and ingame modes uses it's own
		BNE		loc_18A213						; CHR setup code
		LDA		_mmc5_chr_banks_shadow
		STA		_MMC5_CHR_BANKSA
		LDA		_mmc5_chr_banks_shadow+1
		STA		_MMC5_CHR_BANKSA+1
		LDA		_mmc5_chr_banks_shadow+2
		STA		_MMC5_CHR_BANKSA+2
		LDA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
loc_18A213:
		JSR		_mmc5_scrolls_reload			; do scrolls
; FIX, better screen refres sinc, less visible transition between nt and extnt
		LDA		_ppu_mask_shadow
		AND		#$18
		BEQ		loc_18A356
		JSR		_wait_for_mmc5_irq
loc_18A356:
		JSR		_extnt_dequeue
; -
		TSX
		LDY		_cur_thread_idx					; now the thread stuff starting
		TXA										; get the index of current executing thread.
		STA		_thread_stack,Y					; store it's stack and MMC5 parameters
		LDA		_prg_prg8000_shadow				; so we flushed the thread context
		STA		_thread_prg8000,Y
		LDA		_prg_prgA000_shadow
		STA		_thread_prgA000,Y
		LDA		_sram_state_flag
		STA		_thread_sram_state,Y
		JSR		_get_input						; get input here
		INC		_frames_counter					; do actual frame counters. they only
		INC		_frames_div8_counter			; increment if full nmi occurs. doesn't
		LDA		_frames_div8_counter			; changes if nmi is busy or skipped
		AND		#$07
		STA		_frames_div8_counter
		INC		_frames_div4_counter
		LDA		_frames_div4_counter
		AND		#$0F
		STA		_frames_div4_counter
		LDA		#$00
		STA		_spr_buf_pos					; check if we need to draw cursor
		LDA		_scr_mode_idx					; test if we currently ingame
		BEQ		.do_ingame_work					; jump directly to ingame handlers then
; BUG! here, this var never cleared upon exiting of any screen
; resources, it may be either zero/non zero during the game
; but only after entering the info maps mode it will be 1
; so we get here any time even just open the hint message window
; and if we visited info maps recently and not opened any
; other screens before that, the cursor will stay on the
; screen with incorrect CHR bank data every time
; you press a button.
; FIX, added forced value reinitialization after quitting
; the game info screens. Now work properly.
;
		LDA		_scr_res_idx					; if in any other mode, test if we
		CMP		#$01							; at the info screens
		BNE		.skip_ingame_work				; skip if no
		LDA		_cur_info_maps_button_idx		; also skip if any other maps except
		BNE		.skip_ingame_work				; overall city maps are displayed
.do_ingame_work:
		PUSHB	_prg_prg8000_shadow				; save prg banks for now
		PUSHB	_prg_prgA000_shadow
		LDA		#$F5
		STA		_prg_prg8000_shadow				; set default obj handlers bank 5+D
		STA		_MMC5_PRG_PAGES+1
		LDA		#$FD
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_scr_mode_idx					; when we got here
		CMP		#$01
		BEQ		loc_18A284						; for all modes except 1 draw cursor
		PHA
		JSR		_spr_game_interface_cursor_draw
		PLA
		BNE		.skip_goto
		JSR		_goto_alert_spr_draw			; only ingame (_scr_mode_idx = 0) also draw "GOTO"
.skip_goto:
		LDA		_obj_active_flags
		BEQ		loc_18A284						; draw and move objects if any as well,
		JSR		_obj_handlers
loc_18A284:
;		LDA		_scr_mode_idx					; REDUNDANT, some kind of leftovers without
;		BEQ		loc_18A28D						; an actual work, two checks doing nothing
;		LDA		_obj_active_flags
;		BNE		loc_18A28D						; expecially this one lol
;loc_18A28D:
		LDA		_scr_res_window_opened_flag		; all modes except displaying a messages
		BNE		.skip_spr_finish				; will close sprites here
		JSR		_spr_finish
.skip_spr_finish:
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
.skip_ingame_work:
		LDA		_game_wnd_spr_mode_flag			; here we do the threads timers and
		BNE		loc_18A2D6						; priority checks
		LDA		_city._game_speed				; game setting "STOP" test
		CMP		#$03
		BNE		loc_18A2B4						; if enabled, stop core timer
		LDA		#$00
		STA		_game_core_stop_flag
		BEQ		loc_18A2D6
loc_18A2B4:
		LDA		_game_core_stop_flag			; for other speeds do increment if not
		BNE		loc_18A2BC						; stopped somewhere outside
		INC		_game_core_timer
loc_18A2BC:
		LDA		_game_core_timer				; mask timer bits according to selected game speed
		LDX		_city._game_speed
		BNE		loc_18A2CA						; "SLOW" uses 7 bit counter, 128 frames for thread
		AND		#$7F
		BNE		loc_18A2D6						; if timer is zero we need to switch THREADS
		BEQ		loc_18A2CE
loc_18A2CA:
		AND		#$1F							; both "NORMAL" and "FAST" options are the same
		BNE		loc_18A2D6						; counts only 5 bits, 32 frames for thread
loc_18A2CE:
		LDA		#$FF
		STA		_game_core_stop_flag
		STA		_game_core_state
loc_18A2D6:
		LDA		_thread_priority_req			; here is critical sections tests
		BPL		loc_18A2FC						; preventing threads from switching
		AND		#$7F							; if one of them requesting the critical
		TAY										; section,
		LDX		#$00
		CMP		#$08
		BCC		loc_18A2E5						; first 3 bits are for THREAD0 to THREAD1 signals
		INX										; the rest are for THREAD1 exclusive signals
loc_18A2E5:
		LDA		_bitmasks,Y						; test if threads has these bit locked
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
		LDY		_cur_info_maps_button_idx
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
		STY		_mini_map_need_redraw_flag
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
; BUG, too much time between dequeue processess, visible garbage on the screen
;		LDA		_ppu_mask_shadow
;		AND		#$18
;		BEQ		loc_18A356
;		JSR		_wait_for_mmc5_irq
;loc_18A356:
;		JSR		_extnt_dequeue
; -
.nmi_finish:
		DEC		_nmi_busy_flag
.nmi_busy:
;		LDA		#$FA							; REDUNDANT, the rest of the SE code moved
;		STA		_MMC5_PRG_PAGES+1				; to C000 to DPCM samples
		LDA		#$F0
		STA		_MMC5_PRG_PAGES+2
		JSR		_apu_play
;		LDA		_prg_prg8000_shadow				; REDUNDANT, may be used in future to link
;		STA		_MMC5_PRG_PAGES+1				; 16K sound code/data instead of current 8K, if any
		LDA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_cur_thread_idx					; reload MULs only when switch to THREAD1
		BEQ		loc_18A382						; thread 0 should be MUL-safe, only using
		LDA		_mmc5_mul1_shadow				; need to check carefully. the THREAD0 code
		STA		_MMC5_MUL1						; definitely does not care about MUL params
		LDA		_mmc5_mul0_shadow				; but muls can be changed by THREAD1 instead... hmm
		STA		_MMC5_MUL0
loc_18A382:
		LDA		_earthquake_shake_req
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
		LDA		_pad0_held						; soft-reset test
		AND		#$F0
		CMP		#$F0
		BNE		loc_18A3BD
		JMP		RESET
loc_18A3BD:
		INC		_nmi_frames_timer				; raw timer, increment every frame unconditionally
		LDA		_nmi_frames_timer				; 60 frames mean 1 second to pass
		CMP		#$3C
		BCC		loc_18A3CF
		INC		_nmi_seconds_timer				; increment seconds timer, reset frames timer
		LDA		#$00
		STA		_nmi_frames_timer
loc_18A3CF:
		LDA		#$FF
		STA		_nmi_flag						; set nmi is over flag for sync purposes
		PLA
		TAY
		PLA
		TAX
		PLA
		RTI

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
		LDA		_cur_screen_tile._COL
		ASL
		ASL
		LDX		_game_msg_wnd_active_flag
		BNE		loc_18A44B
		LDX		_earthquake_shake_req
		BEQ		loc_18A44B
		STA		_ptr15
		LDA		_frames_counter
		AND		#$03
		BNE		loc_18A438
		DEC		_earthquake_shake_req
loc_18A438:
		LDA		_frames_counter
		AND		#$0F
		CMP		#$08
		LDA		_ptr15
		BCS		loc_18A448
		SEC
		SBC		#$02
		JMP		loc_18A44B
loc_18A448:
		CLC
		ADC		#$02
loc_18A44B:
		TAX
		LDA		_cur_screen_tile._ROW
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
		STA		_irq_sub_idx
		LDA		#$1E
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		RTS
loc_18A48B:
		CMP		#$03
		BEQ		loc_18A49E
		LDA		#$02
		STA		_irq_sub_idx
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
		LDA		_MMC5_IRQ_ENABLE				; irq clear
		LDA		_irq_sub_idx
		BEQ		.irq_mode0
		CMP		#$01
		BEQ		.irq_mode1
		CMP		#$02
		BEQ		.irq_mode2
.irq_exit:
		PLA
		TAY
		PLA
		TAX
		PLA
		RTI
.irq_mode0:
		JSR		_delay							; very precise timing here, need to be adjusted
		JSR		_delay							; somehow if overall timing is changes
		JSR		_delay							; FIX: need more delay here
		LDA		_scroll_msg_shift_pos			; starting line of the scroller message
		LDX		#$00
		STA		_PPU_SCROLL
		STX		_PPU_SCROLL
		LDA		#$01
		STA		_irq_sub_idx					; irq chaining
		LDA		#$26
		STA		_MMC5_SP_PAGE+1
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE				; irq enable
		JMP		.irq_exit

; =============== S U B	R O U T	I N E =======================================
; just to skip some clocks...
; enter/exit takes 6 CLK, JMP - 3 CLK total 15 CLKs
;
_delay:
		JMP		locret_18A4E3
locret_18A4E3:
		RTS

; =============== S U B	R O U T	I N E =======================================
.irq_mode1:
		JSR		_delay							; scrolling message ends here
		JSR		_ppu_scrolls_reload
		LDA		#$02
		STA		_irq_sub_idx					; the rest of the screen
		LDA		#$DE
		STA		_MMC5_SP_PAGE+1					; restore chr page
		LDA		#$80
		STA		_MMC5_IRQ_ENABLE
		JMP		.irq_exit

; =============== S U B	R O U T	I N E =======================================
.irq_mode2:
		LDA		#$00							; start the game screen
		STA		_MMC5_IRQ_ENABLE
		LDA		_APU_STATUS
		AND		#$10
		BNE		loc_18A50A						; dmc adjust
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
		JMP		.irq_exit

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
		LDA		_earthquake_shake_req
		BNE		loc_18A557
loc_18A54E:
		LDX		_input_forbid_delay
		BEQ		loc_18A560
		DEX
		STX		_input_forbid_delay
loc_18A557:
		LDA		#$00
		STA		_pad0_held
		STA		_pad0_press
		STA		_pad0_autorep
		RTS
loc_18A560:
		LDX		#$01
		JSR		_read_pad
		STA		_ptr15
		JSR		_read_pad
		CMP		_ptr15
		BNE		loc_18A560
		STA		_pad1_prev_held
		LDX		#$00
		LDA		_pad0_held
		STA		_pad0_prev_held
		JSR		_read_pad
		STA		_ptr15
loc_18A57B:
		JSR		_read_pad
		CMP		_ptr15
		BEQ		loc_18A587
		STA		_ptr15
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
		AND		_ptr15
		STA		_pad0_press
		STA		_pad0_autorep
		CMP		#$00
		BEQ		loc_18A5C8
		LDA		#$08
		STA		_pad0_autorep_timer
		JMP		loc_18A5C8
loc_18A5AE:
		LDA		#$00
		STA		_pad0_press
		DEC		_pad0_autorep_timer
		BNE		loc_18A5BD
		LDA		_pad0_autorep_delay
		STA		_pad0_autorep_timer
		LDA		_ptr15
loc_18A5BD:
		STA		_pad0_autorep
		JMP		loc_18A5C8
loc_18A5C2:
		LDA		#$00
		STA		_pad0_press
		STA		_pad0_autorep
loc_18A5C8:
		LDA		_ptr15
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

; ---------------------------------------------------------------------------
; ------------------P-P-U--D-E-Q-U-E-U-E--R-O-U-T-I-N-E-S--------------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
_ppu_dequeue:
		LDA		_scr_mode_idx
		BNE		loc_18A6C1
		LDA		_game_wnd_spr_mode_flag
		BMI		loc_18A6C1
		LDA		_game_msg_wnd_active_flag
		BNE		loc_18A6C1
		LDA		_earthquake_shake_req
		BEQ		loc_18A6C1
		RTS
loc_18A6C1:
		LDX		_ppu_queue_pos_tail
		LDA		#$02
		STA		_ptr15
loc_18A6C7:
		CPX		_ppu_queue_pos_head
		BEQ		loc_18A6FB
		DEC		_ptr15
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
		LDA		_earthquake_shake_req
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
		STA		_ptr15
		STY		_ptr15+1
		LDY		#$00
		LDA		_extnt_queue_buf,X
		BPL		loc_18A739
		AND		#$7F
		LDY		#$FF
loc_18A739:
		INX
		STY		_ptr18+1
		STA		_ptr18
		LDY		#$00
loc_18A740:
		LDA		_extnt_queue_buf,X
		INX
		STA		(_ptr15),Y
		JSR		_inc_word15_v_h
		DEC		_ptr18
		BNE		loc_18A740
		BEQ		loc_18A71A
loc_18A74F:
		STX		_extnt_queue_pos_tail
		RTS

; =============== S U B	R O U T	I N E =======================================
_inc_word15_v_h:
		LDA		_ptr18+1
		BNE		loc_18A866
		INCW	_ptr15
		RTS
loc_18A866:
		LDA		_ptr15
		CLC
		ADC		#$20
		STA		_ptr15
		LDA		_ptr15+1
		ADC		#$00
		STA		_ptr15+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_queues_reset:
		LDA		#$00
		STA		_ppu_queue_pos_tail
		STA		_ppu_queue_pos_head
		STA		_extnt_queue_pos_tail
		STA		_extnt_queue_pos_head
		RTS

; -
; HERE LIES ALL GALAXY OF VARIOUS PPU QUEUE SYNC ROUTINES! AMEN
; -

; =============== S U B	R O U T	I N E =======================================
; ok, here we test buffers only
;
_buffers_test_flush:
		LDA		_ppu_queue_pos_tail
		CMP		_ppu_queue_pos_head
		BNE		locret_24A6E
		LDA		_extnt_queue_pos_tail
		CMP		_extnt_queue_pos_head
locret_24A6E:
		RTS

; force dequeue functions
;
; =============== S U B	R O U T	I N E =======================================
; force flush buffers until empty
;
_buffers_force_flush:
		JSR		_ppu_dequeue
		JSR		_extnt_dequeue
		JSR		_buffers_test_flush
		BNE		_buffers_force_flush
		RTS

; =============== S U B	R O U T	I N E =======================================
; REDUNDANT, exactly the same function
;_wait_for_force_flush:
;		JSR		_ppu_dequeue					; force flush buffers, render should be off!
;		JSR		_extnt_dequeue
;		JSR		_buffers_test_flush
;		BNE		_wait_for_force_flush
;		RTS

; =============== S U B	R O U T	I N E =======================================
; a little bit optimized for speed function, but seems the actual benefit
; very small, we may drop this function
; REDUNDANT, merged
;_buffers_force_flush:
;		JSR		_ppu_dequeue					; do dequeue of nt data
;loc_EB043:
;		JSR		_extnt_dequeue					; then extnt
;		LDA		_ppu_queue_pos_tail				; if nt not flushed, repeat both
;		CMP		_ppu_queue_pos_head
;		BNE		_buffers_force_flush			; if flushed, here only extnt may
;		LDA		_extnt_queue_pos_tail			; need to flush, so let's optimize
;		CMP		_extnt_queue_pos_head			; it and jump only to extnt dequeue
;		BNE		loc_EB043
;		RTS

; wait dequeue functions
;
; =============== S U B	R O U T	I N E =======================================
; wait for nmi to flush buffers, then wait until empty
; may take a couple of nmi cycles
;
_buffers_wait_flush:
		JSR		_wait_for_nmi

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
; little bit optimized function to test empty buffers first, then wait
; if not empty, may save a little bit oc CPU time
;
_buffers_test_wait_flush:
		JSR		_buffers_test_flush
		BNE		_buffers_wait_flush
		RTS

; =============== S U B	R O U T	I N E =======================================
; functionaly this entry the same as the previous one
;
;_flush_wait:
;		JSR		_wait_for_nmi
;
; !FALLTHROUGH!
;
; =============== S U B	R O U T	I N E =======================================
; again a little bit optimized function, if buffers are empty, no need
; to wait for nmi... may get some speedup depending on where used
;
;_flush_test_wait:
;		LDA		_extnt_queue_pos_head
;		CMP		_extnt_queue_pos_tail
;		BNE		_flush_wait
;		LDA		_ppu_queue_pos_head
;		CMP		_ppu_queue_pos_tail
;		BNE		_flush_wait
;		RTS

; exactly the same routine from another bank
;
; =============== S U B	R O U T	I N E =======================================
;_buffers_wait_flush_0:
;		JSR		_wait_for_nmi
;
; !FALLTHROUGH!
;
; =============== S U B	R O U T	I N E =======================================
;_flush_test_wait_0:
;		LDA		_extnt_queue_pos_head
;		CMP		_extnt_queue_pos_tail
;		BNE		_buffers_wait_flush_0
;		LDA		_ppu_queue_pos_head
;		CMP		_ppu_queue_pos_tail
;		BNE		_buffers_wait_flush_0
;		RTS

; =============== S U B	R O U T	I N E =======================================
; PRG3 routine, uses manualy flag setting, but only used once in loop waiting
; for buffers are empty
; REDUNDANT now
;_buffers_test_flush:
;		LDA		_ppu_queue_pos_tail
;		CMP		_ppu_queue_pos_head
;		BNE		loc_58CD4
;		LDA		_extnt_queue_pos_tail
;		CMP		_extnt_queue_pos_head
;		BNE		loc_58CD4
;		CLC
;		RTS
;loc_58CD4:
;		SEC
;		RTS

; =============== S U B	R O U T	I N E =======================================
; PRGC routine the same as PRG3 above
; REDUNDANT, merged
;_buffers_test_flush_0:
;		LDA		_ppu_queue_pos_tail
;		CMP		_ppu_queue_pos_head
;		BNE		loc_137D1E
;		LDA		_extnt_queue_pos_tail
;		CMP		_extnt_queue_pos_head
;		BNE		loc_137D1E
;		CLC
;		RTS
;loc_137D1E:
;		SEC
;		RTS

; ---------------------------------------------------------------------------
; ---------------------M-O-R-E--M-I-S-C--H-E-L-P-E-R-S-----------------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
_sram_enable_restore:
		LDA		_sram_state_flag
		BNE		_sram_write_enable
		BEQ		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_sram_write_enable:
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
_sram_write_disable:
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
		DEC		_mmc5_operation_in_progress_flag; raise busy flag
		STA		_tmp3							; backup registers
		STX		_tmp4
		STY		_tmp5
		POPB	_ptr0							; get the data offset from stack
		CLC
		ADC		#$04
		STA		_tmp2							; adjust offset to next valid opcode
		POPB	_ptr0+1
		ADC		#$00
		PHA										; and push it back, not will return
		PUSHB	_tmp2							; as usual by RTS
		PUSHB	_prg_prg8000_shadow				; backup previous prg banks
		PUSHB	_prg_prgA000_shadow
		LDY		#$01
		LDA		(_ptr0),Y						; fetch prg banks, store to stack for now
		INY
		PHA
		LDA		(_ptr0),Y
		INY
		PHA
		LDA		(_ptr0),Y						; fetch routine offset
		INY
		TAX
		LDA		(_ptr0),Y
		STA		_ptr0+1
		STX		_ptr0
		POPB	_prg_prgA000_shadow				; apply new prg banks
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		_tmp3							; load registers / arguments
		LDX		_tmp4
		LDY		_tmp5
		INC		_mmc5_operation_in_progress_flag; busy off
		JSR		_jmp_ptr0						; far jump
		DEC		_mmc5_operation_in_progress_flag; busy again
		STA		_tmp3							; store result regs
		STX		_tmp4
		STY		_tmp5
		POPB	_prg_prgA000_shadow				; restore prg for caller
		STA		_MMC5_PRG_PAGES+2
		POPB	_prg_prg8000_shadow
		STA		_MMC5_PRG_PAGES+1
		LDA		_tmp3							; load results in regs
		LDX		_tmp4
		LDY		_tmp5
		INC		_mmc5_operation_in_progress_flag; busy off, exit
		RTS

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;_jmp_ptr0:
;		JMP		(_ptr0)

; =============== S U B	R O U T	I N E =======================================
; little bit simpler routine, no interferences with tmp0-tmp5, isolated
; tmp vars, but only X,Y stored as arguments, without returning results
; at all...
;
_farcall_argsXY_safe:
		STX		_ptrEE
		STY		_ptrEE+1
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
; REDUNDANT, there were some additional check before, but for some reasons
; they forgot to remove the compare value. this routine used only for
; far call between banks A/B so there must be some optimization check before
; or unfinished.
;
;		CMP		#$FB
; -
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
		LDX		_ptrEE
		LDY		_ptrEE+1
		INC		_mmc5_operation_in_progress_flag
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
		JMP		(_ptrDC)

; =============== S U B	R O U T	I N E =======================================
_pal_ppu_send:
		JSR		_ppu_set_hor_increment			; set ppu increment 1
		LSR										; A - dequeue flags argument
		BCC		loc_18A881						; bit0 - dequeue sprite pal
		PHA
		LDY		#$10
		JSR		_pal_ppu_buf_send
		PLA
loc_18A881:
		LSR
		BCC		locret_18A889					; bit1 - dequeue bg bap
		LDY		#$00
		JMP		_pal_ppu_buf_send
locret_18A889:
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_ppu_buf_send:
		LDX		#$3F							; set ppu hi pal offset
		STX		_PPU_ADDR
		STY		_PPU_ADDR						; lower nibble already in Y as argument
		LDX		#$10
loc_18A894:
		LDA		_pal_bg_buf,Y					; sent 16 bytes
		STA		_PPU_DATA
		INY
		DEX
		BNE		loc_18A894
		LDA		#$3F
		STA		_PPU_ADDR						; reset ppu offs reg
		LDA		#$00
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		STA		_PPU_ADDR
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_load_lib:
		AND		#$FF							; if higher bit is set, then load sprite pal
		BMI		loc_18A8CA						; otherwise load bg pal
		LDX		#$00
		STX		_ptr0+1
		ASL
		ASL
		ASL
		ASL
		ROL		_ptr0+1
		CLC
		ADC		#<[_pal_bg_lib]					; bg pal ofs = (idx * 16) + _pal_bg_lib
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
		ADC		#<[_pal_spr_lib]				; spr pal ofs = ((idx & $7F) * 16) + _pal_spr_lib
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
		STA		_pal_bg_buf,X					; load pal buf
		INX
		DEC		_tmp2
		BNE		loc_18A8E3
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_fade_in:
		PUSHB	_game_core_state				; lock THREAD1 while fading
		LDA		#$01
		STA		_game_core_state
		LDA		_screen_pal_cur_bg_idx			; load desired bg pal
		JSR		_pal_load_lib
		LDA		_screen_pal_cur_spr_idx			; load the same idx spr pal
		ORA		#$80
		JSR		_pal_load_lib
		JSR		_pal_cut_off_half				; cut off palette to lower nibbles
		LDA		#$04
		STA		_ptr0
loc_18A90D:
		LDA		#$05							; fade delay 5 frames
		STA		_ptr0+1
		LDA		#$03
		STA		_ppu_pal_dequeue_req			; set dequeue flag once for both pals
loc_18A916:
		JSR		_wait_for_nmi					; wait for dequeue, also delay
		DEC		_ptr0+1
		BNE		loc_18A916
		JSR		_pal_buf_fade_in				; do fade in
		DEC		_ptr0
		BNE		loc_18A90D						; total 4 steps, 4*5 = 20 frames to fade in
		POPB	_game_core_state				; 1/3 second
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_cut_off_half:
		LDX		#$1F							; mask all palette values to have only lower nibble
loc_18A92B:										; so we may keep BLACK intact and
		LDA		_pal_bg_buf,X					; have lower shades of colors fot the rest
		AND		#$0F							; or also black
		STA		_pal_bg_buf,X
		DEX
		BPL		loc_18A92B
		RTS

; =============== S U B	R O U T	I N E =======================================
_pal_buf_fade_in:
		LDA		#$00							; fade in also fetches ptrs for both palettes
		STA		_tmp3
		LDA		_screen_pal_cur_bg_idx
		ASL
		ASL
		ASL
		ASL
		ROL		_tmp3
		CLC
		ADC		#<[_pal_bg_lib]					; again get the real palette ofs
		STA		_tmp2
		LDA		#>[_pal_bg_lib]
		ADC		_tmp3
		STA		_tmp3
		MOVWO	_tmp4,_pal_bg_buf				; because pal bg buffer cut out
		JSR		__pal_buf_fade_in				; now do fade in
		LDA		_screen_pal_cur_spr_idx			; the same for spr
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
;		JMP		__pal_buf_fade_in				; REDUNDANT

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
__pal_buf_fade_in:
		LDX		#$10							; actual fade in,
		LDY		#$00							; increment high nibble up to
loc_18A97B:
		LDA		(_tmp4),Y						; 4 times (0F-3F)
		CMP		(_tmp2),Y						; with clamp to real pal value
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
		PUSHB	_game_core_state				; fade out mostly tha same
		LDA		#$01
		STA		_game_core_state
		JSR		_pal_buf_fade_out				; but no need to get the actual
		LDA		#$04							; palette offsets one more time
		STA		_ptr0
loc_18A99B:
		LDA		#$05
		STA		_ptr0+1
		LDA		#$03
		STA		_ppu_pal_dequeue_req			; just decrease 4 times hich nibble
loc_18A9A4:
		JSR		_wait_for_nmi					; wait for 5 frames with pal dequeue flags set
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
		LDA		_pal_bg_buf,X					; decrease high nibble with
		SEC										; clamp to $0F al lowest value
		SBC		#$10							; (black)
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
_city_map_init:
		JSR		_sram_write_enable
		LDX		#$00
		MOVWO	_ptr0,_city_map					; map buf ptr
		MOVWI	_tmp2,$1690						; map buf size
		LDA		#$90							; fill with bulldozered grounds
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
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_ppu_reset:
		LDA		_PPU_STATUS
		LDA		#$00
		STA		_PPU_SCROLL
		STA		_PPU_SCROLL
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_nt_data_backup:
		JSR		_sram_write_enable
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
		STA		_nt_backup1,X
		JMP		loc_18AA80
loc_18AA7A:
		LDA		_PPU_DATA
		STA		_nt_backup0,X
loc_18AA80:
		INX
		DEC		byte_9F
		BNE		loc_18AA6D
		DO_WRAP	byte_9F,byte_A0,loc_18AAAF
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
		JSR		_sram_write_disable
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
		STA		_nt_backup1,X
		JMP		loc_18AAE5
loc_18AADF:
		LDA		_PPU_DATA
		STA		_nt_backup0,X
loc_18AAE5:
		INX
		DEC		byte_9F
		BNE		loc_18AAD2
		DO_WRAP	byte_9F,byte_A0,loc_18AAAF
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
		JSR		_sram_write_enable
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
		DO_WRAP	byte_9F,byte_A0,loc_18AB71
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
		JMP		_sram_write_disable
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
		ADDWB	word_9B,#$20
		DEC		byte_9F
		BNE		loc_18AB7B
		DO_WRAP	byte_9F,byte_A0,loc_18AB71
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		STA		word_9B
		LDA		#>[_MMC5_EXRAM]
		STA		word_9B+1
		JMP		loc_18AB7B

; REDUNDANT, moved to common init code to be able to save it in save states
; as well!
; =============== S U B	R O U T	I N E =======================================
;_city._menu_pos_init:
;		LDX		#$0B
;loc_18ABB5:
;		LDA		_gui_ctrl_menu_pos_data,X		; copy initial positions for
;		STA		_city._menu_pos,X		; game configure windows
;		DEX
;		BPL		loc_18ABB5
;		RTS
;
;_gui_ctrl_menu_pos_data:
;		.BYTE	$02,$07
;		.BYTE	$04,$07
;		.BYTE	$06,$07
;		.BYTE	$08,$07
;		.BYTE	$0A,$07
;		.BYTE	$0C,$07

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
;		JSR		_game_chr_common_reload			; REDUNDANT here
;		LDA		#$34							; REDUNDANT, call routine above
;		STA		_mmc5_chr_banks_shadow
;		STA		_MMC5_CHR_BANKSA				; instead.
;		LDA		#$35
;		STA		_mmc5_chr_banks_shadow+1
;		STA		_MMC5_CHR_BANKSA+1
;		LDA		#$3A							; REDUNDANT
;		STA		_mmc5_chr_banks_shadow+2
;		STA		_MMC5_CHR_BANKSA+2
;		LDA		#$37
;		STA		_mmc5_chr_banks_shadow+3
;		STA		_MMC5_CHR_BANKSA+3
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_dpcm_forbid:
		LDA		_APU_STATUS
		CMP		_APU_STATUS
		BNE		_apu_dpcm_forbid
		AND		#$0F
		STA		_APU_STATUS
		LDA		_apu_dpcm_disable_flag
		ORA		#$80
		STA		_apu_dpcm_disable_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_dpcm_allow:
		LDA		_apu_dpcm_disable_flag
		AND		#$7F
		STA		_apu_dpcm_disable_flag
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
		LDA		_goto_in_progress_delay
		BNE		loc_18AEA8
		LDA		#$00
		BEQ		loc_18AEB4
loc_18AEA8:
		LDA		#$02
		DEC		_goto_in_progress_delay
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
_spr_finish:
		LDX		_spr_buf_pos
		BNE		loc_18A763

; !FALLTHROUGH!

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
;		DEC		_spr_insert_args._pos._ROW
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
;		ADC		_spr_insert_args._pos._ROW
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
;		ADC		_spr_insert_args._pos._COL
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
;		DEC		_spr_insert_args._pos._ROW
;		LDX		_spr_buf_pos
;		LDY		#$00
;loc_18A666:
;		LDA		(_ptr0),Y
;		CMP		#$80
;		BEQ		loc_18A6A4
;		LDA		(_ptr0),Y
;		INY
;		CLC
;		ADC		_spr_insert_args._pos._ROW
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
;		ADC		_spr_insert_args._pos._COL
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
; sprite data for this routine in PRG2
;
_spr_lib_attr_insert:
		PUSHB	_prg_prgA000_shadow
		LDA		#$F2
		STA		_MMC5_PRG_PAGES+2
		LDA		#$00							; clip no, attrs yes
		JSR		_spr_lib_insert_ex
		POPB	_MMC5_PRG_PAGES+2
		RTS

; =============== S U B	R O U T	I N E =======================================
; sprite data for this routine in PRG8
;
_spr_lib_clip_insert:
		PUSHB	_prg_prg8000_shadow
		LDA		#$F8
		STA		_MMC5_PRG_PAGES+1
		LDA		#$00							; attrs no
		STA		_spr_insert_args._attr
		LDA		#$80							; clip yes
		JSR		_spr_lib_insert_ex
		POPB	_MMC5_PRG_PAGES+1
		RTS

; =============== S U B	R O U T	I N E =======================================
; Common sprite insert routine for all different kind of sprites.
;
; the rest of common sprites are here in PRGF
;
_spr_lib_insert_ex:
		STA		_tmp17
		LDX		_spr_insert_args._idx
		LDA		_spr_lib_lo,X
		STA		_ptr0
		LDA		_spr_lib_hi,X
		STA		_ptr0+1
		DEC		_spr_insert_args._pos._ROW
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
		ADC		_spr_insert_args._pos._ROW
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
		ADC		_spr_insert_args._pos._COL
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
		.BYTE	<[_spr00]		; generic HAND cursor
		.BYTE	<[_spr01]		; top-left build frame corner
		.BYTE	<[_spr02]		; top-right build frame corner
		.BYTE	<[_spr03]		; bottom-left build frame corner
		.BYTE	<[_spr04]		; bottom-right build frame corner
		.BYTE	<[_spr05]		; options STAR cursor
		.BYTE	<[_spr06]		; mini-map
		.BYTE	<[_spr07]		; mini-map cursor
		.BYTE	<[_spr08]		; map scroll arrow UP
		.BYTE	<[_spr09]		; map scroll arrow DOWN
		.BYTE	<[_spr0A]		; map scroll arrow LEFT
		.BYTE	<[_spr0B]		; map scroll arrow RIGHT
		.BYTE	<[_spr0C]		; push icon YES
		.BYTE	<[_spr0D]		; push icon NO
		.BYTE	<[_spr0E]		; frame *unused*
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
		.BYTE	<[_spr2C]		; Mayor *unused*
		.BYTE	<[_spr2D]		; Mayor *unused*
		.BYTE	<[_spr2E]
		.BYTE	<[_spr2F]
		.BYTE	<[_spr30]
		.BYTE	<[_spr31]
		.BYTE	<[_spr32]
		.BYTE	<[_spr33]
		.BYTE	<[_spr34]
		.BYTE	<[_spr35]		; Mayor *unused*
		.BYTE	<[_spr36]		; Mayor *unused*
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

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;_ppu_increment_select:
;		PHA
;		LDA		_ppu_ctrl_shadow
;		ORA		#$04
;		BCS		loc_18BBAC
;		AND		#$FB
;loc_18BBAC:
;		STA		_PPU_CTRL
;		PLA
;		RTS

; =============== S U B	R O U T	I N E =======================================
_res_ppu_unpack:
		STX		_ptr0
		STY		_ptr0+1
_res_ppu_unpack_loop:
		LDY		#$00
		LDA		(_ptr0),Y						; 00 - end of stream code
		BNE		loc_18BBAB
		JMP		_ppu_reset
loc_18BBAB:
		STA		_PPU_ADDR
		INY
		LDA		(_ptr0),Y
		STA		_PPU_ADDR
		INY
		LDA		(_ptr0),Y						; extract higher bit - vertical store flag
		ASL
;		JSR		_ppu_increment_select			; REDUNDANT
		PHA										; OPTIMIZED
		LDA		_ppu_ctrl_shadow
		ORA		#$04
		BCS		loc_18BBAC						; setup the ppu increment bit accordingly
		AND		#$FB
loc_18BBAC:
		STA		_PPU_CTRL
		PLA
		ASL										; restore code, extract another control bit
		LDA		(_ptr0),Y						; reload command
		AND		#$3F							; extract repeat counter
		TAX										; now C=0 is copy mode, C=1 is repeat mode
		BCC		.send_loop						; very tricky, increment Y if C=1
		INY										; as well as increment Y if C=0 lol
.send_loop:										; the same time being a increment Y for loop
		BCS		.inc_skip						; or not being for loop
		INY										; GENIUS! the same code for repeat and copy data!
.inc_skip:
		LDA		(_ptr0),Y
		STA		_PPU_DATA
		DEX
		BNE		.send_loop
		INY
		TYA
		JSR		_adc_ptr0
		JMP		_res_ppu_unpack_loop

; =============== S U B	R O U T	I N E =======================================
_res_ppu_lz_unpack:
		JSR		_apu_dpcm_forbid
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
		STA		_tmp5AE
loc_18AC59:
		INY
		LDA		(_ptr0),Y
		STA		_tmp6
		INY
		LDA		(_ptr0),Y
		STA		_tmp6+1
		TYA
		PHA
		JSR		_ppu_lz_unpack
		PLA
		TAY
		DEC		_tmp5AE
		BNE		loc_18AC59
		JMP		_apu_dpcm_allow

; =============== S U B	R O U T	I N E =======================================
_ppu_lz_unpack:
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
		STX		_tmp5AD
		LDX		word_5A9
		BNE		loc_18ACD8
		DEC		_tmp5AD
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
		DEC		_tmp5AD
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
		DEC		_tmp5AD
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
		DEC		_tmp5AD
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
		DEC		_tmp5AD
		BPL		loc_18AD45
		JMP		loc_18ACF6
loc_18AD56:
		PHA
		LDA		(_tmp6),Y
		INY
		STA		_tmp5AC
		LDA		(_tmp6),Y
		INY
		CLC
		ADC		_tmp4
		STA		_tmp5AB
		LDA		_tmp5
		ADC		_tmp5AC
		STA		_tmp5AC
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
		JSR		_get_lz_pos
		JSR		_get_lz_reverse_date
		JSR		_send_lz_data
		INC		_tmp5AC
		DEC		word_5A9+1
		BPL		loc_18AD8D
		JMP		loc_18AC7A
loc_18ADA1:
		JSR		_get_lz_pos
		JSR		_get_lz_reverse_date
		JSR		_get_lz_rotate_data
		JSR		_send_lz_data
		INC		_tmp5AC
		DEC		word_5A9+1
		BPL		loc_18ADA1
		JMP		loc_18AC7A
loc_18ADB8:
		JSR		_get_lz_pos
		JSR		_get_lz_raw_date
		JSR		_send_lz_data
		DEC		word_5A9+1
		BPL		loc_18ADB8
		JMP		loc_18AC7A

; =============== S U B	R O U T	I N E =======================================
_get_lz_pos:
		LDX		word_5A9
		LDA		word_5A9+1
		BEQ		loc_18ADD3
		LDX		#$00
loc_18ADD3:
		STX		_tmp5AF
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_lz_reverse_date:
		LDA		_tmp5AC
		STA		_PPU_ADDR
		LDA		_tmp5AB
		STA		_PPU_ADDR
		LDA		_PPU_DATA
loc_18ADE6:
		LDA		_PPU_DATA
		STA		_ppu_queue_buf,X
		DEX
		BNE		loc_18ADE6
		RTS

; =============== S U B	R O U T	I N E =======================================
_send_lz_data:
		LDX		_tmp5AF
		LDA		_tmp3
		STA		_PPU_ADDR
		LDA		_tmp2
		STA		_PPU_ADDR
loc_18ADFD:
		LDA		_ppu_queue_buf,X
		STA		_PPU_DATA
		DEX
		BNE		loc_18ADFD
		DEC		_tmp5AF
		LDA		_tmp5AF
		SEC
		ADC		_tmp2
		STA		_tmp2
		BCC		locret_18AE15
		INC		_tmp3
locret_18AE15:
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_lz_rotate_data:
		LDX		_tmp5AF
loc_18AE19:
		LDA		_ppu_queue_buf,X
		STA		_tmp5AD
		LDY		#$08
loc_18AE21:
		ASL		_tmp5AD
		ROR
		DEY
		BNE		loc_18AE21
		STA		_ppu_queue_buf,X
		DEX
		BNE		loc_18AE19
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_lz_raw_date:
		STX		_tmp5AD
		DEC		_tmp5AD
		LDA		_tmp5AB
		CLC
		SBC		_tmp5AD
		STA		_tmp5AB
		TAY
		LDA		_tmp5AC
		SBC		#$00
		STA		_tmp5AC
		STA		_PPU_ADDR
		STY		_PPU_ADDR
		LDA		_PPU_DATA
		LDA		_PPU_DATA
		LDX		#$00
loc_18AE56:
		INX
		LDA		_PPU_DATA
		STA		_ppu_queue_buf,X
		CPX		_tmp5AF
		BNE		loc_18AE56
		RTS

; =============== S U B	R O U T	I N E =======================================
_tlm_lib_queue:
		LDA		_tlm_lib,X
		STA		_ptr0
		LDA		_tlm_lib+1,X
		STA		_ptr0+1
		LDA		_tlm_lib+2,X
		STA		_ptr2
		LDA		_tlm_lib+3,X
		STA		_ptr2+1
		LDY		#$00
		PPUQSTART
loc_B8CE4:
		LDA		(_ptr0),Y
		CMP		#$FF
		BEQ		loc_B8CF7
		PPUQMOVA
		INY
		BNE		loc_B8CE4
loc_B8CF7:
		PPUQEND
		LDY		#$00
		EXTQSTART
loc_B8CFD:
		LDA		(_ptr2),Y
		CMP		#$FF
		BEQ		loc_B8D10
		EXTQMOVA
		INY
		BNE		loc_B8CFD
loc_B8D10:
		EXTQEND
		RTS

_tlm_lib:
		.WORD	_tlm_nt_bank_loan0,_tlm_extnt_bank_loan0			; 00 PRG7
		.WORD	_tlm_nt_bank_loan1,_tlm_extnt_bank_loan1			; 04 PRG7
		.WORD	_tlm_nt_bank_loan2,_tlm_extnt_bank_loan2			; 08 PRG7
		.WORD	_tlm_nt_bank_loan3,_tlm_extnt_bank_loan3			; 0C PRG7
		.WORD	_tlm_nt_bank_loan_def,_tlm_extnt_bank_loan_def		; 10 PRG7
		.WORD	_tlm_nt_new_game_push,_tlm_extnt_new_game_push		; 14 PRG9
		.WORD	_tlm_nt_next_map_push,_tlm_extnt_next_map_push		; 18 PRG9
		.WORD	_tlm_nt_next_map_release,_tlm_extnt_netx_map_release; 1C PRG9
		.WORD	_tlm_nt_bank_icon,_tlm_extnt_bank_icon				; 20 PRG9
		.WORD	_tlm_nt_bank_loan,_tlm_extnt_bank_loan				; 24 PRG9
		.WORD	_tlm_nt_bank_no_loan,_tlm_extnt_bank_no_loan		; 28 PRG9
		.WORD	_tlm_nt_back_button_press,_tlm_extnt_back_button_press		; 2C PRG1
		.WORD	_tlm_nt_space_press,_tlm_extnt_space_press					; 30 PRG1
		.WORD	_tlm_nt_end_press,_tlm_extnt_end_press						; 34 PRG1
		.WORD	_tlm_nt_all_buttons_release,_tlm_extnt_all_buttons_release	; 38 PRG1

; =============== S U B	R O U T	I N E =======================================
; OPTIMIZED, same as _tmp_map_pos_near_tile_select routine buth with C result
; -0-
; 3*1
; -2-
; C=0 if done ok
; C=1 if cannot change
;
_jump_to_near_to_tmp_map_pos_tile:
		CPX		#$00							; we must keep X here, some routines
		BEQ		_tmp_map_dec_pos_Y				; depends on it
		CPX		#$01
		BEQ		_tmp_map_inc_pos_X
		CPX		#$02
		BEQ		_tmp_map_inc_pos_Y
		CPX		#$03
		BEQ		_tmp_map_dec_pos_X
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_back_from_near_tmp_map_pos_tile:
		CPX		#$00							; same here
		BEQ		_tmp_map_inc_pos_Y
		CPX		#$01
		BEQ		_tmp_map_dec_pos_X
		CPX		#$02
		BEQ		_tmp_map_dec_pos_Y
		CPX		#$03
		BEQ		_tmp_map_inc_pos_X
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_dec_pos_Y:
		LDA		_tmp_map_pos._ROW
		BEQ		_tmp_map_cant_change_pos
		DEC		_tmp_map_pos._ROW
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_inc_pos_X:
		LDA		_tmp_map_pos._COL
		CMP		#$4B
		BEQ		_tmp_map_cant_change_pos
		INC		_tmp_map_pos._COL
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_inc_pos_Y:
		LDA		_tmp_map_pos._ROW
		CMP		#$4B
		BEQ		_tmp_map_cant_change_pos
		INC		_tmp_map_pos._ROW
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_dec_pos_X:
		LDA		_tmp_map_pos._COL
		BEQ		_tmp_map_cant_change_pos
		DEC		_tmp_map_pos._COL
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_cant_change_pos:
		SEC
		RTS

; OPTIMIZED, moved here from PRGB
; =============== S U B	R O U T	I N E =======================================
_map_tile_read:
; REDUNDANT, too slow for such frequently used routine
;		PUSHB	_tmp_map_pos._ROW
;		PUSHB	#$4C
;		JSR		_mmc5_mul8to8
;		PLA
;		CLC
;		ADC		#<[_city_map]
;		STA		_ptrEC
;		PLA
;		ADC		#>[_city_map]
;		STA		_ptrEC+1
;		LDY		_tmp_map_pos._COL
; -
; OPTIMIZED, FASTER!
; FIX, even if BUFPOS isn't safe for other muls, it is pretty safe for _city_map
; muls, because uses table lookup method, not the HW mul registers!
;
; WARNING! BUFPOS not X safe macros, most of code aware of it
; but only _map_tile_read routine need to be X-safe, it used very
; frequently and need to check carefully where it may damage X
; so make a workaround for now here. it's look safe for most of the code
; and not so much slower.
;
		TXA
		PHA
		BUFPOS	_ptrEC,_tmp_map_pos._COL,_tmp_map_pos._ROW,#$4C,#>[_city_map]
		PLA
		TAX
		LDY		#$00
		LDA		(_ptrEC),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_rand:
		LDA		_rand_seed						; result of operations here
		STA		_rand_seed+2					; depends on undefined C state
		ADC		_rand_seed+1					; upon entry, so calcs are
		STA		_rand_seed						; semi-random by itself
		ADC		_rand_seed+2
		STA		_rand_seed+1
		RTS

; ADDED, new common helper for added scrollers.
; =============== S U B	R O U T	I N E =======================================
_scroll_message_restart:
		STA		_scroll_msg_idx
		LDY		#$00
		STY		_scroll_msg_start_pos
		DEY
		STY		_scroll_msg_active_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_rand_clamp_A:
		PHA
		JSR		_rand
		PHA
		JSR		_mmc5_mul8to8
		PLA
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_adc_ptr0:
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

; =============== S U B	R O U T	I N E =======================================
; input:
; dividend fixed point 16.16 (integer E8/E9, fraction E6/E7)
; divider  fixed point 8.8   (integer EB, fraction EA)
; result:
; fixed point 8.8 (integer E7, fraction E6)
;
_div_32to16:
		LDA		_tmpEB							; div-by-zero test
		ORA		_tmpEA							; return C=1 if divider = 0
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

; =============== S U B	R O U T	I N E =======================================
_switch:
		ASL
		STA		_ptr0
		SEC
		PLA
		ADC		_ptr0
		STA		_ptr0
		PLA
		ADC		#$00
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		PHA
		INY
		LDA		(_ptr0),Y
		STA		_ptr0+1
		POPB	_ptr0

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_jmp_ptr0:
		JMP		(_ptr0)

_bitmasks:
		.BYTE	$01,$02,$04,$08,$10,$20,$40,$80,$01,$02,$04

_inv_bitmasks:
		.BYTE	$80,$40,$20,$10,$08,$04,$02,$01

; +0 +1 +2 +3
; 00 82 87 8E +00
; 80 81 86 8D +4C
; 83 84 85 8C +98
; 88 89 8A 8B +E4

_special_tiles_ofs_list:
		.BYTE	$4C,$4D,$01
		.BYTE	$98,$99,$9A,$4E,$02
		.BYTE	$E4,$E5,$E6,$E7,$9B,$4F,$03

_buildings_prices_list:
		.WORD	1
		.WORD	10
		.WORD	5
		.WORD	20
		.WORD	10
		.WORD	100
		.WORD	100
		.WORD	100
		.WORD	500
		.WORD	500
		.WORD	3000
		.WORD	3000
		.WORD	5000
		.WORD	100
		.WORD	5000
		.WORD	10000

_months_lib:
		.BYTE	_J,_A,_N,$80
		.BYTE	_F,_E,_B,$80
		.BYTE	_M,_A,_R,$80
		.BYTE	_A,_P,_R,$80
		.BYTE	_M,_A,_Y,$80
		.BYTE	_J,_U,_N,$80
		.BYTE	_J,_U,_L,$80
		.BYTE	_A,_U,_G,$80
		.BYTE	_S,_E,_P,$80
		.BYTE	_O,_C,_T,$80
		.BYTE	_N,_O,_V,$80
		.BYTE	_D,_E,_C,$80

_bg_music_list:
		.BYTE	_MUS_IDX_VILLAGE0
		.BYTE	_MUS_IDX_TOWN
		.BYTE	_MUS_IDX_CITY
		.BYTE	_MUS_IDX_CAPITAL
		.BYTE	_MUS_IDX_METROPOLIS
		.BYTE	_MUS_IDX_MEGAPOLIS0

_dec_digit_tbl0:
		.BYTE	[[1 >> 0] & $FF]		; $01
_hex_to_dec_tbl0:
		.BYTE	[[10 >> 0] & $FF]		; $0A
		.BYTE	[[100 >> 0] & $FF]		; $64
		.BYTE	[[1000 >> 0] & $FF]		; $E8
		.BYTE	[[10000 >> 0] & $FF]	; $10
		.BYTE	[[100000 >> 0] & $FF]	; $A0
		.BYTE	[[1000000 >> 0] & $FF]	; $40
		.BYTE	[[10000000 >> 0] & $FF] ; $80
_dec_digit_tbl1:
		.BYTE	[[1 >> 8] & $FF]		; $00
_hex_to_dec_tbl1:
		.BYTE	[[10 >> 8] & $FF]		; $00
		.BYTE	[[100 >> 8] & $FF]		; $00
		.BYTE	[[1000 >> 8] & $FF]		; $03
		.BYTE	[[10000 >> 8] & $FF]	; $27
		.BYTE	[[100000 >> 8] & $FF]	; $86
		.BYTE	[[1000000 >> 8] & $FF]	; $42
		.BYTE	[[10000000 >> 8] & $FF] ; $96
_dec_digit_tbl2:
		.BYTE	[[1 >> 16] & $FF]		; $00
_hex_to_dec_tbl2:
		.BYTE	[[10 >> 16] & $FF]		; $00
		.BYTE	[[100 >> 16] & $FF]		; $00
		.BYTE	[[1000 >> 16] & $FF]	; $00
		.BYTE	[[10000 >> 16] & $FF]	; $00
		.BYTE	[[100000 >> 16] & $FF]	; $01
		.BYTE	[[1000000 >> 16] & $FF] ; $0F
		.BYTE	[[10000000 >> 16] & $FF]; $98

; OPTIMIZED, moved here to be usable from both PRGA and PRGB by now
_map_tiles_flags_tbl:
; R areas
		.BYTE	_FL_AREA											; 00
		.BYTE	_FL_AREA											; 01
		.BYTE	_FL_AREA											; 02
		.BYTE	_FL_AREA											; 03
		.BYTE	_FL_AREA											; 04
		.BYTE	_FL_AREA											; 05
		.BYTE	_FL_AREA											; 06
		.BYTE	_FL_AREA											; 07
		.BYTE	_FL_AREA											; 08
		.BYTE	_FL_AREA											; 09
		.BYTE	_FL_AREA											; 0A
		.BYTE	_FL_AREA											; 0B
		.BYTE	_FL_AREA											; 0C
		.BYTE	_FL_AREA											; 0D
		.BYTE	_FL_AREA											; 0E
		.BYTE	_FL_AREA											; 0F
		.BYTE	_FL_AREA											; 10
		.BYTE	_FL_AREA											; 11
		.BYTE	_FL_AREA											; 12
		.BYTE	_FL_AREA											; 13
		.BYTE	_FL_AREA											; 14
		.BYTE	_FL_AREA											; 15
		.BYTE	_FL_AREA											; 16
		.BYTE	_FL_AREA											; 17
		.BYTE	_FL_AREA											; 18
		.BYTE	_FL_AREA											; 19
		.BYTE	_FL_AREA											; 1A
		.BYTE	0													; 1B
		.BYTE	0													; 1C
		.BYTE	0													; 1D
		.BYTE	0													; 1E
		.BYTE	0													; 1F
; C areas
		.BYTE	_FL_AREA											; 20
		.BYTE	_FL_AREA											; 21
		.BYTE	_FL_AREA											; 22
		.BYTE	_FL_AREA											; 23
		.BYTE	_FL_AREA											; 24
		.BYTE	_FL_AREA											; 25
		.BYTE	_FL_AREA											; 26
		.BYTE	_FL_AREA											; 27
		.BYTE	_FL_AREA											; 28
		.BYTE	_FL_AREA											; 29
		.BYTE	_FL_AREA											; 2A
		.BYTE	_FL_AREA											; 2B
		.BYTE	_FL_AREA											; 2C
		.BYTE	_FL_AREA											; 2D
		.BYTE	_FL_AREA											; 2E
		.BYTE	_FL_AREA											; 2F
		.BYTE	_FL_AREA											; 30
		.BYTE	_FL_AREA											; 31
		.BYTE	_FL_AREA											; 32
		.BYTE	_FL_AREA											; 33
		.BYTE	_FL_AREA											; 34
		.BYTE	_FL_AREA											; 35
		.BYTE	_FL_AREA											; 36
		.BYTE	_FL_AREA											; 37
		.BYTE	_FL_AREA											; 38
		.BYTE	0													; 39
		.BYTE	0													; 3A
		.BYTE	0													; 3B
		.BYTE	0													; 3C
		.BYTE	0													; 3D
		.BYTE	0													; 3E
		.BYTE	0													; 3F
; I areas
		.BYTE	_FL_AREA											; 40
		.BYTE	_FL_AREA											; 41
		.BYTE	_FL_AREA											; 42
		.BYTE	_FL_AREA											; 43
		.BYTE	_FL_AREA											; 44
		.BYTE	_FL_AREA											; 45
		.BYTE	_FL_AREA											; 46
		.BYTE	_FL_AREA											; 47
		.BYTE	_FL_AREA											; 48
		.BYTE	0													; 49
		.BYTE	0													; 4A
		.BYTE	0													; 4B
		.BYTE	0													; 4C
		.BYTE	0													; 4D
		.BYTE	0													; 4E
		.BYTE	0													; 4F
; Police station
		.BYTE	_FL_AREA											; 50
; Fire station
		.BYTE	_FL_AREA											; 51
		.BYTE	0													; 52
		.BYTE	0													; 53
		.BYTE	0													; 54
		.BYTE	0													; 55
		.BYTE	0													; 56
		.BYTE	0													; 57
		.BYTE	0													; 58
		.BYTE	0													; 59
		.BYTE	0													; 5A
		.BYTE	0													; 5B
		.BYTE	0													; 5C
		.BYTE	0													; 5D
		.BYTE	0													; 5E
		.BYTE	0													; 5F
; Power stations
		.BYTE	_FL_AREA											; 60
		.BYTE	_FL_AREA											; 61
; Sea port
		.BYTE	_FL_AREA											; 62
; Stadium
		.BYTE	_FL_AREA											; 63
; Air port
		.BYTE	_FL_AREA											; 64
		.BYTE	0													; 65
		.BYTE	0													; 66
		.BYTE	0													; 67
		.BYTE	0													; 68
		.BYTE	0													; 69
		.BYTE	0													; 6A
		.BYTE	0													; 6B
		.BYTE	0													; 6C
		.BYTE	0													; 6D
		.BYTE	0													; 6E
		.BYTE	0													; 6F
; Presents
		.BYTE	_FL_AREA|_FL_TERRAIN								; 70
		.BYTE	_FL_AREA|_FL_TERRAIN								; 71
		.BYTE	_FL_AREA|_FL_TERRAIN								; 72
		.BYTE	_FL_AREA|_FL_TERRAIN								; 73
		.BYTE	_FL_AREA|_FL_TERRAIN								; 74
		.BYTE	_FL_AREA|_FL_TERRAIN								; 75
		.BYTE	_FL_AREA|_FL_TERRAIN								; 76
		.BYTE	_FL_AREA|_FL_TERRAIN								; 77
		.BYTE	_FL_AREA|_FL_TERRAIN								; 78
		.BYTE	_FL_AREA|_FL_TERRAIN								; 79
		.BYTE	_FL_AREA|_FL_TERRAIN								; 7A
		.BYTE	_FL_AREA|_FL_TERRAIN								; 7B
		.BYTE	_FL_AREA|_FL_TERRAIN								; 7C
		.BYTE	_FL_AREA|_FL_TERRAIN								; 7D
		.BYTE	_FL_AREA|_FL_TERRAIN								; 7E
		.BYTE	0													; 7F
; Internal area tiles
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 80
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 81
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 82
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 83
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 84
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 85
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 86
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 87
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 88
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 89
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 8A
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 8B
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 8C
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 8D
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE							; 8E
		.BYTE	0													; 8F
; Ground
		.BYTE	_FL_FLOODABLE										; 90
; Regular roads
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 91
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 92
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 93
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 94
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 95
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 96
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 97
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 98
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 99
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 9A
		.BYTE	_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE				; 9B
; Regular rails
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; 9C
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; 9D
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; 9E
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; 9F
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A0
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A1
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A2
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A3
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A4
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A5
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; A6
; Regular electrics
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; A7
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; A8
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; A9
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; AA
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; AB
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; AC
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; AD
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; AE
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; AF
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; B0
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_FLOODABLE			; B1
; Park grass
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; B2
; Park trees
;		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; B3 BUG for some reasons inflammable!
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; B3 FIX, now flammable
; Rubble ground
		.BYTE	_FL_FLOODABLE										; B4
; Flood and pollution has no flags obviously
		.BYTE	0													; B5
		.BYTE	0													; B6
; Road-rails crossection, NOTE, only rail flag here!
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; B7
		.BYTE	_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE				; B8
; Electro-road crossection
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE	; B9
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_ROAD|_FL_FLOODABLE	; BA
; Electro-rail crossection
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE	; BB
		.BYTE	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_RAIL|_FL_FLOODABLE	; BC
; Water tiles with shores. Pure water non floodable obviously
		.BYTE	_FL_TERRAIN|_FL_WATER								; BD
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; BE
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; BF
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C0
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C1
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C2
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C3
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C4
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C5
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C6
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C7
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C8
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; C9
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; CA
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; CB
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; CC
		.BYTE	_FL_TERRAIN|_FL_FLOODABLE							; CD
; Regular woods
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; CE
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; CF
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D0
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D1
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D2
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D3
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D4
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D5
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D6
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D7
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D8
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; D9
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; DA
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; DB
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; DC
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; DD
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; DE
		.BYTE	_FL_FLAMMABLE|_FL_TERRAIN|_FL_FLOODABLE				; DF
; Water based roads
; UNFINISHED! no road flags here!
;		.BYTE	_FL_TERRAIN|_FL_WATER								; E0
;		.BYTE	_FL_TERRAIN|_FL_WATER								; E1
; NEW
		.BYTE	_FL_ROAD|_FL_TERRAIN|_FL_WATER						; E0
		.BYTE	_FL_ROAD|_FL_TERRAIN|_FL_WATER						; E1
; Water based rails, here see, rails flag set!
		.BYTE	_FL_RAIL|_FL_TERRAIN|_FL_WATER						; E2
		.BYTE	_FL_RAIL|_FL_TERRAIN|_FL_WATER						; E3
; Water based electrics
		.BYTE	_FL_CONDUCTOR|_FL_TERRAIN|_FL_WATER					; E4
		.BYTE	_FL_CONDUCTOR|_FL_TERRAIN|_FL_WATER					; E5
; Fire and rubbish animation
		.BYTE	0													; E6
		.BYTE	0													; E7
		.BYTE	0													; E8
		.BYTE	0													; E9
		.BYTE	0													; EA
; Water based road bridges
; UNFINISHED, no roads flag!
;		.BYTE	_FL_TERRAIN|_FL_WATER								; EB
;		.BYTE	_FL_TERRAIN|_FL_WATER								; EC
; NEW
		.BYTE	_FL_ROAD|_FL_TERRAIN|_FL_WATER						; EB
		.BYTE	_FL_ROAD|_FL_TERRAIN|_FL_WATER						; EC
; Water based opened road bridges tiles
		.BYTE	_FL_TERRAIN|_FL_WATER								; ED
		.BYTE	_FL_TERRAIN|_FL_WATER								; EE
		.BYTE	_FL_TERRAIN|_FL_WATER								; EF
		.BYTE	_FL_TERRAIN|_FL_WATER								; F0
		.BYTE	_FL_TERRAIN|_FL_WATER								; F1
		.BYTE	_FL_TERRAIN|_FL_WATER								; F2
		.BYTE	_FL_TERRAIN|_FL_WATER								; F3
		.BYTE	_FL_TERRAIN|_FL_WATER								; F4
		.BYTE	_FL_TERRAIN|_FL_WATER								; F5
		.BYTE	_FL_TERRAIN|_FL_WATER								; F6
		.BYTE	_FL_TERRAIN|_FL_WATER								; F7
		.BYTE	_FL_TERRAIN|_FL_WATER								; F8
		.BYTE	_FL_TERRAIN|_FL_WATER								; F9
		.BYTE	_FL_TERRAIN|_FL_WATER								; FA
		.BYTE	_FL_TERRAIN|_FL_WATER								; FB
		.BYTE	_FL_TERRAIN|_FL_WATER								; FC
		.BYTE	0													; FD
; Water canal/route
		.BYTE	_FL_TERRAIN|_FL_WATER								; FE
		.BYTE	0													; FF

		LAST_BANK_END FF,$10000

total_banks	= 16
total_size	= 16*8192
