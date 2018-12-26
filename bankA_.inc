
; ---------------------------------------------------------------------------
; ---------E-X-T-R-A--M-U-S-I-C--A-N-D--S-E--C-O-D-E--A-N-D--D-A-T-A---------
; ---------------------------------------------------------------------------

		SECTION_START	01

; =============== S U B	R O U T	I N E =======================================
_apu_se8_start:
		STY		_apu_se_loop_flags
		LDA		#$0A
		STA		byte_F8
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$88
		STA		_APU_PULSE2_ENV
		LDA		#$D7
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se8_loop:
		LDY		byte_F8
		LDA		byte_195834-1,Y
		JSR		_apu_tri_note_load
		LDY		byte_F8
		LDA		byte_195834,Y
		JSR		_apu_pulse1_note_load
		DEC		byte_F8
		BNE		locret_195833
		LDA		#$00
		STA		_apu_se_loop_flags
		JSR		_apu_status_reset
locret_195833:
		RTS
byte_195834:
		.BYTE	$02,$3E,$3A,$36,$46,$3C,$02,$46,$46,$3C,$34,$34

; =============== S U B	R O U T	I N E =======================================
_apu_se80_start:
		JSR		_apu_status_reset
		STY		_apu_se_loop_flags
		LDA		byte_FD
		BNE		loc_19584D
		LDA		#$14
		STA		byte_FD
loc_19584D:
		LDA		#$C6
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se80_loop:
		LDA		byte_FD
		ASL
		ASL
		ASL
		ASL
		BCS		loc_195861
		EOR		#$F0
loc_195861:
		STA		_APU_PULSE2_TIME
		LDA		#$08
		STA		_APU_PULSE2_LEN
		DEC		byte_FD
		DEC		byte_FD
		BNE		locret_195876
		LDA		#$00
		STA		_apu_se_loop_flags
		JSR		_apu_status_reset
locret_195876:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se20_start:
		JSR		_apu_tri_chnl_with_dpcm
		STY		_apu_se_loop_flags
		LDA		#$14
		STA		byte_F8

; =============== S U B	R O U T	I N E =======================================
_apu_se20_loop:
		LDA		#$07
		STA		_APU_TRI_COUNT
		LDY		byte_F8
		LDA		byte_195899-1,Y
		JSR		_apu_tri_note_load
		DEC		byte_F8
		BNE		locret_195898
		LDA		#$00
		STA		_apu_se_loop_flags
		JSR		_apu_status_reset
locret_195898:
		RTS
byte_195899:
		.BYTE	$02,$68,$68,$68,$02,$18,$18,$18,$02,$0E,$0E,$0E,$02,$08,$08,$08
		.BYTE	$02,$04,$04,$04

; =============== S U B	R O U T	I N E =======================================
_apu_se40_start:
		JSR		_apu_tri_chnl_with_dpcm
		STY		_apu_se_loop_flags
		LDA		#$14
		STA		byte_F8
		LDA		#$07
		STA		_APU_TRI_COUNT

; =============== S U B	R O U T	I N E =======================================
_apu_se40_loop:
		LDY		byte_F8
		LDA		byte_1958CF-1,Y
		JSR		_apu_tri_note_load
		DEC		byte_F8
		BNE		locret_1958CE
		LDA		#$00
		STA		_apu_se_loop_flags
		JSR		_apu_status_reset
locret_1958CE:
		RTS
byte_1958CF:
		.BYTE	$02,$16,$16,$16,$02,$1A,$1A,$1A,$02,$6C,$6C,$6C,$02,$74,$74,$74
		.BYTE	$02,$76,$76,$76

; =============== S U B	R O U T	I N E =======================================
_apu_play_se1:
		LDY		_apu_se_flags+1
		TYA
		CMP		#$30
		BEQ		_j_apu_se30_start
		LDA		_apu_se_loop_flags+1
		CMP		#$30
		BEQ		_j_apu_se30_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se100_start
		LSR
		BCS		_j_apu_se100_loop
		LSR		_apu_se_flags+1
		BCS		_apu_se200_start
		LSR
		BCS		_apu_se200_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se400_start
		LSR
		BCS		_j_apu_se400_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se800_start
		LSR
		BCS		_j_apu_se800_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se1000_start
		LSR
		BCS		_j_apu_se1000_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se2000_start
		LSR
		BCS		_j_apu_se2000_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se4000_start
		LSR
		BCS		_j_apu_se4000_loop
		LSR		_apu_se_flags+1
		BCS		_j_apu_se8000_start
		LSR
		BCS		_j_apu_se8000_loop
		RTS

; =============== S U B	R O U T	I N E =======================================
_j_apu_se100_start:
		JMP		_apu_se100_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se100_loop:
		JMP		_apu_se100_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se400_start:
		JMP		_apu_se400_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se400_loop:
		JMP		_apu_se400_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se800_start:
		JMP		_apu_se800_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se800_loop:
		JMP		_apu_se800_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se1000_start:
		JMP		_apu_se1000_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se1000_loop:
		JMP		_apu_se1000_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se2000_start:
		JMP		_apu_se2000_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se2000_loop:
		JMP		_apu_se2000_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se4000_start:
		JMP		_apu_se4000_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se4000_loop:
		JMP		_apu_se4000_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se8000_start:
		JMP		_apu_se8000_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se8000_loop:
		JMP		_apu_se8000_loop

; =============== S U B	R O U T	I N E =======================================
_j_apu_se30_start:
		JMP		_apu_se30_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se30_loop:
		JMP		_apu_se30_loop

; =============== S U B	R O U T	I N E =======================================
_apu_se200_start:
		STY		_apu_se_loop_flags+1
		JSR		_apu_tri_chnl_no_dpcm
		LDA		#$00
		STA		_APU_DMC_ADDR
		LDA		#$FF
		STA		_APU_DMC_LEN
		LDA		#$0C
		STA		_APU_DMC_FREQ
		LDA		_APU_STATUS
		AND		#$09
		LDX		_apu_idx
		BMI		loc_195978
		ORA		#$10
loc_195978:
		STA		_APU_STATUS

; =============== S U B	R O U T	I N E =======================================
_apu_se200_loop:
		LDA		_APU_STATUS
		AND		#$10
		BNE		locret_19598E
		ORA		#$0F
		STA		_APU_STATUS
		JSR		_apu_tri_chnl_no_dpcm
		LDA		#$00
		STA		_apu_se_loop_flags+1
locret_19598E:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se100_start:
		STY		_apu_se_loop_flags+1
		JSR		_apu_tri_chnl_no_dpcm
		LDA		#$40
		STA		_APU_DMC_ADDR
		LDA		#$DF
		STA		_APU_DMC_LEN
		LDA		#$0D
		STA		_APU_DMC_FREQ
		LDA		_APU_STATUS
		AND		#$09
		LDX		_apu_idx
		BMI		loc_1959AE
		ORA		#$19
loc_1959AE:
		STA		_APU_STATUS

; =============== S U B	R O U T	I N E =======================================
_apu_se100_loop:
		LDA		_APU_STATUS
		AND		#$10
		BNE		locret_1959C4
		ORA		#$0F
		STA		_APU_STATUS
		JSR		_apu_tri_chnl_no_dpcm
		LDA		#$00
		STA		_apu_se_loop_flags+1
locret_1959C4:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se400_start:
		STY		_apu_se_loop_flags+1
		LDA		#$7A
		STA		_apu_se_var0
		JSR		_apu_tri_chnl_with_dpcm

; =============== S U B	R O U T	I N E =======================================
_apu_se400_loop:
		LDY		_apu_se_var0
		LDA		byte_1959FA-1,Y
		ORA		#$0C
		STA		_APU_NOISE_LOOP
		LDA		_apu_se_var0
		LSR
		LSR
		LSR
		AND		#$1F
		ORA		#$18
		STA		_APU_NOISE_ENV
		LDA		#$18
		STA		_APU_NOISE_LEN
		DEC		_apu_se_var0
		BNE		locret_1959F9
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$00
		STA		_apu_se_loop_flags+1
		LDA		#$10
		STA		_APU_NOISE_ENV
locret_1959F9:
		RTS
byte_1959FA:
		.BYTE	$10,$63,$63,$93,$74,$B5,$A6,$97,$87,$78,$68,$59,$59,$4A,$4A,$3B
		.BYTE	$3C,$3D,$2D,$2E,$2F,$1F,$2E,$2D,$2C,$2B,$3B,$3A,$49,$48,$58,$67
		.BYTE	$77,$86,$95,$A5,$B4,$54,$54,$63,$63

; =============== S U B	R O U T	I N E =======================================
_apu_se30_start:
		STY		_apu_se_loop_flags+1
		LDA		#$A0
		STA		_apu_se_var0

; =============== S U B	R O U T	I N E =======================================
_apu_se30_loop:
		LDA		_apu_se_var0
		LSR
		LSR
		TAY
		LDA		byte_1959FA,Y
		ORA		#$10
		STA		_APU_NOISE_ENV
		LSR
		LSR
		LSR
		LSR
		STA		_APU_NOISE_LOOP
		LDA		#$18
		STA		_APU_NOISE_LEN
		DEC		_apu_se_var0
		BNE		locret_195A4A
		LDA		#$00
		STA		_apu_se_loop_flags+1
locret_195A4A:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se800_start:
		STY		_apu_se_loop_flags+1
		LDA		#$0A
		STA		_apu_se_var0
		JSR		_apu_tri_chnl_with_dpcm

; =============== S U B	R O U T	I N E =======================================
_apu_se800_loop:
		LDY		_apu_se_var0
		LDA		byte_195A73-1,Y
		LDX		byte_195A7D-1,Y
		STA		_APU_NOISE_ENV
		STX		_APU_NOISE_LOOP
		LDA		#$08
		STA		_APU_NOISE_LEN
		DEC		_apu_se_var0
		BNE		locret_195A72
		JSR		_apu_status_reset
		LDA		#$00
		STA		_apu_se_loop_flags+1
locret_195A72:
		RTS
byte_195A73:
		.BYTE	$01,$02,$00,$03,$01,$00,$03,$01,$00,$03
byte_195A7D:
		.BYTE	$0F,$0F,$0E,$0E,$0F,$0F,$0F,$0F,$0F,$8F

; =============== S U B	R O U T	I N E =======================================
_apu_se1000_start:
		JSR		_apu_tri_chnl_with_dpcm
		STY		_apu_se_loop_flags+1
		LDA		#$40
		STA		_apu_se_var0
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se1000_loop:
		LDA		byte_F8
		BNE		loc_195AB1
		LDY		_apu_se_var0
		LDA		byte_195ABD-1,Y
		STA		_APU_TRI_COUNT
		LDA		byte_195AFD-1,Y
		STA		_APU_PULSE2_ENV
		LDA		#$28
		JSR		_apu_tri_note_load
		LDA		#$38
		JSR		_apu_pulse1_note_load
loc_195AB1:
		DEC		_apu_se_var0
		BNE		locret_195ABC
		LDA		#$00
		STA		_apu_se_loop_flags+1
		JSR		_apu_status_reset
locret_195ABC:
		RTS
byte_195ABD:
		.BYTE	$01,$41,$41,$41,$42,$42,$43,$44,$43,$44,$45,$44,$45,$46,$46,$46
		.BYTE	$45,$45,$44,$44,$43,$42,$41,$41,$01,$41,$41,$41,$41,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$41,$41,$41,$41,$41,$42,$41,$42,$43,$42,$43,$44
		.BYTE	$43,$44,$45,$44,$45,$46,$46,$46,$45,$45,$44,$44,$43,$42,$00,$41
byte_195AFD:
		.BYTE	$D8,$D8,$D8,$D8,$D7,$D7,$D7,$D7,$D6,$D6,$D6,$D6,$D6,$D5,$D5,$D5
		.BYTE	$D5,$D4,$D4,$D4,$D4,$D3,$D2,$D1,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
		.BYTE	$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D8,$D8,$D8,$D8,$D7,$D7,$D7,$D7
		.BYTE	$D6,$D6,$D6,$D6,$D6,$D5,$D5,$D5,$D5,$D4,$D4,$D4,$D4,$D3,$D2,$D1

; =============== S U B	R O U T	I N E =======================================
_apu_se2000_start:
		STY		_apu_se_loop_flags+1
		LDA		#$14
		STA		_apu_se_var0
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$83
		STA		_APU_PULSE2_ENV
		LDA		#$9D
		STA		_APU_PULSE2_SWEEP
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$FE
		STA		_APU_PULSE2_TIME
		STA		_APU_TRI_TIME
		LDA		#$08
		STA		_APU_PULSE2_LEN
		STA		_APU_TRI_LEN

; =============== S U B	R O U T	I N E =======================================
_apu_se2000_loop:
		DEC		_apu_se_var0
		BNE		locret_195B70
		LDA		#$00
		STA		_apu_se_loop_flags+1
		JSR		_apu_status_reset
locret_195B70:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se4000_start:
		STY		_apu_se_loop_flags+1
		LDA		#$0A
		STA		_apu_se_var0
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$83
		STA		_APU_PULSE2_ENV
		LDA		#$9D
		STA		_APU_PULSE2_SWEEP
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$FE
		STA		_APU_PULSE2_TIME
		STA		_APU_TRI_TIME
		LDA		#$09
		STA		_APU_PULSE2_LEN
		STA		_APU_TRI_LEN

; =============== S U B	R O U T	I N E =======================================
_apu_se4000_loop:
		DEC		_apu_se_var0
		BNE		locret_195BA4
		LDA		#$00
		STA		_apu_se_loop_flags+1
		JSR		_apu_status_reset
locret_195BA4:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se8000_start:
		STY		_apu_se_loop_flags+1
		LDA		#$15
		STA		_apu_se_var0
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$83
		STA		_APU_PULSE2_ENV
		LDA		#$A1
		STA		_APU_PULSE2_SWEEP
		LDA		#$64
		JSR		_apu_tri_note_load
		LDA		#$64
		JSR		_apu_pulse1_note_load

; =============== S U B	R O U T	I N E =======================================
_apu_se8000_loop:
		DEC		_apu_se_var0
		BNE		locret_195BD2
		JSR		_apu_status_reset
		LDA		#$00
		STA		_apu_se_loop_flags+1
locret_195BD2:
		RTS

byte_195C00:
		.BYTE	$D0,$D0,$D0,$D1,$D1,$D1,$D1,$D2,$D2,$D2,$D2,$D3,$D3,$D3,$D4,$D5
		.BYTE	$D6,$D7,$D6,$D6,$D6,$D5,$D5,$D4,$90,$90,$90,$91,$91,$91,$92,$92
		.BYTE	$92,$92,$92,$93,$93,$93,$94,$95,$96,$97,$96,$96,$96,$95,$95,$94
		.BYTE	$50,$50,$50,$50,$50,$51,$52,$52,$52,$52,$52,$52,$53,$53,$53,$53
		.BYTE	$54,$95,$56,$56,$57,$97,$D7,$17,$D0,$D0,$D1,$D1,$D2,$D2,$D2,$D2
		.BYTE	$D2,$D3,$D3,$D3,$D3,$D4,$D3,$D2,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1
		.BYTE	$91,$91,$91,$91,$91,$91,$91,$91,$91,$92,$92,$92,$92,$93,$93,$94
		.BYTE	$95,$96,$97,$97,$96,$94,$93,$91,$50,$50,$50,$51,$51,$51,$51,$51
		.BYTE	$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$52,$53,$54,$55,$56,$57
		.BYTE	$90,$90,$90,$91,$91,$91,$92,$92,$92,$93,$93,$93,$93,$94,$93,$92
		.BYTE	$91,$91,$91,$91,$91,$91,$91,$91
byte_195CA8:
		.BYTE	$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
		.BYTE	$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2,$D2
		.BYTE	$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2,$D2
		.BYTE	$D2,$D3,$D3,$D4,$D5,$D5,$D6,$D6,$D7,$D7,$D7,$D7,$D7,$D7,$D7,$D7
		.BYTE	$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90
		.BYTE	$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90
		.BYTE	$91,$91,$91,$91,$91,$91,$91,$91,$91,$92,$92,$92,$92,$93,$93,$93
		.BYTE	$94,$94,$94,$94,$95,$95,$95,$95,$95,$96,$96,$97,$97,$97,$96,$94
		.BYTE	$50,$50,$50,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51
		.BYTE	$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51,$51
		.BYTE	$51,$51,$51,$51,$51,$51,$52,$52,$52,$52,$52,$53,$53,$53,$53,$53
		.BYTE	$53,$54,$54,$54,$54,$94,$D4,$15,$15,$56,$16,$57,$57,$97,$D6,$14
		.BYTE	$F0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0,$D0
		.BYTE	$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D2,$D2,$D3,$D3,$D3,$D3,$D4,$D4
		.BYTE	$D3,$D3,$D3,$D2,$D2,$D2,$D2,$D2,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1
		.BYTE	$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1,$D1
byte_195DA8:
		.BYTE	$B2,$92,$92,$92,$92,$92,$92,$92,$92,$92,$92,$92,$92,$92,$92,$92
		.BYTE	$92,$92,$92,$92,$92,$92,$92,$92,$93,$93,$93,$93,$93,$93,$93,$93
		.BYTE	$93,$93,$94,$94,$94,$94,$94,$95,$95,$95,$95,$96,$96,$96,$96,$96
		.BYTE	$96,$97,$97,$97,$97,$97,$97,$97,$97,$97,$97,$96,$95,$94,$92,$91
		.BYTE	$50,$50,$50,$51,$51,$51,$51,$51,$51,$51,$51,$51,$52,$52,$52,$53
		.BYTE	$50,$50,$51,$51,$51,$51,$52,$52,$52,$52,$53,$53,$53,$54,$54,$55
		.BYTE	$50,$51,$51,$52,$52,$52,$53,$53,$53,$54,$54,$55,$55,$56,$56,$57
		.BYTE	$51,$52,$52,$52,$53,$53,$54,$55,$55,$55,$56,$56,$57,$58,$59,$5A
		.BYTE	$B0,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90,$90
		.BYTE	$91,$91,$91,$91,$91,$91,$91,$91,$92,$92,$93,$93,$93,$93,$94,$94
		.BYTE	$93,$93,$93,$92,$92,$92,$92,$92,$91,$91,$91,$91,$91,$91,$91,$91
		.BYTE	$91,$91,$91,$91,$91,$91,$91,$91,$91,$91,$91,$91,$91,$91,$90,$90

_apu_pulse_len_tbl:
		.WORD	$F007,$0000,$D400,$C800,$BD00,$B200,$A800,$9F00
		.WORD	$9600,$8D00,$8500,$7E00,$7600,$7000,$AB01,$9301
		.WORD	$7C01,$6701,$5201,$3F01,$2D01,$1C01,$0C01,$FD00
		.WORD	$EE00,$E100,$5703,$2703,$F902,$CF02,$A602,$8002
		.WORD	$5C02,$3A02,$1A02,$FC01,$DF01,$C401,$AE06,$4E06
		.WORD	$F305,$9E05,$4D05,$0105,$B904,$7504,$3504,$F803
		.WORD	$BF03,$8903,$6900,$6300,$5E00,$5800,$5300,$4F00
		.WORD	$4A00,$4600,$4200,$3E00
byte_195F78:
		.BYTE	$03,$06,$0C,$18,$30,$12,$24,$09,$01
		.BYTE	$04,$08,$10,$20,$40,$18,$30,$0C,$00
		.BYTE	$05,$0A,$14,$28,$50,$1E,$3C,$0F,$08
		.BYTE	$06,$0C,$18,$30,$60,$24,$48,$12,$08
		.BYTE	$07,$0E,$1C,$38,$70,$2A,$54,$15,$0C
		.BYTE	$09,$12,$24,$48,$90,$36,$6C,$1B,$03
		.BYTE	$0A,$14,$28,$50,$A0,$3C,$78,$1E,$11

; =============== S U B	R O U T	I N E =======================================
_apu_tri_chnl_with_dpcm:
		LDA		_APU_STATUS
		AND		#$19
		STA		_APU_STATUS
		ORA		#$0F
		STA		_APU_STATUS
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$FF
		STA		_APU_TRI_TIME
		LDA		#$0F
		STA		_APU_TRI_LEN
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_tri_chnl_no_dpcm:
		LDA		_APU_STATUS
		AND		#$0F
		ORA		#$0F
		STA		_APU_STATUS
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$FF
		STA		_APU_TRI_TIME
		LDA		#$0F
		STA		_APU_TRI_LEN
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_status_reset:
		LDA		_APU_STATUS
		AND		#$1D
		STA		_APU_STATUS
		ORA		#$0F
		STA		_APU_STATUS
		RTS

		SECTION_END		01
