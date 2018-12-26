
		ORG $A000

		BANK_START $03

; =============== S U B	R O U T	I N E =======================================
_cur_money_draw:
		JSR		_cur_money_str_calc
		JSR		_cur_money_str_pos_calc
		JSR		_cur_money_str_queue
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_money_str_calc:
		LDA		_city._money
		LDX		_city._money+1
		LDY		_city._money+2
		STA		_ptr0
		STX		_ptr0+1
		STY		_tmp2
		LDA		#0
		STA		_tmp3
		LDY		#1
		LDX		#6
		LDA		#$DF
		STA		_pre_ppu_buf
loc_58026:
		JSR		_hex_to_dec_9999999
		CMP		#0
		BNE		loc_5803C
		LDA		_tmp3
		BNE		loc_5804F
		CPX		#0
		BEQ		loc_5803C
		LDA		#$DF
		STA		_pre_ppu_buf,Y
		BNE		loc_5804A
loc_5803C:
		STA		_pre_ppu_buf,Y
		LDA		_tmp3
		BNE		loc_5804A
		LDA		#$30
		STA		_pre_ppu_buf-1,Y
		DEC		_tmp3
loc_5804A:
		INY
		DEX
		BPL		loc_58026
		RTS
loc_5804F:
		LDA		#0
		STA		_pre_ppu_buf,Y
		BEQ		loc_5804A

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec_9999999:
		LDA		#0
		STA		_tmp4
loc_5805A:
		SEC
		LDA		_ptr0
		SBC		_hex_to_dec_9999999_tbl0,X
		STA		_ptr0
		LDA		_ptr0+1
		SBC		_hex_to_dec_9999999_tbl1,X
		STA		_ptr0+1
		LDA		_tmp2
		SBC		_hex_to_dec_9999999_tbl2,X
		STA		_tmp2
		BCC		loc_58076
		INC		_tmp4
		BNE		loc_5805A
loc_58076:
		CLC
		LDA		_ptr0
		ADC		_hex_to_dec_9999999_tbl0,X
		STA		_ptr0
		LDA		_ptr0+1
		ADC		_hex_to_dec_9999999_tbl1,X
		STA		_ptr0+1
		LDA		_tmp2
		ADC		_hex_to_dec_9999999_tbl2,X
		STA		_tmp2
		LDA		_tmp4
		RTS
_hex_to_dec_9999999_tbl0:
		.BYTE	 $01,$0A,$64,$E8,$10,$A0,$40
_hex_to_dec_9999999_tbl1:
		.BYTE	 $00,$00, $00,$03,$27,$86,$42
_hex_to_dec_9999999_tbl2:
		.BYTE	 $00,$00, $00,$00, $00,$01, $0F

; =============== S U B	R O U T	I N E =======================================
_cur_money_str_pos_calc:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		#$1C
		CMP		#$1E
		BCC		loc_580B0
		SBC		#$1E
loc_580B0:
		STA		_ptr0+1
		LDA		_cur_screen_tile._X
		LSR
		CLC
		ADC		#$16
		CMP		#$20
		BCC		loc_580BE
		SBC		#$20
loc_580BE:
		STA		_ptr0
		CLC
		ADC		#7
		CMP		#$20
		BCC		loc_580D4
		SBC		#$1F
		STA		_tmp3
		LDA		#8
		SEC
		SBC		_tmp3
		STA		_tmp2
		BNE		loc_580DC
loc_580D4:
		LDA		#8
		STA		_tmp2
		LDA		#0
		STA		_tmp3
loc_580DC:
		LDA		_ptr0+1
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		_ptr0
		BCC		loc_580F2
		INX
loc_580F2:
		STA		_ptr0
		TXA
		ORA		#$20
		STA		_ptr0+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_money_str_queue:
		LDX		_ppu_queue_pos_head
		LDY		#0
		LDA		_ptr0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		INX
loc_58110:
		LDA		_pre_ppu_buf,Y
		INY
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp2
		BNE		loc_58110
		LDA		_tmp3
		BEQ		loc_5813D
		STA		_tmp2
		LDA		#0
		STA		_tmp3
		LDA		_ptr0
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		INX
		JMP		loc_58110
loc_5813D:
		STX		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_handler:
		LDX		#3
		LDA		#0
loc_58144:
		STA		byte_70,X
		STA		byte_76,X
		STA		byte_78,X
		STA		byte_7A,X
		STA		byte_7C,X
		STA		byte_7E,X
		STA		byte_80,X
		DEX
		BPL		loc_58144
		STA		byte_523
		STA		byte_528
		STA		byte_529
		STA		byte_52A
		STA		byte_52B
		STA		byte_52C
		STA		byte_52D
		STA		byte_52E
		STA		byte_52F
		STA		byte_530
		LDA		#5
		STA		_scr_mode_idx
		JSR		_spr_clear
		JSR		_game_msg_pal_select
		LDA		#1
		STA		_ppu_pal_dequeue_req
		JSR		_game_msg_apu_select
		JSR		_game_msg_wnd_draw
		JSR		_spr_clear
		JSR		_wait_for_nmi
		JSR		_farcall_argsAXY_safe
		.BYTE	$F2,$F2
		.WORD	_game_msg_chr_setup
		LDA		#0
		STA		_ptr0
		JSR		_game_msg_display
		LDA		#0
		STA		byte_525
		STA		byte_524
loc_581A4:
		LDA		_ptr0
		PHA
		JSR		_wait_for_nmi
		LDA		#0
		STA		_spr_buf_pos
		JSR		_game_msg_wnd_animA
		JSR		_game_msg_wnd_animB
		JSR		_game_msg_wnd_animC
		JSR		_spr_finish
		JSR		_game_msg_bonus_item_clear
		JSR		_msg_wnd_cheat_test
		PLA
		STA		_ptr0
		LDA		byte_524
		CMP		#$1E
		BCS		loc_581EC
		LDA		_pad0_autorep_press
		AND		#$C0
		BEQ		loc_581A4
		LDA		_ptr0
		CLC
		ADC		#1
		LDX		_game_msg_idx
		CMP		_game_msg_screens_count_list,X
		BCS		loc_581EC
		STA		_ptr0
		JSR		_game_msg_display
		LDA		#0
		STA		byte_525
		STA		byte_524
		JMP		loc_581A4
loc_581EC:
		LDA		#$20
		STA		_input_forbid_delay
		JSR		_spr_clear
		JSR		_wait_for_nmi
		LDA		#0
		STA		_scr_mode_idx
		LDA		#$80
		JSR		_pal_load_lib
		LDA		#$FF
		STA		_ppu_pal_dequeue_req
		JSR		_game_msg_chr_common_load
		JSR		_game_msg_chr1_load
		LDA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
		LDA		#$FF
		STA		byte_53
		LDA		#0
		STA		byte_54
		STA		byte_56
		LDA		#$4D
		STA		byte_55
		STA		byte_57
		JSR		_game_msg_music_restore
		JSR		_special_building_put_in_list
		JSR		_init_special_year_date
		JSR		_debug_flag_toggle_test
		LDA		#0
		STA		_game_msg_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_apu_select:
		LDX		_game_msg_idx
		LDA		_game_msg_apu_types_list,X
		BEQ		locret_5824B
		CMP		#1
		BNE		loc_58243
		LDA		#$60
		STA		_apu_mus_idx_req
		BNE		locret_5824B
loc_58243:
		LDA		#$10
		STA		_apu_se_flags
		LDA		#$21
		STA		_apu_mus_idx_req
locret_5824B:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_music_restore:
		LDA		#$80
		STA		_apu_mus_idx_req
		JSR		_wait_for_nmi
		LDX		_city._city_type
		LDY		byte_58289,X
		LDA		_city._game_flags
		AND		#8
		BNE		loc_58262
		LDY		#$80
loc_58262:
		STY		_apu_mus_idx_req
		RTS
_game_msg_apu_types_list:
		.BYTE	 $01,$01, $01,$01, $01,$01, $02,$02, $02,$02, $02,$01, $01,$01, $01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$02,$01
		.BYTE	$02,$02,$02,$02
byte_58289:
		.BYTE	$08,$20,$41,$42,$44,$48

; =============== S U B	R O U T	I N E =======================================
_game_msg_display:
		LDA		_ptr0
		PHA
		JSR		_far_wnd_header_draw
		PLA
		STA		_ptr0
		LDA		#0
		STA		_ptr0+1
loc_5829C:
		LDA		_ptr0
		PHA
		LDA		_ptr0+1
		PHA
		JSR		_wait_for_nmi
		PLA
		STA		_ptr0+1
		PLA
		STA		_ptr0
		JSR		_game_msg_line_pos_calc
		JSR		_game_msg_line_display
		INC		_ptr0+1
		LDX		_game_msg_idx
		LDA		off_70E22,X
		STA		_tmp2
		LDA		off_70E46,X
		STA		_tmp3
		LDY		_ptr0
		LDA		_ptr0+1
		CMP		(_tmp2),Y
		BCC		loc_5829C
		JSR		_game_msg_hints_extra_icons_draw
		JSR		_game_msg_bonus_item_icon_draw
		RTS

; =============== S U B	R O U T	I N E =======================================
_far_wnd_header_draw:
		JSR		_farcall_argsAXY_safe
		.BYTE	$F0,$F0
		.WORD	_wnd_header_draw
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_line_pos_calc:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		#4
		ADC		_ptr0+1
		CMP		#$1E
		BCC		loc_582E4
		SBC		#$1E
loc_582E4:
		STA		_tmp2
		LDA		_cur_screen_tile._X
		LSR
		CLC
		ADC		#8
		CMP		#$20
		BCC		loc_582F2
		SBC		#$20
loc_582F2:
		STA		_tmp3
		LDA		#$20
		SEC
		SBC		_tmp3
		CMP		#$17
		BCS		loc_58308
		STA		_tmp4
		LDA		#$17
		SEC
		SBC		_tmp4
		STA		_tmp5
		BNE		loc_58310
loc_58308:
		LDA		#$17
		STA		_tmp4
		LDA		#0
		STA		_tmp5
loc_58310:
		LDA		_tmp2
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		_tmp3
		STA		_tmp2
		TXA
		ADC		#0
		STA		_tmp3
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
		PLA
		STA		_tmp6
		LDA		_ptr0+1
		ASL
		TAY
		LDA		(_tmp6),Y
		INY
		PHA
		LDA		(_tmp6),Y
		STA		_tmp6+1
		PLA
		STA		_tmp6
		LDA		_tmp4
		PHA
		LDA		_tmp5
		PHA
		LDX		_extnt_queue_pos_head
		LDA		_tmp2
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp4
		STA		_extnt_queue_buf,X
		INX
loc_58371:
		LDA		#$B
		STA		_extnt_queue_buf,X
		INX
		DEC		_tmp4
		BNE		loc_58371
		LDA		_tmp5
		BEQ		loc_5839E
		STA		_tmp4
		LDA		#0
		STA		_tmp5
		LDA		_tmp2
		AND		#$E0
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp4
		STA		_extnt_queue_buf,X
		INX
		JMP		loc_58371
loc_5839E:
		STX		_extnt_queue_pos_head
		PLA
		STA		_tmp5
		PLA
		STA		_tmp4
		LDX		_ppu_queue_pos_head
		LDY		#0
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
loc_583BE:
		LDA		(_tmp6),Y
		INY
		CMP		#$40
		BEQ		loc_583FA
		CMP		#$F
		BNE		loc_583CC
		DEY
		LDA		#$20
loc_583CC:
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp4
		BNE		loc_583BE
		LDA		_tmp5
		BEQ		loc_583F7
		STA		_tmp4
		LDA		#0
		STA		_tmp5
		LDA		_tmp2
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
		JMP		loc_583BE
loc_583F7:
		STX		_ppu_queue_pos_head
		RTS
loc_583FA:
		LDA		(_tmp6),Y
		INY
		JSR		_cmd_special_data_insert
		BCS		loc_583F7
		JMP		loc_583BE

; =============== S U B	R O U T	I N E =======================================
_game_msg_chr1_load:
		LDA		_mmc5_chr_banks_shadow+1
		STA		_MMC5_CHR_BANKSA+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_animA:
		LDA		byte_523
		BNE		loc_58419
		LDX		_game_msg_idx
		LDA		byte_703D9,X
		STA		byte_523
loc_58419:
		CMP		#$FA
		BEQ		locret_5846D
		LDY		#$CF
		LDX		_spr_buf_pos
		LDA		#7
		STA		_ptr0
loc_58425:
		LDA		#$F4
		STA		_spr_buf,X
		STA		_spr_buf+4,X
		LDA		byte_523
		CMP		#$FC
		BEQ		loc_58438
		TYA
		STA		_spr_buf,X
loc_58438:
		LDA		byte_523
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
		ADC		#8
		TAX
		TYA
		SEC
		SBC		#8
		TAY
		DEC		_ptr0
		BNE		loc_58425
		STX		_spr_buf_pos
locret_5846D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_bonus_item_clear:
		LDA		byte_528
		CMP		#2
		BEQ		loc_58476
		RTS
loc_58476:
		LDA		#2
		STA		_ptr0+1
loc_5847A:
		JSR		_game_msg_bonus_item_pos_calc
		JSR		_game_msg_bonus_item_line_clear
		DEC		_ptr0+1
		BPL		loc_5847A
		INC		byte_528
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_bonus_item_pos_calc:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		#$C
		ADC		_ptr0+1
		CMP		#$1E
		BCC		loc_58496
		SBC		#$1E
loc_58496:
		STA		_tmp2
		LDA		_cur_screen_tile._X
		LSR
		CLC
		ADC		#$11
		CMP		#$20
		BCC		loc_584A4
		SBC		#$20
loc_584A4:
		STA		_tmp3
		LDA		#$20
		SEC
		SBC		_tmp3
		CMP		#3
		BCS		loc_584BA
		STA		_tmp4
		LDA		#3
		SEC
		SBC		_tmp4
		STA		_tmp5
		BNE		loc_584C2
loc_584BA:
		LDA		#3
		STA		_tmp4
		LDA		#0
		STA		_tmp5
loc_584C2:
		LDA		_tmp2
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		_tmp3
		STA		_tmp2
		TXA
		ADC		#0
		STA		_tmp3
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_bonus_item_line_clear:
		LDX		_extnt_queue_pos_head
		LDY		#$B
		LDA		_tmp2
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp4
		STA		_extnt_queue_buf,X
		STA		_tmp6
		LDA		_tmp5
		STA		_tmp6+1
		INX
loc_584FB:
		TYA
		STA		_extnt_queue_buf,X
		INX
		DEC		_tmp6
		BNE		loc_584FB
		LDA		_tmp6+1
		BEQ		loc_58527
		STA		_tmp6
		LDA		#0
		STA		_tmp6+1
		LDA		_tmp2
		AND		#$E0
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp6
		STA		_extnt_queue_buf,X
		INX
		JMP		loc_584FB
loc_58527:
		STX		_extnt_queue_pos_head
		LDX		_ppu_queue_pos_head
		LDY		#$20
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
loc_58541:
		TYA
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp4
		BNE		loc_58541
		LDA		_tmp5
		BEQ		loc_5856D
		STA		_tmp4
		LDA		#0
		STA		_tmp5
		LDA		_tmp2
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
		JMP		loc_58541
loc_5856D:
		STX		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_animB:
		LDX		_game_msg_idx
		LDA		byte_586B2,X
		BNE		loc_58578
		RTS
loc_58578:
		LDA		#$FF
		STA		_special_constr_palette_redraw_req
		LDA		byte_528
		BEQ		loc_58586
		BPL		loc_585A5
		JMP		loc_58614
loc_58586:
		LDA		#$98
		STA		byte_529
		LDA		#$A8
		STA		byte_52A
		INC		byte_528
		LDA		byte_58659
		STA		byte_52B
		LDA		byte_58684
		STA		byte_52D
		LDA		byte_586A2
		STA		byte_52F
loc_585A5:
		LDY		byte_52C
		LDX		byte_52B
		CPX		#0
		BNE		loc_585CA
		INY
		LDA		byte_58659,Y
		TAX
		CMP		#$FF
		BNE		loc_585BE
		STA		byte_528
		JMP		loc_58614
loc_585BE:
		CMP		#$FE
		BNE		loc_585CA
		INY
		LDA		byte_58659,Y
		TAX
		INC		byte_528
loc_585CA:
		LDA		byte_5864A,Y
		CMP		#$FD
		BNE		loc_585DB
		DEX
		STX		byte_52B
		STY		byte_52C
		JMP		loc_58614
loc_585DB:
		STA		_spr_insert_args._idx
		DEX
		STX		byte_52B
		STY		byte_52C
		LDY		byte_52E
		LDX		byte_52D
		CPX		#0
		BNE		loc_585F2
		INY
		LDX		byte_58684,Y
loc_585F2:
		LDA		byte_58668,Y
		CLC
		ADC		byte_529
		STA		byte_529
		STA		_spr_insert_args._pos._X
		LDA		byte_58676,Y
		CLC
		ADC		byte_52A
		STA		byte_52A
		STA		_spr_insert_args._pos._Y
		DEX
		STX		byte_52D
		STY		byte_52E
		JSR		_spr_lib_prg2_insert
loc_58614:
		LDY		byte_530
		LDX		byte_52F
		CPX		#0
		BNE		loc_58622
		INY
		LDX		byte_586A2,Y
loc_58622:
		LDA		byte_58692,Y
		CMP		#$FF
		BEQ		locret_58641
		CMP		#$FE
		BEQ		loc_58642
		STA		_spr_insert_args._idx
		DEX
		STX		byte_52F
		STY		byte_530
		LDA		#$28
		STA		_spr_insert_args._pos._X
		LDA		#$A8
		STA		_spr_insert_args._pos._Y
		JSR		_spr_lib_prg2_insert
locret_58641:
		RTS
loc_58642:
		DEX
		STX		byte_52F
		STY		byte_530
		RTS
byte_5864A:
		.BYTE	$FD,$FD,$90,$91,$92,$93,$94,$95,$96,$97,$98,$00,$99,$9D,$FF
byte_58659:
		.BYTE	$C0,$E0,$02,$02,$02,$02,$02,$02,$02,$02,$02,$FE,$04,$6E,$FF
byte_58668:
		.BYTE	$00,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FD,$FF
byte_58676:
		.BYTE	$00,$00,$01,$00,$01,$00,$01,$00,$01,$00,$01,$00,$01,$FF
byte_58684:
		.BYTE	$60,$05,$01,$05,$01,$05,$01,$05,$01,$05,$01,$05,$01,$FF
byte_58692:
		.BYTE	$FE,$FE,$FE,$9A,$9B,$9C,$9E,$9F,$9A,$A1,$A1,$A1,$A1,$A1,$A1,$FF
byte_586A2:
		.BYTE	$C0,$94,$AE,$08,$08,$14,$40,$08,$08,$E0,$E0,$E0,$E0,$E0,$E0,$FF
byte_586B2:
		.BYTE	$00,$00,$01,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$00,$01
		.BYTE	$00,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$00,$01,$01
		.BYTE	$01,$01,$01,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$00,$00

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_animC:
		LDX		_game_msg_idx
		LDA		byte_70401,X
		STA		byte_522
loc_586DE:
		LDX		_game_msg_idx
		LDA		off_70429,X
		STA		_ptr0
		LDA		off_7044D,X
		STA		_ptr0+1
		LDY		byte_522
		DEY
		LDA		(_ptr0),Y
		TAX
		LDA		byte_70,Y
		BEQ		loc_586FB
		BPL		loc_58707
		JMP		loc_5886E
loc_586FB:
		LDA		byte_7047E,X
		STA		byte_72,Y
		LDA		byte_70491,X
		STA		byte_74,Y
loc_58707:
		LDA		byte_72,Y
		STA		_spr_insert_args._pos._X
		LDA		byte_74,Y
		STA		_spr_insert_args._pos._Y
		LDA		#1
		STA		byte_70,Y
		LDA		off_704A4,X
		STA		_ptr0
		LDA		off_704B7,X
		STA		_ptr0+1
		LDA		off_704CA,X
		STA		_tmp2
		LDA		off_704DD,X
		STA		_tmp3
		LDA		off_704F0,X
		STA		_tmp4
		LDA		off_70503,X
		STA		_tmp5
		LDA		off_70516,X
		STA		_tmp6
		LDA		off_70529,X
		STA		_tmp6+1
		LDX		byte_76,Y
		LDA		byte_78,Y
		TAY
		CPX		#0
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
		LDY		byte_522
		DEY
		LDA		#$FF
		STA		byte_70,Y
		JMP		loc_5886E
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
		LDY		byte_522
		DEY
		LDA		byte_7E,Y
		BEQ		loc_587AA
		SEC
		SBC		#1
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
		LDY		byte_522
		DEY
		STA		byte_7E,Y
		LDA		byte_83
		SEC
		SBC		byte_82
		TAY
		JMP		loc_58748
loc_587BE:
		INY
		STA		byte_523
		JMP		loc_58748
loc_587C5:
		TYA
		LDY		byte_522
		DEY
		STA		byte_78,Y
		TXA
		STA		byte_76,Y
		LDX		byte_7A,Y
		LDA		byte_7C,Y
		TAY
		CPX		#0
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
		LDY		byte_522
		DEY
		LDA		byte_80,Y
		BEQ		loc_58822
		SEC
		SBC		#1
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
		LDY		byte_522
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
		LDY		byte_522
		DEY
		STA		byte_7C,Y
		TXA
		STA		byte_7A,Y
		PLA
		TAX
		AND		#$F
		CMP		#8
		BCC		loc_58850
		ORA		#$F0
loc_58850:
		CLC
		ADC		byte_72,Y
		STA		byte_72,Y
		TXA
		AND		#$F0
		LSR
		LSR
		LSR
		LSR
		CMP		#8
		BCC		loc_58864
		ORA		#$F0
loc_58864:
		CLC
		ADC		byte_74,Y
		STA		byte_74,Y
		JSR		_spr_lib_prg8_insert
loc_5886E:
		DEC		byte_522
		BEQ		locret_58876
		JMP		loc_586DE
locret_58876:
		RTS

; =============== S U B	R O U T	I N E =======================================
_cmd_special_data_insert:
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
		LDA		byte_51A
		BNE		loc_58896
		CLC
		RTS
loc_58896:
		TYA
		PHA
		LDY		#0
loc_5889A:
		LDA		(word_DA),Y
		INY
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp4
		BNE		loc_588C5
		LDA		_tmp5
		BEQ		loc_588CE
		STA		_tmp4
		LDA		#0
		STA		_tmp5
		LDA		_tmp2
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
loc_588C5:
		DEC		byte_51A
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
		CMP		#0
		BEQ		loc_588E4
		CMP		#1
		BEQ		loc_588F6
		CMP		#2
		BEQ		loc_58908
		CMP		#3
		BEQ		loc_5891A
		BNE		loc_5892C
loc_588E4:
		LDA		_city._name
		SEC
		SBC		#1
		STA		byte_51A
		LDA		#<[_city._name+1]
		STA		word_DA
		LDA		#>[_city._name+1]
		STA		word_DA+1
		RTS
loc_588F6:
		LDA		_city._sister_name
		SEC
		SBC		#1
		STA		byte_51A
		LDA		#<[_city._sister_name+1]
		STA		word_DA
		LDA		#>[_city._sister_name+1]
		STA		word_DA+1
		RTS
loc_58908:
		LDA		_city._bank_name
		SEC
		SBC		#1
		STA		byte_51A
		LDA		#<[_city._bank_name+1]
		STA		word_DA
		LDA		#>[_city._bank_name+1]
		STA		word_DA+1
		RTS
loc_5891A:
		LDA		_city._unused_name3
		SEC
		SBC		#1
		STA		byte_51A
		LDA		#<[_city._unused_name3+1]
		STA		word_DA
		LDA		#>[_city._unused_name3+1]
		STA		word_DA+1
		RTS
loc_5892C:
		LDA		_city._unused_name4
		SEC
		SBC		#1
		STA		byte_51A
		LDA		#<[_city._unused_name4+1]
		STA		word_DA
		LDA		#>[_city._unused_name4+1]
		STA		word_DA+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_cmd40_23_popul:
		LDA		(_tmp6),Y
		INY
		SEC
		SBC		#$30
		JSR		_get_select_popul_num
		LDA		#1
		STA		byte_51B
		TYA
		PHA
		LDY		#5
loc_58950:
		LDA		_tmp_popul_decimal_buf,Y
		DEY
		CMP		#$30
		BNE		loc_58963
		LDA		byte_51B
		BEQ		loc_58961
		CPY		#$FF
		BNE		loc_58950
loc_58961:
		LDA		#$30
loc_58963:
		STA		_ppu_queue_buf,X
		INX
		LDA		#0
		STA		byte_51B
		DEC		_tmp4
		BNE		loc_58990
		LDA		_tmp5
		BEQ		loc_58998
		STA		_tmp4
		LDA		#0
		STA		_tmp5
		LDA		_tmp2
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp3
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
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
		CMP		#0
		BEQ		loc_589A4
		CMP		#1
		BEQ		loc_589B9
loc_589A4:
		LDA		_popul_cur
		STA		_tmp_popul_hex
		LDA		_popul_cur+1
		STA		_tmp_popul_hex+1
		LDA		_popul_cur+2
		STA		_tmp_popul_hex+2
		JMP		loc_589C8
loc_589B9:
		LDA		#$10
		STA		_tmp_popul_hex
		LDA		#$27
		STA		_tmp_popul_hex+1
		LDA		#0
		STA		_tmp_popul_hex+2
loc_589C8:
		TYA
		PHA
		LDY		#5
loc_589CC:
		LDA		#0
		STA		_tmp_popul_decimal_buf,Y
loc_589D1:
		SEC
		LDA		_tmp_popul_hex
		SBC		byte_58A25,Y
		STA		_tmp_popul_hex
		LDA		_tmp_popul_hex+1
		SBC		byte_58A2B,Y
		STA		_tmp_popul_hex+1
		LDA		_tmp_popul_hex+2
		SBC		byte_58A31,Y
		STA		_tmp_popul_hex+2
		BCC		loc_589FA
		LDA		_tmp_popul_decimal_buf,Y
		CLC
		ADC		#1
		STA		_tmp_popul_decimal_buf,Y
		BNE		loc_589D1
loc_589FA:
		CLC
		LDA		_tmp_popul_hex
		ADC		byte_58A25,Y
		STA		_tmp_popul_hex
		LDA		_tmp_popul_hex+1
		ADC		byte_58A2B,Y
		STA		_tmp_popul_hex+1
		LDA		_tmp_popul_hex+2
		ADC		byte_58A31,Y
		STA		_tmp_popul_hex+2
		LDA		_tmp_popul_decimal_buf,Y
		CLC
		ADC		#$30
		STA		_tmp_popul_decimal_buf,Y
		DEY
		BPL		loc_589CC
		PLA
		TAY
		RTS
byte_58A25:
		.BYTE	$01,$0A,$64,$E8,$10,$A0
byte_58A2B:
		.BYTE	$00,$00,$00,$03,$27,$86
byte_58A31:
		.BYTE	$00,$00,$00,$00,$00,$01

; =============== S U B	R O U T	I N E =======================================
; event	1 x
; event	2 item F
; event	3 x
; event	4 x
; event	5 x
; event	6 x
; event	7 x
; event	8 x
; event	9 x
; event	A x
; event	B item 1
; event	C x
; event	D item 2
; event	E item 3
; event	F item 4
; event	10 item	5
; event	11 item	6
; event	12 item	7
; event	13 item	8
; event	14 item	9
; event	15 x
; event	16 item	A
; event	17 item	B
; event	18 item	C
; event	19 item	D
; event	1A item	E
;
;
_special_building_put_in_list:
		LDX		#$F
		LDA		_game_msg_idx
		CMP		#2
		BEQ		loc_58A5A
		LDX		#1
		CMP		#$B
		BEQ		loc_58A5A
		CMP		#$15
		BEQ		locret_58A6D
		CMP		#$D
		BCC		locret_58A6D
		CMP		#$1B
		BCS		locret_58A6D
		SEC
		SBC		#$B
		TAX
		CPX		#$B
		BCC		loc_58A5A
		DEX
loc_58A5A:
		TXA
		PHA
		LDX		#$FF
		LDY		#4
loc_58A60:
		INX
		LDA		_city._special_buildings_list,X
		BEQ		loc_58A69
		DEY
		BNE		loc_58A60
loc_58A69:
		PLA
		STA		_city._special_buildings_list,X
locret_58A6D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_init_special_year_date:
		LDA		_game_msg_idx
		CMP		#$C
		BEQ		loc_58A79
		CMP		#$15
		BEQ		loc_58A85
		RTS
loc_58A79:
		LDA		#$A
		JSR		_adc_cur_year
		STA		_city._year_to_liberty
		STX		_city._year_to_liberty+1
		RTS
loc_58A85:
		LDA		#1
		JSR		_adc_cur_year
		STA		_city._year_to_expo
		STX		_city._year_to_expo+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_adc_cur_year:
		LDX		_city._cur_year+1
		CLC
		ADC		_city._cur_year
		LDA		_city._cur_year
		BCC		locret_58A9E
		INX
locret_58A9E:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_draw:
		JSR		_ppu_queue_empty_test
		BCS		_game_msg_wnd_draw
		LDA		#0
		STA		_ptr0
loc_58AA8:
		LDA		_ptr0
		PHA
		JSR		_game_msg_wnd_line_pos_calc
		JSR		_game_msg_wnd_line_draw
		JSR		_wait_for_nmi
		PLA
		STA		_ptr0
		INC		_ptr0
		LDA		_ptr0
		CMP		#$15
		BCC		loc_58AA8
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_line_pos_calc:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		_ptr0
		CMP		#$1E
		BCC		loc_58ACC
		SBC		#$1E
loc_58ACC:
		STA		_ptr0+1
		LDA		_cur_screen_tile._X
		LSR
		CLC
		ADC		#7
		CMP		#$20
		BCC		loc_58ADA
		SBC		#$20
loc_58ADA:
		STA		_tmp2
		LDA		#$20
		SEC
		SBC		_tmp2
		CMP		#$19
		BCS		loc_58AF0
		STA		_tmp3
		LDA		#$19
		SEC
		SBC		_tmp3
		STA		_tmp4
		BNE		loc_58AF8
loc_58AF0:
		LDA		#$19
		STA		_tmp3
		LDA		#0
		STA		_tmp4
loc_58AF8:
		LDA		_ptr0+1
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		_tmp2
		STA		_ptr0+1
		TXA
		ADC		#0
		STA		_tmp2
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_line_draw:
		LDX		_ptr0
		LDY		byte_58BB7,X
		STY		_tmp6+1
		LDX		_ppu_queue_pos_head
		LDA		_tmp3
		STA		_tmp5
		LDA		_tmp4
		STA		_tmp6
		LDA		_ptr0+1
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp2
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp5
		STA		_ppu_queue_buf,X
		INX
loc_58B38:
		LDA		byte_58C49,Y
		INY
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp5
		BNE		loc_58B38
		LDA		_tmp6
		BEQ		loc_58B67
		STA		_tmp5
		LDA		#0
		STA		_tmp6
		LDA		_ptr0+1
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp2
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp5
		STA		_ppu_queue_buf,X
		INX
		JMP		loc_58B38
loc_58B67:
		LDY		_tmp6+1
		STX		_tmp6+1
		LDX		_extnt_queue_pos_head
		LDA		_ptr0+1
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp2
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp3
		STA		_extnt_queue_buf,X
		INX
loc_58B81:
		LDA		byte_58BCC,Y
		INY
		STA		_extnt_queue_buf,X
		INX
		DEC		_tmp3
		BNE		loc_58B81
		LDA		_tmp4
		BEQ		loc_58BB0
		STA		_tmp3
		LDA		#0
		STA		_tmp4
		LDA		_ptr0+1
		AND		#$E0
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp2
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp3
		STA		_extnt_queue_buf,X
		INX
		JMP		loc_58B81
loc_58BB0:
		LDY		_tmp6+1
		STY		_ppu_queue_pos_head
		STX		_extnt_queue_pos_head
		RTS
byte_58BB7:
		.BYTE	$00,$19,$19,$32,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B
		.BYTE	$4B,$4B,$4B,$4B,$64
byte_58BCC:
		.BYTE	$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58
		.BYTE	$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$98,$98,$98,$98
		.BYTE	$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98
		.BYTE	$98,$98,$98,$98,$58,$58,$98,$98,$98,$98,$98,$98,$98,$98,$98
		.BYTE	$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$58
		.BYTE	$58,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B
		.BYTE	$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$58,$58,$58,$58,$58,$58
		.BYTE	$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58,$58
		.BYTE	$58,$58,$58,$58,$58
byte_58C49:
		.BYTE	$C0,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1
		.BYTE	$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C1,$C2,$D0,$00,$00,$00,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.BYTE	$00,$D2,$D0,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$D2,$D0,$20,$20,$20,$20
		.BYTE	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
		.BYTE	$20,$20,$20,$D2,$E0,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1
		.BYTE	$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E1,$E2

; =============== S U B	R O U T	I N E =======================================
_ppu_queue_empty_test:
		LDA		_ppu_queue_pos_tail
		CMP		_ppu_queue_pos_head
		BNE		loc_58CD4
		LDA		_extnt_queue_pos_tail
		CMP		_extnt_queue_pos_head
		BNE		loc_58CD4
		CLC
		RTS
loc_58CD4:
		SEC
		RTS

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
		AND		#7
		BEQ		loc_58D0D
		LDA		_scroll_msg_shift_pos
		CLC
		ADC		#2
		STA		_scroll_msg_shift_pos
		LDA		_scroll_msg_start_pos
		LDX		_scroll_msg_idx
		CMP		_scroll_msg_sizes_list,X
		BCC		locret_58D0C
		LDA		#0
		STA		_scroll_msg_active_flag
		STA		_scroll_msg_start_pos
		STA		_debug_scroll_msg_flags
locret_58D0C:
		RTS
loc_58D0D:
		LDA		_debug_scroll_msg_flags
		CMP		#3
		BNE		loc_58D30
		LDA		_city._game_flags
		AND		#$10
		BEQ		loc_58D21
		LDA		#<[_scroll_txt_i_am_god]
		LDX		#>[_scroll_txt_i_am_god]
		BNE		loc_58D25
loc_58D21:
		LDA		#<[_scroll_txt_debug_off]
		LDX		#>[_scroll_txt_debug_off]
loc_58D25:
		STA		_tmp15
		STX		_tmp15+1
		LDA		#1
		STA		_scroll_msg_idx
		BNE		loc_58D3D
loc_58D30:
		LDX		_scroll_msg_idx
		LDA		_scroll_msg_lib_lo,X
		STA		_tmp15
		LDA		_scroll_msg_lib_hi,X
		STA		_tmp15+1
loc_58D3D:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		#$1B
		CMP		#$1E
		BCC		loc_58D49
		SBC		#$1E
loc_58D49:
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		STA		_tmp17
		LDA		_scroll_msg_start_pos
		AND		#$1F
		CLC
		ADC		_tmp17
		INC		_scroll_msg_start_pos
		STA		_tmp17
		TXA
		ADC		#$20
		STA		_tmp18
		LDX		_ppu_queue_pos_tail
		DEX
		DEX
		DEX
		DEX
		STX		_ppu_queue_pos_tail
		LDY		_scroll_msg_end_char
		LDA		_tmp17
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp18
		STA		_ppu_queue_buf,X
		INX
		LDA		#1
		STA		_ppu_queue_buf,X
		INX
		LDA		(_tmp15),Y
		CMP		#$FF
		BEQ		loc_58D9E
		STA		_ppu_queue_buf,X
		INC		_scroll_msg_end_char
loc_58D94:
		LDA		_scroll_msg_shift_pos
		CLC
		ADC		#2
		STA		_scroll_msg_shift_pos
		RTS
loc_58D9E:
		LDA		#$DF
		STA		_ppu_queue_buf,X
		BNE		loc_58D94
_scroll_msg_sizes_list:
		.BYTE	$3D,$3C,$3C,$33,$36,$33,$3A,$3C,$3C,$41,$43,$43,$48,$49,$3D,$3F
		.BYTE	$33,$32,$2F,$34,$39,$38,$32,$44,$36

; =============== S U B	R O U T	I N E =======================================
_cur_date_draw:
		JSR		_cur_date_str_calc
		JSR		_cur_date_str_pos_calc
		JSR		_cur_date_str_queue
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_date_str_calc:
		LDX		#0
		JSR		_cur_year_str_queue
		LDA		#$DF
		STA		_pre_ppu_buf,X
		INX
		JSR		_cur_month_str_queue
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_year_str_queue:
		LDA		_city._cur_year
		LDY		_city._cur_year+1
		STA		_ptr0
		STY		_ptr0+1
		LDA		#4
		STA		_tmp2
		LDY		#0
loc_58DE7:
		JSR		_hex_to_dec_9999
		STA		_pre_ppu_buf,X
		INX
		INY
		INY
		DEC		_tmp2
		BNE		loc_58DE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec_9999:
		LDA		#0
		STA		_tmp3
loc_58DF9:
		LDA		_ptr0
		SEC
		SBC		_hex_to_dec_9999_tbl,Y
		STA		_ptr0
		LDA		_ptr0+1
		SBC		_hex_to_dec_9999_tbl+1,Y
		STA		_ptr0+1
		BCC		loc_58E0E
		INC		_tmp3
		BNE		loc_58DF9
loc_58E0E:
		LDA		_ptr0
		CLC
		ADC		_hex_to_dec_9999_tbl,Y
		STA		_ptr0
		LDA		_ptr0+1
		ADC		_hex_to_dec_9999_tbl+1,Y
		STA		_ptr0+1
		LDA		_tmp3
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_month_str_queue:
		LDA		_city._cur_month
		STA		_ptr0
		ASL
		CLC
		ADC		_ptr0
		TAY
		LDA		#3
		STA		_ptr0
loc_58E2E:
		LDA		_months_lib,Y
		INY
		STA		_pre_ppu_buf,X
		INX
		DEC		_ptr0
		BNE		loc_58E2E
		RTS
_hex_to_dec_9999_tbl:
		.BYTE	$E8, $03
		.BYTE	$64,$00
		.BYTE	$0A,$00
		.BYTE	$01,$00
_months_lib:
		.BYTE	$13,$0A,$17
		.BYTE	$0F,$0E,$0B
		.BYTE	$16,$0A,$1B
		.BYTE	$0A,$19,$1B
		.BYTE	$16,$0A,$22
		.BYTE	$13,$1E,$17
		.BYTE	$13,$1E,$15
		.BYTE	$0A,$1E,$10
		.BYTE	$1C,$0E,$19
		.BYTE	$18,$0C,$1D
		.BYTE	$17,$18,$1F
		.BYTE	$0D,$0E,$0C

; =============== S U B	R O U T	I N E =======================================
_cur_date_str_pos_calc:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		#$1A
		CMP		#$1E
		BCC		loc_58E73
		SBC		#$1E
loc_58E73:
		STA		_ptr0+1
		LDA		_cur_screen_tile._X
		LSR
		CLC
		ADC		#$16
		CMP		#$20
		BCC		loc_58E81
		SBC		#$20
loc_58E81:
		STA		_ptr0
		CLC
		ADC		#7
		CMP		#$20
		BCC		loc_58E97
		SBC		#$1F
		STA		_tmp3
		LDA		#8
		SEC
		SBC		_tmp3
		STA		_tmp2
		BNE		loc_58E9F
loc_58E97:
		LDA		#8
		STA		_tmp2
		LDA		#0
		STA		_tmp3
loc_58E9F:
		LDA		_ptr0+1
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		_ptr0
		BCC		loc_58EB5
		INX
loc_58EB5:
		STA		_ptr0
		STX		_ptr0+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_date_str_queue:
		LDX		_ppu_queue_pos_head
		LDY		#0
		LDA		_tmp2
		STA		_tmp4
		LDA		_tmp3
		STA		_tmp5
		LDA		_ptr0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
loc_58EDA:
		LDA		_pre_ppu_buf,Y
		INY
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp4
		BNE		loc_58EDA
		LDA		_tmp5
		BEQ		loc_58F09
		STA		_tmp4
		LDA		#0
		STA		_tmp5
		LDA		_ptr0
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		ORA		#$20
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp4
		STA		_ppu_queue_buf,X
		INX
		JMP		loc_58EDA
loc_58F09:
		STX		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_unref_2:
		LDX		_extnt_queue_pos_head
		LDA		_ptr0
		STA		_extnt_queue_buf,X
		INX
		LDA		_ptr0+1
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp2
		STA		_extnt_queue_buf,X
		INX
loc_58F22:
		LDA		#2
		STA		_extnt_queue_buf,X
		INX
		DEC		_tmp2
		BNE		loc_58F22
		LDA		_tmp3
		BEQ		loc_58F4F
		STA		_tmp2
		LDA		#0
		STA		_tmp3
		LDA		_ptr0
		AND		#$E0
		STA		_extnt_queue_buf,X
		INX
		LDA		_ptr0+1
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,X
		INX
		LDA		_tmp2
		STA		_extnt_queue_buf,X
		INX
		JMP		loc_58F22
loc_58F4F:
		STX		_extnt_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_menu_exec:
		TXA
		PHA
		LDA		#1
		STA		_scr_mode_idx
		JSR		_spr_clear
		JSR		_wait_for_nmi
		PLA
		TAX
		CPX		#0
		BNE		loc_58F72
		LDA		#1
		STA		_apu_se_flags
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_game_save_menu
		JMP		_game_menu_break
loc_58F72:
		DEX
		BNE		loc_58F7F
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_game_load_menu
		JMP		_game_menu_break
loc_58F7F:
		DEX
		BNE		loc_58F90
		LDA		#1
		STA		_apu_se_flags
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_game_title_return
		JMP		_game_menu_break
loc_58F90:
		LDA		#1
		STA		_apu_se_flags
		JSR		_farcall_argsAXY_safe
		.BYTE	$00,$F3
		.WORD	_game_quit
_game_menu_break:
		LDA		#0
		STA		_scr_mode_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_save_menu:
		JSR		_save_menu_exec
		BCS		loc_58FBA
		JSR		_sram_data_save
		JSR		_spr_clear
		JSR		_wait_for_nmi
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_wnd_erase
		JSR		_game_msg_chr_common_load
		SEC
		RTS
loc_58FBA:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_data_save:
		DEC		_mmc5_operation_in_progress_flag
		JSR		_sram_on
		LDA		#<[_wram_map_buf]
		STA		_ptr0
		LDA		#>[_wram_map_buf]
		STA		_ptr0+1
		LDA		#<[_sram_map_buf]
		STA		_tmp2
		LDA		#>[_sram_map_buf]
		STA		_tmp3
		LDY		#0
loc_58FD3:
		LDA		(_ptr0),Y
		STA		(_tmp2),Y
		INC		_ptr0
		BNE		loc_58FDD
		INC		_ptr0+1
loc_58FDD:
		INC		_tmp2
		BNE		loc_58FE3
		INC		_tmp3
loc_58FE3:
		LDA		_ptr0
		CMP		#<[_power_grid_map_buf]
		BNE		loc_58FD3
		LDA		_ptr0+1
		CMP		#>[_power_grid_map_buf]
		BNE		loc_58FD3
		LDX		#0
loc_58FF1:
		LDA		_wram_graphs,X
		STA		_sram_graphs,X
		INX
		CPX		#$78
		BNE		loc_58FF1
		LDX		#0
loc_58FFE:
		LDA		_city,X
		STA		_sram_vars,X
		INX
		CPX		#$80
		BNE		loc_58FFE
		LDA		#1
		STA		_has_saved_city_data_flag
		STA		_sram_has_saved_city_data_flag
		JSR		_sram_sum_reinit
		JSR		_sram_off
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_save_menu_exec:
		LDA		#1
		STA		_wnd_lvl_idx
		LDA		#$B
		STA		_wnd_idx+1
		LDA		#9
		STA		_wnd_left_tile+1
		LDA		#$C
		STA		_wnd_top_tile+1

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_question_menu:
		LDA		#$80
		STA		_pal_load_lib
		LDA		#1
		STA		_ppu_pal_dequeue_req
		JSR		_wait_for_nmi
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		LDA		#0
		STA		_common_yes_no_menu_cursor_pos
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_wnd_draw
loc_59047:
		LDA		#0
		STA		_spr_buf_pos
		JSR		_wait_for_nmi
		LDA		#$37
		STA		_MMC5_CHR_BANKSA+3
		JSR		_common_yes_no_menu_cursor_draw
		LDA		_pad0_autorep_press
		AND		#$80
		BNE		loc_5907B
		LDA		_pad0_autorep_press
		AND		#$40
		BNE		loc_5909C
		LDA		_pad0_autorep_press
		AND		#$F
		BEQ		loc_59047
		LDA		#$40
		STA		_apu_se_flags+1
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
		LDA		#$20
		STA		_apu_se_flags+1
		LDA		#$18
		STA		_ptr0
loc_5908E:
		JSR		_wait_for_nmi
		DEC		_ptr0
		BNE		loc_5908E
		CLC
		RTS
loc_59097:
		LDA		#0
		PHA
		BEQ		loc_5909F
loc_5909C:
		LDA		#$FF
		PHA
loc_5909F:
		JSR		_spr_clear
		JSR		_common_yes_no_menu_no_push_draw
		LDA		#$20
		STA		_apu_se_flags+1
		JSR		_spr_clear
		LDA		#0
		STA		_mmc5_operation_in_progress_flag
		JSR		_wait_for_nmi
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_wnd_erase
		JSR		_game_msg_chr_common_load
		PLA
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_cursor_draw:
		LDA		#0
		LDX		_common_yes_no_menu_cursor_pos
		BEQ		loc_590C9
		LDA		#$30
loc_590C9:
		CLC
		ADC		#$78
		STA		_spr_insert_args._pos._X
		LDA		#$90
		STA		_spr_insert_args._pos._Y
		LDA		#0
		STA		_spr_insert_args._idx
		LDA		#0
		STA		_spr_insert_args._attr
		JMP		_spr_lib_prg2_insert

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_yes_push_draw:
		LDA		#$C
		STA		_spr_insert_args._idx
		LDA		#$60
		BNE		_common_yes_no_menu_button_push_draw

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_no_push_draw:
		LDA		#$D
		STA		_spr_insert_args._idx
		LDA		#$90

; =============== S U B	R O U T	I N E =======================================
_common_yes_no_menu_button_push_draw:
		STA		_spr_insert_args._pos._X
		LDA		#$88
		STA		_spr_insert_args._pos._Y
		LDA		#0
		STA		_spr_insert_args._attr
		JMP		_spr_lib_prg2_insert

; =============== S U B	R O U T	I N E =======================================
_game_load_menu:
		LDA		_has_saved_city_data_flag
		BNE		loc_59103
		LDA		#2
		STA		_apu_se_flags
		BNE		loc_5912E
loc_59103:
		LDA		#0
		STA		_cur_game_mode
		LDA		#1
		STA		_apu_se_flags
		JSR		_load_menu_exec
		BCS		loc_5912E
		JSR		_sram_data_load
		JSR		_spr_clear
		JSR		_wait_for_nmi
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_wnd_erase
		JSR		_game_msg_chr_common_load
		JSR		_pal_fade_out
		TSX
		TXA
		CLC
		ADC		#$10
		TAX
		TXS
loc_5912E:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_load_menu_exec:
		LDA		#1
		STA		_wnd_lvl_idx
		LDA		#$10
		STA		_wnd_idx+1
		LDA		#9
		STA		_wnd_left_tile+1
		LDA		#$C
		STA		_wnd_top_tile+1
		JMP		_common_yes_no_question_menu

; =============== S U B	R O U T	I N E =======================================
_sram_data_load:
		DEC		_mmc5_operation_in_progress_flag
		JSR		_sram_on
		LDA		#<[_sram_map_buf]
		STA		_ptr0
		LDA		#>[_sram_map_buf]
		STA		_ptr0+1
		LDA		#<[_wram_map_buf]
		STA		_tmp2
		LDA		#>[_wram_map_buf]
		STA		_tmp3
		LDY		#0
loc_5915A:
		LDA		(_ptr0),Y
		STA		(_tmp2),Y
		INC		_ptr0
		BNE		loc_59164
		INC		_ptr0+1
loc_59164:
		INC		_tmp2
		BNE		loc_5916A
		INC		_tmp3
loc_5916A:
		LDA		_tmp2
		CMP		#<[_power_grid_map_buf]
		BNE		loc_5915A
		LDA		_tmp3
		CMP		#>[_power_grid_map_buf]
		BNE		loc_5915A
		LDX		#0
loc_59178:
		LDA		_sram_graphs,X
		STA		_wram_graphs,X
		INX
		CPX		#$78
		BNE		loc_59178
		LDX		#0
loc_59185:
		LDA		_sram_vars,X
		STA		_city,X
		INX
		CPX		#$80
		BNE		loc_59185
		JSR		_sram_off
		INC		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_quit:
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
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_wnd_erase
		JSR		_game_msg_chr_common_load
		JSR		_pal_fade_out
loc_591B7:
		JMP		_turn_off_power_screen

; =============== S U B	R O U T	I N E =======================================
_game_title_return:
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
		JSR		_farcall_argsAXY_safe
		.BYTE	$FC,$FD
		.WORD	_wnd_erase
		JSR		_game_msg_chr_common_load
loc_591D8:
		LDA		#0
		STA		_main_sub_idx
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
		LDA		#$80
		STA		_apu_mus_idx_req
		LDA		#0
		STA		_ppu_ctrl_shadow
		STA		_PPU_CTRL
		STA		_ppu_mask_shadow
		STA		_PPU_MASK
		STA		byte_582
		LDA		#$80
		STA		_game_wnd_spr_mode_flag
		LDA		#$53
		STA		_MMC5_CHR_BANKSA+3
		JSR		_ppu_scr_clear
		JSR		_turn_off_power_draw
		LDA		#$13
		STA		_screen_pal_cur_bg_idx
		LDA		#$B
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
		LDX		#0
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
		LDA		#2
		STA		_MMC5_CHR_MODE
		LDA		#>[_MMC5_EXRAM]
		STA		_ptr0+1
		LDA		#<[_MMC5_EXRAM]
		STA		_ptr0
		LDY		#0
		LDX		#4
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
		LDA		#1
		STA		_MMC5_CHR_MODE
		RTS

; =============== S U B	R O U T	I N E =======================================
_turn_off_power_draw:
		LDA		#<[aTurnOffPowerSwitch]
		STA		_ptr0
		LDA		#>[aTurnOffPowerSwitch]
		STA		_ptr0+1
		LDY		#0
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
		STA		_spr_insert_args._pos._X
		LDA		#$40
		STA		_spr_insert_args._pos._Y
		LDA		#$A2
		STA		_spr_insert_args._idx
		JSR		_spr_lib_prg2_insert
		LDA		byte_582
		LDX		#0
loc_592E4:
		CMP		byte_5930F,X
		BCC		loc_592ED
		INX
		INX
		BCS		loc_592E4
loc_592ED:
		LDA		byte_5930F+1,X
		CMP		#$FF
		BEQ		loc_59301
		STA		_spr_insert_args._idx
		LDA		#$78
		STA		_spr_insert_args._pos._X
		LDA		#$48
		STA		_spr_insert_args._pos._Y
		JSR		_spr_lib_prg2_insert
loc_59301:
		LDX		byte_582
		INX
		CPX		#$52
		BCC		loc_5930B
		LDX		#0
loc_5930B:
		STX		byte_582
		RTS
byte_5930F:
		.BYTE	$0A,$A3
		.BYTE	$16,$A4
		.BYTE	$24,$A5
		.BYTE	$34,$A6
		.BYTE	$3F,$A7
		.BYTE	$42,$A8
		.BYTE	$52,$FF

; =============== S U B	R O U T	I N E =======================================
_generate_new_map:
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		LDA		_new_map_number
		STA		_rand_seed
		LDA		_new_map_number+1
		STA		_rand_seed+1
		ROL		_rand_seed
		ROL		_rand_seed+1
		ROL
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
		CMP		#$2B
		BCS		loc_59354
		JSR		_generate_isle_map
		LDA		#0
		STA		_mmc5_operation_in_progress_flag
		RTS
loc_59354:
		JSR		_wram_map_clear
		JSR		_generate_new_mapA
		JSR		_generate_new_mapB
		JSR		_generate_new_mapC
		JSR		_generate_new_mapD
		JSR		_generate_new_mapE
		JSR		_generate_new_mapF
		JSR		_generate_new_mapG
		LDA		#0
		STA		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_isle_map:
		LDA		#$4B
		STA		byte_573
		STA		byte_574
loc_59379:
		LDA		#2
		LDX		byte_573
		CPX		#2
		BCC		loc_59393
		CPX		#$4A
		BCS		loc_59393
		LDX		byte_574
		CPX		#2
		BCC		loc_59393
		CPX		#$4A
		BCS		loc_59393
		LDA		#0
loc_59393:
		JSR		_isle_map_tile_write
		DEC		byte_574
		BPL		loc_59379
		DEC		byte_573
		BMI		loc_593A8
		LDA		#$4B
		STA		byte_574
		JMP		loc_59379
loc_593A8:
		LDA		#0
		STA		byte_573
loc_593AD:
		LDX		#9
		JSR		_mul_rand
		STA		byte_576
		LDA		byte_573
		STA		byte_575
		JSR		_generate_new_map_sub3
		LDX		#9
		JSR		_mul_rand
		STA		_ptr0
		LDA		#$47
		SEC
		SBC		_ptr0
		STA		byte_576
		JSR		_generate_new_map_sub3
		LDA		#0
		STA		byte_576
		JSR		_generate_new_map_sub3
		LDA		#$48
		STA		byte_576
		JSR		_generate_new_map_sub3
		LDX		byte_573
		INX
		INX
		STX		byte_573
		CPX		#$4C
		BCC		loc_593AD
		LDA		#0
		STA		byte_574
loc_593F1:
		LDX		#9
		JSR		_mul_rand
		STA		byte_575
		LDA		byte_574
		STA		byte_576
		JSR		_generate_new_map_sub3
		LDX		#9
		JSR		_mul_rand
		STA		_ptr0
		LDA		#$47
		SEC
		SBC		_ptr0
		STA		byte_575
		JSR		_generate_new_map_sub3
		LDA		#0
		STA		byte_575
		JSR		_generate_new_map_sub3
		LDA		#$48
		STA		byte_575
		JSR		_generate_new_map_sub3
		LDX		byte_574
		INX
		INX
		STX		byte_574
		CPX		#$4C
		BCC		loc_593F1
		JSR		_generate_new_mapD
		JSR		_generate_new_mapE
		JSR		_generate_new_mapF
		JSR		_generate_new_mapG
		RTS

; =============== S U B	R O U T	I N E =======================================
_isle_map_tile_write:
		PHA
		LDA		byte_574
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_573
		STA		_ptr0
		TXA
		ADC		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
		PLA
		STA		(_ptr0),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_wram_map_clear:
		LDA		#<[_wram_map_buf]
		STA		_ptr0
		LDA		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
loc_5946A:
		LDA		#0
		STA		(_ptr0),Y
		INC		_ptr0
		BNE		loc_59474
		INC		_ptr0+1
loc_59474:
		LDA		_ptr0+1
		CMP		#>[_power_grid_map_buf]
		BCC		loc_5946A
		LDA		_ptr0
		CMP		#<[_power_grid_map_buf]
		BCC		loc_5946A
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapA:
		LDX		#$19
		JSR		_mul_rand
		CLC
		ADC		#$1A
		STA		byte_573
		STA		byte_575
		LDX		#$19
		JSR		_mul_rand
		CLC
		ADC		#$1A
		STA		byte_574
		STA		byte_576
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapB:
		LDX		#4
		JSR		_mul_rand
		STA		byte_57F
		STA		byte_57E
		JSR		_generate_new_mapB_0
		LDA		byte_573
		STA		byte_575
		LDA		byte_574
		STA		byte_576
		LDA		byte_57F
		EOR		#4
		STA		byte_57F
		STA		byte_57E
		JSR		_generate_new_mapB_0
		LDA		byte_573
		STA		byte_575
		LDA		byte_574
		STA		byte_576
		LDX		#4
		JSR		_mul_rand
		STA		byte_57F
		JSR		_generate_new_mapB_1
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapB_0:
		JSR		_generate_new_map_sub4
		BCS		locret_5950D
		JSR		_generate_new_map_sub0
		JSR		_rand
		AND		#1
		BEQ		loc_594F0
		INC		byte_57E
loc_594F0:
		JSR		_rand
		AND		#1
		BEQ		loc_594FA
		DEC		byte_57E
loc_594FA:
		JSR		_rand
		CMP		#$18
		BCS		loc_59507
		LDA		byte_57F
		STA		byte_57E
loc_59507:
		JSR		_generate_new_mapB_0A
		JMP		_generate_new_mapB_0
locret_5950D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapB_1:
		JSR		_generate_new_map_sub4
		BCS		locret_5953D
		JSR		_generate_new_map_sub3
		JSR		_rand
		AND		#1
		BEQ		loc_59520
		INC		byte_57E
loc_59520:
		JSR		_rand
		AND		#1
		BEQ		loc_5952A
		DEC		byte_57E
loc_5952A:
		JSR		_rand
		CMP		#$14
		BCS		loc_59537
		LDA		byte_57F
		STA		byte_57E
loc_59537:
		JSR		_generate_new_mapB_0A
		JMP		_generate_new_mapB_1
locret_5953D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapF:
		LDX		#$32
		JSR		_mul_rand
		CLC
		ADC		#$17
		STA		byte_57C
loc_59549:
		LDX		#$4C
		JSR		_mul_rand
		STA		byte_575
		LDX		#$4C
		JSR		_mul_rand
		STA		byte_576
		JSR		_generate_new_mapF_0
		DEC		byte_57C
		BNE		loc_59549
		JSR		_generate_new_map_sub5
		JSR		_generate_new_map_sub5
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapF_0:
		LDX		#$3C
		JSR		_mul_rand
		CLC
		ADC		#$17
		STA		byte_57D
		LDA		byte_575
		STA		byte_577
		LDA		byte_576
		STA		byte_578
loc_5957F:
		LDX		#8
		JSR		_mul_rand
		STA		byte_57E
		JSR		_generate_new_mapF_0A
		JSR		_generate_new_map_sub2
		BCS		locret_595BA
		LDA		byte_578
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_577
		STA		_ptr0
		TXA
		ADC		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
		LDA		(_ptr0),Y
		BNE		loc_595B5
		LDA		#5
		STA		(_ptr0),Y
loc_595B5:
		DEC		byte_57D
		BNE		loc_5957F
locret_595BA:
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapC:
		LDX		#5
		JSR		_mul_rand
		TAX
		INX
		STX		byte_580
loc_595C5:
		LDX		#$3E
		JSR		_mul_rand
		CLC
		ADC		#3
		STA		byte_575
		LDX		#$3E
		JSR		_mul_rand
		CLC
		ADC		#3
		STA		byte_576
		LDX		#6
		JSR		_mul_rand
		ADC		#1
		STA		byte_581
loc_595E5:
		LDX		#7
		JSR		_mul_rand
		SEC
		SBC		#3
		CLC
		ADC		byte_575
		STA		byte_577
		LDX		#7
		JSR		_mul_rand
		SEC
		SBC		#3
		CLC
		ADC		byte_576
		STA		byte_578
		JSR		_rand
		CMP		#$34
		BCC		loc_59610
		JSR		_generate_new_map_sub3
		JMP		loc_59613
loc_59610:
		JSR		_generate_new_map_sub0
loc_59613:
		DEC		byte_581
		BNE		loc_595E5
		DEC		byte_580
		BNE		loc_595C5
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapF_0A:
		LDA		byte_57E
		AND		#7
		STA		byte_57E
		LDX		byte_57E
		LDA		byte_577
		CLC
		ADC		byte_5965E,X
		STA		byte_577
		LDA		byte_578
		CLC
		ADC		byte_59666,X
		STA		byte_578
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapB_0A:
		LDA		byte_57E
		AND		#7
		STA		byte_57E
		LDX		byte_57E
		LDA		byte_575
		CLC
		ADC		byte_5965E,X
		STA		byte_575
		LDA		byte_576
		CLC
		ADC		byte_59666,X
		STA		byte_576
		RTS
byte_5965E:
		.BYTE	$00,$01,$01,$01
		.BYTE	$00,$FF,$FF,$FF
byte_59666:
		.BYTE	$FF,$FF,$00,$01
		.BYTE	$01,$01,$00,$FF

; =============== S U B	R O U T	I N E =======================================
_generate_new_map_sub0:
		LDA		#0
		STA		byte_579
loc_59673:
		LDX		byte_579
		LDA		byte_575
		CLC
		ADC		byte_596CF,X
		STA		byte_577
		LDA		byte_576
		CLC
		ADC		byte_59700,X
		STA		byte_578
		LDA		byte_5969E,X
		BEQ		loc_59692
		JSR		_generate_new_map_sub1
loc_59692:
		LDX		byte_579
		INX
		STX		byte_579
		CPX		#$31
		BCC		loc_59673
		RTS
byte_5969E:
		.BYTE	$00,$00,$03,$03,$03,$00,$00
		.BYTE	$00,$03,$02,$02,$02,$03,$00
		.BYTE	$03,$02,$02,$02,$02,$02,$03
		.BYTE	$03,$02,$02,$04,$02,$02,$03
		.BYTE	$03,$02,$02,$02,$02,$02,$03
		.BYTE	$00,$03,$02,$02,$02,$03,$00
		.BYTE	$00,$00,$03,$03,$03,$00,$00
byte_596CF:
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
		.BYTE	$00,$01,$02,$03,$04,$05,$06
byte_59700:
		.BYTE	$00,$00,$00,$00,$00,$00,$00
		.BYTE	$01,$01,$01,$01,$01,$01,$01
		.BYTE	$02,$02,$02,$02,$02,$02,$02
		.BYTE	$03,$03,$03,$03,$03,$03,$03
		.BYTE	$04,$04,$04,$04,$04,$04,$04
		.BYTE	$05,$05,$05,$05,$05,$05,$05
		.BYTE	$06,$06,$06,$06,$06,$06,$06

; =============== S U B	R O U T	I N E =======================================
_generate_new_map_sub3:
		LDA		#0
		STA		byte_57A
loc_59736:
		LDX		byte_57A
		LDA		byte_575
		CLC
		ADC		byte_59771,X
		STA		byte_577
		LDA		byte_576
		CLC
		ADC		byte_59781,X
		STA		byte_578
		LDA		byte_59761,X
		BEQ		loc_59755
		JSR		_generate_new_map_sub1
loc_59755:
		LDX		byte_57A
		INX
		STX		byte_57A
		CPX		#$10
		BCC		loc_59736
		RTS
byte_59761:
		.BYTE	$00,$03,$03,$00
		.BYTE	$03,$02,$02,$03
		.BYTE	$03,$02,$02,$03
		.BYTE	$00,$03,$03,$00
byte_59771:
		.BYTE	$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03
		.BYTE	$00,$01,$02,$03
byte_59781:
		.BYTE	$00,$00,$00,$00
		.BYTE	$01,$01,$01,$01
		.BYTE	$02,$02,$02,$02
		.BYTE	$03,$03,$03,$03

; =============== S U B	R O U T	I N E =======================================
_generate_new_map_sub1:
		PHA
		JSR		_generate_new_map_sub2
		BCS		loc_597C7
		LDA		byte_578
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_577
		STA		_ptr0
		TXA
		ADC		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
		LDA		(_ptr0),Y
		BEQ		loc_597C9
		CMP		#4
		BEQ		loc_597C7
		CMP		#2
		BNE		loc_597C9
		PLA
		CMP		#4
		BEQ		loc_597CA
		RTS
loc_597C7:
		PLA
		RTS
loc_597C9:
		PLA
loc_597CA:
		STA		(_ptr0),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_map_sub2:
		LDA		byte_577
		CMP		#$4C
		BCS		loc_597DD
		LDA		byte_578
		CMP		#$4C
		BCS		loc_597DD
		CLC
		RTS
loc_597DD:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_map_sub4:
		LDA		byte_575
		BPL		loc_597EA
		CMP		#$FC
		BCC		loc_59800
		BCS		loc_597EE
loc_597EA:
		CMP		#$4A
		BCS		loc_59800
loc_597EE:
		LDA		byte_576
		BPL		loc_597F9
		CMP		#$FC
		BCC		loc_59800
		CLC
		RTS
loc_597F9:
		CMP		#$4A
		BCS		loc_59800
		BCS		loc_59800
		RTS
loc_59800:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_generate_new_map_sub5:
		LDA		#0
		STA		byte_575
		STA		byte_576
loc_5980A:
		LDA		byte_576
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_575
		STA		_ptr0
		TXA
		ADC		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
		LDA		(_ptr0),Y
		BMI		loc_5988D
		BEQ		loc_5988D
		LDA		#0
		STA		byte_57B
		STY		_tmp4
loc_59835:
		LDY		_tmp4
		CPY		#4
		BCS		loc_59883
		LDA		byte_575
		CLC
		ADC		byte_599B1,Y
		STA		byte_577
		LDA		byte_576
		CLC
		ADC		byte_599B5,Y
		STA		byte_578
		JSR		_generate_new_map_sub2
		BCS		loc_5987B
		LDA		byte_578
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_577
		STA		_tmp2
		TXA
		ADC		#>[_wram_map_buf]
		STA		_tmp3
		LDY		#0
		LDA		(_tmp2),Y
		BMI		loc_5987B
		BEQ		loc_5987B
		SEC
		BCS		loc_5987C
loc_5987B:
		CLC
loc_5987C:
		ROL		byte_57B
		INC		_tmp4
		BNE		loc_59835
loc_59883:
		LDX		byte_57B
		LDA		byte_598AD,X
		LDY		#0
		STA		(_ptr0),Y
loc_5988D:
		INC		byte_575
		LDA		byte_575
		CMP		#$4C
		BCS		loc_5989A
		JMP		loc_5980A
loc_5989A:
		INC		byte_576
		LDA		byte_576
		CMP		#$4C
		BCS		locret_598AC
		LDA		#0
		STA		byte_575
		JMP		loc_5980A
locret_598AC:
		RTS
byte_598AD:
		.BYTE	$00,$00,$00,$00
		.BYTE	$00,$16,$10,$13
		.BYTE	$00,$18,$12,$15
		.BYTE	$00,$17,$11,$14

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapD:
		LDA		#0
		STA		byte_575
		STA		byte_576
loc_598C5:
		LDA		byte_576
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_575
		STA		_ptr0
		TXA
		ADC		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
		LDA		(_ptr0),Y
		CMP		#3
		BEQ		loc_598EC
		JMP		loc_59991
loc_598EC:
		LDA		#0
		STA		byte_57B
		STY		_tmp4
loc_598F3:
		LDY		_tmp4
		CPY		#4
		BCC		loc_598FC
		JMP		loc_59987
loc_598FC:
		LDA		byte_575
		CLC
		ADC		byte_599B1,Y
		STA		byte_577
		LDA		byte_576
		CLC
		ADC		byte_599B5,Y
		STA		byte_578
		JSR		_generate_new_map_sub2
		BCC		loc_59957
		LDY		_tmp4
		LDA		byte_599B1,Y
		EOR		#$FF
		CLC
		ADC		#1
		CLC
		ADC		byte_575
		STA		byte_577
		LDA		byte_599B5,Y
		EOR		#$FF
		CLC
		ADC		#1
		CLC
		ADC		byte_576
		STA		byte_578
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_577
		STA		_tmp2
		TXA
		ADC		#>[_wram_map_buf]
		STA		_tmp3
		LDY		#0
		LDA		(_tmp2),Y
		BNE		loc_5997C
		SEC
		BCS		loc_5997D
loc_59957:
		LDA		byte_578
		LDX		#$4C
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		byte_577
		STA		_tmp2
		TXA
		ADC		#>[_wram_map_buf]
		STA		_tmp3
		LDY		#0
		LDA		(_tmp2),Y
		BEQ		loc_5997C
		SEC
		BCS		loc_5997D
loc_5997C:
		CLC
loc_5997D:
		ROL		byte_57B
		INC		_tmp4
		BEQ		loc_59987
		JMP		loc_598F3
loc_59987:
		LDX		byte_57B
		LDA		byte_599B9,X
		LDY		#0
		STA		(_ptr0),Y
loc_59991:
		INC		byte_575
		LDA		byte_575
		CMP		#$4C
		BCS		loc_5999E
		JMP		loc_598C5
loc_5999E:
		INC		byte_576
		LDA		byte_576
		CMP		#$4C
		BCS		locret_599B0
		LDA		#0
		STA		byte_575
		JMP		loc_598C5
locret_599B0:
		RTS
byte_599B1:
		.BYTE	$FF,$01,$00,$00
byte_599B5:
		.BYTE	$00,$00,$01,$FF
byte_599B9:
		.BYTE	$00,$00,$00,$07
		.BYTE	$00,$15,$10,$13
		.BYTE	$00,$17,$12,$14
		.BYTE	$07,$16,$11,$07

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapG:
		LDA		#<[_wram_map_buf]
		STA		_ptr0
		LDA		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
loc_599D3:
		LDA		(_ptr0),Y
		BMI		loc_599E8
		BEQ		loc_599FB
		CLC
		ADC		#$BE
		PHA
		JSR		_rand
		LSR
		PLA
		BCS		loc_599E6
		ADC		#9
loc_599E6:
		STA		(_ptr0),Y
loc_599E8:
		INC		_ptr0
		BNE		loc_599EE
		INC		_ptr0+1
loc_599EE:
		LDA		_ptr0+1
		CMP		#>[_power_grid_map_buf]
		BCC		loc_599D3
		LDA		_ptr0
		CMP		#<[_power_grid_map_buf]
		BCC		loc_599D3
		RTS
loc_599FB:
		LDA		#$90
		STA		(_ptr0),Y
		BNE		loc_599E8
		LDA		#$D2
		STA		(_ptr0),Y
		BNE		loc_599E8

; =============== S U B	R O U T	I N E =======================================
_generate_new_mapE:
		LDA		#<[_wram_map_buf]
		STA		_ptr0
		LDA		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#0
loc_59A11:
		LDA		(_ptr0),Y
		BEQ		loc_59A43
		CMP		#2
		BEQ		loc_59A47
		CMP		#7
		BEQ		loc_59A47
		CMP		#4
		BEQ		loc_59A4D
		CLC
		ADC		#$AE
		PHA
		JSR		_rand
		LSR
		PLA
		BCS		loc_59A2E
		ADC		#8
loc_59A2E:
		STA		(_ptr0),Y
loc_59A30:
		INC		_ptr0
		BNE		loc_59A36
		INC		_ptr0+1
loc_59A36:
		LDA		_ptr0+1
		CMP		#>[_power_grid_map_buf]
		BCC		loc_59A11
		LDA		_ptr0
		CMP		#<[_power_grid_map_buf]
		BCC		loc_59A11
		RTS
loc_59A43:
		LDA		#$90
		BNE		loc_59A30
loc_59A47:
		LDA		#$BD
		STA		(_ptr0),Y
		BNE		loc_59A30
loc_59A4D:
		LDA		#$FE
		STA		(_ptr0),Y
		BNE		loc_59A30

; =============== S U B	R O U T	I N E =======================================
_mul_rand:
		JSR		_rand
		STA		_MMC5_MUL1
		STX		_MMC5_MUL0
		LDX		_MMC5_MUL0
		LDA		_MMC5_MUL1
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_popul_draw:
		JSR		_get_cur_popul
		JSR		_cur_popul_str_calc
		JSR		_cur_popul_str_pos_set
		JSR		_cur_popul_str_queue
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_cur_popul:
		LDA		_popul_cur
		STA		_ptr0
		LDA		_popul_cur+1
		STA		_ptr0+1
		LDA		_popul_cur+2
		STA		_tmp2
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_popul_str_calc:
		LDA		#0
		STA		_tmp3
		LDY		#0
		LDX		#5
loc_59A88:
		JSR		_hex_to_dec_999999
		CMP		#0
		BNE		loc_59A9E
		LDA		_tmp3
		BNE		loc_59AA8
		CPX		#0
		BEQ		loc_59A9E
		LDA		#$DF
		STA		_pre_ppu_buf,Y
		BNE		loc_59AA3
loc_59A9E:
		STA		_pre_ppu_buf,Y
		DEC		_tmp3
loc_59AA3:
		INY
		DEX
		BPL		loc_59A88
		RTS
loc_59AA8:
		LDA		#0
		STA		_pre_ppu_buf,Y
		BEQ		loc_59AA3

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec_999999:
		LDA		#0
		STA		_tmp4
loc_59AB3:
		SEC
		LDA		_ptr0
		SBC		byte_59AE8,X
		STA		_ptr0
		LDA		_ptr0+1
		SBC		byte_59AEE,X
		STA		_ptr0+1
		LDA		_tmp2
		SBC		byte_59AF4,X
		STA		_tmp2
		BCC		loc_59ACF
		INC		_tmp4
		BNE		loc_59AB3
loc_59ACF:
		CLC
		LDA		_ptr0
		ADC		byte_59AE8,X
		STA		_ptr0
		LDA		_ptr0+1
		ADC		byte_59AEE,X
		STA		_ptr0+1
		LDA		_tmp2
		ADC		byte_59AF4,X
		STA		_tmp2
		LDA		_tmp4
		RTS
byte_59AE8:
		.BYTE	$01,$0A,$64,$E8,$10,$A0
byte_59AEE:
		.BYTE	$00,$00,$00,$03,$27,$86
byte_59AF4:
		.BYTE	$00,$00,$00,$00,$00,$01

; =============== S U B	R O U T	I N E =======================================
_cur_popul_str_pos_set:
		LDA		_cur_screen_tile._Y
		LSR
		CLC
		ADC		#$1A
		CMP		#$1E
		BCC		loc_59B06
		SBC		#$1E
loc_59B06:
		STA		_ptr0+1
		LDA		_cur_screen_tile._X
		LSR
		CLC
		ADC		#$E
		CMP		#$20
		BCC		loc_59B14
		SBC		#$20
loc_59B14:
		STA		_ptr0
		CLC
		ADC		#5
		CMP		#$20
		BCC		loc_59B2A
		SBC		#$1F
		STA		_tmp3
		LDA		#6
		SEC
		SBC		_tmp3
		STA		_tmp2
		BNE		loc_59B32
loc_59B2A:
		LDA		#6
		STA		_tmp2
		LDA		#0
		STA		_tmp3
loc_59B32:
		LDA		_ptr0+1
		LDX		#$20
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
		CLC
		ADC		_ptr0
		BCC		loc_59B48
		INX
loc_59B48:
		STA		_ptr0
		TXA
		ORA		#$20
		STA		_ptr0+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_cur_popul_str_queue:
		LDX		_ppu_queue_pos_head
		LDY		#0
		LDA		_ptr0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		INX
loc_59B66:
		LDA		_pre_ppu_buf,Y
		INY
		STA		_ppu_queue_buf,X
		INX
		DEC		_tmp2
		BNE		loc_59B66
		LDA		_tmp3
		BEQ		loc_59B93
		STA		_tmp2
		LDA		#0
		STA		_tmp3
		LDA		_ptr0
		AND		#$E0
		STA		_ppu_queue_buf,X
		INX
		LDA		_ptr0+1
		STA		_ppu_queue_buf,X
		INX
		LDA		_tmp2
		STA		_ppu_queue_buf,X
		INX
		JMP		loc_59B66
loc_59B93:
		STX		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_msg_wnd_cheat_test:
		LDA		_pad0_held
		ORA		_pad1_prev_held
		AND		#$CF
		CMP		#$CF
		BNE		loc_59BA9
		LDA		_debug_scroll_msg_flags
		ORA		#1
		STA		_debug_scroll_msg_flags
		RTS
loc_59BA9:
		LDA		_pad0_held
		AND		_pad1_prev_held
		BEQ		locret_59BBF
		LDA		_pad0_held
		CMP		_pad1_prev_held
		BEQ		locret_59BBF
		BCC		locret_59BBF
		LDA		_debug_scroll_msg_flags
		ORA		#2
		STA		_debug_scroll_msg_flags
locret_59BBF:
		RTS

; =============== S U B	R O U T	I N E =======================================
_debug_flag_toggle_test:
		LDA		_debug_scroll_msg_flags
		CMP		#3
		BNE		locret_59BCF
		LDA		_city._game_flags
		EOR		#$10
		STA		_city._game_flags
locret_59BCF:
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_reset:
		JSR		_sram_on
		INC		_sram_data_crc
		JSR		_sram_off
		JMP		RESET

; =============== S U B	R O U T	I N E =======================================
_sram_sum_reinit:
		JSR		_sram_sum_calc
		LDA		_tmp2
		STA		_sram_data_crc
		LDA		_tmp3
		STA		_sram_data_crc+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_init:
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
		RTS
loc_59C0E:
		LDA		#>[_land_value_map_buf]
		STA		_ptr0+1
		LDA		#<[_land_value_map_buf]
		STA		_ptr0
		STA		_has_saved_city_data_flag
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
		RTS

; =============== S U B	R O U T	I N E =======================================
_sram_sum_calc:
		LDY		#0
		STY		_tmp4
		STY		_tmp5
		STY		_tmp6
		STY		_tmp2
		STY		_tmp3
		STY		_ptr0
		LDA		#>[_sram_graphs]
		STA		_ptr0+1
		LDY		#<[_sram_graphs]
loc_59C3F:
		LDA		(_ptr0),Y
		JSR		_sum_calc
		INY
		BNE		loc_59C3F
		INC		_ptr0+1
		LDA		_ptr0+1
		CMP		#$A0
		BNE		loc_59C3F
		LDA		#0
		JSR		_sum_calc
		LDA		#0

; =============== S U B	R O U T	I N E =======================================
_sum_calc:
		STA		_tmp6
		LDA		_tmp2
		STA		_tmp4
		LDA		_tmp3
		STA		_tmp5
		LDX		#0
loc_59C62:
		LSR		_tmp6
		ROR		_tmp5
		ROR		_tmp4
		BCC		loc_59C76
		LDA		_tmp4
		EOR		#5
		STA		_tmp4
		LDA		_tmp5
		EOR		#$C0
		STA		_tmp5
loc_59C76:
		INX
		CPX		#8
		BNE		loc_59C62
		LDA		_tmp4
		STA		_tmp2
		LDA		_tmp5
		STA		_tmp3
		RTS
_scroll_txt_i_am_god:
		.BYTE	$12,$DF,$0A,$16,$DF,$10,$18, $0D,$DF,$DF,$DF,$12,$DF,$20,$12,$1C
		.BYTE	$11,$DF,$22,$18,$1E,$DF,$11,$0A,$19,$19,$22,$FF
_scroll_txt_debug_off:
		.BYTE	$12,$DF,$0A,$16,$DF,$10,$18,$0D,$DF,$DF,$DF,$0D,$0E,$0B,$1E,$10
		.BYTE	$DF,$18,$0F,$0F,$FF
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
_scroll_txt00_more_residental:
		.BYTE	$16,$18,$1B,$0E,$DF,$1B,$0E,$1C,$12,$0D,$0E,$17,$1D,$12,$0A,$15
		.BYTE	$DF,$23,$18,$17,$0E,$1C,$DF,$17,$0E,$0E,$0D,$0E,$0D,$FF
_scroll_txt01_more_commercial:
		.BYTE	$16,$18,$1B,$0E,$DF,$0C,$18,$16,$16,$0E,$1B,$0C,$12,$0A,$15,$DF
		.BYTE	$23,$18,$17,$0E,$1C,$DF,$17,$0E,$0E,$0D,$0E,$0D,$FF
_scroll_txt02_more_industrial:
		.BYTE	$16,$18,$1B,$0E,$DF,$12,$17,$0D,$1E,$1C,$1D,$1B,$12,$0A,$15,$DF
		.BYTE	$23,$18,$17,$0E,$1C,$DF,$17,$0E,$0E,$0D,$0E,$0D,$FF
_scroll_txt03_more_roads:
		.BYTE	$16,$18,$1B,$0E,$DF,$1B,$18,$0A, $0D,$1C,$DF,$1B, $0E,$1A,$1E,$12
		.BYTE	$1B,$0E,$0D,$FF
_scroll_txt04_inadeqate_rail_system:
		.BYTE	$12,$17,$0A,$0D,$0E,$1A,$1E,$0A,$1D,$0E,$DF,$1B,$0A,$12,$15,$DF
		.BYTE	$1C,$22,$1C,$1D,$0E,$16,$FF
_scroll_txt05_build_a_power_plant:
		.BYTE	$0B,$1E,$12,$15,$0D,$DF,$0A,$DF,$19,$18,$20,$0E,$1B,$DF,$19,$15
		.BYTE	$0A,$17,$1D,$FF
_scroll_txt06_demand_a_stadium:
		.BYTE	$1B,$0E,$1C,$12,$0D,$0E,$17,$1D,$1C,$DF,$0D,$0E,$16,$0A,$17,$0D
		.BYTE	$DF,$0A,$DF,$1C,$1D,$0A,$0D,$12,$1E,$16,$FF
_scroll_txt07_requires_a_sea_port:
		.BYTE	$12,$17,$0D,$1E,$1C,$1D,$1B,$22,$DF,$1B,$0E,$1A,$1E,$12,$1B,$0E
		.BYTE	$1C,$DF,$0A,$DF,$1C,$0E,$0A,$DF,$19,$18,$1B,$1D,$FF
_scroll_txt08_requires_airport:
		.BYTE	$0C,$18,$16,$16,$0E,$1B,$0C,$0E,$DF,$1B,$0E,$1A,$1E,$12,$1B,$0E
		.BYTE	$1C,$DF,$0A,$17,$DF,$0A,$12,$1B,$19,$18,$1B,$1D,$FF
_scroll_txt09_demand_a_fire_department:
		.BYTE	$0C,$12,$1D,$12,$23,$0E,$17,$1C,$DF,$0D,$0E,$16,$0A,$17,$0D,$DF
		.BYTE	$0A,$DF,$0F,$12,$1B,$0E,$DF,$0D,$0E,$19,$0A,$1B,$1D,$16,$0E,$17
		.BYTE	$1D,$FF
_scroll_txt0A_demand_a_police_department:
		.BYTE	 $0C,$12,$1D,$12,$23,$0E,$17,$1C,$DF,$0D, $0E,$16, $0A,$17, $0D,$DF
		.BYTE	$0A,$DF,$19,$18,$15,$12,$0C,$0E,$DF,$0D,$0E,$19,$0A,$1B,$1D,$16
		.BYTE	$0E,$17,$1D,$FF
_scroll_txt0B_blackouts_reported:
		.BYTE	 $0B,$15, $0A,$0C,$14,$18,$1E,$1D,$1C,$DF,$1B,$0E,$19,$18,$1B,$1D
		.BYTE	$0E,$0D,$DF,$DF,$0C,$11,$0E,$0C,$14,$DF,$19,$18,$20,$0E,$1B,$DF
		.BYTE	$16,$0A,$19,$FF
_scroll_txt0C_tax_rate_is_too_high:
		.BYTE	$0C,$12,$1D,$12,$23,$0E,$17,$1C,$DF,$1E,$19,$1C,$0E,$1D,$DF,$DF
		.BYTE	$1D,$11,$0E,$DF,$1D,$0A,$21,$DF,$1B,$0A,$1D,$0E,$DF,$12,$1C,$DF
		.BYTE	$1D,$18,$18,$DF,$11,$12,$10,$11,$FF
_scroll_txt0D_roads_deteriorating:
		.BYTE	$1B,$18,$0A,$0D,$1C,$DF,$0D,$0E,$1D,$0E,$1B,$12,$18,$1B,$0A,$1D
		.BYTE	$12,$17,$10,$DF,$DF,$0D,$1E,$0E,$DF,$1D,$18,$DF,$15,$0A,$0C,$14
		.BYTE	$DF,$18,$0F,$DF,$0F,$1E,$17,$0D,$1C,$FF
_scroll_txt0E_fire_department_need_funding:
		.BYTE	$0F,$12,$1B,$0E,$DF,$0D,$0E,$19,$0A,$1B,$1D,$16,$0E,$17,$1D,$1C
		.BYTE	$DF,$17,$0E,$0E,$0D,$DF,$0F,$1E,$17,$0D,$12,$17,$10,$FF
_scroll_txt0F_police_need_funding:
		.BYTE	$19,$18,$15,$12,$0C,$0E,$DF,$0D,$0E,$19,$0A,$1B,$1D,$16,$0E,$17
		.BYTE	$1D,$1C,$DF,$17,$0E,$0E,$0D,$DF,$0F,$1E,$17,$0D,$12,$17,$10,$FF
_scroll_txt10_plane_crashed:
		.BYTE	$0A,$DF,$19,$15,$0A,$17,$0E,$DF,$11,$0A,$1C,$DF,$0C,$1B,$0A,$1C
		.BYTE	$11,$0E,$0D,$FF
_scroll_txt11_shipwreck_reported:
		.BYTE	$1C,$11,$12,$19,$20,$1B, $0E,$0C,$14,$DF,$1B,$0E,$19,$18,$1B,$1D
		.BYTE	$0E,$0D,$FF
_scroll_txt12_train_crashed:
		.BYTE	$0A,$DF,$1D,$1B,$0A,$12,$17,$DF,$0C,$1B,$0A,$1C,$11,$0E,$0D,$FF
_scroll_txt13_heli_crashed:
		.BYTE	$0A,$DF,$11,$0E,$15,$12,$0C,$18,$19,$1D,$0E,$1B,$DF,$0C,$1B,$0A
		.BYTE	$1C,$11,$0E,$0D,$FF
_scroll_txt14_hi_unemployment_rate:
		.BYTE	$1E,$17,$0E,$16,$19,$15,$18,$22,$16,$0E,$17,$1D,$DF,$1B,$0A,$1D
		.BYTE	$0E,$DF,$12,$1C,$DF,$11,$12,$10,$11,$FF
_scroll_txt15_city_gone_broke:
		.BYTE	$22,$18,$1E,$1B,$DF,$0C,$12,$1D,$22,$DF,$11,$0A,$1C,$DF,$10,$18
		.BYTE	$17,$0E,$DF,$0B,$1B,$18,$14,$0E,$FF
_scroll_txt16_explosion_detected:
		.BYTE	 $0E,$21,$19,$15,$18,$1C,$12,$18,$17,$DF, $0D,$0E,$1D,$0E, $0C,$1D
		.BYTE	$0E,$0D,$FF
_scroll_txt17_brownouts:
		.BYTE	$0B,$1B,$18,$20,$17,$18,$1E,$1D,$1C,$DF,$DF,$0B,$1E,$12,$15,$0D
		.BYTE	$DF,$0A,$17,$18,$1D,$11,$0E,$1B,$DF,$19,$18,$20,$0E,$1B,$DF,$19
		.BYTE	$15,$0A,$17,$1D,$FF
_scroll_txt18_heavy_traffic_repotrted:
		.BYTE	$11,$0E,$0A,$1F,$22,$DF,$1D,$1B,$0A,$0F,$0F,$12,$0C,$DF,$1B,$0E
		.BYTE	$19,$18,$1B,$1D,$0E,$0D,$FF
_unused_trash:
		.BYTE	$1B,$0E,$19,$18,$1B,$1D,$0E,$0D,$FF,$00,$04,$08,$00,$00,$14,$C0
		.BYTE	$00,$00,$44,$0D,$00,$00,$AC,$97,$20,$08,$77,$7A,$00,$00,$E3,$6C
		.BYTE	$04,$00,$4D,$5E,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$7F,$FF
		.BYTE	$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF,$DF
		.BYTE	$FF,$FF,$FF,$FF,$00,$00,$21,$CC,$00,$00,$20,$B4,$00,$20,$2F,$09
		.BYTE	$04,$00,$81,$20,$4B,$80,$2A,$76,$98,$50,$27,$E0,$15,$B5,$26,$B1
		.BYTE	$BA,$EC,$D2,$49

		BANK_END 3,$C000
