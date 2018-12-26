
		ORG $A000

		BANK_START $FA

		SECTION_START B01

; =============== S U B	R O U T	I N E =======================================
; this routine called every year and calc last decimal digit of the year
; number
; used ti determide wether the 100year range graphs need to be updated
;
_test_for_new_decade:
		MOVW	_tmpE6,_city._cur_year
		LDY		#$02
loc_10400C:
		LDA		_tmpE6
		CMP		_hex_to_dec_tbl0,Y
		LDA		_tmpE7
		SBC		_hex_to_dec_tbl1,Y
		BCC		loc_104028
		LDA		_tmpE6
		SBC		_hex_to_dec_tbl0,Y
		STA		_tmpE6
		LDA		_tmpE7
		SBC		_hex_to_dec_tbl1,Y
		STA		_tmpE7
		BCS		loc_10400C
loc_104028:
		DEY
		BPL		loc_10400C
		LDA		_tmpE6
		RTS

;_hex_to_dec_999_tbl0:
;		.BYTE	[[10 >> 0] & $FF]		; $0A
;		.BYTE	[[100 >> 0] & $FF]		; $64
;		.BYTE	[[1000 >> 0] & $FF]		; $E8
;_hex_to_dec_999_tbl1:
;		.BYTE	[[10 >> 8] & $FF]		; $00
;		.BYTE	[[100 >> 8] & $FF]		; $00
;		.BYTE	[[1000 >> 8] & $FF]		; $03

		SECTION_END B01

; =============== S U B	R O U T	I N E =======================================
; city_level_R/8
; city_level_C+city_level_I
_RCI_meters_recalc:
		RSMOVW	_tmp6AA,_city._level._R,3		; 6AA = L_R/8
		MOVW	_prev_RCI_levels_sum,_cur_RCI_levels_sum
		LDA		_city._level._I.FRAC			; TODO macrocify me, split
		CLC										; at all costs
		ADC		_city._level._C.FRAC
		STA		_tmp6AC
		STA		_tmpE7
		LDA		_city._level._I.INT
		ADC		_city._level._C.INT
		STA		_tmp6AC+1						; 6AC = L_I+L_C
		STA		_tmpE8							; dividend = L_I+L_C
		LDA		_tmp6AA
		STA		_tmpEA							; divider = L_R/8
		CLC
		ADC		_tmpE7
		STA		_cur_RCI_levels_sum.FRAC
		LDA		_tmp6AA+1
		STA		_tmpEB
		ADC		_tmpE8
		STA		_cur_RCI_levels_sum.INT			; RCI_S = L_I+L_C+(L_R/8)
		LDA		#$00
		STA		_tmpE9
		STA		_tmpE6
		JSR		_div_32to16						; E6 = (L_I+L_C) / (L_R/8)
		BCC		loc_10409D
		LDA		#$00
		STA		_tmpE8							; TODO: optimize me
		STA		_tmpE9
		LDA		#$01
		STA		_tmpE7
		LDA		#$4D
		STA		_tmpE6							; if divider = 0, then result 1.4Dfp
loc_10409D:
		ADDWB	_tmpE6,#$05						; add 0.05fp
		LDX		#$00
		JSR		_RCI_meter_recalc				; R recalc
		MOVW	_tmpE7,_tmp6AA					; dividend = L_R/8
		MOVW	_tmpEA,_tmp6AC					; divider = L_I+L_C
		LDA		#$00
		STA		_tmpE9
		STA		_tmpE6
		JSR		_div_32to16						; E6 = (L_R/8) / (L_I+L_C)
		BCC		loc_1040D8
		LDA		#$01
		STA		_tmpE7							; TODO OPTIMIZE ME
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE8
		STA		_tmpE9							; if divider = 0, then result 1.00fp
loc_1040D8:
		CMPWI	_tmpE6,$014C
		BCC		loc_1040EA
		MOVWI	_tmpE6,$014C					; clamp to 1.4Cfp
loc_1040EA:
		PUSHW	_tmpE6							; backup now
		MOVWI	_tmpE8,$0133					; E6 * 1.33fp
		PUSHW	_tmpE8
		PUSHW	_tmpE6
		JSR		_mmc5_mul16to16					; TODO: optimize me
		PLA
		POPW	_tmpE6							; get _fix8_8 from _fix16x16 result
		PLA
		LDX		#$02
		JSR		_RCI_meter_recalc				; I recalc
		POPW	_tmpE6							; restore backup
		MOVW	_tmpE8,_cur_RCI_levels_sum		; TODO: optimize me
		PUSHW	_tmpE8
		PUSHW	_tmpE6
		JSR		_mmc5_mul16to16
		PUSHB	#$45
		JSR		_mmc5_mul32to8					; here 16x16fp arg already in stack
		PLA
		POPW	_tmpE6
		POPW	_tmpE8
		MOVW	_tmpEA,_city._level._C
loc_104150:
		JSR		_div_32to16
		BCC		loc_10415F
		LDA		#$00							; TODO, optimize me
		STA		_tmpEB
		LDA		#$01
		STA		_tmpEA							; default 1.00
		BEQ		loc_104150
loc_10415F:
		LDX		#$01							; C recalc

; =============== S U B	R O U T	I N E =======================================
_RCI_meter_recalc:
		LDA		_city._delta,X
		BMI		.RCI_meter_negative_delta
		LDA		#$00
		LDY		_required_flags,X
		BNE		loc_1041BD
.RCI_meter_negative_delta:
		LDA		_tmpE6
		LDY		_tmpE7
		BNE		loc_10417B
		EOR		#$FF
		CLC
		ADC		#$01
		JMP		loc_104181
loc_10417B:
		CPY		#$02
		BCC		loc_104181
		LDA		#$FF
loc_104181:
		PHA
		LDA		#$19
		CPX		#$00
		BEQ		loc_10418A
		LDA		#$14
loc_10418A:
		PHA
		JSR		_mmc5_mul8to8
		PLA
		PLA
		LDY		_tmpE7
		BNE		loc_104199
		EOR		#$FF
		CLC
		ADC		#$01
loc_104199:
		STA		_tmpE8
		LDA		_city._tax_rate
		CLC
		ADC		_city._difficult
		CMP		#$14
		BCC		loc_1041A8
		LDA		#$14
loc_1041A8:
		TAY
		LDA		byte_1041E8,Y
		CLC
		ADC		_tmpE8
		CLC
		ADC		_city._delta,X
		BVC		loc_1041BD
		BMI		loc_1041BB
		LDA		#$80
		BNE		loc_1041BD
loc_1041BB:
		LDA		#$7F
loc_1041BD:
		STA		_city._delta,X
		LDA		_city._delta,X					; TODO optimize me
		BPL		loc_1041DB
		EOR		#$FF
		CLC
		ADC		#$01
		PHA
		PUSHB	#$1B
		JSR		_mmc5_mul8to8
		PLA
		PLA
		EOR		#$FF
		CLC
		ADC		#$01
		JMP		loc_1041E4
loc_1041DB:
		PHA
		PUSHB	#$1B
		JSR		_mmc5_mul8to8
		PLA
		PLA
loc_1041E4:
		STA		_RCI_meters_values,X
		RTS
byte_1041E8:
		.BYTE	$0D,$0A,$08,$06,$05,$03,$02,$00,$FF,$FD,$FA,$F6,$F3,$F0,$ED,$EA
		.BYTE	$E6,$E3,$E0,$DD,$DA

; =============== S U B	R O U T	I N E =======================================
_power_grid_recalc:
		JSR		_sram_on
		LDA		#$00
		TAY
loc_104203:
		STA		_power_grid_map_buf,Y			; clear power grid map
		STA		_power_grid_map_buf+$100,Y		; TODO: merge clear code in one proc
		INY
		BNE		loc_104203
		LDA		#$00
		TAY
loc_10420F:
		STA		_power_grid_map_buf+$200,Y
		INY
		CPY		#$D2
		BNE		loc_10420F
		JSR		_sram_off
		PUSHWI	$02BC							; E6 = COL * 700
		PUSHB	_coal_power_station_count
		JSR		_mmc5_mul16to8
		POPD	_tmpE6
		PUSHWI	$07D0
		PUSHB	_nuke_power_station_count		; E6 = E6 + NUCLEAR * 2000
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		BEQ		loc_104253						; if E6 > $FFFF
		LDA		#$FF							; E6 = E6 | $FFFF
		STA		_tmpE6
		STA		_tmpE7
loc_104253:
		MOVW	_tmp6CC,_tmpE6
		MOVWI	_tmp6CE,0
loc_104265:
		LDA		_power_stations_list_pos
		ORA		_power_stations_list_pos+1
		BNE		loc_10426E
		RTS
loc_10426E:
		LDY		_power_stations_list_pos
		LDA		_power_stations_list_pos+1
		BEQ		loc_10427F
		LDX		_tmp_recalc_bufs,Y
		LDA		_tmp_recalc_bufs+$300,Y
		JMP		loc_104285
loc_10427F:
		LDX		_tmp_recalc_bufs+$100,Y
		LDA		_tmp_recalc_bufs+$200,Y
loc_104285:
		STX		_tmp_map_pos._COL
		STA		_tmp_map_pos._ROW
		DEY
		STY		_power_stations_list_pos
		CPY		#$FF
		BNE		loc_104296
		DEC		_power_stations_list_pos+1
loc_104296:
		LDA		#$04
		STA		_tmp6D0
loc_10429B:
		INCW	_tmp6CE
		CMPW	_tmp6CE,_tmp6CC
		BCS		locret_1042F9
		LDX		_tmp6D0
		JSR		_tmp_map_surrounding_tile_pos_select
		JSR		_power_grid_map_bit_set
		LDA		#$00
		STA		_tmp6D2
		STA		_tmp6D0+1
loc_1042C2:
		LDA		_tmp6D0+1
		CMP		#$04
		BCS		loc_1042E7
		LDA		_tmp6D2
		CMP		#$02
		BCS		loc_1042E7
		FJSR	_test_select_map_tile_if_powered_up,SRAM,PRGB
		BCS		loc_1042E2
		INC		_tmp6D2
		LDA		_tmp6D0+1
		STA		_tmp6D0
loc_1042E2:
		INC		_tmp6D0+1
		BNE		loc_1042C2
loc_1042E7:
		LDA		_tmp6D2
		CMP		#$02
		BCC		loc_1042F1
		JSR		_power_stations_map_pos_list_insert
loc_1042F1:
		LDA		_tmp6D2
		BNE		loc_10429B
		JMP		loc_104265						; TODO: optimize me
locret_1042F9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_surrounding_tile_pos_select:
		CPX		#$00
		BNE		loc_104307
		LDA		_tmp_map_pos._ROW
		BEQ		locret_10432E
		DEC		_tmp_map_pos._ROW
		RTS
loc_104307:
		DEX
		BNE		loc_104315
		LDA		_tmp_map_pos._COL
		CMP		#$4B
		BEQ		locret_10432E
		INC		_tmp_map_pos._COL
		RTS
loc_104315:
		DEX
		BNE		loc_104323
		LDA		_tmp_map_pos._ROW
		CMP		#$4B
		BEQ		locret_10432E
		INC		_tmp_map_pos._ROW
		RTS
loc_104323:
		DEX
		BNE		locret_10432E
		LDA		_tmp_map_pos._COL
		BEQ		locret_10432E
		DEC		_tmp_map_pos._COL
locret_10432E:
		RTS

; =============== S U B	R O U T	I N E =======================================
_power_stations_map_pos_list_insert:
		JSR		_sram_on
		LDY		_power_stations_list_pos
		LDX		_power_stations_list_pos+1
		INY
		BNE		loc_10433C
		INX
loc_10433C:
		CPX		#$02
		BEQ		loc_104368
		STY		_power_stations_list_pos
		STX		_power_stations_list_pos+1
		LDY		_power_stations_list_pos
		CPX		#$00
		BEQ		loc_10435C
		LDA		_tmp_map_pos._COL
		STA		_tmp_recalc_bufs,Y
		LDA		_tmp_map_pos._ROW
		STA		_tmp_recalc_bufs+$300,Y
		JMP		loc_104368						; optimize me
loc_10435C:
		LDA		_tmp_map_pos._COL
		STA		_tmp_recalc_bufs+$100,Y
		LDA		_tmp_map_pos._ROW
		STA		_tmp_recalc_bufs+$200,Y
loc_104368:
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_power_grid_map_bit_set:
		LDA		_tmp_map_pos._ROW
		MULAI	#$4C
		LDA		_MMC5_MUL0
		CLC
		ADC		_tmp_map_pos._COL
		STA		_tmpE6
		PHA
		LDA		_MMC5_MUL1
		ADC		#$00
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		STA		_tmpE7
		LDA		_tmpE6
		CLC
		ADC		#<[_power_grid_map_buf]
		STA		_tmpE6
		LDA		_tmpE7
		ADC		#>[_power_grid_map_buf]
		STA		_tmpE7
		PLA
		AND		#$07
		TAX
		LDY		#$00
		LDA		(_tmpE6),Y
		ORA		_inv_bitmasks,X
		JSR		_sram_on
		STA		(_tmpE6),Y
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_game_vars_init:
; FIX, backup difficult if set at the difficult menu. this is temporary
; workaround though, it is easier to do that just moving the difficult
; setting out of the cleared values above ._level
; -
		LDA		_city._difficult
		PHA
; -
		JSR		_sram_on
		LDX		#$00								; X=A=0
		TXA
loc_1043C4:
		STA		_wram_graphs,X						; graphs data clear
		INX
		CPX		#$78
		BNE		loc_1043C4
		JSR		_sram_off
		LDX		#$00								; A=0 still here
loc_1043D1:
		STA		_city._level,X						; part of the city vars clear
		INX
		CPX		#$DF
		BNE		loc_1043D1
		STA		_obj_active_flags					; A=0 still!
; FIX, continues
; -
		PLA											; restore difficult
		STA		_city._difficult
; -
		LDA		#$FF
		STA		_city._year_to_build_bank			; special years clear
		STA		_city._year_to_build_bank+1
		STA		_city._year_to_expo
		STA		_city._year_to_expo+1
		STA		_city._year_to_liberty
		STA		_city._year_to_liberty+1
		LDX		_cur_game_mode						; dependent from the game mode
		BNE		loc_1043FF
		FJSRA	_sram_data_load,SRAM,PRG3			; game mode 0, load saved city data
		JMP		.complete_game_setup				; skip to the final inits
loc_1043FF:
		DEX
		BNE		loc_104428
		LDY		_city._difficult					; game mode 1, new city
		LDA		_difficult_money_list_lo,Y
		STA		_city._money
		LDA		_difficult_money_list_hi,Y
		STA		_city._money+1
		MOVWI	_city._cur_year,1900				; start 1900 year only
;		LDA		#$09								; REDUNDANT, merged
;		STA		_city._game_flags					; setup default game settings
;		LDA		#$00								; REDUNDANT, already 0 here
;		STA		_city._city_type					; city initial type (town)
;		JMP		.complete_game_setup
		JMP		.default_game_settings
loc_104428:
		DEX
		BNE		.practice_init						; game mode 2, scenario
; FIX, continues
; -
		LDA		#$00								; manually set default difficult
		STA		_city._difficult
; -
		LDA		#$02
		STA		_city._game_speed					; default speed "slow" for all scenarios
		MOVWI	_city._money,20000					; fixed money
; BUG: there is no data for 8th scenario "FREEDOM" at all.
;
		LDY		_scenario_idx
; FIX, let's set defaults of this scenario to Practice mode defaults
; -
		CPY		#$07
		BEQ		.practice_init						; special "FREEDOM" case
; -
		LDA		_scenario_start_year_hi,Y			; different starting years
		STA		_city._cur_year+1
		LDA		_scenario_start_year_lo,Y			; init special disasters parameters
		STA		_city._cur_year						; starting year for first disaster,
		CLC											; and disasters treshold after that
		ADC		_scenario_disaster_end_years_list,Y
		STA		_scenario_disaster_end_year			; low nibble of the last year of scenario
		LDA		_scenario_disasters_count_list,Y
		STA		_scenario_disaster_delay
		LDA		_bg_music_for_scenario_start_list,Y
		STA		_city._city_type
		TYA
		ASL
		ASL
		TAY
		LDX		#$00
loc_10445D:
		LDA		_scenario_presents_flags_lib,Y		; disable already accepted present
		STA		_city._events_used_flags,X			; buildings to stop it's appearance
		INY
		INX
		CPX		#$04
		BNE		loc_10445D
;		LDA		#$09								; REDUNDANT, merged
;		STA		_city._game_flags					; default game settings
;		JMP		.complete_game_setup
		JMP		.default_game_settings
.practice_init:
; FIX, continues
; -
		LDA		#$00								; manually set default difficult
		STA		_city._difficult
; -
		MOVWI	_city._money,20000					; game mode 3, practice
		MOVWI	_city._cur_year,1900				; starting year fixed
; REDUNDANT, already 0 here
;		LDA		#$00
;		STA		_city._city_type
.default_game_settings:
		LDA		#$09
		STA		_city._game_flags
.complete_game_setup:
		LDY		_city._city_type					; there is no city type setting for
		LDA		_city._game_flags					; scenario mode here, it will be set
		AND		#$08								; automatically after first recalc ingame
		BEQ		loc_1044A3							; so this music select just redundant,
		LDA		_bg_music_list,Y					; it will always select the default music
		STA		_apu_mus_idx_req					; anyway, this list actually empty by itself
loc_1044A3:
		LDA		#$07
		STA		_city._tax_rate						; default tax rate always 7
		STA		_tax_alloc._tax
		LDA		#$0F
		STA		_police_fund_rate					; def funding rate, will be recalculated anyways
		STA		_fire_fund_rate						; mostly redundant here too
		LDA		#$20
		STA		_road_fund_rate
		LDA		#$26
		STA		_cur_population_dencity
		STA		_cur_population_dencity+1
		JSR		_sram_on
		MOVWO	_tmpE6,_power_grid_map_buf			; clear power grid + traffic maps
		LDX		#$05								; for some reason, the size of clear here is
		LDY		#$00								; 1392, when both bufs should be 1444
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
		CPY		#$70
		BNE		loc_1044DA
		MOVWO	_tmpE6,_land_value_map_buf			; clear the rest of the statistic maps
		LDX		#$03								; size is correct this time
		LDY		#$00
		TYA
loc_1044EE:
		STA		(_tmpE6),Y
		INY
		BNE		loc_1044EE
		INC		_tmpE7
		DEX
		BNE		loc_1044EE
loc_1044F8:
		STA		(_tmpE6),Y
		INY
		CPY		#$ED
		BNE		loc_1044F8
		JSR		_sram_off
		JMP		_city_stats_reset

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
_scenario_disaster_end_years_list:
		.BYTE	$05,$0A,$0A,$05,$05,$0A,$0A
_scenario_presents_flags_lib:
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$1E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
		.BYTE	$0E,$1F,$FF,$FF
_bg_music_for_scenario_start_list:
		.BYTE	$03,$03,$03,$04,$03,$03,$03

; =============== S U B	R O U T	I N E =======================================
_graphs_10_year_data_update:
		LDA		#$00
		STA		_tmpE6
		JSR		_sram_on
		LDX		#$00
loc_104559:
		LDA		_wram_graphs._10y._R+1,X
		STA		_wram_graphs._10y._R,X
		CMP		_tmpE6
		BCC		loc_104565
		STA		_tmpE6
loc_104565:
		LDA		_wram_graphs._10y._C+1,X
		STA		_wram_graphs._10y._C,X
		CMP		_tmpE6
		BCC		loc_104571
		STA		_tmpE6
loc_104571:
		LDA		_wram_graphs._10y._I+1,X
		STA		_wram_graphs._10y._I,X
		CMP		_tmpE6
		BCC		loc_10457D
		STA		_tmpE6
loc_10457D:
		LDA		_wram_graphs._10y._crime+1,X
		STA		_wram_graphs._10y._crime,X
		LDA		_wram_graphs._10y._money+1,X
		STA		_wram_graphs._10y._money,X
		LDA		_wram_graphs._10y._tax_income+1,X
		STA		_wram_graphs._10y._tax_income,X
		INX
		CPX		#$09
		BNE		loc_104559
		LDA		_tmpE6
		STA		_RCI_max_range_10_years			; TODO: fixme! never used for scaling!
		LDA		_city._level._R.FRAC			; BUG! _tmpE6 writeen twice, low nibble
		STA		_tmpE6							; of _R lost, further calcs are correct
		LDA		_city._level._R.INT
		STA		_tmpE6
		ASL		_tmpE6
		ROL
		STA		_wram_graphs._10y._R+9
		LDA		_city._level._C.INT				; TODO: fixme, differnt order of low/hi
		STA		_tmpE6							; nibbles are in different orders
		LDA		_city._level._C.FRAC			; unificato to make a common macro for it
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		STA		_wram_graphs._10y._C+9
		LDA		_city._level._I.INT
		STA		_tmpE6
		LDA		_city._level._I.FRAC
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		STA		_wram_graphs._10y._I+9
		LDA		_city._crime_level+1
		AND		#$7F
		SEC
		SBC		_wram_graphs._10y._crime+8
		LSR
		LSR
		ADC		_wram_graphs._10y._crime+8
		BCC		loc_1045E9
		LDA		#$FF
loc_1045E9:
		STA		_wram_graphs._10y._crime+9
		LDA		_city._money+2
		AND		#$7F
		SEC
		SBC		_wram_graphs._10y._money+8
		LSR
		LSR
		ADC		_wram_graphs._10y._money+8
		BCC		loc_1045FE
		LDA		#$FF
loc_1045FE:
		STA		_wram_graphs._10y._money+9
		LDA		byte_137
		BNE		loc_104625
		LDA		_taxes_income+1
		STA		_tmpE6
		LDA		_taxes_income
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		LDY		_tmpE6
		BNE		loc_104625
		ADC		#$80
		BCC		loc_104627
loc_104625:
		LDA		#$FF
loc_104627:
		STA		_wram_graphs._10y._tax_income+9
		JSR		_sram_off
		LDX		#$00
		LDA		_city._level._R.INT
		CMP		_R_hospitals_count
		BCC		loc_10463B
		BNE		loc_10463F
		BEQ		loc_104641
loc_10463B:
		LDX		#$FF
		BNE		loc_104641
loc_10463F:
		LDX		#$01
loc_104641:
		STX		_hospitals_delta
		LDX		#$00
		LDA		_city._level._R.INT
		CMP		_R_schools_count
		BCC		loc_104652
		BNE		loc_104656
		BEQ		loc_104658
loc_104652:
		LDX		#$FF
		BNE		loc_104658
loc_104656:
		LDX		#$01
loc_104658:
		STX		_schools_delta
		RTS

; =============== S U B	R O U T	I N E =======================================
_graphs_100_year_data_update:
		LDA		#$00
		STA		_tmpE6
		JSR		_sram_on
		LDX		#$00
loc_104665:
		LDA		_wram_graphs._100y._R+1,X
		STA		_wram_graphs._100y._R,X
		CMP		_tmpE6
		BCC		loc_104671
		STA		_tmpE6
loc_104671:
		LDA		_wram_graphs._100y._C+1,X
		STA		_wram_graphs._100y._C,X
		CMP		_tmpE6
		BCC		loc_10467D
		STA		_tmpE6
loc_10467D:
		LDA		_wram_graphs._100y._I+1,X
		STA		_wram_graphs._100y._I,X
		CMP		_tmpE6
		BCC		loc_104689
		STA		_tmpE6
loc_104689:
		LDA		_wram_graphs._100y._crime+1,X
		STA		_wram_graphs._100y._crime,X
		LDA		_wram_graphs._100y._money+1,X
		STA		_wram_graphs._100y._money,X
		LDA		_wram_graphs._100y._tax_income+1,X
		STA		_wram_graphs._100y._tax_income,X
		INX
		CPX		#$09
		BNE		loc_104665
		LDA		_tmpE6
		STA		_RCI_max_range_100_years		; TODO: fixme, the same unused max value
		LDA		_wram_graphs._10y._R+9			; TODO: simple copy cur data for year,
		STA		_wram_graphs._100y._R+9			; may be not so much relevant. maybe need
		LDA		_wram_graphs._10y._C+9			; an interpolation of all 10years graphs instead?
		STA		_wram_graphs._100y._C+9
		LDA		_wram_graphs._10y._I+9
		STA		_wram_graphs._100y._I+9
		LDA		_wram_graphs._10y._crime+9
		STA		_wram_graphs._100y._crime+9
		LDA		_wram_graphs._10y._money+9
		STA		_wram_graphs._100y._money+9
		LDA		_wram_graphs._10y._tax_income+9
		STA		_wram_graphs._100y._tax_income+9
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_fiscal_year_over_calculations:
		LDA		#$00
		STA		_taxes_income
		MOVW	_tmp6CC,_city._tax_year_rate
		MOVWI	_city._tax_year_rate,0
		LDA		_fiscal_year_results_draw_req
		BEQ		loc_1046EB
		RTS
loc_1046EB:
		LDA		_rails_count+1
		STA		_tmpE7
		LDA		_rails_count
		ASL
		ROL		_tmpE7
		CLC
		ADC		_roads_count
		STA		_tmpE6
		LDA		_tmpE7
		ADC		_roads_count+1
		STA		_tmpE7
		PUSHW	_tmpE6							; E6 = (rails_count*2)+_roads_count
		LDY		_city._difficult
		LDA		_trans_fund_coefs_hi,Y
		PHA
;		LDY		_city._difficult				; REDUNDANT, Y already loaded with difficult here
		LDA		_trans_fund_coefs_lo,Y
		PHA
		JSR		_mmc5_mul16to16					; E6 * [1,2/0.9/0.7]*difficult
		PLA
		POPW	_fund_request_trans
		PLA
		MMULB8	_fund_request_police,_police_stations_count,#$64
		MMULB8	_fund_request_fire,_fire_stations_count,#$64
		MMULB8	_fund_request_education,_R_schools_count,#$32
		PUSHW	_cur_RCI_levels_sum
		PUSHW	_city._overall_land_value
		JSR		_mmc5_mul16to16
		PLA
		PLA
		TAX
		PLA
		TAY
		PLA
		TYA
		PHA
		TXA
		PHA
		LDY		_city._difficult
		LDA		_tax_coefs_hi,Y					; * diff[1.4/1.2/0.8]
		PHA
		LDA		_tax_coefs_lo,Y
		PHA
		JSR		_mmc5_mul16to16
		PLA
		PLA
		TAX
		PLA
		TAY
		PLA
		TYA
		PHA
		TXA
		PHA
		PUSHB	#$B6							; 0.71
		JSR		_mmc5_mul16to8
		PLA
		PUSHW	_tmp6CC							; *tax value
		JSR		_mmc5_mul16to16
		PLA
		POPD	_taxes_collected
		LDA		_cur_RCI_levels_sum.FRAC
		ORA		_cur_RCI_levels_sum.INT
		BNE		loc_1047BF
		LDA		#$0F
		STA		_police_fund_rate
		STA		_fire_fund_rate
		RTS
loc_1047BF:
		LDA		#$01
		STA		_fiscal_year_results_draw_req
		STA		_game_core_state
loc_1047C7:
		LDA		_fiscal_year_results_draw_req
		BNE		loc_1047C7
		LDA		_city._money
		CLC
		ADC		_taxes_collected
		STA		_city._money
		LDA		_city._money+1
		ADC		_taxes_collected+1
		STA		_city._money+1
		BCC		loc_1047E4
		INC		_city._money+2
loc_1047E4:
		SUBDW	_city._money,_road_fund_alloc
		SUBDW	_city._money,_fire_fund_alloc
		SUBDW	_city._money,_police_fund_alloc
		SUBDW	_city._money,_education_fund_alloc
		LDA		_city._loan_period				; if loan period isn't zero
		BEQ		loc_10486C
		DEC		_city._loan_period				; decrement
		BNE		loc_104856
		LDA		_city._bank_flags				; if the last year, clear loan flag
		AND		#$BF
		STA		_city._bank_flags
loc_104856:
		SUBDI16	_city._money,500				; loan amount hardcoded here, the var never used
loc_10486C:
		LDA		_taxes_collected
		SEC
		SBC		_fund_request_trans
		STA		_tmpE6
		LDA		_taxes_collected+1
		SBC		_fund_request_trans+1
		STA		_tmpE7
		LDA		_tmpE6
		SEC
		SBC		_fund_request_fire
		STA		_tmpE6
		LDA		_tmpE7
		SBC		_fund_request_fire+1
		STA		_tmpE7
		LDA		_tmpE6
		SEC
		SBC		_fund_request_police
		STA		_taxes_income
		LDA		_tmpE7
		SBC		_fund_request_police+1
		STA		_taxes_income+1
		LDA		_fund_request_trans
		ORA		_fund_request_trans+1
		BNE		loc_1048A9
		LDA		#$20
		BNE		loc_1048D7
loc_1048A9:
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		LDA		_road_fund_alloc
		STA		_tmpE7
		LDA		_road_fund_alloc+1
		STA		_tmpE8
		LDA		_fund_request_trans
		STA		_tmpEA
		LDA		_fund_request_trans+1
		STA		_tmpEB
		JSR		_div_32to16
		PUSHW	_tmpE6
		PUSHB	#$21
		JSR		_mmc5_mul16to8
		PLA
		PLA
		TAX
		PLA
		TXA
loc_1048D7:
		STA		_road_fund_rate
		LDA		_fund_request_police
		ORA		_fund_request_police+1
		BNE		loc_1048E6
		LDA		#$0F
		BNE		loc_104914
loc_1048E6:
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		LDA		_police_fund_alloc
		STA		_tmpE7
		LDA		_police_fund_alloc+1
		STA		_tmpE8
		LDA		_fund_request_police
		STA		_tmpEA
		LDA		_fund_request_police+1
		STA		_tmpEB
		JSR		_div_32to16
		LDA		_tmpE6
		LDY		_tmpE7
		BEQ		loc_10490B
		LDA		#$FF
loc_10490B:
		PHA
		PUSHB	#$10
		JSR		_mmc5_mul8to8
		PLA
		PLA
loc_104914:
		STA		_police_fund_rate
		LDA		_fund_request_fire
		ORA		_fund_request_fire+1
		BNE		loc_104923
		LDA		#$0F
		BNE		loc_104951
loc_104923:
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		LDA		_fire_fund_alloc
		STA		_tmpE7
		LDA		_fire_fund_alloc+1
		STA		_tmpE8
		LDA		_fund_request_fire
		STA		_tmpEA
		LDA		_fund_request_fire+1
		STA		_tmpEB
		JSR		_div_32to16
		LDA		_tmpE6
		LDY		_tmpE7
		BEQ		loc_104948
		LDA		#$FF
loc_104948:
		PHA
		PUSHB	#$10
		JSR		_mmc5_mul8to8
		PLA
		PLA
loc_104951:
		STA		_fire_fund_rate
		RTS
_trans_fund_coefs_lo:
		.BYTE	$B3,$E6,$33
_trans_fund_coefs_hi:
		.BYTE	$00,$00,$01
_tax_coefs_lo:
		.BYTE	$66,$33,$CC
_tax_coefs_hi:
		.BYTE	$01,$01,$00

; =============== S U B	R O U T	I N E =======================================
_city_stats_recalc:
		LDA		_cur_RCI_levels_sum.FRAC
		ORA		_cur_RCI_levels_sum.INT
		BEQ		loc_10497B
		JSR		_city_assessed_value_recalc
		JSR		_city_popul_relalc
		JSR		_city_problems_list_recalc
		JSR		_city_score_recalc
		JSR		_city_votes_recalc
		JMP		loc_10497E
loc_10497B:
		JSR		_city_stats_reset
loc_10497E:
		LDA		#$01
		RTS

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
		STA		_annual_score_change
		STA		_annual_score_change+1
		MOVWI	_city._score,500
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_assessed_value_recalc:
		PUSHW	_roads_count
		PUSHB	#$05
		JSR		_mmc5_mul16to8
		POPD	_tmpE6
		PUSHW	_rails_count
		PUSHB	#$0A
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$03E8
		LDA		_police_stations_count
		CLC
		ADC		_wrong_var					; BUG! unknown unused var
		PHA
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$9001						; BUG! SWAP NIBBLES
		PUSHB	_R_hospitals_count
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$B80B						; BUG! SWAP NIBBLES
		PUSHB	_stadiums_count
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$8813						; BUG! SWAP NIBBLES
		PUSHB	_sea_ports_count
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$1027						; BUG! SWAP NIBBLES
		PUSHB	_airports_count
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$B80B						; BUG! SWAP NIBBLES
		PUSHB	_coal_power_station_count
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		STA		_tmpE8
		PUSHWI	$7017						; BUG! SWAP NIBBLES
		PUSHB	_nuke_power_station_count
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6
		STA		_assessed_value
		PLA
		ADC		_tmpE7
		STA		_assessed_value+1
		PLA
		ADC		_tmpE8
		STA		_assessed_value+2
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_popul_relalc:
		LDA		_popul_cur
		SEC
		SBC		_popul_last_year
		STA		_popul_net_migration
		LDA		_popul_cur+1
		SBC		_popul_last_year+1
		STA		_popul_net_migration+1
		LDA		_popul_cur+2
		SBC		_popul_last_year+2
		STA		_popul_net_migration+2
		MOVW	_popul_last_year,_popul_cur
		LDA		_popul_cur+2
		STA		_popul_last_year+2
		LDY		#$05
loc_104AF0:
		LDA		_popul_cur
		CMP		byte_104B17,Y
		LDA		_popul_cur+1
		SBC		byte_104B11,Y
		LDA		_popul_cur+2
		SBC		byte_104B0B,Y
		BCS		loc_104B07
		DEY
		BPL		loc_104AF0
loc_104B07:
		STY		_city._city_type
		RTS
byte_104B0B:
		.BYTE	$00,$00,$00,$00,$01,$07
byte_104B11:
		.BYTE	$00,$07,$27,$C3,$86,$A1
byte_104B17:
		.BYTE	$00,$D0,$10,$50,$A0,$20

; =============== S U B	R O U T	I N E =======================================
_city_problems_list_recalc:
		LDA		_city._crime_level
		STA		_tmp6CE
		LDA		_city._crime_level+1
		AND		#$7F
		STA		_tmp6CE+1
		LDA		_city._var_5FC
		STA		_tmp6D0
		LDA		_city._var_5FD
		AND		#$7F
		STA		_tmp6D0+1
		PUSHW	_city._overall_land_value
		PUSHB	#$A0
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmp6D2
		MMULB8	_tmp6D4,_city._tax_rate,#$0A
		FJSR	_traffic_jam_recalc,SRAM,PRGB
		MOVW	_tmp6D6,_traffic_level_value
		JSR		sub_104C2B
		STA		_tmp6D8
		JSR		_wrong_var_mul5
		STA		_tmp6DA
		LDA		#$00
		STA		_tmp6D9
		STA		_tmp6DB
		JSR		sub_104BCE
		LDY		#$00
		TYA
loc_104B8F:
		STA		byte_6DC,Y
		INY
		CPY		#$07
		BNE		loc_104B8F
		LDX		#$00
loc_104B99:
		LDY		#$00
		STY		_tmpE6
loc_104B9D:
		LDA		byte_6DC,Y
		BNE		loc_104BB7
		LDA		byte_6E3,Y
		CMP		_tmpE6
		BCC		loc_104BB7
		STA		_tmpE6
		STA		_worst_problem_percents,X
		TYA
		STA		_worst_problem_list,X
		LDA		#$01
		STA		byte_6DC,Y
loc_104BB7:
		INY
		CPY		#$07
		BNE		loc_104B9D
		LDA		_tmpE6
		BNE		loc_104BC8
		LDA		#$FF
		STA		_worst_problem_percents,X
		STA		_worst_problem_list,X
loc_104BC8:
		INX
		CPX		#$04
		BNE		loc_104B99
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_104BCE:
		LDY		#$00
		TYA
loc_104BD1:
		STA		byte_6E3,Y
		INY
		CPY		#$07
		BNE		loc_104BD1
		STA		_tmpE8
		STA		_tmpE9
		STA		_tmpEA
		STA		_tmpEB
loc_104BE1:
		PUSHWI	$0190
		JSR		_rand
		PHA
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmpE6
		LDA		_tmpE8
		ASL
		TAX
		LDA		_tmpE6
		CMP		_tmp6CE,X
		LDA		_tmpE7
		SBC		_tmp6CE+1,X
		BCS		loc_104C0C
		LDX		_tmpE8
		INC		byte_6E3,X
		INC		_tmpE9
loc_104C0C:
		INC		_tmpE8
		LDA		_tmpE8
		CMP		#$07
		BCC		loc_104C18
		LDA		#$00
		STA		_tmpE8
loc_104C18:
		INC		_tmpEA
		BNE		loc_104C1E
		INC		_tmpEB
loc_104C1E:
		LDA		_tmpEB
		CMP		#$02
		BCS		locret_104C2A
		LDA		_tmpE9
		CMP		#$64
		BCC		loc_104BE1
locret_104C2A:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_104C2B:
		LDA		_city._level._C.FRAC
		CLC
		ADC		_city._level._I.FRAC
		STA		_tmpE6
		LDA		_city._level._C.INT
		ADC		_city._level._I.INT
		ASL		_tmpE6
		ROL
		ASL		_tmpE6
		ROL
		ASL		_tmpE6
		ROL
		STA		_tmpE7
		ORA		_tmpE6
		BNE		loc_104C4C
		LDA		#$00
		RTS
loc_104C4C:
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		LDA		_city._level._R.FRAC
		STA		_tmpE7
		LDA		_city._level._R.INT
		STA		_tmpE8
		JSR		_div_32to16
		LDA		_tmpE6
		LDY		_tmpE7
		BEQ		locret_104C67
		LDA		#$FF
locret_104C67:
		RTS

; =============== S U B	R O U T	I N E =======================================
_wrong_var_mul5:
		LDA		_wrong_var
		ASL
		ASL
		CLC
		ADC		_wrong_var
		BCC		locret_104C75
		LDA		#$FF
locret_104C75:
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_score_recalc:
		MOVW	_prev_city_score,_city._score
		LDY		#$00
		STY		_tmpE6
		STY		_tmpE7
		STY		_tmpE8
		STY		_tmpE9
		STY		_tmpEA
loc_104C8E:
		LDA		_tmp6CE,Y
		CLC
		ADC		_tmpE6
		STA		_tmpE6
		LDA		_tmp6CE+1,Y
		ADC		_tmpE7
		STA		_tmpE7
		INY
		CPY		#$07
		BNE		loc_104C8E
		LDA		#$03
		STA		_tmpEB
		JSR		_div_32to16
		LDA		_tmpE7
		BEQ		loc_104CB1
		LDA		#$FF
		STA		_tmpE6
loc_104CB1:
		LDA		#$00
		STA		_tmpE7
		LDA		_tmpE6
		EOR		#$FF
		CLC
		ADC		#$01
		ASL
		ROL		_tmpE7
		ASL
		ROL		_tmpE7
		LDA		_tmpE6
		CMP		#$E8
		LDA		_tmpE7
		SBC		#$03
		BCC		loc_104CD4
		LDA		#$E8
		STA		_tmpE6
		LDA		#$03
		STA		_tmpE7
loc_104CD4:
		LDA		_required_flags._stadium
		BEQ		loc_104CDC
		JSR		_tmpE6_multiply_217
loc_104CDC:
		LDA		_required_flags._seaport
		BEQ		loc_104CE4
		JSR		_tmpE6_multiply_217
loc_104CE4:
		LDA		_required_flags._airport
		BEQ		loc_104CEC
		JSR		_tmpE6_multiply_217
loc_104CEC:
		LDA		_road_fund_rate
		CMP		#$20
		BEQ		loc_104D0A
		LDA		_tmpE6
		CLC
		ADC		#$20
		STA		_tmpE6
		BCC		loc_104CFE
		INC		_tmpE7
loc_104CFE:
		LDA		_tmpE6
		SEC
		SBC		_road_fund_rate
		STA		_tmpE6
		BCS		loc_104D0A
		DEC		_tmpE7
loc_104D0A:
		LDA		_police_fund_rate
		CMP		#$0F
		BCS		loc_104D14
		JSR		sub_104E54
loc_104D14:
		LDA		_fire_fund_rate
		CMP		#$0F
		BCS		loc_104D1E
		JSR		sub_104E54
loc_104D1E:
		LDA		_city._delta._R
		CMP		#$40
		BVC		loc_104D28
		JSR		_tmpE6_multiply_217
loc_104D28:
		LDA		_city._delta._C
		CMP		#$40
		BVC		loc_104D32
		JSR		_tmpE6_multiply_217
loc_104D32:
		LDA		_city._delta._I
		CMP		#$40
		BVC		loc_104D3C
		JSR		_tmpE6_multiply_217
loc_104D3C:
		LDA		_popul_cur
		ORA		_popul_cur+1
		ORA		_popul_cur+2
		BEQ		loc_104D6A
		LDA		_popul_net_migration
		ORA		_popul_net_migration+1
		ORA		_popul_net_migration+2
		BEQ		loc_104D6A
		LDA		_popul_cur
		CMP		_popul_net_migration
		BNE		loc_104D6D
		LDA		_popul_cur+1
		CMP		_popul_net_migration+1
		BNE		loc_104D6D
		LDA		_popul_cur+2
		CMP		_popul_net_migration+2
		BNE		loc_104D6D
loc_104D6A:
		JMP		loc_104DF9
loc_104D6D:
		LDA		_popul_net_migration+2
		BMI		loc_104DAE
		PUSHW	_tmpE6
		LDA		#$00
		STA		_tmpE9
		LDA		_popul_net_migration
		STA		_tmpE6
		LDA		_popul_net_migration+1
		STA		_tmpE7
		LDA		_popul_net_migration+2
		STA		_tmpE8
		LDA		_popul_cur+1
		STA		_tmpEA
		LDA		_popul_cur+2
		STA		_tmpEB
		JSR		_div_32to16
		INC		_tmpE7
		PUSHW	_tmpE6
		JSR		_mmc5_mul16to16
		PLA
		POPW	_tmpE8
		PLA
		JMP		loc_104DF9
loc_104DAE:
		PUSHW	_tmpE6
		LDA		#$00
		STA		_tmpE9
		LDA		#$00
		SEC
		SBC		_popul_net_migration
		STA		_tmpE6
		LDA		#$00
		SBC		_popul_net_migration+1
		STA		_tmpE7
		LDA		#$00
		SBC		_popul_net_migration+2
		STA		_tmpE8
		LDA		_popul_cur+1
		STA		_tmpEA
		LDA		_popul_cur+2
		STA		_tmpEB
		JSR		_div_32to16
		LDA		#$01
		SEC
		SBC		_tmpE7
		STA		_tmpE7
		LDA		#$00
		SBC		_tmpE6
		STA		_tmpE6
		PUSHW	_tmpE6
		JSR		_mmc5_mul16to16
		PLA
		POPW	_tmpE8
		PLA
loc_104DF9:
		LDA		_tmpE7
		BPL		loc_104E05
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE7
		BEQ		loc_104E17
loc_104E05:
		LDA		_tmpE6
		CMP		#$E8
		LDA		_tmpE7
		SBC		#$03
		BCC		loc_104E17
		LDA		#$E8
		STA		_tmpE6
		LDA		#$03
		STA		_tmpE7
loc_104E17:
		LDA		_city._score
		CLC
		ADC		_tmpE6
		STA		_city._score
		LDA		_city._score+1
		ADC		_tmpE7
		LSR
		STA		_city._score+1
		ROR		_city._score
		LDA		_city._score
		SEC
		SBC		_prev_city_score
		STA		_annual_score_change
		LDA		_city._score+1
		SBC		_prev_city_score+1
		STA		_annual_score_change+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmpE6_multiply_217:
		PUSHW	_tmpE6
		PUSHB	#$D9
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmpE6
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_104E54:
		CLC
		ADC		#$F1
		TAX
		PUSHW	_tmpE6
		TXA
		PHA
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmpE6
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_votes_recalc:
		LDA		#$00
		STA		_doing_a_good_job_votes
		STA		_doing_a_bad_job_votes
		LDY		#$64
loc_104E75:
		PUSHWI	$03E8
		JSR		_rand
		PHA
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmpE6
		LDA		_tmpE6
		CMP		_city._score
		LDA		_tmpE7
		SBC		_city._score+1
		BCS		loc_104E9A
		INC		_doing_a_good_job_votes
		BNE		loc_104E9D
loc_104E9A:
		INC		_doing_a_bad_job_votes
loc_104E9D:
		DEY
		BNE		loc_104E75
		RTS

; =============== S U B	R O U T	I N E =======================================
_events_manager:
		JSR		_scenario_result_test
		JSR		_practice_mode_events_handler
		JSR		_scroll_msg_manager
		LDA		_game_msg_awaits_flag
		BNE		locret_104ED8
		LDA		_game_msg_idx
		STA		_prev_game_msg_idx
		LDX		#$00
		LDY		#$00
loc_104EB7:
		LDA		_city._events_used_flags,X
		STA		_tmpE6
loc_104EBC:
		LSR		_tmpE6
		BCS		loc_104ECB
		TXA
		PHA
		TYA
		PHA
		JSR		_presents_events_check
		PLA
		TAY
		PLA
		TAX
loc_104ECB:
		INY
		CPY		#$1B
		BEQ		locret_104ED8
		TYA
		AND		#$07
		BNE		loc_104EBC
		INX
		BNE		loc_104EB7
locret_104ED8:
		RTS

; =============== S U B	R O U T	I N E =======================================
_presents_events_check:
		LDA		_presents_events_lib_lo,Y
		STA		_tmpE7
		LDA		_presents_events_lib_hi,Y
		STA		_tmpE8
		JMP		(_tmpE7)
_presents_events_lib_lo:
		.BYTE	<[nullsub_1]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event06_total_jam]
		.BYTE	<[_event07_traffic]
		.BYTE	<[_event08_crime]
		.BYTE	<[nullsub_3]
		.BYTE	<[nullsub_3]
		.BYTE	<[_event0B_bank]
		.BYTE	<[_event0C_relationships]
		.BYTE	<[_event0D_amusement]
		.BYTE	<[_event0E_zoo]
		.BYTE	<[_event0F_casino]
		.BYTE	<[_event10_extra_land]
		.BYTE	<[_event11_new_police]
		.BYTE	<[_event12_new_fire]
		.BYTE	<[_event13_10th_monument]
		.BYTE	<[_event14_100th_monument]
		.BYTE	<[_event15_expo]
		.BYTE	<[_event16_expo_monument]
		.BYTE	<[_event17_liberty]
		.BYTE	<[_event18_library]
		.BYTE	<[_event19_park]
		.BYTE	<[_event1A_railroad]
_presents_events_lib_hi:
		.BYTE	>[nullsub_1]
		.BYTE	>[_event01_city_growt]
		.BYTE	>[_event01_city_growt]
		.BYTE	>[_event01_city_growt]
		.BYTE	>[_event01_city_growt]
		.BYTE	>[_event01_city_growt]
		.BYTE	>[_event06_total_jam]
		.BYTE	>[_event07_traffic]
		.BYTE	>[_event08_crime]
		.BYTE	>[nullsub_3]
		.BYTE	>[nullsub_3]
		.BYTE	>[_event0B_bank]
		.BYTE	>[_event0C_relationships]
		.BYTE	>[_event0D_amusement]
		.BYTE	>[_event0E_zoo]
		.BYTE	>[_event0F_casino]
		.BYTE	>[_event10_extra_land]
		.BYTE	>[_event11_new_police]
		.BYTE	>[_event12_new_fire]
		.BYTE	>[_event13_10th_monument]
		.BYTE	>[_event14_100th_monument]
		.BYTE	>[_event15_expo]
		.BYTE	>[_event16_expo_monument]
		.BYTE	>[_event17_liberty]
		.BYTE	>[_event18_library]
		.BYTE	>[_event19_park]
		.BYTE	>[_event1A_railroad]

; =============== S U B	R O U T	I N E =======================================
nullsub_1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_event01_city_growt:
		TYA
		TAX
		DEX
		JSR		_popul_compare
		BCC		nullsub_4
		TYA
		PHA
		STY		_city._city_type
		JSR		_city_history_write
		PLA
		TAY
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event06_total_jam:
		LDA		_traffic_level_value+1
		BMI		nullsub_4
		CMP		#$07
		BCC		nullsub_4
		ORA		#$80
		STA		_traffic_level_value+1
		JMP		_new_event_set

; =============== S U B	R O U T	I N E =======================================
_event07_traffic:
		LDA		_traffic_level_value+1
		BMI		nullsub_4
		CMP		#$02
		BCC		nullsub_4
		ORA		#$80
		STA		_traffic_level_value+1
		JMP		_new_event_set

; =============== S U B	R O U T	I N E =======================================
_event08_crime:
		LDA		_city._crime_level+1
		BMI		nullsub_4
		CMP		#$07
		BCC		nullsub_4
		ORA		#$80
		STA		_city._crime_level+1
		JMP		_new_event_set

; =============== S U B	R O U T	I N E =======================================
nullsub_3:
		RTS

; =============== S U B	R O U T	I N E =======================================
_event0B_bank:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDA		_city._cur_year
		CMP		_city._year_to_build_bank
		LDA		_city._cur_year+1
		SBC		_city._year_to_build_bank+1
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
nullsub_4:
		RTS

; =============== S U B	R O U T	I N E =======================================
_event0C_relationships:
		LDX		#$05
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event0D_amusement:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDX		#$06
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event0E_zoo:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDX		#$07
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event0F_casino:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDX		#$08
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event10_extra_land:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDA		_sea_ports_count
		CMP		#$02
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event11_new_police:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDA		_police_stations_count
		CMP		#$0A
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event12_new_fire:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDA		_fire_stations_count
		CMP		#$0A
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event13_10th_monument:
		JSR		_check_for_free_present_slot
		BNE		nullsub_4
		LDA		_city._cur_year
		CMP		#$76
		LDA		_city._cur_year+1
		SBC		#$07
		BCC		nullsub_4
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event14_100th_monument:
		JSR		_check_for_free_present_slot
		BNE		nullsub_2
		LDA		_city._cur_year
		CMP		#$D0
		LDA		_city._cur_year+1
		SBC		#$07
		BCC		nullsub_2
		JMP		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event15_expo:
		LDA		_city._cur_year
		CMP		#$9E
		BNE		nullsub_2
		LDA		_city._cur_year+1
		CMP		#$07
		BNE		nullsub_2
		BEQ		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event16_expo_monument:
		JSR		_check_for_free_present_slot
		BNE		nullsub_2
		LDA		_city._cur_year
		CMP		_city._year_to_expo
		LDA		_city._cur_year+1
		SBC		_city._year_to_expo+1
		BCC		nullsub_2
		BCS		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event17_liberty:
		JSR		_check_for_free_present_slot
		BNE		nullsub_2
		LDA		_city._cur_year
		CMP		_city._year_to_liberty
		LDA		_city._cur_year+1
		SBC		_city._year_to_liberty+1
		BCC		nullsub_2
		BCS		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
_event18_library:
		JSR		_check_for_free_present_slot
		BNE		nullsub_2
		LDX		#$09
		JSR		_popul_compare
		BCC		nullsub_2
		BCS		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
; 200 parks needed
_event19_park:
		JSR		_check_for_free_present_slot
		BNE		nullsub_2
		LDA		_parks_counter
		CMP		#$C8
		BCC		nullsub_2
		BCS		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
; 150 railroars	needed
_event1A_railroad:
		JSR		_check_for_free_present_slot
		BNE		nullsub_2
		LDA		_rails_count
		CMP		#$96
		BCC		nullsub_2
		BCS		_new_event_register_flag

; =============== S U B	R O U T	I N E =======================================
nullsub_2:
		RTS

; =============== S U B	R O U T	I N E =======================================
_new_event_register_flag:
		STY		_game_msg_idx
		TYA
		LSR
		LSR
		LSR
		TAX
		TYA
		AND		#$07
		TAY
		LDA		_city._events_used_flags,X
		ORA		_bitmasks,Y
		STA		_city._events_used_flags,X
		BNE		_new_event_break

; =============== S U B	R O U T	I N E =======================================
_new_event_set:
		STY		_game_msg_idx

; =============== S U B	R O U T	I N E =======================================
_new_event_break:
; BUG! Y here is not the same after _new_event_register_flag branch, than
; from previous !FALLTHROUGH! entry lol, will fail always, whey event idx
; is pow of 2^3, so it will be 0 and test with 0, then never appear
; automatically
;
		CPY		_prev_game_msg_idx
		BEQ		loc_105059
		LDA		#$01
		STA		_game_msg_awaits_flag
loc_105059:
		PLA
		PLA
		PLA
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_check_for_free_present_slot:
		LDX		#$02
		LDA		_city._events_used_flags			; test if city status acquired
		AND		#$04
		BEQ		loc_105069							; if no, only 3 slots are available
		LDX		#$03
loc_105069:
		LDA		_city._presents_list,X		; technically calc here is correct
; BUG! loop not needed.
		BEQ		locret_105071						; it'll always get last slot
		DEY											; but then DEY loop not needed here then
		BPL		loc_105069
locret_105071:
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_history_write:
		TYA
		PHA
		LDY		#$00
loc_105076:
		LDA		_city._history_list,Y
		BEQ		loc_10508F
		INY
		INY
		INY
		CPY		#$15
		BNE		loc_105076
		LDY		#$00
loc_105084:
		LDA		_city._history_list+3,Y
		STA		_city._history_list,Y
		INY
		CPY		#$12
		BNE		loc_105084
loc_10508F:
		PLA
		STA		_city._history_list,Y
		LDA		_city._cur_year
		STA		_city._history_list+1,Y
		LDA		_city._cur_year+1
		STA		_city._history_list+2,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_popul_compare:
		LDA		_popul_cur
		CMP		_popul_vals_lib0,X
		LDA		_popul_cur+1
		SBC		_popul_vals_lib1,X
		LDA		_popul_cur+2
		SBC		_popul_vals_lib2,X
		RTS

_popul_vals_lib0:
		.BYTE	$D0,$10,$50,$A0,$20,$F0,$C0,$90,$40,$70
_popul_vals_lib1:
		.BYTE	$07,$27,$C3,$86,$A1,$49,$D4,$D0,$0D,$11
_popul_vals_lib2:
		.BYTE	$00,$00,$00,$01,$07,$02,$01,$03,$03,$01

; =============== S U B	R O U T	I N E =======================================
_scenario_result_test:
		LDA		_cur_game_mode
		CMP		#$02
		BNE		locret_105197					; only for scenario mode
		LDA		_scenario_disaster_end_year
		CMP		_city._cur_year
		BNE		locret_105197					; test for scenario end year
		LDX		_scenario_idx
		BNE		loc_105142
		LDA		_city._city_type				; SCENARIO 0 "CISCO"
		CMP		#$04							; test if city degraded slightly
		BCC		.scenario_failed				; less than "CAPITAL"
		BCS		loc_105198
loc_105142:
		DEX
		BNE		loc_105153
		LDA		_traffic_level_value+1			; SCENARIO 1 "BERN"
		BNE		.scenario_failed
		LDA		_traffic_level_value
		CMP		#$50
		BCS		.scenario_failed
		BCC		loc_105198
loc_105153:
		DEX
		BNE		loc_105164
		LDA		_city._crime_level+1			; SCENARIO 2 "DETROIT"
		BNE		.scenario_failed
		LDA		_city._crime_level
		CMP		#$3C
		BCS		.scenario_failed
		BCC		loc_105198
loc_105164:
		DEX
		BNE		loc_105175
		LDA		_city._score					; SCENARIO 3 "TOKYO"
		CMP		#$F4
		LDA		_city._score+1
		SBC		#$01
		BCC		.scenario_failed
		BCS		loc_105198
loc_105175:
		DEX
		BNE		loc_105186
		LDA		_city._score					; SCENARIO 4 "BOSTON"
		CMP		#$F4
		LDA		_city._score+1
		SBC		#$01
		BCC		.scenario_failed
		BCS		loc_105198
loc_105186:
		DEX
		BNE		locret_105197
		LDA		_city._score					; SCENARIO 5 "RIO"
		CMP		#$F4
		LDA		_city._score+1
		SBC		#$01
		BCC		.scenario_failed
		BCS		loc_105198
locret_105197:
		RTS										; no more options, extra scenarios cannot be completed
loc_105198:
		LDA		#$01
		BNE		loc_10519E
.scenario_failed:
		LDA		#$02
loc_10519E:
		STA		_scenario_result_idx
		LDA		#$0B
		STA		_game_setup_sub_idx
		DEC		_main_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_practice_mode_events_handler:
		LDA		_cur_game_mode					; Practice game mode
		CMP		#$03
		BNE		_exit
		LDA		_city._cur_week					; end of month
		AND		#$03
		BNE		_exit
		LDA		_city._cur_year+1				; high nibble of 190x year
		CMP		#$07
		BNE		_exit
		LDA		_city._cur_month				; end of JAN
		CMP		#$01
		BNE		_exit
		LDA		_city._cur_year					; low nibble for 1900 year
		CMP		#$6C
		BNE		loc_1051D1
		LDX		#$1B							; first practice hint message
		BNE		.set_game_msg
loc_1051D1:
		CMP		#$6D							; low nibble for 1901 year
;		BNE		_exit							; REDUNDANT
		BNE		loc_1051F3						; FIX
		LDX		#$1C							; second practice hint message
		BNE		.set_game_msg
; BUG! never executed
; -
;		LDA		_city._cur_year					; REDUNDANT
loc_1051F3:
		CMP		#$71							; test for year 1905
		BNE		_exit
		LDX		#$1D							; message for good practice result
		LDA		_popul_cur+2					; test for population level
		BNE		.set_game_msg
		CMPWI	_popul_cur,30000				; should be above 30000 to win
		INX										; if less, use fail practice message
; -
.set_game_msg:
		STX		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
_exit:
		RTS

; =============== S U B	R O U T	I N E =======================================
; scroll messages slector
; always select random starting point, then test all handlers
; RND -> F, 0 -> RND-1
;
_scroll_msg_manager:
		LDA		#$10							; get RND in range 00-0F
		JSR		_rand_clamp
		STA		_tmpEB
		TAY										; execute handlers from RND
loc_105204:
		STY		_tmpEA							; need for handlers as well
		JSR		_scroll_msg_test
		LDY		_tmpEA							; keep cur loop counter
		INY
		CPY		#$10							; to 0F (max)
		BNE		loc_105212
		LDY		#$00							; wrap to 0
loc_105212:
		CPY		_tmpEB							; test to RND-1
		BNE		loc_105204
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg_test:
		LDA		_scroll_msg_test_lib_lo,Y
		STA		_tmpE8
		LDA		_scroll_msg_test_lib_hi,Y
		STA		_tmpE9
		JMP		(_tmpE8)
_scroll_msg_test_lib_lo:
		.BYTE	<[_scroll_msg00_test]
		.BYTE	<[_scroll_msg01_test]
		.BYTE	<[_scroll_msg02_test]
		.BYTE	<[_scroll_msg03_test]
		.BYTE	<[_scroll_msg04_test]
		.BYTE	<[_scroll_msg05_test]
		.BYTE	<[_scroll_msg06_test]
		.BYTE	<[_scroll_msg07_test]
		.BYTE	<[_scroll_msg08_test]
		.BYTE	<[_scroll_msg09_test]
		.BYTE	<[_scroll_msg0A_test]
		.BYTE	<[nullsub_10]
		.BYTE	<[_scroll_msg0C_test]
		.BYTE	<[_scroll_msg0D_test]
		.BYTE	<[_scroll_msg0E_test]
		.BYTE	<[_scroll_msg0F_test]
_scroll_msg_test_lib_hi:
		.BYTE	>[_scroll_msg00_test]
		.BYTE	>[_scroll_msg01_test]
		.BYTE	>[_scroll_msg02_test]
		.BYTE	>[_scroll_msg03_test]
		.BYTE	>[_scroll_msg04_test]
		.BYTE	>[_scroll_msg05_test]
		.BYTE	>[_scroll_msg06_test]
		.BYTE	>[_scroll_msg07_test]
		.BYTE	>[_scroll_msg08_test]
		.BYTE	>[_scroll_msg09_test]
		.BYTE	>[_scroll_msg0A_test]
		.BYTE	>[nullsub_10]
		.BYTE	>[_scroll_msg0C_test]
		.BYTE	>[_scroll_msg0D_test]
		.BYTE	>[_scroll_msg0E_test]
		.BYTE	>[_scroll_msg0F_test]

; =============== S U B	R O U T	I N E =======================================
_get_RCI_counts_div4:
		MOVW	_tmpE6,_total_RCI_counts
		LDA		_tmpE7
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg00_test:
		JSR		_get_RCI_counts_div4
		STA		_tmpE7
		CMPW	_tmpE6,_RCI_counts._R		; C_R < C_RCI / 4
		BCC		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg01_test:
		JSR		_get_RCI_counts_div4
		LSR
		ROR		_tmpE6
		STA		_tmpE7
		CMPW	_tmpE6,_RCI_counts._C		; C_C < C_RCI / 8
		BCC		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg02_test:
		JSR		_get_RCI_counts_div4
		LSR
		ROR		_tmpE6
		STA		_tmpE7
		CMPW	_tmpE6,_RCI_counts._I		; C_I < C_RCI / 8
		BCC		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg03_test:
		CMPWI	_total_RCI_counts,10
		BCC		nullsub_10
		CMPW	_roads_count,_total_RCI_counts
		BCS		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg04_test:
		CMPWI	_total_RCI_counts,50
		BCC		nullsub_10
		CMPW	_rails_count,_total_RCI_counts
		BCS		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
nullsub_10:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg05_test:
		LDA		_total_RCI_counts+1
		BNE		loc_1052EA
		LDA		_total_RCI_counts
		CMP		#$0A
		BCC		nullsub_10
loc_1052EA:
; BUG! here is the total power stations value
; may be wrong at some point of the game, this var is frequently
; used in many places as temp var
; -
;		LDA		_tmp6CC
; -
; FIX, since we don't know exactly var used here, it's safe to just recalc
; the value here directly, as in the overall info screen
;
		LDA		_coal_power_station_count
		CLC
		ADC		_nuke_power_station_count
; -
		BNE		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg06_test:
		LDA		#$00
		STA		_required_flags._stadium
		CMPWI	_city._level._R,$01F4		; here is the fixed point value, 1.F4fp
		BCC		nullsub_10
		LDA		_stadiums_count
		BNE		nullsub_10
		INC		_required_flags._stadium
		LDA		_tmpEA
		JMP		_scroll_msg_force_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg07_test:
		LDA		#$00
		STA		_required_flags._airport
		CMPWI	_city._level._I,$0064		; 0.64fp
		BCC		nullsub_10
		LDA		_sea_ports_count
		BNE		nullsub_10
		INC		_required_flags._airport
		LDA		_tmpEA
		JMP		_scroll_msg_force_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg08_test:
		LDA		#$00
		STA		_required_flags._seaport
		CMPWI	_city._level._C,$0096		; 0.96fp
		BCC		nullsub_10
		LDA		_airports_count
		BNE		nullsub_5
		INC		_required_flags._seaport
		LDA		_tmpEA
		JMP		_scroll_msg_force_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg09_test:
		CMPWI	_cur_RCI_levels_sum,$003C	; 0.3Cfp
		BCC		nullsub_5
		LDA		_fire_stations_count
		BNE		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0A_test:
		CMPWI	_cur_RCI_levels_sum,$003C	; 0.3Cfp
		BCC		nullsub_5
		LDA		_police_stations_count
		BNE		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0C_test:
		LDA		_city._tax_rate
		CMP		#$0C
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0D_test:
		LDA		_road_fund_rate
		CMP		#$16
		BCS		nullsub_5
		CMPWI	_roads_count,30
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0E_test:
		LDA		_fire_fund_rate
		CMP		#$0B
		BCS		nullsub_5
		CMPWI	_cur_RCI_levels_sum,$0014	; 0.14fp
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0F_test:
		LDA		_police_fund_rate
		CMP		#$0B
		BCS		nullsub_5
		CMPWI	_cur_RCI_levels_sum,$0014	; 0.14fp
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
nullsub_5:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg_start:
		PLA									; odd thing, if any new scroll msg is pending
		PLA									; and not currently displayed, then we do break
		LDA		_tmpEA						; from test loop and show this message
		CMP		_scroll_msg_idx				; prevents from resetting the currently displayed
		BEQ		locret_105124				; scroll message

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
;
_scroll_msg_force_start:
		STA		_scroll_msg_idx				; but for "required" messages we will continue
		LDY		#$00						; search and if next message is triggered,
		STY		_scroll_msg_start_pos		; we will for sure not see the required one
		DEY									; lowest priority messages? see if no other
		STY		_scroll_msg_active_flag		; events happens? yes, but it should STOP
locret_105124:								; any currently displayed one but seems this never
		RTS									; happens just because of long checking period.

_tlm_res_scenario_complete:
		.WORD	$2000
		.BYTE	1
		.WORD	byte_1053C7
byte_1053C7:
		.BYTE	$E4,$61,$DD,$00,$A5,$3A,$A6,$00,$A7,$22,$DD,$1C,$B5,$DF,$DF,$AA
		.BYTE	$AF,$C9,$AF,$C1,$AB,$DF,$AA,$8C,$AF,$C1,$C2,$AC,$C9,$DF,$AA,$C1
		.BYTE	$AF,$8C,$AC,$AD,$C2,$8D,$DF,$DF,$B7,$85,$00,$7F,$16,$BA,$BF,$D9
		.BYTE	$BF,$D1,$BB,$DF,$BA,$89,$BF,$D1,$D2,$BC,$D9,$DF,$BA,$D1,$BF,$9C
		.BYTE	$BC,$BD,$D2,$9D,$86,$00,$9C,$29,$F9,$00,$99,$2F,$F9,$84,$00,$9E
		.BYTE	$01,$DC,$A0,$23,$A1,$00,$A2,$85,$00,$E4,$85,$00,$E4,$86,$00,$E3
		.BYTE	$00,$DC,$85,$00,$DE,$00,$B0,$63,$84,$01,$B2,$B0,$63,$8C,$01,$B2
		.BYTE	$B0,$63,$C4,$02,$B2,$DC,$B0,$63,$80,$00,$B2,$87,$00,$FD,$63,$94
		.BYTE	$01,$B2,$B0,$63,$9C,$01,$B2,$B0,$63,$D4,$02,$B2,$DC,$B0,$63,$90
		.BYTE	$88,$01,$1C,$63,$A4,$01,$B2,$B0,$63,$AC,$01,$B2,$B0,$63,$E4,$02
		.BYTE	$B2,$DC,$B0,$63,$A0,$88,$01,$1C,$63,$B4,$01,$B2,$B0,$63,$BC,$01
		.BYTE	$B2,$B0,$63,$F4,$02,$B2,$DC,$B0,$63,$B0,$87,$01,$1C,$00,$C0,$23
		.BYTE	$C1,$00,$C2,$85,$01,$84,$85,$01,$84,$86,$01,$83,$86,$00,$FD,$62
		.BYTE	$CD,$62,$D3,$03,$DC,$DC,$28,$F3,$22,$DC,$63,$FC,$01,$DC,$DC,$63
		.BYTE	$9C,$01,$A9,$AA,$86,$00,$FD,$03,$DC,$DC,$C7,$D6,$23,$DC,$01,$C7
		.BYTE	$9B,$84,$01,$C8,$00,$85,$24,$DC,$03,$0F,$AC,$DC,$DC,$86,$00,$FD
		.BYTE	$64,$D7,$01,$D1,$DC,$63,$F4,$01,$DC,$DC,$63,$F8,$23,$DC,$04,$AD
		.BYTE	$AE,$DC,$DC,$DC,$F0,$26,$00,$DE,$63,$C0,$01,$B2,$B0,$63,$C8,$01
		.BYTE	$B2,$B0,$63,$CC,$02,$B2,$DC,$B0,$23,$88,$88,$01,$1C,$63,$D0,$01
		.BYTE	$B2,$B0,$63,$D8,$01,$B2,$B0,$63,$DC,$8F,$02,$35,$63,$E0,$01,$B2
		.BYTE	$B0,$63,$E8,$01,$B2,$B0,$63,$EC,$8F,$02,$35,$63,$F0,$01,$B2,$B0
		.BYTE	$63,$F8,$01,$B2,$B0,$63,$FC,$06,$B2,$DC,$B0,$88,$88,$88,$88,$F0
		.BYTE	$27,$01,$7C,$00,$DC,$63,$8C,$83,$01,$F7,$02,$AF,$D6,$DC,$63,$2B
		.BYTE	$03,$60,$61,$DC,$DC,$63,$C3,$00,$DC,$89,$01,$BD,$00,$AB,$84,$01
		.BYTE	$D5,$00,$7C,$84,$01,$D5,$00,$6E,$29,$DC,$86,$01,$BE,$63,$D6,$07
		.BYTE	$DC,$DC,$85,$86,$88,$89,$DC,$DC,$63,$F0,$8F,$02,$F5,$06,$DA,$DB
		.BYTE	$CE,$CF,$DC,$DC,$8A,$62,$9D,$06,$DC,$DC,$DC,$F4,$F5,$F6,$F7,$8B
		.BYTE	$02,$F6,$00,$C5,$3A,$C6,$00,$C7,$E4,$60,$DD,$32,$FF,$00,$42,$22
		.BYTE	$00,$87,$03,$CF,$0F,$FF,$81,$24,$18,$42,$00,$00,$00,$00,$FF,$FF
		.BYTE	$FF,$FF,$FF,$FF,$FF,$8F,$03,$DF,$00,$FF,$E4,$61,$81,$3C,$CA,$83
		.BYTE	$04,$5F,$01,$01,$01,$25,$00,$02,$01,$00,$01,$86,$04,$86,$00,$00
		.BYTE	$84,$04,$8C,$22,$01,$9F,$04,$7E,$84,$04,$7E,$29,$80,$00,$01,$2F
		.BYTE	$80,$04,$CA,$81,$81,$81,$CA,$3A,$01,$86,$04,$7E,$23,$53,$85,$05
		.BYTE	$03,$87,$05,$03,$04,$01,$53,$53,$53,$53,$9F,$04,$FC,$F0,$3F,$04
		.BYTE	$FC,$01,$01,$01,$F0,$25,$04,$DE,$25,$06,$01,$01,$01,$83,$05,$A8
		.BYTE	$00,$01,$23,$03,$07,$01,$01,$06,$06,$06,$06,$06,$06,$88,$05,$7D
		.BYTE	$08,$06,$06,$01,$01,$01,$01,$06,$06,$01,$88,$05,$C3,$02,$01,$06
		.BYTE	$06,$8F,$05,$9B,$10,$06,$06,$06,$06,$01,$01,$03,$03,$03,$03,$01
		.BYTE	$01,$01,$01,$06,$06,$01,$F0,$27,$05,$7C,$F0,$9F,$05,$04,$00,$01
		.BYTE	$23,$02,$86,$05,$AF,$25,$05,$06,$01,$01,$06,$06,$06,$06,$01,$9B
		.BYTE	$05,$BD,$91,$06,$B9,$09,$02,$02,$02,$02,$01,$01,$03,$03,$03,$03
		.BYTE	$9B,$06,$F5,$8C,$07,$10,$00,$CA,$F0,$22,$04,$5F,$E4,$5D,$81,$32
		.BYTE	$00,$87,$07,$BF,$84,$07,$BF,$0B,$42,$18,$24,$81,$00,$00,$00,$00
		.BYTE	$00,$00,$00,$00,$8F,$07,$DC,$23,$00,$FF
_tlm_res_name_init:
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

		BANK_END FA,$B800

		ORG		$B800
		RORG	$9800

		BANK_START $AA

		INCLUDE "..\bankA_.inc"

_bankAA_free	SET	[$A000-.]
_total_free SET _total_free+_bankAA_free
		#if .>$A000
		ECHO	"bank",CUR_PRG_BANK,"[ size =",[.-CUR_BANK_START],"]","exceeded =",[0-_bankAA_free]
		#else
		ECHO	"bank",CUR_PRG_BANK,"[ size =",[.-CUR_BANK_START],"]","free =",_bankAA_free
		#endif
		#if .<$A000-1
		RORG	$A000-1
		ORG		$C000-1
		.BYTE	$FF
		#endif

