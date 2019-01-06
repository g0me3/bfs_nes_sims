
		ORG $A000

		BANK_START $F1

; ---------------------------------------------------------------------------
; -------------G-A-M-E--S-E-T-U-P--A-N-D--I-N-I-T--S-T-U-F-F-----------------
; ---------------------------------------------------------------------------

		SECTION_START F0E

; =============== S U B	R O U T	I N E =======================================
_game_setup:
		LDA		#_MUS_IDX_STOP
		STA		_apu_mus_idx_req				; stop music
		JSR		_wait_for_nmi
		LDA		#_MUS_IDX_SETUP
		STA		_apu_mus_idx_req				; load degault setup music idx
		LDA		#$00
		STA		_tmp6D5
		STA		_tmp6D2
		STA		_tmp6D3
		LDA		_scenario_result_idx			; when return from scenario mode
		BNE		loc_2402A						; do not load_cur_game_mode, jump directly
		LDY		_cur_game_mode					; to new scenario select
;		BNE		loc_24024						; REDUNDANT, weird, we already stop music inside
;		LDA		#_MUS_IDX_STOP					; continue handlers
;		STA		_apu_mus_idx_req
;loc_24024:
		LDA		_game_setup_subs_list,Y			; load actual game mode setup sub
		STA		_game_setup_sub_idx
loc_2402A:
		JSR		_wait_for_nmi					; do loop until you complete corresponding settings
		JSR		_game_setup_sub_exec
		LDA		_main_sub_idx
		CMP		#$01
		BEQ		loc_2402A
		RTS

_game_setup_subs_list:
		.BYTE	$03,$00,$05,$09

; =============== S U B	R O U T	I N E =======================================
_game_setup_sub_exec:
		LDA		_game_setup_sub_idx
		JSR		_switch
		.WORD	_game_setup00_new_city_gen				; city gen start
		.WORD	_game_setup01_name_input_init			; city gen
		.WORD	_game_setup02_name_input_input			; city gen jump 0D
		.WORD	_game_setup03_continue_init				; continue start
		.WORD	_game_setup04_continue_input			; continue stop
		.WORD	_game_setup05_scenario_init				; scenario start
		.WORD	_game_setup06_scenario_select_input		; scenario
		.WORD	_game_setup07_scenario_intro_init		; scenario
		.WORD	_game_setup08_scenario_intro_draw		; scenario stop
		.WORD	_game_setup09_practice_msg_init			; practice start
		.WORD	_game_setup0A_practice_msg_draw			; practice stop
		.WORD	_game_setup0B_scenario_over_init		; game over start
		.WORD	_game_setup0C_scenario_over_input		; game over jump 05/0F
		.WORD	_game_setup0D_difficult_init			; city gen continues
		.WORD	_game_setup0E_difficult_input			; city gen stop
		.WORD	_game_setup0F_all_complete_init			; game won start
		.WORD	_game_setup10_all_complete_input		; game won jump 05

; REDUNDANT
;		LDY		_game_setup_sub_idx
;		LDA		_game_setup_lib_lo,Y
;		STA		_ptr0
;		LDA		_game_setup_lib_hi,Y
;		STA		_ptr0+1
;		JMP		(_ptr0)
;_game_setup_lib_lo:
;		.BYTE	<[_game_setup00_new_city_gen]
;		.BYTE	<[_game_setup01_name_input_init]
;		.BYTE	<[_game_setup02_name_input_input]
;		.BYTE	<[_game_setup03_continue_init]
;		.BYTE	<[_game_setup04_continue_input]
;		.BYTE	<[_game_setup05_scenario_init]
;		.BYTE	<[_game_setup06_scenario_select_input]
;		.BYTE	<[_game_setup07_scenario_intro_init]
;		.BYTE	<[_game_setup08_scenario_intro_draw]
;		.BYTE	<[_game_setup09_practice_msg_init]
;		.BYTE	<[_game_setup0A_practice_msg_draw]
;		.BYTE	<[_game_setup0B_scenario_over_init]
;		.BYTE	<[_game_setup0C_scenario_over_input]
;		.BYTE	<[_game_setup0D_difficult_init]
;		.BYTE	<[_game_setup0E_difficult_input]
;		.BYTE	<[_game_setup0F_all_complete_init]
;		.BYTE	<[_game_setup10_all_complete_input]
;_game_setup_lib_hi:
;		.BYTE	>[_game_setup00_new_city_gen]
;		.BYTE	>[_game_setup01_name_input_init]
;		.BYTE	>[_game_setup02_name_input_input]
;		.BYTE	>[_game_setup03_continue_init]
;		.BYTE	>[_game_setup04_continue_input]
;		.BYTE	>[_game_setup05_scenario_init]
;		.BYTE	>[_game_setup06_scenario_select_input]
;		.BYTE	>[_game_setup07_scenario_intro_init]
;		.BYTE	>[_game_setup08_scenario_intro_draw]
;		.BYTE	>[_game_setup09_practice_msg_init]
;		.BYTE	>[_game_setup0A_practice_msg_draw]
;		.BYTE	>[_game_setup0B_scenario_over_init]
;		.BYTE	>[_game_setup0C_scenario_over_input]
;		.BYTE	>[_game_setup0D_difficult_init]
;		.BYTE	>[_game_setup0E_difficult_input]
;		.BYTE	>[_game_setup0F_all_complete_init]
;		.BYTE	>[_game_setup10_all_complete_input]

; OPTIMIZED, merged
; =============== S U B	R O U T	I N E =======================================
_game_setup_screen_init:
		JSR		_ppu_queues_reset				; reset queue buffers
		JSR		_pal_fade_out					; fade out
		JSR		_render_off						; render off
		LDY		#$80							; prepare vars
		STY		_game_wnd_spr_mode_flag
		STY		_ppu_ctrl_shadow
		LDY		#$01
		STY		_scr_mode_idx					; set screen modes
		INY
		STY		_scr_res_idx
		DEC		_mmc5_operation_in_progress_flag; enter critical section
		LDA		#$08
		STA		_mmc5_nt_mode_shadow			; set mode
		STA		_MMC5_NT_MODE
		JMP		_spr_clear						; ready to ppu unpack

; =============== S U B	R O U T	I N E =======================================
_game_setup05_scenario_init:
;		JSR		_ppu_queues_reset				; REDUNDANT
;		JSR		_pal_fade_out
;		JSR		_render_off
;		LDY		#$80
;		STY		_game_wnd_spr_mode_flag
;		STY		_ppu_ctrl_shadow
;		LDY		#$01
;		STY		_scr_mode_idx
;		INY
;		STY		_scr_res_idx
;		DEC		_mmc5_operation_in_progress_flag
;		LDA		#$08
;		STA		_mmc5_nt_mode_shadow
;		STA		_MMC5_NT_MODE
;		JSR		_spr_clear
; -
		JSR		_game_setup_screen_init			; OPTIMIZED
; -
		LDA		_city._all_scenarios_are_clear_ram_flag
		BNE		.extra_scenarios				;  all scenarios are beaten, unlock full screen
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_scenario_normal
		LDY		#$03							; or load the regular one with 6 scenarios
		BNE		loc_240B2
.extra_scenarios:
		LDXY	_tlm_res_lz_scenario_complete	; tlm for this scenario is in extra bank
		FJSRA	_res_ppu_lz_unpack,PRG6,PRG6	; the only extarnal tlm data here for now
		LDY		#$04
loc_240B2:
		STY		_tmp6D4							; tmp var for number or icons per row
;		JSR		_ppu_queues_reset				; REDUNDANT, already clear
		LDA		#$02
		STA		_MMC5_CHR_MODE
		JSR		_scenario_closed_screen_update	; draw "CLOSED" icons on beaten scenarios
		LDA		#$01
		STA		_MMC5_CHR_MODE
		LDA		#$00
		STA		_tmp6D0
		STA		_tmp6D0+1
		LDA		#$08
		STA		_screen_pal_cur_bg_idx
		LDA		#$02
		STA		_screen_pal_cur_spr_idx

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_game_setup_screen_start:
		INC		_game_setup_sub_idx				; switch to the next sub
		JSR		_render_on						; render on again
		INC		_mmc5_operation_in_progress_flag; exit critical section
		JMP		_pal_fade_in					; fade out screen

; =============== S U B	R O U T	I N E =======================================
_scenario_closed_screen_update:
		LDY		#$00							; icons counter init
;		LDX		#$06							; REDUNDANT, icons counter max
		LDA		_city._all_scenarios_are_clear_ram_flag
		BNE		locret_2411B					; FIX: lets just exit here
;		BEQ		loc_240FA
; REDUNDANT: by this logic, when you complete all scenarios, the extra scenario
; screen reset all cleared pictures and planned to draw a new ones only for
; two extra scenarios lol, BUT, there are no offsets not only for a rearranged
; icons set (4x2 instead of 3x2), but there are no 7th and 8th offsets in
; table. if you ever manage to beat all scenarios, then beat one of the extra
; scenarios, you'll get an error screen. but you actually just can't beat
;
; FIX: disable extra code for this special options, anyway they aren't working
;
;		LDY		#$06
;		LDX		#$08
;		LDA		_city._scenarios_clear_flags
;		LSR
;		LSR
;		LSR
;		LSR
;		LSR
;		LSR
;		BPL		loc_240FD
;loc_240FA:
		LDA		_city._scenarios_clear_flags
loc_240FD:
;		STX		_tmp3							; REDUNDANT, store cycle max idx
; BUG: _tmp2 here used as a cycle counter, also used inside the draw
; routine but nothing is modify it there as against _tmp3 which is
; used in this loop as max counts number, but also used inside draw
; routine as temporary var so erased inside it and loop will run for
; extra 250 cycles because of that.
;
; REDUNDANT: it seems, they save wrong var before drawing routines: pushes
; _tmp2 instead of _tmp3 lol, no need anymore, removed it
;
loc_240FF:
		STY		_tmp2							; store for convinience
		LSR										; here is the bitflags in A
		BCC		loc_2411A						; skip if flag clear
		PHA										; save current shifted flags
;		PUSHB	_tmp3							; REDUNDANT, save cycles max idx
		JSR		_scenario_closed_icon_draw		; draw
;loc_2410B:
;		JSR		_ppu_dequeue					; REDUNDANT, merged
;		JSR		_extnt_dequeue
;		JSR		_buffers_test_flush
;		BNE		loc_2410B
		JSR		_buffers_force_flush			; OPTIMIZED
;		POPB	_tmp3							; REDUNDANT, restore cycle max count
		PLA
loc_2411A:
		LDY		_tmp2							; get cur cycle idx
		INY										; next (will be saved at the loop start)
;		CPY		_tmp3							; REDUNDANT
		CPY		#$06							; OPTIMIZED, fixed now to single value
		BNE		loc_240FF						; test if loops is over
locret_2411B:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scenario_closed_icon_draw:
		PPUQSTART
		LDA		_scenario_closed_ppu_ofs_hi,Y	; get the "CLOSED" ppu ofs for
		STA		_ptr0+1							; particular icon
		PHA
		LDA		_scenario_closed_ppu_ofs_lo,Y
		STA		_ptr0
		PHA
		LDY		#$00
loc_24130:
		PPUQMOVB	_ptr0,OP_NOP,0				; queue ofs, and 6 bytes (one line)
		PPUQMOVB	_ptr0+1,OP_NOP,0			; at once
		PPUQMOVB	#$06,OP_NOP,0
		STA		_tmp3
loc_24144:
		LDA		_tlm_scenario_closed_nt,Y
		INY
		PPUQMOVA
		DEC		_tmp3
		BNE		loc_24144						; go to the next line, draw up to 6x6 bytes
		ADDWB	_ptr0,#$20
		CPY		#$24
		BNE		loc_24130
		PPUQEND
		LDY		_tmp2
		EXTQSTART
; REDUNDANT
;		LDA		_scenario_closed_ppu_ofs_hi,Y
;		CLC
;		ADC		#$3C
;		STA		_ptr0+1
;		LDA		_scenario_closed_ppu_ofs_lo,Y
;		STA		_ptr0
; OPTIMIZED
; -
		PLA
		STA		_ptr0
		PLA
		CLC
		ADC		#$3C
		STA		_ptr0+1
; -
		LDY		#$00
loc_24174:
		EXTQMOVB	_ptr0,OP_NOP,0				; exactly the same for extnt data
		EXTQMOVB	_ptr0+1,OP_NOP,0
		EXTQMOVB	#$06,OP_NOP,0
		STA		_tmp3
loc_24188:
		LDA		_tlm_scenario_closed_extnt,Y
		INY
		EXTQMOVA
		DEC		_tmp3
		BNE		loc_24188
		ADDWB	_ptr0,#$20
		CPY		#$24
		BNE		loc_24174
		EXTQEND
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
		BIT		_pad0_press				; test B button
		BVC		loc_2420C
		LDA		#$02
		STA		_title_sub_idx					; if pressed, return back to the title menu
		DEC		_main_sub_idx
		JMP		_pal_fade_out
loc_2420C:
		JMP		_common_select_cursor_control	; or else draw regular cursor

; =============== S U B	R O U T	I N E =======================================
_scenario_select_input:
		LDA		_pad0_autorep			; test if any d-pad pressed
		AND		#$0F
		BEQ		.no_dpad_pressed
		APUA_SE	_SE_IDX_SELECT					; then do sound
.no_dpad_pressed:
		LDX		_tmp6D2							; column idx selector
		LDA		_pad0_autorep
		LSR
		BCC		loc_24229
		INX										; dpad RIGHT pressed
		CPX		_tmp6D4							; increment idx, compare with number of icons per row
		BNE		loc_24229
		LDX		#$00							; wrap to 0 if more than needed
loc_24229:
		LSR
		BCC		loc_24233
		DEX										; the same for LEFT button
		BPL		loc_24233
		LDX		_tmp6D4							; wrap to max instead
		DEX
loc_24233:
		AND		#$03
		BEQ		.no_up_down_pressed
		LDA		_tmp6D3							; up-down very simple, just toggle low bit
		EOR		#$01							; of column selector idx
		STA		_tmp6D3
.no_up_down_pressed:
		STX		_tmp6D2							; get cursor position of current row/col idxes
		LDY		_tmp6D3
		LDA		_scenario_cursor_col_list,Y		; two columns always
		STA		_tmp6CE
		LDA		_scenario_cursor_row_list3,X	; may be either 3 rows...
		LDY		_city._all_scenarios_are_clear_ram_flag
		BEQ		loc_24256
		LDA		_scenario_cursor_row_list4,X	; ...or 4 if all scenarios cleared
loc_24256:
		STA		_tmp6CC
		LDA		_pad0_press				; store cursor pos tmp
		BPL		locret_24273					; test for A button now
		LDA		_tmp6D3							; calc scenarios list ofs
		ASL
		ASL
		ADC		_tmp6D2
		TAY
		LDA		_scenatios_list,Y				; get an actual scenario idx
		STA		_scenario_idx
		APUA_SE	_SE_IDX_CLICK
		INC		_game_setup_sub_idx
locret_24273:
		RTS

_scenario_cursor_row_list3:
		.BYTE	$4D,$8D,$CD
_scenario_cursor_row_list4:
		.BYTE	$45,$75,$A5,$DD
_scenario_cursor_col_list:
		.BYTE	$5D,$A5
_scenatios_list:
		.BYTE	$00,$01,$02,$06
		.BYTE	$03,$04,$05,$07

; =============== S U B	R O U T	I N E =======================================
_game_setup07_scenario_intro_init:
		JSR		_ppu_queues_reset				; prepare scenario intro screen
		JSR		_pal_fade_out
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		JSR		_spr_clear						; common full-screen window draw
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_fullscreen_wnd
		LDY		_scenario_idx					; scenario name header draw
; NOTE, scenario headers drawn by tilemap method, however they used the same
; font as in PRG3 for ingame message headers draw. for ingame messages are
; headers are ASCII encoded and converted to full 3-line fonts automatically
; here we have a lot of redundancy because may use the same conversion routines
; as for ingame menus and keep all scenario headers in ASCII as well
; the problem is, the ingame window functions are more complex, they need to
; calculate screen offsets on the fly. this code doesn't...
; TODO: check if merging both codes worth it.
		LDA		_tlm_res_scenario_hdr_lib_lo,Y
;		STA		_ptr0
		TAX
		LDA		_tlm_res_scenario_hdr_lib_hi,Y
;		STA		_ptr0+1
		TAY
		JSR		_res_ppu_unpack
; OPTIMIZED, common lib, indexes 0-7 for scenarios
		LDA		_scenario_idx					; now load scenario message data
		JSR		_msg_lib_load					; for printout routine
;		LDA		_scenario_msg_lib_lo,Y
;		STA		_tmp6D6
;		LDA		_scenario_msg_lib_hi,Y
;		STA		_tmp6D6+1
;		LDY		#$00
;		STY		_tmp6DA
;		STY		_tmp6D9
;		DEY
;		STY		_tmp6D8
		LDA		#$11
		STA		_screen_pal_cur_bg_idx
; REDUNDANT
;		INC		_game_setup_sub_idx				; skip to the next sub
;		JSR		_render_on
;		INC		_mmc5_operation_in_progress_flag
;		JMP		_pal_fade_in
		JMP		_game_setup_screen_start

; =============== S U B	R O U T	I N E =======================================
_game_setup08_scenario_intro_draw:
		JSR		_msg_print_out_handler			; do pritnout letter by letter
		BIT		_pad0_press				; all this tile wait for A button
		BPL		loc_242EC
		APUA_SE	_SE_IDX_CLICK					; skip scenario intro if pressed
		JSR		_scenario_map_load				; load scenario map from PPU ROM
		INC		_main_sub_idx					; go to the main game
		LDA		#_MUS_IDX_STOP
		STA		_apu_mus_idx_req				; stop the music, already fade out by map load
		RTS
loc_242EC:
		BVC		locret_242F7					; test if B pressed
		DEC		_game_setup_sub_idx				; then cancel to _game_setup05_scenario_init
		DEC		_game_setup_sub_idx				; to select scenario again
		DEC		_game_setup_sub_idx
locret_242F7:
		RTS

; =============== S U B	R O U T	I N E =======================================
_msg_print_out_handler:
		JSR		_msg_print_out					; print two chars at a time

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_msg_print_out:
		LDY		_tmp6D8							; _tmp6D8 initialized with FF at the beginning
		CPY		#$FF							; so this means we not in dict mode
		BEQ		.skip_dictionary
		INC		_tmp6D8							; or else this is a dictionary char pos
		LDA		_msg_dictionary,Y				; load it, pos already incremented
		BPL		.put_msg_char					; if not FF, print char as is, if FF
		STA		_tmp6D8							; store it, this means end of dict word
.skip_dictionary:
		LDA		_tmp6D6+1						; cur message char ptr, if high nibble is FF
		CMP		#$FF							; the end of message occured
		BEQ		locret_2436F					; so just skip
		STA		_ptr0+1							; if not, store to the tmp ptr
		LDA		_tmp6D6
		STA		_ptr0
		INCW	_tmp6D6							; increment ptr
		LDY		#$00
		LDA		(_ptr0),Y						; then load cur char
		BPL		loc_24330						; hi-bit is clear, then regular chars
		AND		#$7F							; if set, this is a dictionary char offset
		STA		_tmp6D8							; mask it, dic max size if 127 bytes including
		BPL		_msg_print_out					; eofs, jump recursively to char fetch
loc_24330:
		CMP		#$0A							; regular special symbol CR/LF
		BNE		loc_2433D
		LDA		#$00							; reset row pos, increment column pos
		STA		_tmp6D9							; nothing to draw there
		INC		_tmp6DA
		RTS
loc_2433D:
		CMP		#$5B
		BNE		.put_msg_char					; end of msg special code
		LDA		#$FF
		STA		_tmp6D6+1						; set end msg flag
		RTS
.put_msg_char:
		PHA										; backup actual print symbol
		PPUQSTART
		LDY		_tmp6DA							; line ppu ofs already calculated for this screen
		LDA		_msg_print_out_ppu_pos_list_lo,Y; just load them and queue current char
		CLC
		ADC		_tmp6D9
		PPUQMOVA
		LDA		_msg_print_out_ppu_pos_list_hi,Y
		PPUQMOVA
		PPUQMOVB	#$01,OP_NOP,0				; one char queue
		PLA										; restore symbol to print
		PPUQMOVA
		PPUQEND
		INC		_tmp6D9							; next row.
locret_2436F:
		RTS

_msg_print_out_ppu_pos_list_lo:
		.BYTE	$04,$24,$44,$64,$84,$A4,$C4,$E4,$04,$24,$44,$64,$84,$A4,$C4,$E4,$04
_msg_print_out_ppu_pos_list_hi:
		.BYTE	$21,$21,$21,$21,$21,$21,$21,$21,$22,$22,$22,$22,$22,$22,$22,$22,$23

; =============== S U B	R O U T	I N E =======================================
_msg_lib_load:
		TAY
		LDA		_msg_lib_lo,Y					; unificated mag data load  routine
		STA		_tmp6D6
		LDA		_msg_lib_hi,Y
		STA		_tmp6D6+1
		LDY		#$00
		STY		_tmp6DA							; as well as init the msg variables
		STY		_tmp6D9
		DEY
		STY		_tmp6D8
		RTS

; =============== S U B	R O U T	I N E =======================================
_scenario_map_load:
		JSR		_pal_fade_out					; to load a map, we need access to PPU ROM
		JSR		_render_off						; shut down the screen then
		DEC		_mmc5_operation_in_progress_flag
		JSR		_city_map_init					; init map buf with BULLDOZERED_GROUND
		LDA		#$01
		STA		_MMC5_CHR_SIZE					; set CHR bank size 4K
		JSR		_apu_dpcm_forbid				; somehow dpcm may interfer with process, disable it
		LDY		_scenario_idx
		LDX		_scenario_data_chr_bank,Y		; load PPU ROM banks containing a map data
		STX		_MMC5_CHR_BANKSA+3
		INX
		STX		_MMC5_CHR_BANKSA+7
		LDA		_PPU_STATUS
		LDA		_scenario_data_ppu_hi,Y			; also we have list of offsets there
		AND		#$0F
		STA		_PPU_ADDR
		LDA		_scenario_data_ppu_lo,Y
		STA		_PPU_ADDR						; setup address
		LDA		_PPU_DATA						; dummy read, now ready
		LDA		_scenario_map_dicts_lib_lo,Y	; load ptr to current map dictionary
		STA		_tmp2							; of two-tile sctructures used as packing
		LDA		_scenario_map_dicts_lib_hi,Y
		STA		_tmp3
		MOVWO	_tmp4,_city_map					; load city map buf ptr as dst ptr
		JSR		_sram_write_enable				; enable wram bank
		JSR		_scenario_map_unpack			; do unpack
		JSR		_sram_write_disable				; disable back
		LDA		#$03
		STA		_MMC5_CHR_SIZE					; restore CHR bank size 1K
		JSR		_apu_dpcm_allow					; allow dpcm back
; REDUNDANT
;		LDY		_scenario_idx
;		LDX		_scenario_titles_ofs_lib,Y		; load predefined town name for scenario
;		LDY		#$01
;loc_243F1:
;		LDA		_scenario_town_names_list,X		; copy data
;		BMI		loc_243FD
;		STA		_city._name,Y
;		INX
;		INY
;		BNE		loc_243F1
;loc_243FD:
;		STY		_city._name						; store name buf len
; -
		LDX 	_scenario_idx					; OPTIMIZED, default city name, scenario name
		LDY		#$00
		JSR		_set_default_name
; -
		LDA		_mmc5_chr_banks_shadow+3		; restore CHR3 BANK
		STA		_MMC5_CHR_BANKSA+3
		INC		_mmc5_operation_in_progress_flag; restart ppu
		JMP		_render_on

; =============== S U B	R O U T	I N E =======================================
_scenario_map_unpack:
		LDY		#$00
		LDA		_PPU_DATA						; read code
		STA		_tmp6
		AND		#$F0
		CMP		#$80
		BNE		.raw_tiles						; object code as is
		LDA		_tmp6
		CMP		#$8F
		BNE		.dict_tiles						; 8F, RLE code next two bytes are
		LDX		_PPU_DATA						; counter + object idx to insert
		LDA		_PPU_DATA
loc_24424:
		JSR		_scenario_map_put_block			; do rle unpack of next two bytes
		DEX
		BNE		loc_24424
		JMP		_scenario_map_unpack
.dict_tiles:
		LDA		_tmp6							; 80-8E, DICT codes (as inused special tiles)
		SEC										; every scenario has a DICT of 2x1 tiles, 15 each
		SBC		#$80							; extract DICT entry idx
		ASL
		PHA										; OPTIMIZED
		TAY
		LDA		(_tmp2),Y
		JSR		_scenario_map_put_block
;		LDA		_tmp6							; REDUNDANT, save in stack instead
;		SEC										; -
;		SBC		#$80							; -
;		ASL										; -
;		TAY										; -
		PLA										; OPTIMIZED, pop from stack
		TAY										; -
		INY
		LDA		(_tmp2),Y
		JSR		_scenario_map_put_block
		JMP		_scenario_map_unpack
.raw_tiles:
		LDA		_tmp6							; raw tiles 00-7F, 90-FF
		JSR		_scenario_map_put_block
		JMP		_scenario_map_unpack

; =============== S U B	R O U T	I N E =======================================
; NOTE, thatalready buldozzereg ground already in buffer need to be overwritten with
; bulldozered ground in any case, because there is no code for just skip positions
; maybe we would save some space if repack it with such code...
;
_scenario_map_put_block:
		STA		_tmp6+1
.map_tile_fetch:
		LDY		#$00
		LDA		(_tmp4),Y						; put only on buldozered ground
		CMP		#_MAP_GROUND					; when we build 2x2 or more, the next
		BEQ		loc_2446F						; tile would be non _MAP_GROUND, so skip it
		INCW	_tmp4							; this way we don't need to store actual pos
		LDA		_tmp4							; of the buildings in row.
		CMP		#<[_city_map_end]
		BNE		.map_tile_fetch
		LDA		_tmp5
		CMP		#>[_city_map_end]
		BNE		.map_tile_fetch					; try again
		BEQ		.break_unpack					; end of map, break unpack
loc_2446F:
		TXA										; OPTIMIZED: we need X here now
		PHA
		LDA		_tmp6+1
		BMI		loc_244D9						; all tiles above 80 are single
		LDX		#$0F							; OPTIMIZED
		CMP		#_MAP_AIR_PORT					; the reset are a large buildings
		BEQ		loc_244D6						; OPTIMIZED
; REDUNDANT
;		BNE		loc_2449E						; draw airport 4x4 special tiles
;		LDY		#$03							; xxxxxx8E (03)
;		LDA		#$8E							; xxxxxx8D (4F)
;		STA		(_tmp4),Y						; xxxxxx8C (9B)
;		LDY		#$4F							; 88898A8B
;		LDA		#$8D							;(E4E5E6E7)
;		STA		(_tmp4),Y
;		LDY		#$9B
;		LDA		#$8C
;		STA		(_tmp4),Y
;		LDY		#$E4
;		LDA		#$88
;		STA		(_tmp4),Y
;		INY
;		LDA		#$89
;		STA		(_tmp4),Y
;		INY
;		LDA		#$8A
;		STA		(_tmp4),Y
;		INY
;		LDA		#$8B
;		STA		(_tmp4),Y
loc_2449E:
		LDX		#$03							; OPTIMIZED
		CMP		#_MAP_COAL_POWER				; default draw 2x2 tiles
		BCC		loc_244D6						; draw 3x3 buildings tiles
		LDX		#$08							; OPTIMIZED
; REDUNDANT
;		LDY		#$02							; xxxx87
;		LDA		#$87							; xxxx86
;		STA		(_tmp4),Y						; 838485
;		LDY		#$4E
;		LDA		#$86
;		STA		(_tmp4),Y
;		LDY		#$98
;		LDA		#$83
;		STA		(_tmp4),Y
;		INY
;		LDA		#$84
;		STA		(_tmp4),Y
;		INY
;		LDA		#$85
;		STA		(_tmp4),Y
;loc_244BE:
;		LDY		#$00							; the rest is 2x2 buildings
;		LDA		_tmp6+1							; ID82
;		STA		(_tmp4),Y						; 8081
;		INY
;		LDA		#$82
;		STA		(_tmp4),Y
;		LDA		#$80
;		LDY		#$4C
;		STA		(_tmp4),Y
;		INY
;		LDA		#$81
;		STA		(_tmp4),Y
;		BNE		loc_244DA
loc_244D6:
		LDA		_special_tiles_ofs_list-1,X		; OPTIMIZED
		TAY										; -
		LDA		_special_tiles_list-1,X			; -
		STA		(_tmp4),Y						; -
		DEX										; -
		BNE		loc_244D6						; -
		LDA		_tmp6+1
loc_244D9:
		LDY		$00
		STA		(_tmp4),Y
		PLA										; restore X back
		TAX										; -
;loc_244DA:
		INCW	_tmp4
		LDA		_tmp4
		CMP		#<[_city_map_end]
		BNE		.exit_map_tile_store
		LDA		_tmp5
		CMP		#>[_city_map_end]
		BNE		.exit_map_tile_store
.break_unpack:
		PLA
		PLA
.exit_map_tile_store:
		LDA		_tmp6+1
		RTS

; building special tiles scheme to copy to map.
;
_special_tiles_list:
		.BYTE	_MAP_INTERNAL_BUILD0,_MAP_INTERNAL_BUILD1,_MAP_INTERNAL_BUILD2
		.BYTE	_MAP_INTERNAL_BUILD3,_MAP_INTERNAL_BUILD4,_MAP_INTERNAL_BUILD5,_MAP_INTERNAL_BUILD6,_MAP_INTERNAL_BUILD7
		.BYTE	_MAP_INTERNAL_BUILD8,_MAP_INTERNAL_BUILD9,_MAP_INTERNAL_BUILDA,_MAP_INTERNAL_BUILDB,_MAP_INTERNAL_BUILDC,_MAP_INTERNAL_BUILDD,_MAP_INTERNAL_BUILDE

; =============== S U B	R O U T	I N E =======================================
;_unref_19:
;		INC		_ptr0
;		BNE		locret_244F7
;		INC		_ptr0+1
;locret_244F7:
;		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup00_new_city_gen:
		JSR		_ppu_queues_reset				; oddly enough, this is the only
		JSR		_pal_fade_out					; game setup uses the regular ingame
		LDA		#$24							; screen resource code
		STA		_mmc5_nt_mode_shadow
		STA		_MMC5_NT_MODE
		LDA		#$07
		STA		_scr_res_idx
		LDA		#$00
		STA		_scr_res_control_flags
		FJSRA	_scr_res_common,SRAM,PRG9
		LDA		_title_sub_idx
		BEQ		loc_249F4
		DEC		_main_sub_idx
		JSR		_pal_fade_out
loc_249F4:
		INC		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup01_name_input_init:
; REDUNDANT
;		JSR		_ppu_queues_reset
;		JSR		_pal_fade_out
;		JSR		_render_off
;		LDY		#$80
;		STY		_game_wnd_spr_mode_flag
;		STY		_ppu_ctrl_shadow
;		LDY		#$01
;		STY		_scr_mode_idx
;		INY
;		STY		_scr_res_idx
;		DEC		_mmc5_operation_in_progress_flag
;		LDA		#$08
;		STA		_mmc5_nt_mode_shadow
;		STA		_MMC5_NT_MODE
;		JSR		_spr_clear
; -
		JSR		_game_setup_screen_init			; OPTIMIZED
; -
;		LDXY	_tlm_res_lz_name_init			; REDUNDANT, draw external tilemap for name input screen
;		FJSRA	_res_ppu_lz_unpack,SRAM,PRGA
; -
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_name_init	; OPTIMIZED, now local
		LDA		#$00
		STA		_tmp6DC+2
		LDA		#$02
		STA		_MMC5_CHR_MODE					; unpack upper part of the input screen
;		JSRXY	_rle_nt_res_queue,_rle_tlm_nt_input_name
;		JSRXY	_rle_extnt_res_queue,_rle_tlm_extnt_input_name
		LDX		#$04							; OPTIMIZED
		JSR		_rle_tlm_queue
;		JSR		_name_input_char_nt_queue		; REDUNDANT. draw name buffer (now empty)
;		JSR		_name_input_char_extnt_queue
		JSR		_name_input_name_buf_queue		; OPTIMIZED, called both at the same time, merge
;loc_24549:
;		JSR		_ppu_dequeue					; REDUNDANT, merged
;		JSR		_extnt_dequeue
;		JSR		_buffers_test_flush
;		BNE		loc_24549
		JSR		_buffers_force_flush			; OPTIMIZED, force flush common
		LDA		#$01
		STA		_MMC5_CHR_MODE
		LDA		#$2C
		STA		_MMC5_CHR_BANKSA+2				; set CHR banks for sprites here
		LDA		#$2D
		STA		_MMC5_CHR_BANKSA+3
		LDA		#$CD							; init starting cursor position
		STA		_tmp6CC
		LDA		#$A6
		STA		_tmp6CE
		LDA		#$00
		STA		_tmp6D0+1
		STA		_tmp6D0
		LDA		#$09
		STA		_screen_pal_cur_bg_idx			; init palettes
		LDA		#$07
		STA		_screen_pal_cur_spr_idx
; REDUNDANT
;		INC		_game_setup_sub_idx
;		JSR		_render_on
;		INC		_mmc5_operation_in_progress_flag
;		JMP		_pal_fade_in					; show screen
		JMP		_game_setup_screen_start

; =============== S U B	R O U T	I N E =======================================
_game_setup02_name_input_input:
		JSR		_name_input_cursor_move_control	; main name input handler
		JSR		_common_select_cursor_control		; calling all stuff for input handling
		JSR		_name_input_char_select	; printing and storing input data
		JSR		_name_input_button_push_draw
		JSR		_name_input_char_store
;		JSR		_name_input_char_nt_queue		; REDUNDANT
;		JSR		_name_input_char_extnt_queue
		JSR		_name_input_name_buf_queue		; OPTIMIZED, called both at the same time, merge
		JSR		_spr_finish
		BIT		_pad0_press
		BVC		locret_245A9
		JMP		_name_input_previous_scr		; test for back button B
locret_245A9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_name_input_next_scr:
		LDA		_tmp6DC+2						; current input screen type
		BNE		loc_245C9						; if 0, then name input, jump next
		INC		_tmp6DC+2
		JSR		_ppu_queues_reset
		JSR		_name_input_clear_header_area	; redraw only upper portion of the screen
;		JSRXY	_rle_nt_res_queue,_rle_tlm_nt_input_word
;		JSRXY	_rle_extnt_res_queue,_rle_tlm_extnt_input_word
		LDX		#$08							; OPTIMIZED
		JSR		_rle_tlm_queue
		JMP		_buffers_test_wait_flush
loc_245C9:
		LDA		#$0D							; jump to difficult setting
		STA		_game_setup_sub_idx				; if 1, then we at fav word input,
		RTS										; then go to difficult setup

; =============== S U B	R O U T	I N E =======================================
_name_input_previous_scr:
		LDA		_tmp6DC+2						; if we at name input screen
		BEQ		loc_245EE						; then jump to the city gen screen
		DEC		_tmp6DC+2
		JSR		_ppu_queues_reset				; or return from fav word screen back
		JSR		_name_input_clear_header_area	; redraw upper part with name input again
;		JSRXY	_rle_nt_res_queue,_rle_tlm_nt_input_name
;		JSRXY	_rle_extnt_res_queue,_rle_tlm_extnt_input_name
		LDX		#$04							; OPTIMIZED
		JSR		_rle_tlm_queue
		JMP		_buffers_test_wait_flush
loc_245EE:
		LDA		#$00
		STA		_game_setup_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
; clear upper part of the name input screen for new header.
;
_name_input_clear_header_area:
;		JSRXY	_rle_nt_res_queue,_rle_tlm_nt_clear_area
;		JSRXY	_rle_extnt_res_queue,_rle_tlm_extnt_clear_area
		LDX		#$00							; OPTIMIZED
		JSR		_rle_tlm_queue
		JMP		_buffers_test_wait_flush

; =============== S U B	R O U T	I N E =======================================
_common_select_cursor_control:
		LDA		_pad0_press				; test for any button press
		BEQ		loc_24614
		LDA		#$FF							; if any button pressed
		STA		_tmp6D0+1						; stop blinking flag set
loc_24614:
		LDA		_tmp6D0+1
		BMI		.force_draw						; if 80 raized draw always
		LDA		_tmp6D0							;
		BNE		.timer_draw						; or draw while timer not zero
		LDY		#$08							; reload hide time
		LDA		_tmp6D0+1						; toggle draw allow flag low bit
		EOR		#$01							; disable the cursor
		STA		_tmp6D0+1
		BEQ		.reload_draw					; delay for show -10, for hide -8
		LDY		#$10
.reload_draw:
		STY		_tmp6D0							; reload timer
.timer_draw:
		DEC		_tmp6D0							; do
.force_draw:
		LDA		#$00
		STA		_spr_insert_args._idx			; reset spr buf
		STA		_spr_buf_pos
		LDA		_tmp6D0+1
		BEQ		loc_2464A						; check if we allow to show cursor
		LDA		_tmp6CC
		STA		_spr_insert_args._pos._COL		; draw actually
		LDA		_tmp6CE
		STA		_spr_insert_args._pos._ROW
		JMP		_spr_lib_attr_insert
loc_2464A:
		JMP		_spr_finish						; or close spr buffer for now

; =============== S U B	R O U T	I N E =======================================
; BUG: all tlm queue functions here when used frequently overflows the buffer
; (use turbo button for example), to fix it just add some flush wait.
;
_name_input_button_push_draw:
		LDA		_tmp6DC+3						; push anim counter
		BEQ		locret_246DB					; draw if non-zero
		DEC		_tmp6DC+3						; decrease
		LDY		_tmp6DC+6						; get pressed button type
		CPY		#$21
		BCS		.draw_simple_button_press		; all above TAB is simple letters
		CPY		#$08
		BEQ		.draw_simple_button_press		; BACK button simple to
		CMP		#$10
		BNE		.push_wait						; if anim already started, skip
		CPY		#$20							; else anim frame, draw push
		BNE		loc_24676						; two special buttons here
;		JSRXY	_rle_nt_res_queue,_tlm_nt_space_press
;		JSRXY	_rle_extnt_res_queue,_tlm_extnt_space_press
		LDX		#$30
		JSR		_tlm_lib_queue					; OPTIMIZED
		JMP		_buffers_test_wait_flush		; FIX
loc_24676:
		CPY		#$0C
		BNE		locret_246DB
;		JSRXY	_rle_nt_res_queue,_tlm_nt_back_button_press	; REDUNDANT
;		JSRXY	_rle_extnt_res_queue,_tlm_extnt_back_button_press
		LDX		#$2C
		JSR		_tlm_lib_queue					; OPTIMIZED
		JMP		_buffers_test_wait_flush		; FIX
.push_wait:
		CMP		#$01							; last anim frame, release buttons
		BEQ		_all_buttons_release_draw		; all at once.
		RTS
.draw_simple_button_press:
		CMP		#$10							; if you press buttons fast, you may
		BNE		loc_246AC						; reset the nt buttons animation and it won't release
		JSR		_all_buttons_release_draw		; so force all nt buttons to clear first
loc_246AC:
		LDA		#$A0							; this is a sprite effect
		STA		_spr_insert_args._idx
		LDA		_tmp6DC+4
		STA		_spr_insert_args._pos._COL
		LDA		_tmp6DC+5
		STA		_spr_insert_args._pos._ROW
		JSR		_spr_lib_attr_insert			; draw common button frame sprite
		LDX		#$FD							; then manually hack its
		LDA		_tmp6DC+6						; middle tile to display a corresponding
		CMP		#$08							; letter in it
		BEQ		loc_246C9						; test for BACK button tile
		ORA		#$80							; regular tiles in ascii+80
		TAX
loc_246C9:
		STX		_spr_buf+$19					; hacky hacky. they know that sprite here
		LDA		_tmp6DC+6						; always at fixed position
		SEC										; extract number buttons with another attr
		SBC		#$30
		CMP		#$0A
;		BCS		locret_246DB					; WHY? why toggle attrs? why not keep them?
		BCC		locret_246DB					; FIX: same attrs (only CL swapped, because not sprite)
		LDA		#$02
		STA		_spr_buf+$1A
locret_246DB:
		RTS

; =============== S U B	R O U T	I N E =======================================
_all_buttons_release_draw:
;		JSRXY	_rle_nt_res_queue,_tlm_nt_all_buttons_release
;		JSRXY	_rle_extnt_res_queue,_tlm_extnt_all_buttons_release
		LDX		#$38
		JSR		_tlm_lib_queue					; OPTIMIZED
		JMP		_buffers_test_wait_flush

; =============== S U B	R O U T	I N E =======================================
_name_input_cursor_move_control:
		LDA		_pad0_autorep			; here is the calcs for selecting keyboard buttons on the screen
		AND		#$0F
		BEQ		loc_246E6
		APUA_SE	_SE_IDX_SELECT					; if any button pressed, do sound
loc_246E6:
		LDX		_tmp6D2							; load cur button idx in X/Y
		LDY		_tmp6D3
		LDA		_pad0_autorep			; check the actual button pressed
		LSR
		BCC		.name_cur_left
		INX										; right pressed is here
		CPX		#$0A
		BNE		loc_24700						; test for max value 0-9
		CPY		#$00
		BEQ		loc_24700						; the special cases for enter/space and clear
		CPY		#$02
		BNE		loc_246FF						;   0 1 2 3 4 5 6 7 8 9 A B
		INX										; 0 x x x x x x x x x x x x	(0-B, wrap 0)
loc_246FF:										; 1 x x x x x x x x x x - x (0-9,B, wrap 0)
		INX										; 2 x x x x x x x x x x - - (0-9, wrap 0)
loc_24700:										; 3 x x x x x x x x x x - x (0-9,B, wrap 0)
		CPX		#$0C
		BNE		.name_cur_left
		LDX		#$00
.name_cur_left:
		LSR
		BCC		loc_2471C
		DEX
		BPL		loc_24713
		LDX		#$0B
		CPY		#$02
		BNE		loc_24713
		DEX
loc_24713:
		CPX		#$0A
		BNE		loc_2471C
		CPY		#$00
		BEQ		loc_2471C
		DEX
loc_2471C:
		LSR
		BCC		loc_24732
		INY
		CPX		#$0A
		BCC		loc_2472C
		BNE		loc_24727
		INX
loc_24727:
		CPY		#$02
		BNE		loc_2472C
		INY
loc_2472C:
		CPY		#$04
		BNE		loc_24732
		LDY		#$00
loc_24732:
		LSR
		BCC		loc_24746
		DEY
		BPL		loc_2473A
		LDY		#$03
loc_2473A:
		CPX		#$0A
		BCC		loc_24746
		BNE		loc_24741
		INX
loc_24741:
		CPY		#$02
		BNE		loc_24746
		DEY
loc_24746:
		STX		_tmp6D2
		STY		_tmp6D3
		TXA
		ASL										; load cursor positions
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
_name_input_char_select:
		LDA		_pad0_press
		BPL		.no_a_pushed
		APUA_SE	_SE_IDX_CLICK					; OPTIMIZED
		LDA		_tmp6D3							; Y * 12 + X
		ASL
		ADC		_tmp6D3
		ASL
		ASL
		ADC		_tmp6D2
		TAY
		LDA		_input_chars_tbl,Y				; get keyboard char idx
		STA		_tmp6DC+1						; char display tmp var init
		STA		_tmp6DC+6						; copy for extnt
		BPL		.no_enter_pushed				; enter has code $FF
		LDY		#$00
		STY		_tmp6DC+1						; so reset previous chars
		INY
		STY		_tmp6DC+6
;		APUA_SE	_SE_IDX_CLICK					; REDUNDANT, make sound (why twice?)
		LDA		#$08
		STA		_tmp6DC+3						; delay counter load
;		JSRXY	_rle_nt_res_queue,_tlm_nt_end_press	; queue enter button push animation
;		JSRXY	_rle_extnt_res_queue,_tlm_extnt_end_press
		LDX		#$34
		JSR		_tlm_lib_queue					; OPTIMIZED
		JSR		_buffers_test_wait_flush
		JMP		_name_input_next_scr
.no_enter_pushed:
		LDA		_tmp6CC							; prepare button push parameters
		SEC
		SBC		#$04
		STA		_tmp6DC+4
		LDA		_tmp6CE
		SEC
		SBC		#$04
		STA		_tmp6DC+5
		LDA		#$10
		STA		_tmp6DC+3
;		APUA_SE	_SE_IDX_CLICK					; REDUNDANT
		RTS
.no_a_pushed:
		LDA		#$00
		STA		_tmp6DC+1
		RTS

_input_chars_tbl:
		.BYTE	$31,$32,$33,$34,$35,$36,$37,$38,$39,$30,$08,$0C
		.BYTE	$51,$57,$45,$52,$54,$59,$55,$49,$4F,$50,$00,$20
		.BYTE	$41,$53,$44,$46,$47,$48,$4A,$4B,$4C,$3A,$00,$00
		.BYTE	$5A,$58,$43,$56,$42,$4E,$4D,$2C,$2E,$2D,$00,$FF

; =============== S U B	R O U T	I N E =======================================
_name_input_char_store:
		LDA		_tmp6DC+1
		BEQ		locret_24842					; if not zero, we have char to store
		LDX		_tmp6DC+2						; get the name buffer idx
		LDA		_name_store_buf_lo,X
		STA		_ptr0
		LDA		#>[_city._name]
		STA		_ptr0+1							; get the ptr
		LDY		#$00
		LDA		(_ptr0),Y
		STA		_tmp2							; store current length of name
		LDA		_tmp6DC+1
		CMP		#$08
		BNE		loc_24821						; code = 08?
		LDY		_tmp2							; yes, do erase char
		DEY
		BEQ		locret_24842
		LDA		#$7C							; put empty space into current char
		STA		(_ptr0),Y
;		TYA										; REDUNDANT, update name length
;		LDY		#$00
;		STA		(_ptr0),Y
;		BEQ		locret_24842
		BNE		.length_update					; OPTIMIZED
loc_24821:
		CMP		#$0C
		BNE		loc_24834						; code = 0C?
		LDA		#$7C							; yes, do clear all
		LDY		#$0A
loc_24829:
		STA		(_ptr0),Y						; just fill with 7C
		DEY
		BNE		loc_24829
		LDA		#$01
		STA		(_ptr0),Y						; and store length 1 (+1 of name length always)
		BNE		locret_24842
loc_24834:
		LDY		_tmp2							; final test if we at the end of the buffer
		CPY		#$0B
		BEQ		locret_24842
		STA		(_ptr0),Y						; if not, store char
		INY
.length_update:
		TYA										; update length as well
		LDY		#$00
		STA		(_ptr0),Y
locret_24842:
		RTS

; =============== S U B	R O U T	I N E =======================================
;_name_input_char_nt_queue:						; REDUNDANT
_name_input_name_buf_queue:
		LDX		_tmp6DC+2						; get the name idx we entering
		LDA		_name_store_buf_lo,X			; get the corresponding ram ofs
		STA		_ptr0
		LDA		#>[_city._name]
		STA		_ptr0+1
		PPUQSTART
;		LDY		_tmp6DC+2
;		LDA		byte_248BF,Y					; REDUNDANT
;		PPUQMOVA
		PPUQMOVB	#$0B,OP_NOP,0				; OPTIMIZED, fixed
;		LDA		byte_248C1,Y					; REDUNDANT
;		PPUQMOVA
		PPUQMOVB	#$21,OP_NOP,0				; OPTIMIZED, fixed
		PPUQMOVB	#$0A,OP_NOP,0
		LDY		#$01
loc_2486A:
		LDA		(_ptr0),Y						; read full name to queue
		PPUQMOVA
		INY
		CPY		#$0B
		BNE		loc_2486A
		PPUQEND
;		RTS										; REDUNDANT
;
; =============== S U B	R O U T	I N E =======================================
;_name_input_char_extnt_queue:
;		LDX		_tmp6DC+2						; already in _ptr0 here
;		LDA		_name_store_buf_lo,X
;		STA		_ptr0
;		LDA		#>[_city._name]
;		STA		_ptr0+1
		EXTQSTART
;		LDY		_tmp6DC+2						; REDUNDANT
;		LDA		byte_248BF,Y
;		EXTQMOVA
		EXTQMOVB	#$0B,OP_NOP,0				; OPTIMIZED
;		LDA		byte_248C1,Y					; REDUNDANT
;		CLC
;		ADC		#$3C
;		EXTQMOVA
		EXTQMOVB	#$5D,OP_NOP,0				; OPTIMIZED
		EXTQMOVB	#$0A,OP_NOP,0
		LDY		#$01
loc_248A2:
		STX		_tmp3							; ppu queue idx backup
		LDX		#$8B
		LDA		(_ptr0),Y						; here we just need to replace
		CMP		#$7C							; 7C is a empty char filler, "-"
		BEQ		loc_248AE
		LDX		#$0B							; the rest uses 0B extnt
loc_248AE:
		TXA
		LDX		_tmp3
		EXTQMOVA								; store
		INY
		CPY		#$0B
		BNE		loc_248A2
		EXTQEND
		RTS

_name_store_buf_lo:
		.BYTE	<[_city._name]
		.BYTE	<[_city._sister_name]
; REDUNDANT, always the same
;byte_248BF:
;		.BYTE	$0B,$0B
;byte_248C1:
;		.BYTE	$21,$21

; =============== S U B	R O U T	I N E =======================================
_game_setup03_continue_init:
		INC		_game_setup_sub_idx				; dummy leftover
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup04_continue_input:
		INC		_main_sub_idx					; this routine stops the music
		LDA		#_MUS_IDX_STOP					; fade out the screen and go to
		STA		_apu_mus_idx_req				; the main game routine.
		JMP		_pal_fade_out

; =============== S U B	R O U T	I N E =======================================
_game_setup09_practice_msg_init:
; REDUNDANT
;		JSR		_ppu_queues_reset
;		JSR		_pal_fade_out
;		JSR		_render_off
;		LDY		#$80
;		STY		_game_wnd_spr_mode_flag
;		STY		_ppu_ctrl_shadow
;		LDY		#$01
;		STY		_scr_mode_idx
;		INY
;		STY		_scr_res_idx
;		DEC		_mmc5_operation_in_progress_flag
;		LDA		#$08
;		STA		_mmc5_nt_mode_shadow
;		STA		_MMC5_NT_MODE
;		JSR		_spr_clear
; -
		JSR		_game_setup_screen_init			; OPTIMIZED
; -
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_fullscreen_wnd	; unpack common full screen window
;		MOVWO	_ptr0,_tlm_res_practice_wnd_header				; and particular window header separately
;		JSR		_res_ppu_unpack
		JSRXY	_res_ppu_unpack,_tlm_res_practice_wnd_header
; OPTIMIZED, library idx 0A, 0B now
		LDA		_tmp6D5
		CLC
		ADC		#$0A
		JSR		_msg_lib_load					; prepare one of two pactice msgs
; -
; REDUNDANT
;		LDXY	_practice_msg0
;		LDA		_tmp6D5
;		BEQ		loc_24AB0
;		LDXY	_practice_msg1
;loc_24AB0:
;		STX		_tmp6D6
;		STY		_tmp6D6+1
;		LDY		#$00
;		STY		_tmp6DA
;		STY		_tmp6D9
;		DEY
;		STY		_tmp6D8
		LDA		#$08
		STA		_screen_pal_cur_bg_idx
; REDUNDANT
;		INC		_game_setup_sub_idx
;		JSR		_render_on						; return screen back, fade in
;		INC		_mmc5_operation_in_progress_flag
;		JMP		_pal_fade_in
		JMP		_game_setup_screen_start

; =============== S U B	R O U T	I N E =======================================
_game_setup0A_practice_msg_draw:
		JSR		_msg_print_out_handler			; practice mode briefing display
		BIT		_pad0_press
		BVC		loc_24AE4
		LDA		#$02							; press B to return to the title menu
		STA		_title_sub_idx
		DEC		_main_sub_idx
		JMP		_pal_fade_out
loc_24AE4:
		BPL		locret_24B0D
		APUA_SE	_SE_IDX_CLICK					; press A to skip briefing
		LDA		_tmp6D5							; there are two briefing screens
		BNE		loc_24AF6						; if current is the first one
		INC		_tmp6D5							; increment briefing idx and
		DEC		_game_setup_sub_idx				; warp to the briefing init screen again
		RTS
loc_24AF6:
		LDA		#$08							; or else start a new game, special
		STA		_scenario_idx					; scenario index map 8
		JSR		_scenario_map_load
		LDA		#$00
		STA		_scenario_idx					; return back index to 0
		INC		_main_sub_idx					; start a new game
		LDA		#_MUS_IDX_STOP
		STA		_apu_mus_idx_req
;		JMP		_pal_fade_out					; REDUNDANT, already fade by map load
locret_24B0D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup0B_scenario_over_init:
; REDUNDANT
;		JSR		_ppu_queues_reset
;		JSR		_pal_fade_out
;		JSR		_render_off
;		LDY		#$80
;		STY		_game_wnd_spr_mode_flag
;		STY		_ppu_ctrl_shadow
;		LDY		#$01
;		STY		_scr_mode_idx
;		INY
;		STY		_scr_res_idx
;		DEC		_mmc5_operation_in_progress_flag
;		LDA		#$08
;		STA		_mmc5_nt_mode_shadow
;		STA		_MMC5_NT_MODE
;		JSR		_spr_clear
; -
		JSR		_game_setup_screen_init			; OPTIMIZED
; -
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_fullscreen_wnd	; draw common full-screen window
		LDA		_scenario_result_idx			; actually, this window already have "YOU ARE A WINNER!"
		CMP		#$01							; header which overwriting with other header elswhere
		BEQ		loc_24B49						; but here it is already there
;		MOVWO	_ptr0,_tlm_res_scenario_loose_header; so we need another header header only for lose scenario
;		JSR		_res_ppu_unpack
		JSRXY	_res_ppu_unpack,_tlm_res_scenario_loose_header
loc_24B49:
		LDA		_scenario_result_idx
		CMP		#$01
		BNE		loc_24B6C						; if scenario completed, update completion flags in sram
		JSR		_sram_write_enable
		LDY		_scenario_idx
		LDA		_city._scenarios_clear_flags
		ORA		_bitmasks,Y
		STA		_city._scenarios_clear_flags
		STA		_sram_vars._scenarios_clear_flags
		FJSRA	_sram_sum_reinit,SRAM,PRG3
		JSR		_sram_write_disable
loc_24B6C:
; OPTIMIZED, lib indexes are 8, 9, but _scenario_result_idx should be -1
; so +7
		LDA		_scenario_result_idx
		CLC
		ADC		#$07
		JSR		_msg_lib_load
; REDUNDANT
;		LDA		_final_msg_lib_lo,Y
;		STA		_tmp6D6
;		LDA		_final_msg_lib_hi,Y
;		STA		_tmp6D6+1
;		LDY		#$00
;		STY		_tmp6DA
;		STY		_tmp6D9
;		DEY
;		STY		_tmp6D8
		LDA		#$10
		STA		_screen_pal_cur_bg_idx
; REDUNDANT
;		INC		_game_setup_sub_idx
;		JSR		_render_on
;		INC		_mmc5_operation_in_progress_flag
;		JMP		_pal_fade_in
		JMP		_game_setup_screen_start

; =============== S U B	R O U T	I N E =======================================
_game_setup0C_scenario_over_input:
		JSR		_msg_print_out_handler			; similar header as for practice screen
		LDA		_pad0_press				; but A/B skips alltogether
		AND		#$C0
		BEQ		locret_24BC1
		LDA		#$00
		STA		_scenario_result_idx			; reset scenario results for new game
		LDX		#$05
		LDA		_city._all_scenarios_are_clear_ram_flag	; if we already beat all, just select another scenario
		BNE		loc_24BBE
		LDA		_city._scenarios_clear_flags	; else check if we beat all by bitmasks
		CMP		#$3F
		BNE		loc_24BBE
		LDX		#$0F							; if yes, then jump to ending
loc_24BBE:
		STX		_game_setup_sub_idx
locret_24BC1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_setup0D_difficult_init:
		JSR		_ppu_queues_reset				; much more simpler game setup
		JSR		_pal_fade_out
		JSR		_render_off
		DEC		_mmc5_operation_in_progress_flag
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_game_play_level
		JSR		_difficult_scr_cursor_draw
		LDA		#$00
		STA		_tmp6D0+1
		STA		_tmp6D0
; REDUNDANT
;		INC		_game_setup_sub_idx
;		JSR		_render_on
;		INC		_mmc5_operation_in_progress_flag
;		JMP		_pal_fade_in
		JMP		_game_setup_screen_start

; =============== S U B	R O U T	I N E =======================================
_game_setup0E_difficult_input:
		JSR		_difficult_scr_cursor_move		; cursor movement input test
		JSR		_common_select_cursor_control	; cursor draw
		BIT		_pad0_press
		BPL		loc_2493D						; press A button
		APUA_SE	_SE_IDX_CLICK
; REDUNDANT
;		LDA		_city._name						; test for default names
;		CMP		#$01
;		BNE		loc_24916						; test if city name is empty
;		LDY		#$08							; get default name "SIMCITY"
;		LDX		_scenario_titles_ofs_lib,Y
;		LDY		#$01
;loc_24907:
;		LDA		_scenario_town_names_list,X		; write default name to buffer
;		BMI		loc_24913
;		STA		_city._name,Y
;		INX
;		INY
;		BNE		loc_24907
;loc_24913:
;		STY		_city._name
;loc_24916:
; -
		LDX		#$08							; OPTIMIZED, default city name, SIMCITY
		LDY		#$00
		JSR		_set_default_name
; -
; REDUNDANT
;		LDA		_city._sister_name				; the same for sister city name
;		CMP		#$01
;		BNE		loc_24933
;		LDY		#$08
;		LDX		_scenario_titles_ofs_lib,Y		; in this mode only it has a value
;		LDY		#$01
;loc_24924:
;		LDA		_scenario_town_names_list,X
;		BMI		loc_24930
;		STA		_city._sister_name,Y
;		INX
;		INY
;		BNE		loc_24924
;loc_24930:
;		STY		_city._sister_name
; -
		LDX		#$08							; OPTIMIZED, default sister city name, SIMCITY
		LDY		#$0B
		JSR		_set_default_name
; -
loc_24933:
;		INC		_main_sub_idx					; REDUNDANT, go to the main game, with fade out
;		LDA		#_MUS_IDX_STOP
;		STA		_apu_mus_idx_req
;		JMP		_pal_fade_out
		JMP		_game_setup04_continue_input	; OPTIMIZED
loc_2493D:
		BVC		locret_24944					; press the B button
		LDA		#$01
		STA		_game_setup_sub_idx				; return to name entry
locret_24944:
		RTS

; OPTIMIZED, merged common routine, may be used for other names initialization
; further
;
; =============== S U B	R O U T	I N E =======================================
_set_default_name:
		LDA		_city._name,Y					; test for default names
		CMP		#$01
		BNE		locret_24916					; test if city name is empty
		TYA										; backup start of name buffer
		PHA
		LDA		_scenario_titles_ofs_lib,X		; load ofs for name in library
		TAX
		INY
loc_249070:
		LDA		_scenario_town_names_list,X		; write default name to buffer
		BMI		loc_249130
		STA		_city._name,Y
		INX
		INY
		BNE		loc_249070
loc_249130:
		PLA										; restore name buf start
		TAX										; X = start of name buf again
		TYA										; A = Y = name len
		STA		_city._name,X
locret_24916:
		RTS

; =============== S U B	R O U T	I N E =======================================
_difficult_scr_cursor_move:
		LDA		_pad0_press				; up-down test
		AND		#$0C
		BEQ		locret_2496C
		LDY		_city._difficult				; change difficult up down
		CMP		#$04
		BNE		.diff_up						; test for button
		CPY		#$02							; down pressed, test if max value already
		BEQ		locret_2496C
		INY										; increment menu option
		BNE		.diff_store
.diff_up:
		CPY		#$00
		BEQ		locret_2496C					; for up test min value 0
		DEY										; if not, decrease
.diff_store:
		APUA_SE	_SE_IDX_CLICK					; play se
		STY		_city._difficult				; common store
		JMP		_difficult_scr_cursor_draw		; highlit difficult option
locret_2496C:
		RTS

; =============== S U B	R O U T	I N E =======================================
_difficult_scr_cursor_draw:
		EXTQSTART								; menu highlighted with extnt codes
		LDY		#$02							; draw all three options at a time
loc_24971:
		LDA		_diff_scr_extnt_ofs_list_lo,Y	; load pos for cur line
		EXTQMOVA
		LDA		_diff_scr_extnt_ofs_list_hi,Y
		EXTQMOVA
		LDA		_diff_scr_extnt_len_list,Y		; load len for cur line (easy/hard are shorter)
		EXTQMOVA
		STA		_tmp2							; store size as well as tmp counter
		LDA		#$8B
		CPY		_city._difficult				; select extnt byte for highlighted option
		BNE		loc_24991
		LDA		#$4B							; default byte for other options
loc_24991:
		EXTQMOVA
		DEC		_tmp2
		BNE		loc_24991						; fill
		DEY
		BPL		loc_24971						; repeat for all options
		EXTQEND
		LDY		_city._difficult
		LDA		_diff_scr_cursor_pos_list,Y		; set the cursor position for an option
		STA		_tmp6CE
		LDA		#$50
		STA		_tmp6CC
		JMP		_common_select_cursor_control	; draw cursor

_diff_scr_extnt_ofs_list_lo:
		.BYTE	<[_MMC5_EXRAM+$1A6]
		.BYTE	<[_MMC5_EXRAM+$1E6]
		.BYTE	<[_MMC5_EXRAM+$226]
_diff_scr_extnt_ofs_list_hi:
		.BYTE	>[_MMC5_EXRAM+$1A6]
		.BYTE	>[_MMC5_EXRAM+$1E6]
		.BYTE	>[_MMC5_EXRAM+$226]
_diff_scr_extnt_len_list:
		.BYTE	$04,$06,$04
_diff_scr_cursor_pos_list:
		.BYTE	$6E,$7E,$8E

; =============== S U B	R O U T	I N E =======================================
_game_setup0F_all_complete_init:
; REDUNDANT
;		JSR		_ppu_queues_reset
;		JSR		_pal_fade_out
;		JSR		_render_off
;		LDY		#$80
;		STY		_game_wnd_spr_mode_flag
;		STY		_ppu_ctrl_shadow
;		LDY		#$01
;		STY		_scr_mode_idx
;		INY
;		STY		_scr_res_idx
;		DEC		_mmc5_operation_in_progress_flag
;		LDA		#$08
;		STA		_mmc5_nt_mode_shadow
;		STA		_MMC5_NT_MODE
;		JSR		_spr_clear
; -
		JSR		_game_setup_screen_init			; OPTIMIZED
; -
		JSRXY	_res_ppu_lz_unpack,_tlm_res_lz_fullscreen_wnd	; this is a game complete
;		MOVWO	_ptr0,_tlm_res_all_complete						; screen
;		JSR		_res_ppu_unpack
		JSRXY	_res_ppu_unpack,_tlm_res_all_complete
; OPTIMIZED, msg idx 0C
		LDA		#$0C
		JSR		_msg_lib_load					; all complete message load
; REDUNDANT
;		MOVWO	_tmp6D6,_all_scenarios_clear_msg
;		LDY		#$00
;		STY		_tmp6DA
;		STY		_tmp6D9
;		DEY
;		STY		_tmp6D8
		JSR		_sram_write_enable
		LDA		#$01
		STA		_city._all_scenarios_are_clear_ram_flag		; set all complete global flag
		STA		_sram_vars._all_scenarios_are_clear_ram_flag; now it's stored in sram forever
		FJSRA	_sram_sum_reinit,SRAM,PRG3
		JSR		_sram_write_disable
		LDA		#$08
		STA		_screen_pal_cur_bg_idx
; REDUNDANT
;		INC		_game_setup_sub_idx
;		JSR		_render_on
;		INC		_mmc5_operation_in_progress_flag
;		JMP		_pal_fade_in
		JMP		_game_setup_screen_start

; =============== S U B	R O U T	I N E =======================================
_game_setup10_all_complete_input:
		JSR		_msg_print_out_handler			; the same as for scenario complete screen
		LDA		_pad0_press				; A/B skips message
		AND		#$C0
		BEQ		locret_24C44
		LDA		#$00
		STA		_scenario_result_idx			; but now you will jump to scenario screen
		LDA		#$05							; in any case
		STA		_game_setup_sub_idx
locret_24C44:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_vars_init:
		JSR		_sram_write_enable
		LDX		#$00							; X=A=0
		TXA
loc_1043C4:
		STA		_graphs,X						; graphs data clear
		INX
		CPX		#$78
		BNE		loc_1043C4
		JSR		_sram_write_disable
		LDX		#$00							; A=0 still here
loc_1043D1:
		STA		_city._level,X					; clear the RAM range including
		INX										; most of calculated values
		CPX		#[_tmp6CC-_city._level]			; for city, the rest is tmp values
		BNE		loc_1043D1
		STA		_obj_active_flags				; A=0 still!
		LDA		#$FF
		STA		_city._year_to_bank				; special years clear
		STA		_city._year_to_bank+1
		STA		_city._year_to_expo
		STA		_city._year_to_expo+1
		STA		_city._year_to_liberty
		STA		_city._year_to_liberty+1
; FIX, initialize correctly to hide 0 percents from the list at the beginning
		STA		_worst_problem_percents
		STA		_worst_problem_percents+1
		STA		_worst_problem_percents+2
		STA		_worst_problem_percents+3
		STA		_worst_problem_list
		STA		_worst_problem_list+1
		STA		_worst_problem_list+2
		STA		_worst_problem_list+3
; -
; FIX, moved here from PRGF to allow to save this values as a
; save state as well. init them here by default for all new game
; modes, but will load them from save state if needed
		LDX		#$0B
loc_18ABB5:
		LDA		_gui_ctrl_menu_pos_data,X		; copy initial positions for
		STA		_city._menu_pos,X				; game configure windows
		DEX
		BPL		loc_18ABB5
; -
		LDX		_cur_game_mode					; dependent from the game mode
		BNE		loc_1043FF						; the last place we need it ZERO
		FJSRA	_sram_data_load,SRAM,PRG3		; load saved city data, reload _game_vars_init
		JMP		.complete_game_setup			; skip to the final inits
loc_1043FF:
		DEX
		BNE		loc_104428
		LDY		_city._difficult				; game mode 1, new city
		LDA		_difficult_money_list_lo,Y
		STA		_city._money
		LDA		_difficult_money_list_hi,Y
		STA		_city._money+1
		MOVWI	_city._cur_year,1900			; start 1900 year only
;		LDA		#$09							; REDUNDANT, merged
;		STA		_city._game_flags				; setup default game settings
;		LDA		#$00							; REDUNDANT, already 0 here
;		STA		_city._city_type				; city initial type (town)
;		JMP		.complete_game_setup
		JMP		.default_game_settings
loc_104428:
		DEX
		BNE		.practice_init					; game mode 2, scenario
; FIX, continues
; -
		LDA		#$00							; manually set default difficult
		STA		_city._difficult
; -
		LDA		#$02
		STA		_city._game_speed				; default speed "slow" for all scenarios
		MOVWI	_city._money,20000				; fixed money
; BUG: there is no data for 8th scenario "FREEDOM" at all.
;
		LDY		_scenario_idx
; FIX, let's set defaults of this scenario to Practice mode defaults
; -
		CPY		#$07
		BEQ		.practice_init					; special "FREEDOM" case
; -
		LDA		_scenario_start_year_hi,Y		; different starting years
		STA		_city._cur_year+1
		LDA		_scenario_start_year_lo,Y		; init special disasters parameters
		STA		_city._cur_year					; starting year for first disaster,
		CLC										; and disasters treshold after that
		ADC		_city._disaster_ends_list,Y
		STA		_city._disaster_end				; low nibble of the last year of scenario
		LDA		_scenario_disasters_count_list,Y
		STA		_city._didaster_delay
		LDA		_scenario_city_type_list,Y
		STA		_city._city_type
		TYA
		ASL
		ASL
		TAY
		LDX		#$00
loc_10445D:
		LDA		_scenario_presents_flags_lib,Y	; disable already accepted present
		STA		_city._events_flags,X		; buildings to stop it's appearance
		INY
		INX
		CPX		#$04
		BNE		loc_10445D
;		LDA		#$09							; REDUNDANT, merged
;		STA		_city._game_flags				; default game settings
;		JMP		.complete_game_setup
		JMP		.default_game_settings
.practice_init:
; FIX, continues
; -
		LDA		#$00							; manually set default difficult
		STA		_city._difficult
; -
; FIX, move wnd pos initialization here, so we may save it in savestates as well
		MOVWI	_city._wnd_pos,$1B19
; -
		MOVWI	_city._money,20000				; game mode 3, practice
		MOVWI	_city._cur_year,1900			; starting year fixed
; REDUNDANT, already 0 here
;		LDA		#$00
;		STA		_city._city_type
.default_game_settings:
		LDA		#$09
		STA		_city._game_flags
.complete_game_setup:
		LDY		_city._city_type				; there is no city type setting for
		LDA		_city._game_flags				; scenario mode here, it will be set
		AND		#$08							; automatically after first recalc ingame
		BEQ		loc_1044A3						; so this music select just redundant,
		LDA		_bg_music_list,Y				; it will always select the default music
		STA		_apu_mus_idx_req				; anyway, this list actually empty by itself
loc_1044A3:
		LDA		#$07
		STA		_city._tax_rate					; default tax rate always 7
		STA		_budget_fund_alloc._tax
		LDA		#$0F
		STA		_fund_rate_police				; def funding rate, will be recalculated anyways
		STA		_fund_rate_fire					; mostly redundant here too
		LDA		#$20
		STA		_fund_rate_trans
		LDA		#$26
		STA		_city_center_pos._COL
		STA		_city_center_pos._ROW
		JSR		_sram_write_enable
		MOVWO	_tmpE6,_power_map				; clear power grid + traffic maps
; REDUNDANT, for some reason they clear $570 instead of full $5A4
;		LDX		#$05
;		LDY		#$00
;		TYA
;loc_1044D0:
;		STA		(_tmpE6),Y
;		INY
;		BNE		loc_1044D0
;		INC		_tmpE7
;		DEX
;		BNE		loc_1044D0
;loc_1044DA:
;		STA		(_tmpE6),Y
;		INY
;		CPY		#$70
;		BNE		loc_1044DA
; -
; OPTIMIZED
		LDX		#>[_traffic_map_end-_power_map]
		LDY		#<[_traffic_map_end-_power_map]
		JSR		_clear_buf
		MOVWO	_tmpE6,_land_val_map			; clear the rest of the statistic maps
; -
; REDUNDANT
;		LDX		#$03
;		LDY		#$00
;		TYA
;loc_1044EE:
;		STA		(_tmpE6),Y
;		INY
;		BNE		loc_1044EE
;		INC		_tmpE7
;		DEX
;		BNE		loc_1044EE
;loc_1044F8:
;		STA		(_tmpE6),Y
;		INY
;		CPY		#$ED
;		BNE		loc_1044F8
; -
; OPTIMIZED
		LDX		#>[_fire_map_end-_land_val_map]	; size is correct this time
		LDY		#<[_fire_map_end-_land_val_map]
		JSR		_clear_buf
; -
		JSR		_sram_write_disable
		JMP		_city_stats_reset				; ALERT! _city_stats_reset used here from PRGB as well!

_gui_ctrl_menu_pos_data:
		.BYTE	$02,$07
		.BYTE	$04,$07
		.BYTE	$06,$07
		.BYTE	$08,$07
		.BYTE	$0A,$07
		.BYTE	$0C,$07
_difficult_money_list_lo:
		.BYTE	<[20000]
		.BYTE	<[10000]
		.BYTE	<[5000]
_difficult_money_list_hi:
		.BYTE	>[20000]
		.BYTE	>[10000]
		.BYTE	>[5000]
_scenario_start_year_lo:
		.BYTE	<[1906]	; $72
		.BYTE	<[1965]	; $AD
		.BYTE	<[1972]	; $B4
		.BYTE	<[1961]	; $A9
		.BYTE	<[2010] ; $DA
		.BYTE	<[2047] ; $FF
		.BYTE	<[2100] ; $34
_scenario_start_year_hi:
		.BYTE	>[1906]	; $07
		.BYTE	>[1965]	; $07
		.BYTE	>[1972]	; $07
		.BYTE	>[1961]	; $07
		.BYTE	>[2010]	; $07
		.BYTE	>[2047]	; $07
		.BYTE	>[2100]	; $08
_scenario_disasters_count_list:
		.BYTE	$0A,$14,$05,$03,$05,$F0,$03
_city._disaster_ends_list:
		.BYTE	$05,$0A,$0A,$05,$05,$0A,$0A

; NOTE, here is the default flags for scenarios, most of usual alerts are
; disabled, most of scenarios have allowed only two town statuses update:
; METROPOLIS and MEGAPOLIS, and only "TOKYO" scenario allows only the last
; one.
; also for all scenarios allowed AMUSEMENT PARK, ZOO and CASINO alerts
; the rest are forbidden.
;
_scenario_presents_flags_lib:					; 7654 3210 FEDC BA98
		.BYTE	$0E,$1F,$FF,$FF					; 0000 1110 0001 1111 1111 1111 1111 1111
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$1E,$1F,$FF,$FF					; 0001 1110 0001 1111 1111 1111 1111 1111
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
_scenario_city_type_list:
		.BYTE	$03,$03,$03,$04,$03,$03,$03

; REDUNDANT, two functions to send another type of tilemaps: RLE-packed queue
; lists, both called in conjunction always so why we don't merge them and
; make same librarification as for other similar routines?
; this routine used only in this bank, so no need to store it in system bank
;
; usage of this routine here is kinda redundant, for small arrays it used
; just as method to determine the data size, not for pack it.. so it may
; be converted to regular _tlm_lib_queue reoutine instead.
;
; repeated bytes less than 5 in size aren't so optimal though, only 1 byte
; per 4 will be saved...
;
; TODO, all this stuff may be removed if we'd using the same rle codes for
; dequeue routines and store already rle packed data into the buffer
;
; =============== S U B	R O U T	I N E =======================================
;_rle_nt_res_queue:
_rle_tlm_queue:
		LDA		_rle_tlm_lib,X					; OPTIMIZED, librarification
		STA		_ptr0
		LDA		_rle_tlm_lib+1,X
		STA		_ptr0+1
		LDA		_rle_tlm_lib+2,X
		STA		_ptr4
		LDA		_rle_tlm_lib+3,X
		STA		_ptr4+1
;		STY		_ptr0+1							; store ptr in X/Y
;		STX		_ptr0
		LDY		#$00
		PPUQSTART
.nt_rle_fetch:
		LDA		(_ptr0),Y						; fetch RLE code
		BEQ		.nt_rle_stop
		BMI		.nt_rle_repeat
		INY										; bit 80 clear, copy code
		STA		_tmp2							; store copy counter
.nt_rle_copy:
		LDA		(_ptr0),Y						; copy data
		PPUQMOVA
		INY
		DEC		_tmp2
		BNE		.nt_rle_copy
		BEQ		.nt_rle_fetch					; fetch again
.nt_rle_repeat:
		INY										; bit 80 set, repeat mode
		AND		#$7F							; clear bit
		STA		_tmp2							; the rest is repeat counter
		LDA		(_ptr0),Y						; read byte to repeat
loc_24A1D:
		PPUQMOVA								; store
		DEC		_tmp2
		BNE		loc_24A1D
		INY
		BNE		.nt_rle_fetch					; fetch again
.nt_rle_stop:
		STA		_ppu_queue_buf,X				; store zero as stop contition for ppu queue
		PPUQEND									; exit
;		RTS
; REDUNDANT, merged
; =============== S U B	R O U T	I N E =======================================
;_rle_extnt_res_queue:
;		STY		_ptr0+1							; the same for extnt, except another buffer used
;		STX		_ptr0
		LDY		#$00
		EXTQSTART
loc_24A36:
		LDA		(_ptr4),Y
		BEQ		loc_24A5E
		BMI		loc_24A4C
		INY
		STA		_tmp2
loc_24A3F:
		LDA		(_ptr4),Y
		EXTQMOVA
		INY
		DEC		_tmp2
		BNE		loc_24A3F
		BEQ		loc_24A36
loc_24A4C:
		INY
		AND		#$7F
		STA		_tmp2
		LDA		(_ptr4),Y
loc_24A53:
		EXTQMOVA
		DEC		_tmp2
		BNE		loc_24A53
		INY
		BNE		loc_24A36
loc_24A5E:
		STA		_extnt_queue_buf,X
		EXTQEND
		RTS

_rle_tlm_lib:
		.WORD	_rle_tlm_nt_clear_area,_rle_tlm_extnt_clear_area	; 00
		.WORD	_rle_tlm_nt_input_name,_rle_tlm_extnt_input_name	; 04
		.WORD	_rle_tlm_nt_input_word,_rle_tlm_extnt_input_word	; 08

; OPTIMIZED, not RLE anymore, moved to simple tlm transfer routine instead.
_tlm_nt_back_button_press:
;		.BYTE	$0A		; RLE copy
		.BYTE	$1A,$22,$02,$C0,$C0,$3A,$22,$02,$C4,$DA
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_extnt_back_button_press:
;		.BYTE	$0A		; RLE copy
		.BYTE	$1A,$5E,$02,$0A,$0A,$3A,$5E,$02,$0A,$0A
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_nt_space_press:
;		.BYTE	$0B		; RLE copy
		.BYTE	$58,$22,$04,$C2,$C3,$C2,$C2,$78,$22,$01,$C1
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_extnt_space_press:
;		.BYTE	$0B		; RLE copy
		.BYTE	$58,$5E,$04,$0A,$0A,$0A,$0A,$78,$5E,$01,$0A
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_nt_end_press:
;		.BYTE	$15		; RLE copy
		.BYTE	$B8,$22,$04,$C0,$C0,$C0,$C0,$D8,$22,$04,$B0,$B1,$B2,$B3,$F8,$22,$04,$C0,$C1,$C2,$C3
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_extnt_end_press:
;		.BYTE	$15		; RLE copy
		.BYTE	$B8,$5E,$04,$0A,$0A,$0A,$0A,$D8,$5E,$04,$C8,$C8,$C8,$C8,$F8,$5E,$04,$C8,$C8,$C8,$C8
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_nt_all_buttons_release:
;		.BYTE	$2A		; RLE copy
		.BYTE	$1A,$22,$02,$C1,$C1,$3A,$22,$02,$43,$4C,$58,$22,$04,$2A,$4E,$2A
		.BYTE	$2A,$78,$22,$01,$DD,$B8,$22,$04,$C1,$C1,$C1,$C1,$D8,$22,$04,$A1
		.BYTE	$A2,$A3,$A4,$F8,$22,$04,$B1,$B2,$B3,$B4
		.BYTE	$FF
;		.BYTE	$00		; RLE stop
_tlm_extnt_all_buttons_release:
;		.BYTE	$2A		; RLE copy
		.BYTE	$1A,$5E,$02,$06,$06,$3A,$5E,$02,$8B,$8B,$58,$5E,$04,$07,$07,$07
		.BYTE	$07,$78,$5E,$01,$89,$B8,$5E,$04,$06,$06,$06,$06,$D8,$5E,$04,$CA
		.BYTE	$CA,$CA,$CA,$F8,$5E,$04,$CA,$CA,$CA,$CA
		.BYTE	$FF
;		.BYTE	$00		; RLE stop

; RLE data left
_rle_tlm_nt_clear_area:
		.BYTE	$03		; RLE copy
		.BYTE	$42,$21,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$03		; RLE code
		.BYTE	$22,$21,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$03		; RLE code
		.BYTE	$02,$21,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$03		; RLE code
		.BYTE	$E2,$20,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$03		; RLE code
		.BYTE	$C2,$20,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$03		; RLE code
		.BYTE	$A2,$20,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$03		; RLE code
		.BYTE	$82,$20,$1C
		.BYTE	$9C,$DD	; RLE repeat
		.BYTE	$00		; RLE stop
_rle_tlm_extnt_clear_area:
		.BYTE	$03		; RLE copy
		.BYTE	$42,$5D,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$03		; RLE copy
		.BYTE	$22,$5D,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$03		; RLE copy
		.BYTE	$02,$5D,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$03		; RLE copy
		.BYTE	$E2,$5C,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$03		; RLE copy
		.BYTE	$C2,$5C,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$03		; RLE copy
		.BYTE	$A2,$5C,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$03		; RLE copy
		.BYTE	$82,$5C,$1C
		.BYTE	$9C,$86	; RLE repeat
		.BYTE	$00		; RLE stop
_rle_tlm_nt_input_name:
		.BYTE	$04		; RLE copy
		.BYTE	$83,$20,$1A,$A5
		.BYTE	$98,$A6	; RLE repeat
		.BYTE	$43		; RLE copy
		.BYTE	$A7,$A3,$20,$1A,$B5,$DF,$86,$8C,$8C,$C5,$AB,$DF,$88,$8D,$C5,$AD
		.BYTE	$DF,$8A,$C2,$AB,$7B,$79,$C4,$DF,$AE,$AC,$C6,$87,$DF,$B7,$C3,$20
		.BYTE	$1A,$B5,$DF,$96,$9C,$89,$D5,$BB,$DF,$98,$9D,$D5,$BD,$DF,$9A,$D2
		.BYTE	$BB,$8B,$DF,$D4,$DF,$BE,$BC,$D6,$97,$DF,$B7,$E3,$20,$1A,$B5,$F9
		.BYTE	$F9,$F9,$99
		.BYTE	$8B,$F9	; RLE repeat
		.BYTE	$01		; RLE copy
		.BYTE	$7A
		.BYTE	$88,$F9	; RLE repeat
		.BYTE	$05		; RLE copy
		.BYTE	$B7,$03,$21,$1A,$B5
		.BYTE	$98,$DC	; RLE repeat
		.BYTE	$05		; RLE copy
		.BYTE	$B7,$23,$21,$1A,$B5
		.BYTE	$98,$DC	; RLE repeat
		.BYTE	$05		; RLE copy
		.BYTE	$B7,$43,$21,$1A,$C5
		.BYTE	$98,$C6	; RLE repeat
		.BYTE	$01		; RLE copy
		.BYTE	$C7
		.BYTE	$00		; RLE stop
_rle_tlm_extnt_input_name:
		.BYTE	$03		; RLE copy
		.BYTE	$83,$5C,$1A
		.BYTE	$9A,$CA	; RLE repeat
		.BYTE	$42		; RLE copy
		.BYTE	$A3,$5C,$1A,$CA,$01,$01,$01,$01,$00,$00,$01,$01,$01,$00,$00,$01
		.BYTE	$01,$00,$00,$01,$01,$00,$01,$00,$00,$00,$01,$01,$CA,$C3,$5C,$1A
		.BYTE	$CA,$01,$01,$01,$01,$00,$00,$01,$01,$01,$00,$00,$01,$01,$00,$00
		.BYTE	$01,$01,$00,$01,$00,$00,$00,$01,$01,$CA,$E3,$5C,$1A,$CA,$80,$80
		.BYTE	$80,$01
		.BYTE	$8B,$80	; RLE repeat
		.BYTE	$0E		; RLE copy
		.BYTE	$01,$80,$80,$80,$80,$80,$80,$80,$80,$CA,$03,$5D,$1A,$CA
		.BYTE	$98,$06	; RLE repeat
		.BYTE	$05		; RLE copy
		.BYTE	$CA,$23,$5D,$1A,$CA
		.BYTE	$98,$06	; RLE repeat
		.BYTE	$04		; RLE copy
		.BYTE	$CA,$43,$5D,$1A
		.BYTE	$9A,$CA	; RLE repeat
		.BYTE	$00		; RLE stop
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

		MAC		CNAME
{1}		EQU		[.-_scenario_town_names_list]
		.BYTE	{2},$FF
		ENDM

_scenario_titles_ofs_lib:
		.BYTE	_name_cisc
		.BYTE	_name_bern
		.BYTE	_name_detr
		.BYTE	_name_toky
		.BYTE	_name_bost
		.BYTE	_name_rio
		.BYTE	_name_vegs
		.BYTE	_name_free
		.BYTE	_name_sims

_scenario_town_names_list:
		CNAME	_name_cisc,	"CISCO"
		CNAME	_name_bern,	"BERN"
		CNAME	_name_detr,	"DETROIT"
		CNAME	_name_toky,	"TOKYO"
		CNAME	_name_bost,	"BOSTON"
		CNAME	_name_rio,	"RIO"
		CNAME	_name_vegs,	"LAS VEGAS"
		CNAME	_name_free,	"FREEDOM"
		CNAME	_name_sims,	"SIMCITY"

_tlm_res_scenario_hdr_lib_lo:
		.BYTE	<[_tlm_res_scenario_hdr0]
		.BYTE	<[_tlm_res_scenario_hdr1]
		.BYTE	<[_tlm_res_scenario_hdr2]
		.BYTE	<[_tlm_res_scenario_hdr3]
		.BYTE	<[_tlm_res_scenario_hdr4]
		.BYTE	<[_tlm_res_scenario_hdr5]
;		.BYTE	<[_tlm_res_scenario_hdr6]
		.BYTE	<[_tlm_res_scenario_hdr_dummy]
;		.BYTE	<[_tlm_res_scenario_hdr7]
		.BYTE	<[_tlm_res_scenario_hdr_dummy]
_tlm_res_scenario_hdr_lib_hi:
		.BYTE	>[_tlm_res_scenario_hdr0]
		.BYTE	>[_tlm_res_scenario_hdr1]
		.BYTE	>[_tlm_res_scenario_hdr2]
		.BYTE	>[_tlm_res_scenario_hdr3]
		.BYTE	>[_tlm_res_scenario_hdr4]
		.BYTE	>[_tlm_res_scenario_hdr5]
;		.BYTE	>[_tlm_res_scenario_hdr6]
		.BYTE	>[_tlm_res_scenario_hdr_dummy]
;		.BYTE	>[_tlm_res_scenario_hdr7]
		.BYTE	>[_tlm_res_scenario_hdr_dummy]

; OPTIMIZED, by now, unless you add actual headers and briefings
_tlm_res_scenario_hdr_dummy:
		.WORD	$8520
		.BYTE	$56,$00
		.WORD	$A520
		.BYTE	$56,$00
		.BYTE	0
; REDUNDANT
;_tlm_res_scenario_hdr6:
;		.WORD	$8520
;		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
;		.WORD	$A520
;		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
;		.BYTE	0
;_tlm_res_scenario_hdr7:
;		.WORD	$8520
;		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
;		.WORD	$A520
;		.BYTE	$16,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
;		.BYTE	0
_tlm_res_scenario_hdr0:
		.WORD	$8520
		.BYTE	$16,$13,$21,$2E,$00,$06,$32,$21,$2E,$23,$29,$33,$23,$2F,$00,$03,$01,$00,$00,$1C,$3F,$1B,$3C
		.WORD	$A520
		.BYTE	$16,$53,$61,$6E,$00,$46,$72,$61,$6E,$63,$69,$73,$63,$6F,$00,$43,$41,$BC,$00,$5C,$7F,$7C,$7C
		.BYTE	0
_tlm_res_scenario_hdr1:
		.WORD	$8620
		.BYTE	$15,$02,$25,$32,$2E,$9B,$13,$37,$29,$34,$3A,$25,$32,$2C,$21,$2E,$24,$00,$1C,$3F,$3C,$3B
		.WORD	$A620
		.BYTE	$15,$42,$65,$72,$6E,$BB,$53,$77,$69,$74,$7A,$65,$72,$6C,$61,$6E,$64,$00,$5C,$7F,$7C,$7B
		.BYTE	0
_tlm_res_scenario_hdr2:
		.WORD	$8820
		.BYTE	$10,$04,$25,$34,$32,$2F,$29,$34,$9B,$0D,$09,$9C,$1C,$3F,$3D,$1D,$00
		.WORD	$A820
		.BYTE	$10,$44,$65,$74,$72,$6F,$69,$74,$BB,$6D,$49,$BC,$5C,$7F,$7D,$5D,$00
		.BYTE	0
_tlm_res_scenario_hdr3:
		.WORD	$8820
		.BYTE	$10,$14,$2F,$2B,$39,$2F,$00,$0A,$21,$30,$21,$2E,$00,$1C,$3F,$3C,$1C
		.WORD	$A820
		.BYTE	$10,$54,$6F,$6B,$79,$6F,$BB,$4A,$61,$70,$61,$6E,$00,$5C,$7F,$7C,$5C
		.WORD	$C820
		.BYTE	$09,$94,$AF,$AB,$B9,$AF,$BA,$8A,$A1,$B0
		.BYTE	0
_tlm_res_scenario_hdr4:
		.WORD	$8820
		.BYTE	$10,$02,$2F,$23,$34,$2F,$2E,$9B,$0D,$01,$00,$00,$1D,$1B,$1C,$1B,$00
		.WORD	$A820
		.BYTE	$10,$42,$6F,$73,$74,$6F,$6E,$BB,$6D,$41,$BC,$00,$5D,$5B,$5C,$5B,$00
		.BYTE	0
_tlm_res_scenario_hdr5:
		.WORD	$8320
		.BYTE	$1A,$12,$29,$2F,$00,$24,$25,$00,$0A,$21,$2E,$25,$29,$32,$2F,$9B,$02,$32,$21,$3A,$29,$2C,$00,$1D,$1B,$1F,$3D
		.WORD	$A320
		.BYTE	$1A,$52,$69,$6F,$00,$64,$65,$00,$4A,$61,$6E,$65,$69,$72,$6F,$BB,$42,$72,$61,$7A,$69,$6C,$00,$5D,$5B,$5F,$7D
		.BYTE	0
_tlm_res_scenario_loose_header:
		.WORD	$8720
		.BYTE	$12,$09,$2D,$30,$25,$21,$23,$28,$2D,$25,$2E,$34,$00,$2E,$2F,$34,$29,$23,$25
		.WORD	$A720
		.BYTE	$12,$49,$6D,$70,$65,$61,$63,$68,$6D,$65,$6E,$74,$00,$6E,$6F,$74,$69,$63,$65
		.BYTE	0
_tlm_res_practice_wnd_header:
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
_tlm_res_all_complete:
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
_scenario_map_dicts_lib_lo:
		.BYTE	<[_scenario0_map_dict]
		.BYTE	<[_scenario1_map_dict]
		.BYTE	<[_scenario2_map_dict]
		.BYTE	<[_scenario3_map_dict]
		.BYTE	<[_scenario4_map_dict]
		.BYTE	<[_scenario5_map_dict]
		.BYTE	<[_scenario2_map_dict]
		.BYTE	<[_scenario0_map_dict]
		.BYTE	<[_scenario4_map_dict]
_scenario_map_dicts_lib_hi:
		.BYTE	>[_scenario0_map_dict]
		.BYTE	>[_scenario1_map_dict]
		.BYTE	>[_scenario2_map_dict]
		.BYTE	>[_scenario3_map_dict]
		.BYTE	>[_scenario4_map_dict]
		.BYTE	>[_scenario5_map_dict]
		.BYTE	>[_scenario2_map_dict]
		.BYTE	>[_scenario0_map_dict]
		.BYTE	>[_scenario4_map_dict]
_scenario0_map_dict:
		.BYTE	_MAP_GROUND,_MAP_GROUND
		.BYTE	_MAP_GROUND,_MAP_ROAD_UD
		.BYTE	_MAP_ROAD_UD,_MAP_GROUND
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_TCROSS_L
		.BYTE	_MAP_ROAD_TCROSS_R,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_UD,_MAP_RAIL_UD
		.BYTE	_MAP_ROAD_TCORSS_D,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_TCORSS_D
		.BYTE	_MAP_RAIL_UD,_MAP_GROUND
		.BYTE	_MAP_RAIL_UD,_MAP_ROAD_UD
		.BYTE	_MAP_GROUND,_MAP_RAIL_UD
		.BYTE	_MAP_WOODSD,_MAP_WOODS3
		.BYTE	_MAP_ROAD_BEND_RU,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_BEND_RD,_MAP_ROAD_LR
_scenario1_map_dict:
		.BYTE	_MAP_ROAD_CROSS,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_CROSS
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_LR
		.BYTE	_MAP_GROUND,_MAP_GROUND
		.BYTE	_MAP_WOODSC,_MAP_WOODSD
		.BYTE	_MAP_ROAD_UD,_MAP_GROUND
		.BYTE	_MAP_WOODS10,_MAP_WOODSD
		.BYTE	_MAP_GROUND,_MAP_ROAD_UD
		.BYTE	_MAP_WOODSD,_MAP_WOODS10
		.BYTE	_MAP_WOODSF,_MAP_WOODS10
		.BYTE	_MAP_WOODS10,_MAP_WOODS11
		.BYTE	_MAP_ROAD_TCROSS_U,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_TCORSS_D
		.BYTE	_MAP_WOODS9,_MAP_WOODSA
		.BYTE	_MAP_ROAD_TCROSS_R,_MAP_ROAD_LR
_scenario2_map_dict:
		.BYTE	_MAP_ROAD_CROSS,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_CROSS
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_LR
		.BYTE	_MAP_WATER,_MAP_WATER
		.BYTE	_MAP_GROUND,_MAP_GROUND
		.BYTE	_MAP_ROAD_LR,_MAP_ELECTRO_ROAD_UD
		.BYTE	_MAP_RAIL_UD,_MAP_ROAD_UD
		.BYTE	_MAP_WATER,_MAP_ELECTRO_TCROSS_L
		.BYTE	_MAP_ROAD_UD,_MAP_RAIL_UD
		.BYTE	_MAP_GROUND,_MAP_ROAD_UD
		.BYTE	_MAP_ELECTRO_TCROSS_L,_MAP_WATER
		.BYTE	_MAP_ROAD_RAIL_UD,_MAP_RAIL_LR
		.BYTE	_MAP_ROAD_RAIL_LR,_MAP_ROAD_CROSS
		.BYTE	_MAP_ROAD_UD,_MAP_GROUND
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_RAIL_LR
_scenario3_map_dict:
		.BYTE	_MAP_ROAD_LR,_MAP_ELECTRO_ROAD_UD
		.BYTE	_MAP_ROAD_CROSS,_MAP_ROAD_LR
		.BYTE	_MAP_ELECTRO_ROAD_UD,_MAP_ROAD_CROSS
		.BYTE	_MAP_GROUND,_MAP_GROUND
		.BYTE	_MAP_RAIL_LR,_MAP_ELECTRO_RAIL_UD
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_LR
		.BYTE	_MAP_RAIL_UD,_MAP_ROAD_UD
		.BYTE	_MAP_WATER,_MAP_WATER
		.BYTE	_MAP_WOODSC,_MAP_WOODSD
		.BYTE	_MAP_GROUND,_MAP_ROAD_UD
		.BYTE	_MAP_ROAD_UD,_MAP_ROAD_UD
		.BYTE	_MAP_WOODSD,_MAP_WOODSE
		.BYTE	_MAP_WOODSB,_MAP_GROUND
		.BYTE	_MAP_ROAD_TCROSS_U,_MAP_ROAD_LR
		.BYTE	_MAP_WOODSD,_MAP_WOODS10
_scenario4_map_dict:
		.BYTE	_MAP_GROUND,_MAP_GROUND
		.BYTE	_MAP_SHOREE,_MAP_SHORE6
		.BYTE	_MAP_SHORE6,_MAP_SHOREE
		.BYTE	_MAP_SHORE9,_MAP_SHORE1
		.BYTE	_MAP_WOODS3,_MAP_WOODS4
		.BYTE	_MAP_SHORE1,_MAP_SHORE9
		.BYTE	_MAP_WATER,_MAP_WATER
		.BYTE	_MAP_WOODSF,_MAP_WOODS10
		.BYTE	_MAP_WOODSC,_MAP_WOODSD
		.BYTE	_MAP_WOODS10,_MAP_WOODS11
		.BYTE	_MAP_WOODSA,_MAP_WOODSB
		.BYTE	_MAP_WOODSD,_MAP_WOODSE
		.BYTE	_MAP_SHOREF,_MAP_GROUND
		.BYTE	_MAP_SHOREB,_MAP_WATER
		.BYTE	_MAP_WOODS9,_MAP_WOODSA
_scenario5_map_dict:
		.BYTE	_MAP_WOODSF,_MAP_WOODS10
		.BYTE	_MAP_WOODSC,_MAP_WOODSD
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_UD,_MAP_GROUND
		.BYTE	_MAP_ROAD_UD,_MAP_ROAD_UD
		.BYTE	_MAP_ROAD_TCROSS_U,_MAP_ROAD_LR
		.BYTE	_MAP_WOODS10,_MAP_WOODS11
		.BYTE	_MAP_ELECTRO_ROAD_UD,_MAP_ROAD_LR
		.BYTE	_MAP_GRASS,_MAP_ROAD_UD
		.BYTE	_MAP_RAIL_UD,_MAP_ROAD_UD
		.BYTE	_MAP_GROUND,_MAP_ROAD_UD
		.BYTE	_MAP_WOODS10,_MAP_WOODSD
		.BYTE	_MAP_GRASS,_MAP_GRASS
		.BYTE	_MAP_ROAD_TCORSS_D,_MAP_ROAD_LR
		.BYTE	_MAP_ROAD_LR,_MAP_ROAD_TCORSS_D

_msg_lib_lo:
		.BYTE	<[_scenario_msg0]	; 00		; all messages in one lib now
		.BYTE	<[_scenario_msg1]	; 01
		.BYTE	<[_scenario_msg2]	; 02
		.BYTE	<[_scenario_msg3]	; 03
		.BYTE	<[_scenario_msg4]	; 04
		.BYTE	<[_scenario_msg5]	; 05
		.BYTE	<[_scenario_dummy]	; 06 *dummy*
		.BYTE	<[_scenario_dummy]	; 07 *dummy*
		.BYTE	<[_game_won_msg]	; 08
		.BYTE	<[_game_lose_msg]	; 09
		.BYTE	<[_practice_msg0]	; 0A
		.BYTE	<[_practice_msg1]	; 0B
		.BYTE	<[_all_scenarios_clear_msg]	; 0C
_msg_lib_hi:
		.BYTE	>[_scenario_msg0]
		.BYTE	>[_scenario_msg1]
		.BYTE	>[_scenario_msg2]
		.BYTE	>[_scenario_msg3]
		.BYTE	>[_scenario_msg4]
		.BYTE	>[_scenario_msg5]
		.BYTE	>[_scenario_dummy]
		.BYTE	>[_scenario_dummy]
		.BYTE	>[_game_won_msg]
		.BYTE	>[_game_lose_msg]
		.BYTE	>[_practice_msg0]
		.BYTE	>[_practice_msg1]
		.BYTE	>[_all_scenarios_clear_msg]

_eof	EQU		$5B
_quo	EQU		$22

		MAC		MSG_DIC
{1}		EQU		[[.-_msg_dictionary]|$80]
		.BYTE	{2},$FF
		ENDM

_msg_dictionary:
		MSG_DIC	_the,	"the"
		MSG_DIC	_to,	"to"
		MSG_DIC	_and,	"and"
		MSG_DIC	_of,	"of"
		MSG_DIC	_have,	"have"
		MSG_DIC	_city_,	"city"
		MSG_DIC	_You,	"You"
		MSG_DIC	_years,	"years"
		MSG_DIC	_in,	"in"
		MSG_DIC	_your,	"your"
		MSG_DIC	_you,	"you"
		MSG_DIC	_con,	"con"

; REDUNDANT
;_scenario_dummy:
;		.BYTE	_eof
;_scenario_dummy:
;		.BYTE	_eof

_scenario_msg0:
		.BYTE	" Damage from ",_the," earth-",$A
		.BYTE	"quake was m",_in,"or compared",$A
		.BYTE	_to," that ",_of," ",_the," ensu",_in,"g",$A
		.BYTE	"fires,which ",_to,"ok days ",_to,$A
		.BYTE	_con,"trol.1500 people died.",$A				; FIX: "1500[ ]people"
		.BYTE	" Controll",_in,"g ",_the," fires",$A
		.BYTE	"should be ",_your," ",_in,"itial",$A
		.BYTE	_con,"cern.Then clear ",_the,$A
		.BYTE	"rubble ",_and," start rebuild-",$A
		.BYTE	_in,"g.",_You," ",_have," 5 ",_years,".",$A
_scenario_dummy:
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
		.BYTE	"ture has been spotted",$A				; FIX: [spooted]->[spotted]
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
		.BYTE	_in,"ated by radiation,forc-",$A		; FIX: [rediation]->[radiation]
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
		.BYTE	"snarls,",_con,"trol enviro-",$A					; FIX: snarls[,]control
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

		SECTION_ENDS F0E, "PRGF GAME SETUP AND INIT ROUTINES"

		SECTION_START F0F

_tlm_res_lz_fullscreen_wnd:
		.WORD	$2000
		.BYTE	1
		.WORD	_pak_tlm_fullscreen_wnd0
_pak_tlm_fullscreen_wnd0:
		.BYTE	$E4,$61,$DD,$00,$A5,$39,$A6,$00,$A7,$23,$DD,$00,$B5,$24,$00,$0F
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
_tlm_res_lz_scenario_normal:
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
_tlm_res_lz_game_play_level:
		.WORD	$2000
		.BYTE	1
		.WORD	_pak_tlm_game_play_level0
_pak_tlm_game_play_level0:
		.BYTE	$E5,$03,$DD,$00,$A5,$35,$A6,$00,$A7,$27,$DD,$00,$B5,$23,$DF,$0E
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
_tlm_res_lz_name_init:
		.WORD	$2000
		.BYTE	1
		.WORD	_pak_tlm_name_init0
_pak_tlm_name_init0:
		.BYTE	$E5,$E1,$DD,$00,$60,$39,$61,$00,$62,$23,$DD,$01,$66,$C0,$4A,$C1
		.BYTE	$0C,$03,$C1,$C1,$C2,$67,$84,$01,$FE,$19,$D0,$31,$1C,$32,$1C,$33
		.BYTE	$1C,$34,$1C,$35,$1C,$36,$1C,$37,$1C,$38,$1C,$39,$1C,$30,$1C,$7D
		.BYTE	$1C,$43,$4C,$D2,$85,$02,$1D,$00,$3A,$49,$2A,$3C,$04,$2A,$4E,$2A
		.BYTE	$2A,$3D,$86,$02,$1D,$13,$51,$1C,$57,$1C,$45,$1C,$52,$1C,$54,$1C
		.BYTE	$59,$1C,$55,$1C,$49,$1C,$4F,$1C,$50,$1C,$23,$DD,$9A,$02,$3C,$00
		.BYTE	$29,$23,$E1,$00,$E2,$86,$02,$1D,$18,$41,$1C,$53,$1C,$44,$1C,$46
		.BYTE	$1C,$47,$1C,$48,$1C,$4A,$1C,$4B,$1C,$4C,$1C,$3A,$1A,$C1,$C1,$C1
		.BYTE	$C1,$C2,$99,$02,$3D,$00,$1D,$63,$A1,$86,$02,$3C,$14,$D0,$5A,$1C
		.BYTE	$58,$1C,$43,$1C,$56,$1C,$42,$1C,$4E,$1C,$4D,$1C,$2C,$1C,$2E,$1C
		.BYTE	$2D,$1C,$63,$B1,$86,$02,$3C,$00,$E0,$49,$E1,$2C,$89,$02,$98,$00
		.BYTE	$63,$39,$64,$00,$65,$E4,$81,$DD,$2B,$FF,$23,$00,$8F,$03,$C0,$9B
		.BYTE	$03,$C4,$23,$FF,$E5,$E1,$86,$3B,$06,$05,$86,$86,$86,$86,$06,$06
		.BYTE	$4A,$06,$07,$89,$05,$FA,$49,$0B,$07,$03,$8B,$07,$8B,$8B,$86,$05
		.BYTE	$FC,$39,$07,$86,$05,$FD,$49,$8B,$07,$23,$89,$9B,$06,$3C,$04,$06
		.BYTE	$06,$06,$06,$06,$9A,$06,$5D,$03,$06,$06,$06,$06,$9B,$06,$3C,$23
		.BYTE	$CA,$9B,$06,$9C,$03,$CA,$CA,$CA,$CA,$9C,$05,$FC,$04,$06,$06,$06
		.BYTE	$06,$06,$F0,$23,$05,$DE,$E4,$7D,$86,$27,$00,$07,$FF,$FF,$FF,$FF
		.BYTE	$00,$00,$00,$00,$8F,$07,$C0,$93,$07,$CC,$8B,$07,$C0,$FF

		SECTION_ENDS	F0F, "TLM GAME SETUP"

		BANK_END F1,$C000
