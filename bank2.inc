
		ORG $A000

		BANK_START $F2

; =============== S U B	R O U T	I N E =======================================
_presents_wnd_draw:
		JSR		_sram_on
		LDX		#$F0
loc_3E0B0:
		LDA		byte_3E143-1,X
		STA		_nt_backup_buf0+$F0-1,X
		LDA		byte_3E224-1,X
		STA		_extnt_backup0+$F0-1,X
		DEX
		BNE		loc_3E0B0
		LDX		#$03
loc_3E0C1:
		LDA		_city._presents_list,X
		BEQ		loc_3E0CD
		TXA
		PHA
		JSR		_present_icon_draw
		PLA
		TAX
loc_3E0CD:
		DEX
		BPL		loc_3E0C1
		JSR		_sram_off
		RTS

; =============== S U B	R O U T	I N E =======================================
_present_icon_draw:
		LDY		_city._presents_list,X
		DEY
		LDA		_present_icon_tlm_lib_lo,Y
		STA		_ptr0
		LDA		_present_icon_tlm_lib_hi,Y
		STA		_ptr0+1
		LDA		_present_icon_nt_dst_lo,X
		STA		_tmp2
		LDA		_present_icon_nt_dst_hi,X
		STA		_tmp3
		LDA		_present_icon_extnt_tlm_lib_hi,Y
		STA		_tmp4
		LDA		_present_icon_extnt_tlm_lib_lo,Y
		STA		_tmp5
		LDA		_present_icon_extnt_dst_lo,X
		STA		_tmp6
		LDA		_present_icon_extnt_dst_hi,X
		STA		_tmp6+1
		LDX		#$05
loc_3E102:
		LDY		#$04
loc_3E104:
		LDA		(_ptr0),Y
		STA		(_tmp2),Y
		LDA		(_tmp4),Y
		STA		(_tmp6),Y
		DEY
		BPL		loc_3E104
		JSR		_present_icon_pos_adjust
		DEX
		BNE		loc_3E102
		RTS

; =============== S U B	R O U T	I N E =======================================
_present_icon_pos_adjust:
		ADDW8	_ptr0,#$05
		ADDW8	_tmp4,#$05
		ADDW8	_tmp2,#$0F
		ADDW8	_tmp6,#$0F
		RTS

byte_3E143:
		.BYTE	$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5
		.BYTE	$D1,$00,$00,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$00,$00,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$10,$50,$74,$D9,$E9,$F9,$DC,$DC,$D9,$E9,$F9,$DC,$DC,$D6
		.BYTE	$D1,$12,$52,$74,$DA,$EA,$FA,$DC,$DC,$DA,$EA,$FA,$DC,$DC,$D6
		.BYTE	$D1,$05,$45,$74,$DB,$EB,$FB,$DC,$DC,$DB,$EB,$FB,$DC,$DC,$D6
		.BYTE	$D1,$13,$53,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$05,$45,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$0E,$4E,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$14,$54,$74,$D9,$E9,$F9,$DC,$DC,$D9,$E9,$F9,$DC,$DC,$D6
		.BYTE	$D1,$13,$53,$74,$DA,$EA,$FA,$DC,$DC,$DA,$EA,$FA,$DC,$DC,$D6
		.BYTE	$D1,$00,$00,$74,$DB,$EB,$FB,$DC,$DC,$DB,$EB,$FB,$DC,$DC,$D6
		.BYTE	$D1,$00,$00,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$00,$00,$74,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D2,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D7
byte_3E224:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$12,$12,$12,$41,$41,$12,$12,$12,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$12,$12,$12,$41,$41,$12,$12,$12,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$12,$12,$12,$41,$41,$12,$12,$12,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$12,$12,$12,$41,$41,$12,$12,$12,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$12,$12,$12,$41,$41,$12,$12,$12,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$12,$12,$12,$41,$41,$12,$12,$12,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$98,$98,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_present_icon_tlm_lib_lo:
		.BYTE	<[_present_icon_tlm0]
		.BYTE	<[_present_icon_tlm1]
		.BYTE	<[_present_icon_tlm2]
		.BYTE	<[_present_icon_tlm3]
		.BYTE	<[_present_icon_tlm4]
		.BYTE	<[_present_icon_tlm5]
		.BYTE	<[_present_icon_tlm6]
		.BYTE	<[_present_icon_tlm7]
		.BYTE	<[_present_icon_tlm8]
		.BYTE	<[_present_icon_tlm9]
		.BYTE	<[_present_icon_tlmA]
		.BYTE	<[_present_icon_tlmB]
		.BYTE	<[_present_icon_tlmC]
		.BYTE	<[_present_icon_tlmD]
		.BYTE	<[_present_icon_tlmE]
_present_icon_tlm_lib_hi:
		.BYTE	>[_present_icon_tlm0]
		.BYTE	>[_present_icon_tlm1]
		.BYTE	>[_present_icon_tlm2]
		.BYTE	>[_present_icon_tlm3]
		.BYTE	>[_present_icon_tlm4]
		.BYTE	>[_present_icon_tlm5]
		.BYTE	>[_present_icon_tlm6]
		.BYTE	>[_present_icon_tlm7]
		.BYTE	>[_present_icon_tlm8]
		.BYTE	>[_present_icon_tlm9]
		.BYTE	>[_present_icon_tlmA]
		.BYTE	>[_present_icon_tlmB]
		.BYTE	>[_present_icon_tlmC]
		.BYTE	>[_present_icon_tlmD]
		.BYTE	>[_present_icon_tlmE]
_present_icon_nt_dst_lo:
		.BYTE	<[_nt_backup_buf1+$12]
		.BYTE	<[_nt_backup_buf1+$6C]
		.BYTE	<[_nt_backup_buf1+$17]
		.BYTE	<[_nt_backup_buf1+$71]
_present_icon_nt_dst_hi:
		.BYTE	>[_nt_backup_buf1+$12]
		.BYTE	>[_nt_backup_buf1+$6C]
		.BYTE	>[_nt_backup_buf1+$17]
		.BYTE	>[_nt_backup_buf1+$71]
_present_icon_extnt_tlm_lib_hi:
		.BYTE	<[_present_icon_extnt_tlm0]
		.BYTE	<[_present_icon_extnt_tlm1]
		.BYTE	<[_present_icon_extnt_tlm2]
		.BYTE	<[_present_icon_extnt_tlm3]
		.BYTE	<[_present_icon_extnt_tlm4]
		.BYTE	<[_present_icon_extnt_tlm5]
		.BYTE	<[_present_icon_extnt_tlm6]
		.BYTE	<[_present_icon_extnt_tlm7]
		.BYTE	<[_present_icon_extnt_tlm8]
		.BYTE	<[_present_icon_extnt_tlm9]
		.BYTE	<[_present_icon_extnt_tlmA]
		.BYTE	<[_present_icon_extnt_tlmB]
		.BYTE	<[_present_icon_extnt_tlmC]
		.BYTE	<[_present_icon_extnt_tlmD]
		.BYTE	<[_present_icon_extnt_tlmE]
_present_icon_extnt_tlm_lib_lo:
		.BYTE	>[_present_icon_extnt_tlm0]
		.BYTE	>[_present_icon_extnt_tlm1]
		.BYTE	>[_present_icon_extnt_tlm2]
		.BYTE	>[_present_icon_extnt_tlm3]
		.BYTE	>[_present_icon_extnt_tlm4]
		.BYTE	>[_present_icon_extnt_tlm5]
		.BYTE	>[_present_icon_extnt_tlm6]
		.BYTE	>[_present_icon_extnt_tlm7]
		.BYTE	>[_present_icon_extnt_tlm8]
		.BYTE	>[_present_icon_extnt_tlm9]
		.BYTE	>[_present_icon_extnt_tlmA]
		.BYTE	>[_present_icon_extnt_tlmB]
		.BYTE	>[_present_icon_extnt_tlmC]
		.BYTE	>[_present_icon_extnt_tlmD]
		.BYTE	>[_present_icon_extnt_tlmE]
_present_icon_extnt_dst_lo:
		.BYTE	<[_extnt_backup1+$12]
		.BYTE	<[_extnt_backup1+$6C]
		.BYTE	<[_extnt_backup1+$17]
		.BYTE	<[_extnt_backup1+$71]
_present_icon_extnt_dst_hi:
		.BYTE	>[_extnt_backup1+$12]
		.BYTE	>[_extnt_backup1+$6C]
		.BYTE	>[_extnt_backup1+$17]
		.BYTE	>[_extnt_backup1+$71]
_present_icon_tlm0:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$03,$13,$23,$66,$DC
		.BYTE	$04,$14,$24,$67,$DC
		.BYTE	$05,$15,$25,$68,$DC
		.BYTE	$DC,$DC,$DC,$DC,$DC
_present_icon_tlm1:
		.BYTE	$DC,$DC,$DC,$91,$DC
		.BYTE	$31,$41,$51,$92,$96
		.BYTE	$32,$42,$52,$93,$97
		.BYTE	$33,$43,$53,$94,$98
		.BYTE	$DC,$DC,$DC,$95,$DC
_present_icon_tlm2:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$09,$19,$29,$86,$DC
		.BYTE	$0A,$1A,$2A,$87,$DC
		.BYTE	$0B,$1B,$2B,$88,$DC
		.BYTE	$DC,$DC,$DC,$DC,$DC
_present_icon_tlm3:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$06,$16,$26,$7D,$DC
		.BYTE	$07,$17,$27,$7E,$DC
		.BYTE	$08,$18,$28,$7F,$DC
		.BYTE	$DC,$DC,$DC,$DC,$DC
_present_icon_tlm4:
		.BYTE	$DC,$DC,$DC,$DC,$3B
		.BYTE	$DD,$ED,$FD,$33,$3C
		.BYTE	$DE,$EE,$FE,$34,$3D
		.BYTE	$DF,$EF,$FF,$35,$3E
		.BYTE	$DC,$DC,$DC,$DC,$3F
_present_icon_tlm5:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$A6,$B6,$C6,$43,$2B
		.BYTE	$A7,$B7,$C7,$44,$2C
		.BYTE	$A8,$B8,$C8,$45,$2D
		.BYTE	$DC,$DC,$DC,$DC,$2E
_present_icon_tlm6:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$A9,$B9,$C9,$56,$2B
		.BYTE	$AA,$BA,$CA,$57,$2C
		.BYTE	$AB,$BB,$CB,$5B,$2D
		.BYTE	$DC,$DC,$DC,$DC,$2E
_present_icon_tlm7:
		.BYTE	$DC,$DC,$DC,$DC,$4B
		.BYTE	$A0,$B0,$C0,$5C,$4C
		.BYTE	$A1,$B1,$C1,$6B,$4D
		.BYTE	$A2,$B2,$C2,$6C,$4E
		.BYTE	$DC,$DC,$DC,$DC,$4F
_present_icon_tlm8:
		.BYTE	$DC,$DC,$DC,$DC,$4B
		.BYTE	$A3,$B3,$C3,$53,$4C
		.BYTE	$A4,$B4,$C4,$54,$4D
		.BYTE	$A5,$B5,$C5,$55,$4E
		.BYTE	$DC,$DC,$DC,$DC,$4F
_present_icon_tlm9:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$D0,$E0,$F0,$30,$DC
		.BYTE	$D1,$E1,$F1,$40,$DC
		.BYTE	$D2,$E2,$F2,$65,$DC
		.BYTE	$DC,$DC,$DC,$DC,$DC
_present_icon_tlmA:
		.BYTE	$DC,$DC,$DC,$6D,$DC
		.BYTE	$00,$10,$20,$6E,$DC
		.BYTE	$01,$11,$21,$6F,$DC
		.BYTE	$02,$12,$22,$76,$DC
		.BYTE	$DC,$DC,$DC,$77,$DC
_present_icon_tlmB:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$AC,$BC,$CC,$36,$DC
		.BYTE	$AD,$BD,$CD,$37,$DC
		.BYTE	$AE,$BE,$CE,$46,$DC
		.BYTE	$DC,$DC,$DC,$47,$DC
_present_icon_tlmC:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$0C,$1C,$2C,$99,$DC
		.BYTE	$0D,$1D,$2D,$9A,$DC
		.BYTE	$0E,$1E,$2E,$9B,$DC
		.BYTE	$DC,$DC,$DC,$DC,$DC
_present_icon_tlmD:
		.BYTE	$DC,$DC,$DC,$DC,$DC
		.BYTE	$37,$47,$57,$69,$DC
		.BYTE	$38,$48,$58,$6A,$DC
		.BYTE	$39,$49,$59,$6B,$DC
		.BYTE	$DC,$DC,$DC,$6C,$DC
_present_icon_tlmE:
		.BYTE	$DC,$DC,$DC,$78,$DC
		.BYTE	$3A,$4A,$5A,$79,$DC
		.BYTE	$3B,$4B,$5B,$7A,$DC
		.BYTE	$3C,$4C,$5C,$7B,$DC
		.BYTE	$DC,$DC,$DC,$7C,$DC
_present_icon_extnt_tlm0:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$12,$92,$12,$52,$41
		.BYTE	$92,$92,$12,$52,$41
		.BYTE	$92,$92,$12,$52,$41
		.BYTE	$41,$41,$41,$41,$41
_present_icon_extnt_tlm1:
		.BYTE	$41,$41,$41,$52,$41
		.BYTE	$D2,$D2,$12,$52,$52
		.BYTE	$D2,$D2,$12,$52,$52
		.BYTE	$92,$92,$12,$52,$52
		.BYTE	$41,$41,$41,$52,$41
_present_icon_extnt_tlm2:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$41,$41,$41,$41,$41
_present_icon_extnt_tlm3:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$92,$92,$92,$52,$41
		.BYTE	$D2,$92,$92,$52,$41
		.BYTE	$D2,$92,$92,$52,$41
		.BYTE	$41,$41,$41,$41,$41
_present_icon_extnt_tlm4:
		.BYTE	$41,$41,$41,$41,$56
		.BYTE	$12,$12,$12,$56,$56
		.BYTE	$92,$92,$12,$56,$56
		.BYTE	$92,$92,$12,$56,$56
		.BYTE	$41,$41,$41,$41,$56
_present_icon_extnt_tlm5:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$12,$12,$52,$56,$56
		.BYTE	$12,$12,$12,$56,$56
		.BYTE	$12,$12,$12,$56,$56
		.BYTE	$41,$41,$41,$41,$56
_present_icon_extnt_tlm6:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$92,$92,$12,$56,$56
		.BYTE	$92,$92,$92,$56,$56
		.BYTE	$92,$92,$92,$56,$56
		.BYTE	$41,$41,$41,$41,$56
_present_icon_extnt_tlm7:
		.BYTE	$41,$41,$41,$41,$56
		.BYTE	$D2,$D2,$D2,$56,$56
		.BYTE	$52,$52,$D2,$56,$56
		.BYTE	$52,$52,$D2,$56,$56
		.BYTE	$41,$41,$41,$41,$56
_present_icon_extnt_tlm8:
		.BYTE	$41,$41,$41,$41,$56
		.BYTE	$D2,$52,$52,$56,$56
		.BYTE	$D2,$52,$52,$56,$56
		.BYTE	$D2,$52,$D2,$56,$56
		.BYTE	$41,$41,$41,$41,$56
_present_icon_extnt_tlm9:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$12,$12,$52,$56,$41
		.BYTE	$12,$12,$52,$56,$41
		.BYTE	$12,$12,$52,$56,$41
		.BYTE	$41,$41,$41,$41,$41
_present_icon_extnt_tlmA:
		.BYTE	$41,$41,$41,$52,$41
		.BYTE	$D2,$D2,$12,$52,$41
		.BYTE	$12,$12,$12,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$41,$41,$41,$52,$41
_present_icon_extnt_tlmB:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$D2,$D2,$D2,$56,$41
		.BYTE	$92,$92,$D2,$56,$41
		.BYTE	$92,$92,$D2,$56,$41
		.BYTE	$41,$41,$41,$56,$41
_present_icon_extnt_tlmC:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$41,$41,$41,$41,$41
_present_icon_extnt_tlmD:
		.BYTE	$41,$41,$41,$41,$41
		.BYTE	$12,$12,$12,$52,$41
		.BYTE	$12,$12,$12,$52,$41
		.BYTE	$12,$12,$12,$52,$41
		.BYTE	$41,$41,$41,$52,$41
_present_icon_extnt_tlmE:
		.BYTE	$41,$41,$41,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$D2,$92,$D2,$52,$41
		.BYTE	$D2,$D2,$D2,$52,$41
		.BYTE	$41,$41,$41,$52,$41

; =============== S U B	R O U T	I N E =======================================
_presents_build_menu:
		LDA		#$FF
		STA		byte_37
		LDA		#$00
		STA		_cur_present_select_idx
		JSR		_presents_constr_palette_draw
loc_3E64B:
		JSR		_wait_for_nmi
		JSR		_spr_clear
		LDA		_cur_present_select_idx
		AND		#$01
		ASL
		ASL
		ASL
		ASL
		STA		_ptr0
		ASL
		CLC
		ADC		_ptr0
		ADC		#$5B
		STA		_spr_insert_args._pos._X
		LDA		_cur_present_select_idx
		AND		#$02
		ASL
		ASL
		STA		_ptr0
		ASL
		ASL
		CLC
		ADC		_ptr0
		ADC		#$A4
		STA		_spr_insert_args._pos._Y
		LDA		#$00
		STA		_spr_insert_args._idx
		JSR		_spr_lib_attr_insert
		LDA		_pad0_autorep_press
		AND		#$0F
		BNE		loc_3E68B
		LDA		_pad0_autorep_press
		AND		#$C0
		BEQ		loc_3E64B
		BNE		loc_3E6C7
loc_3E68B:
		AND		#$03
		BEQ		loc_3E6A3
		LSR
		LDA		_cur_present_select_idx
		BCS		loc_3E69E
		SEC
		SBC		#$01
		BCS		loc_3E6AF
		ADC		#$04
		BNE		loc_3E6AF
loc_3E69E:
		CLC
		ADC		#$01
		BNE		loc_3E6A9
loc_3E6A3:
		LDA		_cur_present_select_idx
		CLC
		ADC		#$02
loc_3E6A9:
		CMP		#$04
		BCC		loc_3E6AF
		SBC		#$04
loc_3E6AF:
		STA		_cur_present_select_idx
		TAX
		LDA		_city._presents_list,X
		BNE		loc_3E6BD
		LDA		#$00
		STA		_cur_present_select_idx
loc_3E6BD:
		LDA		#$40
		STA		_apu_se_flags+1
		JSR		_presents_constr_palette_draw
		JMP		loc_3E64B
loc_3E6C7:
		LDA		#$03
		STA		byte_C0
		JSR		_spr_clear
		RTS
byte_3E6CF:
		.BYTE	$DF,$DF,$DF,$90,$91,$92,$93,$94,$95,$DF,$30,$26,$DF,$DF,$DF,$90
		.BYTE	$96,$97,$98,$99,$DF,$DF,$30,$56,$DF,$DF,$90,$96,$3C,$9B,$9C,$A0
		.BYTE	$A1,$DF,$30,$27,$DF,$90,$96,$A2,$A3,$A4,$A5,$A6,$A7,$DF,$30,$57
		.BYTE	$DF,$DF,$DF,$90,$96,$9A,$A8,$A9,$DF,$DF,$30,$56,$DF,$AA,$A1,$97
		.BYTE	$AB,$AC,$AD,$30,$DF,$30,$26,$5A,$DF,$AA,$A1,$31,$32,$33,$34,$35
		.BYTE	$36,$30,$26,$5A,$DF,$AA,$A1,$37,$38,$39,$3A,$3B,$DF,$30,$26,$5A
		.BYTE	$DF,$90,$96,$3C,$3D,$3E,$40,$41,$DF,$30,$27,$5A,$DF,$90,$96,$42
		.BYTE	$43,$44,$45,$46,$DF,$30,$27,$5A,$DF,$90,$96,$47,$48,$49,$4A,$DF
		.BYTE	$DF,$30,$58,$5A,$DF,$90,$96,$3C,$9B,$9C,$4B,$4C,$4D,$30,$58,$5A
		.BYTE	$DF,$90,$96,$4E,$4F,$50,$51,$DF,$DF,$30,$59,$5A,$DF,$90,$96,$97
		.BYTE	$98,$99,$9A,$DF,$DF,$30,$26,$5A,$DF,$90,$96,$3C,$9B,$9C,$4B,$4C
		.BYTE	$4D,$30,$59,$5A,$DF,$90,$96,$52,$53,$54,$55,$DF,$30,$26,$5A,$5A
		.BYTE	$DF,$A3,$A4,$A5,$A6,$A7,$A8,$DF,$DF,$DF,$DF,$DF
byte_3E79B:
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$06,$02,$02,$02,$02
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$06,$02,$02,$02,$02,$06,$02,$02,$02
		.BYTE	$02,$02,$02,$06,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$06
		.BYTE	$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$02,$06,$02,$02,$02,$02
		.BYTE	$02,$02,$02,$06,$02,$02,$06,$06,$02,$02,$02,$06,$06,$06,$06,$06
		.BYTE	$06,$02,$06,$06,$02,$02,$02,$06,$06,$06,$06,$06,$02,$02,$06,$06
		.BYTE	$02,$02,$02,$06,$06,$06,$06,$06,$02,$02,$06,$06,$02,$02,$02,$06
		.BYTE	$06,$06,$06,$06,$02,$02,$06,$06,$02,$02,$02,$06,$06,$06,$06,$02
		.BYTE	$02,$02,$06,$06,$02,$02,$02,$06,$02,$02,$06,$06,$06,$02,$06,$06
		.BYTE	$02,$02,$02,$06,$06,$06,$06,$02,$02,$02,$06,$06,$02,$02,$02,$06
		.BYTE	$06,$06,$06,$02,$02,$02,$06,$06,$02,$02,$02,$06,$02,$02,$06,$06
		.BYTE	$06,$02,$06,$06,$02,$02,$02,$06,$06,$06,$06,$02,$02,$06,$06,$06
		.BYTE	$02,$06,$06,$06,$06,$06,$06,$02,$02,$02,$02,$02
_wnd_height0_list:
		.BYTE	$0D,$0D,$0F,$0F,$0E,$0C,$0D,$10,$0F,$0F,$0E,$10,$0F,$0F,$0F,$0F
		.BYTE	$10,$10,$10
_wnd_height1_list:
		.BYTE	$0F,$10,$10,$0F,$0F,$0F,$0A,$0A,$0A,$0F,$0B,$09,$0F,$0F,$0F,$0F
		.BYTE	$09,$0A,$0A
_wnd_type_list:
		.BYTE	$00,$00,$00,$00,$00,$00,$01,$01,$01,$01,$01,$00,$01,$01,$01,$01
		.BYTE	$00,$00,$00
_wnd_width_list:
		.BYTE	$0D,$0D,$0F,$0F,$0E,$0C,$0A,$0A,$0A,$0F,$0B,$10,$0F,$0F,$0F,$0F
		.BYTE	$10,$10,$10
_wnd_nt_lib_lo:
		.BYTE	<[_wnd_nt00]
		.BYTE	<[_wnd_nt01]
		.BYTE	<[_wnd_nt02]
		.BYTE	<[_wnd_nt03]
		.BYTE	<[_wnd_nt04]
		.BYTE	<[_wnd_nt00]
		.BYTE	<[_wnd_nt06]
		.BYTE	<[_wnd_nt07]
		.BYTE	<[_wnd_nt08]
		.BYTE	<[_wnd_nt09]
		.BYTE	<[_wnd_nt0A]
		.BYTE	<[_wnd_nt0B]
		.BYTE	<[_nt_backup_buf0+$F0]
		.BYTE	<[_nt_backup_buf0+$F0]
		.BYTE	<[_nt_backup_buf0+$F0]
		.BYTE	<[_nt_backup_buf0+$F0]
		.BYTE	<[_wnd_nt10]
		.BYTE	<[_wnd_nt11]
		.BYTE	<[_wnd_nt12]
_wnd_nt_lib_hi:
		.BYTE	>[_wnd_nt00]
		.BYTE	>[_wnd_nt01]
		.BYTE	>[_wnd_nt02]
		.BYTE	>[_wnd_nt03]
		.BYTE	>[_wnd_nt04]
		.BYTE	>[_wnd_nt00]
		.BYTE	>[_wnd_nt06]
		.BYTE	>[_wnd_nt07]
		.BYTE	>[_wnd_nt08]
		.BYTE	>[_wnd_nt09]
		.BYTE	>[_wnd_nt0A]
		.BYTE	>[_wnd_nt0B]
		.BYTE	>[_nt_backup_buf0+$F0]
		.BYTE	>[_nt_backup_buf0+$F0]
		.BYTE	>[_nt_backup_buf0+$F0]
		.BYTE	>[_nt_backup_buf0+$F0]
		.BYTE	>[_wnd_nt10]
		.BYTE	>[_wnd_nt11]
		.BYTE	>[_wnd_nt12]
_wnd_extnt_lib_lo:
		.BYTE	<[_wnd_extnt00]
		.BYTE	<[_wnd_extnt01]
		.BYTE	<[_wnd_extnt02]
		.BYTE	<[_wnd_extnt03]
		.BYTE	<[_wnd_extnt04]
		.BYTE	<[_wnd_extnt06]
		.BYTE	<[_wnd_extnt06]
		.BYTE	<[_wnd_extnt07]
		.BYTE	<[_wnd_extnt08]
		.BYTE	<[_wnd_extnt09]
		.BYTE	<[_wnd_extnt0A]
		.BYTE	<[_wnd_extnt0B]
		.BYTE	<[_extnt_backup0+$F0]
		.BYTE	<[_extnt_backup0+$F0]
		.BYTE	<[_extnt_backup0+$F0]
		.BYTE	<[_extnt_backup0+$F0]
		.BYTE	<[_wnd_extnt0B]
		.BYTE	<[_wnd_extnt11]
		.BYTE	<[_wnd_extnt11]
_wnd_extnt_lib_hi:
		.BYTE	>[_wnd_extnt00]
		.BYTE	>[_wnd_extnt01]
		.BYTE	>[_wnd_extnt02]
		.BYTE	>[_wnd_extnt03]
		.BYTE	>[_wnd_extnt04]
		.BYTE	>[_wnd_extnt06]
		.BYTE	>[_wnd_extnt06]
		.BYTE	>[_wnd_extnt07]
		.BYTE	>[_wnd_extnt08]
		.BYTE	>[_wnd_extnt09]
		.BYTE	>[_wnd_extnt0A]
		.BYTE	>[_wnd_extnt0B]
		.BYTE	>[_extnt_backup0+$F0]
		.BYTE	>[_extnt_backup0+$F0]
		.BYTE	>[_extnt_backup0+$F0]
		.BYTE	>[_extnt_backup0+$F0]
		.BYTE	>[_wnd_extnt0B]
		.BYTE	>[_wnd_extnt11]
		.BYTE	>[_wnd_extnt11]
_wnd_nt00:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$00,$00,$00,$13,$10,$05,$05,$04,$00,$00,$00,$D4
		.BYTE	$D3,$00,$00,$00,$53,$50,$45,$45,$44,$00,$00,$00,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$00,$01,$02,$DC,$DC,$DC,$03,$04,$05,$DC,$D4
		.BYTE	$D3,$DC,$10,$11,$12,$DC,$DC,$DC,$13,$14,$15,$DC,$D4
		.BYTE	$D3,$DC,$20,$21,$22,$DC,$DC,$DC,$23,$24,$25,$DC,$D4
		.BYTE	$D3,$DC,$00,$01,$02,$DC,$DC,$DC,$03,$04,$05,$A0,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$DC,$06,$07,$08,$DC,$DC,$DC,$09,$0A,$0B,$DC,$D4
		.BYTE	$D3,$DC,$16,$17,$18,$DC,$DC,$DC,$19,$1A,$1B,$DC,$D4
		.BYTE	$D3,$DC,$26,$27,$28,$DC,$DC,$DC,$29,$2A,$2B,$DC,$D4
		.BYTE	$D3,$DC,$06,$07,$08,$DC,$DC,$DC,$09,$0A,$0B,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_nt01:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$00,$00,$0F,$10,$14,$09,$0F,$0E,$13,$00,$00,$D4
		.BYTE	$D3,$00,$00,$4F,$50,$54,$49,$4F,$4E,$53,$00,$00,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$0C,$0D,$0E,$DC,$DC,$DC,$30,$31,$32,$DC,$D4
		.BYTE	$D3,$DC,$1C,$1D,$1E,$DC,$DC,$DC,$40,$41,$42,$DC,$D4
		.BYTE	$D3,$DC,$2C,$2D,$2E,$DC,$DC,$DC,$50,$51,$52,$DC,$D4
		.BYTE	$D3,$DC,$0C,$0D,$0E,$DC,$DC,$DC,$0C,$0D,$0E,$DC,$D4
		.BYTE	$D3,$0F,$10,$11,$12,$13,$DC,$DC,$14,$15,$16,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$DC,$33,$34,$35,$DC,$DC,$DC,$36,$37,$38,$DC,$D4
		.BYTE	$D3,$DC,$43,$44,$45,$DC,$DC,$DC,$46,$47,$48,$DC,$D4
		.BYTE	$D3,$DC,$53,$54,$55,$DC,$DC,$DC,$56,$57,$58,$DC,$D4
		.BYTE	$D3,$DC,$0C,$0D,$0E,$DC,$DC,$1A,$1B,$1C,$1D,$1E,$D4
		.BYTE	$D3,$DC,$17,$18,$19,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_nt02:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$00,$00,$04,$09,$13,$01,$13,$14,$05,$12,$13,$00,$00,$D4
		.BYTE	$D3,$00,$00,$44,$49,$53,$41,$53,$54,$45,$52,$53,$00,$00,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$39,$3A,$3B,$DC,$3C,$3D,$3E,$DC,$60,$61,$62,$DC,$D4
		.BYTE	$D3,$DC,$49,$4A,$4B,$DC,$4C,$4D,$4E,$DC,$70,$71,$72,$DC,$D4
		.BYTE	$D3,$DC,$59,$5A,$5B,$DC,$5C,$5D,$5E,$DC,$80,$81,$82,$DC,$D4
		.BYTE	$D3,$DC,$1F,$20,$21,$DC,$22,$23,$24,$25,$26,$27,$28,$29,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$DC,$63,$64,$65,$DC,$66,$67,$68,$DC,$69,$6A,$6B,$DC,$D4
		.BYTE	$D3,$DC,$73,$74,$75,$DC,$76,$77,$78,$DC,$79,$7A,$7B,$DC,$D4
		.BYTE	$D3,$DC,$83,$84,$85,$DC,$86,$87,$88,$DC,$89,$8A,$8B,$DC,$D4
		.BYTE	$D3,$2A,$2B,$2C,$2D,$2E,$2F,$30,$31,$DC,$35,$36,$37,$38,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$32,$33,$34,$DC,$39,$3A,$3B,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_nt03:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$00,$00,$00,$17,$09,$0E,$04,$0F,$17,$13,$00,$00,$00,$D4
		.BYTE	$D3,$00,$00,$00,$57,$49,$4E,$44,$4F,$57,$53,$00,$00,$00,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$6C,$6D,$6E,$DC,$96,$97,$98,$DC,$90,$91,$92,$DC,$D4
		.BYTE	$D3,$DC,$7C,$7D,$7E,$DC,$A6,$A7,$A8,$DC,$A0,$A1,$A2,$DC,$D4
		.BYTE	$D3,$DC,$8C,$8D,$8E,$DC,$B6,$B7,$B8,$DC,$B0,$B1,$B2,$DC,$D4
		.BYTE	$D3,$DC,$3C,$3D,$3E,$45,$46,$47,$48,$49,$3F,$40,$41,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$DC,$93,$94,$95,$DC,$82,$83,$84,$DC,$88,$89,$8A,$DC,$D4
		.BYTE	$D3,$DC,$A3,$A4,$A5,$DC,$92,$93,$94,$DC,$8B,$8C,$8D,$DC,$D4
		.BYTE	$D3,$DC,$B3,$B4,$B5,$DC,$85,$86,$87,$DC,$9B,$9C,$9D,$DC,$D4
		.BYTE	$D3,$DC,$42,$43,$44,$95,$96,$97,$98,$99,$8E,$8F,$9E,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_nt04:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$00,$00,$00,$00,$07,$01,$0D,$05,$00,$00,$00,$00,$D4
		.BYTE	$D3,$00,$00,$00,$00,$47,$41,$4D,$45,$00,$00,$00,$00,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$DC,$9C,$9D,$9E,$DC,$DC,$99,$9A,$9B,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$AC,$AD,$AE,$DC,$DC,$A9,$AA,$AB,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$BC,$BD,$BE,$DC,$DC,$B9,$BA,$BB,$DC,$DC,$D4
		.BYTE	$D3,$DC,$4A,$4B,$4C,$4D,$4E,$4F,$50,$51,$4D,$4E,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$68,$69,$6A,$DC,$DC,$C3,$C4,$C5,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$6B,$6C,$6D,$DC,$DC,$D3,$D4,$D5,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$5D,$5E,$5F,$DC,$DC,$E3,$E4,$E5,$DC,$DC,$D4
		.BYTE	$D3,$DC,$70,$71,$72,$73,$74,$DC,$5A,$5B,$5C,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_nt06:
		.BYTE	$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5
		.BYTE	$D1,$20,$50,$74,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$20,$4F,$74,$E0,$F0,$E3,$5D,$DC,$D6
		.BYTE	$D1,$20,$57,$74,$E1,$F1,$E4,$5E,$DC,$D6
		.BYTE	$D1,$20,$45,$74,$E2,$F2,$E5,$5F,$DC,$D6
		.BYTE	$D1,$20,$52,$74,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$20,$50,$74,$DC,$DC,$DC,$60,$DC,$D6
		.BYTE	$D1,$20,$4C,$74,$F3,$E6,$F6,$61,$DC,$D6
		.BYTE	$D1,$20,$41,$74,$F4,$E7,$F7,$62,$DC,$D6
		.BYTE	$D1,$20,$4E,$74,$F5,$E8,$F8,$63,$DC,$D6
		.BYTE	$D1,$20,$54,$74,$DC,$DC,$DC,$64,$DC,$D6
		.BYTE	$D2,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D7
_wnd_nt07:
		.BYTE	$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5
		.BYTE	$D1,$DC,$50,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$4F,$74,$DD,$DD,$DD,$65,$DD,$D6
		.BYTE	$D1,$DC,$50,$74,$C6,$D6,$E6,$66,$6A,$D6
		.BYTE	$D1,$DC,$55,$74,$C7,$D7,$E7,$67,$6B,$D6
		.BYTE	$D1,$DC,$4C,$74,$C8,$D8,$E8,$68,$6C,$D6
		.BYTE	$D1,$DC,$41,$74,$DD,$DD,$DD,$69,$6D,$D6
		.BYTE	$D1,$DC,$54,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$49,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$4F,$74,$DD,$DD,$DD,$6E,$DD,$D6
		.BYTE	$D1,$DC,$4E,$74,$C9,$D9,$E9,$6F,$73,$D6
		.BYTE	$D1,$DC,$DC,$74,$CA,$DA,$EA,$70,$74,$D6
		.BYTE	$D1,$DC,$4D,$74,$CB,$DB,$EB,$71,$75,$D6
		.BYTE	$D1,$DC,$41,$74,$DD,$DD,$DD,$72,$B0,$D6
		.BYTE	$D1,$DC,$50,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D2,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D7
_wnd_nt08:
		.BYTE	$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5
		.BYTE	$D1,$DC,$43,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$49,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$54,$74,$F0,$F3,$F6,$76,$79,$D6
		.BYTE	$D1,$DC,$59,$74,$F1,$F4,$F7,$77,$7A,$D6
		.BYTE	$D1,$DC,$DC,$74,$F2,$F5,$F8,$78,$7B,$D6
		.BYTE	$D1,$DC,$53,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$45,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$52,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$56,$74,$EC,$FC,$F9,$7C,$79,$D6
		.BYTE	$D1,$DC,$49,$74,$ED,$FD,$FA,$7D,$7A,$D6
		.BYTE	$D1,$DC,$43,$74,$EE,$FE,$FB,$7E,$7B,$D6
		.BYTE	$D1,$DC,$45,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$53,$74,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D2,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D7
_wnd_nt09:
		.BYTE	$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5
		.BYTE	$D1,$DC,$DC,$74,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$DC,$74,$DD,$DD,$DD,$7F,$DD,$DD,$DD,$DD,$8C,$DD,$D6
		.BYTE	$D1,$DC,$43,$74,$0F,$1F,$2F,$80,$DD,$D0,$E0,$F0,$8D,$89,$D6
		.BYTE	$D1,$DC,$49,$74,$3F,$4F,$5F,$81,$DD,$D1,$E1,$F1,$8E,$8A,$D6
		.BYTE	$D1,$DC,$54,$74,$6F,$7F,$8F,$82,$DD,$D2,$E2,$F2,$8F,$8B,$D6
		.BYTE	$D1,$DC,$59,$74,$DD,$DD,$DD,$83,$DD,$DD,$DD,$DD,$90,$DD,$D6
		.BYTE	$D1,$DC,$DC,$74,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D1,$DC,$4D,$74,$DD,$DD,$DD,$84,$DD,$DD,$DD,$DD,$91,$DD,$D6
		.BYTE	$D1,$DC,$41,$74,$9F,$AF,$BF,$85,$89,$D3,$E3,$F3,$92,$89,$D6
		.BYTE	$D1,$DC,$50,$74,$CC,$CD,$CE,$86,$8A,$D4,$E4,$F4,$93,$8A,$D6
		.BYTE	$D1,$DC,$DC,$74,$CF,$EF,$FF,$87,$8B,$D5,$E5,$F5,$94,$8B,$D6
		.BYTE	$D1,$DC,$DC,$74,$DD,$DD,$DD,$88,$DD,$DD,$DD,$DD,$95,$DD,$D6
		.BYTE	$D1,$DC,$DC,$74,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$DD,$D6
		.BYTE	$D2,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D7
_wnd_nt0A:
		.BYTE	$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5
		.BYTE	$D1,$20,$20,$74,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$E5,$E8,$EB,$DC,$96,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$E6,$E9,$EC,$DC,$97,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$E7,$EA,$ED,$DC,$98,$DC,$D6
		.BYTE	$D1,$20,$50,$74,$DC,$DC,$DC,$DC,$99,$DC,$D6
		.BYTE	$D1,$20,$4F,$74,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$20,$52,$74,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D1,$20,$54,$74,$00,$04,$08,$0C,$9A,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$01,$05,$09,$0D,$9B,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$02,$06,$0A,$0E,$98,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$03,$07,$0B,$0F,$99,$DC,$D6
		.BYTE	$D1,$20,$20,$74,$DC,$DC,$DC,$DC,$DC,$DC,$D6
		.BYTE	$D2,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D7
_wnd_nt0B:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$20,$44,$6F,$20,$79,$6F,$75,$20,$77,$61,$6E,$74,$20,$20,$D4
		.BYTE	$D3,$20,$74,$6F,$20,$73,$61,$76,$65,$3F,$20,$20,$20,$20,$20,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$DC,$E8,$E9,$EA,$EB,$DC,$DC,$EC,$ED,$EE,$EF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$F8,$F9,$FA,$FB,$DC,$DC,$FC,$FD,$FE,$FF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_extnt0B:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$81
		.BYTE	$81,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$81
		.BYTE	$81,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$06,$41,$41,$06,$06,$06,$06,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$06,$41,$41,$06,$06,$06,$06,$41,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_nt10:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$20,$44,$6F,$20,$79,$6F,$75,$20,$77,$61,$6E,$74,$20,$20,$D4
		.BYTE	$D3,$20,$74,$6F,$20,$6C,$6F,$61,$64,$3F,$20,$20,$20,$20,$20,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$DC,$E8,$E9,$EA,$EB,$DC,$DC,$EC,$ED,$EE,$EF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$F8,$F9,$FA,$FB,$DC,$DC,$FC,$FD,$FE,$FF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_nt11:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$20,$44,$6F,$20,$79,$6F,$75,$20,$71,$75,$69,$74,$20,$20,$D4
		.BYTE	$D3,$20,$70,$6C,$61,$79,$69,$6E,$67,$20,$77,$69,$74,$68,$2D,$D4
		.BYTE	$D3,$20,$6F,$75,$74,$20,$73,$61,$76,$69,$6E,$67,$3F,$20,$20,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$DC,$E8,$E9,$EA,$EB,$DC,$DC,$EC,$ED,$EE,$EF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$F8,$F9,$FA,$FB,$DC,$DC,$FC,$FD,$FE,$FF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_extnt11:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$81
		.BYTE	$81,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$81
		.BYTE	$81,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$81
		.BYTE	$81,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$06,$41,$41,$06,$06,$06,$06,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$06,$41,$41,$06,$06,$06,$06,$41,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_nt12:
		.BYTE	$D0,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D3,$20,$44,$6F,$20,$79,$6F,$75,$20,$77,$61,$6E,$74,$20,$20,$D4
		.BYTE	$D3,$20,$74,$6F,$20,$73,$74,$61,$72,$74,$20,$6E,$65,$77,$20,$D4
		.BYTE	$D3,$20,$63,$69,$74,$79,$3F,$20,$20,$20,$20,$20,$20,$20,$20,$D4
		.BYTE	$D3,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$74,$D4
		.BYTE	$D3,$DC,$DC,$E8,$E9,$EA,$EB,$DC,$DC,$EC,$ED,$EE,$EF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$F8,$F9,$FA,$FB,$DC,$DC,$FC,$FD,$FE,$FF,$DC,$DC,$D4
		.BYTE	$D3,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$D4
		.BYTE	$D5,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D6,$D7
_wnd_extnt00:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$41,$41,$41,$4A,$4A,$4A,$4A,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$41,$41,$09,$09,$09,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$41,$41,$41,$4A,$4A,$4A,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt01:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$49,$49,$09,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$49,$49,$09,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$49,$49,$09,$41,$41,$41,$89,$89,$89,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$41,$41,$41,$4A,$4A,$4A,$41,$81
		.BYTE	$81,$4A,$4A,$4A,$4A,$4A,$41,$41,$4A,$4A,$4A,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$09,$09,$09,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$09,$09,$09,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$09,$09,$09,$41,$41,$41,$49,$49,$49,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$41,$41,$4A,$4A,$4A,$4A,$4A,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt02:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$89,$89,$89,$41,$09,$09,$09,$41,$89,$89,$89,$41,$81
		.BYTE	$81,$41,$89,$89,$89,$41,$09,$09,$09,$41,$89,$89,$89,$41,$81
		.BYTE	$81,$41,$89,$89,$89,$41,$09,$09,$09,$41,$89,$89,$89,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$41,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$49,$49,$49,$41,$09,$09,$09,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$49,$49,$49,$41,$09,$09,$09,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$89,$89,$89,$41,$09,$09,$09,$41,$81
		.BYTE	$81,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$41,$4A,$4A,$4A,$4A,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$4A,$4A,$4A,$41,$4A,$4A,$4A,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt03:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$09,$09,$09,$41,$49,$49,$49,$41,$89,$89,$89,$41,$81
		.BYTE	$81,$41,$09,$09,$09,$41,$49,$49,$49,$41,$89,$89,$89,$41,$81
		.BYTE	$81,$41,$09,$09,$09,$41,$49,$49,$49,$41,$09,$09,$09,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$89,$89,$89,$41,$4B,$4B,$4B,$41,$0B,$0B,$0B,$41,$81
		.BYTE	$81,$41,$89,$89,$89,$41,$4B,$4B,$4B,$41,$0B,$0B,$0B,$41,$81
		.BYTE	$81,$41,$49,$49,$49,$41,$4B,$4B,$4B,$41,$0B,$0B,$0B,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$4B,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt04:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$98,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$41,$89,$89,$89,$41,$41,$09,$09,$09,$41,$41,$81
		.BYTE	$81,$41,$41,$89,$89,$89,$41,$41,$09,$09,$09,$41,$41,$81
		.BYTE	$81,$41,$41,$89,$89,$89,$41,$41,$09,$09,$09,$41,$41,$81
		.BYTE	$81,$41,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$4A,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$41,$41,$09,$09,$49,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$41,$41,$09,$09,$49,$41,$41,$81
		.BYTE	$81,$41,$41,$06,$06,$06,$41,$41,$49,$49,$49,$41,$41,$81
		.BYTE	$81,$41,$46,$46,$46,$46,$46,$41,$4A,$4A,$4A,$41,$41,$81
		.BYTE	$81,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt06:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$0B,$0B,$41,$87,$87,$87,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$47,$47,$87,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$47,$87,$87,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$87,$87,$87,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$87,$87,$87,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$87,$87,$87,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$4A,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt07:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$49,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$4A,$81
		.BYTE	$81,$01,$01,$41,$49,$49,$49,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt08:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$01,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$89,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt09:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$01,$01,$41,$49,$49,$49,$49,$49,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$01,$41,$49,$49,$49,$4A,$49,$49,$49,$49,$4A,$49,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$49,$4A,$49,$0A,$0A,$CA,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$49,$4A,$49,$0A,$0A,$CA,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$49,$4A,$49,$0A,$0A,$CA,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$49,$49,$49,$49,$4A,$49,$81
		.BYTE	$81,$01,$01,$41,$49,$49,$49,$49,$49,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$01,$0B,$41,$49,$49,$49,$4A,$49,$49,$49,$49,$4A,$49,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$C9,$4A,$4A,$4A,$4A,$CA,$4A,$4A,$81
		.BYTE	$81,$01,$0B,$41,$89,$89,$C9,$4A,$4A,$4A,$4A,$CA,$4A,$4A,$81
		.BYTE	$81,$01,$01,$41,$89,$89,$C9,$4A,$4A,$4A,$4A,$CA,$4A,$4A,$81
		.BYTE	$81,$01,$01,$41,$49,$49,$49,$4A,$49,$49,$49,$49,$4A,$49,$81
		.BYTE	$81,$01,$01,$41,$49,$49,$49,$49,$49,$49,$49,$49,$49,$49,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
_wnd_extnt0A:
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$0B,$0B,$41,$00,$00,$00,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$00,$00,$00,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$00,$00,$00,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$0B,$0B,$41,$01,$01,$81,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$01,$01,$81,$41,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$01,$01,$81,$81,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$01,$01,$01,$01,$4A,$41,$81
		.BYTE	$81,$0B,$0B,$41,$41,$41,$41,$41,$41,$41,$81
		.BYTE	$81,$81,$81,$81,$81,$81,$81,$81,$81,$81,$81

		SECTION_START 102

_spr90:
		.BYTE	$F0,$C0,$03,$F0
		.BYTE	$F0,$C1,$03,$F8
		.BYTE	$F0,$C0,$43,$00
		.BYTE	$F8,$C2,$03,$F0
		.BYTE	$F8,$C2,$43,$00
		.BYTE	$00,$C0,$83,$F0
		.BYTE	$00,$C1,$83,$F8
		.BYTE	$00,$C0,$C3,$00
		.BYTE	$80
_spr91:
		.BYTE	$F0,$C3,$03,$F0
		.BYTE	$F0,$C4,$03,$F8
		.BYTE	$F0,$C3,$43,$00
		.BYTE	$F8,$C5,$03,$F0
		.BYTE	$F8,$C5,$43,$00
		.BYTE	$00,$C3,$83,$F0
		.BYTE	$00,$C4,$83,$F8
		.BYTE	$00,$C3,$C3,$00
		.BYTE	$80
_spr92:
		.BYTE	$F0,$C6,$03,$F0
		.BYTE	$F0,$C7,$03,$F8
		.BYTE	$F0,$C6,$43,$00
		.BYTE	$F8,$C8,$03,$F0
		.BYTE	$F8,$C8,$43,$00
		.BYTE	$00,$C6,$83,$F0
		.BYTE	$00,$C7,$83,$F8
		.BYTE	$00,$C6,$C3,$00
		.BYTE	$80
_spr93:
		.BYTE	$F0,$C9,$03,$F0
		.BYTE	$F0,$CA,$03,$F8
		.BYTE	$F0,$C9,$43,$00
		.BYTE	$F8,$CB,$03,$F0
		.BYTE	$F8,$CB,$43,$00
		.BYTE	$00,$C9,$83,$F0
		.BYTE	$00,$CA,$83,$F8
		.BYTE	$00,$C9,$C3,$00
		.BYTE	$80
_spr94:
		.BYTE	$F0,$CC,$03,$F0
		.BYTE	$F0,$CD,$03,$F8
		.BYTE	$F0,$CC,$43,$00
		.BYTE	$F8,$CE,$03,$F0
		.BYTE	$F8,$CE,$43,$00
		.BYTE	$00,$CC,$83,$F0
		.BYTE	$00,$CD,$83,$F8
		.BYTE	$00,$CC,$C3,$00
		.BYTE	$80
_spr95:
		.BYTE	$F0,$CF,$03,$F0
		.BYTE	$F0,$D0,$03,$F8
		.BYTE	$F0,$CF,$43,$00
		.BYTE	$F8,$D1,$03,$F0
		.BYTE	$F8,$D1,$43,$00
		.BYTE	$00,$CF,$83,$F0
		.BYTE	$00,$D0,$83,$F8
		.BYTE	$00,$CF,$C3,$00
		.BYTE	$80
_spr96:
		.BYTE	$F0,$D2,$03,$F0
		.BYTE	$F0,$D3,$03,$F8
		.BYTE	$F0,$D2,$43,$00
		.BYTE	$F8,$D4,$03,$F0
		.BYTE	$F8,$D4,$43,$00
		.BYTE	$00,$D2,$83,$F0
		.BYTE	$00,$D3,$83,$F8
		.BYTE	$00,$D2,$C3,$00
		.BYTE	$80
_spr97:
		.BYTE	$F0,$D5,$03,$F0
		.BYTE	$F0,$D6,$03,$F8
		.BYTE	$F0,$D5,$43,$00
		.BYTE	$F8,$D7,$03,$F0
		.BYTE	$F8,$D7,$43,$00
		.BYTE	$00,$D5,$83,$F0
		.BYTE	$00,$D6,$83,$F8
		.BYTE	$00,$D5,$C3,$00
		.BYTE	$80
_spr98:
		.BYTE	$F0,$DA,$03,$F0
		.BYTE	$F0,$DA,$03,$F8
		.BYTE	$F0,$DA,$43,$00
		.BYTE	$F8,$DA,$03,$F0
		.BYTE	$F8,$D8,$03,$F8
		.BYTE	$F8,$DA,$43,$00
		.BYTE	$00,$DA,$83,$F0
		.BYTE	$00,$DA,$83,$F8
		.BYTE	$00,$DA,$C3,$00
		.BYTE	$80
_spr99:
		.BYTE	$F0,$DA,$03,$F0
		.BYTE	$F0,$DA,$03,$F8
		.BYTE	$F0,$DA,$43,$00
		.BYTE	$F8,$DA,$03,$F0
		.BYTE	$F8,$D9,$03,$F8
		.BYTE	$F8,$DA,$43,$00
		.BYTE	$00,$DA,$83,$F0
		.BYTE	$00,$DA,$83,$F8
		.BYTE	$00,$DA,$C3,$00
		.BYTE	$80
_spr9A:
		.BYTE	$F8,$E0,$03,$F8
		.BYTE	$F8,$E1,$03,$00
		.BYTE	$00,$F0,$03,$F8
		.BYTE	$00,$F1,$03,$00
		.BYTE	$80
_spr9B:
		.BYTE	$F8,$E2,$03,$F8
		.BYTE	$F8,$E3,$03,$00
		.BYTE	$00,$F2,$03,$F8
		.BYTE	$00,$F3,$03,$00
		.BYTE	$80
_spr9C:
		.BYTE	$F8,$E4,$03,$F8
		.BYTE	$F8,$E5,$03,$00
		.BYTE	$00,$F4,$03,$F8
		.BYTE	$00,$F5,$03,$00
		.BYTE	$80
_spr9D:
		.BYTE	$F8,$D9,$03,$F8
		.BYTE	$80
_spr9E:
		.BYTE	$F8,$E6,$03,$F8
		.BYTE	$F8,$E7,$03,$00
		.BYTE	$00,$F6,$03,$F8
		.BYTE	$00,$F7,$03,$00
		.BYTE	$80
_spr9F:
		.BYTE	$F8,$E8,$03,$F8
		.BYTE	$F8,$E9,$03,$00
		.BYTE	$00,$F8,$03,$F8
		.BYTE	$00,$F9,$03,$00
		.BYTE	$80
_sprA1:
		.BYTE	$F8,$EA,$03,$F8
		.BYTE	$F8,$EB,$03,$00
		.BYTE	$00,$FA,$03,$F8
		.BYTE	$00,$FB,$03,$00
		.BYTE	$80
_spr0C:
		.BYTE	$00,$D0,$00,$00
		.BYTE	$00,$D1,$00,$08
		.BYTE	$00,$D2,$00,$10
		.BYTE	$00,$D3,$00,$18
		.BYTE	$08,$D4,$00,$00
		.BYTE	$08,$D5,$00,$08
		.BYTE	$08,$D6,$00,$10
		.BYTE	$08,$D7,$00,$18
		.BYTE	$80
_spr0D:
		.BYTE	$00,$D8,$00,$00
		.BYTE	$00,$D9,$00,$08
		.BYTE	$00,$DA,$00,$10
		.BYTE	$00,$DB,$00,$18
		.BYTE	$08,$DC,$00,$00
		.BYTE	$08,$DD,$00,$08
		.BYTE	$08,$DE,$00,$10
		.BYTE	$08,$DF,$00,$18
		.BYTE	$80
_spr0E:
		.BYTE	$00,$CA,$00,$00
		.BYTE	$00,$CB,$00,$08
		.BYTE	$00,$CB,$00,$10
		.BYTE	$00,$CB,$00,$18
		.BYTE	$00,$CB,$00,$20
		.BYTE	$00,$CC,$00,$28
		.BYTE	$08,$CD,$00,$00
		.BYTE	$08,$CD,$00,$26
		.BYTE	$10,$CD,$00,$00
		.BYTE	$10,$CD,$00,$26
		.BYTE	$18,$CE,$00,$00
		.BYTE	$16,$CB,$00,$08
		.BYTE	$16,$CB,$00,$10
		.BYTE	$16,$CB,$00,$18
		.BYTE	$16,$CB,$00,$20
		.BYTE	$18,$CF,$00,$28
		.BYTE	$80
_sprA2:
		.BYTE	$00,$F0,$00,$08
		.BYTE	$00,$F1,$00,$10
		.BYTE	$08,$F2,$00,$08
		.BYTE	$08,$F3,$00,$10
		.BYTE	$08,$F4,$00,$18
		.BYTE	$10,$F8,$00,$00
		.BYTE	$10,$F9,$00,$08
		.BYTE	$18,$FE,$00,$10
		.BYTE	$80
_sprA3:
		.BYTE	$F3,$FD,$00,$F4
		.BYTE	$80
_sprA4:
		.BYTE	$F0,$FC,$00,$F0
		.BYTE	$F2,$FD,$00,$F3
		.BYTE	$80
_sprA5:
		.BYTE	$EB,$FB,$00,$EB
		.BYTE	$EF,$FC,$00,$EF
		.BYTE	$80
_sprA6:
		.BYTE	$E5,$FA,$00,$E8
		.BYTE	$EB,$FB,$00,$EB
		.BYTE	$80
_sprA7:
		.BYTE	$E2,$FA,$00,$E6
		.BYTE	$80
_sprA8:
		.BYTE	$E4,$EF,$00,$E8
		.BYTE	$80

		SECTION_END 102

		BANK_END F2,$C000
