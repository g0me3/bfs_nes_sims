
		ORG $8000

		BANK_START $F4

; ---------------------------------------------------------------------------
; -------------G-A-M-E--S-E-T-U-P--A-N-D--I-N-I-T--S-T-U-F-F-----------------
; ---------------------------------------------------------------------------

		SECTION_START 301

; =============== S U B	R O U T	I N E =======================================
_game_msg_pal_select:
		LDA		_game_msg_idx
		CMP		#$08
		BEQ		loc_7000E
		CMP		#$09
		BEQ		loc_70012
		LDA		#$81
		BNE		loc_70014
loc_7000E:
		LDA		#$89
		BNE		loc_70014
loc_70012:
		LDA		#$8A
loc_70014:
		JMP		_pal_load_lib

; =============== S U B	R O U T	I N E =======================================
_game_msg_hints_extra_icons_draw:
		LDA		_game_msg_idx
		CMP		#$1C
		BEQ		loc_7001E
		RTS
loc_7001E:
		LDA		_ptr0
		BNE		loc_7002F
		JSR		_game_msg_extra_icon5_draw
		JSR		_game_msg_extra_icon4_draw
		JSR		_game_msg_extra_icon3_draw
		JMP		_game_msg_extra_icon2_draw
loc_7002F:
		JSR		_game_msg_extra_icon0_draw
		JMP		_game_msg_extra_icon1_draw

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icon5_draw:
;		JSR		_game_msg_extra_icons_set_tmp3
		LDA		#$06
		STA		_tmp1
		LDA		#$11
		STA		_tmp2
		JSR		_game_msg_extra_icons_pos_calc
		LDY		#$00
		JMP		_game_msg_extra_icons_draw

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icon4_draw:
;		JSR		_game_msg_extra_icons_set_tmp3
		LDA		#$07
		STA		_tmp1
		LDA		#$11
		STA		_tmp2
		JSR		_game_msg_extra_icons_pos_calc
		LDY		#$04
		JMP		_game_msg_extra_icons_draw

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icon3_draw:
;		JSR		_game_msg_extra_icons_set_tmp3
		LDA		#$09
		STA		_tmp1
		LDA		#$1A
		STA		_tmp2
		JSR		_game_msg_extra_icons_pos_calc
		LDY		#$08
		JMP		_game_msg_extra_icons_draw

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icon2_draw:
;		JSR		_game_msg_extra_icons_set_tmp3
		LDA		#$0A
		STA		_tmp1
		LDA		#$1A
		STA		_tmp2
		JSR		_game_msg_extra_icons_pos_calc
		LDY		#$0C
		BNE		_game_msg_extra_icons_draw

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icon0_draw:
;		JSR		_game_msg_extra_icons_set_tmp3
		LDA		#$06
		STA		_tmp1
		LDA		#$16
		STA		_tmp2
		JSR		_game_msg_extra_icons_pos_calc
		LDY		#$10
		BNE		_game_msg_extra_icons_draw

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icon1_draw:
;		JSR		_game_msg_extra_icons_set_tmp3
		LDA		#$07
		STA		_tmp1
		LDA		#$16
		STA		_tmp2
		JSR		_game_msg_extra_icons_pos_calc
		LDY		#$14
		BNE		_game_msg_extra_icons_draw

; =============== S U B	R O U T	I N E =======================================
;_game_msg_extra_icons_set_tmp3:
;		LDA		#$04
;		STA		_tmp3
;		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icons_pos_calc:
		CIADDB	_tmp1,_cur_screen_tile._ROW,_tmp1,NONE,#$1E
		CIADDB	_tmp2,_cur_screen_tile._COL,_tmp2,NONE,#$20
		WRAPA	_tmp4,_tmp5,#$04,#$20
		BUFPOS	_ptr2,_tmp2,_tmp1,#$20,#$00	; DST=(_ptr0+1*#$20+_tmp2)+#$0000
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_msg_extra_icons_draw:
		STY		_ptr0+1
		PUSHB	_tmp4
		PUSHB	_tmp5
		EXTQSTART
		EXTQMOVB	_tmp2,OP_NOP,0
loc_7011D:
		EXTQMOVB	_tmp3,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_tmp4,OP_NOP,0
loc_7011E:
		LDA		byte_77199,Y
		EXTQMOVA
		INY
		DEC		_tmp4
		BNE		loc_7011E
		DO_WRAP	_tmp4,_tmp5,loc_7014D
		EXTQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_7011D
loc_7014D:
		EXTQEND
		LDY		_ptr0+1
		POPB	_tmp5
		POPB	_tmp4
		PPUQSTART
		PPUQMOVB	_tmp2,OP_NOP,0
loc_70166:
		PPUQMOVB	_tmp3,OP_ORA,#$20
		PPUQMOVB	_tmp4,OP_NOP,0
loc_70167:
		LDA		byte_771B1,Y
		PPUQMOVA
		INY
		DEC		_tmp4
		BNE		loc_70167
		DO_WRAP	_tmp4,_tmp5,loc_70196
		PPUQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_70166
loc_70196:
		PPUQEND
		RTS

byte_77199:
		.BYTE	$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B
byte_771B1:
		.BYTE	$04,$05,$06,$09
		.BYTE	$14,$15,$16,$19
		.BYTE	$04,$07,$08,$09
		.BYTE	$14,$17,$18,$19
		.BYTE	$00,$01,$02,$03
		.BYTE	$10,$11,$12,$13

; =============== S U B	R O U T	I N E =======================================
_game_msg_presents_icon_draw:
		LDX		_game_msg_idx
		LDA		_game_msg_presentss_list,X
		BPL		loc_701D1
		RTS
loc_701D1:
		LDX		#$02
		STX		_ptr0+1
loc_701D5:
		PHA
		LDX		#$09
		MULXA
		STA		_tmp2
		CLC
		ADC		#<[_tlm_nt_game_msg_presentss_icons_lib]
		STA		_tmp_ptrC1
		TXA
		ADC		#>[_tlm_nt_game_msg_presentss_icons_lib]
		STA		_tmp_ptrC1+1
		LDA		_tmp2
		CLC
		ADC		#<[_tlm_extnt_game_msg_presentss_icons_lib]
		STA		_tmp_ptrC3
		TXA
		ADC		#>[_tlm_extnt_game_msg_presentss_icons_lib]
		STA		_tmp_ptrC3+1
		JSR		_game_msg_presents_pos_calc
		LDX		_ptr0+1
		LDY		byte_7720D,X
		JSR		_game_msg_presents_icon_line_draw
		PLA
		DEC		_ptr0+1
		BPL		loc_701D5
		RTS

byte_7720D:
		.BYTE	$00,$03,$06

_game_msg_presentss_list:
		.BYTE	$80,$80,$00,$80,$80,$80,$80,$80,$80,$80,$80,$01,$80,$02,$03,$04
		.BYTE	$05,$06,$07,$08,$09,$80,$0A,$0B,$0C,$0D,$0E,$80,$80,$80,$80,$80
		.BYTE	$80,$80,$80,$80

; =============== S U B	R O U T	I N E =======================================
_game_msg_presents_icon_line_draw:
		TYA
		PHA
		PUSHB	_tmp4
		PUSHB	_tmp5
		EXTQSTART
		EXTQMOVB	_tmp2,OP_NOP,0
loc_70251:
		EXTQMOVB	_tmp3,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_tmp4,OP_NOP,0
loc_70252:
		LDA		(_tmp_ptrC3),Y
		EXTQMOVA
		INY
		DEC		_tmp4
		BNE		loc_70252
		DO_WRAP	_tmp4,_tmp5,loc_70280
		EXTQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_70251
loc_70280:
		EXTQEND
		POPB	_tmp5
		POPB	_tmp4
		PLA
		TAY
		PPUQSTART
		PPUQMOVB	_tmp2,OP_NOP,0
loc_70299:
		PPUQMOVB	_tmp3,OP_ORA,#$20
		PPUQMOVB	_tmp4,OP_NOP,0
loc_7029A:
		LDA		(_tmp_ptrC1),Y
		PPUQMOVA
		INY
		DEC		_tmp4
		BNE		loc_7029A
		DO_WRAP	_tmp4,_tmp5,loc_702C8
		PPUQMOVB	_tmp2,OP_AND,#$E0
		JMP		loc_70299
loc_702C8:
		PPUQEND
		RTS

_tlm_nt_game_msg_presentss_icons_lib:
		.BYTE	$3A,$3B,$3C
		.BYTE	$4A,$4B,$4C
		.BYTE	$5A,$5B,$5C
		.BYTE	$03,$04,$05
		.BYTE	$13,$14,$15
		.BYTE	$23,$24,$25
		.BYTE	$31,$32,$33
		.BYTE	$41,$42,$43
		.BYTE	$51,$52,$53
		.BYTE	$09,$0A,$0B
		.BYTE	$19,$1A,$1B
		.BYTE	$29,$2A,$2B
		.BYTE	$06,$07,$08
		.BYTE	$16,$17,$18
		.BYTE	$26,$27,$28
		.BYTE	$DD,$DE,$DF
		.BYTE	$ED,$EE,$EF
		.BYTE	$FD,$FE,$FF
		.BYTE	$A6,$A7,$A8
		.BYTE	$B6,$B7,$B8
		.BYTE	$C6,$C7,$C8
		.BYTE	$A9,$AA,$AB
		.BYTE	$B9,$BA,$BB
		.BYTE	$C9,$CA,$CB
		.BYTE	$A0,$A1,$A2
		.BYTE	$B0,$B1,$B2
		.BYTE	$C0,$C1,$C2
		.BYTE	$A3,$A4,$A5
		.BYTE	$B3,$B4,$B5
		.BYTE	$C3,$C4,$C5
		.BYTE	$D0,$D1,$D2
		.BYTE	$E0,$E1,$E2
		.BYTE	$F0,$F1,$F2
		.BYTE	$00,$01,$02
		.BYTE	$10,$11,$12
		.BYTE	$20,$21,$22
		.BYTE	$AC,$AD,$AE
		.BYTE	$BC,$BD,$BE
		.BYTE	$CC,$CD,$CE
		.BYTE	$0C,$0D,$0E
		.BYTE	$1C,$1D,$1E
		.BYTE	$2C,$2D,$2E
		.BYTE	$37,$38,$39
		.BYTE	$47,$48,$49
		.BYTE	$57,$58,$59
_tlm_extnt_game_msg_presentss_icons_lib:
		.BYTE	$D2,$D2,$D2
		.BYTE	$D2,$92,$D2
		.BYTE	$D2,$D2,$D2
		.BYTE	$92,$92,$92
		.BYTE	$92,$92,$92
		.BYTE	$92,$92,$92
		.BYTE	$92,$92,$92
		.BYTE	$92,$92,$92
		.BYTE	$12,$12,$12
		.BYTE	$D2,$D2,$D2
		.BYTE	$D2,$D2,$D2
		.BYTE	$D2,$D2,$D2
		.BYTE	$92,$D2,$D2
		.BYTE	$92,$92,$92
		.BYTE	$92,$92,$92
		.BYTE	$12,$92,$92
		.BYTE	$12,$92,$92
		.BYTE	$12,$12,$12
		.BYTE	$12,$12,$12
		.BYTE	$12,$12,$12
		.BYTE	$52,$12,$12
		.BYTE	$92,$92,$92
		.BYTE	$92,$92,$92
		.BYTE	$12,$92,$92
		.BYTE	$D2,$52,$52
		.BYTE	$D2,$52,$52
		.BYTE	$D2,$D2,$D2
		.BYTE	$D2,$D2,$D2
		.BYTE	$52,$52,$52
		.BYTE	$52,$52,$D2
		.BYTE	$12,$12,$12
		.BYTE	$12,$12,$12
		.BYTE	$52,$52,$52
		.BYTE	$D2,$12,$D2
		.BYTE	$D2,$12,$D2
		.BYTE	$12,$12,$D2
		.BYTE	$D2,$92,$92
		.BYTE	$D2,$92,$92
		.BYTE	$D2,$D2,$D2
		.BYTE	$D2,$D2,$D2
		.BYTE	$92,$92,$92
		.BYTE	$D2,$92,$D2
		.BYTE	$12,$12,$12
		.BYTE	$12,$12,$12
		.BYTE	$12,$12,$12

		SECTION_END 301

; ---------------------------------------------------------------------------
; ----------------G-A-M-E--E-V-E-N-T-S--A-N-I-M--D-A-T-A---------------------
; ---------------------------------------------------------------------------

		SECTION_START 302

_anim_seq_cmd_list:
		.BYTE	$FB,$FB,$FC,$FB,$FB,$FB,$FC,$FB
		.BYTE	$FC,$FB,$FB,$FC,$FB,$FC,$FC,$FC
		.BYTE	$FC,$FC,$FC,$FC,$FC,$FB,$FC,$FC
		.BYTE	$FC,$FC,$FC,$FC,$FC,$FC,$FB,$FB
		.BYTE	$FB,$FB,$FB,$FB

; NOTE: this is an evidence that there should be 40 messages, 4 more than current 36
; and there are 4 unused animation sequences for those messages still here
;
;		.BYTE	$FB,$FB,$FB,$FB					; UNUSED, 36 msgs, here 40 entries

_anim_seq_obj_count_list:
		.BYTE	$01,$01,$01,$01,$01,$01,$02,$01
		.BYTE	$02,$02,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01

;		.BYTE	$01,$01,$01,$01					; UNUSED, 36 msgs, here 40 entries

_anim_seq_lib_lo:
		.BYTE	<[_anim_seq00]	; Good Job
		.BYTE	<[_anim_seq00]	; Become Town
		.BYTE	<[_anim_seq02]	; Become City
		.BYTE	<[_anim_seq03]	; Become Capital
		.BYTE	<[_anim_seq03]	; Become Metropolis
		.BYTE	<[_anim_seq03]	; Become Megapolis
		.BYTE	<[_anim_seq06]	; Total Jam
		.BYTE	<[_anim_seq07]	; Traffic Jam
		.BYTE	<[_anim_seq08]	; Crime Rate
		.BYTE	<[_anim_seq09]	; Monster Attack
		.BYTE	<[_anim_seq0A]	; Earthquake
		.BYTE	<[_anim_seq02]	; Bank establish
		.BYTE	<[_anim_seq07]	; Relationship
		.BYTE	<[_anim_seq02]	; Amusement Park
		.BYTE	<[_anim_seq02]	; Zoo
		.BYTE	<[_anim_seq02]	; Casino
		.BYTE	<[_anim_seq02]	; Extra Land
		.BYTE	<[_anim_seq02]	; New Police
		.BYTE	<[_anim_seq02]	; New Fire
		.BYTE	<[_anim_seq02]	; 10th Monument
		.BYTE	<[_anim_seq02]	; 100th Monument
		.BYTE	<[_anim_seq00]	; Expo
		.BYTE	<[_anim_seq02]	; Expo Monument
		.BYTE	<[_anim_seq02]	; Liberty
		.BYTE	<[_anim_seq02]	; Library
		.BYTE	<[_anim_seq02]	; Park
		.BYTE	<[_anim_seq02]	; Railroad
		.BYTE	<[_anim_seq1B]	; Practice Intro
		.BYTE	<[_anim_seq1C]	; Practice Hint
		.BYTE	<[_anim_seq1B]	; Practice Success
		.BYTE	<[_anim_seq0A]	; Practice Failed
		.BYTE	<[_anim_seq1C]	; Bank Hint
		.BYTE	<[_anim_seq0A]	; Fire Dept Hint
		.BYTE	<[_anim_seq0A]	; Flood
		.BYTE	<[_anim_seq0A]	; Airplane crash
		.BYTE	<[_anim_seq0A]	; Tornado
_anim_seq_lib_hi:
		.BYTE	>[_anim_seq00]
		.BYTE	>[_anim_seq00]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq03]
		.BYTE	>[_anim_seq03]
		.BYTE	>[_anim_seq03]
		.BYTE	>[_anim_seq06]
		.BYTE	>[_anim_seq07]
		.BYTE	>[_anim_seq08]
		.BYTE	>[_anim_seq09]
		.BYTE	>[_anim_seq0A]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq07]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq00]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq02]
		.BYTE	>[_anim_seq1B]
		.BYTE	>[_anim_seq1C]
		.BYTE	>[_anim_seq1B]
		.BYTE	>[_anim_seq0A]
		.BYTE	>[_anim_seq1C]
		.BYTE	>[_anim_seq0A]
		.BYTE	>[_anim_seq0A]
		.BYTE	>[_anim_seq0A]
		.BYTE	>[_anim_seq0A]

_anim_seq00:
		.BYTE	$03
_anim_seq03:
		.BYTE	$04
_anim_seq06:
		.BYTE	$02,$05
_anim_seq07:
		.BYTE	$00
_anim_seq08:
		.BYTE	$0E,$0F
_anim_seq09:
		.BYTE	$0C,$0D
_anim_seq0A:
		.BYTE	$01
_anim_seq02:
		.BYTE	$10
_anim_seq1B:
		.BYTE	$11
_anim_seq1C:
		.BYTE	$12
_anim_pos_col_list:
		.BYTE	$20,$20,$9C,$20,$20,$1F,$C8,$A0,$9C,$9C,$D4,$D4,$20,$20,$20,$20
		.BYTE	$20,$20,$20
_anim_pos_row_list:
		.BYTE	$B8,$B8,$B8,$B8,$B8,$C0,$B8,$B8,$B8,$B8,$B8,$B8,$B8,$B8,$B8,$B8
		.BYTE	$B8,$B8,$B8
_anim_spr_lib_lo:
		.BYTE	<[_anim_spr00]	; Default animation
		.BYTE	<[_anim_spr01]	; Alert animations
		.BYTE	<[_anim_spr02]	; Traffic Jam animation A
		.BYTE	<[_anim_spr03]	; Generic animation
		.BYTE	<[_anim_spr04]	; Congratulate animation
		.BYTE	<[_anim_spr05]	; Traffic Jam animation B
		.BYTE	<[_anim_spr06]	; Ramming to the door animation A *unused*
		.BYTE	<[_anim_spr07]	; Ramming to the door animation B *unused*
		.BYTE	<[_anim_spr08]	; Need Medic animation *unused*
		.BYTE	<[_anim_spr09]	; Back to the viewer animation *unused*
		.BYTE	<[_anim_spr0A]	; Pollution animation A *unused*
		.BYTE	<[_anim_spr0B]	; Pollution animation B *unused*
		.BYTE	<[_anim_spr0C]	; Monster Attack animation A
		.BYTE	<[_anim_spr0D]	; Monster Attack animation B
		.BYTE	<[_anim_spr0E]	; Crime Rate animation A
		.BYTE	<[_anim_spr0F]	; Crime Rate animation B
		.BYTE	<[_anim_spr10]	; Present animation
		.BYTE	<[_anim_spr11]	; Practice Congrats animation
		.BYTE	<[_anim_spr12]	; Practice Info animation
_anim_spr_lib_hi:
		.BYTE	>[_anim_spr00]
		.BYTE	>[_anim_spr01]
		.BYTE	>[_anim_spr02]
		.BYTE	>[_anim_spr03]
		.BYTE	>[_anim_spr04]
		.BYTE	>[_anim_spr05]
		.BYTE	>[_anim_spr06]
		.BYTE	>[_anim_spr07]
		.BYTE	>[_anim_spr08]
		.BYTE	>[_anim_spr09]
		.BYTE	>[_anim_spr0A]
		.BYTE	>[_anim_spr0B]
		.BYTE	>[_anim_spr0C]
		.BYTE	>[_anim_spr0D]
		.BYTE	>[_anim_spr0E]
		.BYTE	>[_anim_spr0F]
		.BYTE	>[_anim_spr10]
		.BYTE	>[_anim_spr11]
		.BYTE	>[_anim_spr12]
_anim_data_lib_lo:
		.BYTE	<[_anim_data0_00]
		.BYTE	<[_anim_data0_01]
		.BYTE	<[_anim_data0_02]
		.BYTE	<[_anim_data0_03]
		.BYTE	<[_anim_data0_04]
		.BYTE	<[_anim_data0_05]
		.BYTE	<[_anim_data0_06]
		.BYTE	<[_anim_data0_07]
		.BYTE	<[_anim_data0_08]
		.BYTE	<[_anim_data0_09]
		.BYTE	<[_anim_data0_0A]
		.BYTE	<[_anim_data0_0B]
		.BYTE	<[_anim_data0_0C]
		.BYTE	<[_anim_data0_0D]
		.BYTE	<[_anim_data0_0E]
		.BYTE	<[_anim_data0_0F]
		.BYTE	<[_anim_data0_10]
		.BYTE	<[_anim_data0_11]
		.BYTE	<[_anim_data0_12]
_anim_data_lib_hi:
		.BYTE	>[_anim_data0_00]
		.BYTE	>[_anim_data0_01]
		.BYTE	>[_anim_data0_02]
		.BYTE	>[_anim_data0_03]
		.BYTE	>[_anim_data0_04]
		.BYTE	>[_anim_data0_05]
		.BYTE	>[_anim_data0_06]
		.BYTE	>[_anim_data0_07]
		.BYTE	>[_anim_data0_08]
		.BYTE	>[_anim_data0_09]
		.BYTE	>[_anim_data0_0A]
		.BYTE	>[_anim_data0_0B]
		.BYTE	>[_anim_data0_0C]
		.BYTE	>[_anim_data0_0D]
		.BYTE	>[_anim_data0_0E]
		.BYTE	>[_anim_data0_0F]
		.BYTE	>[_anim_data0_10]
		.BYTE	>[_anim_data0_11]
		.BYTE	>[_anim_data0_12]
_anim_dataA_lib_lo:
		.BYTE	<[_anim_dataA_00]
		.BYTE	<[_anim_dataA_01]
		.BYTE	<[_anim_dataA_02]
		.BYTE	<[_anim_dataA_03]
		.BYTE	<[_anim_dataA_04]
		.BYTE	<[_anim_dataA_05]
		.BYTE	<[_anim_dataA_06]
		.BYTE	<[_anim_dataA_07]
		.BYTE	<[_anim_dataA_08]
		.BYTE	<[_anim_dataA_09]
		.BYTE	<[_anim_dataA_0A]
		.BYTE	<[_anim_dataA_0B]
		.BYTE	<[_anim_dataA_0C]
		.BYTE	<[_anim_dataA_0D]
		.BYTE	<[_anim_dataA_0E]
		.BYTE	<[_anim_dataA_0F]
		.BYTE	<[_anim_dataA_10]
		.BYTE	<[_anim_dataA_11]
		.BYTE	<[_anim_dataA_12]
_anim_dataA_lib_hi:
		.BYTE	>[_anim_dataA_00]
		.BYTE	>[_anim_dataA_01]
		.BYTE	>[_anim_dataA_02]
		.BYTE	>[_anim_dataA_03]
		.BYTE	>[_anim_dataA_04]
		.BYTE	>[_anim_dataA_05]
		.BYTE	>[_anim_dataA_06]
		.BYTE	>[_anim_dataA_07]
		.BYTE	>[_anim_dataA_08]
		.BYTE	>[_anim_dataA_09]
		.BYTE	>[_anim_dataA_0A]
		.BYTE	>[_anim_dataA_0B]
		.BYTE	>[_anim_dataA_0C]
		.BYTE	>[_anim_dataA_0D]
		.BYTE	>[_anim_dataA_0E]
		.BYTE	>[_anim_dataA_0F]
		.BYTE	>[_anim_dataA_10]
		.BYTE	>[_anim_dataA_11]
		.BYTE	>[_anim_dataA_12]
_anim_dataB_lib_lo:
		.BYTE	<[_anim_dataB_00]
		.BYTE	<[_anim_dataB_01]
		.BYTE	<[_anim_dataB_02]
		.BYTE	<[_anim_dataB_03]
		.BYTE	<[_anim_dataB_04]
		.BYTE	<[_anim_dataB_05]
		.BYTE	<[_anim_dataB_06]
		.BYTE	<[_anim_dataB_07]
		.BYTE	<[_anim_dataB_08]
		.BYTE	<[_anim_dataB_09]
		.BYTE	<[_anim_dataB_0A]
		.BYTE	<[_anim_dataB_0B]
		.BYTE	<[_anim_dataB_0C]
		.BYTE	<[_anim_dataB_0D]
		.BYTE	<[_anim_dataB_0E]
		.BYTE	<[_anim_dataB_0F]
		.BYTE	<[_anim_dataB_10]
		.BYTE	<[_anim_dataB_11]
		.BYTE	<[_anim_dataB_12]
_anim_dataB_lib_hi:
		.BYTE	>[_anim_dataB_00]
		.BYTE	>[_anim_dataB_01]
		.BYTE	>[_anim_dataB_02]
		.BYTE	>[_anim_dataB_03]
		.BYTE	>[_anim_dataB_04]
		.BYTE	>[_anim_dataB_05]
		.BYTE	>[_anim_dataB_06]
		.BYTE	>[_anim_dataB_07]
		.BYTE	>[_anim_dataB_08]
		.BYTE	>[_anim_dataB_09]
		.BYTE	>[_anim_dataB_0A]
		.BYTE	>[_anim_dataB_0B]
		.BYTE	>[_anim_dataB_0C]
		.BYTE	>[_anim_dataB_0D]
		.BYTE	>[_anim_dataB_0E]
		.BYTE	>[_anim_dataB_0F]
		.BYTE	>[_anim_dataB_10]
		.BYTE	>[_anim_dataB_11]
		.BYTE	>[_anim_dataB_12]

_anim_spr00:
		.BYTE	$26,$10,$11,$29,$11,$10,$00,$00,$00,$2A,$12,$13,$14,$13,$14,$00
		.BYTE	$00,$00,$15,$13,$14,$13,$14,$13,$14,$13,$14,$12,$16,$13,$14,$13
		.BYTE	$14,$13,$14,$13,$14,$18,$19,$18,$19,$18,$19,$18,$19,$14,$15,$17
		.BYTE	$15,$12,$15,$17,$15,$12,$00,$00,$00,$26,$10,$11,$29,$11,$10
_anim_data0_00:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$01,$10,$10,$08,$08,$0C,$0C,$FF
		.BYTE	$06,$03,$10,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$40,$80,$08,$08,$0C
		.BYTE	$0C,$08,$08,$0C,$0C,$10,$10,$10,$10,$10,$10,$10,$10,$20,$04,$0C
		.BYTE	$04,$08,$04,$0C,$04,$08,$FF,$26,$01,$06,$04,$04,$06,$04,$04,$FF
		.BYTE	$08,$01,$00
_anim_dataA_00:
		.BYTE	$0E,$0F,$00,$00,$00,$00,$00,$00,$00,$0E
_anim_dataB_00:
		.BYTE	$38,$10,$B0,$F0,$F0,$68,$FF,$05,$01,$38,$00

_anim_spr01:
		.BYTE	$26,$10,$11,$29,$11,$10,$00,$00,$00,$2A,$18,$19,$18,$19,$00,$00
		.BYTE	$00,$1A,$14,$13,$14,$13,$14,$13,$14,$13,$16,$15,$14,$15,$16,$18
		.BYTE	$19,$18,$19,$18,$19,$18,$19,$16,$15,$17,$16,$15,$17,$16,$18,$19
		.BYTE	$18,$19,$18,$19,$18,$19,$00,$00,$00,$26,$10,$11,$29,$11,$10
_anim_data0_01:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$01,$10,$08,$08,$0C,$0C,$FF,$06
		.BYTE	$01,$40,$10,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$10,$10,$10,$10,$08
		.BYTE	$08,$0C,$0C,$08,$08,$0C,$0C,$08,$04,$10,$08,$04,$10,$40,$08,$08
		.BYTE	$0C,$0C,$08,$08,$0C,$0C,$FF,$25,$01,$06,$04,$04,$06,$04,$04,$FF
		.BYTE	$08,$01,$00
_anim_dataA_01:
		.BYTE	$0E,$0F,$00,$00,$00,$00,$00,$00,$0E
_anim_dataB_01:
		.BYTE	$38,$10,$A0,$F0,$B8,$FF,$04,$01,$38,$00

_anim_spr02:
		.BYTE	$1B,$1E,$1B,$1E,$1B,$1E,$1B,$1C,$1F,$20,$1F,$20,$1B,$1E,$1B,$1E
		.BYTE	$1B,$1E,$1B,$1C,$1B,$1B,$1E,$1B,$1E,$1B,$1E,$1B,$1C,$1F,$20,$1F
		.BYTE	$20,$00,$00,$00,$1B,$1E,$1B,$1E,$1B,$1E,$1B,$1C,$21,$15,$21,$15
		.BYTE	$21,$21,$22,$15,$1D,$2E,$23,$2E,$1D,$15,$24,$12,$1C,$1B,$1E,$1B
		.BYTE	$1E,$00,$00,$00,$12,$00,$26,$27,$28,$29,$28,$27
_anim_data0_02:
		.BYTE	$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$08,$08,$08,$08,$0C,$0C
		.BYTE	$08,$08,$0C,$0C,$40,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C
		.BYTE	$0C,$FF,$06,$01,$08,$08,$08,$08,$08,$08,$08,$08,$40,$0C,$0C,$08
		.BYTE	$08,$30,$30,$04,$04,$04,$40,$04,$04,$04,$30,$40,$20,$08,$08,$0C
		.BYTE	$0C,$FF,$06,$03,$40,$FD,$06,$04,$04,$06,$04,$04,$FF,$08,$01,$00
_anim_dataA_02:
		.BYTE	$00,$00,$00,$F0,$E0,$F0,$E0,$F0,$E0,$F0,$00,$F0,$00,$F0,$00,$F0
		.BYTE	$00,$F0,$00,$F0,$00,$10,$00,$10,$00,$10,$00,$10,$00,$10,$00,$10
		.BYTE	$20,$10,$20,$10,$20,$10,$00,$00,$0E
_anim_dataB_02:
		.BYTE	$F0,$F0,$D4,$01,$01,$02,$01,$03,$01,$04,$01,$03,$01,$02,$01,$01
		.BYTE	$01,$01,$01,$01,$0C,$01,$01,$01,$01,$01,$01,$02,$01,$03,$01,$04
		.BYTE	$01,$03,$01,$02,$01,$01,$F0,$8C,$38,$00

_anim_spr03:
		.BYTE	$26,$27,$28,$29,$28,$27,$00,$00,$00,$2A,$14,$13,$14,$13,$14,$13
		.BYTE	$14,$13,$15,$2B,$2E,$15,$2B,$2E,$15,$2B,$2E,$15,$2B,$2E,$14,$13
		.BYTE	$15,$14,$13,$15,$14,$13,$14,$13,$14,$13,$14,$13,$14,$00,$00,$00
		.BYTE	$26,$27,$28,$29,$28,$27,$00,$00,$00
_anim_data0_03:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$01,$10,$08,$08,$08,$08,$08,$08
		.BYTE	$08,$08,$08,$04,$14,$08,$04,$14,$08,$04,$14,$08,$04,$14,$18,$08
		.BYTE	$10,$18,$08,$10,$08,$08,$08,$08,$08,$08,$08,$08,$40,$FF,$25,$02
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$01,$00
_anim_dataA_03:
		.BYTE	$0E,$0F,$00,$00,$00,$00,$00,$00,$0E
_anim_dataB_03:
		.BYTE	$38,$10,$F0,$F0,$F0,$F0,$F0,$30,$38,$00

_anim_spr04:
		.BYTE	$26,$27,$28,$29,$28,$27,$26,$27,$28,$29,$28,$27,$26,$27,$28,$29
		.BYTE	$28,$27,$2A,$12,$15,$2B,$2E,$30,$12,$15,$2B,$2E,$30,$48,$49,$4A
		.BYTE	$4B,$4A,$49,$48,$49,$4A,$64,$12,$15,$2B,$2E,$30,$12,$15,$2B,$2E
		.BYTE	$30,$12,$14,$13,$14,$13,$14,$13,$14,$13,$15,$2B,$2E,$15,$2B,$2E
		.BYTE	$15,$2B,$2E,$14,$13,$14,$13,$14,$13,$14,$13,$14,$13,$12,$00,$00
		.BYTE	$00,$26,$27,$28,$29,$28,$27,$00,$00,$00,$26,$27,$28,$29
_anim_data0_04:
		.BYTE	$06,$04,$04,$06,$04,$04,$06,$04,$04,$06,$04,$04,$06,$04,$04,$06
		.BYTE	$04,$04,$10,$08,$04,$04,$04,$10,$08,$04,$04,$04,$10,$06,$04,$04
		.BYTE	$06,$04,$04,$06,$04,$04,$10,$08,$04,$04
		.BYTE	4
		.BYTE	$10,$08,$04,$04,$04,$10,$08,$08,$08,$08,$08,$08,$08,$08,$08,$08
		.BYTE	$04,$14,$08,$04,$14,$08,$04,$14,$08,$08,$08,$08,$08,$08,$08,$08
		.BYTE	$08,$08,$40,$FF,$47,$01,$06,$04,$04,$06,$04,$04,$FF,$08,$01,$06
		.BYTE	$04,$04,$06,$00
_anim_dataA_04:
		.BYTE	$0E,$0E,$0F,$00,$02,$01,$00,$00,$00,$00,$00,$0E
_anim_dataB_04:
		.BYTE	$2A,$2A,$10,$48,$2A,$10,$F0,$90,$FF,$09,$01,$4C,$00

_anim_spr05:
		.BYTE	$25,$25,$25,$25
_anim_data0_05:
		.BYTE	$F0,$F0,$D4,$3C,$00
_anim_dataA_05:
		.BYTE	$00,$00,$00,$04
_anim_dataB_05:
		.BYTE	$F0,$F0,$D4,$3C,$00

_anim_spr06:
		.BYTE	$31,$13,$14,$13,$14,$13,$14,$13,$1C,$00,$00,$00,$31,$32,$31,$32
		.BYTE	$13,$14,$13,$14,$13,$14,$13,$1C,$00,$00,$00,$31,$32,$31,$32,$31
		.BYTE	$32,$31,$32,$33,$34,$33,$34,$33,$34,$33,$34,$13,$14,$13,$1C,$13
		.BYTE	$14,$13,$1C,$13,$14,$13,$1C,$13,$14,$13,$1C,$13,$14,$13,$1C,$13
		.BYTE	$14,$13,$1C,$37,$32,$37,$32,$37,$32,$37,$32
_anim_data0_06:
		.BYTE	$20,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$FF,$0A,$01,$08,$30,$10,$10
		.BYTE	$08,$08,$0C,$0C,$08,$08,$0C,$0C,$FF,$0A,$01,$20,$20,$10,$10,$10
		.BYTE	$10,$10,$10,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08
		.BYTE	$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08
		.BYTE	$08,$0C,$0C,$40,$10,$10,$10,$10,$10,$10,$10,$FE,$32,$00
_anim_dataA_06:
		.BYTE	$00
_anim_dataB_06:
		.BYTE	$F0,$FE,$03,$00

_anim_spr07:
		.BYTE	$38,$38,$39,$38,$39,$38,$3A,$38,$3A,$3B,$38,$3A,$3B,$38,$3A,$3B
		.BYTE	$38,$3A,$3B,$38,$3A,$3B
_anim_data0_07:
		.BYTE	$40,$20,$20,$10,$10,$10,$10,$10,$10,$10,$08,$08,$08,$08,$08,$08
		.BYTE	$08,$08,$08,$08,$08,$08,$FE,$05,$00
_anim_dataA_07:
		.BYTE	$00
_anim_dataB_07:
		.BYTE	$F0,$FE,$03,$00

_anim_spr08:
		.BYTE	$3C,$3D,$3C,$3D,$3C,$3D,$3C,$3D
_anim_data0_08:
		.BYTE	$10,$10,$10,$10,$10,$10,$10,$10,$00
_anim_dataA_08:
		.BYTE	$00
_anim_dataB_08:
		.BYTE	$80,$00

_anim_spr09:
		.BYTE	$3E,$3F,$3E,$3F,$3E,$3F,$3E,$41,$3E,$3F,$3E,$3F,$3E,$3F,$3E,$41
		.BYTE	$3E,$3F,$3E,$3F,$3E,$3F,$3E,$3F,$3E,$3F,$3E,$3F,$3E,$3F,$3E,$3F
		.BYTE	$3E,$40,$3E,$41,$3E,$3E,$3F,$3E,$3F,$3E,$3F,$3E,$41,$3E,$3F,$3E
		.BYTE	$3F,$3E,$3F,$3E,$41,$3E,$40,$3E,$41,$3E
_anim_data0_09:
		.BYTE	$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C
		.BYTE	$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C
		.BYTE	$40,$30,$0C,$A0,$60,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C
		.BYTE	$0C,$08,$08,$0C,$0C,$40,$30,$0C,$A0,$60,$FE,$3C
_anim_dataA_09:
		.BYTE	$00
_anim_dataB_09:
		.BYTE	$F0,$FE,$03

_anim_spr0A:
		.BYTE	$46,$47,$4C,$4D,$46,$47,$4C,$4D,$47,$56,$57,$56,$57,$56,$57,$56
		.BYTE	$57,$58,$42,$43,$44,$45,$44,$43,$42,$43,$44,$45,$44,$43,$4E,$4F
		.BYTE	$50,$51,$4F,$50,$51,$4F,$50,$4F,$50,$46,$47,$4C,$4D,$46,$47,$4C
		.BYTE	$4D,$59,$48,$49,$4A,$4B,$4A,$49,$48,$49,$4A,$4B,$4A,$49,$55,$4F
		.BYTE	$50,$54,$4F,$50,$54,$4F,$50,$4F,$50
_anim_data0_0A:
		.BYTE	$10,$10,$10,$10,$10,$10,$10,$10,$30,$08,$08,$08,$08,$08,$08,$08
		.BYTE	$08,$10,$06,$04,$04,$06,$04,$04,$06,$04,$04,$06,$04,$04,$10,$0C
		.BYTE	$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$10,$10,$10,$10,$10,$10,$10
		.BYTE	$10,$10,$06,$04,$04,$06,$04,$04,$06,$04,$04,$06,$04,$04,$10,$0C
		.BYTE	$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$0C,$FE,$4B
_anim_dataA_0A:
		.BYTE	$00,$00,$0E,$00,$00,$02,$00
_anim_dataB_0A:
		.BYTE	$80,$80,$38,$80,$98,$38,$88,$FE,$09,$00

_anim_spr0B:
		.BYTE	$52,$53
_anim_data0_0B:
		.BYTE	$10,$10,$FE,$04,$00
_anim_dataA_0B:
		.BYTE	$00,$00,$0F,$00,$01,$00
_anim_dataB_0B:
		.BYTE	$80,$88,$70,$E0,$70,$48,$FE,$08,$00

_anim_spr0C:
		.BYTE	$5A,$5B,$5C,$5D,$5C,$5B,$00,$00,$00,$5E,$5F,$5E,$5F,$60,$61,$62
		.BYTE	$63,$64,$65,$66,$67,$66,$67,$66,$67,$66,$67,$00,$00,$00,$6E,$65
		.BYTE	$6E,$65,$68,$68,$6D,$00,$00,$00,$5A,$5B,$5C,$5D,$5C,$5B
_anim_data0_0C:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$02,$04,$04,$04,$04,$06,$04,$04
		.BYTE	$06,$10,$1A,$08,$08,$0A,$0A,$08,$08,$0A,$5A,$FF,$0A,$03,$0A,$0A
		.BYTE	$06,$06,$3A,$04,$04,$FF,$04,$07,$06,$04,$04,$06,$04,$04,$FF,$08
		.BYTE	$02,$00
_anim_dataA_0C:
		.BYTE	$0E,$0F,$02,$01,$00,$00,$00,$00,$0E,$00
_anim_dataB_0C:
		.BYTE	$54,$10,$14,$2A,$F0,$F0,$30,$EA,$49,$0B,$00

_anim_spr0D:
		.BYTE	$69,$69,$69,$69,$69,$69,$6A,$00,$00,$00,$6B,$6C,$00,$00,$00,$71
		.BYTE	$72,$00,$00,$00,$75,$76,$00,$00,$00,$6F,$70,$00,$00,$00,$73,$74
		.BYTE	$00,$00,$00,$6B,$6C,$00,$00,$00
_anim_data0_0D:
		.BYTE	$E0,$E4,$F0,$F0,$30,$02,$02,$FF,$04,$0A,$02,$02,$FF,$04,$05,$02
		.BYTE	$02,$FF,$04,$05,$02,$02,$FF,$04,$04,$02,$02,$FF,$04,$04,$02,$02
		.BYTE	$FF,$04,$04,$02,$02,$FF,$04,$15,$00
_anim_dataA_0D:
		.BYTE	$00,$00,$00,$00,$00,$0E,$02,$0E
_anim_dataB_0D:
		.BYTE	$E0,$E4,$F0,$F0,$30,$5C,$3C,$54,$00

_anim_spr0E:
		.BYTE	$77,$78,$79,$7A,$79,$78,$00,$00,$00,$77,$78,$79,$7A,$7D,$7D,$7E
		.BYTE	$7F,$80,$7F,$7E,$00,$00,$00,$7D,$7E,$7F,$80
_anim_data0_0E:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$03,$06,$04,$04,$06,$3C,$06,$04
		.BYTE	$04,$06,$04,$04,$FF,$08,$03,$06,$04,$04,$06,$00
_anim_dataA_0E:
		.BYTE	$0E,$00,$02
_anim_dataB_0E:
		.BYTE	$84,$3C,$84,$00

_anim_spr0F:
		.BYTE	$81,$81,$82,$83,$84,$83,$82,$81,$82,$83,$00,$84,$83,$82,$81,$82
		.BYTE	$83,$00,$00,$00,$84,$85,$85,$86,$87,$88,$87,$86,$00,$85,$86,$87
		.BYTE	$88,$87,$86,$00,$00,$00,$85,$86,$87,$88
_anim_data0_0F:
		.BYTE	$3C,$06,$04,$04,$06,$04,$04,$06,$04,$04,$FD,$06,$04,$04,$06,$04
		.BYTE	$04,$FF,$08,$02,$06,$3C,$06,$04,$04,$06,$04,$04,$FC,$06,$04,$04
		.BYTE	$06,$04,$04,$FF,$08,$02,$06,$04,$04,$06,$00
_anim_dataA_0F:
		.BYTE	$00,$0E,$00,$02
_anim_dataB_0F:
		.BYTE	$3C,$84,$3C,$84,$00

_anim_spr10:
		.BYTE	$26,$27,$28,$29,$28,$27,$26,$27,$28,$2A,$14,$2E,$2F,$14,$13,$14
		.BYTE	$13,$14,$15,$2F,$15,$14,$13,$14,$13,$14,$12,$13,$1F,$20,$1F,$20
		.BYTE	$00,$00,$00,$15,$13,$14,$13,$14,$00,$00,$00,$14,$12,$48,$49,$4A
		.BYTE	$4B,$4A,$49,$FF,$06,$01
_anim_data0_10:
		.BYTE	$06,$04,$04,$06,$04,$04,$06,$04,$04,$10,$20,$40,$08,$30,$08,$08
		.BYTE	$0C,$0C,$08,$10,$08,$08,$08,$08,$0C,$0C,$40,$08,$08,$08,$0C,$0C
		.BYTE	$FF,$06,$02,$06,$08,$08,$0C,$0C,$FF,$06,$01,$10,$30,$06,$04,$04
		.BYTE	$06,$04,$04,$FF,$06,$01,$00,$00
_anim_dataA_10:
		.BYTE	$0E,$0F,$00,$00,$00,$02
_anim_dataB_10:
		.BYTE	$2A,$10,$E0,$E0,$A6,$38,$00

_anim_spr11:
		.BYTE	$26,$27,$28,$29,$28,$27,$00,$00,$00,$26,$27,$28,$2A,$14,$2E,$2F
		.BYTE	$14,$13,$14,$13,$14,$00,$00,$00,$15,$2F,$15,$14,$13,$14,$13,$14
		.BYTE	$00,$00,$00,$12,$48,$49,$4A,$4B,$4A,$49,$00,$00,$00,$48,$49,$4A
_anim_data0_11:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$01,$06,$04,$04,$10,$20,$40,$08
		.BYTE	$30,$08,$08,$0C,$0C,$FF,$06,$03,$08,$10,$08,$08,$08,$08,$0C,$0C
		.BYTE	$FF,$06,$02,$40,$06,$04,$04,$06,$04,$04,$FF,$08,$01,$06,$04,$04
		.BYTE	$00
_anim_dataA_11:
		.BYTE	$0E,$0F,$00,$00,$00,$02
_anim_dataB_11:
		.BYTE	$46,$10,$F0,$F0,$38,$46,$00

_anim_spr12:
		.BYTE	$26,$27,$28,$29,$28,$27,$00,$00,$00,$26,$27,$28,$2A,$14,$13,$14
		.BYTE	$13,$14,$13,$14,$13,$14,$13,$14,$13,$14,$13,$14,$13,$14,$15,$2F
		.BYTE	$15,$14,$13,$14,$13,$14,$13,$14,$13,$14,$13,$14,$13,$14,$13,$14
		.BYTE	$13,$14,$15,$2F,$15,$14,$15,$2F,$15,$14,$13,$14,$13,$14,$13,$14
		.BYTE	$13,$14,$13,$14,$13,$14,$13,$14,$13,$14,$12,$00,$00,$48,$49,$4A
		.BYTE	$4B,$4A,$49,$00,$00,$00,$48,$49,$4A
_anim_data0_12:
		.BYTE	$06,$04,$04,$06,$04,$04,$FF,$08,$01,$06,$04,$04,$10,$50,$08,$08
		.BYTE	$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$10
		.BYTE	$08,$08,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$08,$08
		.BYTE	$0C,$0C,$08,$10,$08,$08,$08,$10,$08,$08,$08,$08,$0C,$0C,$08,$08
		.BYTE	$0C,$0C,$08,$08,$0C,$0C,$08,$08,$0C,$0C,$40,$FE,$40,$06,$04,$04
		.BYTE	$06,$04,$04,$FF,$08,$01,$06,$04,$04,$00
_anim_dataA_12:
		.BYTE	$0E,$0F,$00,$00,$00,$00
_anim_dataB_12:
		.BYTE	$46,$10,$E0,$E0,$E0,$48,$FE,$06,$00

		SECTION_ENDS 302, "PRG3 EVENTS ANIM DATA"

; ---------------------------------------------------------------------------
; ------------G-A-M-E--E-V-E-N-T-S--M-E-S-S-A-G-E-S--D-A-T-A-----------------
; ---------------------------------------------------------------------------

		SECTION_START 303

_game_msg_screens_count_list:
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$03,$02,$01,$01,$01
		.BYTE	$01,$01,$01,$01

; OPTIMIZED
_game_msg_lines_cnt_lib:
		.BYTE	[_game_msg_lines6-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines3-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines4-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines6-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines3-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines6-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines6-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines4-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines8-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines3-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines7-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines4-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines4-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines4-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines7-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines3-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines3-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines989-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines99-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines9-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines9-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines2-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines6-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines5-_game_msg_lines_cnt_lib]
		.BYTE	[_game_msg_lines7-_game_msg_lines_cnt_lib]

; REDUNDANT
;_game_msg_lines_cnt_lib_lo:
;		.BYTE	<[_game_msg_lines6]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines3]
;		.BYTE	<[_game_msg_lines4]
;		.BYTE	<[_game_msg_lines6]
;		.BYTE	<[_game_msg_lines3]
;		.BYTE	<[_game_msg_lines6]
;		.BYTE	<[_game_msg_lines6]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines4]
;		.BYTE	<[_game_msg_lines8]
;		.BYTE	<[_game_msg_lines3]
;		.BYTE	<[_game_msg_lines7]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines4]
;		.BYTE	<[_game_msg_lines4]
;		.BYTE	<[_game_msg_lines4]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines7]
;		.BYTE	<[_game_msg_lines3]
;		.BYTE	<[_game_msg_lines3]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines989]
;		.BYTE	<[_game_msg_lines99]
;		.BYTE	<[_game_msg_lines9]
;		.BYTE	<[_game_msg_lines9]
;		.BYTE	<[_game_msg_lines2]
;		.BYTE	<[_game_msg_lines6]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines5]
;		.BYTE	<[_game_msg_lines7]
;_game_msg_lines_cnt_lib_hi:
;		.BYTE	>[_game_msg_lines6]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines3]
;		.BYTE	>[_game_msg_lines4]
;		.BYTE	>[_game_msg_lines6]
;		.BYTE	>[_game_msg_lines3]
;		.BYTE	>[_game_msg_lines6]
;		.BYTE	>[_game_msg_lines6]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines4]
;		.BYTE	>[_game_msg_lines8]
;		.BYTE	>[_game_msg_lines3]
;		.BYTE	>[_game_msg_lines7]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines4]
;		.BYTE	>[_game_msg_lines4]
;		.BYTE	>[_game_msg_lines4]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines7]
;		.BYTE	>[_game_msg_lines3]
;		.BYTE	>[_game_msg_lines3]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines989]
;		.BYTE	>[_game_msg_lines99]
;		.BYTE	>[_game_msg_lines9]
;		.BYTE	>[_game_msg_lines9]
;		.BYTE	>[_game_msg_lines2]
;		.BYTE	>[_game_msg_lines6]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines5]
;		.BYTE	>[_game_msg_lines7]

_game_msg_lines2:
		.BYTE	$02
_game_msg_lines3:
		.BYTE	$03
_game_msg_lines4:
		.BYTE	$04
_game_msg_lines5:
		.BYTE	$05
_game_msg_lines6:
		.BYTE	$06
_game_msg_lines7:
		.BYTE	$07
_game_msg_lines8:
		.BYTE	$08
_game_msg_lines9:
		.BYTE	$09
_game_msg_lines989:
		.BYTE	$09,$08,$09
_game_msg_lines99:
		.BYTE	$09,$09

_game_msg_lisb_lo:
		.BYTE	<[_msg00]
		.BYTE	<[_msg01_town]
		.BYTE	<[_msg02_city]
		.BYTE	<[_msg03_capital]
		.BYTE	<[_msg04_metropolis]
		.BYTE	<[_msg05_megapolis]
		.BYTE	<[_msg06_total_jam]
		.BYTE	<[_msg07_traffic]
		.BYTE	<[_msg08_crime]
		.BYTE	<[_msg09_monster]
		.BYTE	<[_msg0A_quake]
		.BYTE	<[_msg0B_bank]
		.BYTE	<[_msg0C_establish_relationship]
		.BYTE	<[_msg0D_amusement_park]
		.BYTE	<[_msg0E_zoo]
		.BYTE	<[_msg0F_casino]
		.BYTE	<[_msg10_extra_land]
		.BYTE	<[_msg11_new_police]
		.BYTE	<[_msg12_new_fire]
		.BYTE	<[_msg13_monument_10th_years]
		.BYTE	<[_msg14_monument_100th_years]
		.BYTE	<[_msg15_expo]
		.BYTE	<[_msg16_expo_monument]
		.BYTE	<[_msg17_liberty]
		.BYTE	<[_msg18_library]
		.BYTE	<[_msg19_parks_expand]
		.BYTE	<[_msg1A_railroad_station]
		.BYTE	<[_msg1B_practice_intro]
		.BYTE	<[_msg1C_practice_hints]
		.BYTE	<[_msg1D_practice_success]
		.BYTE	<[_msg1E_practice_fail]
		.BYTE	<[_msg1F_bank_hint]
		.BYTE	<[_msg20_fire_dept_hint]
		.BYTE	<[_msg21_flood]
		.BYTE	<[_msg22_airplane_crash]
		.BYTE	<[_msg23_tornado]
_game_msg_lisb_hi:
		.BYTE	>[_msg00]
		.BYTE	>[_msg01_town]
		.BYTE	>[_msg02_city]
		.BYTE	>[_msg03_capital]
		.BYTE	>[_msg04_metropolis]
		.BYTE	>[_msg05_megapolis]
		.BYTE	>[_msg06_total_jam]
		.BYTE	>[_msg07_traffic]
		.BYTE	>[_msg08_crime]
		.BYTE	>[_msg09_monster]
		.BYTE	>[_msg0A_quake]
		.BYTE	>[_msg0B_bank]
		.BYTE	>[_msg0C_establish_relationship]
		.BYTE	>[_msg0D_amusement_park]
		.BYTE	>[_msg0E_zoo]
		.BYTE	>[_msg0F_casino]
		.BYTE	>[_msg10_extra_land]
		.BYTE	>[_msg11_new_police]
		.BYTE	>[_msg12_new_fire]
		.BYTE	>[_msg13_monument_10th_years]
		.BYTE	>[_msg14_monument_100th_years]
		.BYTE	>[_msg15_expo]
		.BYTE	>[_msg16_expo_monument]
		.BYTE	>[_msg17_liberty]
		.BYTE	>[_msg18_library]
		.BYTE	>[_msg19_parks_expand]
		.BYTE	>[_msg1A_railroad_station]
		.BYTE	>[_msg1B_practice_intro]
		.BYTE	>[_msg1C_practice_hints]
		.BYTE	>[_msg1D_practice_success]
		.BYTE	>[_msg1E_practice_fail]
		.BYTE	>[_msg1F_bank_hint]
		.BYTE	>[_msg20_fire_dept_hint]
		.BYTE	>[_msg21_flood]
		.BYTE	>[_msg22_airplane_crash]
		.BYTE	>[_msg23_tornado]

_cur	EQU		$B0
_cmd	EQU		$40
_popl	EQU		$23
_name	EQU		$24
_

_msg00:
		.WORD	off_70EC1
off_70EC1:
		.WORD	aGoodJobMr_mayor
		.WORD	aYourFineCityIsGrow
		.WORD	aIngVeryNicely_
		.WORD	aIDonTSeeAnyProblem
		.WORD	aAreasSoKeepUpThe
		.WORD	aGoodWork
aGoodJobMr_mayor:		.BYTE	" Good job Mr.Mayor!",$F
aYourFineCityIsGrow:	.BYTE	"Your fine city is grow-"
aIngVeryNicely_:		.BYTE	"ing very nicely.",$F
aIDonTSeeAnyProblem:	.BYTE	"I don*t see any problem"
aAreasSoKeepUpThe:		.BYTE	"areas, so keep up the",$F
aGoodWork:				.BYTE	"good work!",$F

_msg01_town:
		.WORD	off_70F43
off_70F43:
		.WORD	aCongratulations
		.WORD	asc_70F5F
		.WORD	aATown_
		.WORD	aYourPopulationIs
		.WORD	byte_77F88
aCongratulations:		.BYTE	" Congratulations!",$F
asc_70F5F:				.BYTE	" ",$40,$24,$30," becomes",$F
aATown_:				.BYTE	"a town.",$F
aYourPopulationIs:		.BYTE	" Your population is",$F
byte_77F88:				.BYTE	$40,$23,$30,".",$F

_msg02_city:
		.WORD	off_70F8F
off_70F8F:
		.WORD	aCongratulations
		.WORD	asc_70F99
		.WORD	aACity_
		.WORD	aSettleInAndBuild
		.WORD	aYourOwnHouse
							;	" Congratulations!",$F
asc_70F99:				.BYTE	" ",$40,$24,$30," has become",$F
aACity_:				.BYTE	"a city.",$F
aSettleInAndBuild:		.BYTE	" Settle in and build",$F
aYourOwnHouse:			.BYTE	"your own house?",$F

_msg03_capital:
		.WORD	off_70FD8
off_70FD8:
		.WORD	aCongratulations
		.WORD	asc_70F5F
		.WORD	aACapital_
		.WORD	aYourPopulationIs
		.WORD	byte_77F88
							;	" Congratulations!",$F
							;	" ",$40,$24,$30," becomes",$F
aACapital_:				.BYTE	"a capital.",$F
							;	" Your population is",$F
							;	$40,$23,$30,".",$F

_msg04_metropolis:
		.WORD	off_70FEF
off_70FEF:
		.WORD	aCongratulations
		.WORD	asc_70F5F
		.WORD	aAMetropolis_
		.WORD	aYourPopulationIs
		.WORD	byte_77F88
							;	" Congratulations!",$F
							;	" ",$40,$24,$30," becomes",$F
aAMetropolis_:			.BYTE	"a metropolis.",$F
							;	" Your population is",$F
							;	$40,$23,$30,".",$F

_msg05_megapolis:
		.WORD	off_71009
off_71009:
		.WORD	aCongratulations
		.WORD	asc_70F5F
		.WORD	aAMegalopolis_
		.WORD	aYourPopulationIs
		.WORD	byte_77F88
							;	" Congratulations!",$F
							;	" ",$40,$24,$30," becomes",$F
aAMegalopolis_:			.BYTE	"a megalopolis.",$F
							;	" Your population is",$F
							;	$40,$23,$30,".",$F

_msg06_total_jam:
		.WORD	off_71024
off_71024:
		.WORD	aTrafficIsBumperTo
		.WORD	aBumper_ResidentsAre
		.WORD	aBecomingHostile_
aTrafficIsBumperTo:		.BYTE	" Traffic is bumper to",$F
aBumper_ResidentsAre:	.BYTE	"bumper. Residents are",$F
aBecomingHostile_:		.BYTE	"becoming hostile.",$F

_msg07_traffic:
		.WORD	off_7106A
off_7106A:
		.WORD	aCommutersAreGrid
		.WORD	aLocked_MassTransitIn
		.WORD	aCongestedAreasWould
		.WORD	aEaseTheTension_
aCommutersAreGrid:		.BYTE	" Commuters are grid-",$F
aLocked_MassTransitIn:	.BYTE	"locked. Mass transit in"
aCongestedAreasWould:	.BYTE	"congested areas would",$F
aEaseTheTension_:		.BYTE	"ease the tension.",$F

_msg08_crime:
		.WORD	off_710C8
off_710C8:
		.WORD	aTheCrimeRateIsMuch
		.WORD	aTooHigh_MorePolice
		.WORD	aStationsAreNeededIn
		.WORD	aHighCrimeAreas_
		.WORD	byte_7112A
		.WORD	aFunded
aTheCrimeRateIsMuch:	.BYTE	" The crime rate is much"
aTooHigh_MorePolice:	.BYTE	"too high. More police",$F
aStationsAreNeededIn:	.BYTE	"stations are needed in "
aHighCrimeAreas_:		.BYTE	"high crime areas.",$F
byte_7112A:				.BYTE	" Are the police 100%",$F
aFunded:				.BYTE	"funded?",$F

_msg09_monster:
		.WORD	off_71149
off_71149:
		.WORD	aAGiantMonsterHas
		.WORD	aBeenSightedLeveling
		.WORD	aPartsOf
aAGiantMonsterHas:		.BYTE	" A giant monster has",$F
aBeenSightedLeveling:	.BYTE	"been sighted leveling",$F
aPartsOf:				.BYTE	"parts of ",$40,$24,$30,".",$F

_msg0A_quake:
		.WORD	off_7118A
off_7118A:
		.WORD	aYourCityIsBeing
		.WORD	aShakenToRubbleByAn
		.WORD	aEarthquakeWeNeedTo
		.WORD	aBeSureAnyFiresAre
		.WORD	aPutOutAndEveryone
		.WORD	aHasPower_
aYourCityIsBeing:		.BYTE	" Your city is being",$F
aShakenToRubbleByAn:	.BYTE	"shaken to rubble by an "
aEarthquakeWeNeedTo:	.BYTE	"earthquake! We need to "
aBeSureAnyFiresAre:		.BYTE	"be sure any fires are",$F
aPutOutAndEveryone:		.BYTE	"put out and everyone",$F
aHasPower_:				.BYTE	"has power.",$F

_msg0B_bank:
		.WORD	off_71210
off_71210:
		.WORD	aMr_mayor
		.WORD	aBankWantsToBuildA
		.WORD	aBranchOffice_
		.WORD	aIfThereIsAnother
		.WORD	aBankBuiltYouCan
		.WORD	aBorrowMoreMoney_
aMr_mayor:				.BYTE	" Mr.Mayor,",$40,$24,$32,$F
aBankWantsToBuildA:		.BYTE	"bank wants to build a",$F
aBranchOffice_:			.BYTE	"branch office.",$F
aIfThereIsAnother:		.BYTE	" If there is another",$F
aBankBuiltYouCan:		.BYTE	"Bank built, you can",$F
aBorrowMoreMoney_:		.BYTE	"borrow more money.",$F

_msg0C_establish_relationship:
		.WORD	off_7128D
off_7128D:
		.WORD	aMr_mayor_0
		.WORD	aCityWantsToEstablish
		.WORD	aASisterCityRelation
		.WORD	aShip_WouldYouLikeTo
		.WORD	aParticipate
aMr_mayor_0:			.BYTE	" Mr.Mayor,",$40,$24,$31,$0F
aCityWantsToEstablish:	.BYTE	"city wants to establish"
aASisterCityRelation:	.BYTE	"a sister-city relation-"
aShip_WouldYouLikeTo:	.BYTE	"ship. Would you like to"
aParticipate:			.BYTE	"participate?",$F

_msg0D_amusement_park:
		.WORD	off_712FA
off_712FA:
		.WORD	aMr_mayorTheCitizens
		.WORD	aOfTheCityDemandEn
		.WORD	aTertainment_Perhaps
		.WORD	aShouldOpenAnAmuse
		.WORD	aMentPark_
aMr_mayorTheCitizens:	.BYTE	" Mr.Mayor,the citizens "
aOfTheCityDemandEn:		.BYTE	"of the city demand en- "
aTertainment_Perhaps:	.BYTE	"tertainment. Perhaps we"
aShouldOpenAnAmuse:		.BYTE	"should open an Amuse-",$F
aMentPark_:				.BYTE	"ment Park.",$F

_msg0E_zoo:
		.WORD	off_7136C
off_7136C:
		.WORD	aMr_mayorTheCitizens
		.WORD	aOfTheCityDemandEn
		.WORD	aTertainment_Perhaps
		.WORD	aShouldOpenAZoo_
							;	" Mr.Mayor,the citizens "
							;	"of the city demand en- "
							;	"tertainment. Perhaps we"
aShouldOpenAZoo_:		.BYTE	"should open a Zoo.",$F

_msg0F_casino:
		.WORD	off_713CE
off_713CE:
		.WORD	aMr_mayorTheCitizens
		.WORD	aOfTheCityDemandEn
		.WORD	aTertainment_Perhaps
		.WORD	aShouldBuildACasino_
		.WORD	aCasinoSGenerateRev
		.WORD	aEnueButSometimesThe
		.WORD	aNeighborsComplainA
		.WORD	aBoutTheNoise_
							;	" Mr.Mayor,the citizens "
							;	"of the city demand en- "
							;	"tertainment. Perhaps we"
aShouldBuildACasino_:	.BYTE	"should build a Casino. "
aCasinoSGenerateRev:	.BYTE	" Casino*s generate rev-"
aEnueButSometimesThe:	.BYTE	"enue,but sometimes the "
aNeighborsComplainA:	.BYTE	"neighbors complain a-",$F
aBoutTheNoise_:			.BYTE	"bout the noise.",$F

_msg10_extra_land:
		.WORD	off_71490
off_71490:
		.WORD	aNowThatYouCanCre
		.WORD	aAteMoreLandBeSure
		.WORD	aToUseItWisely_
aNowThatYouCanCre:		.BYTE	" Now that you can cre- "
aAteMoreLandBeSure:		.BYTE	"ate more land, be sure "
aToUseItWisely_:		.BYTE	"to use it wisely.",$F

_msg11_new_police:
		.WORD	off_714D8
off_714D8:
		.WORD	aMr_mayorPerhapsWe
		.WORD	aCanBuildANewPolice
		.WORD	aStationForThePolice
		.WORD	aDepartmentOf
		.WORD	byte_71537
		.WORD	aWillSurelyIncrease
		.WORD	aTheMoralOfTheForce_
aMr_mayorPerhapsWe:		.BYTE	" Mr.Mayor,perhaps we",$F
aCanBuildANewPolice:	.BYTE	"can build a new Police "
aStationForThePolice:	.BYTE	"Station for the Police "
aDepartmentOf:			.BYTE	"Department of",$F
byte_71537:				.BYTE	$40,$24,$30," city. It",$F
aWillSurelyIncrease:	.BYTE	"will surely increase",$F
aTheMoralOfTheForce_:	.BYTE	"the moral of the force."

_msg12_new_fire:
		.WORD	off_71572
off_71572:
		.WORD	aMr_mayorOurFiremen
		.WORD	aNeedANewFirestation_
		.WORD	aWithBetterWorking
		.WORD	aConditionsTheyWill
		.WORD	aWorkMuchHarder_
aMr_mayorOurFiremen:	.BYTE	" Mr.Mayor,our firemen",$F
aNeedANewFirestation_:	.BYTE	"need a new FireStation."
aWithBetterWorking:		.BYTE	" With better working",$F
aConditionsTheyWill:	.BYTE	"conditions,they will",$F
aWorkMuchHarder_:		.BYTE	"work much harder.",$F

_msg13_monument_10th_years:
		.WORD	off_715E7
off_715E7:
		.WORD	aLetSErectAMonument
		.WORD	aToCommemorateThe10th
		.WORD	aAnniversaryOfOurFine
		.WORD	aTown_
aLetSErectAMonument:	.BYTE	" Let*s erect a Monument"
aToCommemorateThe10th:	.BYTE	"to commemorate the 10th"
aAnniversaryOfOurFine:	.BYTE	"anniversary of our fine"
aTown_:					.BYTE	"town.",$F

_msg14_monument_100th_years:
		.WORD	off_7163C
off_7163C:
		.WORD	aLetSErectAMonument
		.WORD	aToCommemorateThe
		.WORD	a100thAnniversaryOf
		.WORD	aOurFairCity_
							;	" Let*s erect a Monument"
aToCommemorateThe:		.BYTE	"to commemorate the",$F
a100thAnniversaryOf:	.BYTE	"100th anniversary of",$F
aOurFairCity_:			.BYTE	"our fair city.",$F

_msg15_expo:
		.WORD	off_71694
off_71694:
		.WORD	aMr_mayorHowAbout
		.WORD	aHoldingAWorldExposi
		.WORD	aTionIn
		.WORD	aCity_AnExpo
aMr_mayorHowAbout:		.BYTE	" Mr.Mayor,how about",$F
aHoldingAWorldExposi:	.BYTE	"holding a World Exposi-"
aTionIn:				.BYTE	"tion in ",$40,$24,$30,$0F
aCity_AnExpo:			.BYTE	"city. An Expo!",$F

_msg16_expo_monument:
		.WORD	off_716E4
off_716E4:
		.WORD	aNetEarningsFromThe
		.WORD	aExpoIs
		.WORD	aErectAMonumentTo
		.WORD	aCelebrateTheWorld
		.WORD	aExposition_
aNetEarningsFromThe:	.BYTE	" Net Earnings from the "
aExpoIs:				.BYTE	"Expo is ",$40,$23,$31,". Let*s",$F
aErectAMonumentTo:		.BYTE	"erect a Monument to",$F
aCelebrateTheWorld:		.BYTE	"celebrate the World",$F
aExposition_:			.BYTE	"Exposition.",$F

_msg17_liberty:
		.WORD	off_7174E
off_7174E:
		.WORD	aYourSisterCity
		.WORD	byte_7176F
		.WORD	aStatueOfLibertyAsA
		.WORD	aGoodwillGestureCom
		.WORD	aMemorating50YearsOf
		.WORD	aCooperationBetweenOur
		.WORD	aTwoCities_
aYourSisterCity:		.BYTE	" Your sister city,",$F
byte_7176F:				.BYTE	$40,$24,$31," city sent a",$F
aStatueOfLibertyAsA:	.BYTE	"Statue of Liberty as a "
aGoodwillGestureCom:	.BYTE	"goodwill gesture,com-",$F
aMemorating50YearsOf:	.BYTE	"memorating 50 years of "
aCooperationBetweenOur:	.BYTE	"cooperation between our"
aTwoCities_:			.BYTE	"two cities.",$F

_msg18_library:
		.WORD	off_717E8
off_717E8:
		.WORD	aMr_mayorLetSBuildA
		.WORD	aLibraryForTheCiti
		.WORD	aZens_
aMr_mayorLetSBuildA:	.BYTE	" Mr.Mayor,let*s build a"
aLibraryForTheCiti:		.BYTE	"Library for the citi-",$F
aZens_:					.BYTE	"zens.",$F

_msg19_parks_expand:
		.WORD	off_71823
off_71823:
		.WORD	aMr_mayorPerhapsWe
		.WORD	aShouldExpandTheParks
		.WORD	aInOurFairCity_
							;	" Mr.Mayor,perhaps we",$F
aShouldExpandTheParks:	.BYTE	"should expand the Parks"
aInOurFairCity_:		.BYTE	"in our fair city.",$F

_msg1A_railroad_station:
		.WORD	off_71869
off_71869:
		.WORD	aLetSBuildARailroad
		.WORD	aStation_
		.WORD	aTheAreaAroundThe
		.WORD	aStationWillSurelyBe
		.WORD	aWellDeveloped_
aLetSBuildARailroad:	.BYTE	" Let*s build a Railroad"
aStation_:				.BYTE	"Station.",$F
aTheAreaAroundThe:		.BYTE	" The area around the",$F
aStationWillSurelyBe:	.BYTE	"Station will surely be "
aWellDeveloped_:		.BYTE	"well-developed.",$F

_msg1B_practice_intro:
		.WORD	off_718D5
		.WORD	off_718E7
		.WORD	off_718F7
off_718D5:
		.WORD	aCongratulationsMr_
		.WORD	aMayorANewTownHas
		.WORD	aBeenCreated_
		.WORD	aIMDr_wrightAnd
		.WORD	aIVeBeenSentToHelp
		.WORD	aYou_WhyDonTYouGet
		.WORD	aInvolvedAndDevelop
		.WORD	aTheTownOnYourOwn
		.WORD	asc_71AD9
off_718E7:
		.WORD	aStartWithAPower
		.WORD	aPlantThenBuildSome
		.WORD	aRoads_AlongTheRoads
		.WORD	aZoneForResidential
		.WORD	aCommercialAndIndusri
		.WORD	aAlAreas_NextConnect
		.WORD	aPowerLinesToAll
		.WORD	_eol
off_718F7:
		.WORD	aZonesToMakeYourTown
		.WORD	aFunctional_Continue
		.WORD	aDevelopmentInThis
		.WORD	aWayMaintainingABal
		.WORD	aAncedSocietyAndWork
		.WORD	aIngTowardsAThriving
		.WORD	aMetropolis_
		.WORD	_eol
		.WORD	_eol
aCongratulationsMr_:	.BYTE	" Congratulations Mr.",$F
aMayorANewTownHas:		.BYTE	"Mayor! A new town has",$F
aBeenCreated_:			.BYTE	"been created.",$F
aIMDr_wrightAnd:		.BYTE	" I*m Dr.Wright, and",$F
aIVeBeenSentToHelp:		.BYTE	"I*ve been sent to help "
aYou_WhyDonTYouGet:		.BYTE	"you. Why don*t you get "
aInvolvedAndDevelop:	.BYTE	"involved and develop",$F
aTheTownOnYourOwn:		.BYTE	"the town on your own?",$F
asc_71AD9:				.BYTE	"                   ",_cur,$F

aStartWithAPower:		.BYTE	" Start with a power",$F
aPlantThenBuildSome:	.BYTE	"plant, then build some "
aRoads_AlongTheRoads:	.BYTE	"roads. Along the roads,"
aZoneForResidential:	.BYTE	"zone for residential,",$F
aCommercialAndIndusri:	.BYTE	"commercial,and indusri-"
aAlAreas_NextConnect:	.BYTE	"al areas. Next connect "
aPowerLinesToAll:		.BYTE	"power lines to all",$F
_eol:					.BYTE	$F

aZonesToMakeYourTown:	.BYTE	"zones to make your town"
aFunctional_Continue:	.BYTE	"functional. continue",$F
aDevelopmentInThis:		.BYTE	"development in this",$F
aWayMaintainingABal:	.BYTE	"way, maintaining a bal-"
aAncedSocietyAndWork:	.BYTE	"anced society and work-"
aIngTowardsAThriving:	.BYTE	"ing towards a thriving "
aMetropolis_:			.BYTE	"metropolis.",$F
							;	$F
							;	$F

_msg1C_practice_hints:
		.WORD	off_71AF2
		.WORD	off_71B04
off_71AF2:
		.WORD	aMr_MayorIsEvery
		.WORD	aThingGoingAlright
		.WORD	aTheIconGivesYou
		.WORD	asc_71B58
		.WORD	aInformationAboutYour
		.WORD	aCity_UseTheIcon
		.WORD	asc_71B58
		.WORD	aToSaveTheCityAtIts
		.WORD	asc_71AD9
off_71B04:
		.WORD	aCurrentStageOfDevel
		.WORD	aOpment_IfYouWantTo
		.WORD	aSeeMePress
		.WORD	asc_71B58
		.WORD	aWheneverYouGetStuck
		.WORD	_eol
		.WORD	_eol
		.WORD	_eol
		.WORD	_eol
aMr_MayorIsEvery:		.BYTE	" Mr. Mayor! Is every-",$F
aThingGoingAlright:		.BYTE	"thing going alright?",$F
aTheIconGivesYou:		.BYTE	" The ICON    gives you "
asc_71B58:				.BYTE	"            ",$F
aInformationAboutYour:	.BYTE	"information about your "
aCity_UseTheIcon:		.BYTE	"city. Use the ICON     "
							;	"            ",$F
aToSaveTheCityAtIts:	.BYTE	"to save the city at its"
							;	"                   ",$B0,$F

aCurrentStageOfDevel:	.BYTE	"current stage of devel-"
aOpment_IfYouWantTo:	.BYTE	"opment. If you want to "
aSeeMePress:			.BYTE	"see me, Press     ",$F
							;	"            ",$F
aWheneverYouGetStuck:	.BYTE	"whenever you get stuck!"
							;	$F
							;	$F
							;	$F
							;	$F

_msg1D_practice_success:
		.WORD	off_71C04
off_71C04:
		.WORD	aCongratulations_1
		.WORD	aMr_MayorYourCity
		.WORD	aNowHasAPopulationOf
		.WORD	a30000_YourExpert
		.WORD	aPlanningAndKindness
		.WORD	aHaveMadeYouARe
		.WORD	aSpectableMayor_Build
		.WORD	aANewTownByChoosing
		.WORD	aTheStartNewCity_
aCongratulations_1:		.BYTE	" Congratulations,",$F
aMr_MayorYourCity:		.BYTE	"Mr. Mayor! Your city",$F
aNowHasAPopulationOf:	.BYTE	"now has a population of"
a30000_YourExpert:		.BYTE	"30,000. Your expert",$F
aPlanningAndKindness:	.BYTE	"planning and kindness",$F
aHaveMadeYouARe:		.BYTE	"have made you a re-",$F
aSpectableMayor_Build:	.BYTE	"spectable mayor. Build "
aANewTownByChoosing:	.BYTE	"a new town by choosing "
aTheStartNewCity_:		.BYTE	"the START NEW CITY.",$F

_msg1E_practice_fail:
		.WORD	off_71CD6
off_71CD6:
		.WORD	aSorryMr_MayorYour
		.WORD	aCityWasnTSuccessful_
		.WORD	aDidYouHaveEvenZon
		.WORD	aIngWasTheBudgetOk
		.WORD	aDidYouLowerTheCrime
		.WORD	aLevelsDidYouTake
		.WORD	aCareOfTheTraffic
		.WORD	aSnarlsPleaseTryA
		.WORD	aGain_GoodLuck_
aSorryMr_MayorYour:		.BYTE	" Sorry Mr. Mayor, your "
aCityWasnTSuccessful_:	.BYTE	"city wasn*t successful."
aDidYouHaveEvenZon:		.BYTE	"Did you have even zon- "
aIngWasTheBudgetOk:		.BYTE	"ing? Was the budget OK?"
aDidYouLowerTheCrime:	.BYTE	"Did you lower the crime"
aLevelsDidYouTake:		.BYTE	"levels? Did you take",$F
aCareOfTheTraffic:		.BYTE	"care of the traffic",$F
aSnarlsPleaseTryA:		.BYTE	"snarls? Please try a-",$F
aGain_GoodLuck_:		.BYTE	"gain. Good luck.",$F

_msg1F_bank_hint:
		.WORD	off_71DAD
off_71DAD:
		.WORD	aYouCanGoToTheBank
		.WORD	aFromTheBudgetScreen_
aYouCanGoToTheBank:		.BYTE	" You can go to the bank"
aFromTheBudgetScreen_:	.BYTE	"from the budget screen."

_msg20_fire_dept_hint:
		.WORD	off_71DE1
off_71DE1:
		.WORD	aHaveYouBuiltAFire
		.WORD	aDepartmentYetIWould
		.WORD	aStronglySuggestDoing
		.WORD	aSoBeforeTheRestOf
		.WORD	aTheCityIsAnAshen
		.WORD	aEmber_
aHaveYouBuiltAFire:		.BYTE	" Have you built a fire "
aDepartmentYetIWould:	.BYTE	"department yet? I would"
aStronglySuggestDoing:	.BYTE	"strongly suggest doing "
aSoBeforeTheRestOf:		.BYTE	"so before the rest of",$F
aTheCityIsAnAshen:		.BYTE	"the city is an ashen",$F
aEmber_:				.BYTE	"ember.",$F

_msg21_flood:
		.WORD	off_71E66
off_71E66:
		.WORD	aMr_MayorTheLandIs
		.WORD	aBeingFlooded_Develop
		.WORD	aIngTheShorelineWill
		.WORD	aHelpPreventThisProb
		.WORD	aLem_
aMr_MayorTheLandIs:		.BYTE	" Mr. Mayor! The land is"
aBeingFlooded_Develop:	.BYTE	"being flooded. Develop-"
aIngTheShorelineWill:	.BYTE	"ing the shoreline will "
aHelpPreventThisProb:	.BYTE	"help prevent this prob-"
aLem_:					.BYTE	"lem.",$F

_msg22_airplane_crash:
		.WORD	off_71ED3
off_71ED3:
		.WORD	aAnAirplaneCrashHas
		.WORD	aStartedAFire_DonT
		.WORD	aLetTheFireGetOutOf
		.WORD	aControlOrTheWhole
		.WORD	aCityMayBeLost_
aAnAirplaneCrashHas:	.BYTE	" An airplane crash has "
aStartedAFire_DonT:		.BYTE	"started a fire. Don*t",$F
aLetTheFireGetOutOf:	.BYTE	"let the fire get out of"
aControlOrTheWhole:		.BYTE	"control or the whole",$F
aCityMayBeLost_:		.BYTE	"city may be lost.",$F

_msg23_tornado:
		.WORD	off_71F4A
off_71F4A:
		.WORD	aATornadoHasBeen
		.WORD	aSpottedAndIsCausing
		.WORD	aTremendousDamege_IM
		.WORD	aAfraidTheOnlyThing
		.WORD	aYouCanDoIsWait
		.WORD	aWhileItDestroysEvery
		.WORD	aThingInItSPath_
aATornadoHasBeen:		.BYTE	" A tornado has been",$F
aSpottedAndIsCausing:	.BYTE	"spotted and is causing "
aTremendousDamege_IM:	.BYTE	"tremendous damege. I*m "
aAfraidTheOnlyThing:	.BYTE	"afraid the only thing",$F
aYouCanDoIsWait:		.BYTE	"you can do is wait",$F
aWhileItDestroysEvery:	.BYTE	"while it destroys every"
aThingInItSPath_:		.BYTE	"thing in it*s path.",$F

		SECTION_ENDS 303, "PRG3 EVENTS MESSAGES DATA"

		BANK_END F4,$A000
