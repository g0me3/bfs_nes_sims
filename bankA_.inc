
; ---------------------------------------------------------------------------
; ---------E-X-T-R-A--M-U-S-I-C--A-N-D--S-E--C-O-D-E--A-N-D--D-A-T-A---------
; ---------------------------------------------------------------------------

		SECTION_START	01

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
_j_apu_se30_start:
		JMP		_apu_se30_start

; =============== S U B	R O U T	I N E =======================================
_j_apu_se30_loop:
		JMP		_apu_se30_loop

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

; =============== S U B	R O U T	I N E =======================================
_apu_tri_chnl_with_dpcm:
		LDA		_APU_STATUS
		AND		#$19
		STA		_APU_STATUS
		ORA		#$0F
		STA		_APU_STATUS
		BNE		_apu_tri_load					; OPTIMIZE
;		LDA		#$01							; REDUNDANT
;		STA		_APU_TRI_COUNT
;		LDA		#$FF
;		STA		_APU_TRI_TIME
;		LDA		#$0F
;		STA		_APU_TRI_LEN
;		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_tri_chnl_no_dpcm:
		LDA		_APU_STATUS
		AND		#$0F
		ORA		#$0F
		STA		_APU_STATUS

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_tri_load:
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
