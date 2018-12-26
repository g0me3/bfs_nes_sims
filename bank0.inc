
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
		LDA		#$C0							; hmm, why every time disable frame-irq?
		STA		_PAD1_REG						; it were disabled once at the boot
		LDA		_apu_dpcm_disable_flag
		BPL		loc_A014						; mask out dpcm channel if forbidden
		LDA		_APU_STATUS
		AND		#$0F
		STA		_APU_STATUS
loc_A014:
		JSR		_apu_play_mus
		JSR		_apu_play_se0
		JSR		_apu_play_se1
		LDA		#$00							; hmmm, these will clear mus/se requests
		STA		_apu_mus_idx_req				; one more time, but they already cleared
		STA		_apu_se_idx_req					; inside play routines.
		STA		_apu_se_idx_req+1
		PLA
		STA		_ptr0+1
		PLA
		STA		_ptr0
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_init:
		LDA		#$40
		STA		_PAD1_REG						; once more frame-irq disable here
;		LDA		#$80							; debug leftovers
;		STA		byte_4083
;		ASL
		LDA		#$00
		STA		_APU_DMC_FREQ
		STA		_APU_DMC_LOAD
;		STA		byte_4083						; debug leftovers
		STA		_mus_fade_out_timer
		LDA		#$0F
		STA		_APU_STATUS						; enable channels except dpcm
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_all_stop:
		LDA		#$00
		STA		_apu_mus_seq_ofs
		STA		_apu_mus_cur_idx
		STA		_mus_fade_out_timer
		STA		_APU_TRI_COUNT

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_all_reset:
		LDX		#$00
		STX		_APU_STATUS
		LDX		#$0F
		STX		_APU_STATUS
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_play_mus:
		LDA		_apu_mus_idx_req				; hi-bit or request means stop all
		BMI		_apu_all_stop					; if non-zero - request to music reload
		BNE		.apu_mus_load					; value is an encoded bit-field
		LDA		_apu_mus_seq_ofs				; non-zero offset means mus currently playing
		BNE		.j_mus_play
		RTS
.j_mus_play:
		JMP		.mus_play
.apu_mus_load:
		STA		_apu_mus_cur_idx				; currently played mus idx as bit-field
; TODO, optimize me
		CMP		#$01							; check for special mus tracks that should
		BEQ		.apu_mus_stop_reload			; stop current mus instantly and start immediately
		CMP		#$04							; tracks 01, 04, 21 and 60 are such tracks
		BEQ		.apu_mus_stop_reload			; (title, game setup, alert, game message)
		CMP		#$21
		BEQ		.apu_mus_stop_reload
		CMP		#$60
		BEQ		.apu_mus_stop_reload
; -
; FIX
;		TAX
;		LDA		_mus_fade_require_list,X
;		BEQ		.apu_mus_stop_reload
; -
		LDA		_apu_mus_seq_ofs				; the rest of tracks are fadable, when requested
		BEQ		.apu_mus_stop_reload			; to play new sound, fade out timer loaded
		BMI		.apu_mus_stop_reload			; it will fade out all tracks until zero, then
		LDA		#$C0							; jump to reload new music idx
		STA		_mus_fade_out_timer
		BNE		.j_mus_play
.apu_mus_stop_reload:
		LDA		_apu_mus_cur_idx				; stop before reload
		JSR		_apu_all_reset					; A is safe here
.apu_mus_reload:
; TODO: optimize me!
		TAX										; decode bitfields:
		ASL										; 01 02 04 05 10 20 21 22 40 41 42 44 48 50 60
		BMI		loc_A09D						; 00 01 02 03 04 05 0D 0E 0C 06 07 08 09 0A 0B
		ASL										; &40!=0  + 6 (40<=X<=60 -> 06-0C)
		BPL		loc_A0A1						; &23!=0  + D (21<=X<=22 -> 21-22)
		ASL										; rest    + 0 (01<=X<=20 -> 00-05)
		BEQ		loc_A0A1						; very criptic
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
		BCC		loc_A0A4						; extract lower bit-field idx
; -
		LDA		_apu_mus_seq_ofs_list,Y			; load sequence offset
		STA		_apu_mus_seq_ofs
		TAY
.mus_trk_data_ofs_load:
		LDA		_apu_mus_seq_ofs_list,Y			; load track data offset in sequence
;		BEQ		_apu_all_stop					; REDUNDANT, there are no zero values there
		CMP		#$FF
		BNE		loc_A0BE						; sequence REPEAT code test
		LDA		_apu_mus_cur_idx				; reload current idx and start from the beginning
;		LDY		#$FF							; REDUNDANT, Y will be initialized by the reload code
		BNE		.apu_mus_reload					; since we store encoded value, go to decode it again
loc_A0BE:
		CMP		#$FE							; sequence NEXT code test
		BNE		.mus_trk_data_load
; TODO: optimize me!
		LDA		_apu_mus_cur_idx				; calculate next mus idx to continue
		ASL										; since this is still a bitfield, we need some more
		BPL		loc_A0CC						; criptic calculations to get the next encoded
		ORA		#$40							; bitfield value.
		BNE		loc_A0D8						; we need to convert 01 to 02, 08 to 10 (<<1, easy)
loc_A0CC:										; also 48 to 50 (<<1|$40, easy?), and finally
		TAX										; 21 to 22 ((<<1&$3F)|$20), EASY?!)
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
		STA		_apu_mus_cur_idx
;		LDY		#$FF							; REDUNDANT, Y will be initialized by the reload code
		BNE		.apu_mus_reload					; unconditional jump
; -
.mus_trk_data_load:
		TAY
		LDA		[_trk00._env]-1],Y				; wave-table offset for current track common for all channels
		STA		_apu_mus_wave
		LDA		[_trk00._ptr]-1],Y				; mus data block ptr for all channels
		STA		_apu_trk_data_ptr
		LDA		[_trk00._ptr],Y
		STA		_apu_trk_data_ptr+1
		LDA		[_trk00._var0]-1],Y				; starting offset for SQUARE2 data
		STA		_apu_mus_p2_data_pos
		LDA		[_trk00._var1]-1],Y
		STA		_apu_mus_tri_data_pos				; starting offset for TRIANGLE data
		LDA		[_trk00._var2]-1],Y
		STA		_apu_mus_envelope				; common envelope parameter for all channels
		LDA		[_trk00._var3]-1],Y
		STA		_apu_mus_dmc_data_pos				; starting offset for DMC data
		LDA		#$01
		STA		_apu_mus_p1_timer				; default values for channel timers, makes
		STA		_apu_mus_p2_timer				; the first fetch load an envelopes first
		STA		_apu_mus_tri_timer
		STA		_apu_mus_dmc_timer
		LDA		#$00
		STA		_apu_mus_p1_data_pos				; starting offset for SQUARE1 always 0 in data block
		LDA		_APU_STATUS						; reset TRIANGLE channel
		AND		#$1B
		STA		_APU_STATUS
;		LDA		_APU_STATUS						; REDUNDANT, already loaded and properly masked
;		AND		#$10
		ORA		#$0F							; re-enable channels
		STA		_APU_STATUS
.mus_play:
		LDA		_mus_fade_out_timer		; if fade out timer is zero, skip
		BEQ		.mus_p1_play
		DEC		_mus_fade_out_timer		; else decrement until zero
		BNE		.mus_p1_play
		LDA		_apu_mus_cur_idx				; when zero, load another mus track
		JMP		.apu_mus_stop_reload
; -
; - start of SQUARE1 channel work
.mus_p1_play:
		DEC		_apu_mus_p1_timer
		BNE		.mus_p1_do_envelope				; note loaded and played, do special envelope work
		LDY		_apu_mus_p1_data_pos				; fetch track cmd, always present in data
		INC		_apu_mus_p1_data_pos
		LDA		(_apu_trk_data_ptr),Y			; CMD: ab--cccc
		BNE		.mus_p1_continues				; CMD=0, end of sequence
		INC		_apu_mus_seq_ofs				; fetch next track offset in sequence
		LDY		_apu_mus_seq_ofs
		JMP		.mus_trk_data_ofs_load
.mus_p1_continues:
		BPL		.mus_p1_load					; a = 0, branch (one-byte 00-7F raw codes)
		TAX										; a = 1, backup for now
		ASL										;
		BMI		.mus_p1_cmdC0					; b test
		LDA		#$00							; b = 0, two-bytes cmd80 codes
		STA		_apu_mus_p1_time_recalc_mode		; p1time recalc mode flag = 0
		TXA										; restore actual code without shift
		BNE		.mus_p1_extra_fetch
; REDUNDANT, no such commands here!, even more, they aren't useful actually
; since we have only 0-15 wave form indexes, we could store waveform delta in two-codes as well
; there may be another music uses more than 00-63 delta range where it may be helpful
.mus_p1_cmdC0:
		LDA		#$80							; b = 1, three-bytes cmdC0 codes
		STA		_apu_mus_p1_time_recalc_mode		; p1time recalc mode flag != 0
		LDY		_apu_mus_p1_data_pos				; read second envelope data byte
		INC		_apu_mus_p1_data_pos
		LDA		(_apu_trk_data_ptr),Y
; -
.mus_p1_extra_fetch:
		AND		#$0F							; wave-form delta
		CLC
		ADC		_apu_mus_wave					; wave-form ofs
		TAY
		LDA		_apu_mus_wave_tbl,Y				; read wave-form delay
		STA		_apu_mus_p1_delay
		LDY		_apu_mus_p1_data_pos
		INC		_apu_mus_p1_data_pos
		LDA		(_apu_trk_data_ptr),Y			; read actual p1time freq
.mus_p1_load:
		JSR		_apu_p1_time_load				; finally load an actual channe freq
		LDA		_apu_mus_p1_delay				; reload timer with delay value for SQUARE1
		STA		_apu_mus_p1_timer
		JSR		_apu_mus_env_delta_calc
		STA		_apu_mus_p1_env_delta
		LDA		#$00
		STA		_apu_mus_p1_env_cnt				; reset env counter for SQIARE1
.mus_p1_do_envelope:
		INC		_apu_mus_p1_env_cnt
		LDY		_apu_mus_p1_env_delta
		BEQ		loc_A1A6
		DEC		_apu_mus_p1_env_delta
loc_A1A6:
		LDA		_apu_mus_envelope
		AND		#$70
		LSR
		LSR
		LSR
		LSR
		TAX
		LDA		_apu_mus_p1_delay
		JSR		_apu_mus_p_sweep_calc
		JSR		_apu_mus_p_fade_out
		STA		_APU_PULSE1_ENV
		LDA		#$7F
		STA		_APU_PULSE1_SWEEP
		LDA		_apu_mus_p1_time_lo
		BEQ		.mus_p2_play
		LDA		_apu_mus_p1_env_cnt
		LDX		_apu_mus_p1_time_lo
		LDY		_apu_mus_p1_time_recalc_mode
		BNE		loc_A1D7
		JSR		_apu_mus_p_time_recalcA
		JMP		loc_A1DC
loc_A1D7:
		LDY		#$00
		JSR		_apu_mus_p_time_recalcB
loc_A1DC:
		STX		_APU_PULSE1_TIME
; - end of SQUARE1 channel work
;
; - start of SQUARE2 channel work
.mus_p2_play:
		DEC		_apu_mus_p2_timer
		BNE		.mus_p2_do_envelope
		LDY		_apu_mus_p2_data_pos
		BNE		.mus_p2_continues				; if no SQAURE2 data (offset = 0)
		LDA		#$50							; then force it to mute, skip to TRI
		STA		_APU_PULSE2_ENV
		JMP		.mus_tri_play
.mus_p2_continues:
		INC		_apu_mus_p2_data_pos
		LDA		(_apu_trk_data_ptr),Y			; fetch data cmd
		BPL		.mus_p2_load					; if cmd 00-7F, load as raw data
		TAX										; the rest is the same as for SQUARE1
		ASL
		BMI		.mus_p2_cmdC0					; once more redundant
		LDA		#$00
		STA		_apu_mus_p2_time_recalc_mode		; recalc mode for SQUARE2
		TXA
		BNE		.mus_p2_extra_fetch
; REDUNDANT as well, there aren't such codes in current music data
;
.mus_p2_cmdC0:
		LDA		#$80
		STA		_apu_mus_p2_time_recalc_mode
		LDY		_apu_mus_p2_data_pos
		INC		_apu_mus_p2_data_pos
		LDA		(_apu_trk_data_ptr),Y
; -
.mus_p2_extra_fetch:
		AND		#$0F							; similar to SQUARE2 load of wave-form data
		CLC
		ADC		_apu_mus_wave
		TAY
		LDA		_apu_mus_wave_tbl,Y
		STA		_apu_mus_p2_delay
		LDY		_apu_mus_p2_data_pos
		INC		_apu_mus_p2_data_pos
		LDA		(_apu_trk_data_ptr),Y			; load an actual data byte
.mus_p2_load:
		LDX		_apu_se_loop_counters+1			; SE1 can be either DMC or SQIARE2, so we may check
		BNE		.mus_p2_skip_load				; only if loop counter is active. for SE0 any can be
		LDX		_apu_se_loop_idx				; SUARE2, so check the loop_idx is active
		BNE		.mus_p2_skip_load				; SE have priority here, skip load music channel here

;		LDX		#$97							; BUG, makes SQUARE2 cracking sometimes
		LDX		#$90							; FIX, for crackling sounds by Chris Covell
		STX		_APU_PULSE2_ENV
		LDX		#$7F
		STX		_APU_PULSE2_SWEEP				; or set the default envelope first...
		JSR		_apu_p2_time_load				; ...then actually load a SQUARE2 data
.mus_p2_skip_load:
		LDA		_apu_mus_p2_delay
		STA		_apu_mus_p2_timer
		JSR		_apu_mus_env_delta_calc
		STA		_apu_mus_p2_env_delta
		LDA		#$00
		STA		_apu_mus_p2_env_cnt
.mus_p2_do_envelope:
		INC		_apu_mus_p2_env_cnt
		LDY		_apu_mus_p2_env_delta
		BEQ		loc_A257
		DEC		_apu_mus_p2_env_delta
loc_A257:
		LDA		_apu_se_loop_counters+1			; here we skip the envelope settings as well
		BNE		.mus_tri_play					; for SE proiority on SQUARE2, but still need calc it!
		LDA		_apu_se_loop_idx				; or else when SE is over, music will garbled
		BNE		.mus_tri_play
		LDA		_apu_mus_envelope
		AND		#$07
		TAX
		LDA		_apu_mus_p2_delay
		JSR		_apu_mus_p_sweep_calc
		JSR		_apu_mus_p_fade_out
		TAX
		LDA		_apu_mus_envelope
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
		STA		_apu_tmpF6
		TXA
		AND		#$F0
		ORA		_apu_tmpF6
		JMP		loc_A293
loc_A292:
		TXA
loc_A293:
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP
		LDA		_apu_mus_p2_time_lo
		BEQ		.mus_tri_play
		LDA		_apu_mus_p2_env_cnt
		LDX		_apu_mus_p2_time_lo
		LDY		_apu_mus_p2_time_recalc_mode
		BNE		loc_A2B1
		JSR		_apu_mus_p_time_recalcA
		JMP		loc_A2B6
loc_A2B1:
		LDY		#$04
		JSR		_apu_mus_p_time_recalcB
loc_A2B6:
		STX		_APU_PULSE2_TIME
; - end of SQIARE2 channel work
;
; - start TRIANGLE chanel work
.mus_tri_play:
		LDA		_mus_fade_out_timer			; the last 16 clocks of fade out
		BEQ		.mus_tri_continues				; TRI/DMC channels are mute, no need to fetch data then
		AND		#$C0
		BNE		.mus_tri_continues				; fade out is over, stop sounds
		LDA		_APU_STATUS						; reset TRIANGLE/NOISE/DMC channels
		AND		#$03
		STA		_APU_STATUS						; activate all except DMC
		ORA		#$0F
		STA		_APU_STATUS
		RTS
.mus_tri_continues:
		DEC		_apu_mus_tri_timer
		BNE		.mus_dmc_play
		LDY		_apu_mus_tri_data_pos
		INC		_apu_mus_tri_data_pos
		LDA		(_apu_trk_data_ptr),Y
		BPL		loc_A2F4
		AND		#$0F
		CLC
		ADC		_apu_mus_wave
		TAY
		LDA		_apu_mus_wave_tbl,Y
		STA		_apu_mus_tri_delay
		LDY		_apu_mus_tri_data_pos
		INC		_apu_mus_tri_data_pos
		LDA		(_apu_trk_data_ptr),Y
loc_A2F4:
		LDX		_apu_se_loop_idx
		BNE		loc_A318
		LDX		_apu_se_loop_idx+1
		BNE		loc_A318
		STA		_apu_tmpF6
		LDX		#$2F
		LDY		_apu_mus_envelope
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
		LDA		_apu_tmpF6
		JSR		_apu_tritime_load
loc_A318:
		LDA		_apu_mus_tri_delay
		STA		_apu_mus_tri_timer
; - end TRIANGLE chanel work
;
; - start DMC chanel work
.mus_dmc_play:
		DEC		_apu_mus_dmc_timer
		BNE		.mus_exit
		LDY		_apu_mus_dmc_data_pos			; also skip fetch if pos = 0
		BEQ		.mus_exit
		INC		_apu_mus_dmc_data_pos			; fetch data
		LDA		(_apu_trk_data_ptr),Y
		STA		_apu_tmpF6
		AND		#$0F
		CLC
		ADC		_apu_mus_wave					; no special codes, data combined with wave info
		TAY
		LDA		_apu_mus_wave_tbl,Y
		STA		_apu_mus_dmc_delay
		LDX		_apu_se_loop_idx+1				; test for any other SE are active now
		BNE		.mus_dmc_load_skip				; then skip DMC load
		LDX		_apu_se_loop_idx
		BNE		.mus_dmc_load_skip
		LDA		_APU_STATUS
		AND		#$10
		BNE		.mus_dmc_load_skip				; dmc active, reload timers only
		LDA		_apu_tmpF6
		AND		#$70
		BEQ		.mus_dmc_load_skip
		AND		#$10
		BEQ		loc_A366
		LDA		#_DPCM_IDX2						; select between two DMC notes
		STA		_APU_DMC_ADDR					; data the same, len/freq are different
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
		LDA		_apu_dpcm_disable_flag			; enable dmc when loaded
		BMI		.mus_dmc_load_skip
		LDA		_APU_STATUS
		ORA		#$10
		STA		_APU_STATUS
.mus_dmc_load_skip:
		LDA		_apu_mus_dmc_delay
		STA		_apu_mus_dmc_timer
; - end DMC chanel work
.mus_exit:
		RTS

;_mus_fade_require_list:
;		.BYTE	$00,$01,$00,$01,$01,$01,$01,$01,$01,$01,$01,$00,$01,$00,$01

; =============== S U B	R O U T	I N E =======================================
_apu_mus_env_delta_calc:
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
_apu_mus_p_sweep_calc:
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
		STA		_apu_tmpF6
		TYA
		CLC
		ADC		_apu_tmpF6
		TAY
		LDA		_mus_p_sweep_tbl0,Y
		BNE		locret_A3CE
loc_A3AE:
		STX		_apu_tmpF7
		INX
		LDA		#$C0
loc_A3B3:
		CLC
		ADC		#$40
		DEX
		BNE		loc_A3B3
		STA		_apu_tmpF6
		TYA
		CLC
		ADC		_apu_tmpF6
		TAY
		LDA		_apu_tmpF7
		AND		#$04
		BNE		loc_A3CB
		LDA		_mus_p_sweep_tbl1,Y
		BNE		locret_A3CE
loc_A3CB:
		LDA		_mus_p_sweep_tbl2,Y
locret_A3CE:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_mus_p_time_recalcA:
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
_apu_mus_p_time_recalcB:
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
		STA		_apu_mus_p1_time_lo,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_mus_p_fade_out:
		LDY		_mus_fade_out_timer
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
		STA		_apu_tmpF6
		TXA
		AND		#$0F
		BEQ		loc_A417
		SEC
		SBC		_apu_tmpF6
		BPL		loc_A417
		LDA		#$01
loc_A417:
		STA		_apu_tmpF6
		TXA
		AND		#$F0
		ORA		_apu_tmpF6
locret_A41E:
		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref_7:
;		LDY		#$7F
;		STY		_APU_PULSE1_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_tritime_load:
		LDX		#$08							; loads TRIANGLE
		BNE		_apu_time_load

; =============== S U B	R O U T	I N E =======================================
_apu_p2_time_load:
		LDX		#$04							; loads SQUARE2
		BNE		_apu_time_load

; =============== S U B	R O U T	I N E =======================================
_apu_p1_time_load:
		LDX		#$00							; LOADS SQUARE1

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_time_load:
		TAY
		LDA		_apu_freq_tbl+1,Y				; low 8-bit nibble of frequency
		STA		_apu_mus_p1_time_lo,X			; store shadow value to RAM
		BEQ		_apu_mute_load					; if zero, mute note instead
		STA		_APU_PULSE1_TIME,X				; load APU with value
		LDA		_apu_freq_tbl,Y					; high 3-bit nibble of frequency

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_len_load:
		ORA		#$08							; set default time 1
		STA		_APU_PULSE1_LEN,X				; load APU with value
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_mute_load:
		LDA		#$50							; playback rate 0, duty 75%, fixed env
		STA		_APU_PULSE1_ENV,X
		LDA		#$00							; frequency 0
		STA		_APU_PULSE1_TIME,X
		BEQ		_apu_len_load

; =============== S U B	R O U T	I N E =======================================
_apu_p2_reset_tri_mute:
		LDA		_APU_STATUS
		AND		#$19
		STA		_APU_STATUS
		ORA		#$0F
		STA		_APU_STATUS
		BNE		_apu_tri_mute_common			; OPTIMIZED
;		LDA		#$01							; REDUNDANT
;		STA		_APU_TRI_COUNT
;		LDA		#$FF
;		STA		_APU_TRI_TIME
;		LDA		#$0F
;		STA		_APU_TRI_LEN
;		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_dmc_reset_tri_mute:
		LDA		_APU_STATUS
		AND		#$0F
		ORA		#$0F
		STA		_APU_STATUS

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_apu_tri_mute_common:
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$FF
		STA		_APU_TRI_TIME
		LDA		#$0F
		STA		_APU_TRI_LEN
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_p2_reset:
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
		JSR		_apu_p2_reset_tri_mute
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$83
		STA		_APU_PULSE2_ENV
		LDA		#$7F
		STA		_APU_PULSE2_SWEEP

; =============== S U B	R O U T	I N E =======================================
_apu_se1_loop:
		LDA		#$6E
		JSR		_apu_tritime_load
		LDA		#$6E
		JSR		_apu_p2_time_load
		DEC		_apu_se_loop_counters
		BNE		locret_A4DC
		JSR		_apu_p2_reset
		LDA		#$00
		STA		_apu_se_loop_idx
locret_A4DC:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se2_start:
;		STY		_apu_se_loop_idx
		LDA		#$18
		STA		_apu_se_loop_counters
		JSR		_apu_p2_reset_tri_mute
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
		JMP		_apu_p2_reset
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
		JSR		_apu_p2_reset_tri_mute
		LDA		#$01
		STA		_APU_TRI_COUNT
		LDA		#$83
		STA		_APU_PULSE2_ENV

; =============== S U B	R O U T	I N E =======================================
_apu_se4_loop:
		LDY		_apu_se_loop_counters
		LDA		byte_A5A3-1,Y
		JSR		_apu_tritime_load
		LDY		_apu_se_loop_counters
		LDA		byte_A5A3,Y
		JSR		_apu_p2_time_load
		DEC		_apu_se_loop_counters
		BNE		locret_A5A2
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_p2_reset
locret_A5A2:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se8_start:
;		STY		_apu_se_loop_idx
		LDA		#$0A
		STA		_apu_se_loop_counters
		JSR		_apu_p2_reset_tri_mute
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
		JSR		_apu_tritime_load
		LDY		_apu_se_loop_counters
		LDA		byte_195834,Y
		JSR		_apu_p2_time_load
		DEC		_apu_se_loop_counters
		BNE		locret_195833
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_p2_reset
locret_195833:
		RTS
byte_195834:
		.BYTE	$02,$3E,$3A,$36,$46,$3C,$02,$46,$46,$3C,$34,$34

; =============== S U B	R O U T	I N E =======================================
_apu_se10_start:
		JSR		_apu_p2_reset_tri_mute
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
		JMP		_apu_p2_reset
locret_A573:
		RTS

; =============== S U B	R O U T	I N E =======================================
_apu_se20_start:
		JSR		_apu_p2_reset_tri_mute
;		STY		_apu_se_loop_idx
		LDA		#$14
		STA		_apu_se_loop_counters

; =============== S U B	R O U T	I N E =======================================
_apu_se20_loop:
		LDA		#$07
		STA		_APU_TRI_COUNT
		LDY		_apu_se_loop_counters
		LDA		byte_195899-1,Y
		JSR		_apu_tritime_load
		DEC		_apu_se_loop_counters
		BNE		locret_195898
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_p2_reset
locret_195898:
		RTS
byte_195899:
		.BYTE	$02,$68,$68,$68,$02,$18,$18,$18,$02,$0E,$0E,$0E,$02,$08,$08,$08
		.BYTE	$02,$04,$04,$04

; =============== S U B	R O U T	I N E =======================================
_apu_se40_start:
		JSR		_apu_p2_reset_tri_mute
;		STY		_apu_se_loop_idx
		LDA		#$14
		STA		_apu_se_loop_counters
		LDA		#$07
		STA		_APU_TRI_COUNT

; =============== S U B	R O U T	I N E =======================================
_apu_se40_loop:
		LDY		_apu_se_loop_counters
		LDA		byte_1958CF-1,Y
		JSR		_apu_tritime_load
		DEC		_apu_se_loop_counters
		BNE		locret_1958CE
		LDA		#$00
		STA		_apu_se_loop_idx
		JMP		_apu_p2_reset
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
		JSR		_apu_p2_reset
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
		JMP		_apu_p2_reset
locret_195876:
		RTS

byte_A5A3:
		.BYTE	$30,$32,$30,$2E
_apu_freq_tbl:
		.BYTE	$07,$F0
		.BYTE	$00,$00
		.BYTE	$00,$D4
		.BYTE	$00,$C8
		.BYTE	$00,$BD
		.BYTE	$00,$B2
		.BYTE	$00,$A8
		.BYTE	$00,$9F
		.BYTE	$00,$96
		.BYTE	$00,$8D
		.BYTE	$00,$85
		.BYTE	$00,$7E
		.BYTE	$00,$76
		.BYTE	$00,$70
		.BYTE	$01,$AB
		.BYTE	$01,$93
		.BYTE	$01,$7C
		.BYTE	$01,$67
		.BYTE	$01,$52
		.BYTE	$01,$3F
		.BYTE	$01,$2D
		.BYTE	$01,$1C
		.BYTE	$01,$0C
		.BYTE	$00,$FD
		.BYTE	$00,$EE
		.BYTE	$00,$E1
		.BYTE	$03,$57
		.BYTE	$03,$27
		.BYTE	$02,$F9
		.BYTE	$02,$CF
		.BYTE	$02,$A6
		.BYTE	$02,$80
		.BYTE	$02,$5C
		.BYTE	$02,$3A
		.BYTE	$02,$1A
		.BYTE	$01,$FC
		.BYTE	$01,$DF
		.BYTE	$01,$C4
		.BYTE	$06,$AE
		.BYTE	$06,$4E
		.BYTE	$05,$F3
		.BYTE	$05,$9E
		.BYTE	$05,$4D
		.BYTE	$05,$01
		.BYTE	$04,$B9
		.BYTE	$04,$75
		.BYTE	$04,$35
		.BYTE	$03,$F8
		.BYTE	$03,$BF
		.BYTE	$03,$89
		.BYTE	$00,$69
		.BYTE	$00,$63
		.BYTE	$00,$5E
		.BYTE	$00,$58
		.BYTE	$00,$53
		.BYTE	$00,$4F
		.BYTE	$00,$4A
		.BYTE	$00,$46
		.BYTE	$00,$42
		.BYTE	$00,$3E
_apu_mus_wave_tbl:
		.BYTE	$03,$06,$0C,$18,$30,$12,$24,$09,$01	; 00
		.BYTE	$04,$08,$10,$20,$40,$18,$30,$0C,$00 ; 09
		.BYTE	$05,$0A,$14,$28,$50,$1E,$3C,$0F,$08 ; 12
		.BYTE	$06,$0C,$18,$30,$60,$24,$48,$12,$08 ; 1B
		.BYTE	$07,$0E,$1C,$38,$70,$2A,$54,$15,$0C ; 24
		.BYTE	$09,$12,$24,$48,$90,$36,$6C,$1B,$03 ; 2D
		.BYTE	$0A,$14,$28,$50,$A0,$3C,$78,$1E,$11 ; 36

_mus_p_sweep_tbl0:
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
_mus_p_sweep_tbl1:
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
_mus_p_sweep_tbl2:
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

	MAC	_SOFS
		.BYTE	[{1}-_apu_mus_seq_ofs_list]
	ENDM

	MAC	_TOFS
		.BYTE	[{1}-_trk00+1]
	ENDM

	MAC	_NEXT
		.BYTE	$FE
	ENDM

	MAC	_REPEAT
		.BYTE	$FF
	ENDM

_apu_mus_seq_ofs_list:
		_SOFS	_seq0	;	$0F
		_SOFS	_seq1	;	$12
		_SOFS	_seq2	;	$16
		_SOFS	_seq3	;	$19
		_SOFS	_seq4	;	$1C
		_SOFS	_seq6	;	$24
		_SOFS	_seq5	;	$1F
		_SOFS	_seq7	;	$28
		_SOFS	_seq8	;	$2C
		_SOFS	_seq9	;	$32
		_SOFS	_seqA	;	$34
		_SOFS	_seqB	;	$3A
		_SOFS	_seqC	;	$3C
		_SOFS	_seqD	;	$3E
		_SOFS	_seqE	;	$40
_seq0:
		_TOFS	_trk00	;	$01
		_TOFS	_trk01	;	$08
		_NEXT			;	$FE
_seq1:
		_TOFS	_trk02	;	$0F
		_TOFS	_trk03	;	$16
		_TOFS	_trk04	;	$1D
		_REPEAT			;	$FF
_seq2:
		_TOFS	_trk05	;	$24
		_TOFS	_trk06	;	$2B
		_REPEAT			;	$FF
_seq3:
		_TOFS	_trk07	;	$32
		_TOFS	_trk07	;	$32
		_NEXT			;	$FE
_seq4:
		_TOFS	_trk08	;	$39
		_TOFS	_trk09	;	$40
		_REPEAT			;	$FF
_seq5:
		_TOFS	_trk0A	;	$47
		_TOFS	_trk0B	;	$4E
		_TOFS	_trk0C	;	$55
		_TOFS	_trk0D	;	$5C
		_REPEAT			;	$FF
_seq6:
		_TOFS	_trk0E	;	$63
		_TOFS	_trk0F	;	$6A
		_TOFS	_trk10	;	$71
		_REPEAT			;	$FF
_seq7:
		_TOFS	_trk11	;	$78
		_TOFS	_trk12	;	$7F
		_TOFS	_trk13	;	$86
		_REPEAT			;	$FF
_seq8:
		_TOFS	_trk14	;	$8D
		_TOFS	_trk15	;	$94
		_TOFS	_trk16	;	$9B
		_TOFS	_trk17	;	$A2
		_TOFS	_trk18	;	$A9
		_REPEAT			;	$FF
_seq9:
		_TOFS	_trk19	;	$B0
		_NEXT			;	$FE
_seqA:
		_TOFS	_trk1A	;	$B7
		_TOFS	_trk1B	;	$BE
		_TOFS	_trk1C	;	$C5
		_TOFS	_trk1D	;	$CC
		_TOFS	_trk1E	;	$D3
		_REPEAT			;	$FF
_seqB:
		_TOFS	_trk1F	;	$DA
		_NEXT			;	$FE
_seqC:
		_TOFS	_trk20	;	$E1
		_REPEAT			;	$FF
_seqD:
		_TOFS	_trk21	;	$E8
		_NEXT			;	$FE
_seqE:
		_TOFS	_trk22	;	$EF
		_REPEAT			;	$FF

	MAC	_TTRK
{1}		SET	.
{1}._env:	.BYTE	{2}
{1}._ptr:	.WORD	{3}
{1}._var0:	.BYTE	{4}
{1}._var1:	.BYTE	{5}
{1}._var2:	.BYTE	{6}
{1}._var3:	.BYTE	{7}
	ENDM

;			    name    chl         ptr      v0   v1   v2   v3
		_TTRK _trk00,	$1B,_trk00_data,	$52, $A5, $D3, $00
		_TTRK _trk01,	$1B,_trk01_data,	$3A, $75, $D3, $A2
		_TTRK _trk02,	$1B,_trk02_data,	$42, $85, $C6, $B1
		_TTRK _trk03,	$1B,_trk03_data,	$32, $65, $C6, $86
		_TTRK _trk04,	$1B,_trk04_data,	$42, $85, $CE, $AD
		_TTRK _trk05,	$24,_trk05_data,	$35, $6B, $C6, $8F
		_TTRK _trk06,	$24,_trk06_data,	$34, $69, $C1, $8F
		_TTRK _trk07,	$1B,_trk07_data,	$15, $29, $00, $00
		_TTRK _trk08,	$1B,_trk08_data,	$3E, $7E, $10, $00
		_TTRK _trk09,	$1B,_trk09_data,	$35, $78, $10, $00
		_TTRK _trk0A,	$24,_trk0A_data,	$26, $4D, $03, $00
		_TTRK _trk0B,	$24,_trk0B_data,	$1A, $3B, $04, $00
		_TTRK _trk0C,	$24,_trk0C_data,	$22, $30, $04, $00
		_TTRK _trk0D,	$24,_trk0D_data,	$11, $42, $00, $00
		_TTRK _trk0E,	$24,_trk0E_data,	$3F, $9D, $01, $00
		_TTRK _trk0F,	$24,_trk0F_data,	$24, $35, $CC, $00
		_TTRK _trk10,	$24,_trk10_data,	$32, $50, $DC, $00
		_TTRK _trk11,	$36,_trk11_data,	$1D, $3B, $46, $00
		_TTRK _trk12,	$36,_trk12_data,	$16, $28, $46, $00
		_TTRK _trk13,	$36,_trk13_data,	$46, $8A, $DE, $00
		_TTRK _trk14,	$2D,_trk14_data,	$2B, $55, $22, $79
		_TTRK _trk15,	$2D,_trk15_data,	$2B, $55, $22, $76
		_TTRK _trk16,	$2D,_trk16_data,	$2C, $57, $22, $78
		_TTRK _trk17,	$2D,_trk17_data,	$2B, $55, $22, $78
		_TTRK _trk18,	$2D,_trk18_data,	$37, $6D, $22, $94
		_TTRK _trk19,	$36,_trk19_data,	$1C, $39, $C6, $00
		_TTRK _trk1A,	$1B,_trk1A_data,	$40, $88, $40, $D0
		_TTRK _trk1B,	$1B,_trk1B_data,	$49, $79, $40, $A9
		_TTRK _trk1C,	$1B,_trk1C_data,	$25, $3D, $40, $55
		_TTRK _trk1D,	$1B,_trk1D_data,	$27, $4F, $83, $72
		_TTRK _trk1E,	$1B,_trk1E_data,	$2D, $5B, $83, $75
		_TTRK _trk1F,	$12,_trk1F_data,	$0A, $13, $00, $00
		_TTRK _trk20,	$12,_trk20_data,	$2F, $41, $40, $00
		_TTRK _trk21,	$24,_trk21_data,	$05, $09, $00, $0D
		_TTRK _trk22,	$24,_trk22_data,	$1E, $3B, $22, $49
_trk00_data:
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
_trk01_data:
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
_trk02_data:
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
_trk03_data:
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
_trk04_data:
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
_trk21_data:
		.BYTE	$80,$02,$83,$02,$00,$80,$02,$83,$02,$80,$02,$83,$02,$80,$83
_trk22_data:
		.BYTE	$81,$2A,$2A,$87,$2A,$80,$2A,$02,$81,$2A,$80,$2A,$81,$2A,$2A,$28
		.BYTE	$28,$87,$28,$80,$28,$02,$81,$28,$80,$28,$81,$28,$28,$00,$81,$22
		.BYTE	$22,$87,$22,$80,$22,$02,$81,$22,$80,$22,$81,$22,$22,$20,$20,$87
		.BYTE	$20,$80,$20,$02,$81,$20,$80,$20,$81,$20,$20,$82,$26,$1C,$26,$81
		.BYTE	$1C,$26,$82,$24,$4A,$24,$81,$4A,$24,$91,$91,$A1,$80,$90,$80,$90
		.BYTE	$91,$A1,$91,$91,$91,$A1,$80,$90,$80,$90,$91,$A1,$91
_trk05_data:
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
_trk06_data:
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
_trk07_data:
		.BYTE	$82,$24,$24,$24,$24,$26,$26,$81,$26,$26,$82,$26,$24,$24,$24,$81
		.BYTE	$24,$26,$84,$02,$00,$82,$4A,$4A,$46,$46,$1C,$1C,$81,$4A,$1C,$82
		.BYTE	$4A,$4A,$4A,$46,$81,$46,$1C,$84,$02,$85,$1C,$81,$1C,$42,$82,$42
		.BYTE	$81,$42,$85,$20,$81,$20,$42,$46,$82,$42,$85,$1C,$81,$1C,$42,$82
		.BYTE	$42,$81,$20,$02,$20,$82,$42,$46,$4A
_trk08_data:
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
_trk09_data:
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
_trk0A_data:
		.BYTE	$82,$22,$81,$20,$22,$82,$26,$81,$30,$2C,$81,$2A,$2C,$2A,$26,$82
		.BYTE	$22,$48,$81,$1C,$20,$82,$22,$85,$48,$81,$42,$82,$44,$81,$48,$82
		.BYTE	$1C,$81,$48,$82,$44,$00,$88,$02,$82,$22,$81,$20,$22,$82,$26,$81
		.BYTE	$30,$2C,$81,$2A,$2C,$2A,$26,$82,$22,$48,$81,$1C,$20,$82,$22,$85
		.BYTE	$48,$81,$42,$82,$44,$81,$48,$82,$1C,$81,$48,$82,$44,$82,$0A,$02
		.BYTE	$08,$02,$04,$02,$2A,$02,$2C,$02,$2A,$02,$26,$02,$22,$02
_trk0B_data:
		.BYTE	$86,$26,$81,$22,$20,$86,$22,$81,$20,$1C,$86,$20,$81,$1C,$48,$82
		.BYTE	$1C,$81,$20,$22,$82,$26,$81,$2A,$22,$00,$82,$1C,$81,$48,$44,$80
		.BYTE	$48,$44,$83,$48,$81,$02,$44,$42,$80,$44,$42,$83,$44,$81,$02,$42
		.BYTE	$3E,$80,$42,$3E,$85,$42,$82,$42,$02,$1C,$02,$82,$20,$02,$2A,$02
		.BYTE	$1C,$02,$26,$02,$48,$02,$22,$02,$44,$02,$46,$02
_trk0C_data:
		.BYTE	$80,$20,$22,$85,$20,$81,$22,$20,$1C,$20,$02,$2C,$2A,$26,$22,$20
		.BYTE	$1C,$20,$80,$22,$20,$86,$22,$81,$02,$02,$2A,$26,$22,$20,$22,$1C
		.BYTE	$20,$00,$83,$1C,$48,$4A,$82,$1C,$20,$84,$42,$83,$3E,$82,$42,$46
		.BYTE	$82,$48,$30,$2C,$44,$42,$46,$4A,$42,$1C,$81,$04,$30,$82,$04,$81
		.BYTE	$1C,$48,$82,$46,$42,$46,$3E
_trk0D_data:
		.BYTE	$81,$22,$20,$1C,$20,$82,$48,$81,$1C,$20,$84,$22,$26,$2A,$26,$48
		.BYTE	$00,$83,$48,$3E,$81,$02,$1E,$1C,$48,$82,$1C,$81,$48,$44,$81,$02
		.BYTE	$22,$20,$1C,$82,$20,$81,$1C,$48,$02,$26,$22,$20,$82,$22,$81,$20
		.BYTE	$1C,$02,$20,$1C,$48,$3E,$48,$3A,$46,$02,$3A,$3E,$34,$80,$38,$34
		.BYTE	$85,$38,$82,$48,$30,$2C,$44,$42,$2A,$44,$2C,$46,$2E,$48,$30,$4A
		.BYTE	$32,$1C,$22,$83,$26,$3E,$48,$30
_trk0E_data:
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
_trk0F_data:
		.BYTE	$83,$2A,$81,$26,$1C,$85,$22,$81,$20,$22,$26,$20,$48,$84,$48,$82
		.BYTE	$02,$83,$2A,$81,$26,$1C,$85,$22,$81,$20,$22,$04,$84,$30,$81,$02
		.BYTE	$22,$26,$2A,$00,$86,$1C,$44,$81,$3E,$42,$84,$42,$82,$02,$86,$1C
		.BYTE	$44,$84,$26,$83,$02,$81,$44,$22,$83,$04,$81,$48,$26,$83,$08,$81
		.BYTE	$3A,$48,$26,$22,$2A,$30,$0E,$0A,$30,$2A,$26,$22,$81,$44,$22,$83
		.BYTE	$04,$81,$48,$26,$83,$08,$81,$3A,$48,$26,$22,$2A,$30,$0E,$0A,$30
		.BYTE	$2A,$26,$22
_trk10_data:
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
_trk11_data:
		.BYTE	$84,$24,$82,$1C,$2A,$87,$28,$80,$20,$84,$46,$82,$28,$84,$26,$82
		.BYTE	$2A,$26,$80,$26,$24,$20,$84,$24,$82,$02,$80,$02,$00,$88,$02,$84
		.BYTE	$24,$82,$1C,$2A,$87,$28,$80,$20,$84,$46,$82,$28,$84,$26,$82,$2A
		.BYTE	$26,$80,$26,$24,$20,$84,$24,$82,$02,$80,$02,$81,$04,$04,$02,$04
		.BYTE	$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04
		.BYTE	$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04
		.BYTE	$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04
_trk12_data:
		.BYTE	$84,$24,$82,$1C,$2A,$87,$28,$80,$20,$84,$46,$82,$28,$84,$26,$82
		.BYTE	$2A,$2C,$86,$04,$08,$00,$84,$42,$82,$3C,$4A,$84,$46,$82,$40,$20
		.BYTE	$84,$44,$82,$48,$1C,$86,$26,$2C,$04,$04,$02,$04,$04,$04,$04,$04
		.BYTE	$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04
		.BYTE	$04,$04,$02,$04,$04,$04,$04,$04,$02,$04,$04,$04,$04,$04,$02,$04
		.BYTE	$04,$04,$30,$30,$02,$30,$30,$30
_trk13_data:
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
_trk14_data:
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
_trk15_data:
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
_trk16_data:
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
_trk17_data:
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
_trk18_data:
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
_trk19_data:
		.BYTE	$86,$2A,$82,$20,$83,$2A,$81,$02,$1C,$20,$26,$83,$2A,$81,$02,$04
		.BYTE	$2E,$26,$83,$2A,$81,$02,$26,$2A,$04,$84,$08,$00,$88,$02,$86,$2A
		.BYTE	$82,$20,$83,$2A,$81,$02,$1C,$20,$26,$83,$2A,$81,$02,$04,$2E,$26
		.BYTE	$83,$2A,$81,$02,$26,$2A,$04,$84,$32,$84,$02,$02,$86,$02,$82,$20
		.BYTE	$2A,$81,$20,$2A,$82,$04,$2C,$84,$2A
_trk1A_data:
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
_trk1B_data:
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
_trk1C_data:
		.BYTE	$84,$12,$81,$02,$80,$14,$04,$81,$0A,$80,$08,$04,$86,$2C,$81,$02
		.BYTE	$80,$2C,$30,$81,$04,$80,$08,$0A,$81,$0E,$83,$04,$81,$02,$08,$30
		.BYTE	$84,$12,$82,$02,$00,$85,$3A,$3A,$82,$38,$3E,$85,$3A,$3A,$82,$38
		.BYTE	$3E,$85,$3A,$3A,$82,$38,$3E,$85,$1C,$1C,$82,$1E,$1E,$85,$2C,$2C
		.BYTE	$82,$2C,$2C,$85,$2C,$2C,$82,$2C,$2C,$85,$2C,$2C,$82,$2C,$2C,$85
		.BYTE	$2A,$2A,$82,$0A,$0A,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$91
		.BYTE	$91,$95,$95,$91,$A1,$91,$91,$95,$95,$91,$A1,$A1,$A1
_trk1D_data:
		.BYTE	$81,$0A,$08,$0A,$83,$18,$81,$02,$0A,$82,$08,$0A,$81,$08,$0A,$84
		.BYTE	$30,$81,$02,$81,$0A,$08,$0A,$83,$18,$81,$02,$0A,$82,$08,$0A,$81
		.BYTE	$08,$0A,$84,$30,$81,$02,$00,$88,$02,$81,$0A,$08,$0A,$83,$18,$81
		.BYTE	$02,$0A,$82,$08,$0A,$81,$08,$0A,$84,$30,$81,$02,$81,$0A,$08,$0A
		.BYTE	$83,$18,$81,$02,$0A,$82,$08,$0A,$81,$08,$0A,$84,$30,$81,$02,$85
		.BYTE	$14,$12,$83,$0E,$81,$02,$12,$85,$14,$12,$83,$0E,$81,$02,$12,$85
		.BYTE	$14,$12,$83,$0E,$81,$02,$12,$85,$14,$12,$82,$0E,$81,$12,$82,$14
		.BYTE	$81,$18,$95,$95,$95,$81,$A1,$81,$95,$95,$95,$81,$A1,$81,$95,$95
		.BYTE	$95,$81,$A1,$81,$95,$95,$95,$81,$A1,$81
_trk1E_data:
		.BYTE	$81,$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$10,$81,$0E,$10,$84
		.BYTE	$06,$81,$02,$81,$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$81,$10
		.BYTE	$81,$16,$14,$16,$85,$6C,$81,$64,$1A,$64,$85,$72,$00,$88,$02,$81
		.BYTE	$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$10,$81,$0E,$10,$84,$06
		.BYTE	$81,$02,$81,$10,$0E,$10,$83,$66,$81,$02,$10,$82,$0E,$81,$10,$81
		.BYTE	$16,$14,$16,$85,$6C,$81,$64,$1A,$64,$85,$72,$85,$1A,$18,$83,$14
		.BYTE	$81,$02,$18,$85,$1A,$18,$83,$14,$81,$02,$18,$85,$1A,$18,$14,$10
		.BYTE	$85,$0C,$08,$04,$32,$95,$95,$95,$81,$A1,$81,$95,$95,$95,$81,$A1
		.BYTE	$81,$95,$95,$95,$81,$A1,$81,$95,$95,$95,$81,$A1,$81
_trk1F_data:
		.BYTE	$82,$04,$04,$81,$08,$82,$0A,$84,$0C,$00,$82,$24,$24,$81,$26,$82
		.BYTE	$28,$84,$2A,$82,$1C,$1C,$81,$1C,$82,$1C,$84,$1C
_trk20_data:
		.BYTE	$82,$02,$81,$2A,$04,$0C,$12,$0E,$02,$0C,$0E,$02,$16,$12,$82,$02
		.BYTE	$02,$81,$64,$85,$02,$02,$82,$02,$81,$2E,$04,$0E,$16,$12,$82,$02
		.BYTE	$02,$81,$04,$82,$0E,$81,$0C,$82,$0E,$81,$12,$85,$02,$02,$00,$86
		.BYTE	$24,$26,$83,$28,$81,$02,$86,$2A,$81,$02,$86,$26,$2A,$2E,$85,$04
		.BYTE	$32,$81,$34,$02,$02,$1C,$02,$34,$38,$02,$02,$20,$02,$38,$3A,$02
		.BYTE	$02,$22,$02,$3A,$3C,$02,$02,$24,$02,$3C,$3E,$02,$02,$26,$02,$3E
		.BYTE	$3C,$02,$02,$24,$02,$3C,$38,$02,$3C,$3E,$02,$42,$02,$02,$38,$42
		.BYTE	$02,$02

		BANK_END F0,$C000
