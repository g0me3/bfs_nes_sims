
		ORG $A000

		BANK_START $FA

; ---------------------------------------------------------------------------
; ------C-I-T-Y--C-O-R-E--R-E-C-A-L-C--E-X-T-E-R-N-A-L--R-O-U-T-I-N-E-S------
; ---------------------------------------------------------------------------

		SECTION_START B01

; =============== S U B	R O U T	I N E =======================================
; this routine called every year and calc last decimal digit of the year
; number. used to determide wether the 100year range graphs need to be updated
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

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_recalc:
		RSMOVW	_tmp6AA,_city._level._R,3					; 6AA = L_R/8
;		MOVW	_prev_RCI_levels_sum,_cur_RCI_levels_sum	; REDUNDANT, never used
		MADDW	_tmp6AC,_city._level._I,_city._level._C		; 6AC = L_I+L_C
		MADDW	_cur_RCI_levels_sum,_tmp6AA,_tmp6AC			; RCI_S = L_I+L_C+(L_R/8)
		MOVW	_tmpE7,_tmp6AC					; dividend = L_I+L_C
		MOVW	_tmpEA,_tmp6AA					; divider = L_R/8
		LDA		#$00
		STA		_tmpE9
		STA		_tmpE6
		JSR		_div_32to16						; E6 = (L_I+L_C)/(L_R/8) (two fp8_8)
		BCC		loc_10409D
		MOVWI	_tmpE6,$014D					; if divider = 0, then result 1.3, so with R=0
		MOVWI	_tmpE8,0						; we will always grow!
loc_10409D:
		ADDWB	_tmpE6,#$05						; add 0.02 to result, FIXME! may cause overflow!!
		LDX		#R_AREA_BIDX
		JSR		_RCI_meter_recalc				; R recalc
		MOVW	_tmpE7,_tmp6AA					; dividend = L_R/8
		MOVW	_tmpEA,_tmp6AC					; divider = L_I+L_C
		LDA		#$00
		STA		_tmpE9
		STA		_tmpE6
		JSR		_div_32to16						; E6 = (L_R/8)/(L_I+L_C)
		BCC		loc_1040D8
		MOVWI	_tmpE6,$0100					; if divider = 0, then result 1.0, so with L+C=0 no changes
		MOVWI	_tmpE8,0
loc_1040D8:
		CMPWI	_tmpE6,$014C					; clamp to 1.3 max as well
		BCC		loc_1040EA
		MOVWI	_tmpE6,$014C
loc_1040EA:
		PUSHW	_tmpE6							; backup now
		PUSHWI	$0133							; arg 1.2
		PUSHW	_tmpE6							; arg E6
		JSR		_mmc5_mul16to16					; weightnig I growth with E6 * 1.2
		PLA										; skip unnecessary results of fp16_16 value
		POPW	_tmpE6							; got proportion for I aread meters, recalc
		PLA
		LDX		#I_AREA_BIDX
		JSR		_RCI_meter_recalc				; I recalc
; REDUNDANT, no need to retrive it from stack if we need to push it again
; to stack again.
;		POPW	_tmpE6							; restore C/I base proportion already calculated
		PUSHW	_cur_RCI_levels_sum				; arg RCI levels sum * 256 (as integer)
;		PUSHW	_tmpE6							; arg R6 (already in stack)
		JSR		_mmc5_mul16to16					; int16*fp8_8
		PUSHB	#$45
		JSR		_mmc5_mul32to8					; fp8_8*fp0_8 (* 0.27)
		PLA
		POPW	_tmpE6							; result fp8_8
		POPW	_tmpE8
		MOVW	_tmpEA,_city._level._C			; fp8_8/int16
loc_104150:
		JSR		_div_32to16						; here if L_C=0 we won't set a default result value
;		BCC		loc_10415F						; instead we set the default minimum L_C to 1
;		LDA		#$00							; REDUNDANT, LDA $01 set Z=0, so branch does not occur
;		STA		_tmpEB							; however, the default value to div here is 1
;		LDA		#$01							; so we will get the same result as divident loaded
;		STA		_tmpEA							; we may assume that division already done here then!
;		BEQ		loc_104150						; and does not require to divide twice
loc_10415F:
		LDX		#C_AREA_BIDX					; C recalc

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
; _tmpE6-_tmpE7 here is a proportion for R and C/I levels (not values!!).
;
; L_C+L_I = L_CI
; L_R/8 = L_r
; for R proportion = L_CI/L_r, default 1.3, max 2.00
; for C proportion = ((L_r/L_CI)*(L_CI+L_r)*0.27)/L_C
; for I proportion = (L_r/L_CI)*1.2), default 1.2, max 1.56
;
; NOTE, RCI levels here are depending on people requirements, if no requirements,
; then calc as usual if any requirement is set, then delta will be always 0,
; .ignore_requirements delta already bad enough, so no need to force it to zero.
;
; Requirements for R meter depending on stadium, for C meter - on airport
; and for I meter on sea port
;
_RCI_meter_recalc:
		LDA		_city._delta,X					; requirements are preventing only growth
		BMI		.ignore_requirements			; they does not work for fall down, so test
		LDA		#$00							; if delta negative here and if yes, skip
		LDY		_required,X
		BNE		.delta_update
.ignore_requirements:
		LDA		_tmpE6							; here we must multiply fixed point number
		LDY		_tmpE7							; to small weighted parameter ~0.08-0.1
		BNE		loc_10417B						; that determine accel of growth
		NEGA									; for _tmpE6 less than 1.00, accel will be negative
												; so we need to invert FRAC portion here
		JMP		loc_104181						; this won't be a negative value, just complement of 1
loc_10417B:
		CPY		#$02							; for _tmpE6 greater than 1.00, accel will be
		BCC		loc_104181						; positive, with maximum FRAC = 0.999
		LDA		#$FF
loc_104181:
		PHA
		LDA		#$19							; accel coeff for R area 0.098 ~ 0.10
		CPX		#$00
		BEQ		loc_10418A
		LDA		#$14							; accel coeff for C/I areas 0.078 ~ 0.08
loc_10418A:
		PHA
		JSR		_mmc5_mul8to8					; multiply, level.FRAC * coeff
		PLA										;
		PLA										; for R we will have accels 0 to 24
		LDY		_tmpE7							; for C/I accordingly 0 to 20
		BNE		loc_104199						; for levels < 1.0 we must invert result so
		NEGA									; we will get a negative number here
loc_104199:
		STA		_tmpE8							; store RCI delta accel value
		LDA		_city._tax_rate					; calculation includes higher tax rate for higher difficult
		CLC
		ADC		_city._difficult				; but not higher than 20 anyway, so if you already set to 20
		CMP		#$14							; on hard mode, there won't be any difference here
		BCC		loc_1041A8
		LDA		#$14
loc_1041A8:
		TAY
		LDA		_rci_meters_accel_tax_list,Y	; additional accel modifier according to current tax value
		CLC										; positive < 7, zero = 8, negative > 7
		ADC		_tmpE8							; NOTE, here we drop carry bits and does not check overflow!
		CLC										; but here it is not important, values are small enough here
		ADC		_city._delta,X					; now adjust growth speed
		BVC		.delta_update					; negative results or results lower than $7F are passes as is
		BMI		.delta_clamp					; + with + turns - = positive overflow, clamp to max positive $7F
		LDA		#$80							; - with - turns + = negative overflow, clamp to min negative $80
		BNE		.delta_update
.delta_clamp:
		LDA		#$7F
.delta_update:
		STA		_city._delta,X					; store adjusted speed value
;		LDA		_city._delta,X					; OPTIMIZED, already in A
		BPL		.positive_speed					; now calc the RCI meter value for display
		NEGA									; for negative rci_meter = -abs(speed)*0.1
		PHA
		PUSHB	#$1B
		JSR		_mmc5_mul8to8
		PLA
		PLA
		NEGA
		JMP		loc_1041E4
.positive_speed:
		PHA										; for positive accordingly rci_meter = speed*0.1
		PUSHB	#$1B
		JSR		_mmc5_mul8to8
		PLA
		PLA
loc_1041E4:
		STA		_RCI_meters_values,X			; done
		RTS

_rci_meters_accel_tax_list:
;				0%  1%  2%  3%  4%  5%  6%  7%  8%  9%  10% 11% 12% 13% 14% 15% 16% 17% 18% 19% 20%
		.BYTE	$0D,$0A,$08,$06,$05,$03,$02,$00,$FF,$FD,$FA,$F6,$F3,$F0,$ED,$EA,$E6,$E3,$E0,$DD,$DA

; =============== S U B	R O U T	I N E =======================================
; consumption calculated on the tile-basis. every building (includnig the power
; station itself and all power lines) consume 1 point for one tile used plus
; additional value depending on how many conjunctions it has with other powerable
; tiles details are below.
;
; at the beginning the power nodes list consists only of power station coordinates
;
; 1. fetch current power node until there are no more, else exit
; 2. enable power grid for current node, increment power consumption value
; 3. search if nearest tiles from 0 to 3 clockwise are need power
;	(if tile powerable and not powered yet and not the power station itself)
; 4. jump 3. unless there are no more or there are more than one
; 5. if there are more than one, then
;		put current node to the nodes list, jump 6.
;	 if there is only one tile, then
;		jump 6.
;	 if there are no more, then
;		jump 1.
; 6. set the last tile need to be powered we found as a current node, jump 2.
;
; NOTE, as we can see, this algo recursively put any tested tile in the power nodes
; list as much times, as many powerable tiles need to be powered up are connected
; with it. so, this particular tile will consume the power every time it fetched
; from the nodes list again.
; NOTE, also we can see, the very time-cost operation of test the tile if need power
; can be performed a couple of times on the same tile, which is not so much optimal too.
;
; for example: when put one power station to the map, the algo will go following way
; inside the power station tiles. numbers are represent order/power consumption,
; * represent insertion into the power nodes list:
;    0    1    2		0  fetch power node 0,0
;   +----+----+----+	1  0,0 powered, 1,0 and 0,1 are need power, put 0,0 as new power node, jump to 0,1
; 0 |1*13|8   |7   |	2  0,1 powered, 1,1 and 0,2 are need power, put 0,1 as new power node, jump to 0,2
;   +----+----+----+	3  0,2 powered, 1,2 need power, jump 1,2
; 1 |2*12|9   |6*10|	4  1,2 powered, 1,1 and 2,2 are need power, put 1,2 as new power node, jump to 2,2
;   +----+----+----+	5  2,2 powered, 2,1 need power, jump 2,1
; 2 |3   |4*11|5   |	6  2,1 powered, 2,0 and 1,1 are need power, put 2,1 as new power node, jump to 2,0
;   +----+----+----+	7  2,0 powered, 1,0 need power, jump 1,0
;						8  1,0 powered, 1,1 need power, jump 1,1
;						9  1,1 powered, no more tiles, fetch power node 2,1
;						10 2,1 repowered, no more tiles, fetch power node 1,2
;						11 1,2 repowered, no more tiles, fetch power node 0,1
;						12 0,1 repowered, no more tiles, fetch power node 0,0
;						13 0,0 repowered, no more tiles, end of power nodes list, exit
;
; if we put additional connection wires near the points like 0,0 or 2,1 we will see they put in the
; node list twice or three times.
;
; This algo was taken by NES developers from Micropolis source code as is, so this is definitely
; original behaviour/problems of calculating power consumption...
;
_power_grid_recalc:
		JSR		_sram_write_enable				; set game buffers to read
; REDUNDANT
;		LDA		#$00
;		TAY
;loc_104203:
;		STA		_power_map,Y					; clear power grid map
;		STA		_power_map+$100,Y				; TODO: merge clear code in one proc
;		INY
;		BNE		loc_104203
;		LDA		#$00
;		TAY
;loc_10420F:
;		STA		_power_map+$200,Y
;		INY
;		CPY		#$D2
;		BNE		loc_10420F
; -
; OPTIMIZED
		MOVWO	_tmpE6,_power_map				; clear power grid map
		LDX		#>[_power_map_size]				; 76*76/8 = 722
		LDY		#<[_power_map_size]
		JSR		_clear_buf
; -
		JSR		_sram_write_disable
		PUSHWI	700								; calculate power amount
		PUSHB	_coal_power_station_count
		JSR		_mmc5_mul16to8					; E6 = COL * 700
		POPD	_tmpE6
		PUSHWI	2000
		PUSHB	_nuke_power_station_count		; TMP = NUCLEAR * 2000
		JSR		_mmc5_mul16to8
		PLA
		CLC
		ADC		_tmpE6							; E6 = E6 + TMP
		STA		_tmpE6
		PLA
		ADC		_tmpE7
		STA		_tmpE7
		PLA
		ADC		_tmpE8
		BEQ		loc_104253						; if result > 65535
		LDA		#$FF							; then clamp to 65535
		STA		_tmpE6
		STA		_tmpE7
loc_104253:
		MOVW	_tmp6CC,_tmpE6					; _tmp6CC here is full power generation value
		MOVWI	_tmp6CE,0						; _tmp6CE current consumtion counter, initial value 0
.power_grid_recalc_loop:
		LDA		_power_nodes_cnt				; here must be already collected list of power stations
		ORA		_power_nodes_cnt+1				; if not, skip calcs.
		BNE		loc_10426E
		RTS
loc_10426E:
		LDY		_power_nodes_cnt				; load counter
		LDA		_power_nodes_cnt+1				; to start with list0 or list1 test higher counter byte
		BEQ		loc_10427F
		LDX		_tmp_map,Y						; list 1 read
		LDA		_tmp_map+$300,Y
		JMP		loc_104285
loc_10427F:
		LDX		_tmp_map+$100,Y					; list 0 read
		LDA		_tmp_map+$200,Y
loc_104285:
		STX		_tmp_map_pos._COL				; fetch last power station position on map
		STA		_tmp_map_pos._ROW
		DEY										; decrement power stations list counter
		STY		_power_nodes_cnt
		CPY		#$FF
		BNE		loc_104296
		DEC		_power_nodes_cnt+1
loc_104296:
		LDA		#$04
		STA		_tmp6D0							; idx=4 isn't valid, with this it will skip position change
.power_search_loop0:
		INCW	_tmp6CE							; for every entry in the power nodes list, increase power consumption
		CMPW	_tmp6CE,_tmp6CC					; then compare with max possible according to power stations number
		BCS		locret_1042F9
		LDX		_tmp6D0							; load near tile idx and change cur pos to it
		JSR		_jump_to_near_to_tmp_map_pos_tile; at the beginning of loop, this is out power station tile
		JSR		_power_grid_set					; set the power grid flag for current tile
		LDA		#$00
		STA		_tmp6D2							; counter found tiles to be powered
		STA		_tmp6D1							; loop1 counter
.power_search_loop1:
		LDA		_tmp6D1							; test all 4 nearest tiles for current selected tile
		CMP		#$04							; in this loop we does nothing except looking for powerable tiles
		BCS		.power_next_loop0
		LDA		_tmp6D2
		CMP		#$02
		BCS		.power_next_loop0				; FIXME, redundant double check jump here
;		FJSR	_power_tile_test,SRAM,PRGB		; REDUNDANT, test if nearest tile powerable but not powered
		JSR		_power_tile_test				; OPTIMIZED, now local, speed-up up to two times
		BCS		.power_no_need					; skip if already powerable or not need to power
		INC		_tmp6D2							; increment counter of tiles found
		LDA		_tmp6D1							; current tile need to power idx goes to main near tile selector
		STA		_tmp6D0							; upon search we will have here idx of every 3th powerable tile found or less
.power_no_need:
		INC		_tmp6D1							; next tile select
		BNE		.power_search_loop1				; unconditional break
.power_next_loop0:
		LDA		_tmp6D2							; when we found more than 2 powerable tiles, we need to add third one
		CMP		#$02							; to the power nodes list, so two other we will skip then
		BCC		.power_no_insert				; for every powerable tile we must add at least 2 of 4 to nodes
		JSR		_power_node_insert
.power_no_insert:
		LDA		_tmp6D2
		BNE		.power_search_loop0				; search until we have no more tiles to power up
		JMP		.power_grid_recalc_loop			; then we jump to retch another power node from list
locret_1042F9:
; -
; TODO: put brownouts scroller message start here.
; -
		RTS

; REDUNDANT, merged with _jump_to_near_to_tmp_map_pos_tile, moved to system bank
; =============== S U B	R O U T	I N E =======================================
; this is a copy of similar routine in bank which cannot be used
; also _back_from_near_tmp_map_pos_tile cannot be optimized like this routine
; because we need to keep X there
;
;_tmp_map_pos_near_tile_select:
;		CPX		#$00							; input arg - one of the select direction
;		BNE		loc_104307						; -0-
;		LDA		_tmp_map_pos._ROW				; 3*1
;		BEQ		locret_10432E					; -2-
;		DEC		_tmp_map_pos._ROW				; if at the map edge, use the same tile
;		RTS
;loc_104307:
;		DEX
;		BNE		loc_104315
;		LDA		_tmp_map_pos._COL
;		CMP		#$4B
;		BEQ		locret_10432E
;		INC		_tmp_map_pos._COL
;		RTS
;loc_104315:
;		DEX
;		BNE		loc_104323
;		LDA		_tmp_map_pos._ROW
;		CMP		#$4B
;		BEQ		locret_10432E
;		INC		_tmp_map_pos._ROW
;		RTS
;loc_104323:
;		DEX
;		BNE		locret_10432E
;		LDA		_tmp_map_pos._COL
;		BEQ		locret_10432E
;		DEC		_tmp_map_pos._COL
;		SEC
;		RTS
;locret_10432E:
;		RTS

; =============== S U B	R O U T	I N E =======================================
; used here locally as well as external routine for PRGB
;
_power_node_insert:
		JSR		_sram_write_enable
; REDUNDANT
;		LDY		_power_nodes_cnt				; get counter 16-bit value
;		LDX		_power_nodes_cnt+1				; increment by 1
;		INY										; X/Y = list_pos+1 now
;		BNE		loc_10433C
;		INX
;loc_10433C:
;		CPX		#$02
;		BEQ		loc_104368						; maximum entries in list 512
;		STY		_power_nodes_cnt				; save value
;		STX		_power_nodes_cnt+1
;		LDY		_power_nodes_cnt
;		CPX		#$00
;		BEQ		.power_grid_list0_store
; -
; OPTIMIZED
		INCW	_power_nodes_cnt
		LDY		_power_nodes_cnt
		LDX		_power_nodes_cnt+1
		BEQ		.power_grid_list0_store			; less than 256 entries, store list 0
		CPX		#$02
		BEQ		loc_104368						; maximum entries in list 512
; -
		LDA		_tmp_map_pos._COL				; other entries stored in list 1
		STA		_tmp_map,Y
		LDA		_tmp_map_pos._ROW
		STA		_tmp_map+$300,Y
		JMP		loc_104368						; optimize me
.power_grid_list0_store:
		LDA		_tmp_map_pos._COL
		STA		_tmp_map+$100,Y
		LDA		_tmp_map_pos._ROW
		STA		_tmp_map+$200,Y
loc_104368:
		JMP		_sram_write_disable

; OPTIMIZED, moved here from PRGB to be local
; =============== S U B	R O U T	I N E =======================================
_power_tile_test:
		PUSHW	_tmp_map_pos					; backup current _tmp_map_pos
		LDX		_tmp6D1							; external near tile idx selector
		JSR		_jump_to_near_to_tmp_map_pos_tile	; change cur pos to selected near tile
		BCS		.cant_test_power_map_tile		; skip if can't change, tile is on the map edge
		JSR		_power_grid_test				; if tile is powerable but has no power, return C=0
		BCS		.cant_test_power_map_tile
		POPW	_tmp_map_pos
		CLC
		RTS
.cant_test_power_map_tile:
		POPW	_tmp_map_pos					; return C=1 otherwise
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
; return C=1 if not powerable tile (also when tile is a power plant)
; or tile already powered.
;
_power_grid_test:
		JSR		_map_tile_read					; read nearest tile
		TAY
		LDA		_map_tiles_flags_tbl,Y
		BPL		.nothing_to_power				; test if powerable
		TYA
		CMP		#_MAP_COAL_POWER				; also test for power stations we
		BEQ		.nothing_to_power				; already have in our list
		CMP		#_MAP_NUCLEAR_POWER
		BEQ		.nothing_to_power
; REDUNDANT
;		LDA		_tmp_map_pos._ROW				; now calc the power map grid idx again
;		MULAI	#$4C							; as done in power map set routine before
;		LDA		_MMC5_MUL0
;		CLC
;		ADC		_tmp_map_pos._COL
;		STA		_tmpE6
;		PHA
;		LDA		_MMC5_MUL1
;		ADC		#$00
;		LSR
;		ROR		_tmpE6
;		LSR
;		ROR		_tmpE6
;		LSR
;		ROR		_tmpE6
;		STA		_tmpE7
;		LDA		_tmpE6
;		CLC
;		ADC		#<[_power_map]
;		STA		_tmpE6
;		LDA		_tmpE7
;		ADC		#>[_power_map]
;		STA		_tmpE7
;		PLA
;		AND		#$07
;		TAX
;		LDY		#$00
;		LDA		_inv_bitmasks,X					; test if selected tile is powered already
;		AND		(_tmpE6),Y						; return C=0 need power up
; -
; OPTIMIZED
		JSR		_power_grid_read
		AND		_inv_bitmasks,X					; test if selected tile is powered already
; -
		BNE		.nothing_to_power
		CLC
		RTS
.nothing_to_power:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_power_grid_set:
;		LDA		_tmp_map_pos._ROW				; calculate map buf offset
;		MULAI	#$4C
;		LDA		_MMC5_MUL0
;		CLC
;		ADC		_tmp_map_pos._COL
;		STA		_tmpE6							; store low nibble
;		PHA										; backup it for now
;		LDA		_MMC5_MUL1						; adjust high nibble in A
;		ADC		#$00
;		LSR										; div 8, since every tile uses
;		ROR		_tmpE6							; one bit in power grid map
;		LSR
;		ROR		_tmpE6
;		LSR
;		ROR		_tmpE6
;		STA		_tmpE7							; store shifted high nibble
;		LDA		_tmpE6							; lower already shifted, load
;		CLC
;		ADC		#<[_power_map]					; add buffer offset to get full ptr
;		STA		_tmpE6
;		LDA		_tmpE7
;		ADC		#>[_power_map]
;		STA		_tmpE7
;		PLA
;		AND		#$07							; extract lower 3 bits for number of
;		TAX										; selected bit in current byte of grid map
;		LDY		#$00
;		LDA		(_tmpE6),Y						; load it
; OPTIMIZED
		JSR		_power_grid_read
; -
		ORA		_inv_bitmasks,X					; set flag
		JSR		_sram_write_enable
		STA		(_tmpE6),Y						; store byte back
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
; TODO: when space is enough, merge me with my copy
; copy of _test_power_grid_map_bit in PRGB
;
_power_grid_read:
		LDA		_tmp_map_pos._ROW				; calculate map buf offset
		MULAI	#$4C
		LDA		_MMC5_MUL0
		CLC
		ADC		_tmp_map_pos._COL
		STA		_tmpE6							; store low nibble
		TAY										; backup it for now
		LDA		_MMC5_MUL1						; adjust high nibble in A
		ADC		#$00
		LSR										; div 8, since every tile uses
		ROR		_tmpE6							; one bit in power grid map
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		STA		_tmpE7							; store shifted high nibble
		LDA		_tmpE6							; lower already shifted, load
		CLC
		ADC		#<[_power_map]					; add buffer offset to get full ptr
		STA		_tmpE6
		LDA		_tmpE7
		ADC		#>[_power_map]
		STA		_tmpE7
		TYA
		AND		#$07							; extract lower 3 bits for number of
		TAX										; selected bit in current byte of grid map
		LDY		#$00
		LDA		(_tmpE6),Y						; load it
		RTS

		SECTION_ENDS B01, "PRGB RCI METERS POWER GRID RECALC"

		SECTION_START B02

; =============== S U B	R O U T	I N E =======================================
; TODO, some parameters are interpolated with previous ones, some not.
; they need to be interpolated in common routine at once.
;
; Micropolis core sources has a different scaling values
; Rh = R/8
; Ch = C
; Ih = I
; Money
_graphs_10_year_data_update:
		LDA		#$00
		STA		_tmpE6
		JSR		_sram_write_enable				; prepape sram
		LDX		#$00
loc_104559:
		LDA		_graphs._10y._R+1,X				; shift graphs data to the left
		STA		_graphs._10y._R,X				; collect R/C/I max value
		CMP		_tmpE6
		BCC		loc_104565
		STA		_tmpE6
loc_104565:
		LDA		_graphs._10y._C+1,X
		STA		_graphs._10y._C,X
		CMP		_tmpE6
		BCC		loc_104571
		STA		_tmpE6
loc_104571:
		LDA		_graphs._10y._I+1,X
		STA		_graphs._10y._I,X
		CMP		_tmpE6
		BCC		loc_10457D
		STA		_tmpE6
loc_10457D:
		LDA		_graphs._10y._pollution+1,X		; the rest does not need to save min max
		STA		_graphs._10y._pollution,X
		LDA		_graphs._10y._money+1,X
		STA		_graphs._10y._money,X
		LDA		_graphs._10y._crime+1,X
		STA		_graphs._10y._crime,X
		INX
		CPX		#$09
		BNE		loc_104559
		LDA		_tmpE6
; TODO: fixme! never used for scaling!
		STA		_RCI_max_range_10_years
; -
		LDA		_city._level._R.FRAC			; here we multiply to 2 the whole fixed point number
		STA		_tmpE6							; take higher bit of FRAC into integer part once
		LDA		_city._level._R.INT				; then shift integer part
; BUG! _tmpE6 writeen twice, FRAC nibble of _R lost then
; FIX, remove it and we get actual value
;		STA		_tmpE6
		ASL		_tmpE6
		ROL
		STA		_graphs._10y._R+9				; graph data = [L_R * 2]
		LDA		_city._level._C.INT				; little bit ticky here, we divide frac by 16
		STA		_tmpE6							; but then get the result as an integer. this
		LDA		_city._level._C.FRAC			; equivalent of * 256 of the fixed point result
		LSR		_tmpE6							;
		ROR										; NOTE, if we do the same as for _R calc,
		LSR		_tmpE6							; swap INT/FRAC and get left shift/rors instead
		ROR										; result would be the same
		LSR		_tmpE6
		ROR
		LSR		_tmpE6
		ROR
		STA		_graphs._10y._C+9				; graph data = [L_C * 16]
		LDA		_city._level._I.INT				; same as previous
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
		STA		_graphs._10y._I+9				; graph data = [L_I * 16]
; BUG! "crime" button is the last one, this is a pollution button.
; use pollution value instead!
;		LDA		_city._crime_lvl_avr.INT
		LDA		_city._poll_lvl_avr.INT		; FIX, correct value
; NOTE, integer part of the value is a special
; flag for ingame warning messages, so remove it
; interpolate with previous values for nice curvy graph
;
		AND		#$7F
		SEC
		SBC		_graphs._10y._pollution+8		; interpolate with previous value
		LSR
		LSR
		ADC		_graphs._10y._pollution+8
		BCC		loc_1045E9
		LDA		#$FF							; clamp to 255 max
loc_1045E9:
		STA		_graphs._10y._pollution+9
; BUG, it seems they copy=pasted code above and just put the money var here
; all this thing looks wrong then.
;
;		LDA		_city._money+2					; money above 65535 counts but clamped to 7 bit only
		LDA		_city._crime_lvl_avr.INT			; FIX, correct values here
		AND		#$7F							; lol
		SEC
;		SBC		_graphs._10y._money+8			; BUG
		SBC		_graphs._10y._crime+8			; FIX
		LSR
		LSR
;		ADC		_graphs._10y._money+8			; BUG
		ADC		_graphs._10y._crime+8			; FIX
		BCC		loc_1045FE
		LDA		#$FF							; clamp to 255 max
loc_1045FE:
;		STA		_graphs._10y._money+9			; BUG
		STA		_graphs._10y._crime+9			; FIX
; BUG, Also odd calculation, _taxes_income never gets higher nibble calculated before now in the original code
; also _taxes_income may be negative, so further calculations may be incorrect and clamp negative values to
; max on the graph instead...
;
;		LDA		_taxes_income+2					; NOTE, taxes wer income never calculated as 24-bit value
;		BNE		loc_104625						; only as 16-bit, now it is so this check isnt redundant now
;		LDA		_taxes_income+1					; graph value = (_taxes_income / 32) - 128
;		STA		_tmpE6							; so values here are inrange -128 to +127
;		LDA		_taxes_income
;		LSR		_tmpE6
;		ROR
;		LSR		_tmpE6
;		ROR
;		LSR		_tmpE6
;		ROR
;		LSR		_tmpE6
;		ROR
;		LSR		_tmpE6
;		ROR
;		LDY		_tmpE6
;		BNE		loc_104625						; attempt to clamp
;		ADC		#$80
;		BCC		.money_store					; attempt to shift graph to the middle of the axis
;loc_104625:
;		LDA		#$FF							; clamp to 255 max
; -
; FIX, may be optimized a little by rearranging some opcodes
		LDA		_taxes_income+2
		BMI		.negative_income				; negative
		BNE		.clamp_to_max_m					; non-zero
		LDA		_taxes_income+1					; the rest is shifted
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
		BNE		.clamp_to_max_m
		CLC
		ADC		#$80
		BCC		.money_store
.clamp_to_max_m:
		LDA		#$FF
		BNE		.money_store
.negative_income:
		LDA		_taxes_income					; negative value
		EOR		#$FF
		CLC
		ADC		#$01
		STA		_tmpE6
		LDA		_taxes_income+1
		EOR		#$FF
		ADC		#$00
		STA		_tmpE7
		LDA		_taxes_income+2
		EOR		#$FF
		ADC		#$00
		BNE		.clamp_to_min_m					; clamp if higher nibble non-zero
		LDA		_tmpE7							; already got values in tmp vars
		LSR										; so just /32 here
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		CMP		#$00							; for high nibble in A
		BNE		.clamp_to_min_m					; opposite values here as well.
		LDA		#$80
		SEC
		SBC		_tmpE6
		BCS		.money_store
.clamp_to_min_m:
		LDA		#$00
; -
.money_store:
;		STA		_graphs._10y._crime+9			; BUG
		STA		_graphs._10y._money+9			; FIX
		JSR		_sram_write_disable
;		LDX		#$00							; the final calculation for schools and hospitals
		LDA		_city._level._R.INT				; based on integer part of R level value
		CMP		_hospitals_count				; [L_R]<school/hospitals, school/hospitals delta negative
;		BCC		loc_10463B						; [L_R]=school/hospitals, school/hospitals delta zero
;		BNE		loc_10463F						; [L_R]<school/hospitals, school/hospitals delta positive
;		BEQ		loc_104641						; so we see that a number of schools/hospitals would be exactly
;loc_10463B:										; equal the R level
;		LDX		#$FF
;		BNE		loc_104641
;loc_10463F:
;		LDX		#$01
;loc_104641:
		JSR		_delta_calc
		STX		_hospitals_delta
;		LDX		#$00
;		LDA		_city._level._R.INT				; REDUNDANT, already loaded
		CMP		_schools_count
;		BCC		loc_104652
;		BNE		loc_104656
;		BEQ		loc_104658
;loc_104652:
;		LDX		#$FF
;		BNE		loc_104658
;loc_104656:
;		LDX		#$01
;loc_104658:
		JSR		_delta_calc
		STX		_schools_delta
		RTS

; OPTIMIZED size, merged same compares
; =============== S U B	R O U T	I N E =======================================
_delta_calc:
		LDX		#$00
		BCC		loc_104652
		BNE		loc_104656
		BEQ		loc_104658
loc_104652:
		LDX		#$FF
		BNE		loc_104658
loc_104656:
		LDX		#$01
loc_104658:
		RTS

; =============== S U B	R O U T	I N E =======================================
_graphs_100_year_data_update:
		LDA		#$00							; FIXME: simple copy cur data for year,
		STA		_tmpE6							; not so much relevant info. maybe need
		JSR		_sram_write_enable				; an interpolation of all 10years graphs instead?
		LDX		#$00
loc_104665:
		LDA		_graphs._100y._R+1,X
		STA		_graphs._100y._R,X
		CMP		_tmpE6
		BCC		loc_104671
		STA		_tmpE6
loc_104671:
		LDA		_graphs._100y._C+1,X
		STA		_graphs._100y._C,X
		CMP		_tmpE6
		BCC		loc_10467D
		STA		_tmpE6
loc_10467D:
		LDA		_graphs._100y._I+1,X
		STA		_graphs._100y._I,X
		CMP		_tmpE6
		BCC		loc_104689
		STA		_tmpE6
loc_104689:
		LDA		_graphs._100y._pollution+1,X
		STA		_graphs._100y._pollution,X
		LDA		_graphs._100y._money+1,X
		STA		_graphs._100y._money,X
		LDA		_graphs._100y._crime+1,X
		STA		_graphs._100y._crime,X
		INX
		CPX		#$09
		BNE		loc_104665
		LDA		_tmpE6
		STA		_RCI_max_range_100_years		; TODO: fixme, the same unused max value
		LDA		_graphs._10y._R+9
		STA		_graphs._100y._R+9
		LDA		_graphs._10y._C+9
		STA		_graphs._100y._C+9
		LDA		_graphs._10y._I+9
		STA		_graphs._100y._I+9
		LDA		_graphs._10y._pollution+9
		STA		_graphs._100y._pollution+9
		LDA		_graphs._10y._money+9
		STA		_graphs._100y._money+9
		LDA		_graphs._10y._crime+9
		STA		_graphs._100y._crime+9
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_fiscal_year_results_calc:
; FIX, for some reason, here were zeroed only lower nibble of value, let them zero completely
		MOVDI	_taxes_income, 0
; -
		MOVW	_tmp6CC,_city._tax_year_sum	; get sum of year taxes
		MOVWI	_city._tax_year_sum,0			; reset global value then
		LDA		_budget_scr_draw_req			; if we already on the budget screen when end of the year
		BEQ		loc_1046EB						; happens, skip this
		RTS
loc_1046EB:
		LDA		_rails_count+1					; summing all needed values
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
		PUSHW	_tmpE6							; multiplier 1 E6 = (rails_count*2)+_roads_count
		LDY		_city._difficult
		LDA		_trans_fund_coefs_int,Y			; multiplier 2 coeff = diff[1,2/0.9/0.7]
		PHA
;		LDY		_city._difficult				; REDUNDANT, Y already loaded with difficult here
		LDA		_trans_fund_coefs_frac,Y
		PHA
		JSR		_mmc5_mul16to16					; E6 * coeff
		PLA
		POPW	_fund_request_trans				; total funds request for transport system value
		PLA
		MMULB8	_fund_request_police,_police_stations_count,#$64	; other requests are simpler. just
		MMULB8	_fund_request_fire,_fire_stations_count,#$64		; number of corresponding building times 100
		MMULB8	_fund_request_education,_schools_count,#$32			; or times 50 for schools
;
; NOTE, see, there is no hospitals because this is CAPITALISM and medicine uses insurance systemfor funding itself
;
		PUSHW	_cur_RCI_levels_sum				; two fp8_8 values multiply
		PUSHW	_city._land_val_avr
		JSR		_mmc5_mul16to16					; (L_R+L_C+L_I)*_land_val_avr
		PLA										; skip lower FRAC nibbles
		PLA
		TAX
		PLA
		TAY										; result in X/Y now
		PLA										; skip higher nibble as well
		TYA										; push result again for next multiply
		PHA
		TXA
		PHA
		LDY		_city._difficult
		LDA		_tax_coefs_int,Y				; multiply to scale coeff diff[1.4/1.2/0.8]
		PHA
		LDA		_tax_coefs_frac,Y
		PHA
		JSR		_mmc5_mul16to16					; (L_R+L_C+L_I)*_land_val_avr*diff[1.4/1.2/0.8]
		PLA										; migrate results to the multiply arguments again
		PLA										; looks crypic but little bit faster than using
		TAX										; temp vars
		PLA
		TAY
		PLA
		TYA
		PHA
		TXA
		PHA
		PUSHB	#$B6							; final scale coeff common for all difficulties = 0.71
		JSR		_mmc5_mul16to8					; multiply fp8_8 to fp0_8
		PLA
		PUSHW	_tmp6CC							; result already in stack, just push _tax_year_sum
		JSR		_mmc5_mul16to16					; full equation is
		PLA										; _city_taxes_collected=(L_R+L_C+L_I)*_land_val_avr*diff[1.4/1.2/0.8]*0.71*_tax_year_sum
		POPD	_city_taxes_collected			; result is 24-bit
		LDA		_cur_RCI_levels_sum.FRAC		; if there is no buildings at all, then skip budget screen
		ORA		_cur_RCI_levels_sum.INT
		BNE		.do_budget_screen
		LDA		#$0F
		STA		_fund_rate_police				; default values preventing unnecessary alerts
		STA		_fund_rate_fire
		RTS
.do_budget_screen:
		LDA		#$01							; or else set the budget screen request
		STA		_budget_scr_draw_req			; as well as suspend THREAD1 by now
		STA		_game_core_state
loc_1047C7:
		LDA		_budget_scr_draw_req			; wait for request is clear means that
		BNE		loc_1047C7						; the budget screen were displayed, and
; REDUNDANT, all cals here are already has been done in budget screen calculations
; as against the budget screen, these cals are linear and non-onpimal at all
; also, most of these values are already calculated in budget screen
; -
; REDUNDANT, this value already calculated and can be accessed via _budget_money
;		LDA		_city._money					; player has finished adjustements, so
;		CLC										; now all fundings are recalculated
;		ADC		_city_taxes_collected			; and we can finally spend our money.
;		STA		_city._money					; NOTE, that budget can't get more money
;		LDA		_city._money+1					; than you have, so further subtraction never
;		ADC		_city_taxes_collected+1			; goes over
;		STA		_city._money+1
;		BCC		loc_1047E4
;		INC		_city._money+2					; first add taxes collected to our money
; -
; OPTIMIZED
		MOVD	_city._money,_budget_money
; -
loc_1047E4:
; REDUNDANT, all funding alloc sums including loan are already calculated in _budget_funds_alloc_sum_loan
; we only need to adjust loan period here
;		SUBDW	_city._money,_fund_alloc_trans	; subtract funds allocated
;		SUBDW	_city._money,_fund_alloc_fire
;		SUBDW	_city._money,_fund_alloc_police
;		SUBDW	_city._money,_fund_alloc_education
; -
; OPTIMIZED
		SUBD	_city._money,_budget_funds_alloc_sum_loan
; -
		LDA		_city._loan_period				; if loan period isn't zero
		BEQ		loc_10486C
		DEC		_city._loan_period				; decrement loan year count
		BNE		loc_104856
		LDA		_city._bank_flags				; if the last year, clear loan flag
		AND		#$BF
		STA		_city._bank_flags
loc_104856:
;		SUBDI16	_city._money,500				; REDUNDANT, loan amount hardcoded here, the var never used
loc_10486C:
; BUG, note, we calculate here _taxes_income value, it used only for graphs data display
; but as you may see, they calculate it for fund request values, not fund alloc values
; so results may be either negative and sometimes not correct, however, if all fundings
; on 100%, then these values are equal, also this value does not include education alloc!
;		MSUBW	_tmpE6,_city_taxes_collected,_fund_request_trans
;		MSUBW	_tmpE6,_tmpE6,_fund_request_fire
;		MSUBW	_taxes_income,_tmpE6,_fund_request_police
; -
; FIX, OPTIMIZED
		MSUBD	_taxes_income,_city_taxes_collected,_budget_funds_alloc_sum_loan
; -
; REDUNDANT, also ingame rates values are not in percents, but in fixed point fraction only
; form: 0.xx and in range [0.00 to 0.129] for transport funds and [0.00 to 0.062] for
; the rest. or in integer form: [0 to 21] and [0 to F] correspondingly.
;
; also, for some odd reason, rates other than transport are clamped to max 0.99, and
; only transport funds may be 1.00 when fully funded...
;
;		LDA		_fund_request_trans				; test if divider is zero
;		ORA		_fund_request_trans+1
;		BNE		loc_1048A9
;		LDA		#$20							; max default value here is $20 (0.125) but
;		BNE		loc_1048D7						; next code does not clamp 1.0 to 0.99
;loc_1048A9:									; and thus this value may be $21 instead
;		MOVW	_tmpE7,_fund_alloc_trans
;		MOVW	_tmpEA,_fund_request_trans
;		LDA		#$00
;		STA		_tmpE6
;		STA		_tmpE9
;		JSR		_div_32to16						; alloc/request proportion
; BUG, for some reason, this value not clamped to 0.99 as the rest ones, but
; normalized the same way with value +1 greater than max default rate ($20 -> $21)
; so at some point of the game, calculated value will be greater than max $20
; and tested at some point incorrectly to be exactly 20. there are two ways to
; fix this. make similar clamp to 0.99 here. or remove strict comparison
; there... better here, because we may optimize the whole block slightly then
;
;		PUSHW	_tmpE6
;		PUSHB	#$21
;		JSR		_mmc5_mul16to8					; normalized to $00-$21 range as int8
;		PLA
;		PLA										; we need only FRAC part of result
;		TAX										; keep it in X for now
;		PLA										; drop INT part, then
;		TXA										; put FRAC in A again
; -
;loc_1048D7:
;		STA		_fund_rate_trans				; store normalized value
; REDUNDANT, two more identical code portions which may be merged to one routine
;
;		LDA		_fund_request_police
;		ORA		_fund_request_police+1
;		BNE		loc_1048E6
;		LDA		#$0F							; default $0F (0.059)
;		BNE		loc_104914
;loc_1048E6:
;		MOVW	_tmpE7,_fund_alloc_police
;		MOVW	_tmpEA,_fund_request_police
;		LDA		#$00
;		STA		_tmpE6
;		STA		_tmpE9
;		JSR		_div_32to16						; alloc/request proportion
;		LDA		_tmpE6							; get FRAC part
;		LDY		_tmpE7							; if INT part not zero,
;		BEQ		loc_10490B						; clamp FRAC part to 0.999
;		LDA		#$FF
;loc_10490B:
;		PHA
;		PUSHB	#$10							; and then normalize 0.0-0.999 range to
;		JSR		_mmc5_mul8to8					; int [0-F]
;		PLA
;		PLA
;loc_104914:
;		STA		_fund_rate_police				; store value
;		LDA		_fund_request_fire				; the last calc for fire stations
;		ORA		_fund_request_fire+1
;		BNE		loc_104923						; the same default value as for police
;		LDA		#$0F
;		BNE		loc_104951
;loc_104923:
;		MOVW	_tmpE7,_fund_alloc_fire
;		MOVW	_tmpEA,_fund_request_fire
;		LDA		#$00
;		STA		_tmpE6
;		STA		_tmpE9
;		JSR		_div_32to16
;		LDA		_tmpE6
;		LDY		_tmpE7
;		BEQ		loc_104948
;		LDA		#$FF
;loc_104948:
;		PHA
;		PUSHB	#$10
;		JSR		_mmc5_mul8to8					; the same normalization
;		PLA
;		PLA
;loc_104951:
;		STA		_fund_rate_fire
;		RTS
; -
; OPTIMIZED
		LDX		#$00
		JSR		_city_fund_rate_recalc
		LDX		#$02
		JSR		_city_fund_rate_recalc
		LDX		#$04
		JMP		_city_fund_rate_recalc
; -

_trans_fund_coefs_frac:
		.BYTE	$B3,$E6,$33
_trans_fund_coefs_int:
		.BYTE	$00,$00,$01
_tax_coefs_frac:
		.BYTE	$66,$33,$CC
_tax_coefs_int:
		.BYTE	$01,$01,$00

; =============== S U B	R O U T	I N E =======================================
; var order, fire, police, trans in ram.
;
_city_fund_rate_recalc:
		LDA		_fund_request_fire,X			; test if divider is zero
		ORA		_fund_request_fire+1,X
		BNE		loc_1048A9
		LDA		_default_city_fund_rate_list,X	; load default values
		PHA										; stack store trick
		BNE		loc_1048D7
loc_1048A9:
		LDA		_fund_alloc_fire,X
		STA		_tmpE7
		LDA		_fund_alloc_fire+1,X
		STA		_tmpE7+1
		LDA		_fund_request_fire,X
		STA		_tmpEA
		LDA		_fund_request_fire+1,X
		STA		_tmpEA+1
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		JSR		_div_32to16						; alloc/request proportion
		LDA		_tmpE6							; get FRAC part
		LDY		_tmpE7							; if INT part not zero,
		BEQ		loc_1048D6						; clamp FRAC part to 0.999
		LDA		#$FF
loc_1048D6:
		PHA
		LDA		_default_city_fund_rate_list+1,X; and then normalize 0.0-0.99 range to
		PHA										; int [0-20] for trans, [0-F] for fire/police
		JSR		_mmc5_mul8to8
		PLA										; result here still in stack! keep it
loc_1048D7:
		TXA										; recalc X*2 to X
		LSR
		TAX
		PLA										; retrive result from stack
		STA		_fund_rate_fire,X				; store normalized value
		RTS

_default_city_fund_rate_list:
		.BYTE	$0F,$10							; default value + normalization value
		.BYTE	$0F,$10
		.BYTE	$20,$21

; =============== S U B	R O U T	I N E =======================================
; NOTE, all this stuff calculated at the end of the year only.
; so the city problems don't work for the first year and
; may be alrady solved but you don't know until the next year
;
_city_stats_recalc:
		LDA		_cur_RCI_levels_sum.FRAC		; calculated previously
		ORA		_cur_RCI_levels_sum.INT
		BEQ		loc_10497B
		JSR		_city_assessed_value_recalc		; assessed value recalc
		JSR		_city_net_migration_relalc		; city net migration recalc
		JSR		_city_problems_list_recalc		; list of current city problems
		JSR		_city_score_recalc
		JMP		_city_votes_recalc				; OPTIMIZED
;		JMP		loc_10497E						; REDUNDANT
loc_10497B:
		JSR		_city_stats_reset				; moved to PRGF system bank
loc_10497E:
;		LDA		#$01							; REDUNDANT, never used
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_assessed_value_recalc:
		PUSHW	_roads_count					; E6 = roads*5
		PUSHB	#$05
		JSR		_mmc5_mul16to8
		POPD	_tmpE6
		PUSHW	_rails_count
		PUSHB	#$0A
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += rails*10
		PUSHWI	$03E8
		LDA		_police_stations_count
		CLC
;		ADC		_fires_count					; BUG! wrong var used
		ADC		_fire_stations_count			; FIX, proper value, based on Micropolis source
		PHA
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += (police + fire) * 1000
;		PUSHWI	$9001							; BUG! SWAP NIBBLES
		PUSHWI	$0190							; FIX, correct value!
		PUSHB	_hospitals_count
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += hospitals * 400
;		PUSHWI	$B80B							; BUG! SWAP NIBBLES
		PUSHWI	$0BB8							; FIX, correct value!
		PUSHB	_stadiums_count
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += stadiums * 3000
;		PUSHWI	$8813							; BUG! SWAP NIBBLES
		PUSHWI	$1388							; FIX, correct value!
		PUSHB	_sea_ports_count
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += sea ports * 5000
;		PUSHWI	$1027							; BUG! SWAP NIBBLES
		PUSHWI	$2710							; FIX, correct value!
		PUSHB	_airports_count
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += air ports * 10000
;		PUSHWI	$B80B							; BUG! SWAP NIBBLES
		PUSHWI	$0BB8							; FIX, correct value!
		PUSHB	_coal_power_station_count
		JSR		_mmc5_mul16to8
		POPADDD	_tmpE6,_tmpE6					; E6 += coal power * 3000
;		PUSHWI	$7017							; BUG! SWAP NIBBLES
		PUSHWI	$1770							; FIX, correct value!
		PUSHB	_nuke_power_station_count
		JSR		_mmc5_mul16to8
; NOTE, the result is assessed value of micropolis source code has this value
; additionally multiplied to 1000, the NES version don't
;
		POPADDD	_assessed_value,_tmpE6			; _assessed_value = E6 + (nuclear power * 6000)
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_net_migration_relalc:
		MSUBD	_popul_net_migration,_popul_cur,_popul_last_year	; calc delta between current population and last year population
		MOVD	_popul_last_year,_popul_cur	; save current population for the next year
; REDUNDANT, merged, reused code
;		LDY		#$05
;loc_104AF0:
;		LDA		_popul_cur
;		CMP		_popul_city_type_list0,Y
;		LDA		_popul_cur+1
;		SBC		_popul_city_type_list1,Y
;		LDA		_popul_cur+2
;		SBC		_popul_city_type_list2,Y
;		BCS		loc_104B07
;		DEY
;		BPL		loc_104AF0
;loc_104B07:
;		STY		_city._city_type
; -
; OPTIMIZED
		LDX		#$04							; start from MEGAPOLIS value and down
loc_104AF0:
		JSR		_popul_compare					; existing routine used
		BCS		loc_104AF1
		DEX
		BPL		loc_104AF0
loc_104AF1:
		INX
		STX		_city._city_type
; -
		RTS

; REDUNDANT, merged copies
;_popul_city_type_list2:
;		.BYTE	$00
;		.BYTE	$00
;		.BYTE	$00
;		.BYTE	$00
;		.BYTE	$01
;		.BYTE	$07
;_popul_city_type_list1:
;		.BYTE	$00
;		.BYTE	$07
;		.BYTE	$27
;		.BYTE	$C3
;		.BYTE	$86
;		.BYTE	$A1
;_popul_city_type_list0:
;		.BYTE	$00
;		.BYTE	$D0
;		.BYTE	$10
;		.BYTE	$50
;		.BYTE	$A0
;		.BYTE	$20


; =============== S U B	R O U T	I N E =======================================
; this routine calculates the list of city wors problems,
; there are three tmp buffers 2*7-bytes each:
; 6CE-6DB (_problem_data) 		- set initial parameters
; 6DC-6E2 (_problem_accepted)	- flags when problem is selected to sorting list
; 6E3-6E9 (_problem_percent)	- a number of problem occurance during the random test
;
; the overall problem of comparing all these values is they are a different
; types of measurement units. they just can't be compared directly.
; there is no formula to merge all these values to the same value unit and
; then compare...
; so they decided to made it randomly selected. they compared all overall
; basic values against the randomly generated value. the higher currently
; selected problem, the higher probability they will fit inside the random
; generated value. however, even the value ranges are different, but the
; random test value is the same for all of them... so some problems
; would be higher than others always...
;
_city_problems_list_recalc:
		LDA		_city._crime_lvl_avr.FRAC		; crime level uses high bit as special flag
		STA		_problem_data					; remove it first
		LDA		_city._crime_lvl_avr.INT		; 6CE/6CF = crime_level & 0x7FFF
		AND		#$7F
		STA		_problem_data+1
		LDA		_city._poll_lvl_avr.FRAC		; 6D0/6D1 = pollution_level & 0x7FFF
		STA		_problem_data+2					; remove special flag bit as well, however
		LDA		_city._poll_lvl_avr.INT			; it never used by the game anymore,
		AND		#$7F							; pollution alert isn't implemented
		STA		_problem_data+3
		PUSHW	_city._land_val_avr				; 6D2/6D3 = land_value * 0.625
		PUSHB	#$A0							; (0.7 for Micropolis source code)
		JSR		_mmc5_mul16to8
		PLA
		POPW	_problem_data+4
		MMULB8	_problem_data+6,_city._tax_rate,#$0A; 6D4/6D5 = tax_rate * 10
		FJSR	_traf_lvl_avr_recalc,SRAM,PRGB
		MOVW	_problem_data+8,_city._traf_lvl_avr	; 6D6/6D7 = traffic level
		JSR		_get_unemployment
		STA		_problem_data+10				; FIX, now 6D8 = R/(C+I)*8 - 1
		JSR		_get_fires						; FIX, now 6DA = _fires_count * 5
		STA		_problem_data+12
		LDA		#$00
		STA		_problem_data+11				; 6D9 = 0
		STA		_problem_data+13				; 6DB = 0
		JSR		_city_problems_percents_calc	; random calc of the problems percents
		LDY		#$00
		TYA
loc_104B8F:
		STA		_problem_accepted,Y				; reset problems order list
		INY
		CPY		#$07
		BNE		loc_104B8F
		LDX		#$00							; init the problems counter (out of 4)
.do_all_fields:
		LDY		#$00							; reset max value for curretn cycle
		STY		_tmpE6
; BUG, this algo wery weak and gives incorrect sorting results.
; the Micropolis sources has the correct algo for this section.
;
; 1. it compares for greater AND equals, all equal values will be inserted one by one
;    and only the last of them will be stored
; 2. when store intermediate max value for current cycle, it marks stored value
;    as accepted in all cases, even if it will be overwritten with another value later.
;    as a result, this value won't be selected at all for other cycles
; 3. both of that problems cause that some problems going missing
;    and not the exactly maxed out percents are displayed!
;
;.do_all_problems:
;		LDA		_problem_accepted,Y				; test if accepted already
;		BNE		.next_problem_test
;		LDA		_problem_percent,Y				; find max value
;		CMP		_tmpE6
;		BCC		.next_problem_test				; greather or equal counts, make greater only!
;		STA		_tmpE6							; so we will record always the last from equals in list
;		STA		_worst_problem_percents,X		; then update percent and list idx
;		TYA
;		STA		_worst_problem_list,X			; store problem idx from 0 to 6
;		LDA		#$01
;		STA		_problem_accepted,Y				; mark problem as assepted even if they aren't max!
;.next_problem_test:
;		INY
;		CPY		#$07
;		BNE		.do_all_problems				; compare all 7 problems to find the max one
;		LDA		_tmpE6
;		BNE		.next_field_test				; if there is no one, set default values
;		LDA		#$FF
;		STA		_worst_problem_percents,X
;		STA		_worst_problem_list,X
; -
; FIX, correct algo
.do_all_problems:
		LDA		_problem_accepted,Y				; test if accepted already
		BNE		.next_problem_test
		LDA		_problem_percent,Y				; find max value
		CMP		_tmpE6
		BCC		.next_problem_test				; greather only
		BEQ		.next_problem_test
		STA		_tmpE6							; store max value
		TYA
		STA		_tmpE7							; as well store idx of cur max value only
.next_problem_test:
		INY
		CPY		#$07
		BNE		.do_all_problems				; compare all 7 problems to find the max one
		LDA		_tmpE6
		BNE		.store_problem					; if any non-zero value is in max var, store it
		LDA		#$FF							; if sero, then fill it with dummy value
		STA		_worst_problem_percents,X
		STA		_worst_problem_list,X
		BNE		.next_field_test
.store_problem:
		LDA		_tmpE6
		STA		_worst_problem_percents,X		; then update percent and list idx
		LDA		_tmpE7
		STA		_worst_problem_list,X			; store problem idx from 0 to 6
		TAY
		LDA		#$01
		STA		_problem_accepted,Y				; mark problem as assepted even if they aren't max!
; -
.next_field_test:
		INX
		CPX		#$04
		BNE		.do_all_fields					; repeat for 4 problems
		RTS

; =============== S U B	R O U T	I N E =======================================
; NOTE, from calculations below we can see, the problems tested agains
; random value from range [0-1.56], if random value less than problem
; value, this means problem is high enough to get a percent up
; also random value may be low enough to add to a problem percentage
; even small problem, so problems will always be here! randomly!
; also problem values often much higher than 1.56 even in better random
; distribution case, so all values above 1.56 will always get percents
; if there more than one problem higher than 1.56, they will share almost
; the same amount of percents.
;
; TODO: maybe we should rebalance this for real values ranges?
;
; In the micropolis sources its called "vote problems" and semi-random as well.
; but with other constants: 600 iteration per voting instead 512 (just for
; code optimization), and random treshold 300 instead of 400
;
_city_problems_percents_calc:
		LDY		#$00
		TYA
loc_104BD1:
		STA		_problem_percent,Y				; problems percents counter
		INY
		CPY		#$07
		BNE		loc_104BD1
		STA		_tmpE8
		STA		_tmpE9							; percents counter
		STA		_tmpEA
		STA		_tmpEB
loc_104BE1:
		PUSHWI	$0190							; fp8_8 = 1.56 or int = 400
		JSR		_rand							; (300 in original Micropolis source)
		PHA
		JSR		_mmc5_mul16to8					; E6/E7 = 400 * rand(256) integer or
		PLA										; or in fp8_8 1.56*[0-0.999]
		POPW	_tmpE6
		LDA		_tmpE8
		ASL
		TAX										; load current problem parameter idx
		LDA		_tmpE6							; random compared with selected problem parameter
		CMP		_problem_data,X
		LDA		_tmpE7
		SBC		_problem_data+1,X
		BCS		loc_104C0C						; if selected parameter higher than [0-1.56] range
		LDX		_tmpE8
		INC		_problem_percent,X				; then problem percent increased
		INC		_tmpE9							; and counter of percents decreased
loc_104C0C:
		INC		_tmpE8							; or else skip this value and choose the next one
		LDA		_tmpE8
		CMP		#$07
		BCC		loc_104C18						; wrap to the first problem value again
		LDA		#$00
		STA		_tmpE8
loc_104C18:
		INCW	_tmpEA							; increase overall attempts counter, if there are
		LDA		_tmpEB							; not much problems to get all 10 percents range
		CMP		#$02
		BCS		locret_104C2A					; stop when until overall attempts counter >512
		LDA		_tmpE9							; also stop when we distributed all percents
		CMP		#$64
		BCC		loc_104BE1
locret_104C2A:
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_unemployment:
; BUG! calculations chould prepare divider and divident, but
; divider uses wrong tmp var! E6 instead EA. so division here performed
; on correct divident and random divider
;
;		LSADDW	_tmpE6,_city._level._C,_city._level._I,3	; E6 =(L_C+L_I)*8
;		ORA		_tmpE6							; test if result 0
;
; FIX
		LSADDW	_tmpEA,_city._level._C,_city._level._I,3	; EA =(L_C+L_I)*8
		ORA		_tmpEA
; -
		BNE		loc_104C4C						; if yes, then result 0
;		LDA		#$00							; REDUNDANT, we get here if A already 0
		RTS
loc_104C4C:
		MOVW	_tmpE7,_city._level._R			; divident = L_R
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		JSR		_div_32to16
		LDA		_tmpE6
		LDY		_tmpE7
		DEY										; FIX for incorrect clamping!
		BEQ		locret_104C67					; clamp result to 0.999
		LDA		#$FF
locret_104C67:
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_fires:
		LDA		_fires_count					; FIX now has correct properly
		ASL										; utilized variables here
		ASL
		CLC
		ADC		_fires_count
		BCC		locret_104C75
		LDA		#$FF
locret_104C75:
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_score_recalc:
		MOVW	_prev_city_score,_city._score	; store the previous score for
		LDY		#$00							; annual change comparison
		STY		_tmpE6							; clear tmp vars
		STY		_tmpE7
		STY		_tmpE8
		STY		_tmpE9
		STY		_tmpEA
loc_104C8E:
		LDA		_problem_data,Y					; we uses here the problem values
		CLC										; calculated in previous routine
;		ADC		_tmpE6							; summing all 7 parameters
;		STA		_tmpE6
		ADC		_tmpE7							; FIX, rebalance
		STA		_tmpE7
		LDA		_problem_data+1,Y
; BUG, for some reasons they stored values
; one nibble lower than for usual dividends
; this lead the result be always too small
;		ADC		_tmpE7
;		STA		_tmpE7
		ADC		_tmpE8							; FIX, rebalance
		STA		_tmpE8
; BUG!!! AAAHAHAHAHAH
; _problem_data array consists of WORDS!, we need to
; increment counter by TWO! so the problems sum here would be
; less than actual value, but this HELP to avoid
; other bugs below! they goes same low for a
; base scores and go on... when we fixed the most of bugs except this
; the game started to show ALL GOOD SCORES even in case of total disaster
; lol
; -
;		INY
;		CPY		#$07
; FIX
		INY
		INY
		CPY		#$0E
; -
		BNE		loc_104C8E
		LDA		#$03							; average from Micropolis
		STA		_tmpEB
		JSR		_div_32to16						; E6 = problems_sum / 3
		LDA		_tmpE7
		BEQ		loc_104CB1
		LDA		#$FF
		STA		_tmpE6							; clamp E6 = {E6} (0.999)
loc_104CB1:
		LDA		#$00
		STA		_tmpE7
		LDA		_tmpE6							; E6 = (0.999-E6)*4, reverse value
; WARNING, case with 0 will calculated incorrectly here
; probably we need to use other method of negating this value
;		NEGA									; more problems, less score
; -
; TRY ME,
		EOR		#$FF
; -
		ASL
		ROL		_tmpE7
		ASL
		ROL		_tmpE7
; BUG! low nibble or multiplication not saved! means
; we compare _tmpE6 as a frankenstain combined from not-xored
; low nibble of _tmpE6 and high nibble of multiply result
; so we will have here wrong reversed value!
; FIX! (not so much optimal in code, STA and CMPWI will be
; load the same var, but easier to read, though.
		STA		_tmpE6
;
		CMPWI	_tmpE6,1000						; can't be unsigned here so no need to
		BCC		loc_104CD4						; clamp negative values
		MOVWI	_tmpE6,1000						; clamp to 1000
loc_104CD4:
		LDA		_required._stadium				; unfulfilled requests lower
		BEQ		loc_104CDC						; the score by 0.85 each
		JSR		_mul_fp0_85
loc_104CDC:
		LDA		_required._airport
		BEQ		loc_104CE4
		JSR		_mul_fp0_85
loc_104CE4:
		LDA		_required._seaport
		BEQ		loc_104CEC
		JSR		_mul_fp0_85
loc_104CEC:
; NOTE, previous routine were calculated this
; value with overflow, when at 100% the value
; here would be 21 instead of 20 which lowered the score!
;
		LDA		_fund_rate_trans				; test if transport funds are 100%
		CMP		#$20							; if less than, decrease scores to negative amount
		BEQ		loc_104D0A						; E6 += (32 - _fund_rate_trans)
		ADDWB	_tmpE6,#$20
		LDA		_tmpE6
		SEC
		SBC		_fund_rate_trans
		STA		_tmpE6
		BCS		loc_104D0A
		DEC		_tmpE7
loc_104D0A:
		LDA		_fund_rate_police				; lack of police and fire funding lowers
		CMP		#$0F							; the scores to E6 = (E6 * (0.95 + funding))
		BCS		loc_104D14
		JSR		_mul_a_add_fp0_95
loc_104D14:
		LDA		_fund_rate_fire
		CMP		#$0F
		BCS		loc_104D1E
		JSR		_mul_a_add_fp0_95
loc_104D1E:
; BUG!, comparison on the delta value results always
; false here, because CMP opcode does not set the V
; overflow flag! also, the original Micropolis code
; lowers the score here only if delta is negative.
; values for negative here are FF-80, so we must test
; the S bit instead!
		LDA		_city._delta._R
;		CMP		#$40							; BUG
;		BVC		loc_104D28
		BPL		loc_104D28						; FIX, lower scores if negative
		JSR		_mul_fp0_85
loc_104D28:
		LDA		_city._delta._C
;		CMP		#$40							; BUG
;		BVC		loc_104D32
		BPL		loc_104D32						; FIX, lower scores if negative
		JSR		_mul_fp0_85
loc_104D32:
		LDA		_city._delta._I
;		CMP		#$40							; BUG
;		BVC		loc_104D3C
		BPL		loc_104D3C						; FIX, lower scores if negative
		JSR		_mul_fp0_85
loc_104D3C:
		LDA		_popul_cur						; test for cur population, if zero,
		ORA		_popul_cur+1					; skip net migration test
		ORA		_popul_cur+2
		BEQ		.j_sity_score_finish
		LDA		_popul_net_migration			; also if _popul_net_migration = 0
		ORA		_popul_net_migration+1
		ORA		_popul_net_migration+2
		BEQ		.j_sity_score_finish
		LDA		_popul_cur
		CMP		_popul_net_migration			; also if _popul_net_migration = _popul_cur
		BNE		.net_migration_sum				; which will be the case after first year
		LDA		_popul_cur+1					; of your city.
		CMP		_popul_net_migration+1
		BNE		.net_migration_sum
		LDA		_popul_cur+2
		CMP		_popul_net_migration+2
;		BNE		.net_migration_sum				; REDUNDANT
.j_sity_score_finish:
;		JMP		.sity_score_finish				; REDUNDANT, too far jump for near opcode
		BEQ		.sity_score_finish				; OPTIMIZED
.net_migration_sum:
		PUSHW	_tmpE6							; OPTIMIZED
		LDA		_popul_net_migration+2
		BMI		.negative_net_migration
;		PUSHW	_tmpE6							; REDUNDANT, push current score value as a multiply arg0
		LDA		#$00							; if net migration is positive
		STA		_tmpE9
		MOVD	_tmpE6,_popul_net_migration		; divident = _popul_net_migration
		MOVW	_tmpEA,_popul_cur+1				; divider = higher nibbles of _popul_cur
		JSR		_div_32to16						; result = ((_popul_net_migration)/(_popul_cur/256))+1.0
		INC		_tmpE7
; REDUNDANT
;		PUSHW	_tmpE6							; second multiply arg
;		JSR		_mmc5_mul16to16
;		PLA
; BUG! and again we have here WRONG result var. following code uses E6 as a result
; and clamp it once more after multiplications... as a result, we have correct score
; values only for first two years, for all next years the multiply result here
; is lost and the previous operation result used instead.
; so we have the annual score here not E6=E6*(1.0+((_popul_net_migration)/(_popul_cur/256)))
; but only E6=(1.0+((_popul_net_migration)/(_popul_cur/256)))
;
;		POPW	_tmpE8							; result = E6*(1.0+((_popul_net_migration)/(_popul_cur/256)))
;		POPW	_tmpE6							; FIX
;		PLA
;		JMP		.sity_score_finish				; lowers the score wehn lower migration is
; -
		JMP		.net_migration_mul				; OPTIMIZED
.negative_net_migration:
;		PUSHW	_tmpE6							; REDUNDANT, for negative net migration all mostly the same
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
		MOVWI	_tmpEA,_popul_cur+1
		JSR		_div_32to16
		LDA		#$01
		SEC
		SBC		_tmpE7
		STA		_tmpE7
		LDA		#$00
		SBC		_tmpE6
		STA		_tmpE6
.net_migration_mul:
		PUSHW	_tmpE6
		JSR		_mmc5_mul16to16					; result = E6*(1.0-(abs(_popul_net_migration)/(_popul_cur/256)))
		PLA										; little bit different from Micropolis maybe some kind of balancing there was
;		POPW	_tmpE8							; BUG!
		POPW	_tmpE6							; FIX
		PLA
.sity_score_finish:
		LDA		_tmpE7
		BPL		loc_104E05						; if negative, clamp to 0
		MOVWI	_tmpE6,0
		BEQ		loc_104E17
loc_104E05:
		CMPWI	_tmpE6,1000						; if positive, clamp to 1000
		BCC		loc_104E17
		MOVWI	_tmpE6,1000
loc_104E17:
		LDA		_city._score					; interpolate with previous value
		CLC
		ADC		_tmpE6							; annual change = (new score + old score)/2 - old score
		STA		_city._score					; so every time we got completely zero score, the
		LDA		_city._score+1					; annual change is half of previous value.
		ADC		_tmpE7
		LSR
		STA		_city._score+1
		ROR		_city._score
		LDA		_city._score
		SEC
		SBC		_prev_city_score
		STA		_annual_score_change			; calculate annual score change value
		LDA		_city._score+1
		SBC		_prev_city_score+1
		STA		_annual_score_change+1
		RTS

; =============== S U B	R O U T	I N E =======================================
; helper mul 0.85
;
_mul_fp0_85:
		PUSHW	_tmpE6
;		PUSHB	#$D9							; REDUNDANT
		LDA		#$D9							; OPTIMIZED
		BNE		_mul_stack_a_popw_tmpE6			; -
;		JSR		_mmc5_mul16to8					; REDUNDANT
;		PLA
;		POPW	_tmpE6
;		RTS

; =============== S U B	R O U T	I N E =======================================
; helper mul (arg + 0.94), arg in range [0-14], mul range [0.94-0.99]
;
_mul_a_add_fp0_95:
		CLC
		ADC		#$F1
		TAX
		PUSHW	_tmpE6
		TXA

; !FGALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_mul_stack_a_popw_tmpE6:						; OPTIMIZED, merged same parts
		PHA
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmpE6
		RTS

; =============== S U B	R O U T	I N E =======================================
_city_votes_recalc:
		LDA		#$00							; very simple votes calculation
		STA		_doing_a_good_job_votes
		STA		_doing_a_bad_job_votes
		LDY		#100							; 100 iterations to get all votes
loc_104E75:
		PUSHWI	1000							; random value from 0 to 1000...
		JSR		_rand
		PHA
		JSR		_mmc5_mul16to8
		PLA
		POPW	_tmpE6
		CMPW	_tmpE6,_city._score				; ...compared with city score
		BCS		loc_104E9A						; if random number less than score
		INC		_doing_a_good_job_votes			; increase good job counter
		BNE		loc_104E9D						; if higher, then bad job increased
loc_104E9A:
		INC		_doing_a_bad_job_votes			; so, the higher scores, less bad votes
loc_104E9D:
		DEY
		BNE		loc_104E75
		RTS

		SECTION_ENDS B02, "PRGB GRAPHS FISCAL YEAR CITY EVAL RECALCS"

; ---------------------------------------------------------------------------
; ----------------------E-V-E-N-T-S--S-E-L-E-C-T-O-R-------------------------
; ---------------------------------------------------------------------------

		SECTION_START B03

; =============== S U B	R O U T	I N E =======================================
; selector run one cycle for all possible events and test unmasked ones
; if they need to be displayed.
;
_events_selector:
		JSR		_scenario_result_test			; here we test for scenarios completion
		JSR		_practice_events_test			; practice has additional hint events
		JSR		_scroll_msg_selector			; select scroll warning messages
		LDA		_game_msg_awaits_flag
		BNE		locret_104ED8					; skip not if we already selected any messages
;		LDA		_game_msg_idx					; REDUNDANT, we not using it anymore
;		STA		_prev_game_msg_idx				; REDUNDANT, save currently selected message to prevent
		LDX		#$00							; double triggering by accident
		LDY		#$00
.next_byte_loop:
		LDA		_city._events_flags,X		; first fetch event triggers
		STA		_tmpE6
.bits_loop:
		LSR		_tmpE6							; exctract bit
		BCS		loc_104ECB						; if already set, skip check
		TXA										; backup X and Y counters
		PHA
		TYA
		PHA
		JSR		_presents_events_check			; execute test for first $1B special
		PLA										; items, the rest $1B to $23 are
		TAY										; called from special handlers
		PLA
		TAX
loc_104ECB:
		INY
		CPY		#$1B
		BEQ		locret_104ED8
		TYA
		AND		#$07
		BNE		.bits_loop						; every 8 bits, get new one from array
		INX
		BNE		.next_byte_loop
locret_104ED8:
		RTS

; =============== S U B	R O U T	I N E =======================================
_presents_events_check:
		LDA		_presents_events_lib_lo,Y		; fast switch, _switch is unsafe for _ptr0 anyway
		STA		_tmpE7
		LDA		_presents_events_lib_hi,Y
		STA		_tmpE8
		JMP		(_tmpE7)
_presents_events_lib_lo:
		.BYTE	<[nullsub_1]					; default hint event have no conditions to test
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event01_city_growt]
		.BYTE	<[_event06_total_jam]
		.BYTE	<[_event07_traffic]
		.BYTE	<[_event08_crime]
		.BYTE	<[nullsub_1]					; as well as MONSTER and QUAKE events, then forced
		.BYTE	<[nullsub_1]					; at a custom handler
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
		.BYTE	>[nullsub_1]
		.BYTE	>[nullsub_1]
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
		TYA										; common handler for all city growth messages
		TAX										; Y here 1 to 5, message indexes = Y-1
		DEX
		JSR		_popul_compare					; compare population with library value
		BCC		nullsub_4						; if less, then skip
		TYA
		PHA
		STY		_city._city_type				; store new city type
		JSR		_city_history_write				; write history record
		PLA
		TAY
		JMP		_register_new_event				; use this only once even if your is small again

; =============== S U B	R O U T	I N E =======================================
_event06_total_jam:
		LDA		_city._traf_lvl_avr.INT			; very high traffic value above $700
		BMI		nullsub_4						; test if already displayed for current loop
		CMP		#$07
		BCC		nullsub_4						; to trigger this event needed
		ORA		#$80
		STA		_city._traf_lvl_avr.INT			; set flag to avoid triggering before next loop
		JMP		_force_new_event				; can't be masked, always active

; =============== S U B	R O U T	I N E =======================================
_event07_traffic:
		LDA		_city._traf_lvl_avr.INT			; the same for low traffic alert
		BMI		nullsub_4
		CMP		#$02
		BCC		nullsub_4						; trigger value $200
		ORA		#$80
		STA		_city._traf_lvl_avr.INT			; this flag clears only at the end of the year
		JMP		_force_new_event				; when city problems are recalculated

; =============== S U B	R O U T	I N E =======================================
_event08_crime:
		LDA		_city._crime_lvl_avr.INT		; crime above $700 average (old values)
		BMI		nullsub_4						; new values are up to 1.6
		CMP		#$07							; TODO, recalibrate me!
		BCC		nullsub_4
		ORA		#$80
		STA		_city._crime_lvl_avr.INT		; this flag reset when value recalculated
		JMP		_force_new_event

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;nullsub_3:
;		RTS

; =============== S U B	R O U T	I N E =======================================
_event0B_bank:
		JSR		_check_for_free_present_slot	; lets start to give a presents
		BNE		nullsub_4						; check if we have free slot.
		CMPW	_city._cur_year,_city._year_to_bank	; if not, don't worry, the event will wait
		BCC		nullsub_4						; test for selected year
		JMP		_register_new_event				; register new event

; =============== S U B	R O U T	I N E =======================================
nullsub_4:
		RTS

; =============== S U B	R O U T	I N E =======================================
_event0C_relationships:
		LDX		#$05							; relationship event triggers
		JSR		_popul_compare					; above 150000 population
		BCC		nullsub_4						; no present here yet
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event0D_amusement:
		JSR		_check_for_free_present_slot	; for amuseemnt park present
		BNE		nullsub_4						; you need at least 150000 population
		LDX		#$06
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event0E_zoo:
		JSR		_check_for_free_present_slot	; 250000 for zoo
		BNE		nullsub_4
		LDX		#$07
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event0F_casino:
		JSR		_check_for_free_present_slot	; 200000 for casino
		BNE		nullsub_4						; nice, eh? you need less people to
		LDX		#$08							; build casino than a zoo
		JSR		_popul_compare
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event10_extra_land:
		JSR		_check_for_free_present_slot	; extra land given only once sadly
		BNE		nullsub_4						; as against SNES version...
		LDA		_sea_ports_count				; just build more than one port
		CMP		#$02
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event11_new_police:
		JSR		_check_for_free_present_slot	; 10 police stations gives new
		BNE		nullsub_4						; hyper mega police station.
		LDA		_police_stations_count			; maxes out the police range
		CMP		#$0A
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event12_new_fire:
		JSR		_check_for_free_present_slot	; the same for fire stations
		BNE		nullsub_4						; a lot of money for funding!
		LDA		_fire_stations_count
		CMP		#$0A
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event13_10th_monument:
		JSR		_check_for_free_present_slot	; 10th year anniversary for town
		BNE		nullsub_4						; build a monument
		CMPWI	_city._cur_year,1910
		BCC		nullsub_4
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event14_100th_monument:
		JSR		_check_for_free_present_slot	; also 100 anniversary, blocked in
		BNE		nullsub_2						; all scenarios though
		CMPWI	_city._cur_year,2000
		BCC		nullsub_2
		JMP		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event15_expo:
		CMPWI	_city._cur_year,1950			; start an expo fixed year. available
		BNE		nullsub_2						; only in practice and free modes
		BEQ		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event16_expo_monument:
		JSR		_check_for_free_present_slot	; the year later after expo
		BNE		nullsub_2						; we build a monument
		CMPW	_city._cur_year,_city._year_to_expo
		BCC		nullsub_2
		BCS		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event17_liberty:
		JSR		_check_for_free_present_slot	; 10 years after establishing relationship
		BNE		nullsub_2
		CMPW	_city._cur_year,_city._year_to_liberty
		BCC		nullsub_2
		BCS		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event18_library:
		JSR		_check_for_free_present_slot	; 70000 needed to build a library
		BNE		nullsub_2
		LDX		#$09
		JSR		_popul_compare
		BCC		nullsub_2
		BCS		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event19_park:
		JSR		_check_for_free_present_slot	; 200 park tiles to build an larger park
		BNE		nullsub_2
		LDA		_parks_counter
		CMP		#200
		BCC		nullsub_2
		BCS		_register_new_event

; =============== S U B	R O U T	I N E =======================================
_event1A_railroad:
		JSR		_check_for_free_present_slot	; 150 railroad tiles also
		BNE		nullsub_2
		LDA		_rails_count
		CMP		#150
		BCC		nullsub_2
		BCS		_register_new_event

; =============== S U B	R O U T	I N E =======================================
nullsub_2:
		RTS

; =============== S U B	R O U T	I N E =======================================
_register_new_event:
		STY		_game_msg_idx					; here we jump to register new event
		TYA										; calculate byte idx8 = msg_idx / 8
		LSR										; and a bit index idx1 = msg_idx & 7
		LSR
		LSR
		TAX
		TYA
		AND		#$07
		TAY
		LDA		_city._events_flags,X		; read flags, set corresponding one
		ORA		_bitmasks,Y
		STA		_city._events_flags,X		; _game_msg_idx already set, Y missnig
		BNE		_new_event_break				; unconditional jump

; =============== S U B	R O U T	I N E =======================================
_force_new_event:
		STY		_game_msg_idx					; events that need to be displayed constantly goes here

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_new_event_break:
; BUG! Y here is not the same after _register_new_event branch, than
; from previous !FALLTHROUGH! entry lol, will fail always, whey event idx
; is pow of 2^3, so it will be 0 and test with 0, then never appear
; automatically
; FIX, not sure why we should test if it can be double triggered, this
; seems impossible currently, so just disable this code to fix it
;		CPY		_prev_game_msg_idx
;		BEQ		loc_105059
		LDA		#$01
		STA		_game_msg_awaits_flag
;loc_105059:
		PLA										; break two JSRs up
		PLA
		PLA
		PLA
		RTS

; =============== S U B	R O U T	I N E =======================================
_check_for_free_present_slot:
		LDX		#$02
		LDA		_city._events_flags		; WEIRD, test if city status acquired, but why?
		AND		#$04
		BEQ		loc_105069						; if no, only 3 slots are available
		LDX		#$03
loc_105069:
		LDA		_city._presents_list,X			; empty slot always zero
; BUG! it'll always get the last slot, there can't by holes
; in presents list, they are always sorted after item removed from them
; so we don't need to loop here. anyway, this loop does worse thing
; it may loop for 256 redundant cycles if all slots are filled already
;		BEQ		locret_105071					; FIX, just remove it
;		DEY
;		BPL		loc_105069
;locret_105071:
		RTS

; =============== S U B	R O U T	I N E =======================================
; most of the time this routine called from PRGB, here it used only once
;
_city_history_write:
		TYA										; backup history event index
		PHA
		LDY		#$00
loc_105076:
		LDA		_city._history_list,Y			; look for free slot
		BEQ		loc_10508F
		INY
		INY
		INY
		CPY		#$15
		BNE		loc_105076
		LDY		#$00							; when history is full,
loc_105084:
		LDA		_city._history_list+3,Y			; shift up and free last slot
		STA		_city._history_list,Y
		INY
		CPY		#$12
		BNE		loc_105084
loc_10508F:
		PLA										; put an history index, then year
		STA		_city._history_list,Y
		LDA		_city._cur_year
		STA		_city._history_list+1,Y
		LDA		_city._cur_year+1
		STA		_city._history_list+2,Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_popul_compare:
		LDA		_popul_cur						; library compare function,
		CMP		_popul_vals_lib0,X				; all population values stored here
		LDA		_popul_cur+1					; no need to have a separate comparinsons
		SBC		_popul_vals_lib1,X
		LDA		_popul_cur+2
		SBC		_popul_vals_lib2,X
		RTS

_popul_vals_lib0:
		.BYTE	[[2000 >> 0] & $FF]				; TOWN
		.BYTE	[[10000 >> 0] & $FF]			; CITY
		.BYTE	[[50000 >> 0] & $FF]			; CAPITAL
		.BYTE	[[100000 >> 0] & $FF]			; METROPOLIS
		.BYTE	[[500000 >> 0] & $FF]			; MEGAPOLIS
		.BYTE	[[150000 >> 0] & $FF]			; RELATIONSHIP
		.BYTE	[[120000 >> 0] & $FF]			; AMUSEMENT PARK
		.BYTE	[[250000 >> 0] & $FF]			; ZOO
		.BYTE	[[200000 >> 0] & $FF]			; CASINO
		.BYTE	[[70000 >> 0] & $FF]			; LIBERTY STATUE
_popul_vals_lib1:
		.BYTE	[[2000 >> 8] & $FF]
		.BYTE	[[10000 >> 8] & $FF]
		.BYTE	[[50000 >> 8] & $FF]
		.BYTE	[[100000 >> 8] & $FF]
		.BYTE	[[500000 >> 8] & $FF]
		.BYTE	[[150000 >> 8] & $FF]
		.BYTE	[[120000 >> 8] & $FF]
		.BYTE	[[250000 >> 8] & $FF]
		.BYTE	[[200000 >> 8] & $FF]
		.BYTE	[[70000 >> 8] & $FF]
_popul_vals_lib2:
		.BYTE	[[2000 >> 16] & $FF]
		.BYTE	[[10000 >> 16] & $FF]
		.BYTE	[[50000 >> 16] & $FF]
		.BYTE	[[100000 >> 16] & $FF]
		.BYTE	[[500000 >> 16] & $FF]
		.BYTE	[[150000 >> 16] & $FF]
		.BYTE	[[120000 >> 16] & $FF]
		.BYTE	[[250000 >> 16] & $FF]
		.BYTE	[[200000 >> 16] & $FF]
		.BYTE	[[70000 >> 16] & $FF]

; =============== S U B	R O U T	I N E =======================================
_scenario_result_test:
		LDA		_cur_game_mode
		CMP		#$02
		BNE		locret_105197					; only for scenario mode
		LDA		_city._disaster_end
		CMP		_city._cur_year
		BNE		locret_105197					; test for scenario end year
		LDX		_scenario_idx
		BNE		loc_105142
		LDA		_city._city_type				; SCENARIO 0 "CISCO"
		CMP		#$04							; test if city degraded slightly
		BCC		.scenario_failed				; less than "CAPITAL"
		BCS		.scenario_won
loc_105142:
		DEX
		BNE		loc_105153
		LDA		_city._traf_lvl_avr.INT			; SCENARIO 1 "BERN"
		BNE		.scenario_failed				; traffic level should be less than 0.50fp
		LDA		_city._traf_lvl_avr.FRAC		; actually, it always IS there
		CMP		#$50
		BCS		.scenario_failed
		BCC		.scenario_won
loc_105153:
		DEX
		BNE		loc_105164
		LDA		_city._crime_lvl_avr.INT		; SCENARIO 2 "DETROIT"
		BNE		.scenario_failed
		LDA		_city._crime_lvl_avr.FRAC		; crime level need to be less than 0.3C
		CMP		#$3C
		BCS		.scenario_failed
		BCC		.scenario_won
loc_105164:
		DEX
		BNE		loc_105175
		CMPWI	_city._score,500				; SCENARIO 3 "TOKYO"
		BCC		.scenario_failed				; the overall sicy score tested above 500
		BCS		.scenario_won
loc_105175:
		DEX
		BNE		loc_105186
		CMPWI	_city._score,500				; SCENARIO 4 "BOSTON"
		BCC		.scenario_failed
		BCS		.scenario_won
loc_105186:
		DEX
		BNE		locret_105197
		CMPWI	_city._score,500				; SCENARIO 5 "RIO"
		BCC		.scenario_failed
		BCS		.scenario_won
locret_105197:
		RTS										; TODO: no more options, extra scenarios cannot be completed
.scenario_won:
		LDA		#$01
		BNE		loc_10519E
.scenario_failed:
		LDA		#$02
loc_10519E:
		STA		_scenario_result_idx			; set additional scenario result flag
		LDA		#$0B
		STA		_game_setup_sub_idx				; special case for scenario over mode
		DEC		_main_sub_idx					; exit from the game to the game setup mode
		RTS

; =============== S U B	R O U T	I N E =======================================
_practice_events_test:
		LDA		_cur_game_mode					; practice game mode
		CMP		#$03
		BNE		_exit
		LDA		_city._cur_week					; end of month
		AND		#$03
		BNE		_exit
		LDA		_city._cur_year+1				; high nibble of 190x year
		CMP		#>[1900]
		BNE		_exit
		LDA		_city._cur_month				; end of JAN
		CMP		#$01
		BNE		_exit
		LDA		_city._cur_year					; low nibble for 1900 year
		CMP		#<[1900]
		BNE		loc_1051D1
		LDX		#$1B							; first practice hint message
		BNE		.set_game_msg
loc_1051D1:
		CMP		#<[1901]						; low nibble for 1901 year
;		BNE		_exit							; BUG, prevents from result display for practice
		BNE		loc_1051F3						; FIX
		LDX		#$1C							; second practice hint message
		BNE		.set_game_msg					; unconditional branch
; BUG! never executed
; -
;		LDA		_city._cur_year					; REDUNDANT
loc_1051F3:
		CMP		#<[1905]						; test for year 1905
		BNE		_exit
		LDX		#$1D							; message for good practice result
		LDA		_popul_cur+2					; test for population level
		BNE		.set_game_msg
		CMPWI	_popul_cur,30000				; should be above 30000 to win
		INX										; if less, use fail practice message
; -
.set_game_msg:
		STX		_game_msg_idx					; trigger the special event msg
		LDA		#$01
		STA		_game_msg_awaits_flag
_exit:
		RTS

		SECTION_ENDS B03, "PRGB EVENTS AND DISASTERS SELECTORS"

; ---------------------------------------------------------------------------
; -----------------S-C-R-O-L-L--M-E-S-S-A-G-E-S--M-A-N-A-G-R-----------------
; ---------------------------------------------------------------------------

		SECTION_START B04

; =============== S U B	R O U T	I N E =======================================
; scroll messages slector
; always select random starting point, then test all handlers
; RND -> F, 0 -> RND-1
;
_scroll_msg_selector:
		LDA		#$10							; get RND in range 00-0F
		JSR		_rand_clamp_A
		STA		_tmpEB
		TAY										; execute handler with idx starting from RNG value
loc_105204:
		STY		_tmpEA							; Y are passed to handlers as index of current item
		JSR		_scroll_msg_test
		LDY		_tmpEA							; keep cur loop counter
		INY
		CPY		#$10							; to 0F (max)
		BNE		loc_105212
		LDY		#$00							; wrap to 0
loc_105212:
		CPY		_tmpEB							; test to RNG-1
		BNE		loc_105204
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg_test:
		LDA		_scroll_msg_test_lib_lo,Y		; one more last fast table jump here, also because
		STA		_tmpE8							; we may damage _tmp0 with _switch
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
		.BYTE	<[nullsub_10]					; msgB blackouts unused here, in Micropolis
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
_get_RCI_counts_div4:							; OPTIMIZED, size reduced for c ommon calcs
		MOVW	_tmpE6,_total_RCI_counts		; scale counters /4
		LDA		_tmpE7
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg00_test:
		JSR		_get_RCI_counts_div4			; R areas less then 1/4 of all zones
		STA		_tmpE7
		CMPW	_tmpE6,_RCI_counts._R
		BCC		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg01_test:
		JSR		_get_RCI_counts_div4			; C areas less than 1/8 of all zones
		LSR
		ROR		_tmpE6
		STA		_tmpE7
		CMPW	_tmpE6,_RCI_counts._C
		BCC		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg02_test:
		JSR		_get_RCI_counts_div4			; the same for I area
		LSR
		ROR		_tmpE6
		STA		_tmpE7
		CMPW	_tmpE6,_RCI_counts._I
		BCC		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg03_test:
		CMPWI	_total_RCI_counts,10			; if sum of areas > 10, but
		BCC		nullsub_10						; road tiles are < 10, then
		CMPW	_roads_count,_total_RCI_counts	; trigger message
		BCS		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg04_test:
		CMPWI	_total_RCI_counts,50			; rails needed when more than 50 areas
		BCC		nullsub_10
		CMPW	_rails_count,_total_RCI_counts	; and also <50 are built
		BCS		nullsub_10
		JMP		_scroll_msg_start

; TODO: if triggers when _powered_areas/_stats._total < 0.7
; and call "BLACKOUTS" message.
;
; =============== S U B	R O U T	I N E =======================================
nullsub_10:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg05_test:
		LDA		_total_RCI_counts+1				; optimized value compare if counter is more than 256
		BNE		loc_1052EA
		LDA		_total_RCI_counts				; test if >=10 buildings already and no power stations
		CMP		#$0A							; then message about need of power stations
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
; NOTE in theoretically possible case when we have 128+128 power stations, the result will be
; zero, but C=1 then, so let's be paranoid and test this case also
		BCS		nullsub_10
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
; NOTE, this may cause funny situation, when in scenario mode and existing stadiums
; are destroyed, sity on fire, but citizens still demant a stadium!
;
_scroll_msg06_test:
		LDA		#$00
		STA		_required._stadium				; clear required flag
		CMPWI	_city._level._R,$01F4			; R_L>1.95
		BCC		nullsub_10
		LDA		_stadiums_count					; also test if no stadiums.
		BNE		nullsub_10
		INC		_required._stadium				; set stadium request, also stops R growth
		LDA		_tmpEA
		JMP		_scroll_msg_force_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg07_test:
		LDA		#$00
		STA		_required._seaport				; the same as for R
		CMPWI	_city._level._I,$0064			; I_L>0.39
		BCC		nullsub_10
		LDA		_sea_ports_count
		BNE		nullsub_10
		INC		_required._seaport				; stops I growth
		LDA		_tmpEA
		JMP		_scroll_msg_force_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg08_test:
		LDA		#$00
		STA		_required._airport
		CMPWI	_city._level._C,$0096			; C_L>0.59
		BCC		nullsub_10
		LDA		_airports_count
		BNE		nullsub_5
		INC		_required._airport				; stops C growth
		LDA		_tmpEA
		JMP		_scroll_msg_force_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg09_test:
		CMPWI	_cur_RCI_levels_sum,$003C		; R_C+I_C+C_L>0.24
		BCC		nullsub_5
		LDA		_fire_stations_count			; and no firesstations
		BNE		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0A_test:
		CMPWI	_cur_RCI_levels_sum,$003C		; R_C+I_C+C_L>0.24
		BCC		nullsub_5
		LDA		_police_stations_count			; and no police stations
		BNE		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0C_test:
		LDA		_city._tax_rate					; tax rate >=12%
		CMP		#$0C
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0D_test:
		LDA		_fund_rate_trans				; funding ~ < 69%
		CMP		#$16
		BCS		nullsub_5
		CMPWI	_roads_count,30					; roads count >=30
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0E_test:
		LDA		_fund_rate_fire					; funding ~ < 73%
		CMP		#$0B
		BCS		nullsub_5
		CMPWI	_cur_RCI_levels_sum,$0014		; R_C+I_C+C_L>=0.08
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
_scroll_msg0F_test:
		LDA		_fund_rate_police				; funding ~ < 73%
		CMP		#$0B
		BCS		nullsub_5
		CMPWI	_cur_RCI_levels_sum,$0014		; R_C+I_C+C_L>=0.08
		BCC		nullsub_5
		JMP		_scroll_msg_start

; =============== S U B	R O U T	I N E =======================================
nullsub_5:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg_start:
		PLA										; if any new scroll msg is pending while other
		PLA										; currently is displayed, then we just break
		LDA		_tmpEA							; and reattempt to display this message on the
		CMP		_scroll_msg_idx					; next loop.
		BEQ		locret_105124

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
;
_scroll_msg_force_start:
		STA		_scroll_msg_idx					; however, there are some events that can
		LDY		#$00							; break current scroll message display
		STY		_scroll_msg_start_pos
		DEY										; theoretically this may lead to garbage data
		STY		_scroll_msg_active_flag			; of the previous scroller on the screen
locret_105124:									; but due to long duration period between tests
		RTS										; this won't happen most of the time

		SECTION_ENDS B04, "PRGB SCROLL MESSAGES SELECTOR"

		BANK_END FA,$C000
