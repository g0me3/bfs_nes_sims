
		ORG $A000

		ECHO "PROGRAM BEGIN"
		ECHO "-------------"

		BANK_START $F0

; ---------------------------------------------------------------------------
; -------------M-A-I-N--M-U-S-I-C--A-N-D--S-E--E-N-G-I-N-E-------------------
; ---------------------------------------------------------------------------

; =============== S U B	R O U T	I N E =======================================
_apu_play:
		LDA		_ptr0							; FIX, since we using common _switch
		PHA										; here, need to save common ptr0 var
		LDA		_ptr0+1							; which can be used by random code
		PHA
		LDA		#$C0
		STA		_PAD1_REG
		LDA		_apu_idx
		BPL		loc_A014
		LDA		_APU_STATUS
		AND		#$0F
		STA		_APU_STATUS
loc_A014:
		JSR		_apu_play_mus
		JSR		_apu_play_se0
		JSR		_apu_play_se1
		LDA		#$00
		STA		_apu_mus_idx_req
		STA		_apu_se_idx_req
		STA		_apu_se_idx_req+1
		PLA
		STA		_ptr0+1
		PLA
		STA		_ptr0
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_init:
		LDA		#$40
		STA		_PAD1_REG
;		LDA		#$80							; debug leftovers
;		STA		byte_4083
;		ASL
		LDA		#$00
		STA		_APU_DMC_FREQ
		STA		_APU_DMC_LOAD
;		STA		byte_4083						; debug leftovers
		STA		_apu_mus_tri_shadow+3
		LDA		#$0F
		STA		_APU_STATUS
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_stop:
		LDA		#$00
		STA		byte_F3
		STA		_apu_mus_pulse2_shadow+2
		STA		_apu_mus_tri_shadow+3
		STA		_APU_TRI_COUNT

; =============== S U B	R O U T	I N E =======================================
_apu_subA050:
		LDX		#$00
		STX		_APU_STATUS
		LDX		#$0F
		STX		_APU_STATUS
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_play_mus:
		LDA		_apu_mus_idx_req
		BMI		_apu_stop
		BNE		.mus_active
		LDA		byte_F3
		BNE		loc_A066
		RTS
loc_A066:
		JMP		loc_A131
.mus_active:
		STA		_apu_mus_pulse2_shadow+2
		CMP		#$01
		BEQ		loc_A089
		CMP		#$04
		BEQ		loc_A089
		CMP		#$21
		BEQ		loc_A089
		CMP		#$60
		BEQ		loc_A089
		LDA		byte_F3
		BEQ		loc_A089
		BMI		loc_A089
		LDA		#$C0
		STA		_apu_mus_tri_shadow+3
		BNE		loc_A066
loc_A089:
		LDA		_apu_mus_pulse2_shadow+2
		JSR		_apu_subA050
loc_A08F:
		TAX
		ASL
		BMI		loc_A09D
		ASL
		BPL		loc_A0A1
		ASL
		BEQ		loc_A0A1
		LDY		#$0C
		BNE		loc_A0A3
loc_A09D:
		LDY		#$05
		BNE		loc_A0A3
loc_A0A1:
		LDY		#$FF
loc_A0A3:
		TXA
loc_A0A4:
		INY
		LSR
		BCC		loc_A0A4
		LDA		_apu_mus_seq_ofs_list,Y
		STA		byte_F3
		TAY
loc_A0AE:
		LDA		_apu_mus_seq_ofs_list,Y
		BEQ		_apu_stop
		CMP		#$FF
		BNE		loc_A0BE
		LDA		_apu_mus_pulse2_shadow+2
		LDY		#$FF
		BNE		loc_A08F
loc_A0BE:
		CMP		#$FE
		BNE		loc_A0E1
		LDA		_apu_mus_pulse2_shadow+2
		ASL
		BPL		loc_A0CC
		ORA		#$40
		BNE		loc_A0D8
loc_A0CC:
		TAX
		ASL
		BPL		loc_A0D7
		TXA
		AND		#$3F
		ORA		#$20
		BNE		loc_A0D8
loc_A0D7:
		TXA
loc_A0D8:
		AND		#$7F
		STA		_apu_mus_pulse2_shadow+2
		LDY		#$FF
		BNE		loc_A08F
loc_A0E1:
		TAY
		LDA		_apu_mus_res_lib-1,Y
		STA		_apu_mus_chnl_flags
		LDA		_apu_mus_res_lib,Y
		STA		_apu_ptr
		LDA		_apu_mus_res_lib+1,Y
		STA		_apu_ptr+1
		LDA		_apu_mus_res_lib+2,Y
		STA		_apu_mus_v07
		LDA		_apu_mus_res_lib+3,Y
		STA		_apu_mus_v08
		LDA		_apu_mus_res_lib+4,Y
		STA		_apu_mus_pulse2_shadow+1
		LDA		_apu_mus_res_lib+5,Y
		STA		_apu_mus_var3
		LDA		#$01
		STA		_apu_mus_v03
		STA		_apu_mus_v04
		STA		_apu_mus_v05
		STA		_apu_mus_var7
		LDA		#$00
		STA		_apu_mus_v06
		LDA		_APU_STATUS
		AND		#$1B
		STA		_APU_STATUS
		LDA		_APU_STATUS
		AND		#$10
		ORA		#$0F
		STA		_APU_STATUS
loc_A131:
		LDA		_apu_mus_tri_shadow+3
		BEQ		loc_A141
		DEC		_apu_mus_tri_shadow+3
		BNE		loc_A141
		LDA		_apu_mus_pulse2_shadow+2
		JMP		loc_A089
loc_A141:
		DEC		_apu_mus_v03
		BNE		loc_A19B
		LDY		_apu_mus_v06
		INC		_apu_mus_v06
		LDA		(_apu_ptr),Y
		BNE		loc_A157
		INC		byte_F3
		LDY		byte_F3
		JMP		loc_A0AE
loc_A157:
		BPL		loc_A187
		TAX
		ASL
		BMI		loc_A165
		LDA		#$00
		STA		_apu_mus_tri_shadow+1
		TXA
		BNE		loc_A172
loc_A165:
		LDA		#$80
		STA		_apu_mus_tri_shadow+1
		LDY		_apu_mus_v06
		INC		_apu_mus_v06
		LDA		(_apu_ptr),Y
loc_A172:
		AND		#$0F
		CLC
		ADC		_apu_mus_chnl_flags
		TAY
		LDA		byte_195F78,Y
		STA		_apu_mus_v00
		LDY		_apu_mus_v06
		INC		_apu_mus_v06
		LDA		(_apu_ptr),Y
loc_A187:
		JSR		_apu_pulse0_note_load
		LDA		_apu_mus_v00
		STA		_apu_mus_v03
		JSR		_apu_subA388
		STA		_apu_mus_var8
		LDA		#$00
		STA		_apu_mus_pulse1_shadow+2
loc_A19B:
		INC		_apu_mus_pulse1_shadow+2
		LDY		_apu_mus_var8
		BEQ		loc_A1A6
		DEC		_apu_mus_var8
loc_A1A6:
		LDA		_apu_mus_pulse2_shadow+1
		AND		#$70
		LSR
		LSR
		LSR
		LSR
		TAX
		LDA		_apu_mus_v00
		JSR		_apu_subA394
		JSR		_apu_subA3F9
		STA		_APU_PULSE1_ENV
		LDA		#$7F
		STA		_APU_PULSE1_SWEEP
		LDA		_apu_mus_pulse1_shadow
		BEQ		loc_A1DF
		LDA		_apu_mus_pulse1_shadow+2
		LDX		_apu_mus_pulse1_shadow
		LDY		_apu_mus_tri_shadow+1
		BNE		loc_A1D7
		JSR		_apu_subA3CF
		JMP		loc_A1DC
loc_A1D7:
		LDY		#$00
		JSR		_aou_subA3DF
loc_A1DC:
		STX		_APU_PULSE1_TIME
loc_A1DF:
		DEC		_apu_mus_v04
		BNE		loc_A24C
		LDY		_apu_mus_v07
		BNE		loc_A1F1
		LDA		#$50
		STA		_APU_PULSE2_ENV
		JMP		loc_A2B9
loc_A1F1:
		INC		_apu_mus_v07
		LDA		(_apu_ptr),Y
		BPL		loc_A226
		TAX
		ASL
		BMI		loc_A204
		LDA		#$00
		STA		_apu_mus_tri_shadow+2
		TXA
		BNE		loc_A211
loc_A204:
		LDA		#$80
		STA		_apu_mus_tri_shadow+2
		LDY		_apu_mus_v07
		INC		_apu_mus_v07
		LDA		(_apu_ptr),Y
loc_A211:
		AND		#$0F
		CLC
		ADC		_apu_mus_chnl_flags
		TAY
		LDA		byte_195F78,Y
		STA		_apu_mus_v01
		LDY		_apu_mus_v07
		INC		_apu_mus_v07
		LDA		(_apu_ptr),Y
loc_A226:
		LDX		_apu_se_loop_counters+1
		BNE		loc_A23B
		LDX		_apu_se_loop_idx
		BNE		loc_A23B
		LDX		#$97
		STX		_APU_PULSE2_ENV
		LDX		#$7F
		STX		_APU_PULSE2_SWEEP
		JSR		_apu_pulse1_note_load
loc_A23B:
		LDA		_apu_mus_v01
		STA		_apu_mus_v04
		JSR		_apu_subA388
		STA		_apu_mus_pulse1_shadow+1
		LDA		#$00
		STA		_apu_mus_pulse1_shadow+3
loc_A24C:
		INC		_apu_mus_pulse1_shadow+3
		LDY		_apu_mus_pulse1_shadow+1
		BEQ		loc_A257
		DEC		_apu_mus_pulse1_shadow+1
loc_A257:
		LDA		_apu_se_loop_counters+1
		BNE		loc_A2B9
		LDA		_apu_se_loop_idx
		BNE		loc_A2B9
		LDA		_apu_mus_pulse2_shadow+1
		AND		#$07
		TAX
		LDA		_apu_mus_v01
		JSR		_apu_subA394
		JSR		_apu_subA3F9
		TAX
		LDA		_apu_mus_pulse2_shadow+1
		AND		#$07
		CMP		#$03
		BEQ		loc_A292
		CMP		#$06
		BEQ		loc_A292
		TXA
		AND		#$0F
		BEQ		loc_A288
		SEC
		SBC		#$03
		BPL		loc_A288
		LDA		#$01
loc_A288:
		STA		byte_F6
		TXA
		AND		#$F0
		ORA		byte_F6
		JMP		loc_A293
loc_A292:
		TXA
loc_A293:
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP
		LDA		_apu_mus_pulse2_shadow
		BEQ		loc_A2B9
		LDA		_apu_mus_pulse1_shadow+3
		LDX		_apu_mus_pulse2_shadow
		LDY		_apu_mus_tri_shadow+2
		BNE		loc_A2B1
		JSR		_apu_subA3CF
		JMP		loc_A2B6
loc_A2B1:
		LDY		#$04
		JSR		_aou_subA3DF
loc_A2B6:
		STX		_APU_PULSE2_TIME
loc_A2B9:
		LDA		_apu_mus_tri_shadow+3
		BEQ		loc_A2D0
		AND		#$C0
		BNE		loc_A2D0
		LDA		_APU_STATUS
		AND		#$03
		STA		_APU_STATUS
		ORA		#$0F
		STA		_APU_STATUS
		RTS
loc_A2D0:
		DEC		_apu_mus_v05
		BNE		loc_A31E
		LDY		_apu_mus_v08
		INC		_apu_mus_v08
		LDA		(_apu_ptr),Y
		BPL		loc_A2F4
		AND		#$0F
		CLC
		ADC		_apu_mus_chnl_flags
		TAY
		LDA		byte_195F78,Y
		STA		_apu_mus_v02
		LDY		_apu_mus_v08
		INC		_apu_mus_v08
		LDA		(_apu_ptr),Y
loc_A2F4:
		LDX		_apu_se_loop_idx
		BNE		loc_A318
		LDX		_apu_se_loop_idx+1
		BNE		loc_A318
		STA		byte_F6
		LDX		#$2F
		LDY		_apu_mus_pulse2_shadow+1
		BPL		loc_A310
		TYA
		AND		#$08
		BNE		loc_A30E
		LDX		#$FF
		BNE		loc_A310
loc_A30E:
		LDX		#$49
loc_A310:
		STX		_APU_TRI_COUNT
		LDA		byte_F6
		JSR		_apu_tri_note_load
loc_A318:
		LDA		_apu_mus_v02
		STA		_apu_mus_v05
loc_A31E:
		DEC		_apu_mus_var7
		BNE		locret_A387
		LDY		_apu_mus_var3
		BEQ		locret_A387
		INC		_apu_mus_var3
		LDA		(_apu_ptr),Y
		STA		byte_F6
		AND		#$0F
		CLC
		ADC		_apu_mus_chnl_flags
		TAY
		LDA		byte_195F78,Y
		STA		_apu_mus_pulse2_shadow+3
		LDX		_apu_se_loop_idx+1
		BNE		loc_A381
		LDX		_apu_se_loop_idx
		BNE		loc_A381
		LDA		_APU_STATUS
		AND		#$10
		BNE		loc_A381
		LDA		byte_F6
		AND		#$70
		BEQ		loc_A381
		AND		#$10
		BEQ		loc_A366
		LDA		#_DPCM_IDX2
		STA		_APU_DMC_ADDR
		LDA		#$03
		STA		_APU_DMC_LEN
		LDA		#$06
		STA		_APU_DMC_FREQ
		BNE		loc_A375
loc_A366:
		LDA		#_DPCM_IDX2
		STA		_APU_DMC_ADDR
		LDA		#$08
		STA		_APU_DMC_LEN
		LDA		#$0F
		STA		_APU_DMC_FREQ
loc_A375:
		LDA		_apu_idx
		BMI		loc_A381
		LDA		_APU_STATUS
		ORA		#$10
		STA		_APU_STATUS
loc_A381:
		LDA		_apu_mus_pulse2_shadow+3
		STA		_apu_mus_var7
locret_A387:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_subA388:
		CLC
		CMP		#$13
		BCS		loc_A391
		LDA		#$17
		BNE		locret_A393
loc_A391:
		LDA		#$3F
locret_A393:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_subA394:
		CLC
		CMP		#$13
		BCS		loc_A3AE
		INX
		LDA		#$E8
		CLC
loc_A39D:
		ADC		#$18
		DEX
		BNE		loc_A39D
		STA		byte_F6
		TYA
		CLC
		ADC		byte_F6
		TAY
		LDA		byte_195C00,Y
		BNE		locret_A3CE
loc_A3AE:
		STX		byte_F7
		INX
		LDA		#$C0
loc_A3B3:
		CLC
		ADC		#$40
		DEX
		BNE		loc_A3B3
		STA		byte_F6
		TYA
		CLC
		ADC		byte_F6
		TAY
		LDA		byte_F7
		AND		#$04
		BNE		loc_A3CB
		LDA		byte_195CA8,Y
		BNE		locret_A3CE
loc_A3CB:
		LDA		byte_195DA8,Y
locret_A3CE:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_subA3CF:
		SEC
		CMP		#$19
		BCC		locret_A3DE
		LSR
		LSR
		LSR
		LSR
		BCS		loc_A3DD
		DEX
		BNE		locret_A3DE
loc_A3DD:
		INX
locret_A3DE:
		RTS

; =============== S U B	R O U T	I N E =======================================
_aou_subA3DF:
		AND		#$08
		BNE		loc_A3ED
		TXA
		CLC
		ADC		#$02
		BCC		loc_A3F5
		LDA		#$FF
		BNE		loc_A3F5
loc_A3ED:
		TXA
		SEC
		SBC		#$02
		BCS		loc_A3F5
		LDA		#$00
loc_A3F5:
		STA		_apu_mus_pulse1_shadow,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_subA3F9:
		LDY		_apu_mus_tri_shadow+3
		BEQ		locret_A41E
		TAX
		TYA
		LSR
		LSR
		LSR
		LSR
		LSR
		EOR		#$FF
		AND		#$07
		STA		byte_F6
		TXA
		AND		#$0F
		BEQ		loc_A417
		SEC
		SBC		byte_F6
		BPL		loc_A417
		LDA		#$01
loc_A417:
		STA		byte_F6
		TXA
		AND		#$F0
		ORA		byte_F6
locret_A41E:
		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref_7:
;		LDY		#$7F
;		STY		_APU_PULSE1_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_pulse0_note_load:
		LDX		#$00

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_note_load:
		TAY
		LDA		_apu_pulse_len_tbl+1,Y
		STA		_apu_mus_pulse1_shadow,X
		BEQ		_apu_def_note_load
		STA		_APU_PULSE1_TIME,X
		LDA		_apu_pulse_len_tbl,Y

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_chnl_len_load:
		ORA		#$08
		STA		_APU_PULSE1_LEN,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_pulse1_note_load:
		LDX		#$04
		BNE		_apu_note_load

; =============== S U B	R O U T	I N E =======================================
_apu_tri_note_load:
		LDX		#$08
		BNE		_apu_note_load

; =============== S U B	R O U T	I N E =======================================
_apu_def_note_load:
		LDA		#$50
		STA		_APU_PULSE1_ENV,X
		LDA		#$00
		STA		_APU_PULSE1_TIME,X
		BEQ		_apu_chnl_len_load

; =============== S U B	R O U T	I N E =======================================
_apu_tri_chnl_with_dpcm:
		LDA		_APU_STATUS
		AND		#$19
		STA		_APU_STATUS
		ORA		#$0F
		STA		_APU_STATUS
		BNE		_apu_tri_load					; OPTIMIZED
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

; =============== S U B	R O U T	I N E =======================================
_apu_play_se0:
		LDY		_apu_se_idx_req
		BEQ		.apu_loop_se0					; if request is zero, then either do loop or nothing
		LDA		#$00							; and manually clear request flag now
		STA		_apu_se_idx_req
		STY		_apu_se_loop_idx				; store at once here, instead of in every single routine
		DEY
		TYA
		JSR		_switch
		.WORD	_apu_se1_start
		.WORD	_apu_se2_start
		.WORD	_apu_se4_start
		.WORD	_apu_se8_start
		.WORD	_apu_se10_start
		.WORD	_apu_se20_start
		.WORD	_apu_se40_start
		.WORD	_apu_se80_start
.apu_loop_se0:
		LDY		_apu_se_loop_idx				; if zero, idle
		BEQ		_apu_se0_done
		DEY
		TYA
		JSR		_switch
		.WORD	_apu_se1_loop
		.WORD	_apu_se2_loop
		.WORD	_apu_se4_loop
		.WORD	_apu_se8_loop
		.WORD	_apu_se10_loop
		.WORD	_apu_se20_loop
		.WORD	_apu_se40_loop
		.WORD	_apu_se80_loop
_apu_se0_done:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_play_se1:
		LDY		_apu_se_idx_req+1
		BEQ		.apu_loop_se1
		LDA		#$00
		STA		_apu_se_idx_req+1
		STY		_apu_se_loop_idx+1
		DEY
		TYA
		JSR		_switch
		.WORD	_apu_se100_start
		.WORD	_apu_se200_start
		.WORD	_apu_se400_start
		.WORD	_apu_se800_start
		.WORD	_apu_se1000_start
		.WORD	_apu_se2000_start
		.WORD	_apu_se3000_start
		.WORD	_apu_se4000_start
		.WORD	_apu_se8000_start
.apu_loop_se1:
		LDY		_apu_se_loop_idx+1			; if zero, idle
		BEQ		_apu_se1_done
		DEY
		TYA
		JSR		_switch
		.WORD	_apu_se100_loop
		.WORD	_apu_se200_loop
		.WORD	_apu_se400_loop
		.WORD	_apu_se800_loop
		.WORD	_apu_se1000_loop
		.WORD	_apu_se2000_loop
		.WORD	_apu_se3000_loop
		.WORD	_apu_se4000_loop
		.WORD	_apu_se8000_loop
_apu_se1_done:
		RTS

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;_apu_play_se0:
;		LDY		_apu_se_idx_req					; load requested flags byte
;		LDA		_apu_se_loop_idx				; load currently playing flags
;		LSR		_apu_se_idx_req					; extract bit from requested flags
;		BCS		_apu_se1_start					; non-zero on the first run, zero
;		LSR										; on all following steps, so single exec here
;		BCS		_apu_se1_loop					; while here still 0, so no loop exec
;		LSR		_apu_se_idx_req					; and so on for all bits
;		BCS		_j_apu_se2_start				; this system is good for old simple games
;		LSR										; where it taken, but when your se
;		BCS		_j_apu_se2_loop					; library grow, this may be a problem
;		LSR		_apu_se_idx_req
;		BCS		_j_apu_se4_start
;		LSR
;		BCS		_j_apu_se4_loop
;		LSR		_apu_se_idx_req
;		BCS		_j_apu_se8_start
;		LSR
;		BCS		_j_apu_se8_loop
;		LSR		_apu_se_idx_req
;		BCS		_j_apu_se10_start
;		LSR
;		BCS		_j_apu_se10_loop
;		LSR		_apu_se_idx_req
;		BCS		_j_apu_se20_start
;		LSR
;		BCS		_j_apu_se20_loop
;		LSR		_apu_se_idx_req
;		BCS		_j_apu_se40_start
;		LSR
;		BCS		_j_apu_se40_loop
;		LSR		_apu_se_idx_req				; re-enable unused sound effect
;		BCS		_j_apu_se80_start
;		LSR
;		BCS		_j_apu_se80_loop
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se2_start:
;		JMP		_apu_se2_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se2_loop:
;		JMP		_apu_se2_loop
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se4_start:
;		JMP		_apu_se4_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se4_loop:
;		JMP		_apu_se4_loop
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se8_start:
;		JMP		_apu_se8_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se8_loop:
;		JMP		_apu_se8_loop
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se10_start:
;		JMP		_apu_se10_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se10_loop:
;		JMP		_apu_se10_loop
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se20_start:
;		JMP		_apu_se20_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se20_loop:
;		JMP		_apu_se20_loop
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se40_start:
;		JMP		_apu_se40_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se40_loop:
;		JMP		_apu_se40_loop
;
; FIX: re-enabled, TODO: find where may be used
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se80_start:
;		JMP		_apu_se80_start
;
; =============== S U B	R O U T	I N E =======================================
;_j_apu_se80_loop:
;		JMP		_apu_se80_loop

; =============== S U B	R O U T	I N E =======================================
_apu_se1_start:
;		STY		_apu_se_loop_idx
		LDA		#$02
		STA		_apu_se_loop_counters
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$83
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se1_loop:
		LDA		#$6E
		JSR		_apu_tri_note_load
		LDA		#$6E
		JSR		_apu_pulse1_note_load
		DEC		_apu_se_loop_counters
		BNE		locret_A4DC
		JSR		_apu_status_reset
		LDA		#$00
		STA		_apu_se_loop_idx
locret_A4DC:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se2_start:
;		STY		_apu_se_loop_idx
		LDA		#$18
		STA		_apu_se_loop_counters
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP
		LDA		#$96
		STA		_APU_PULSE2_ENV

; =============== S U B	R O U T	I N E =======================================
_apu_se2_loop:
		LDY		_apu_se_loop_counters
		LDA		byte_A51C,Y
		STA		_APU_TRI_TIME
		BEQ		loc_A505
		LDA		#$7C
		LDX		#$09
		BNE		loc_A507
loc_A505:
		LDX		#$08
loc_A507:
		STA		_APU_PULSE2_TIME
		STX		_APU_PULSE2_LEN
		STX		_APU_TRI_LEN
		DEC		_apu_se_loop_counters
		BNE		locret_A51B
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_A51B:
		RTS
byte_A51C:
		.BYTE	$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$00,$00,$00,$00
		.BYTE	$00,$00,$D4,$D4,$D4,$D4,$D4,$D4

; =============== S U B	R O U T	I N E =======================================
_apu_se4_start:
;		STY		_apu_se_loop_idx
		LDA		#$03
		STA		_apu_se_loop_counters
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$83
		STA		_APU_PULSE2_ENV

; =============== S U B	R O U T	I N E =======================================
_apu_se4_loop:
		LDY		_apu_se_loop_counters
		LDA		byte_A5A3-1,Y
		JSR		_apu_tri_note_load
		LDY		_apu_se_loop_counters
		LDA		byte_A5A3,Y
		JSR		_apu_pulse1_note_load
		DEC		_apu_se_loop_counters
		BNE		locret_A5A2
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_A5A2:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se8_start:
;		STY		_apu_se_loop_idx
		LDA		#$0A
		STA		_apu_se_loop_counters
		JSR		_apu_tri_chnl_with_dpcm
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$88
		STA		_APU_PULSE2_ENV
		LDA		#$D7
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se8_loop:
		LDY		_apu_se_loop_counters
		LDA		byte_195834-1,Y
		JSR		_apu_tri_note_load
		LDY		_apu_se_loop_counters
		LDA		byte_195834,Y
		JSR		_apu_pulse1_note_load
		DEC		_apu_se_loop_counters
		BNE		locret_195833
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_195833:
		RTS
byte_195834:
		.BYTE	$02,$3E,$3A,$36,$46,$3C,$02,$46,$46,$3C,$34,$34

; =============== S U B	R O U T	I N E =======================================
_apu_se10_start:
		JSR		_apu_tri_chnl_with_dpcm
;		STY		_apu_se_loop_idx
		LDA		#$7E
		STA		_apu_se_loop_counters
		LDA		#$82
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se10_loop:
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		_apu_se_loop_counters
		ASL
		ASL
		ASL
		ASL
		BCS		loc_A556
		EOR		#$F0
loc_A556:
		STA		_APU_TRI_TIME
		STA		_APU_PULSE2_TIME
		LDA		#$0A
		STA		_APU_TRI_LEN
		LDA		#$09
		STA		_APU_PULSE2_LEN
		DEC		_apu_se_loop_counters
		DEC		_apu_se_loop_counters
		BNE		locret_A573
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_A573:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se20_start:
		JSR		_apu_tri_chnl_with_dpcm
;		STY		_apu_se_loop_idx
		LDA		#$14
		STA		_apu_se_loop_counters

; =============== S U B	R O U T	I N E =======================================
_apu_se20_loop:
		LDA		#$07
		STA		_APU_TRI_COUNT
		LDY		_apu_se_loop_counters
		LDA		byte_195899-1,Y
		JSR		_apu_tri_note_load
		DEC		_apu_se_loop_counters
		BNE		locret_195898
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_195898:
		RTS
byte_195899:
		.BYTE	$02,$68,$68,$68,$02,$18,$18,$18,$02,$0E,$0E,$0E,$02,$08,$08,$08
		.BYTE	$02,$04,$04,$04

; =============== S U B	R O U T	I N E =======================================
_apu_se40_start:
		JSR		_apu_tri_chnl_with_dpcm
;		STY		_apu_se_loop_idx
		LDA		#$14
		STA		_apu_se_loop_counters
		LDA		#$07
		STA		_APU_TRI_COUNT

; =============== S U B	R O U T	I N E =======================================
_apu_se40_loop:
		LDY		_apu_se_loop_counters
		LDA		byte_1958CF-1,Y
		JSR		_apu_tri_note_load
		DEC		_apu_se_loop_counters
		BNE		locret_1958CE
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_1958CE:
		RTS
byte_1958CF:
		.BYTE	$02,$16,$16,$16,$02,$1A,$1A,$1A,$02,$6C,$6C,$6C,$02,$74,$74,$74
		.BYTE	$02,$76,$76,$76

; =============== S U B	R O U T	I N E =======================================
; as against two other unused SE here (1000, 8000), this one were completely
; unreferenced here. there is unknown what for it intended to be used, but
; even loop counter here is different from those used in other SE routines
;
_apu_se80_start:
		JSR		_apu_status_reset
;		STY		_apu_se_loop_idx
		LDA		_apu_se80_loop_counter
		BNE		loc_19584D
		LDA		#$14
		STA		_apu_se80_loop_counter
loc_19584D:
		LDA		#$C6
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se80_loop:
		LDA		_apu_se80_loop_counter
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
		DEC		_apu_se80_loop_counter
		DEC		_apu_se80_loop_counter
		BNE		locret_195876
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_status_reset
locret_195876:
		RTS

byte_A5A3:
		.BYTE	$30,$32,$30,$2E
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

	MAC	_TMUS
		.BYTE	[{1}-_apu_mus_seq_ofs_list]
	ENDM

	MAC	_SEQ
		.BYTE	[{1}-_apu_mus_res_lib+1]
	ENDM

	MAC	_NEXT
		.BYTE	$FE
	ENDM

	MAC	_END
		.BYTE	$FF
	ENDM

_apu_mus_seq_ofs_list:
		_TMUS	_mus0	;	$0F
		_TMUS	_mus1	;	$12
		_TMUS	_mus2	;	$16
		_TMUS	_mus3	;	$19
		_TMUS	_mus4	;	$1C
		_TMUS	_mus6	;	$24
		_TMUS	_mus5	;	$1F
		_TMUS	_mus7	;	$28
		_TMUS	_mus8	;	$2C
		_TMUS	_mus9	;	$32
		_TMUS	_musA	;	$34
		_TMUS	_musB	;	$3A
		_TMUS	_musC	;	$3C
		_TMUS	_musD	;	$3E
		_TMUS	_musE	;	$40
_mus0:
		_SEQ	_trk00	;	$01
		_SEQ	_trk01	;	$08
		_NEXT			;	$FE
_mus1:
		_SEQ	_trk02	;	$0F
		_SEQ	_trk03	;	$16
		_SEQ	_trk04	;	$1D
		_END			;	$FF
_mus2:
		_SEQ	_trk05	;	$24
		_SEQ	_trk06	;	$2B
		_END			;	$FF
_mus3:
		_SEQ	_trk07	;	$32
		_SEQ	_trk07	;	$32
		_NEXT			;	$FE
_mus4:
		_SEQ	_trk08	;	$39
		_SEQ	_trk09	;	$40
		_END			;	$FF
_mus5:
		_SEQ	_trk0A	;	$47
		_SEQ	_trk0B	;	$4E
		_SEQ	_trk0C	;	$55
		_SEQ	_trk0D	;	$5C
		_END			;	$FF
_mus6:
		_SEQ	_trk0E	;	$63
		_SEQ	_trk0F	;	$6A
		_SEQ	_trk10	;	$71
		_END			;	$FF
_mus7:
		_SEQ	_trk11	;	$78
		_SEQ	_trk12	;	$7F
		_SEQ	_trk13	;	$86
		_END			;	$FF
_mus8:
		_SEQ	_trk14	;	$8D
		_SEQ	_trk15	;	$94
		_SEQ	_trk16	;	$9B
		_SEQ	_trk17	;	$A2
		_SEQ	_trk18	;	$A9
		_END			;	$FF
_mus9:
		_SEQ	_trk19	;	$B0
		_NEXT			;	$FE
_musA:
		_SEQ	_trk1A	;	$B7
		_SEQ	_trk1B	;	$BE
		_SEQ	_trk1C	;	$C5
		_SEQ	_trk1D	;	$CC
		_SEQ	_trk1E	;	$D3
		_END			;	$FF
_musB:
		_SEQ	_trk1F	;	$DA
		_NEXT			;	$FE
_musC:
		_SEQ	_trk20	;	$E1
		_END			;	$FF
_musD:
		_SEQ	_trk21	;	$E8
		_NEXT			;	$FE
_musE:
		_SEQ	_trk22	;	$EF
		_END			;	$FF

_apu_mus_res_lib:
		_tapu _trk00,$1B,byte_A746,$52,$A5,$D3,$00
		_tapu _trk01,$1B,byte_A821,$3A,$75,$D3,$A2
		_tapu _trk02,$1B,byte_A8D4,$42,$85,$C6,$B1
		_tapu _trk03,$1B,byte_A9AD,$32,$65,$C6,$86
		_tapu _trk04,$1B,byte_AA51,$42,$85,$CE,$AD
		_tapu _trk05,$24,byte_AB92,$35,$6B,$C6,$8F
		_tapu _trk06,$24,byte_AC43,$34,$69,$C1,$8F
		_tapu _trk07,$1B,byte_AD0A,$15,$29,$00,$00
		_tapu _trk08,$1B,byte_AD53,$3E,$7E,$10,$00
		_tapu _trk09,$1B,byte_AE12,$35,$78,$10,$00
		_tapu _trk0A,$24,byte_AECC,$26,$4D,$03,$00
		_tapu _trk0B,$24,byte_AF2A,$1A,$3B,$04,$00
		_tapu _trk0C,$24,byte_AF76,$22,$30,$04,$00
		_tapu _trk0D,$24,byte_AFBD,$11,$42,$00,$00
		_tapu _trk0E,$24,byte_B015,$3F,$9D,$01,$00
		_tapu _trk0F,$24,byte_B0E7,$24,$35,$CC,$00
		_tapu _trk10,$24,byte_B14A,$32,$50,$DC,$00
		_tapu _trk11,$36,byte_B1EB,$1D,$3B,$46,$00
		_tapu _trk12,$36,byte_B257,$16,$28,$46,$00
		_tapu _trk13,$36,byte_B2AF,$46,$8A,$DE,$00
		_tapu _trk14,$2D,byte_B3A6,$2B,$55,$22,$79
		_tapu _trk15,$2D,byte_B449,$2B,$55,$22,$76
		_tapu _trk16,$2D,byte_B4E9,$2C,$57,$22,$78
		_tapu _trk17,$2D,byte_B588,$2B,$55,$22,$78
		_tapu _trk18,$2D,byte_B627,$37,$6D,$22,$94
		_tapu _trk19,$36,byte_B6E2,$1C,$39,$C6,$00
		_tapu _trk1A,$1B,byte_B72B,$40,$88,$40,$D0
		_tapu _trk1B,$1B,byte_B843,$49,$79,$40,$A9
		_tapu _trk1C,$1B,byte_B91C,$25,$3D,$40,$55
		_tapu _trk1D,$1B,byte_B989,$27,$4F,$83,$72
		_tapu _trk1E,$1B,byte_BA13,$2D,$5B,$83,$75
		_tapu _trk1F,$12,byte_BAA0,$0A,$13,$00,$00
		_tapu _trk20,$12,byte_BABC,$2F,$41,$40,$00
		_tapu _trk21,$24,byte_AB26,$05,$09,$00,$0D
		_tapu _trk22,$24,byte_AB35,$1E,$3B,$22,$49
byte_A746:
		.BYTE	$81,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08
		.BYTE	$2A,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08
		.BYTE	$2A,$08,$2E,$06,$24,$08,$2E,$06,$24,$08,$2E,$06,$24,$08,$2E,$06
		.BYTE	$24,$08,$2C,$04,$26,$08,$2C,$04,$26,$08,$2C,$04,$26,$08,$2C,$04
		.BYTE	$26,$08,$2A,$04,$24,$08,$2A,$04,$24,$08,$2A,$04,$24,$08,$2A,$04
		.BYTE	$24,$00,$88,$02,$81,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08
		.BYTE	$2A,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08,$2A,$0A,$30,$08
		.BYTE	$2A,$0A,$30,$08,$2A,$08,$2E,$06,$24,$08,$2E,$06,$24,$08,$2E,$06
		.BYTE	$24,$08,$2E,$06,$24,$08,$2C,$04,$26,$08,$2C,$04,$26,$08,$2C,$04
		.BYTE	$26,$08,$2C,$04,$26,$08,$2A,$04,$24,$08,$2A,$04,$24,$08,$2A,$04
		.BYTE	$24,$08,$2A,$04,$24,$84,$02,$02,$81,$12,$14,$18,$82,$6A,$18,$81
		.BYTE	$14,$12,$14,$18,$82,$6A,$18,$81,$12,$10,$12,$16,$84,$68,$83,$02
		.BYTE	$81,$02,$0E,$12,$14,$82,$68,$14,$81,$12,$0E,$12,$14,$82,$68,$14
		.BYTE	$81,$0E,$0C,$0E,$12,$84,$64,$83,$02,$81,$02
byte_A821:
		.BYTE	$81,$08,$28,$04,$22,$08,$28,$04,$22,$08,$28,$04,$22,$08,$28,$04
		.BYTE	$22,$04,$26,$30,$20,$04,$26,$30,$20,$04,$26,$30,$20,$04,$26,$30
		.BYTE	$20,$30,$22,$2E,$1C,$30,$22,$2E,$1C,$30,$22,$2E,$22,$08,$22,$04
		.BYTE	$22,$2E,$20,$2C,$20,$06,$20,$32,$20,$00,$88,$02,$81,$08,$28,$04
		.BYTE	$22,$08,$28,$04,$22,$08,$28,$04,$22,$08,$28,$04,$22,$04,$26,$30
		.BYTE	$20,$04,$26,$30,$20,$04,$26,$30,$20,$04,$26,$30,$20,$30,$22,$2E
		.BYTE	$1C,$30,$22,$2E,$1C,$30,$22,$2E,$22,$08,$22,$04,$22,$2E,$20,$2C
		.BYTE	$20,$06,$20,$32,$20,$81,$0A,$0E,$10,$82,$68,$64,$81,$10,$0A,$0E
		.BYTE	$10,$82,$68,$64,$81,$10,$08,$0A,$0E,$82,$64,$85,$18,$81,$12,$16
		.BYTE	$18,$82,$6A,$85,$68,$81,$04,$08,$0A,$82,$18,$85,$16,$84,$0E,$83
		.BYTE	$32,$0C,$84,$84,$84,$84,$84,$82,$82,$82,$82,$81,$A1,$A1,$81,$A1
		.BYTE	$A1,$81,$A1
byte_A8D4:
		.BYTE	$81,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C,$82,$76,$6C,$81
		.BYTE	$68,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C,$82,$76,$6C,$81
		.BYTE	$68,$64,$68,$6C,$82,$76,$6C,$81,$68,$64,$68,$6C,$82,$76,$6C,$81
		.BYTE	$64,$1A,$64,$68,$82,$76,$68,$81,$64,$1A,$64,$68,$82,$76,$68,$81
		.BYTE	$6C,$00,$88,$2E,$81,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C
		.BYTE	$82,$76,$6C,$81,$68,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C
		.BYTE	$82,$76,$6C,$81,$68,$64,$68,$6C,$82,$76,$6C,$81,$68,$64,$68,$6C
		.BYTE	$82,$76,$6C,$81,$64,$1A,$64,$68,$82,$76,$68,$81,$64,$1A,$64,$68
		.BYTE	$82,$76,$68,$81,$6C,$85,$16,$6C,$83,$76,$81,$02,$85,$6C,$82,$16
		.BYTE	$85,$12,$6C,$83,$76,$81,$02,$85,$6C,$82,$12,$85,$0E,$6C,$83,$76
		.BYTE	$81,$02,$85,$6C,$82,$0E,$85,$12,$6C,$83,$76,$81,$02,$85,$6C,$82
		.BYTE	$12,$91,$91,$81,$91,$85,$91,$A5,$91,$85,$91,$91,$91,$81,$91,$85
		.BYTE	$91,$A5,$91,$85,$91,$91,$91,$81,$91,$85,$91,$A5,$91,$85,$91,$91
		.BYTE	$91,$81,$91,$85,$91,$A5,$91,$85,$91
byte_A9AD:
		.BYTE	$81,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C,$82,$76,$6C,$81
		.BYTE	$68,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C,$82,$76,$6C,$81
		.BYTE	$68,$64,$68,$6C,$82,$76,$6C,$81,$68,$64,$68,$6C,$82,$76,$6C,$81
		.BYTE	$64,$00,$88,$02,$81,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C
		.BYTE	$82,$76,$6C,$81,$68,$66,$68,$6C,$82,$76,$6C,$81,$68,$66,$68,$6C
		.BYTE	$82,$76,$6C,$81,$68,$64,$68,$6C,$82,$76,$6C,$81,$68,$64,$68,$6C
		.BYTE	$82,$76,$6C,$81,$64,$85,$16,$6C,$83,$76,$81,$02,$85,$6C,$82,$16
		.BYTE	$85,$12,$6C,$83,$76,$81,$02,$85,$6C,$82,$12,$85,$0E,$6C,$83,$76
		.BYTE	$81,$02,$85,$6C,$82,$0E,$91,$91,$81,$91,$85,$91,$A5,$91,$85,$91
		.BYTE	$91,$91,$81,$91,$85,$91,$A5,$91,$85,$91,$91,$91,$81,$91,$85,$91
		.BYTE	$A5,$91,$85,$91
byte_AA51:
		.BYTE	$81,$76,$1A,$68,$76,$1A,$68,$76,$1A,$68,$76,$1A,$68,$76,$1A,$68
		.BYTE	$76,$74,$16,$1A,$74,$16,$1A,$74,$16,$1A,$74,$16,$1A,$70,$16,$74
		.BYTE	$16,$76,$66,$68,$76,$66,$68,$76,$66,$68,$76,$66,$68,$76,$66,$68
		.BYTE	$76,$74,$16,$1A,$74,$16,$1A,$74,$16,$1A,$74,$16,$1A,$70,$16,$74
		.BYTE	$16,$00,$88,$02,$81,$76,$1A,$68,$76,$1A,$68,$76,$1A,$68,$76,$1A
		.BYTE	$68,$76,$1A,$68,$76,$74,$16,$1A,$74,$16,$1A,$74,$16,$1A,$74,$16
		.BYTE	$1A,$70,$16,$74,$16,$76,$66,$68,$76,$66,$68,$76,$66,$68,$76,$66
		.BYTE	$68,$76,$66,$68,$76,$74,$16,$1A,$74,$16,$1A,$74,$16,$1A,$74,$16
		.BYTE	$1A,$70,$16,$74,$16,$85,$0C,$81,$0C,$82,$02,$84,$0C,$82,$02,$85
		.BYTE	$0C,$81,$0C,$82,$02,$84,$0C,$82,$02,$85,$0C,$81,$0C,$82,$02,$84
		.BYTE	$0C,$82,$02,$85,$0C,$81,$0C,$82,$02,$84,$0C,$82,$02,$91,$91,$81
		.BYTE	$91,$85,$91,$A5,$91,$85,$91,$91,$91,$81,$91,$85,$91,$A5,$91,$85
		.BYTE	$91,$91,$91,$81,$91,$85,$91,$A5,$91,$85,$91,$91,$91,$81,$91,$85
		.BYTE	$91,$A5,$91,$85,$91
byte_AB26:
		.BYTE	$80,$02,$83,$02,$00,$80,$02,$83,$02,$80,$02,$83,$02,$80,$83
byte_AB35:
		.BYTE	$81,$2A,$2A,$87,$2A,$80,$2A,$02,$81,$2A,$80,$2A,$81,$2A,$2A,$28
		.BYTE	$28,$87,$28,$80,$28,$02,$81,$28,$80,$28,$81,$28,$28,$00,$81,$22
		.BYTE	$22,$87,$22,$80,$22,$02,$81,$22,$80,$22,$81,$22,$22,$20,$20,$87
		.BYTE	$20,$80,$20,$02,$81,$20,$80,$20,$81,$20,$20,$82,$26,$1C,$26,$81
		.BYTE	$1C,$26,$82,$24,$4A,$24,$81,$4A,$24,$91,$91,$A1,$80,$90,$80,$90
		.BYTE	$91,$A1,$91,$91,$91,$A1,$80,$90,$80,$90,$91,$A1,$91
byte_AB92:
		.BYTE	$82,$02,$81,$24,$26,$82,$2A,$81,$2E,$86,$2A,$81,$02,$20,$24,$82
		.BYTE	$26,$81,$24,$85,$26,$81,$2A,$84,$24,$81,$02,$82,$02,$1C,$20,$1C
		.BYTE	$85,$2A,$81,$2A,$24,$85,$1C,$82,$46,$4A,$1C,$20,$85,$2A,$82,$2A
		.BYTE	$81,$2E,$2A,$26,$00,$88,$02,$82,$02,$81,$24,$26,$82,$2A,$81,$2E
		.BYTE	$86,$2A,$81,$02,$20,$24,$82,$26,$81,$24,$85,$26,$81,$2A,$84,$24
		.BYTE	$81,$02,$82,$02,$1C,$20,$1C,$85,$2A,$81,$2A,$24,$85,$1C,$82,$46
		.BYTE	$4A,$1C,$20,$85,$2A,$82,$2A,$81,$2E,$2A,$26,$82,$04,$12,$6C,$04
		.BYTE	$32,$12,$68,$32,$30,$0E,$68,$30,$2E,$0E,$64,$81,$26,$2A,$82,$2C
		.BYTE	$0E,$64,$2C,$2A,$0C,$64,$2A,$28,$08,$64,$28,$2A,$08,$16,$1A,$95
		.BYTE	$91,$92,$A2,$95,$91,$92,$A2,$95,$91,$92,$A2,$95,$91,$92,$A2,$95
		.BYTE	$91,$92,$A2,$95,$91,$92,$A2,$95,$91,$92,$A2,$95,$91,$91,$A1,$81
		.BYTE	$A1
byte_AC43:
		.BYTE	$82,$02,$81,$24,$26,$82,$2A,$81,$2E,$86,$2A,$81,$02,$20,$24,$82
		.BYTE	$26,$81,$24,$85,$26,$81,$2A,$84,$24,$81,$02,$82,$02,$1C,$20,$1C
		.BYTE	$83,$24,$82,$04,$81,$1C,$20,$82,$26,$24,$1C,$46,$81,$1C,$3C,$3E
		.BYTE	$40,$83,$42,$00,$88,$02,$82,$02,$81,$24,$26,$82,$2A,$81,$2E,$86
		.BYTE	$2A,$81,$02,$20,$24,$82,$26,$81,$24,$85,$26,$81,$2A,$84,$24,$81
		.BYTE	$02,$82,$02,$1C,$20,$1C,$83,$24,$82,$04,$81,$1C,$20,$82,$26,$24
		.BYTE	$1C,$46,$81,$1C,$3C,$3E,$40,$83,$42,$82,$04,$12,$6C,$04,$32,$12
		.BYTE	$68,$32,$30,$0E,$68,$30,$2E,$0E,$64,$81,$26,$2A,$82,$2C,$0E,$64
		.BYTE	$2C,$2A,$0C,$28,$64,$84,$2A,$82,$04,$81,$08,$0A,$82,$0C,$2A,$91
		.BYTE	$91,$A1,$91,$92,$A1,$91,$91,$91,$A1,$91,$92,$A1,$91,$91,$91,$A1
		.BYTE	$91,$92,$A1,$91,$91,$91,$A1,$91,$92,$A1,$91,$91,$91,$A1,$91,$92
		.BYTE	$A1,$91,$91,$91,$A1,$91,$92,$A1,$91,$91,$91,$A1,$91,$92,$A1,$91
		.BYTE	$91,$91,$A1,$91,$92,$A1,$91
byte_AD0A:
		.BYTE	$82,$24,$24,$24,$24,$26,$26,$81,$26,$26,$82,$26,$24,$24,$24,$81
		.BYTE	$24,$26,$84,$02,$00,$82,$4A,$4A,$46,$46,$1C,$1C,$81,$4A,$1C,$82
		.BYTE	$4A,$4A,$4A,$46,$81,$46,$1C,$84,$02,$85,$1C,$81,$1C,$42,$82,$42
		.BYTE	$81,$42,$85,$20,$81,$20,$42,$46,$82,$42,$85,$1C,$81,$1C,$42,$82
		.BYTE	$42,$81,$20,$02,$20,$82,$42,$46,$4A
byte_AD53:
		.BYTE	$82,$02,$83,$12,$82,$0C,$0E,$0C,$81,$08,$82,$0C,$83,$2A,$80,$2A
		.BYTE	$02,$2A,$02,$82,$2A,$80,$2A,$02,$81,$02,$80,$2A,$02,$82,$2A,$2A
		.BYTE	$2A,$02,$83,$12,$82,$0C,$0E,$0C,$81,$08,$82,$04,$83,$0C,$80,$0C
		.BYTE	$02,$0C,$02,$82,$0C,$81,$0C,$02,$0C,$82,$0C,$0C,$08,$00,$82,$4A
		.BYTE	$4A,$46,$81,$46,$82,$1C,$1C,$81,$1C,$4A,$1C,$82,$20,$85,$20,$80
		.BYTE	$20,$02,$1E,$02,$82,$1E,$80,$1C,$02,$81,$02,$1C,$82,$1C,$4A,$4A
		.BYTE	$82,$4A,$4A,$46,$81,$46,$82,$1C,$1C,$81,$1C,$4A,$82,$46,$83,$46
		.BYTE	$80,$46,$02,$46,$02,$82,$46,$44,$81,$44,$82,$44,$46,$4A,$85,$1C
		.BYTE	$81,$1C,$81,$42,$82,$42,$81,$42,$85,$20,$81,$20,$82,$42,$3E,$85
		.BYTE	$3C,$81,$3C,$85,$3A,$81,$3A,$85,$38,$81,$38,$42,$46,$82,$42,$85
		.BYTE	$1C,$81,$1C,$42,$82,$42,$81,$42,$85,$20,$81,$20,$85,$1C,$81,$1C
		.BYTE	$85,$4A,$81,$4A,$85,$4A,$81,$4A,$85,$3C,$81,$3C,$82,$40,$44
byte_AE12:
		.BYTE	$84,$04,$83,$0C,$85,$12,$86,$64,$81,$02,$1A,$64,$82,$68,$64,$81
		.BYTE	$1A,$82,$16,$83,$12,$81,$0C,$85,$64,$81,$04,$82,$0E,$0C,$81,$04
		.BYTE	$82,$08,$83,$04,$80,$04,$02,$04,$02,$82,$04,$81,$04,$81,$02,$04
		.BYTE	$82,$04,$04,$08,$00,$82,$4A,$4A,$81,$46,$82,$46,$81,$48,$02,$48
		.BYTE	$82,$48,$48,$48,$85,$24,$81,$24,$24,$82,$24,$81,$24,$85,$22,$81
		.BYTE	$22,$22,$82,$22,$83,$20,$81,$02,$83,$2A,$82,$46,$42,$81,$3E,$82
		.BYTE	$3E,$81,$3E,$85,$42,$80,$42,$02,$40,$02,$82,$40,$80,$3E,$02,$81
		.BYTE	$02,$80,$3E,$02,$82,$3E,$3E,$3E,$85,$46,$81,$46,$46,$82,$46,$81
		.BYTE	$46,$85,$42,$81,$42,$85,$42,$81,$42,$85,$40,$81,$40,$40,$82,$40
		.BYTE	$81,$40,$85,$3E,$81,$3E,$85,$3E,$81,$3E,$85,$3C,$81,$3C,$85,$46
		.BYTE	$81,$46,$85,$38,$81,$38,$85,$42,$81,$44,$85,$46,$81,$46,$85,$38
		.BYTE	$81,$38,$85,$42,$81,$42,$42,$46,$82,$4A
byte_AECC:
		.BYTE	$82,$22,$81,$20,$22,$82,$26,$81,$30,$2C,$81,$2A,$2C,$2A,$26,$82
		.BYTE	$22,$48,$81,$1C,$20,$82,$22,$85,$48,$81,$42,$82,$44,$81,$48,$82
		.BYTE	$1C,$81,$48,$82,$44,$00,$88,$02,$82,$22,$81,$20,$22,$82,$26,$81
		.BYTE	$30,$2C,$81,$2A,$2C,$2A,$26,$82,$22,$48,$81,$1C,$20,$82,$22,$85
		.BYTE	$48,$81,$42,$82,$44,$81,$48,$82,$1C,$81,$48,$82,$44,$82,$0A,$02
		.BYTE	$08,$02,$04,$02,$2A,$02,$2C,$02,$2A,$02,$26,$02,$22,$02
byte_AF2A:
		.BYTE	$86,$26,$81,$22,$20,$86,$22,$81,$20,$1C,$86,$20,$81,$1C,$48,$82
		.BYTE	$1C,$81,$20,$22,$82,$26,$81,$2A,$22,$00,$82,$1C,$81,$48,$44,$80
		.BYTE	$48,$44,$83,$48,$81,$02,$44,$42,$80,$44,$42,$83,$44,$81,$02,$42
		.BYTE	$3E,$80,$42,$3E,$85,$42,$82,$42,$02,$1C,$02,$82,$20,$02,$2A,$02
		.BYTE	$1C,$02,$26,$02,$48,$02,$22,$02,$44,$02,$46,$02
byte_AF76:
		.BYTE	$80,$20,$22,$85,$20,$81,$22,$20,$1C,$20,$02,$2C,$2A,$26,$22,$20
		.BYTE	$1C,$20,$80,$22,$20,$86,$22,$81,$02,$02,$2A,$26,$22,$20,$22,$1C
		.BYTE	$20,$00,$83,$1C,$48,$4A,$82,$1C,$20,$84,$42,$83,$3E,$82,$42,$46
		.BYTE	$82,$48,$30,$2C,$44,$42,$46,$4A,$42,$1C,$81,$04,$30,$82,$04,$81
		.BYTE	$1C,$48,$82,$46,$42,$46,$3E
byte_AFBD:
		.BYTE	$81,$22,$20,$1C,$20,$82,$48,$81,$1C,$20,$84,$22,$26,$2A,$26,$48
		.BYTE	$00,$83,$48,$3E,$81,$02,$1E,$1C,$48,$82,$1C,$81,$48,$44,$81,$02
		.BYTE	$22,$20,$1C,$82,$20,$81,$1C,$48,$02,$26,$22,$20,$82,$22,$81,$20
		.BYTE	$1C,$02,$20,$1C,$48,$3E,$48,$3A,$46,$02,$3A,$3E,$34,$80,$38,$34
		.BYTE	$85,$38,$82,$48,$30,$2C,$44,$42,$2A,$44,$2C,$46,$2E,$48,$30,$4A
		.BYTE	$32,$1C,$22,$83,$26,$3E,$48,$30
byte_B015:
		.BYTE	$86,$04,$85,$32,$81,$2E,$81,$2A,$26,$83,$2A,$82,$08,$86,$04,$83
		.BYTE	$0C,$82,$2C,$83,$08,$82,$2A,$82,$04,$2E,$26,$24,$81,$20,$2A,$2E
		.BYTE	$32,$86,$04,$85,$32,$81,$2E,$2A,$26,$83,$2A,$82,$08,$04,$1C,$20
		.BYTE	$26,$24,$1C,$83,$1C,$82,$4A,$84,$1C,$81,$02,$20,$22,$26,$00,$82
		.BYTE	$02,$81,$1C,$02,$1C,$02,$82,$02,$81,$1C,$02,$4A,$02,$82,$02,$81
		.BYTE	$20,$02,$4A,$02,$82,$02,$81,$1C,$02,$82,$02,$02,$81,$1C,$02,$24
		.BYTE	$02,$82,$02,$81,$20,$02,$1E,$02,$46,$02,$1C,$02,$26,$02,$26,$02
		.BYTE	$83,$26,$82,$02,$81,$1C,$02,$1C,$02,$82,$02,$81,$1C,$02,$4A,$02
		.BYTE	$82,$02,$81,$20,$02,$4A,$02,$24,$02,$3C,$02,$40,$02,$1C,$02,$42
		.BYTE	$02,$3C,$02,$83,$3E,$82,$3E,$86,$3E,$83,$3C,$82,$02,$82,$1C,$0C
		.BYTE	$0C,$26,$12,$12,$24,$12,$12,$2E,$0C,$2A,$28,$0C,$26,$24,$0C,$22
		.BYTE	$20,$0E,$16,$04,$83,$32,$82,$1C,$0C,$0C,$26,$12,$12,$24,$12,$12
		.BYTE	$83,$2E,$82,$2C,$86,$2A,$82,$2A,$08,$12,$83,$1C,$82,$2A,$83,$1C
		.BYTE	$82,$48
byte_B0E7:
		.BYTE	$83,$2A,$81,$26,$1C,$85,$22,$81,$20,$22,$26,$20,$48,$84,$48,$82
		.BYTE	$02,$83,$2A,$81,$26,$1C,$85,$22,$81,$20,$22,$04,$84,$30,$81,$02
		.BYTE	$22,$26,$2A,$00,$86,$1C,$44,$81,$3E,$42,$84,$42,$82,$02,$86,$1C
		.BYTE	$44,$84,$26,$83,$02,$81,$44,$22,$83,$04,$81,$48,$26,$83,$08,$81
		.BYTE	$3A,$48,$26,$22,$2A,$30,$0E,$0A,$30,$2A,$26,$22,$81,$44,$22,$83
		.BYTE	$04,$81,$48,$26,$83,$08,$81,$3A,$48,$26,$22,$2A,$30,$0E,$0A,$30
		.BYTE	$2A,$26,$22
byte_B14A:
		.BYTE	$82,$2E,$2E,$2E,$2E,$2A,$26,$2A,$2A,$2A,$2A,$26,$24,$26,$26,$26
		.BYTE	$26,$24,$20,$84,$24,$83,$02,$82,$24,$24,$24,$20,$1C,$4A,$86,$1C
		.BYTE	$83,$20,$81,$24,$26,$85,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$81,$2A,$2E
		.BYTE	$32,$00,$86,$1C,$4A,$20,$1E,$1C,$4A,$85,$4A,$46,$4A,$46,$86,$46
		.BYTE	$44,$42,$83,$46,$81,$44,$4A,$85,$20,$1E,$1C,$4A,$20,$1E,$1C,$26
		.BYTE	$85,$20,$82,$24,$81,$26,$85,$2A,$81,$42,$2A,$26,$85,$24,$82,$26
		.BYTE	$81,$2A,$85,$2E,$81,$46,$4A,$1E,$85,$20,$82,$24,$81,$26,$85,$2A
		.BYTE	$82,$42,$81,$2A,$85,$1C,$82,$42,$81,$42,$85,$1C,$82,$42,$81,$42
		.BYTE	$85,$4A,$82,$1C,$81,$20,$85,$24,$82,$28,$81,$2C,$83,$2E,$82,$2A
		.BYTE	$83,$28,$82,$26,$85,$24,$22,$20,$82,$2A,$81,$26,$85,$24,$22,$20
		.BYTE	$2A
byte_B1EB:
		.BYTE	$84,$24,$82,$1C,$2A,$87,$28,$80,$20,$84,$46,$82,$28,$84,$26,$82
		.BYTE	$2A,$26,$80,$26,$24,$20,$84,$24,$82,$02,$80,$02,$00,$88,$02,$84
		.BYTE	$24,$82,$1C,$2A,$87,$28,$80,$20,$84,$46,$82,$28,$84,$26,$82,$2A
		.BYTE	$26,$80,$26,$24,$20,$84,$24,$82,$02,$80,$02,$81,$04,$04,$02,$04
		.BYTE	$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04
		.BYTE	$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04
		.BYTE	$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04
byte_B257:
		.BYTE	$84,$24,$82,$1C,$2A,$87,$28,$80,$20,$84,$46,$82,$28,$84,$26,$82
		.BYTE	$2A,$2C,$86,$04,$08,$00,$84,$42,$82,$3C,$4A,$84,$46,$82,$40,$20
		.BYTE	$84,$44,$82,$48,$1C,$86,$26,$2C,$04,$04,$02,$04,$04,$04,$04,$04
		.BYTE	$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04
		.BYTE	$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04
		.BYTE	$04,$04,$30,$30,$02,$30,$30,$30
byte_B2AF:
		.BYTE	$84,$0C,$82,$04,$12,$87,$10,$80,$08,$86,$2E,$82,$30,$16,$84,$12
		.BYTE	$82,$04,$12,$87,$10,$80,$08,$86,$2E,$82,$30,$16,$84,$12,$82,$04
		.BYTE	$1A,$87,$18,$80,$0E,$86,$08,$82,$0C,$68,$84,$64,$82,$14,$6E,$87
		.BYTE	$64,$80,$16,$83,$0E,$87,$16,$80,$0E,$83,$04,$87,$0E,$80,$04,$83
		.BYTE	$2E,$2C,$81,$2A,$26,$00,$84,$2A,$82,$2A,$04,$87,$2E,$80,$32,$86
		.BYTE	$28,$82,$26,$04,$84,$32,$82,$2A,$04,$87,$2E,$80,$32,$86,$28,$82
		.BYTE	$26,$04,$84,$32,$82,$2A,$08,$87,$08,$80,$30,$86,$26,$82,$2A,$0C
		.BYTE	$84,$0A,$82,$04,$14,$87,$0E,$80,$04,$83,$2E,$87,$04,$80,$2E,$83
		.BYTE	$26,$87,$2E,$80,$26,$83,$1C,$1C,$82,$20,$81,$2E,$0C,$1A,$64,$6C
		.BYTE	$72,$08,$16,$64,$68,$6C,$72,$2A,$08,$10,$12,$1A,$68,$04,$12,$18
		.BYTE	$68,$6E,$30,$2E,$0C,$1A,$64,$6C,$72,$08,$16,$64,$68,$6C,$72,$2A
		.BYTE	$08,$10,$12,$1A,$68,$04,$12,$18,$68,$6E,$30,$2E,$0C,$1A,$64,$6C
		.BYTE	$72,$08,$16,$64,$68,$70,$76,$2A,$08,$12,$18,$68,$6E,$04,$12,$64
		.BYTE	$68,$30,$6C,$2C,$0A,$18,$64,$6A,$74,$1E,$2C,$0A,$0E,$14,$66,$12
		.BYTE	$12,$02,$12,$12,$12,$12,$12,$02,$12,$12,$12,$12,$12,$02,$12,$12
		.BYTE	$12,$12,$12,$02,$12,$12,$12
byte_B3A6:
		.BYTE	$80,$48,$48,$81,$48,$46,$80,$02,$42,$81,$02,$46,$02,$48,$02,$1C
		.BYTE	$02,$83,$42,$81,$02,$80,$48,$48,$81,$48,$46,$80,$02,$42,$81,$02
		.BYTE	$46,$02,$48,$02,$1C,$02,$83,$42,$81,$02,$00,$80,$42,$42,$81,$42
		.BYTE	$3E,$80,$02,$3A,$81,$02,$3E,$02,$42,$02,$46,$02,$83,$3A,$81,$02
		.BYTE	$80,$42,$42,$81,$42,$3E,$80,$02,$3A,$81,$02,$3E,$02,$42,$02,$46
		.BYTE	$02,$83,$3A,$81,$02,$81,$34,$34,$3A,$34,$48,$46,$26,$22,$81,$34
		.BYTE	$34,$3A,$34,$48,$46,$26,$22,$81,$34,$34,$3A,$34,$48,$46,$26,$22
		.BYTE	$81,$34,$34,$3A,$34,$48,$46,$26,$22,$90,$90,$91,$A7,$90,$97,$A0
		.BYTE	$81,$91,$90,$90,$91,$A7,$90,$97,$A0,$A0,$A0,$80,$A0,$90,$90,$91
		.BYTE	$A7,$90,$97,$A0,$81,$91,$90,$90,$91,$A7,$90,$80,$A0,$A0,$A0,$A0
		.BYTE	$A0,$A0,$A0
byte_B449:
		.BYTE	$80,$44,$44,$81,$44,$42,$80,$02,$3E,$81,$02,$42,$02,$44,$02,$48
		.BYTE	$02,$83,$3E,$81,$02,$80,$44,$44,$81,$44,$42,$80,$02,$3E,$81,$02
		.BYTE	$42,$02,$44,$02,$48,$02,$83,$3E,$81,$02,$00,$80,$3E,$3E,$81,$3E
		.BYTE	$3A,$80,$02,$36,$81,$02,$3A,$02,$3E,$02,$42,$02,$83,$36,$81,$02
		.BYTE	$80,$3E,$3E,$81,$3E,$3A,$80,$02,$36,$81,$02,$3A,$02,$3E,$02,$42
		.BYTE	$02,$83,$36,$81,$02,$81,$60,$60,$36,$60,$44,$42,$22,$1E,$60,$60
		.BYTE	$36,$60,$44,$42,$22,$1E,$60,$60,$36,$60,$44,$42,$22,$1E,$60,$60
		.BYTE	$36,$60,$44,$42,$22,$1E,$90,$90,$91,$A7,$90,$97,$A0,$81,$91,$90
		.BYTE	$90,$91,$A7,$90,$97,$A0,$A0,$A0,$80,$A0,$90,$90,$91,$A7,$90,$97
		.BYTE	$A0,$81,$91,$90,$90,$91,$A7,$90,$A0,$A0,$80,$A0,$A0,$80,$A0,$A0
byte_B4E9:
		.BYTE	$80,$48,$48,$81,$48,$46,$80,$02,$42,$81,$02,$46,$02,$48,$02,$1C
		.BYTE	$02,$83,$42,$81,$02,$80,$48,$48,$81,$48,$46,$80,$02,$42,$81,$02
		.BYTE	$46,$02,$48,$02,$1C,$02,$20,$02,$22,$02,$24,$00,$80,$42,$42,$81
		.BYTE	$42,$3E,$80,$02,$3A,$81,$02,$3E,$02,$42,$02,$46,$02,$83,$3A,$81
		.BYTE	$02,$80,$42,$42,$81,$42,$3E,$80,$02,$3A,$81,$02,$3E,$02,$42,$02
		.BYTE	$44,$02,$48,$02,$4A,$02,$1E,$81,$34,$34,$3A,$34,$48,$46,$26,$22
		.BYTE	$34,$34,$3A,$34,$48,$46,$26,$22,$34,$34,$3A,$34,$48,$46,$60,$62
		.BYTE	$34,$34,$34,$34,$34,$34,$1C,$1C,$90,$90,$91,$A7,$90,$97,$A0,$81
		.BYTE	$91,$90,$90,$91,$A7,$90,$97,$A0,$A0,$A0,$80,$A0,$90,$90,$91,$A7
		.BYTE	$90,$97,$A0,$81,$91,$91,$A1,$A1,$A1,$A0,$A0,$A0,$A0,$80,$A7
byte_B588:
		.BYTE	$80,$02,$24,$81,$24,$22,$80,$02,$1E,$81,$02,$22,$02,$22,$02,$1E
		.BYTE	$02,$83,$4A,$81,$02,$80,$20,$20,$81,$20,$1E,$80,$02,$4A,$81,$02
		.BYTE	$1E,$02,$1E,$02,$4A,$02,$83,$46,$81,$02,$00,$80,$02,$1E,$81,$1E
		.BYTE	$4A,$80,$02,$46,$81,$02,$4A,$02,$4A,$02,$46,$02,$83,$44,$81,$02
		.BYTE	$80,$4A,$4A,$81,$4A,$46,$80,$02,$42,$81,$02,$46,$02,$46,$02,$42
		.BYTE	$02,$83,$40,$81,$02,$80,$4A,$4A,$81,$4A,$32,$28,$2E,$32,$06,$0A
		.BYTE	$3C,$3C,$24,$4A,$20,$24,$2E,$2C,$46,$46,$2E,$24,$2A,$2E,$32,$06
		.BYTE	$38,$38,$20,$46,$20,$1E,$1C,$4A,$90,$90,$91,$A7,$90,$97,$A0,$81
		.BYTE	$91,$90,$90,$91,$A7,$90,$97,$A0,$A0,$A0,$80,$A0,$90,$90,$91,$A7
		.BYTE	$90,$97,$A0,$81,$91,$91,$A1,$A1,$A1,$A0,$A0,$A0,$A0,$80,$A7
byte_B627:
		.BYTE	$80,$02,$24,$81,$24,$22,$80,$02,$1E,$81,$02,$22,$02,$22,$02,$1E
		.BYTE	$02,$83,$4A,$81,$02,$80,$20,$20,$81,$20,$1E,$80,$02,$4A,$81,$02
		.BYTE	$1E,$02,$1E,$02,$24,$24,$24,$80,$24,$24,$24,$24,$02,$80,$24,$88
		.BYTE	$20,$1E,$1C,$4A,$48,$46,$00,$80,$02,$1E,$81,$1E,$4A,$80,$02,$46
		.BYTE	$81,$02,$4A,$02,$4A,$02,$46,$02,$83,$44,$81,$02,$80,$4A,$4A,$81
		.BYTE	$4A,$46,$80,$02,$42,$81,$02,$46,$02,$46,$02,$46,$46,$46,$80,$46
		.BYTE	$46,$46,$46,$02,$80,$3E,$88,$3C,$3A,$38,$36,$34,$62,$80,$4A,$4A
		.BYTE	$81,$4A,$32,$28,$2E,$32,$06,$0A,$3C,$3C,$24,$4A,$20,$24,$2E,$2C
		.BYTE	$46,$46,$2E,$3C,$42,$46,$46,$42,$40,$40,$40,$40,$80,$4A,$4A,$4A
		.BYTE	$4A,$02,$87,$42,$90,$90,$91,$A7,$90,$97,$A0,$81,$91,$90,$90,$91
		.BYTE	$A7,$90,$97,$A0,$A0,$A0,$80,$A0,$90,$90,$91,$A7,$90,$97,$A0,$81
		.BYTE	$91,$91,$A1,$A1,$A1,$A0,$A0,$A0,$A0,$80,$A7
byte_B6E2:
		.BYTE	$86,$2A,$82,$20,$83,$2A,$81,$02,$1C,$20,$26,$83,$2A,$81,$02,$04
		.BYTE	$2E,$26,$83,$2A,$81,$02,$26,$2A,$04,$84,$08,$00,$88,$02,$86,$2A
		.BYTE	$82,$20,$83,$2A,$81,$02,$1C,$20,$26,$83,$2A,$81,$02,$04,$2E,$26
		.BYTE	$83,$2A,$81,$02,$26,$2A,$04,$84,$32,$84,$02,$02,$86,$02,$82,$20
		.BYTE	$2A,$81,$20,$2A,$82,$04,$2C,$84,$2A
byte_B72B:
		.BYTE	$85,$24,$24,$82,$20,$28,$85,$24,$24,$82,$2A,$28,$85,$24,$24,$82
		.BYTE	$20,$28,$85,$24,$24,$82,$2A,$28,$84,$16,$81,$02,$80,$12,$10,$81
		.BYTE	$12,$80,$10,$08,$86,$2E,$81,$02,$80,$32,$04,$81,$08,$80,$0C,$10
		.BYTE	$81,$12,$83,$10,$81,$02,$12,$08,$80,$04,$08,$84,$04,$81,$2E,$00
		.BYTE	$85,$42,$42,$82,$40,$46,$85,$42,$42,$82,$4A,$46,$85,$42,$42,$82
		.BYTE	$40,$46,$85,$42,$42,$82,$4A,$46,$85,$42,$42,$82,$40,$46,$85,$42
		.BYTE	$42,$82,$4A,$46,$85,$42,$42,$82,$40,$46,$85,$42,$42,$82,$4A,$46
		.BYTE	$85,$24,$24,$82,$20,$28,$85,$24,$24,$82,$20,$28,$85,$24,$24,$82
		.BYTE	$2A,$28,$85,$24,$24,$82,$20,$28,$85,$1C,$1C,$82,$1C,$1C,$85,$1C
		.BYTE	$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C
		.BYTE	$85,$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82
		.BYTE	$1C,$1C,$85,$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C,$85,$1C
		.BYTE	$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C
		.BYTE	$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$91,$A1,$95,$95,$91,$A1
		.BYTE	$91,$91,$95,$95,$91,$A1,$A1,$A1,$95,$95,$91,$A1,$91,$91,$95,$95
		.BYTE	$91,$A1,$91,$91,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$91,$A1
		.BYTE	$95,$95,$91,$A1,$91,$A1,$95,$95,$91,$A1,$91,$A1,$95,$95,$91,$A1
		.BYTE	$91,$A1,$95,$95,$91,$A1,$91,$A1
byte_B843:
		.BYTE	$84,$16,$81,$02,$80,$1A,$0C,$81,$12,$80,$10,$08,$86,$2E,$81,$02
		.BYTE	$80,$32,$04,$81,$08,$80,$0C,$10,$81,$12,$83,$10,$81,$02,$12,$08
		.BYTE	$84,$68,$82,$02,$84,$16,$81,$02,$80,$18,$08,$81,$0E,$80,$0C,$08
		.BYTE	$86,$30,$81,$02,$80,$30,$04,$81,$08,$80,$0C,$0E,$81,$12,$83,$08
		.BYTE	$81,$02,$0C,$04,$84,$64,$82,$02,$00,$85,$42,$42,$82,$40,$46,$85
		.BYTE	$42,$42,$82,$4A,$46,$85,$42,$42,$82,$40,$46,$85,$42,$42,$82,$4A
		.BYTE	$46,$85,$3E,$3E,$82,$3C,$42,$85,$3E,$3E,$82,$3C,$42,$85,$3E,$3E
		.BYTE	$82,$3C,$42,$85,$3E,$3E,$82,$3C,$42,$85,$1C,$1C,$82,$1C,$1C,$85
		.BYTE	$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C,$1C,$85,$1C,$1C,$82,$1C
		.BYTE	$1C,$85,$48,$48,$82,$48,$48,$85,$48,$48,$82,$48,$48,$85,$48,$48
		.BYTE	$82,$48,$48,$85,$48,$48,$82,$48,$48,$95,$95,$91,$A1,$91,$91,$95
		.BYTE	$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$A1
		.BYTE	$A1,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$91,$91,$95,$95,$91
		.BYTE	$A1,$91,$91,$95,$95,$91,$A1,$91,$91
byte_B91C:
		.BYTE	$84,$12,$81,$02,$80,$14,$04,$81,$0A,$80,$08,$04,$86,$2C,$81,$02
		.BYTE	$80,$2C,$30,$81,$04,$80,$08,$0A,$81,$0E,$83,$04,$81,$02,$08,$30
		.BYTE	$84,$12,$82,$02,$00,$85,$3A,$3A,$82,$38,$3E,$85,$3A,$3A,$82,$38
		.BYTE	$3E,$85,$3A,$3A,$82,$38,$3E,$85,$1C,$1C,$82,$1E,$1E,$85,$2C,$2C
		.BYTE	$82,$2C,$2C,$85,$2C,$2C,$82,$2C,$2C,$85,$2C,$2C,$82,$2C,$2C,$85
		.BYTE	$2A,$2A,$82,$0A,$0A,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$91
		.BYTE	$91,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$A1,$A1
byte_B989:
		.BYTE	$81,$0A,$08,$0A,$83,$18,$81,$02,$0A,$82,$08,$0A,$81,$08,$0A,$84
		.BYTE	$30,$81,$02,$81,$0A,$08,$0A,$83,$18,$81,$02,$0A,$82,$08,$0A,$81
		.BYTE	$08,$0A,$84,$30,$81,$02,$00,$88,$02,$81,$0A,$08,$0A,$83,$18,$81
		.BYTE	$02,$0A,$82,$08,$0A,$81,$08,$0A,$84,$30,$81,$02,$81,$0A,$08,$0A
		.BYTE	$83,$18,$81,$02,$0A,$82,$08,$0A,$81,$08,$0A,$84,$30,$81,$02,$85
		.BYTE	$14,$12,$83,$0E,$81,$02,$12,$85,$14,$12,$83,$0E,$81,$02,$12,$85
		.BYTE	$14,$12,$83,$0E,$81,$02,$12,$85,$14,$12,$82,$0E,$81,$12,$82,$14
		.BYTE	$81,$18,$95,$95,$95,$81,$A1,$81,$95,$95,$95,$81,$A1,$81,$95,$95
		.BYTE	$95,$81,$A1,$81,$95,$95,$95,$81,$A1,$81
byte_BA13:
		.BYTE	$81,$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$10,$81,$0E,$10,$84
		.BYTE	$06,$81,$02,$81,$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$81,$10
		.BYTE	$81,$16,$14,$16,$85,$6C,$81,$64,$1A,$64,$85,$72,$00,$88,$02,$81
		.BYTE	$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$10,$81,$0E,$10,$84,$06
		.BYTE	$81,$02,$81,$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$81,$10,$81
		.BYTE	$16,$14,$16,$85,$6C,$81,$64,$1A,$64,$85,$72,$85,$1A,$18,$83,$14
		.BYTE	$81,$02,$18,$85,$1A,$18,$83,$14,$81,$02,$18,$85,$1A,$18,$14,$10
		.BYTE	$85,$0C,$08,$04,$32,$95,$95,$95,$81,$A1,$81,$95,$95,$95,$81,$A1
		.BYTE	$81,$95,$95,$95,$81,$A1,$81,$95,$95,$95,$81,$A1,$81
byte_BAA0:
		.BYTE	$82,$04,$04,$81,$08,$82,$0A,$84,$0C,$00,$82,$24,$24,$81,$26,$82
		.BYTE	$28,$84,$2A,$82,$1C,$1C,$81,$1C,$82,$1C,$84,$1C
byte_BABC:
		.BYTE	$82,$02,$81,$2A,$04,$0C,$12,$0E,$02,$0C,$0E,$02,$16,$12,$82,$02
		.BYTE	$02,$81,$64,$85,$02,$02,$82,$02,$81,$2E,$04,$0E,$16,$12,$82,$02
		.BYTE	$02,$81,$04,$82,$0E,$81,$0C,$82,$0E,$81,$12,$85,$02,$02,$00,$86
		.BYTE	$24,$26,$83,$28,$81,$02,$86,$2A,$81,$02,$86,$26,$2A,$2E,$85,$04
		.BYTE	$32,$81,$34,$02,$02,$1C,$02,$34,$38,$02,$02,$20,$02,$38,$3A,$02
		.BYTE	$02,$22,$02,$3A,$3C,$02,$02,$24,$02,$3C,$3E,$02,$02,$26,$02,$3E
		.BYTE	$3C,$02,$02,$24,$02,$3C,$38,$02,$3C,$3E,$02,$42,$02,$02,$38,$42
		.BYTE	$02,$02

		BANK_END F0,$C000
