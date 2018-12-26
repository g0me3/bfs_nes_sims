
		ORG $A000

		BANK_START $FD

; =============== S U B	R O U T	I N E =======================================
sub_150124:
		LDA		byte_71
		BEQ		loc_15012B
		JMP		loc_1501C5
loc_15012B:
		LDA		_cur_map_wnd_pos._COL
		SEC
		SBC		#$07
		STA		_tmp_tile_pos._COL
		LDA		_cur_map_wnd_pos._ROW
		CLC
		ADC		_anim_pos_col
		STA		_tmp_tile_pos._ROW
		JSR		sub_150262
		LDA		_cur_screen_tile._COL
		LSR
		STA		_anim_pos_row
		CIADDB	byte_75,_cur_screen_tile._ROW,byte_73,NONE,#$1E
		JSR		_buf_pos_calc_74_75_77
		LDA		_cur_screen_tile._COL
		LSR
		EOR		#$FF
		SEC
		ADC		#$20
		AND		#$1F
		STA		byte_76
		PPUQSTART
		PPUQMOVB	byte_77,OP_AND,#$E0
		PPUQMOVB	byte_78,OP_NOP,0
		PPUQMOVB	#$20,OP_NOP,0
		STA		_ptr0
		LDY		byte_76
loc_150178:
		LDA		_pre_ppu_buf,Y
		PPUQMOVA
		INY
		DEC		_ptr0
		BEQ		loc_15018C
		CPY		#$20
		BCC		loc_150178
		LDY		#$00
		BEQ		loc_150178
loc_15018C:
		TXA
		PHA
		EXTQSTART
		EXTQMOVB	byte_77,OP_AND,#$E0
		EXTQMOVB	byte_78,OP_ADD,#$3C
		EXTQMOVB	#$20,OP_NOP,0
		STA		_ptr0
		LDY		byte_76
loc_1501AB:
		LDA		_pre_ppu_buf+$20,Y
		EXTQMOVA
		INY
		DEC		_ptr0
		BEQ		loc_1501BF
		CPY		#$20
		BCC		loc_1501AB
		LDY		#$00
		BEQ		loc_1501AB
loc_1501BF:
		POPB	_ppu_queue_pos_head
		EXTQEND
		RTS
loc_1501C5:
		LDA		_cur_map_wnd_pos._COL
		CLC
		ADC		_anim_pos_col
		STA		_tmp_tile_pos._COL
		LDA		_cur_map_wnd_pos._ROW
		SEC
		SBC		#$07
		STA		_tmp_tile_pos._ROW
		JSR		sub_150282
		CIADDB	_anim_pos_row,_cur_screen_tile._COL,byte_73,NONE,#$20
		LDA		#$00
		STA		byte_75
		JSR		_buf_pos_calc_74_75_77
		LDA		_cur_screen_tile._ROW
		LSR
		STA		_ptr0
		LDA		#$07
		SEC
		SBC		_ptr0
		BCS		loc_1501F9
		ADC		#$1E
loc_1501F9:
		STA		byte_76
		PPUQSTART
		PPUQMOVB	byte_77,OP_NOP,0
		PPUQMOVB	byte_78,OP_NOP,0
		LDA		#$1E
		STA		_ptr0
		ORA		#$80
		PPUQMOVA
		LDY		byte_76
loc_150215:
		LDA		_pre_ppu_buf,Y
		PPUQMOVA
		INY
		DEC		_ptr0
		BEQ		loc_150229
		CPY		#$1E
		BCC		loc_150215
		LDY		#$00
		BEQ		loc_150215
loc_150229:
		TXA
		PHA
		EXTQSTART
		EXTQMOVB	byte_77,OP_NOP,0
		EXTQMOVB	byte_78,OP_ADD,#$3C
		LDA		#$1E
		STA		_ptr0
		ORA		#$80
		EXTQMOVA
		LDY		byte_76
loc_150248:
		LDA		_pre_ppu_buf+$20,Y
		EXTQMOVA
		INY
		DEC		_ptr0
		BEQ		loc_15025C
		CPY		#$1E
		BCC		loc_150248
		LDY		#$00
		BEQ		loc_150248
loc_15025C:
		POPB	_ppu_queue_pos_head
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_150262:
		LDA		#$20
		STA		byte_79
		LDX		#$00
		SEC
loc_150269:
		TXA
		PHA
		JSR		sub_137E6E
		PLA
		TAX
		INC		_tmp_tile_pos._COL
		LDA		word_6C
		STA		_pre_ppu_buf,X
		LDA		word_6C+1
		STA		_pre_ppu_buf+$20,X
		INX
		DEC		byte_79
		BNE		loc_150269
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_150282:
		LDA		#$1E
		STA		byte_79
		LDX		#$00
loc_150288:
		TXA
		PHA
		SEC
		JSR		sub_137E6E
		PLA
		TAX
		INC		_tmp_tile_pos._ROW
		LDA		word_6C
		STA		_pre_ppu_buf,X
		LDA		word_6C+1
		STA		_pre_ppu_buf+$20,X
		INX
		DEC		byte_79
		BNE		loc_150288
		RTS

; =============== S U B	R O U T	I N E =======================================
_buf_pos_calc_74_75_77:
		BUFPOS	byte_77,_anim_pos_row,byte_75,#$20,#$20	; DST=(byte_75*#$20+_anim_pos_row)+#$2000
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_cursor_input_control:
		LDA		_scr_mode_idx
		BEQ		loc_1502C4
		JMP		loc_150306
loc_1502C4:
		LDA		_pad0_held
		AND		#$0F
		BNE		loc_1502CB
		RTS
loc_1502CB:
		LDA		_game_msg_wnd_active_flag
		BNE		loc_1502D4
		LDA		_cur_game_cursor_area
		BNE		loc_15030D
loc_1502D4:
		LDA		_pad0_held
		AND		#$03
		BEQ		loc_1502EC
		LSR
		LDA		_cur_game_cursor_scr_pos._COL
		BCC		loc_1502E7
		CLC
		ADC		#$02
		STA		_cur_game_cursor_scr_pos._COL
		JMP		loc_1502EC
loc_1502E7:
		SEC
		SBC		#$02
		STA		_cur_game_cursor_scr_pos._COL
loc_1502EC:
		LDA		_pad0_held
		AND		#$0C
		BEQ		loc_150306
		LSR
		LSR
		LSR
		LDA		_cur_game_cursor_scr_pos._ROW
		BCC		loc_150301
		CLC
		ADC		#$02
		STA		_cur_game_cursor_scr_pos._ROW
		JMP		loc_150306
loc_150301:
		SEC
		SBC		#$02
		STA		_cur_game_cursor_scr_pos._ROW
loc_150306:
		JSR		sub_15035F
		JMP		sub_150383

loc_15030D:
		LDA		#$00
		STA		_game_map_area_cursor_pos_backup._COL
		LDA		_pad0_autorep_release
		AND		#$03
		BEQ		loc_15032D
		LSR
		BCC		loc_150324
		LDA		_cur_game_cursor_scr_pos._COL
		CLC
		ADC		#$08
		STA		_cur_game_cursor_scr_pos._COL
		BNE		loc_15032D
loc_150324:
		LDA		_cur_game_cursor_scr_pos._COL
		SEC
		SBC		#$08
		STA		_cur_game_cursor_scr_pos._COL
		BNE		loc_15032D
loc_15032D:
		LDA		_pad0_autorep_release
		AND		#$0C
		BEQ		loc_15034A
		LSR
		LSR
		LSR
		BCC		loc_150341
		LDA		_cur_game_cursor_scr_pos._ROW
		CLC
		ADC		#$08
		STA		_cur_game_cursor_scr_pos._ROW
		BNE		loc_15034A
loc_150341:
		LDA		_cur_game_cursor_scr_pos._ROW
		SEC
		SBC		#$08
		STA		_cur_game_cursor_scr_pos._ROW
		BNE		loc_15034A
loc_15034A:
		JSR		sub_15035F
		JMP		sub_150383

_building_sizes_list:
		.BYTE	$01,$01,$01,$01,$01,$02,$02,$02,$02,$02,$03,$03,$03,$03

; =============== S U B	R O U T	I N E =======================================
sub_15035F:
		LDA		_cur_game_cursor_scr_pos._COL
		CMP		#$38
		BCC		loc_15037E
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$38
		BCC		loc_15037E
		LDA		_cur_game_cursor_scr_pos._COL
		LSR
		LSR
		LSR
		STA		_constr_cursor_tile._COL
		LDA		_cur_game_cursor_scr_pos._ROW
		LSR
		LSR
		LSR
		STA		_constr_cursor_tile._ROW
		LDA		#$01
		STA		_cur_game_cursor_area
		RTS
loc_15037E:
		LDA		#$00
		STA		_cur_game_cursor_area
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_150383:
		LDA		_cur_game_cursor_area
		BEQ		loc_1503D2
		LDY		_const_palette_selected_idx
		LDX		_building_sizes_list,Y
		CPY		#$0C
		BNE		loc_150395
		LDY		_cur_port_type_select_idx
		BEQ		loc_150395
		INX
loc_150395:
		STX		_cur_selected_buildings_size
		TXA
		ASL
		ASL
		ASL
		STA		_ptr0
		LDA		#$F0
		SEC
		SBC		_ptr0
		CMP		_cur_game_cursor_scr_pos._COL
		BCS		loc_1503AD
		LDA		#$F0
		SEC
		SBC		_ptr0
		STA		_cur_game_cursor_scr_pos._COL
loc_1503AD:
		LDA		#$E0
		SEC
		SBC		_ptr0
		CMP		_cur_game_cursor_scr_pos._ROW
		BCS		loc_1503BD
		LDA		#$E0
		SEC
		SBC		_ptr0
		STA		_cur_game_cursor_scr_pos._ROW
loc_1503BD:
		LDA		_cur_game_cursor_scr_pos._COL
		LSR
		LSR
		LSR
		SEC
		SBC		#$07
		STA		_constr_cursor_tile._COL
		LDA		_cur_game_cursor_scr_pos._ROW
		LSR
		LSR
		LSR
		SEC
		SBC		#$07
		STA		_constr_cursor_tile._ROW
		RTS
loc_1503D2:
		LDA		_cur_game_cursor_scr_pos._COL
		CMP		#$10
		BCS		loc_1503DE
		LDA		#$10
		STA		_cur_game_cursor_scr_pos._COL
		BNE		loc_1503E6
loc_1503DE:
		CMP		#$E9
		BCC		loc_1503E6
		LDA		#$E8
		STA		_cur_game_cursor_scr_pos._COL
loc_1503E6:
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$10
		BCS		loc_1503F2
		LDA		#$10
		STA		_cur_game_cursor_scr_pos._ROW
		BNE		loc_1503E6
loc_1503F2:
		CMP		#$D9
		BCC		locret_1503FA
		LDA		#$D8
		STA		_cur_game_cursor_scr_pos._ROW
locret_1503FA:
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_push_area_test:
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$38
		BCS		loc_150425
		CMP		#$28
		BCC		loc_150450
		LDA		_cur_game_cursor_scr_pos._COL
		CMP		#$98
		BCS		loc_150450
		CMP		#$10
		BCC		loc_150450
		SBC		#$10
		LSR
		LSR
		LSR
		LSR
		CMP		#$06
		BNE		loc_15041B
		LDA		#$05
loc_15041B:
		CMP		#$07
		BCC		loc_150421
		LDA		#$06
loc_150421:
		STA		_gui_ctrl_menu_press_idx
		CLC
		RTS
loc_150425:
		LDA		_cur_game_cursor_scr_pos._COL
		CMP		#$30
		BCS		loc_150450
		CMP		#$10
		BCC		loc_150450
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$B0
		BCS		loc_150450
		SEC
		SBC		#$40
		BCC		loc_150450
		LSR
		LSR
		LSR
		AND		#$FE
		STA		_ptr0
		LDA		_cur_game_cursor_scr_pos._COL
		SEC
		SBC		#$10
		LSR
		LSR
		LSR
		LSR
		ORA		_ptr0
		STA		_const_palette_press_idx
		CLC
		RTS
loc_150450:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_bank_build_year_init:
		LDA		_const_palette_selected_idx
		CMP		#$0D
		BNE		locret_150474
		LDX		_cur_present_select_idx
		LDA		_city._presents_list,X
		CMP		#$0F
		BNE		locret_150474
		LDA		_city._cur_year
		CLC
		ADC		#$05
		STA		_city._Year_to_build_bank
		LDX		_city._cur_year+1
		BCC		loc_150471
		INX
loc_150471:
		STX		_city._Year_to_build_bank+1
locret_150474:
		RTS

; =============== S U B	R O U T	I N E =======================================
_present_remove_from_list:
		LDA		_const_palette_selected_idx
		CMP		#$0D
		BEQ		loc_15047C
		RTS
loc_15047C:
		LDX		_cur_present_select_idx
		LDA		#$00
		STA		_city._presents_list,X
loc_150484:
		INX
		CPX		#$04
		BCS		loc_150492
		LDA		_city._presents_list,X
		STA		_city._presents_list-1,X
		JMP		loc_150484
loc_150492:
		LDA		#$00
		STA		_city._presents_list+3
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_cur_building_price:
		LDA		_const_palette_selected_idx		; power station choosen?
		CMP		#$0B
		BNE		loc_1504A6
		LDX		_cur_power_type_select_idx		; test additionally for its type
		BEQ		loc_1504B2
		LDA		#$0E
		BNE		loc_1504BF						; nuke has index E in prices list
loc_1504A6:
		CMP		#$0C
		BNE		loc_1504B2						; the same for airports
		LDX		_cur_port_type_select_idx
		BEQ		loc_1504B2
		LDA		#$0F							; extra port has index F
;		BNE		loc_1504BF
loc_1504B2:
;		CMP		#$0D							; REDUNDANT
;		BNE		loc_1504BF						; special prizes list seems planned to
;		LDY		_cur_present_select_idx			; have different prices, but currently
;		LDX		_city._presents_list,Y			; exactly the same 100
;		LDA		_presents_cost_idx_list,X
loc_1504BF:
		ASL										; fetch price
		TAX
		LDA		_buildings_prices_list,X
		STA		_cur_building_price
		INX
		LDA		_buildings_prices_list,X
		STA		_cur_building_price+1
		RTS

; REDUNDANT
;_presents_cost_idx_list:
;		.BYTE	$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D

; =============== S U B	R O U T	I N E =======================================
; no input, calc from cur construction palette button and selected buildings
; type current position of the construction cursor on the map, take the
; "sample" at this location, as well as loads the current building scheme list
;
; output: _tmp_ptrC1 - cur map ptr
;		  _cur_build_sample_tile_idx    - cur map tile sample
;
_get_cur_constr_cursor_map_tile:
		JSR		_get_cur_constr_cursor_map_ofs
		JSR		_get_cur_building_scheme_ptr
		LDY		#$00
		LDA		(_tmp_ptrC1),Y
		STA		_cur_build_sample_tile_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_build_cur_constr_cursor_area:
		JSR		_sram_on
		LDA		_cur_build_store_tile_idx		; previously calcularet duild tile idx
		CMP		#$FF
		BNE		.instant_build_single_tile		; if !=FF, then custom 1x1 tile here
		JSR		_get_build_construc_data_ptr
		LDY		#$00
loc_150518:
		LDA		(_tmp_ptrC3),Y					; read building data tiles from lib
		BMI		loc_150531
		LDX		_const_palette_selected_idx		; top-left corner of the building starts here
		CPX		#$0B
		BNE		loc_150527						; additional test for power station type
		CLC
		ADC		_cur_power_type_select_idx
		BNE		loc_150531
loc_150527:
		CPX		#$0C
		BNE		loc_150531						; additional test for airport
		LDX		_cur_port_type_select_idx
		BEQ		loc_150531
		LDA		#_MAP_AIR_PORT
loc_150531:
		CMP		#$DF
		BEQ		loc_150545						; test if building data is over
		STY		_ptr0
		STA		_ptr0+1
		LDA		(_tmp_ptrC5),Y					; read building scheme
		TAY
		LDA		_ptr0+1
		STA		(_tmp_ptrC1),Y					; put byte into the map
		LDY		_ptr0
		INY
		BNE		loc_150518						; repeat
loc_150545:
		JMP		_sram_off
.instant_build_single_tile:
		LDY		#$00
		STA		(_tmp_ptrC1),Y
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_adjust_surround_net_tilesCC:
		JSR		_sram_on
		LDX		byte_CC
		JMP		_adjust_surround_net_tiles_ex

; =============== S U B	R O U T	I N E =======================================
_adjust_surround_net_tiles:
		JSR		_sram_on
		LDX		_cur_selected_buildings_size

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_adjust_surround_net_tiles_ex:
		DEX
		LDA		_surround_tiles_lib_lo,X
		STA		word_D0
		LDA		_surround_tiles_lib_hi,X
		STA		word_D0+1
		LDY		#$00
loc_15056C:
		LDA		(word_D0),Y
		CMP		#$80
		BEQ		loc_150581
		JSR		_get_building_tile_map_ptr
		BCS		loc_15057E
		STY		byte_D4
		JSR		_adjust_conjunction_tiles
		LDY		byte_D4
loc_15057E:
		INY
		BNE		loc_15056C
loc_150581:
		JMP		_sram_off

_surround_tiles_lib_lo:
		.BYTE	<[_surround_tiles0_list]
		.BYTE	<[_surround_tiles1_list]
		.BYTE	<[_surround_tiles2_list]
		.BYTE	<[_surround_tiles3_list]
_surround_tiles_lib_hi:
		.BYTE	>[_surround_tiles0_list]
		.BYTE	>[_surround_tiles1_list]
		.BYTE	>[_surround_tiles2_list]
		.BYTE	>[_surround_tiles3_list]

; surrounding tiles 1x1 building (2=*)
; -0-
; 123
; -4-
;
_surround_tiles0_list:
		.BYTE	$00,$FF
		.BYTE	$FF,$00
		.BYTE	$00,$00
		.BYTE	$01,$00
		.BYTE	$00,$01
		.BYTE	$80

; surrounding tiles 2x2 building
; -01-
; 2*-3
; 4--5
; -67-
;
_surround_tiles1_list:
		.BYTE	$00,$FF
		.BYTE	$01,$FF
		.BYTE	$FF,$00
		.BYTE	$02,$00
		.BYTE	$FF,$01
		.BYTE	$02,$01
		.BYTE	$00,$02
		.BYTE	$01,$02
		.BYTE	$80

; surrounding tiles 3x3 building
; -013-
; 4*--5
; 6---7
; 8---9
; -ABC-
;
_surround_tiles2_list:
		.BYTE	$00,$FF
		.BYTE	$01,$FF
		.BYTE	$02,$FF
		.BYTE	$FF,$00
		.BYTE	$03,$00
		.BYTE	$FF,$01
		.BYTE	$03,$01
		.BYTE	$FF,$02
		.BYTE	$03,$02
		.BYTE	$00,$03
		.BYTE	$01,$03
		.BYTE	$02,$03
		.BYTE	$80

; surrounding tiles 3x3 building
; -0123-
; 4*---5
; 6----7
; 8----9
; A----B
; -CDEF-
;
_surround_tiles3_list:
		.BYTE	$00,$FF
		.BYTE	$01,$FF
		.BYTE	$02,$FF
		.BYTE	$03,$FF
		.BYTE	$FF,$00
		.BYTE	$04,$00
		.BYTE	$FF,$01
		.BYTE	$04,$01
		.BYTE	$FF,$02
		.BYTE	$04,$02
		.BYTE	$FF,$03
		.BYTE	$04,$03
		.BYTE	$00,$04
		.BYTE	$01,$04
		.BYTE	$02,$04
		.BYTE	$03,$04
		.BYTE	$80

; =============== S U B	R O U T	I N E =======================================
; looks like too much bounds checking with no actual reason..
; when the building is build at the map side, the first two test
; are enough to check if it out of right/bottom bounds,
; and test 2 is enough to test left/top bounds
;
; return C=0 if ptr calculated,
; 		 C=1 out of bounds
;		word_D2 - ptr to map tile
;
_get_building_tile_map_ptr:
		INY
		CLC
		ADC		_cur_constr_cursor_map_pos._COL
		STA		byte_511
		CMP		#$4C
		BCS		.out_of_bounds					; out of bounds test 0
		LDA		(word_D0),Y
		CLC
		ADC		_cur_constr_cursor_map_pos._ROW
		STA		byte_512
		CMP		#$4C
		BCS		.out_of_bounds					; out of bounds test 1
		LDX		#$4C
		MULXA
		CLC
		ADC		byte_511
		STA		word_D2
		TXA
		ADC		#$00
		STA		word_D2+1
		LDA		word_D2
		CLC
		ADC		#<[_wram_map_buf]
		STA		word_D2
		LDA		word_D2+1
		ADC		#>[_wram_map_buf]
		STA		word_D2+1
		CMP		#>[_wram_map_buf]				; OUT UF BOUNDS TEST 2!!
		BCC		.out_of_bounds
		CMP		#>[_power_grid_map_buf]			; out of bounds test 3
		BCC		.calc_ok
		BNE		.out_of_bounds
		LDA		word_D2
		CMP		#<[_power_grid_map_buf]			; out of bounds test 4
		BCS		.out_of_bounds
.calc_ok:
		CLC
		RTS
.out_of_bounds:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_adjust_conjunction_tiles:
		LDY		#$00
		STY		byte_D5
		LDA		(word_D2),Y						; get surrounding map tile
		CMP		#_MAP_ROAD_LR					; all you need is electro/transport tile here
		BCC		locret_150644
		CMP		#_MAP_GRASS
		BCC		loc_150645
locret_150644:
		RTS
loc_150645:
		CMP		#_MAP_RAIL_LR					; ok, check what exactly tile is here
		BCS		loc_15065C
		JSR		_test_conjunction_road_tiles	; test for more roads near surrounding tile
		LDA		#_MAP_ROAD_LR					; OPTIMIZED
		BNE		loc_150674						; -
;		JSR		_clamp_conjunction_flags		; REDUNDANT
;		LDX		byte_D5
;		LDA		_tile_conjunctions_table,X		; calc new bend of the road
;		CLC
;		ADC		#_MAP_ROAD_LR
;		LDY		#$00
;		STA		(word_D2),Y						; store current surrounding tile
;		RTS
loc_15065C:
		CMP		#_MAP_ELECTRO_LR
		BCS		loc_150673
		JSR		_test_conjunction_rail_tiles	; rails here
		LDA		#_MAP_RAIL_LR					; OPTIMIZED
		BNE		loc_150674						; -
;		JSR		_clamp_conjunction_flags		; REDUNDANT
;		LDX		byte_D5
;		LDA		_tile_conjunctions_table,X
;		CLC
;		ADC		#_MAP_RAIL_LR
;		LDY		#$00
;		STA		(word_D2),Y
;		RTS
loc_150673:
		JSR		_test_conjunction_electro_tiles	; electro here
		LDA		#_MAP_ELECTRO_LR
loc_150674:
		PHA										; OPTIMIZED
		JSR		_clamp_conjunction_flags
		PLA
		LDX		byte_D5
		CLC
		ADC		_tile_conjunctions_table,X
		LDY		#$00
		STA		(word_D2),Y
		RTS
;		JSR		_clamp_conjunction_flags		; REDUNDANT
;		LDX		byte_D5
;		LDA		_tile_conjunctions_table,X
;		CLC
;		ADC		#_MAP_ELECTRO_LR
;		LDY		#$00
;		STA		(word_D2),Y
;		RTS

_tile_conjunctions_table:
		.BYTE	$00,$00,$00,$00,$01,$02,$03,$08,$01,$04,$05,$09,$01,$06,$07,$0A

; =============== S U B	R O U T	I N E =======================================
_clamp_conjunction_flags:
		LDA		byte_D5
		LDX		byte_511
		BEQ		loc_1506AA						; test for side cases
		CPX		#$4B
		BEQ		loc_1506A5
		BCS		loc_1506AA
		BCC		loc_1506AC
loc_1506A5:
		AND		#$0E							; remove right side flag
		JMP		loc_1506AC
loc_1506AA:
		AND		#$0D							; remove left side flag
loc_1506AC:
		LDX		byte_512
		BNE		loc_1506B3						; the same for top side
		AND		#$07
loc_1506B3:
		CPX		#$4B
		BCC		loc_1506B9						; and bottom sides
		AND		#$0B
loc_1506B9:
		STA		byte_D5
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_conjunction_road_tiles:
		JSR		_get_tile_above
		JSR		_is_tile_a_road
		BCS		loc_1506D4						; yes if C=1
		JSR		_is_tile_a_road_cross_ud		; OPTIMIZED
; REDUNDANT
;		CMP		#_MAP_ROAD_RAIL_UD				; test for additional UD tiles
;		BEQ		loc_1506D3						;
;		CMP		#_MAP_ELECTRO_ROAD_LR
;		BEQ		loc_1506D3
;		CMP		#_MAP_WATER_ROAD_UD
;		BEQ		loc_1506D3
;		CLC
;		BCC		loc_1506D4
;loc_1506D3:
;		SEC
loc_1506D4:
		ROL		byte_D5							; memorize the result
		JSR		_get_tile_below
		JSR		_is_tile_a_road
		BCS		loc_1506EE						; once again, may be optimized
		JSR		_is_tile_a_road_cross_ud		; OPTIMIZSED
;		CMP		#_MAP_ROAD_RAIL_UD				; REDUNDANT
;		BEQ		loc_1506ED
;		CMP		#_MAP_ELECTRO_ROAD_LR
;		BEQ		loc_1506ED
;		CMP		#_MAP_WATER_ROAD_UD
;		BEQ		loc_1506ED
;		CLC
;		BCC		loc_1506EE
;loc_1506ED:
;		SEC
loc_1506EE:
		ROL		byte_D5							; more results
		JSR		_get_tile_leftmost
		JSR		_is_tile_a_road
		BCS		loc_150708
		JSR		_is_tile_a_road_cross_lr		; OPTIMIZED
;		CMP		#_MAP_ROAD_RAIL_LR				; REDUNDANT
;		BEQ		loc_150707
;		CMP		#_MAP_ELECTRO_ROAD_UD
;		BEQ		loc_150707
;		CMP		#_MAP_WATER_ROAD_LR
;		BEQ		loc_150707
;		CLC
;		BCC		loc_150708
;loc_150707:
;		SEC
loc_150708:
		ROL		byte_D5
		JSR		_get_tile_rightmost
		JSR		_is_tile_a_road
		BCS		loc_150722
		JSR		_is_tile_a_road_cross_lr		; OPTIMIZED
;		CMP		#_MAP_ROAD_RAIL_LR				; REDUNDANT
;		BEQ		loc_150721
;		CMP		#_MAP_ELECTRO_ROAD_UD
;		BEQ		loc_150721
;		CMP		#_MAP_WATER_ROAD_LR
;		BEQ		loc_150721
;		CLC
;		BCC		loc_150722
;loc_150721:
;		SEC
loc_150722:
		ROL		byte_D5
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_road_cross_ud:
		CMP		#_MAP_ROAD_RAIL_UD
		BEQ		loc_1506ED
		CMP		#_MAP_ELECTRO_ROAD_LR
		BEQ		loc_1506ED
		CMP		#_MAP_WATER_ROAD_UD
		BEQ		loc_1506ED
		CLC
		RTS
loc_1506ED:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_road_cross_lr:
		CMP		#_MAP_ROAD_RAIL_LR
		BEQ		loc_1506ED
		CMP		#_MAP_ELECTRO_ROAD_UD
		BEQ		loc_1506ED
		CMP		#_MAP_WATER_ROAD_LR
		BEQ		loc_1506ED
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_conjunction_rail_tiles:
		MOVW	_tmp6,word_D2
		JSR		_get_tile_above
		JSR		_is_tile_a_rail
		BCS		loc_150745
		JSR		_is_tile_a_rail_cross_ud		; OPTIMIZED
;		CMP		#_MAP_ROAD_RAIL_LR				; REDUNDANT
;		BEQ		loc_150744
;		CMP		#_MAP_ELECTRO_RAIL_LR
;		BEQ		loc_150744
;		CMP		#_MAP_WATER_RAIL_UD
;		BEQ		loc_150744
;		CLC
;		BCC		loc_150745
;loc_150744:
;		SEC
loc_150745:
		ROL		byte_D5
		MOVW	_tmp6,word_D2
		JSR		_get_tile_below
		JSR		_is_tile_a_rail
		BCS		loc_150767
		JSR		_is_tile_a_rail_cross_ud		; OPTIMIZED
;		CMP		#_MAP_ROAD_RAIL_LR				; REDUNDANT
;		BEQ		loc_150766
;		CMP		#_MAP_ELECTRO_RAIL_LR
;		BEQ		loc_150766
;		CMP		#_MAP_WATER_RAIL_UD
;		BEQ		loc_150766
;		CLC
;		BCC		loc_150767
;loc_150766:
;		SEC
loc_150767:
		ROL		byte_D5
		MOVW	_tmp6,word_D2
		JSR		_get_tile_leftmost
		JSR		_is_tile_a_rail
		BCS		loc_150789
		JSR		_is_tile_a_rail_cross_lr		; OPTIMIZED
;		CMP		#_MAP_ROAD_RAIL_UD				; REDUNDANT
;		BEQ		loc_150788
;		CMP		#_MAP_ELECTRO_RAIL_UD
;		BEQ		loc_150788
;		CMP		#_MAP_WATER_RAIL_LR
;		BEQ		loc_150788
;		CLC
;		BCC		loc_150789
;loc_150788:
;		SEC
loc_150789:
		ROL		byte_D5
		MOVW	_tmp6,word_D2
		JSR		_get_tile_rightmost
		JSR		_is_tile_a_rail
		BCS		loc_1507AB
		JSR		_is_tile_a_rail_cross_lr		; OPTIMIZED
;		CMP		#_MAP_ROAD_RAIL_UD				; REDUNDANT
;		BEQ		loc_1507AA
;		CMP		#_MAP_ELECTRO_RAIL_UD
;		BEQ		loc_1507AA
;		CMP		#_MAP_WATER_RAIL_LR
;		BEQ		loc_1507AA
;		CLC
;		BCC		loc_1507AB
;loc_1507AA:
;		SEC
loc_1507AB:
		ROL		byte_D5
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_rail_cross_ud:
		CMP		#_MAP_ROAD_RAIL_LR
		BEQ		loc_150744
		CMP		#_MAP_ELECTRO_RAIL_LR
		BEQ		loc_150744
		CMP		#_MAP_WATER_RAIL_UD
		BEQ		loc_150744
		CLC
		RTS
loc_150744:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_rail_cross_lr:
		CMP		#_MAP_ROAD_RAIL_UD
		BEQ		loc_150744
		CMP		#_MAP_ELECTRO_RAIL_UD
		BEQ		loc_150744
		CMP		#_MAP_WATER_RAIL_LR
		BEQ		loc_150744
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_conjunction_electro_tiles:
		JSR		_get_tile_above
		JSR		_is_tile_a_electro
		BCS		loc_1507C6
		JSR		_is_tile_a_electro_cross_ud		; OPTIMIZED
;		CMP		#_MAP_ELECTRO_ROAD_UD			; REDUNDANT
;		BEQ		loc_1507C5
;		CMP		#_MAP_ELECTRO_RAIL_UD
;		BEQ		loc_1507C5
;		CMP		#_MAP_WATER_ELECTRIC_UD
;		BEQ		loc_1507C5
;		CLC
;		BCC		loc_1507C6
;loc_1507C5:
;		SEC
loc_1507C6:
		ROL		byte_D5
		JSR		_get_tile_below
		JSR		_is_tile_a_electro
		BCS		loc_1507E0
		JSR		_is_tile_a_electro_cross_ud		; OPTIMIZED
;		CMP		#_MAP_ELECTRO_ROAD_UD			; REDUNDANT
;		BEQ		loc_1507DF
;		CMP		#_MAP_ELECTRO_RAIL_UD
;		BEQ		loc_1507DF
;		CMP		#_MAP_WATER_ELECTRIC_UD
;		BEQ		loc_1507DF
;		CLC
;		BCC		loc_1507E0
;loc_1507DF:
;		SEC
loc_1507E0:
		ROL		byte_D5
		JSR		_get_tile_leftmost
		JSR		_is_tile_a_electro
		BCS		loc_1507FA
		JSR		_is_tile_a_electro_cross_lr		; OPTIMIZED
;		CMP		#_MAP_ELECTRO_ROAD_LR			; REDUNDANT
;		BEQ		loc_1507F9
;		CMP		#_MAP_ELECTRO_RAIL_LR
;		BEQ		loc_1507F9
;		CMP		#_MAP_WATER_ELECTRIC_LR
;		BEQ		loc_1507F9
;		CLC
;		BCC		loc_1507FA
;loc_1507F9:
;		SEC
loc_1507FA:
		ROL		byte_D5
		JSR		_get_tile_rightmost
		JSR		_is_tile_a_electro
		BCS		loc_150814
		JSR		_is_tile_a_electro_cross_lr		; OPTIMIZED
;		CMP		#_MAP_ELECTRO_ROAD_LR			; OPTIMIZED
;		BEQ		loc_150813
;		CMP		#_MAP_ELECTRO_RAIL_LR
;		BEQ		loc_150813
;		CMP		#_MAP_WATER_ELECTRIC_LR
;		BEQ		loc_150813
;		CLC
;		BCC		loc_150814
;loc_150813:
;		SEC
loc_150814:
		ROL		byte_D5
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_electro_cross_ud:
		CMP		#_MAP_ELECTRO_ROAD_UD
		BEQ		loc_1507C5
		CMP		#_MAP_ELECTRO_RAIL_UD
		BEQ		loc_1507C5
		CMP		#_MAP_WATER_ELECTRIC_UD
		BEQ		loc_1507C5
		CLC
		RTS
loc_1507C5:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_electro_cross_lr:
		CMP		#_MAP_ELECTRO_ROAD_LR
		BEQ		loc_1507C5
		CMP		#_MAP_ELECTRO_RAIL_LR
		BEQ		loc_1507C5
		CMP		#_MAP_WATER_ELECTRIC_LR
		BEQ		loc_1507C5
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_tile_above:
		DEC		_tmp6+1
		DEC		word_D2+1						; test one tile above surronding ones
		LDA		word_D2+1
		CMP		#>[_wram_map_buf]
		BCC		loc_150828						; rough out of bounds test again
loc_150821:
		LDY		#$B4
		LDA		(word_D2),Y						; read it
		INC		word_D2+1
		RTS
loc_150828:
		LDA		word_D2							; precise out of bounds test
		CMP		#$4C
		BCS		loc_150821
		LDA		#$FF
		INC		word_D2+1						; read nothing, return
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_tile_leftmost:
		DEC		_tmp6+1
		DEC		word_D2+1
		LDA		word_D2+1
		CMP		#>[_wram_map_buf]
		BCC		loc_150844
loc_15083D:
		LDY		#$FF
		LDA		(word_D2),Y
		INC		word_D2+1
		RTS
loc_150844:
		LDA		word_D2
		BNE		loc_15083D
		LDA		#$FF
		INC		word_D2+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_tile_rightmost:
		LDY		#$01
		LDA		(word_D2),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_tile_below:
		LDY		#$4C
		LDA		(word_D2),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_road:
		CMP		#_MAP_ROAD_LR
		BCC		loc_150861
		CMP		#_MAP_RAIL_LR
		BCS		loc_150861
		SEC
		RTS
loc_150861:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_rail:
		CMP		#_MAP_TRAIN_STATION				; special case for train station
		BEQ		loc_150873
		CMP		#_MAP_INTERNAL_BUILD8			; we can be inside the building only
		BCC		loc_150877						; if we in train station...
		CMP		#_MAP_RAIL_LR
		BCC		loc_150875
		CMP		#_MAP_ELECTRO_LR
		BCS		loc_150875
loc_150873:
		SEC
		RTS
loc_150875:
		CLC
		RTS
loc_150877:
		CMP		#_MAP_INTERNAL_BUILD0			; the same calcs as for train object
		BCC		loc_150875						; for detecting it is inside train
		PHA										; station, here we treat any tile of
		AND		#$0F							; train station as a straight rail line
;		ASL										; REDUNDANT
		TAX
		LDA		_tmp6
		SEC
		SBC		_special_tiles_ofs_list,X		; REDUNDANT, the same list here used, but
		STA		_tmp6							; with 16-bit redundancy
;		INX										; REDUNDANT, always 0, simple 8bit subtract
;		LDA		_tmp6+1
;		SBC		byte_15089C,X
;		STA		_tmp6+1
		BCS		loc_150898						; OPTIMIZED
		DEC		_tmp6+1							; -
loc_150898:										; -
		LDA		(_tmp6),Y
		CMP		#_MAP_TRAIN_STATION				; test for actual building tile idx
		BNE		loc_150899
		PLA
		SEC
		RTS
loc_150899:
		PLA
		CLC
		RTS

; REDUNDANT
;byte_15089C:
;		.BYTE	$4C,$00,$4D,$00,$01,$00,$98,$00,$99,$00,$9A,$00,$4E,$00,$02,$00

; =============== S U B	R O U T	I N E =======================================
_is_tile_a_electro:
		CMP		#_MAP_GROUND0
		BCC		loc_1508B8
		CMP		#_MAP_ELECTRO_LR
		BCC		loc_1508BA
		CMP		#_MAP_GRASS
		BCS		loc_1508BA
loc_1508B8:
		SEC
		RTS
loc_1508BA:
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_bulldoze_a_building:
		JSR		_get_cur_constr_cursor_map_ofs
		JSR		_get_price_to_bulldoze_a_building	; once again we calculate price of bulldozing
		JSR		_test_valid_buildings_to_bulldoze
		BCS		locret_1508E5
		JSR		_get_building_scheme_ex
		JSR		_sram_on
		LDY		#$00
loc_1508CF:
		STY		_ptr0								; erase building by scheme for all tiles inside
		LDA		(_tmp_ptrC5),Y
		CMP		#$80
		BEQ		loc_1508E1							; end of scheme list
		TAY
		LDA		byte_CB
		STA		(_tmp_ptrC1),Y
		LDY		_ptr0
		INY
		BNE		loc_1508CF
loc_1508E1:
		JSR		_sram_off
		CLC
locret_1508E5:
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_price_to_bulldoze_a_building:
		LDA		#$00
		STA		byte_CE
		STA		byte_CF
		LDY		#$00
		LDA		(_tmp_ptrC1),Y					; read map tile sample
		BPL		.do_instant_bulldoze
		CMP		#_MAP_GROUND0
		BCS		.do_instant_bulldoze			; anything but the buildings no need to additional work here
		SEC										; get inside the building special tiles
		SBC		#_MAP_INTERNAL_BUILD0
		TAX
		LDA		_special_build_tile_delta_ROW_list,X
		STA		byte_CE							; get tile deltas
		LDA		_special_build_tile_delta_COL_list,X
		STA		byte_CF
		LDA		_special_tiles_ofs_list,X		; get tile Y idx for fetch opcode
		STA		_ptr0
		LDA		_tmp_ptrC1
		SEC
		SBC		_ptr0
		STA		_tmp_ptrC1
		LDA		_tmp_ptrC1+1
		SBC		#$00
		STA		_tmp_ptrC1+1
		LDA		(_tmp_ptrC1),Y					; read actual building tile idx
.do_instant_bulldoze:
		LDX		#$01
		STA		byte_CB
		AND		#$FF
		BMI		loc_150930
		INX
		CMP		#_MAP_FIRE_STATION+1			; case for 2x2 buildings
		BCC		loc_150930
		INX
		CMP		#_MAP_AIR_PORT					; case for 3x3 buildings
		BCC		loc_150930
		INX
		CMP		#_MAP_BANK						; test for nonbreakable buildings
		BCC		loc_150930
		DEX
loc_150930:
		STX		byte_CC							; tmp building size type idx
		DEX
		LDA		_bulldoze_building_price_list,X
		STA		_cur_building_price
		LDA		#$00
		STA		_cur_building_price+1
		RTS
; REDUNDANT
;_special_tiles_ofs_list:
;		.BYTE	$4C,$4D,$01,$98,$99,$9A,$4E,$02,$E4,$E5,$E6,$E7,$9B,$4F,$03
_special_build_tile_delta_ROW_list:
		.BYTE	$00,$01,$01,$00,$01,$02,$02,$02,$00,$01,$02,$03,$03,$03,$03
_special_build_tile_delta_COL_list:
		.BYTE	$01,$01,$00,$02,$02,$02,$01,$00,$03,$03,$03,$03,$02,$01,$00
_bulldoze_building_price_list:
		.BYTE	$01,$04,$09,$10

; =============== S U B	R O U T	I N E =======================================
_test_valid_buildings_to_bulldoze:
		LDA		_city._game_flags				; cheat for forced bulldozing a sigle tile
		AND		#$10
		BEQ		.no_bulldoze_cheat
		LDX		#_MAP_GROUND0
		BNE		.large_bulldoze_area_test		; force to bulldoze anything
.no_bulldoze_cheat:
		LDX		#_MAP_CORRUPT0					; start corruption animation with this tile
		LDA		byte_CC
		CMP		#$02
		BCS		.large_bulldoze_area_test		; for single tiled bulldoze areas
		LDA		byte_CB
		CMP		#_MAP_FLOOD
		BEQ		.cant_bulldoze_that0
		CMP		#_MAP_POLLUTION
		BEQ		.cant_bulldoze_that0
		CMP		#_MAP_FIRE
		BEQ		.cant_bulldoze_that0
		CMP		#_MAP_ROAD_DIAG0
		BCC		loc_150997
		CMP		#_MAP_ROAD_DIAGF+1
		BCC		.cant_bulldoze_that0
loc_150997:
		CMP		#_MAP_WATER_ROUTE
		BEQ		.cant_bulldoze_that1
		CMP		#_MAP_GROUND0
		BEQ		.cant_bulldoze_that1
		CMP		#_MAP_WATER
		BEQ		.cant_bulldoze_that1
		LDX		#_MAP_GROUND0
		CMP		#_MAP_WATER_ROAD_LR
		BCC		.large_bulldoze_area_test
		CMP		#_MAP_CORRUPT0
		BCC		loc_1509B5
		CMP		#_MAP_BRIDGE_WATER_LR
		BCC		.large_bulldoze_area_test
		CMP		#_MAP_ROAD_DIAG0
		BCS		.cant_bulldoze_that0
loc_1509B5:
		LDX		#_MAP_WATER
.large_bulldoze_area_test:
		STX		byte_CB
		FJSRA	_money_subtract,PRG7,PRG3
		BCS		.cant_bulldoze_that0
		RTS
.cant_bulldoze_that0:
		LDA		#$00
		STA		byte_CD
		SEC
		RTS
.cant_bulldoze_that1:
		LDA		#$FF
		STA		byte_CD
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_1509CF:
		LDA		byte_CC
		STA		_ptr0
		BNE		loc_1509DF

; =============== S U B	R O U T	I N E =======================================
_calc_constr_area_update_params:
		LDA		#$00
		STA		byte_CE
		STA		byte_CF
		LDA		_cur_selected_buildings_size
		STA		_ptr0
loc_1509DF:
		LDA		_cur_map_wnd_pos._COL
		CLC
		ADC		_constr_cursor_tile._COL
		SEC
		SBC		byte_CE
		SBC		#$01
		TAX
		BPL		loc_1509EE
		LDX		#$00
loc_1509EE:
		STX		byte_5E
		SEC
		ADC		_ptr0
		CMP		#$4C
		BCC		loc_1509F9
		LDA		#$4B
loc_1509F9:
		STA		byte_5F
		LDA		_cur_map_wnd_pos._ROW
		CLC
		ADC		_constr_cursor_tile._ROW
		SEC
		SBC		#$01
		SBC		byte_CF
		TAX
		BPL		loc_150A0A
		LDX		#$00
loc_150A0A:
		STX		byte_60
		SEC
		ADC		_ptr0
		CMP		#$4C
		BCC		loc_150A15
		LDA		#$4B
loc_150A15:
		STA		byte_61
		LDA		#$FF
		STA		byte_5D
		STA		byte_5C
		JMP		_swap_constr_area_update_params

; =============== S U B	R O U T	I N E =======================================
_get_cur_constr_cursor_map_ofs:
		LDA		_cur_map_wnd_pos._COL			; map pos + const cursor pos = map pos
		CLC
		ADC		_constr_cursor_tile._COL
		STA		_cur_constr_cursor_map_pos._COL
		LDA		_cur_map_wnd_pos._ROW
		CLC
		ADC		_constr_cursor_tile._ROW
		STA		_cur_constr_cursor_map_pos._ROW	; map pos row + map pos col * $4C = map ofs
		LDX		#$4C
		MULXA
		CLC
		ADC		_cur_constr_cursor_map_pos._COL
		STA		_tmp_ptrC1
		TXA
		ADC		#>[_wram_map_buf]				; map ofs + map buf start = map ptr
		STA		_tmp_ptrC1+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_build_construc_data_ptr:
		LDX		_const_palette_selected_idx
		CPX		#$0D
		BNE		loc_150A5C
		LDX		_cur_present_select_idx			; prize buildings idx to lib idx
		LDA		_city._presents_list,X			; (0E - 1C)
		CLC
		ADC		_const_palette_selected_idx
		TAX
		BNE		loc_150A70
loc_150A5C:
		DEX										; weird calcs...
;		CPX		#$0C							; REDUNDANT, does nothing here
		CPX		#$03
		BNE		loc_150A66						; garden constr idx to lib idx (03 or 0C)
		JSR		_build_part_tile_select
loc_150A66:
		CPX		#$0B
		BNE		loc_150A70
		LDA		_cur_port_type_select_idx		; sea/air ports to lib (0B, 0D)
		BEQ		loc_150A70
		LDX		#$0D
loc_150A70:
		LDA		_build_constuct_data_lib_lo,X	; the rest are the same (constr idx - 1)
		STA		_tmp_ptrC3
		LDA		_build_constuct_data_lib_hi,X
		STA		_tmp_ptrC3+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_build_part_tile_select:
		TXA
		PHA
		JSR		_rand
		CMP		#$20
		BCS		loc_150A88						; random, grass tile or a tree
		PLA
		LDX		#$0C
		RTS
loc_150A88:
		PLA
		TAX
		RTS

_build_constuct_data_lib_lo:
		.BYTE	<[_build_constuct_data00]
		.BYTE	<[_build_constuct_data01]
		.BYTE	<[_build_constuct_data02]
		.BYTE	<[_build_constuct_data03]
		.BYTE	<[_build_constuct_data04]
		.BYTE	<[_build_constuct_data05]
		.BYTE	<[_build_constuct_data06]
		.BYTE	<[_build_constuct_data07]
		.BYTE	<[_build_constuct_data08]
		.BYTE	<[_build_constuct_data09]
		.BYTE	<[_build_constuct_data0A]
		.BYTE	<[_build_constuct_data0B]
		.BYTE	<[_build_constuct_data0C]
		.BYTE	<[_build_constuct_data0D]
		.BYTE	<[_build_constuct_data0E]
		.BYTE	<[_build_constuct_data0F]
		.BYTE	<[_build_constuct_data10]
		.BYTE	<[_build_constuct_data11]
		.BYTE	<[_build_constuct_data12]
		.BYTE	<[_build_constuct_data13]
		.BYTE	<[_build_constuct_data14]
		.BYTE	<[_build_constuct_data15]
		.BYTE	<[_build_constuct_data16]
		.BYTE	<[_build_constuct_data17]
		.BYTE	<[_build_constuct_data18]
		.BYTE	<[_build_constuct_data19]
		.BYTE	<[_build_constuct_data1A]
		.BYTE	<[_build_constuct_data1B]
		.BYTE	<[_build_constuct_data1C]
_build_constuct_data_lib_hi:
		.BYTE	>[_build_constuct_data00]
		.BYTE	>[_build_constuct_data01]
		.BYTE	>[_build_constuct_data02]
		.BYTE	>[_build_constuct_data03]
		.BYTE	>[_build_constuct_data04]
		.BYTE	>[_build_constuct_data05]
		.BYTE	>[_build_constuct_data06]
		.BYTE	>[_build_constuct_data07]
		.BYTE	>[_build_constuct_data08]
		.BYTE	>[_build_constuct_data09]
		.BYTE	>[_build_constuct_data0A]
		.BYTE	>[_build_constuct_data0B]
		.BYTE	>[_build_constuct_data0C]
		.BYTE	>[_build_constuct_data0D]
		.BYTE	>[_build_constuct_data0E]
		.BYTE	>[_build_constuct_data0F]
		.BYTE	>[_build_constuct_data10]
		.BYTE	>[_build_constuct_data11]
		.BYTE	>[_build_constuct_data12]
		.BYTE	>[_build_constuct_data13]
		.BYTE	>[_build_constuct_data14]
		.BYTE	>[_build_constuct_data15]
		.BYTE	>[_build_constuct_data16]
		.BYTE	>[_build_constuct_data17]
		.BYTE	>[_build_constuct_data18]
		.BYTE	>[_build_constuct_data19]
		.BYTE	>[_build_constuct_data1A]
		.BYTE	>[_build_constuct_data1B]
		.BYTE	>[_build_constuct_data1C]
_build_constuct_data00:
		.BYTE	$91,$DF
_build_constuct_data01:
		.BYTE	$A7,$DF
_build_constuct_data02:
		.BYTE	$9C,$DF
_build_constuct_data03:
		.BYTE	$B2,$DF
_build_constuct_data0C:
		.BYTE	$B3,$DF
_build_constuct_data04:
		.BYTE	$00,$80,$81,$82,$DF
_build_constuct_data05:
		.BYTE	$20,$80,$81,$82,$DF
_build_constuct_data06:
		.BYTE	$40,$80,$81,$82,$DF
_build_constuct_data07:
		.BYTE	$50,$80,$81,$82,$DF
_build_constuct_data08:
		.BYTE	$51,$80,$81,$82,$DF
_build_constuct_data09:
		.BYTE	$63,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data0A:
		.BYTE	$60,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data0B:
		.BYTE	$62,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data0D:
		.BYTE	$64,$80,$81,$82,$83,$84,$85,$86,$87,$88,$89,$8A,$8B,$8C,$8D,$8E
		.BYTE	$DF
_build_constuct_data0E:
		.BYTE	$70,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data0F:
		.BYTE	$71,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data10:
		.BYTE	$72,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data11:
		.BYTE	$73,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data12:
		.BYTE	$90,$90,$90,$90,$90,$90,$90,$90,$90,$DF
_build_constuct_data13:
		.BYTE	$75,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data14:
		.BYTE	$76,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data15:
		.BYTE	$77,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data16:
		.BYTE	$78,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data17:
		.BYTE	$79,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data18:
		.BYTE	$7A,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data19:
		.BYTE	$7B,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data1A:
		.BYTE	$7C,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data1B:
		.BYTE	$7D,$80,$81,$82,$83,$84,$85,$86,$87,$DF
_build_constuct_data1C:
		.BYTE	$7E,$80,$81,$82,$83,$84,$85,$86,$87,$DF

; =============== S U B	R O U T	I N E =======================================
_get_building_scheme_ex:
		LDY		byte_CC
		DEY
		JMP		_get_building_scheme_ptr

; =============== S U B	R O U T	I N E =======================================
_get_cur_building_scheme_ptr:
		LDX		_const_palette_selected_idx
		LDY		_building_sizes_list,X
		DEY
		CPX		#$0C
		BNE		_get_building_scheme_ptr
		LDA		_cur_port_type_select_idx
		BEQ		_get_building_scheme_ptr
		INY

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_get_building_scheme_ptr:
		LDA		_build_schemes_lib_lo,Y
		STA		_tmp_ptrC5
		LDA		_build_schemes_lib_hi,Y
		STA		_tmp_ptrC5+1
		RTS

_build_schemes_lib_lo:
		.BYTE	<[_build_scheme_1_1]
		.BYTE	<[_build_scheme_2_2]
		.BYTE	<[_build_scheme_3_3]
		.BYTE	<[_build_scheme_4_4]
_build_schemes_lib_hi:
		.BYTE	>[_build_scheme_1_1]
		.BYTE	>[_build_scheme_2_2]
		.BYTE	>[_build_scheme_3_3]
		.BYTE	>[_build_scheme_4_4]
_build_scheme_1_1:
		.BYTE	$00,$80
_build_scheme_2_2:
		.BYTE	$00,$4C,$4D,$01,$80
_build_scheme_3_3:
		.BYTE	$00,$4C,$4D,$01,$98,$99,$9A,$4E,$02,$80
_build_scheme_4_4:
		.BYTE	$00,$4C,$4D,$01,$98,$99,$9A,$4E,$02,$E4,$E5,$E6,$E7,$9B,$4F,$03
		.BYTE	$80

; =============== S U B	R O U T	I N E =======================================
sub_150BF7:
		CMP		#$60
		BEQ		loc_150C2B
		CMP		#$61
		BEQ		loc_150C47
		CMP		#$71
		BEQ		loc_150C63
		CMP		#$72
		BEQ		loc_150C7E
		CMP		#$63
		BNE		loc_150C0E
		JMP		loc_150C99
loc_150C0E:
		CMP		#$41
		BCC		loc_150C16
		CMP		#$49
		BCC		loc_150C18
loc_150C16:
		CLC
		RTS
loc_150C18:
		SEC
		SBC		#$41
		ASL
		TAY
		LDA		off_150DDF,Y
		STA		_ptr0
		INY
		LDA		off_150DDF,Y
		STA		_ptr0+1
		JMP		(_ptr0)
loc_150C2B:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150CBF,X
		STA		word_6C+1
		INX
		LDA		byte_150CBF,X
		STA		word_6C
		SEC
		RTS
loc_150C47:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150D07,X
		STA		word_6C+1
		INX
		LDA		byte_150D07,X
		STA		word_6C
		SEC
		RTS
loc_150C63:
		LDA		byte_535
		AND		#$01
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150D4F,X
		STA		word_6C+1
		INX
		LDA		byte_150D4F,X
		STA		word_6C
		SEC
		RTS
loc_150C7E:
		LDA		byte_535
		AND		#$01
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150D73,X
		STA		word_6C+1
		INX
		LDA		byte_150D73,X
		STA		word_6C
		SEC
		RTS
loc_150C99:
		PHA
		LDA		_stadium_R_delta_positive_bonus_flag
		BNE		loc_150CA2
		PLA
		CLC
		RTS
loc_150CA2:
		PLA
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150D97,X
		STA		word_6C+1
		INX
		LDA		byte_150D97,X
		STA		word_6C
		SEC
		RTS
byte_150CBF:
		.BYTE	$87,$E0
		.BYTE	$87,$E0
		.BYTE	$87,$E0
		.BYTE	$87,$E0
		.BYTE	$87,$F0
		.BYTE	$87,$F0
		.BYTE	$87,$F0
		.BYTE	$87,$F0
		.BYTE	$87,$21
		.BYTE	$87,$14
		.BYTE	$87,$54
		.BYTE	$87,$17
		.BYTE	$87,$01
		.BYTE	$87,$04
		.BYTE	$87,$44
		.BYTE	$87,$16
		.BYTE	$87,$E3
		.BYTE	$87,$E3
		.BYTE	$87,$E3
		.BYTE	$87,$E3
		.BYTE	$87,$E4
		.BYTE	$87,$E4
		.BYTE	$87,$E4
		.BYTE	$87,$E4
		.BYTE	$87,$E5
		.BYTE	$87,$E5
		.BYTE	$87,$E5
		.BYTE	$87,$E5
		.BYTE	$87,$61
		.BYTE	$87,$34
		.BYTE	$87,$74
		.BYTE	$87,$37
		.BYTE	$87,$41
		.BYTE	$87,$24
		.BYTE	$87,$64
		.BYTE	$87,$36
byte_150D07:
		.BYTE	$87,$F3
		.BYTE	$87,$F3
		.BYTE	$87,$F3
		.BYTE	$87,$F3
		.BYTE	$87,$80
		.BYTE	$87,$82
		.BYTE	$87,$84
		.BYTE	$87,$86
		.BYTE	$87,$81
		.BYTE	$87,$83
		.BYTE	$87,$85
		.BYTE	$87,$87
		.BYTE	$87,$56
		.BYTE	$87,$76
		.BYTE	$87,$49
		.BYTE	$87,$4B
		.BYTE	$87,$90
		.BYTE	$87,$92
		.BYTE	$87,$94
		.BYTE	$87,$96
		.BYTE	$87,$91
		.BYTE	$87,$93
		.BYTE	$87,$95
		.BYTE	$87,$97
		.BYTE	$87,$F8
		.BYTE	$87,$09
		.BYTE	$87,$F8
		.BYTE	$87,$09
		.BYTE	$87,$E8
		.BYTE	$87,$E8
		.BYTE	$87,$E8
		.BYTE	$87,$E8
		.BYTE	$87,$57
		.BYTE	$87,$77
		.BYTE	$87,$4A
		.BYTE	$87,$4C
byte_150D4F:
		.BYTE	$D2,$31
		.BYTE	$D2,$D3
		.BYTE	$D2,$41
		.BYTE	$D2,$E3
		.BYTE	$D2,$42
		.BYTE	$D2,$E4
		.BYTE	$D2,$32
		.BYTE	$D2,$D4
		.BYTE	$12,$51
		.BYTE	$12,$F3
		.BYTE	$12,$52
		.BYTE	$12,$F4
		.BYTE	$12,$53
		.BYTE	$12,$F5
		.BYTE	$92,$43
		.BYTE	$92,$E5
		.BYTE	$92,$33
		.BYTE	$92,$D5
byte_150D73:
		.BYTE	$D2,$09
		.BYTE	$D2,$D6
		.BYTE	$D2,$19
		.BYTE	$D2,$E6
		.BYTE	$D2,$1A
		.BYTE	$D2,$E7
		.BYTE	$D2,$0A
		.BYTE	$D2,$D7
		.BYTE	$D2,$29
		.BYTE	$D2,$F6
		.BYTE	$D2,$2A
		.BYTE	$D2,$F7
		.BYTE	$D2,$2B
		.BYTE	$D2,$F8
		.BYTE	$D2,$1B
		.BYTE	$D2,$E8
		.BYTE	$D2,$0B
		.BYTE	$D2,$D8
byte_150D97:
		.BYTE	$00,$CE
		.BYTE	$00,$CE
		.BYTE	$00,$CE
		.BYTE	$00,$CE
		.BYTE	$00,$DF
		.BYTE	$00,$DF
		.BYTE	$00,$DF
		.BYTE	$00,$DF
		.BYTE	$C0,$EF
		.BYTE	$C0,$F0
		.BYTE	$C0,$F1
		.BYTE	$C0,$F3
		.BYTE	$C0,$EE
		.BYTE	$C0,$CF
		.BYTE	$C0,$CF
		.BYTE	$C0,$CF
		.BYTE	$00,$E2
		.BYTE	$00,$E2
		.BYTE	$00,$E2
		.BYTE	$00,$E2
		.BYTE	$C0,$E3
		.BYTE	$C0,$E3
		.BYTE	$C0,$F2
		.BYTE	$C0,$E3
		.BYTE	$00,$E4
		.BYTE	$00,$E4
		.BYTE	$00,$E4
		.BYTE	$00,$E4
		.BYTE	$00,$E1
		.BYTE	$00,$E1
		.BYTE	$00,$E1
		.BYTE	$00,$E1
		.BYTE	$00,$DE
		.BYTE	$00,$DE
		.BYTE	$00,$DE
		.BYTE	$00,$DE
off_150DDF:
		.WORD	sub_150DEF
		.WORD	sub_150E2B
		.WORD	sub_150E67
		.WORD	sub_150EA3
		.WORD	sub_150EDF
		.WORD	sub_150F1B
		.WORD	sub_150F57
		.WORD	sub_150F93

; =============== S U B	R O U T	I N E =======================================
sub_150DEF:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150E0B,X
		STA		word_6C+1
		INX
		LDA		byte_150E0B,X
		STA		word_6C
		SEC
		RTS
byte_150E0B:
		.BYTE	$87,$00
		.BYTE	$87,$20
		.BYTE	$87,$40
		.BYTE	$87,$60
		.BYTE	$47,$10
		.BYTE	$47,$30
		.BYTE	$47,$50
		.BYTE	$47,$70
		.BYTE	$87,$11
		.BYTE	$87,$31
		.BYTE	$87,$51
		.BYTE	$87,$71
		.BYTE	$47,$A1
		.BYTE	$47,$A1
		.BYTE	$47,$A1
		.BYTE	$47,$A1

; =============== S U B	R O U T	I N E =======================================
sub_150E2B:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150E47,X
		STA		word_6C+1
		INX
		LDA		byte_150E47,X
		STA		word_6C
		SEC
		RTS
byte_150E47:
		.BYTE	$47,$02
		.BYTE	$47,$22
		.BYTE	$47,$42
		.BYTE	$47,$62
		.BYTE	$47,$12
		.BYTE	$47,$32
		.BYTE	$47,$52
		.BYTE	$47,$72
		.BYTE	$87,$13
		.BYTE	$87,$33
		.BYTE	$87,$53
		.BYTE	$87,$73
		.BYTE	$47,$03
		.BYTE	$47,$23
		.BYTE	$47,$43
		.BYTE	$47,$63

; =============== S U B	R O U T	I N E =======================================
sub_150E67:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150E83,X
		STA		word_6C+1
		INX
		LDA		byte_150E83,X
		STA		word_6C
		SEC
		RTS
byte_150E83:
		.BYTE	$87,$A4
		.BYTE	$87,$A4
		.BYTE	$87,$A4
		.BYTE	$87,$A4
		.BYTE	$87,$B4
		.BYTE	$87,$B4
		.BYTE	$87,$B4
		.BYTE	$87,$B4
		.BYTE	$47,$15
		.BYTE	$47,$35
		.BYTE	$47,$55
		.BYTE	$47,$75
		.BYTE	$47,$05
		.BYTE	$47,$25
		.BYTE	$47,$45
		.BYTE	$47,$65

; =============== S U B	R O U T	I N E =======================================
sub_150EA3:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150EBF,X
		STA		word_6C+1
		INX
		LDA		byte_150EBF,X
		STA		word_6C
		SEC
		RTS
byte_150EBF:
		.BYTE	$47,$06
		.BYTE	$47,$26
		.BYTE	$47,$46
		.BYTE	$47,$66
		.BYTE	$87,$B6
		.BYTE	$87,$B6
		.BYTE	$87,$B6
		.BYTE	$87,$B6
		.BYTE	$87,$B7
		.BYTE	$87,$B7
		.BYTE	$87,$B7
		.BYTE	$87,$B7
		.BYTE	$47,$07
		.BYTE	$47,$27
		.BYTE	$47,$47
		.BYTE	$47,$67

; =============== S U B	R O U T	I N E =======================================
sub_150EDF:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150EFB,X
		STA		word_6C+1
		INX
		LDA		byte_150EFB,X
		STA		word_6C
		SEC
		RTS
byte_150EFB:
		.BYTE	$47,$08
		.BYTE	$47,$28
		.BYTE	$47,$48
		.BYTE	$47,$68
		.BYTE	$47,$18
		.BYTE	$47,$38
		.BYTE	$47,$58
		.BYTE	$47,$78
		.BYTE	$47,$19
		.BYTE	$47,$39
		.BYTE	$47,$59
		.BYTE	$47,$79
		.BYTE	$47,$C1
		.BYTE	$47,$C1
		.BYTE	$47,$C1
		.BYTE	$47,$C1

; =============== S U B	R O U T	I N E =======================================
sub_150F1B:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150F37,X
		STA		word_6C+1
		INX
		LDA		byte_150F37,X
		STA		word_6C
		SEC
		RTS
byte_150F37:
		.BYTE	$47,$0A
		.BYTE	$47,$C2
		.BYTE	$47,$C2
		.BYTE	$47,$C2
		.BYTE	$47,$D2
		.BYTE	$47,$D2
		.BYTE	$47,$5A
		.BYTE	$47,$7A
		.BYTE	$47,$D3
		.BYTE	$47,$D3
		.BYTE	$47,$5B
		.BYTE	$47,$7B
		.BYTE	$47,$C3
		.BYTE	$47,$2B
		.BYTE	$47,$C3
		.BYTE	$47,$C3

; =============== S U B	R O U T	I N E =======================================
sub_150F57:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150F73,X
		STA		word_6C+1
		INX
		LDA		byte_150F73,X
		STA		word_6C
		SEC
		RTS
byte_150F73:
		.BYTE	$47,$C4
		.BYTE	$47,$C4
		.BYTE	$47,$C4
		.BYTE	$47,$C4
		.BYTE	$47,$D4
		.BYTE	$47,$D4
		.BYTE	$47,$D4
		.BYTE	$47,$D4
		.BYTE	$47,$D5
		.BYTE	$47,$D5
		.BYTE	$47,$D5
		.BYTE	$47,$D5
		.BYTE	$47,$0D
		.BYTE	$47,$2D
		.BYTE	$47,$4D
		.BYTE	$47,$6D

; =============== S U B	R O U T	I N E =======================================
sub_150F93:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		TXA
		ASL
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_150FAF,X
		STA		word_6C+1
		INX
		LDA		byte_150FAF,X
		STA		word_6C
		SEC
		RTS
byte_150FAF:
		.BYTE	$47,$0E
		.BYTE	$47,$2E
		.BYTE	$47,$C6
		.BYTE	$47,$C6
		.BYTE	$47,$1E
		.BYTE	$47,$3E
		.BYTE	$47,$D6
		.BYTE	$47,$D6
		.BYTE	$87,$1F
		.BYTE	$87,$3F
		.BYTE	$87,$5F
		.BYTE	$87,$7F
		.BYTE	$47,$0F
		.BYTE	$47,$2F
		.BYTE	$47,$4F
		.BYTE	$47,$6F

; =============== S U B	R O U T	I N E =======================================
sub_150FCF:
		TAY
		CMP		#$91
		BCC		loc_150FF8
		CMP		#$9C
		BCC		loc_150FFF
		CMP		#$B3
		BEQ		loc_151029
		CMP		#$B7
		BCC		loc_150FF8
		CMP		#$BB
		BCC		loc_15101B
		CMP		#$BE
		BCC		loc_150FF8
		CMP		#$CE
		BCC		loc_151041
		CMP		#$E0
		BCC		loc_150FF8
		CMP		#$E2
		BCC		loc_15100D
		CMP		#$E6
		BEQ		loc_151035
loc_150FF8:
		LDA		#$00
		STA		byte_533
		TYA
		RTS
loc_150FFF:
		TYA
		SEC
		SBC		#$91
		STA		byte_534
		LDA		#$02
		STA		byte_533
		TYA
		RTS
loc_15100D:
		TYA
		SEC
		SBC		#$D1
		STA		byte_534
		LDA		#$02
		STA		byte_533
		TYA
		RTS
loc_15101B:
		TYA
		SEC
		SBC		#$AC
		STA		byte_534
		LDA		#$02
		STA		byte_533
		TYA
		RTS
loc_151029:
		LDA		#$00
		STA		byte_534
		LDA		#$08
		STA		byte_533
		TYA
		RTS
loc_151035:
		LDA		#$01
		STA		byte_534
		LDA		#$08
		STA		byte_533
		TYA
		RTS
loc_151041:
		SEC
		SBC		#$BE
		STA		byte_534
		LDA		#$04
		STA		byte_533
		TYA
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_15104E:
		LDA		byte_533
		BNE		loc_151054
		RTS
loc_151054:
		LSR
		BCS		sub_151061
		LSR
		BCS		sub_1510B2
		LSR
		BCS		sub_1510D9
		LSR
		JMP		sub_1510F5

; =============== S U B	R O U T	I N E =======================================
sub_151061:
		LDA		byte_535
		AND		#$01
		BEQ		nullsub_7
		JSR		_special_power_grid_bit_test
		BCC		nullsub_7
		LDX		byte_534
		CPX		#$00
		BNE		nullsub_7
		MOVWI	word_6C,$0287
nullsub_7:
		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref:
;		LDA		word_6C+1
;		AND		#$0F
;		BNE		loc_1510A7
;		LDA		word_6C
;		CMP		#$CE
;		BCC		loc_151091
;		CMP		#$D0
;		BCC		loc_15109C
;		CMP		#$DE
;		BCS		loc_15109C
;loc_151091:
;		LDA		byte_151125,X
;		STA		word_6C+1
;		LDA		byte_151129,X
;		STA		word_6C
;		RTS
;loc_15109C:
;		LDA		byte_15112D,X
;		STA		word_6C+1
;		LDA		byte_151136,X
;		STA		word_6C
;		RTS
;loc_1510A7:
;		LDA		byte_15113F,X
;		STA		word_6C+1
;		LDA		byte_15114F,X
;		STA		word_6C
;		RTS
;byte_151125:
;		.BYTE	$86,$86,$86,$86
;byte_151129:
;		.BYTE	$61,$63,$64,$62
;byte_15112D:
;		.BYTE	$86,$86,$86,$86,$86,$86,$86,$86,$86
;byte_151136:
;		.BYTE	$60,$66,$67,$65,$60,$69,$60,$68,$60
;byte_15113F:
;		.BYTE	$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86
;byte_15114F:
;		.BYTE	$60,$60,$61,$60,$60,$63,$64,$62,$60,$60,$60,$60,$60,$60,$60,$60

; =============== S U B	R O U T	I N E =======================================
sub_1510B2:
		JSR		_traffic_dencity_area_read
		BCC		locret_1510D8
		STA		_ptr0
		LDA		byte_534
		ASL
		ORA		_ptr0
		ASL
		ASL
		ASL
		STA		_ptr0
		LDA		byte_535
		AND		#$03
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_15115F,X
		STA		word_6C+1
		INX
		LDA		byte_15115F,X
		STA		word_6C
locret_1510D8:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_1510D9:
		LDA		byte_535
		AND		#$01
		ASL
		STA		_ptr0
		LDA		byte_534
		ASL
		ASL
		ORA		_ptr0
		TAX
		LDA		byte_15124F,X
		STA		word_6C+1
		INX
		LDA		byte_15124F,X
		STA		word_6C
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_1510F5:
		LDA		byte_535
		AND		#$03
		ASL
		STA		_ptr0
		LDA		byte_534
		ASL
		ASL
		ASL
		PHA
		LDA		byte_534
		BNE		loc_151115
		LDA		_tmp_tile_pos._COL
		EOR		_tmp_tile_pos._ROW
		ASL
		CLC
		ADC		_ptr0
		AND		#$06
		STA		_ptr0
loc_151115:
		PLA
		ORA		_ptr0
		TAX
		LDA		byte_15128F,X
		STA		word_6C+1
		INX
		LDA		byte_15128F,X
		STA		word_6C
		RTS
byte_15115F:
		.BYTE	$41,$E2
		.BYTE	$42,$E2
		.BYTE	$43,$E2
		.BYTE	$44,$E2
		.BYTE	$41,$F1
		.BYTE	$42,$F1
		.BYTE	$41,$F1
		.BYTE	$42,$F1
		.BYTE	$41,$E5
		.BYTE	$42,$E5
		.BYTE	$43,$E5
		.BYTE	$44,$E5
		.BYTE	$41,$F4
		.BYTE	$42,$F4
		.BYTE	$41,$F4
		.BYTE	$42,$F4
		.BYTE	$41,$E1
		.BYTE	$42,$E1
		.BYTE	$43,$E1
		.BYTE	$44,$E1
		.BYTE	$41,$F0
		.BYTE	$42,$F0
		.BYTE	$41,$F0
		.BYTE	$42,$F0
		.BYTE	$41,$E4
		.BYTE	$42,$E4
		.BYTE	$43,$E4
		.BYTE	$44,$E4
		.BYTE	$41,$F3
		.BYTE	$42,$F3
		.BYTE	$41,$F3
		.BYTE	$42,$F3
		.BYTE	$41,$E9
		.BYTE	$42,$E9
		.BYTE	$43,$E9
		.BYTE	$44,$E9
		.BYTE	$41,$F8
		.BYTE	$42,$F8
		.BYTE	$41,$F8
		.BYTE	$42,$F8
		.BYTE	$41,$EC
		.BYTE	$42,$EC
		.BYTE	$43,$EC
		.BYTE	$44,$EC
		.BYTE	$41,$FB
		.BYTE	$42,$FB
		.BYTE	$41,$FB
		.BYTE	$42,$FB
		.BYTE	$41,$E6
		.BYTE	$42,$E6
		.BYTE	$43,$E6
		.BYTE	$44,$E6
		.BYTE	$41,$F5
		.BYTE	$42,$F5
		.BYTE	$41,$F5
		.BYTE	$42,$F5
		.BYTE	$41,$E8
		.BYTE	$42,$E8
		.BYTE	$43,$E8
		.BYTE	$44,$E8
		.BYTE	$41,$F7
		.BYTE	$42,$F7
		.BYTE	$41,$F7
		.BYTE	$42,$F7
		.BYTE	$41,$E3
		.BYTE	$42,$E3
		.BYTE	$43,$E3
		.BYTE	$44,$E3
		.BYTE	$41,$F2
		.BYTE	$42,$F2
		.BYTE	$41,$F2
		.BYTE	$42,$F2
		.BYTE	$41,$EB
		.BYTE	$42,$EB
		.BYTE	$43,$EB
		.BYTE	$44,$EB
		.BYTE	$41,$FA
		.BYTE	$42,$FA
		.BYTE	$41,$FA
		.BYTE	$42,$FA
		.BYTE	$41,$E7
		.BYTE	$42,$E7
		.BYTE	$43,$E7
		.BYTE	$44,$E7
		.BYTE	$41,$F6
		.BYTE	$42,$F6
		.BYTE	$41,$F6
		.BYTE	$42,$F6
		.BYTE	$41,$EA
		.BYTE	$42,$EA
		.BYTE	$43,$EA
		.BYTE	$44,$EA
		.BYTE	$41,$F9
		.BYTE	$42,$F9
		.BYTE	$41,$F9
		.BYTE	$42,$F9
		.BYTE	$41,$ED
		.BYTE	$42,$ED
		.BYTE	$43,$ED
		.BYTE	$44,$ED
		.BYTE	$41,$FC
		.BYTE	$42,$FC
		.BYTE	$41,$FC
		.BYTE	$42,$FC
		.BYTE	$41,$EE
		.BYTE	$42,$EE
		.BYTE	$43,$EE
		.BYTE	$44,$EE
		.BYTE	$41,$FD
		.BYTE	$42,$FD
		.BYTE	$41,$FD
		.BYTE	$42,$FD
		.BYTE	$41,$EF
		.BYTE	$42,$EF
		.BYTE	$43,$EF
		.BYTE	$44,$EF
		.BYTE	$41,$FE
		.BYTE	$42,$FE
		.BYTE	$41,$FE
		.BYTE	$42,$FE
byte_15124F:
		.BYTE	$C1,$40
		.BYTE	$C1,$70
		.BYTE	$C1,$41
		.BYTE	$C1,$71
		.BYTE	$C1,$42
		.BYTE	$C1,$72
		.BYTE	$C1,$50
		.BYTE	$C1,$80
		.BYTE	$C1,$52
		.BYTE	$C1,$82
		.BYTE	$C1,$60
		.BYTE	$C1,$90
		.BYTE	$C1,$61
		.BYTE	$C1,$91
		.BYTE	$C1,$62
		.BYTE	$C1,$92
		.BYTE	$C1,$43
		.BYTE	$C1,$73
		.BYTE	$C1,$44
		.BYTE	$C1,$84
		.BYTE	$C1,$45
		.BYTE	$C1,$75
		.BYTE	$C1,$53
		.BYTE	$C1,$83
		.BYTE	$C1,$55
		.BYTE	$C1,$85
		.BYTE	$C1,$63
		.BYTE	$C1,$93
		.BYTE	$C1,$64
		.BYTE	$C1,$94
		.BYTE	$C1,$65
		.BYTE	$C1,$95
byte_15128F:
		.BYTE	$C1,$E0
		.BYTE	$C2,$E0
		.BYTE	$C3,$E0
		.BYTE	$C4,$E0
		.BYTE	$81,$3E
		.BYTE	$81,$3F
		.BYTE	$81,$5E
		.BYTE	$81,$5F

; =============== S U B	R O U T	I N E =======================================
_special_power_grid_bit_test:
		LDA		_thread0_flags
		AND		#$02
		BNE		loc_1512D7
		LDA		_tmp6E
		STA		_ptr0
		LDA		_tmp6E+1
		SEC
		SBC		#>[_wram_map_buf]
		STA		_ptr0+1
		LDY		#$03
loc_1512B3:
		LSR		_ptr0+1
		ROR		_ptr0
		ROR
		DEY
		BNE		loc_1512B3
		LSR
		LSR
		LSR
		LSR
		LSR
		TAX
		CLC
		LDA		_ptr0
		ADC		#<[_power_grid_map_buf]
		STA		_ptr0
		LDA		_ptr0+1
		ADC		#>[_power_grid_map_buf]
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		AND		_inv_bitmasks,X
		BEQ		loc_1512D9
loc_1512D7:
		CLC
		RTS
loc_1512D9:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_traffic_dencity_area_read:
		LDX		#$00
		LDA		_tmp_tile_pos._ROW
		LSR
		LDY		#$13
		MULYA
		STA		_ptr0
		LDA		_tmp_tile_pos._COL
		LSR
		LSR
		BCC		loc_1512FF
		INX
loc_1512FF:
		CLC
		ADC		_ptr0
		STA		_ptr0
		BCC		loc_151307
		INY
loc_151307:
		CLC
		ADC		#<[_traffic_density_map_buf]
		STA		_ptr0
		TYA
		ADC		#>[_traffic_density_map_buf]
		STA		_ptr0+1
		LDY		#$00
		LDA		(_ptr0),Y
		AND		byte_15132F,X
		CPX		#$01
		BEQ		loc_151320
		LSR
		LSR
		LSR
		LSR
loc_151320:
		LDX		#$00
		CMP		#$05
		BCC		locret_15132E
		CMP		#$0B
		BCC		loc_15132B
		INX
loc_15132B:
		TXA
		SEC
		RTS
locret_15132E:
		RTS
byte_15132F:
		.BYTE	$F0,$0F

; =============== S U B	R O U T	I N E =======================================
;_unref_4:
;		LDA		#$00
;		JSR		_pal_load_lib
;		LDA		#$80
;		JSR		_pal_load_lib
;		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_palette_redraw_req_set:
		LDA		#$FF
		STA		_constr_palette_redraw_req
		RTS

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_icon_push_draw:
; REDUNDANT, merged too
;		LDX		_gui_ctrl_menu_press_idx
;		LDA		gui_ctrl_menu_icons_posY_list,X
;		STA		_tmp6
;		LDA		gui_ctrl_menu_icons_posX_list,X
;		STA		_tmp4
;		LDA		#$1C
;		STA		_tmp5
;		JSR		gui_ctrl_menu_icon_pos_calc
;		JSR		gui_ctrl_menu_icon_push_queue
;		RTS
		LDY		#$00							; OPTIMIZED, merged
		BEQ		_gui_ctrl_menu_icon_draw

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_icon_release_draw:
		LDY		#$01							; OPTIMIZED, merged

; !FALLTHROUTH!

_gui_ctrl_menu_icon_draw:
		LDX		_gui_ctrl_menu_press_idx
		LDA		gui_ctrl_menu_icons_posY_list,X
		STA		_tmp6
		LDA		gui_ctrl_menu_icons_posX_list,X
		STA		_tmp4
		LDA		#$1C
		STA		_tmp5
		JSR		gui_ctrl_menu_icon_pos_calc
;		JSR		gui_ctrl_menu_icon_release_queue; REDUNDANT, merged
		JMP		gui_ctrl_menu_icon_queue		; FIX: merged


gui_ctrl_menu_icons_posX_list:
		.BYTE	$02,$04,$06,$08,$0A,$0C
gui_ctrl_menu_icons_posY_list:
		.BYTE	$02,$02,$02,$02,$02,$04

; =============== S U B	R O U T	I N E =======================================
; NOTE: this is the only place, where WRAP code has both BEQ/BSC opcodes
; so it avoid extra calculations, the rest of the code does not!
;
; FIX: when we switch to macroses, we put extra BEQ opcode in other such plces
;
gui_ctrl_menu_icon_pos_calc:
		CIADDB	_tmp0,_cur_screen_tile._COL,_tmp4,NONE,#$20
		WRAPB	_tmp2,_tmp3,_tmp0,_tmp6,#$20
		CIADDB	_tmp1,_cur_screen_tile._ROW,_tmp5,NONE,#$1E
		WRAPA	_tmp4,_tmp5,#$02,#$1E
		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#>[_MMC5_EXRAM]	; DST=(_ptr0+1*#$20+_ptr0)+#>[_MMC5_EXRAM]00
		RTS

; FIX: merged version of two following functions without using arrays of
; similar bytes.
; CAUTION! if you ever need to move the gui ctrl icons graphics, make sure
; they aren't in different banks and are together, or else re-enable all
; redundant functions.
;
; =============== S U B	R O U T	I N E =======================================
; input Y = byte to draw.
; draw 2x2 rect of extnt bytes to push/release icons
;
gui_ctrl_menu_icon_queue:
		EXTQSTART
loc_15145B:
		MOVW	_tmp6,_tmp2
		EXTQMOVB	_ptr0,OP_NOP,0
loc_151474:
		EXTQMOVB	_ptr0+1,OP_NOP,0
		EXTQMOVB	_tmp6,OP_NOP,0
loc_151475:
		LDA		gui_ctrl_menu_mode_byte,Y
		EXTQMOVA
		DEC		_tmp6
		BNE		loc_151475
		LDA		_tmp6+1
		BEQ		loc_1514A0
		STA		_tmp6
		EXTQMOVB	_ptr0,OP_AND,#$E0
		LDA		#$00
		STA		_tmp6+1
		BEQ		loc_151474
loc_1514A0:
		ADDWB	_ptr0,#$20
		DEC		_tmp4
		BNE		loc_15145B
		LDA		_tmp5
		BEQ		loc_1514C7
		STA		_tmp4
		LDA		_ptr0
		AND		#<[_MMC5_EXRAM+$1F]
		STA		_ptr0
		LDA		#>[_MMC5_EXRAM+$1F]
		STA		_ptr0+1
		LDA		#$00
		STA		_tmp5
		BEQ		loc_15145B
loc_1514C7:
		EXTQEND
		RTS

gui_ctrl_menu_mode_byte:
		.BYTE	$83,$82

; REDUNDANT: even more, the values are the same for every window, maybe they have been
; different someday, but not now.
;
; =============== S U B	R O U T	I N E =======================================
;gui_ctrl_menu_icon_release_queue:
;		EXTQSTART
;		LDY		_gui_ctrl_menu_press_idx
;loc_1513E8:
;		MOVW	_tmp6,_tmp2
;		EXTQMOVB	_ptr0,OP_NOP,0
;		EXTQMOVB	_ptr0+1,OP_NOP,0
;		EXTQMOVB	_tmp6,OP_NOP,0
;loc_151402:
;		LDA		byte_1514CA,Y
;		EXTQMOVA
;		DEC		_tmp6
;		BNE		loc_151402
;		LDA		_tmp6+1
;		BEQ		loc_15142D
;		STA		_tmp6
;		EXTQMOVB	_ptr0,OP_AND,#$E0
;		EXTQMOVB	_ptr0+1,OP_NOP,0
;		EXTQMOVB	_tmp6,OP_NOP,0
;		LDA		#$00
;		STA		_tmp6+1
;		BEQ		loc_151402
;loc_15142D:
;		LDA		_ptr0							; TODO: optimize, this is ADDWB opcode actually
;		CLC
;		ADC		#$20
;		STA		_ptr0
;		LDA		_ptr0+1
;		ADC		#$00
;		STA		_ptr0+1
;		DEC		_tmp4
;		BNE		loc_1513E8
;		LDA		_tmp5
;		BEQ		loc_151454
;		STA		_tmp4
;		LDA		_ptr0
;		AND		#<[_MMC5_EXRAM+$1F]
;		STA		_ptr0
;		LDA		#>[_MMC5_EXRAM+$1F]
;		STA		_ptr0+1
;		LDA		#$00
;		STA		_tmp5
;		BEQ		loc_1513E8
;loc_151454:
;		EXTQEND
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;gui_ctrl_menu_icon_push_queue:
;		EXTQSTART
;		LDY		_gui_ctrl_menu_press_idx
;loc_15145B:
;		MOVW	_tmp6,_tmp2
;		EXTQMOVB	_ptr0,OP_NOP,0
;		EXTQMOVB	_ptr0+1,OP_NOP,0
;		EXTQMOVB	_tmp6,OP_NOP,0
;loc_151475:
;		LDA		byte_1514D0,Y
;		EXTQMOVA
;		DEC		_tmp6
;		BNE		loc_151475
;		LDA		_tmp6+1
;		BEQ		loc_1514A0
;		STA		_tmp6
;		EXTQMOVB	_ptr0,OP_AND,#$E0
;		EXTQMOVB	_ptr0+1,OP_NOP,0
;		EXTQMOVB	_tmp6,OP_NOP,0
;		LDA		#$00
;		STA		_tmp6+1
;		BEQ		loc_151475
;loc_1514A0:
;		LDA		_ptr0
;		CLC
;		ADC		#$20
;		STA		_ptr0
;		LDA		_ptr0+1
;		ADC		#$00
;		STA		_ptr0+1
;		DEC		_tmp4
;		BNE		loc_15145B
;		LDA		_tmp5
;		BEQ		loc_1514C7
;		STA		_tmp4
;		LDA		_ptr0
;		AND		#<[_MMC5_EXRAM+$1F]
;		STA		_ptr0
;		LDA		#>[_MMC5_EXRAM+$1F]
;		STA		_ptr0+1
;		LDA		#$00
;		STA		_tmp5
;		BEQ		loc_15145B
;loc_1514C7:
;		EXTQEND
;		RTS
;
;byte_1514CA:
;		.BYTE	$82,$82,$82,$82,$82,$82
;byte_1514D0:
;		.BYTE	$83,$83,$83,$83,$83,$83

byte_1514D6:
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$DF,$AF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$60,$B0,$B1,$B2,$B3,$B4,$B5,$B6,$B7,$B8,$B9,$BA,$BB,$BC,$BD
		.BYTE	$DF,$DF,$DF,$DF,$DA,$DB,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$70,$C0,$C1,$C2,$C3,$C4,$C5,$C6,$C7,$C8,$C9,$CA,$CB,$CC,$CD
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$00
byte_1515B7:
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$59,$59,$59,$59,$59,$59,$59,$59,$59,$59,$01,$82,$42,$42
		.BYTE	$42,$42,$42,$42,$01,$01,$42,$42,$42,$42,$01,$42,$42,$42,$01,$01
		.BYTE	$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82
		.BYTE	$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82
		.BYTE	$01,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82
		.BYTE	$01,$01,$01,$01,$04,$04,$02,$02,$02,$02,$02,$02,$02,$02,$01,$01
		.BYTE	$01,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82,$82
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$00
byte_151698:
		.BYTE	$82,$82,$82,$82,$82,$82,$82,$82
		.BYTE	$82,$82,$82,$82,$02,$02,$02,$02
		.BYTE	$C2,$C2,$C2,$C2,$82,$82,$82,$82
		.BYTE	$02,$02,$02,$02,$82,$82,$82,$82
		.BYTE	$02,$02,$02,$02,$82,$82,$82,$82
		.BYTE	$82,$82,$82,$82,$02,$82,$02,$02
		.BYTE	$02,$02,$02,$02,$82,$82,$82,$82
byte_1516D0:
		.BYTE	$83,$83,$83,$83,$83,$83,$83,$83
		.BYTE	$83,$83,$83,$83,$03,$03,$03,$03
		.BYTE	$C3,$C3,$C3,$C3,$83,$83,$83,$83
		.BYTE	$03,$03,$03,$03,$83,$83,$83,$83
		.BYTE	$03,$03,$03,$03,$83,$83,$83,$83
		.BYTE	$83,$83,$83,$83,$03,$83,$03,$03
		.BYTE	$03,$03,$03,$03,$83,$83,$83,$83
byte_151708:
		.BYTE	$84,$84,$84,$84,$84,$84,$84,$84
		.BYTE	$84,$84,$84,$84,$04,$04,$04,$04
		.BYTE	$C4,$C4,$C4,$C4,$84,$84,$84,$84
		.BYTE	$04,$04,$04,$04,$84,$84,$84,$84
		.BYTE	$04,$04,$04,$04,$84,$84,$84,$84
		.BYTE	$84,$84,$84,$84,$04,$84,$04,$04
		.BYTE	$04,$04,$04,$04,$84,$84,$84,$84
byte_151740:
		.BYTE	$0B,$17,$03,$02,$01,$00
byte_151746:
		.BYTE	$0B,$17,$16,$15,$14,$13,$12,$11,$10
byte_15174F:
		.BYTE	$0B,$1E,$1D,$1C,$1B,$1A,$19,$18,$10
byte_151758:
		.BYTE	$0F,$09,$08,$07,$06,$05
byte_15175E:
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$D0,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3
		.BYTE	$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D3,$D5,$D1,$24,$34,$28,$38,$2C
		.BYTE	$3C,$40,$50,$44,$54,$48,$58,$4C,$5C,$0B,$0B,$80,$0B,$0F,$D6,$D1
		.BYTE	$25,$35,$29,$39,$2D,$3D,$41,$51,$45,$55,$49,$59,$4D,$5D,$0B,$0B
		.BYTE	$81,$0B,$0F,$D6,$D1,$26,$36,$2A,$3A,$2E,$3E,$42,$52,$46,$56,$4A
		.BYTE	$5A,$4E,$5E,$0B,$0B,$82,$0B,$0F,$D6,$D1,$27,$37,$2B,$3B,$2F,$3F
		.BYTE	$43,$53,$47,$57,$4B,$5B,$4F,$5F,$0A,$0C,$7F,$0C,$0E,$D6,$D2,$D4
		.BYTE	$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4,$D4
		.BYTE	$D4,$D4,$D7
byte_1517F1:
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$82,$82,$82,$82,$C2
		.BYTE	$C2,$02,$02,$02,$02,$82,$82,$02,$02,$99,$99,$82,$99,$99,$01,$01
		.BYTE	$82,$82,$82,$82,$C2,$C2,$02,$02,$02,$02,$82,$82,$02,$02,$19,$19
		.BYTE	$02,$19,$19,$01,$01,$82,$82,$02,$02,$82,$82,$82,$82,$82,$82,$82
		.BYTE	$82,$02,$02,$99,$99,$82,$99,$99,$01,$01,$82,$82,$02,$02,$82,$82
		.BYTE	$82,$82,$82,$82,$82,$82,$02,$02,$99,$99,$82,$99,$99,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01
off_151884:
		.BYTE	<[byte_151A82]
		.BYTE	<[byte_151A9A]
		.BYTE	<[byte_151AA2]
		.BYTE	<[byte_151AAA]
		.BYTE	<[byte_151AB2]
		.BYTE	<[byte_151ABA]
		.BYTE	<[byte_151AC2]
		.BYTE	<[byte_151ACA]
		.BYTE	<[byte_151AD2]
		.BYTE	<[byte_151ADA]
		.BYTE	<[byte_151AE2]
		.BYTE	<[byte_151AEA]
		.BYTE	<[byte_151AF2]
		.BYTE	<[byte_151AFA]
		.BYTE	<[byte_151B02]
		.BYTE	<[byte_151B0A]
		.BYTE	<[byte_151B12]
		.BYTE	<[byte_151B1A]
		.BYTE	<[byte_151B22]
		.BYTE	<[byte_151B2A]
		.BYTE	<[byte_151B32]
		.BYTE	<[byte_151B3A]
		.BYTE	<[byte_151B42]
		.BYTE	<[byte_151B4A]
		.BYTE	<[byte_151B52]
		.BYTE	<[byte_151A8A]
		.BYTE	<[byte_151A92]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151B5A]
		.BYTE	<[byte_151B62]
		.BYTE	<[byte_151B6A]
		.BYTE	<[byte_151B72]
		.BYTE	<[byte_151B7A]
		.BYTE	<[byte_151B82]
		.BYTE	<[byte_151B8A]
		.BYTE	<[byte_151B92]
		.BYTE	<[byte_151B9A]
		.BYTE	<[byte_151BA2]
		.BYTE	<[byte_151BAA]
		.BYTE	<[byte_151BB2]
		.BYTE	<[byte_151BBA]
		.BYTE	<[byte_151BC2]
		.BYTE	<[byte_151BCA]
		.BYTE	<[byte_151BD2]
		.BYTE	<[byte_151BDA]
		.BYTE	<[byte_151BE2]
		.BYTE	<[byte_151BEA]
		.BYTE	<[byte_151BF2]
		.BYTE	<[byte_151BFA]
		.BYTE	<[byte_151C02]
		.BYTE	<[byte_151C0A]
		.BYTE	<[byte_151C12]
		.BYTE	<[byte_151C1A]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151C22]
		.BYTE	<[byte_151C2A]
		.BYTE	<[byte_151C32]
		.BYTE	<[byte_151C3A]
		.BYTE	<[byte_151C42]
		.BYTE	<[byte_151C4A]
		.BYTE	<[byte_151C52]
		.BYTE	<[byte_151C5A]
		.BYTE	<[byte_151C62]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151C6A]
		.BYTE	<[byte_151C72]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151C7A]
		.BYTE	<[byte_151C8C]
		.BYTE	<[byte_151C9E]
		.BYTE	<[byte_151CB0]
		.BYTE	<[byte_151CC2]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151CE2]
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151CF4]
		.BYTE	<[byte_151D06]
		.BYTE	<[byte_151D18]
		.BYTE	<[byte_151D2A]
		.BYTE	<[byte_151D3C]
		.BYTE	<[byte_151D4E]
		.BYTE	<[byte_151D60]
		.BYTE	<[byte_151D72]
		.BYTE	<[byte_151D84]
		.BYTE	<[byte_151D96]
		.BYTE	<[byte_151DA8]
		.BYTE	<[byte_151DBA]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	<[byte_151DCC]
		.BYTE	<[byte_151DCE]
		.BYTE	<[byte_151DD0]
		.BYTE	<[byte_151DD2]
		.BYTE	<[byte_151DD4]
		.BYTE	<[byte_151DD6]
		.BYTE	<[byte_151DD8]
		.BYTE	<[byte_151DDA]
		.BYTE	<[byte_151DDC]
		.BYTE	<[byte_151DDE]
		.BYTE	<[byte_151DE0]
		.BYTE	<[byte_151DE2]
		.BYTE	<[byte_151DE4]
		.BYTE	<[byte_151DE6]
		.BYTE	<[byte_151DE8]
		.BYTE	<[byte_151DEA]
		.BYTE	<[byte_151DEC]
		.BYTE	<[byte_151DEE]
		.BYTE	<[byte_151DF0]
		.BYTE	<[byte_151DF2]
		.BYTE	<[byte_151DF4]
		.BYTE	<[byte_151DF6]
		.BYTE	<[byte_151DF8]
		.BYTE	<[byte_151DFA]
		.BYTE	<[byte_151DFC]
		.BYTE	<[byte_151DFE]
		.BYTE	<[byte_151E00]
		.BYTE	<[byte_151E02]
		.BYTE	<[byte_151E04]
		.BYTE	<[byte_151E06]
		.BYTE	<[byte_151E08]
		.BYTE	<[byte_151E0A]
		.BYTE	<[byte_151E0C]
		.BYTE	<[byte_151E0E]
		.BYTE	<[byte_151E10]
		.BYTE	<[byte_151E12]
		.BYTE	<[byte_151E14]
		.BYTE	<[byte_151E18]
		.BYTE	<[byte_151E16]
		.BYTE	<[byte_151E1A]
		.BYTE	<[byte_151E1C]
		.BYTE	<[byte_151E1E]
		.BYTE	<[byte_151E20]
		.BYTE	<[byte_151E22]
		.BYTE	<[byte_151E24]
		.BYTE	<[byte_151E26]
		.BYTE	<[byte_151E28]
		.BYTE	<[byte_151E2A]
		.BYTE	<[byte_151E2C]
		.BYTE	<[byte_151E2E]
		.BYTE	<[byte_151E30]
		.BYTE	<[byte_151E32]
		.BYTE	<[byte_151E34]
		.BYTE	<[byte_151E36]
		.BYTE	<[byte_151E38]
		.BYTE	<[byte_151E3A]
		.BYTE	<[byte_151E3C]
		.BYTE	<[byte_151E3E]
		.BYTE	<[byte_151E40]
		.BYTE	<[byte_151E42]
		.BYTE	<[byte_151E44]
		.BYTE	<[byte_151E46]
		.BYTE	<[byte_151E48]
		.BYTE	<[byte_151E4A]
		.BYTE	<[byte_151E4C]
		.BYTE	<[byte_151E4E]
		.BYTE	<[byte_151E50]
		.BYTE	<[byte_151E52]
		.BYTE	<[byte_151E54]
		.BYTE	<[byte_151E56]
		.BYTE	<[byte_151E58]
		.BYTE	<[byte_151E5A]
		.BYTE	<[byte_151E5C]
		.BYTE	<[byte_151E5E]
		.BYTE	<[byte_151E60]
		.BYTE	<[byte_151E62]
		.BYTE	<[byte_151E64]
		.BYTE	<[byte_151E66]
		.BYTE	<[byte_151E68]
		.BYTE	<[byte_151E6A]
		.BYTE	<[byte_151E6C]
		.BYTE	<[byte_151E6E]
		.BYTE	<[byte_151E70]
		.BYTE	<[byte_151E72]
		.BYTE	<[byte_151E74]
		.BYTE	<[byte_151E76]
		.BYTE	<[byte_151E78]
		.BYTE	<[byte_151E7A]
		.BYTE	<[byte_151E7C]
		.BYTE	<[byte_151E7E]
		.BYTE	<[byte_151E80]
		.BYTE	<[byte_151E82]
		.BYTE	<[byte_151E84]
		.BYTE	<[byte_151E86]
		.BYTE	<[byte_151E88]
		.BYTE	<[byte_151E8A]
		.BYTE	<[byte_151E8C]
		.BYTE	<[byte_151E8E]
		.BYTE	<[byte_151E90]
		.BYTE	<[byte_151E92]
		.BYTE	<[byte_151E94]
		.BYTE	<[byte_151E96]
		.BYTE	<[byte_151E98]
		.BYTE	<[byte_151E9A]
		.BYTE	<[byte_151E9C]
		.BYTE	<[byte_151E9E]
		.BYTE	<[byte_151EA0]
		.BYTE	<[byte_151EA2]
		.BYTE	<[byte_151EA4]
		.BYTE	0
		.BYTE	<[byte_151E26]
off_151983:
		.BYTE	>[byte_151A82]
		.BYTE	>[byte_151A9A]
		.BYTE	>[byte_151AA2]
		.BYTE	>[byte_151AAA]
		.BYTE	>[byte_151AB2]
		.BYTE	>[byte_151ABA]
		.BYTE	>[byte_151AC2]
		.BYTE	>[byte_151ACA]
		.BYTE	>[byte_151AD2]
		.BYTE	>[byte_151ADA]
		.BYTE	>[byte_151AE2]
		.BYTE	>[byte_151AEA]
		.BYTE	>[byte_151AF2]
		.BYTE	>[byte_151AFA]
		.BYTE	>[byte_151B02]
		.BYTE	>[byte_151B0A]
		.BYTE	>[byte_151B12]
		.BYTE	>[byte_151B1A]
		.BYTE	>[byte_151B22]
		.BYTE	>[byte_151B2A]
		.BYTE	>[byte_151B32]
		.BYTE	>[byte_151B3A]
		.BYTE	>[byte_151B42]
		.BYTE	>[byte_151B4A]
		.BYTE	>[byte_151B52]
		.BYTE	>[byte_151A8A]
		.BYTE	>[byte_151A92]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151B5A]
		.BYTE	>[byte_151B62]
		.BYTE	>[byte_151B6A]
		.BYTE	>[byte_151B72]
		.BYTE	>[byte_151B7A]
		.BYTE	>[byte_151B82]
		.BYTE	>[byte_151B8A]
		.BYTE	>[byte_151B92]
		.BYTE	>[byte_151B9A]
		.BYTE	>[byte_151BA2]
		.BYTE	>[byte_151BAA]
		.BYTE	>[byte_151BB2]
		.BYTE	>[byte_151BBA]
		.BYTE	>[byte_151BC2]
		.BYTE	>[byte_151BCA]
		.BYTE	>[byte_151BD2]
		.BYTE	>[byte_151BDA]
		.BYTE	>[byte_151BE2]
		.BYTE	>[byte_151BEA]
		.BYTE	>[byte_151BF2]
		.BYTE	>[byte_151BFA]
		.BYTE	>[byte_151C02]
		.BYTE	>[byte_151C0A]
		.BYTE	>[byte_151C12]
		.BYTE	>[byte_151C1A]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151C22]
		.BYTE	>[byte_151C2A]
		.BYTE	>[byte_151C32]
		.BYTE	>[byte_151C3A]
		.BYTE	>[byte_151C42]
		.BYTE	>[byte_151C4A]
		.BYTE	>[byte_151C52]
		.BYTE	>[byte_151C5A]
		.BYTE	>[byte_151C62]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151C6A]
		.BYTE	>[byte_151C72]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151C7A]
		.BYTE	>[byte_151C8C]
		.BYTE	>[byte_151C9E]
		.BYTE	>[byte_151CB0]
		.BYTE	>[byte_151CC2]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151CE2]
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151CF4]
		.BYTE	>[byte_151D06]
		.BYTE	>[byte_151D18]
		.BYTE	>[byte_151D2A]
		.BYTE	>[byte_151D3C]
		.BYTE	>[byte_151D4E]
		.BYTE	>[byte_151D60]
		.BYTE	>[byte_151D72]
		.BYTE	>[byte_151D84]
		.BYTE	>[byte_151D96]
		.BYTE	>[byte_151DA8]
		.BYTE	>[byte_151DBA]
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	0
		.BYTE	>[byte_151DCC]
		.BYTE	>[byte_151DCE]
		.BYTE	>[byte_151DD0]
		.BYTE	>[byte_151DD2]
		.BYTE	>[byte_151DD4]
		.BYTE	>[byte_151DD6]
		.BYTE	>[byte_151DD8]
		.BYTE	>[byte_151DDA]
		.BYTE	>[byte_151DDC]
		.BYTE	>[byte_151DDE]
		.BYTE	>[byte_151DE0]
		.BYTE	>[byte_151DE2]
		.BYTE	>[byte_151DE4]
		.BYTE	>[byte_151DE6]
		.BYTE	>[byte_151DE8]
		.BYTE	>[byte_151DEA]
		.BYTE	>[byte_151DEC]
		.BYTE	>[byte_151DEE]
		.BYTE	>[byte_151DF0]
		.BYTE	>[byte_151DF2]
		.BYTE	>[byte_151DF4]
		.BYTE	>[byte_151DF6]
		.BYTE	>[byte_151DF8]
		.BYTE	>[byte_151DFA]
		.BYTE	>[byte_151DFC]
		.BYTE	>[byte_151DFE]
		.BYTE	>[byte_151E00]
		.BYTE	>[byte_151E02]
		.BYTE	>[byte_151E04]
		.BYTE	>[byte_151E06]
		.BYTE	>[byte_151E08]
		.BYTE	>[byte_151E0A]
		.BYTE	>[byte_151E0C]
		.BYTE	>[byte_151E0E]
		.BYTE	>[byte_151E10]
		.BYTE	>[byte_151E12]
		.BYTE	>[byte_151E14]
		.BYTE	>[byte_151E18]
		.BYTE	>[byte_151E16]
		.BYTE	>[byte_151E1A]
		.BYTE	>[byte_151E1C]
		.BYTE	>[byte_151E1E]
		.BYTE	>[byte_151E20]
		.BYTE	>[byte_151E22]
		.BYTE	>[byte_151E24]
		.BYTE	>[byte_151E26]
		.BYTE	>[byte_151E28]
		.BYTE	>[byte_151E2A]
		.BYTE	>[byte_151E2C]
		.BYTE	>[byte_151E2E]
		.BYTE	>[byte_151E30]
		.BYTE	>[byte_151E32]
		.BYTE	>[byte_151E34]
		.BYTE	>[byte_151E36]
		.BYTE	>[byte_151E38]
		.BYTE	>[byte_151E3A]
		.BYTE	>[byte_151E3C]
		.BYTE	>[byte_151E3E]
		.BYTE	>[byte_151E40]
		.BYTE	>[byte_151E42]
		.BYTE	>[byte_151E44]
		.BYTE	>[byte_151E46]
		.BYTE	>[byte_151E48]
		.BYTE	>[byte_151E4A]
		.BYTE	>[byte_151E4C]
		.BYTE	>[byte_151E4E]
		.BYTE	>[byte_151E50]
		.BYTE	>[byte_151E52]
		.BYTE	>[byte_151E54]
		.BYTE	>[byte_151E56]
		.BYTE	>[byte_151E58]
		.BYTE	>[byte_151E5A]
		.BYTE	>[byte_151E5C]
		.BYTE	>[byte_151E5E]
		.BYTE	>[byte_151E60]
		.BYTE	>[byte_151E62]
		.BYTE	>[byte_151E64]
		.BYTE	>[byte_151E66]
		.BYTE	>[byte_151E68]
		.BYTE	>[byte_151E6A]
		.BYTE	>[byte_151E6C]
		.BYTE	>[byte_151E6E]
		.BYTE	>[byte_151E70]
		.BYTE	>[byte_151E72]
		.BYTE	>[byte_151E74]
		.BYTE	>[byte_151E76]
		.BYTE	>[byte_151E78]
		.BYTE	>[byte_151E7A]
		.BYTE	>[byte_151E7C]
		.BYTE	>[byte_151E7E]
		.BYTE	>[byte_151E80]
		.BYTE	>[byte_151E82]
		.BYTE	>[byte_151E84]
		.BYTE	>[byte_151E86]
		.BYTE	>[byte_151E88]
		.BYTE	>[byte_151E8A]
		.BYTE	>[byte_151E8C]
		.BYTE	>[byte_151E8E]
		.BYTE	>[byte_151E90]
		.BYTE	>[byte_151E92]
		.BYTE	>[byte_151E94]
		.BYTE	>[byte_151E96]
		.BYTE	>[byte_151E98]
		.BYTE	>[byte_151E9A]
		.BYTE	>[byte_151E9C]
		.BYTE	>[byte_151E9E]
		.BYTE	>[byte_151EA0]
		.BYTE	>[byte_151EA2]
		.BYTE	>[byte_151EA4]
		.BYTE	0
		.BYTE	>[byte_151E26]
byte_151A82:
		.BYTE	$80,$00,$80,$10,$80,$11,$80,$01
byte_151A8A:
		.BYTE	$80,$04,$80,$14,$80,$15,$80,$05
byte_151A92:
		.BYTE	$80,$02,$C0,$12,$C0,$13,$80,$03
byte_151A9A:
		.BYTE	$80,$06,$80,$16,$80,$17,$80,$07
byte_151AA2:
		.BYTE	$80,$08,$80,$18,$80,$19,$80,$09
byte_151AAA:
		.BYTE	$80,$0A,$80,$1A,$80,$1B,$80,$0B
byte_151AB2:
		.BYTE	$80,$0C,$80,$1C,$80,$1D,$80,$0D
byte_151ABA:
		.BYTE	$40,$0E,$80,$1E,$80,$1F,$80,$0F
byte_151AC2:
		.BYTE	$40,$20,$80,$30,$80,$31,$80,$21
byte_151ACA:
		.BYTE	$40,$22,$80,$32,$80,$33,$80,$23
byte_151AD2:
		.BYTE	$40,$24,$80,$34,$80,$35,$80,$25
byte_151ADA:
		.BYTE	$40,$26,$80,$36,$80,$37,$80,$27
byte_151AE2:
		.BYTE	$40,$28,$80,$38,$80,$39,$80,$29
byte_151AEA:
		.BYTE	$40,$2A,$80,$3A,$80,$3B,$80,$2B
byte_151AF2:
		.BYTE	$40,$2C,$80,$3C,$80,$3D,$80,$2D
byte_151AFA:
		.BYTE	$80,$2E,$80,$3E,$80,$3F,$80,$2F
byte_151B02:
		.BYTE	$80,$40,$80,$50,$80,$51,$80,$41
byte_151B0A:
		.BYTE	$80,$42,$80,$52,$80,$53,$80,$43
byte_151B12:
		.BYTE	$80,$44,$80,$54,$80,$55,$80,$45
byte_151B1A:
		.BYTE	$80,$46,$80,$56,$80,$57,$80,$47
byte_151B22:
		.BYTE	$80,$48,$80,$58,$80,$59,$80,$49
byte_151B2A:
		.BYTE	$80,$4A,$80,$5A,$80,$5B,$80,$4B
byte_151B32:
		.BYTE	$80,$4C,$80,$5C,$80,$5D,$80,$4D
byte_151B3A:
		.BYTE	$87,$A8,$87,$B8,$87,$B9,$87,$A9
byte_151B42:
		.BYTE	$87,$C8,$87,$D8,$87,$D9,$87,$C9
byte_151B4A:
		.BYTE	$81,$6C,$81,$7C,$81,$7D,$81,$6D
byte_151B52:
		.BYTE	$81,$6E,$81,$7E,$81,$7F,$81,$6F
byte_151B5A:
		.BYTE	$00,$4E,$00,$5E,$00,$5F,$00,$4F
byte_151B62:
		.BYTE	$00,$60,$00,$70,$00,$71,$00,$61
byte_151B6A:
		.BYTE	$00,$62,$00,$72,$00,$73,$00,$63
byte_151B72:
		.BYTE	$00,$64,$00,$74,$00,$75,$00,$65
byte_151B7A:
		.BYTE	$00,$66,$00,$76,$80,$77,$00,$67
byte_151B82:
		.BYTE	$00,$68,$80,$78,$80,$79,$00,$69
byte_151B8A:
		.BYTE	$00,$6A,$00,$7A,$00,$7B,$00,$6B
byte_151B92:
		.BYTE	$00,$6C,$40,$7C,$40,$7D,$00,$6D
byte_151B9A:
		.BYTE	$00,$6E,$80,$7E,$80,$7F,$00,$6F
byte_151BA2:
		.BYTE	$00,$80,$00,$90,$80,$91,$00,$81
byte_151BAA:
		.BYTE	$00,$82,$40,$92,$80,$93,$00,$83
byte_151BB2:
		.BYTE	$00,$84,$00,$94,$00,$95,$00,$85
byte_151BBA:
		.BYTE	$00,$86,$00,$96,$00,$97,$00,$87
byte_151BC2:
		.BYTE	$00,$88,$00,$98,$00,$99,$00,$89
byte_151BCA:
		.BYTE	$00,$8A,$80,$9A,$80,$9B,$00,$8B
byte_151BD2:
		.BYTE	$00,$8C,$80,$9C,$80,$9D,$00,$8D
byte_151BDA:
		.BYTE	$00,$8E,$00,$9E,$00,$9F,$00,$8F
byte_151BE2:
		.BYTE	$00,$A0,$00,$B0,$00,$B1,$00,$A1
byte_151BEA:
		.BYTE	$00,$A2,$80,$B2,$80,$B3,$00,$A3
byte_151BF2:
		.BYTE	$00,$A4,$80,$B4,$80,$B5,$00,$A5
byte_151BFA:
		.BYTE	$00,$A6,$80,$B6,$80,$B7,$00,$A7
byte_151C02:
		.BYTE	$07,$AA,$07,$BA,$07,$BB,$07,$AB
byte_151C0A:
		.BYTE	$07,$CA,$07,$DA,$07,$DB,$07,$CB
byte_151C12:
		.BYTE	$07,$88,$07,$98,$07,$99,$07,$89
byte_151C1A:
		.BYTE	$07,$8A,$07,$9A,$07,$9B,$07,$8B
byte_151C22:
		.BYTE	$40,$A8,$40,$B8,$40,$B9,$40,$A9
byte_151C2A:
		.BYTE	$80,$AA,$80,$BA,$80,$BB,$80,$AB
byte_151C32:
		.BYTE	$80,$AC,$80,$BC,$80,$BD,$80,$AD
byte_151C3A:
		.BYTE	$40,$AE,$80,$BE,$80,$BF,$40,$AF
byte_151C42:
		.BYTE	$40,$C0,$80,$D0,$80,$D1,$40,$C1
byte_151C4A:
		.BYTE	$80,$C2,$80,$D2,$80,$D3,$80,$C3
byte_151C52:
		.BYTE	$40,$C4,$80,$D4,$80,$D5,$40,$C5
byte_151C5A:
		.BYTE	$40,$C6,$80,$D6,$80,$D7,$40,$C7
byte_151C62:
		.BYTE	$40,$C8,$80,$D8,$80,$D9,$40,$C9
byte_151C6A:
		.BYTE	$00,$CC,$80,$DC,$80,$DD,$00,$CD
byte_151C72:
		.BYTE	$40,$CA,$80,$DA,$80,$DB,$80,$CB
byte_151C7A:
		.BYTE	$80,$EE,$80,$F1,$40,$F2,$40,$EF,$80,$F4,$80,$F5,$80,$F6,$80,$F3
		.BYTE	$40,$F0
byte_151C8C:
		.BYTE	$80,$F7,$80,$FA,$80,$FB,$80,$F8,$80,$FD,$80,$FE,$80,$FF,$80,$FC
		.BYTE	$80,$F9
byte_151C9E:
		.BYTE	$00,$E5,$00,$E8,$00,$E9,$00,$E6,$00,$EB,$00,$EC,$00,$ED,$00,$EA
		.BYTE	$00,$E7
byte_151CB0:
		.BYTE	$00,$CE,$00,$DF,$C0,$E0,$C0,$CF,$00,$E2,$C0,$E3,$00,$E4,$00,$E1
		.BYTE	$00,$DE
byte_151CC2:
		.BYTE	$01,$00,$01,$04,$01,$05,$01,$01,$81,$08,$81,$09,$81,$0A,$01,$06
		.BYTE	$01,$02,$41,$0C,$41,$0D,$81,$0E,$01,$0F,$01,$0B,$01,$07,$01,$03
byte_151CE2:
		.BYTE	$12,$03,$92,$13,$92,$14,$92,$04,$12,$23,$12,$24,$12,$25,$92,$15
		.BYTE	$92,$05
byte_151CF4:
		.BYTE	$92,$06,$92,$16,$92,$17,$D2,$07,$92,$26,$92,$27,$92,$28,$92,$18
		.BYTE	$D2,$08
byte_151D06:
		.BYTE	$41,$DE,$41,$DE,$41,$DE,$41,$DE,$41,$DE,$41,$DE,$41,$DE,$41,$DE
		.BYTE	$41,$DE
byte_151D18:
		.BYTE	$12,$A6,$12,$B6,$12,$B7,$12,$A7,$52,$C6,$12,$C7,$12,$C8,$12,$B8
		.BYTE	$12,$A8
byte_151D2A:
		.BYTE	$92,$A9,$92,$B9,$92,$BA,$92,$AA,$12,$C9,$92,$CA,$92,$CB,$92,$BB
		.BYTE	$92,$AB
byte_151D3C:
		.BYTE	$D2,$A0,$D2,$B0,$52,$B1,$52,$A1,$D2,$C0,$D2,$C1,$D2,$C2,$52,$B2
		.BYTE	$52,$A2
byte_151D4E:
		.BYTE	$D2,$A3,$52,$B3,$52,$B4,$D2,$A4,$52,$C3,$52,$C4,$D2,$C5,$52,$B5
		.BYTE	$D2,$A5
byte_151D60:
		.BYTE	$12,$D0,$12,$E0,$12,$E1,$12,$D1,$52,$F0,$52,$F1,$52,$F2,$12,$E2
		.BYTE	$12,$D2
byte_151D72:
		.BYTE	$D2,$00,$D2,$10,$12,$11,$12,$01,$12,$20,$12,$21,$D2,$22,$D2,$12
		.BYTE	$D2,$02
byte_151D84:
		.BYTE	$D2,$AC,$D2,$BC,$92,$BD,$92,$AD,$D2,$CC,$D2,$CD,$D2,$CE,$92,$BE
		.BYTE	$92,$AE
byte_151D96:
		.BYTE	$D2,$0C,$D2,$1C,$D2,$1D,$D2,$0D,$D2,$2C,$D2,$2D,$D2,$2E,$D2,$1E
		.BYTE	$D2,$0E
byte_151DA8:
		.BYTE	$12,$37,$12,$47,$12,$48,$12,$38,$12,$57,$12,$58,$12,$59,$12,$49
		.BYTE	$12,$39
byte_151DBA:
		.BYTE	$D2,$3A,$D2,$4A,$92,$4B,$D2,$3B,$D2,$5A,$D2,$5B,$D2,$5C,$D2,$4C
		.BYTE	$D2,$3C
byte_151DCC:
		.BYTE	$C1,$DD
byte_151DCE:
		.BYTE	$41,$11
byte_151DD0:
		.BYTE	$41,$20
byte_151DD2:
		.BYTE	$41,$10
byte_151DD4:
		.BYTE	$41,$13
byte_151DD6:
		.BYTE	$41,$30
byte_151DD8:
		.BYTE	$41,$33
byte_151DDA:
		.BYTE	$41,$21
byte_151DDC:
		.BYTE	$41,$23
byte_151DDE:
		.BYTE	$41,$12
byte_151DE0:
		.BYTE	$41,$32
byte_151DE2:
		.BYTE	$41,$22
byte_151DE4:
		.BYTE	$C1,$15
byte_151DE6:
		.BYTE	$C1,$24
byte_151DE8:
		.BYTE	$C1,$14
byte_151DEA:
		.BYTE	$C1,$17
byte_151DEC:
		.BYTE	$C1,$34
byte_151DEE:
		.BYTE	$C1,$37
byte_151DF0:
		.BYTE	$C1,$25
byte_151DF2:
		.BYTE	$C1,$27
byte_151DF4:
		.BYTE	$C1,$16
byte_151DF6:
		.BYTE	$C1,$36
byte_151DF8:
		.BYTE	$C1,$26
byte_151DFA:
		.BYTE	$C1,$19
byte_151DFC:
		.BYTE	$C1,$28
byte_151DFE:
		.BYTE	$C1,$18
byte_151E00:
		.BYTE	$C1,$1B
byte_151E02:
		.BYTE	$C1,$38
byte_151E04:
		.BYTE	$C1,$3B
byte_151E06:
		.BYTE	$C1,$29
byte_151E08:
		.BYTE	$C1,$2B
byte_151E0A:
		.BYTE	$C1,$1A
byte_151E0C:
		.BYTE	$C1,$3A
byte_151E0E:
		.BYTE	$C1,$2A
byte_151E10:
		.BYTE	$C1,$3D
byte_151E12:
		.BYTE	$C1,$E0
byte_151E14:
		.BYTE	$41,$5C
byte_151E16:
		.BYTE	$41,$2F
byte_151E18:
		.BYTE	$01,$1F
byte_151E1A:
		.BYTE	$41,$31
byte_151E1C:
		.BYTE	$41,$35
byte_151E1E:
		.BYTE	$41,$39
byte_151E20:
		.BYTE	$41,$3C
byte_151E22:
		.BYTE	$C1,$1C
byte_151E24:
		.BYTE	$C1,$2C
byte_151E26:
		.BYTE	$01,$DE
byte_151E28:
		.BYTE	$C1,$40
byte_151E2A:
		.BYTE	$C1,$41
byte_151E2C:
		.BYTE	$C1,$42
byte_151E2E:
		.BYTE	$C1,$50
byte_151E30:
		.BYTE	$C1,$52
byte_151E32:
		.BYTE	$C1,$60
byte_151E34:
		.BYTE	$C1,$61
byte_151E36:
		.BYTE	$C1,$62
byte_151E38:
		.BYTE	$C1,$43
byte_151E3A:
		.BYTE	$C1,$44
byte_151E3C:
		.BYTE	$C1,$45
byte_151E3E:
		.BYTE	$C1,$53
byte_151E40:
		.BYTE	$C1,$55
byte_151E42:
		.BYTE	$C1,$63
byte_151E44:
		.BYTE	$C1,$64
byte_151E46:
		.BYTE	$C1,$65
byte_151E48:
		.BYTE	$C1,$46
byte_151E4A:
		.BYTE	$C1,$47
byte_151E4C:
		.BYTE	$C1,$48
byte_151E4E:
		.BYTE	$C1,$56
byte_151E50:
		.BYTE	$C1,$57
byte_151E52:
		.BYTE	$C1,$58
byte_151E54:
		.BYTE	$C1,$66
byte_151E56:
		.BYTE	$C1,$67
byte_151E58:
		.BYTE	$C1,$68
byte_151E5A:
		.BYTE	$C1,$49
byte_151E5C:
		.BYTE	$C1,$4A
byte_151E5E:
		.BYTE	$C1,$4B
byte_151E60:
		.BYTE	$C1,$59
byte_151E62:
		.BYTE	$C1,$5A
byte_151E64:
		.BYTE	$C1,$5B
byte_151E66:
		.BYTE	$C1,$69
byte_151E68:
		.BYTE	$C1,$6A
byte_151E6A:
		.BYTE	$C1,$6B
byte_151E6C:
		.BYTE	$07,$69
byte_151E6E:
		.BYTE	$07,$6E
byte_151E70:
		.BYTE	$01,$2D
byte_151E72:
		.BYTE	$01,$2E
byte_151E74:
		.BYTE	$01,$1D
byte_151E76:
		.BYTE	$01,$1E
byte_151E78:
		.BYTE	$81,$3F
byte_151E7A:
		.BYTE	$41,$4C
byte_151E7C:
		.BYTE	$41,$4D
byte_151E7E:
		.BYTE	$41,$4E
byte_151E80:
		.BYTE	$41,$4F
byte_151E82:
		.BYTE	$07,$69
byte_151E84:
		.BYTE	$07,$6E
byte_151E86:
		.BYTE	$07,$5C
byte_151E88:
		.BYTE	$07,$5E
byte_151E8A:
		.BYTE	$07,$5D
byte_151E8C:
		.BYTE	$07,$6A
byte_151E8E:
		.BYTE	$07,$5C
byte_151E90:
		.BYTE	$07,$5E
byte_151E92:
		.BYTE	$07,$6B
byte_151E94:
		.BYTE	$07,$6C
byte_151E96:
		.BYTE	$07,$7C
byte_151E98:
		.BYTE	$07,$7D
byte_151E9A:
		.BYTE	$07,$7E
byte_151E9C:
		.BYTE	$07,$E9
byte_151E9E:
		.BYTE	$07,$7C
byte_151EA0:
		.BYTE	$07,$7D
byte_151EA2:
		.BYTE	$07,$EA
byte_151EA4:
		.BYTE	$07,$EB

; =============== S U B	R O U T	I N E =======================================
_map_scroll_mode_minimap_draw:
		LDA		#$07
		STA		_spr_insert_args._idx
		LDA		_cur_map_wnd_pos._COL
		LSR
		CLC
		ADC		#$0C
		STA		_spr_insert_args._pos._COL
		LDA		_cur_map_wnd_pos._ROW
		LSR
		CLC
		ADC		#$53
		STA		_spr_insert_args._pos._ROW
		JSR		_spr_lib_attr_insert
		LDA		#$06
		STA		_spr_insert_args._idx
		LDA		#$08
		STA		_spr_insert_args._pos._COL
		LDA		#$48
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._attr
		JMP		_spr_lib_attr_insert

; =============== S U B	R O U T	I N E =======================================
_map_scroll_mode_arrows_draw:
		JSR		_get_zero
		LDY		#$03
loc_151ED5:
		LSR
		PHA
		BCC		loc_151EE6
		BCS		loc_151EE0
loc_151EDB:
		PLA
		DEY
		BPL		loc_151ED5
		RTS
loc_151EE0:
		LDA		_frames_counter
		AND		#$20
		BEQ		loc_151EDB
loc_151EE6:
		TYA
		PHA
		CLC
		ADC		#$08
		STA		_spr_insert_args._idx
		LDA		_map_scroll_arrows_posX_list,Y
		STA		_spr_insert_args._pos._COL
		LDA		_map_scroll_arrows_posY_list,Y
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._attr
		JSR		_spr_lib_attr_insert
		PLA
		TAY
		JMP		loc_151EDB
_map_scroll_arrows_posX_list:
		.BYTE	$8C,$8C,$38,$E0
_map_scroll_arrows_posY_list:
		.BYTE	$38,$C8,$84,$84

; =============== S U B	R O U T	I N E =======================================
_get_zero:
		LDA		#$00
		RTS

; =============== S U B	R O U T	I N E =======================================
_scroll_msg_reset:
		LDA		#$00
		STA		_scroll_msg_active_flag
		STA		_scroll_msg_start_pos
		STA		_scroll_msg_shift_pos
		LDA		#$FF
		STA		_scroll_msg_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_hud_full_draw:
		JSR		_render_off
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		LDA		#$02
		STA		_MMC5_CHR_MODE
		JSR		_ppu_flush_wait
		JSR		sub_136761
		JSR		_ppu_flush_wait
		JSR		sub_13671F
		LDA		#$03
		STA		byte_3C
loc_151F3B:
		JSR		_ppu_flush_wait
		JSR		_constr_palette_redraw
		LDA		_constr_palette_redraw_req
		BNE		loc_151F3B
		JSR		_ppu_flush_wait
		JSR		_building_name_and_cost_hud_draw
		JSR		_RCI_meters_draw
;		FJSRA	_hud_date_draw,PRG7,PRG3
		JSR		_hud_date_draw
;		FJSRA	_hud_money_draw,PRGC,PRG3
		JSR		_hud_money_draw
;		FJSRA	_hud_popul_draw,PRG7,PRG3
		JSR		_hud_popul_draw
;		FJSRA	_hud_city_name_draw,PRG7,PRG3
		JSR		_hud_city_name_draw
		JSR		_ppu_flush_wait
loc_151F6D:
		JSR		_ppu_flush_wait
		JSR		sub_137CD4
		LDA		byte_53
		BNE		loc_151F6D
		LDA		#$01
		STA		_MMC5_CHR_MODE
		LDA		#$00
		STA		_mmc5_operation_in_progress_flag
		JMP		_render_on


; =============== S U B	R O U T	I N E =======================================
_ppu_flush_wait:
		JSR		_ppu_dequeue					; TODO: merge me
		JSR		_extnt_dequeue
		JSR		_ppu_queue_empty_test_0
		BCS		_ppu_flush_wait
		RTS

; =============== S U B	R O U T	I N E =======================================
_far_heavy_traffic_report_test:
		LDA		_frames_counter
		BNE		locret_151FA1
		LDX		_obj_pos._heli._COL
		LDY		_obj_pos._heli._ROW
		FJSRA	_heavy_traffic_report_test,SRAM,PRGB
locret_151FA1:
		RTS

; FIX: moved here from bank3 to bank it belongs
; now all calls are local here
;
; =============== S U B	R O U T	I N E =======================================
_hud_money_draw:
		JSR		_hud_money_to_str
;		JSR		_hud_money_pos_calc				; OPTIMIZED, merged
		LDA		#$16							; -
		STA		_tmp2							; -
		LDA		#$1C							; -
		STA		_tmp3							; -
		LDA		#$08							; -
		STA		_tmp4							; -
		JMP		_hud_str_pos_calc_and_queue		; -
;		JSR		_pre_buf_str_queue				; -
;		RTS										; -

; =============== S U B	R O U T	I N E =======================================
_hud_money_to_str:
		MOVD	_ptr0,_city._money
		LDA		#$00
		STA		_tmp3							; tmp flag
		LDY		#$01							; start writing number in buf +1 pos
		LDX		#$06							; start with highest digit
		LDA		#$DF
		STA		_pre_ppu_buf					; fill leftmost byte with space or maybe $ later
loc_58026:
		JSR		_get_dec_digit24				; extract decimal digit
		CMP		#$00
		BNE		loc_5803C						; if no-zero, put a number
		LDA		_tmp3							; if zero, then there are two opts
		BNE		loc_5804F						; opt 1 flag clear
		CPX		#$00							; here two more opts
		BEQ		loc_5803C						; if last digit, put it as zero (number 0)
		LDA		#$DF							; or else trim it, becaue this is not a number still
		STA		_pre_ppu_buf,Y
		BNE		loc_5804A
loc_5803C:
		STA		_pre_ppu_buf,Y					; store non-zero number
		LDA		_tmp3
		BNE		loc_5804A						; if tmp flag set, we already done this
		LDA		#$30							; if flag clear, then this is first
		STA		_pre_ppu_buf-1,Y				; non zero letter, set $ symbol before it
		DEC		_tmp3							; raise flag, we need this just once
loc_5804A:
		INY
		DEX
		BPL		loc_58026						; cycle
		RTS
loc_5804F:
		LDA		#$00							; opt 2 flag already set, so we have
		STA		_pre_ppu_buf,Y					; zero inside number, put it as is
		BEQ		loc_5804A

; =============== S U B	R O U T	I N E =======================================
;_hud_money_pos_calc:
;		CIADDB	_tmp1,_cur_screen_tile._ROW,#$1C,NONE,#$1E
;		CIADDB	_tmp0,_cur_screen_tile._COL,#$16,NONE,#$20
;		WRAPA	_tmp2,_tmp3,#$08,#$20
;		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#$00
;		RTS

; REDUNDANT, as soon as all three routines here appears exactly the same after
; rearranged some constants and vars used, we can merge them and use only single
; copy without any harm
;
; =============== S U B	R O U T	I N E =======================================
;_hud_money_str_queue:
;		PPUQSTART
;		LDY		#$00
;		PPUQMOVB	_ptr0,OP_NOP,0
;loc_5810F:
;		PPUQMOVB	_ptr0+1,OP_ORA,#$20
;		PPUQMOVB	_tmp2,OP_NOP,0
;loc_58110:
;		LDA		_pre_ppu_buf,Y
;		INY
;		PPUQMOVA
;		DEC		_tmp2
;		BNE		loc_58110
;		LDA		_tmp3
;		BEQ		loc_5813D
;		STA		_tmp2
;		LDA		#$00
;		STA		_tmp3
;		PPUQMOVB	_ptr0,OP_AND,#$E0
;		JMP		loc_5810F
;loc_5813D:
;		PPUQEND
;		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_date_draw:
		JSR		_hud_date_to_str
;		JSR		_hud_date_pos_calc					; OPTIMIZED, merged
		LDA		#$16
		STA		_tmp2
		LDA		#$1A
		STA		_tmp3
		LDA		#$08
		STA		_tmp4
		JMP		_hud_str_pos_calc_and_queue
;		JSR		_pre_buf_str_queue
;		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_date_to_str:
		LDX		#$00
		JSR		_hud_year_str_queue
		LDA		#$DF
		STA		_pre_ppu_buf,X
		INX
		JMP		_hud_month_str_queue


; =============== S U B	R O U T	I N E =======================================
_hud_year_str_queue:
		LDA		_city._cur_year
		LDY		_city._cur_year+1
		STA		_tmp0
		STY		_tmp1
		LDA		#$04
		STA		_tmp2
;		LDY		#$00						; FIX similar to the rest of the code
		LDY		#$03						; -
loc_58DE7:
		JSR		_get_dec_digit16
		STA		_pre_ppu_buf,X
		INX
;		INY									; FIX
;		INY									; -
		DEY									; -
		DEC		_tmp2
		BNE		loc_58DE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_dec_digit16:
		LDA		#$00
		STA		_tmp3
loc_58DF9:
;		LDA		_tmp0						; FIX
;		SEC
;		SBC		_dec_digit16_tbl,Y
;		STA		_tmp0
;		LDA		_tmp1
;		SBC		_dec_digit16_tbl+1,Y
;		STA		_tmp1
;		BCC		loc_58E0E
		LDA		_tmp0						; OPTIMIZED
		CMP		_dec_digit_tbl0,Y			; -
		LDA		_tmp1						; -
		SBC		_dec_digit_tbl1,Y			; -
		BCC		loc_58E0E
		LDA		_tmp0						; FIX, we has to sub here
		SBC		_dec_digit_tbl0,Y			; also C already set as well
		STA		_tmp0
		LDA		_tmp1
		SBC		_dec_digit_tbl1,Y
		STA		_tmp1
		INC		_tmp3
		BNE		loc_58DF9
loc_58E0E:
;		LDA		_tmp0						; REDUNDANT
;		CLC
;		ADC		_dec_digit16_tbl,Y
;		STA		_tmp0
;		LDA		_tmp1
;		ADC		_dec_digit16_tbl+1,Y
;		STA		_tmp1
		LDA		_tmp3
		RTS

;_dec_digit16_tbl:							; REDUNDANT
;		.WORD	1000
;		.WORD	100
;		.WORD	10
;		.WORD	1

; =============== S U B	R O U T	I N E =======================================
; TODO: bank 9 different type of month display lib
;
_hud_month_str_queue:
		LDA		_city._cur_month
		STA		_ptr0
		ASL
		ASL
;		CLC							; FIX: now entry size always 4
;		ADC		_ptr0
		TAY
		LDA		#$03
		STA		_ptr0
loc_58E2E:
		LDA		_months_lib,Y
		INY
		STA		_pre_ppu_buf,X
		INX
		DEC		_ptr0
		BNE		loc_58E2E
		RTS

; REDUNDANT, merger in system bank
;_months_lib:
;		.BYTE	_J,_A,_N
;		.BYTE	_F,_E,_B
;		.BYTE	_M,_A,_R
;		.BYTE	_A,_P,_R
;		.BYTE	_M,_A,_Y
;		.BYTE	_J,_U,_N
;		.BYTE	_J,_U,_L
;		.BYTE	_A,_U,_G
;		.BYTE	_S,_E,_P
;		.BYTE	_O,_C,_T
;		.BYTE	_N,_O,_V
;		.BYTE	_D,_E,_C

; =============== S U B	R O U T	I N E =======================================
_hud_date_pos_calc:
;		CIADDB	_tmp1,_cur_screen_tile._ROW,#$1A,NONE,#$1E
;		CIADDB	_tmp0,_cur_screen_tile._COL,#$16,NONE,#$20
;		WRAPA	_tmp2,_tmp3,#$08,#$20
;		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#$00			; OPTIMIZED, merged
;		RTS

; REDUNDANT, the same for this one
;
; =============== S U B	R O U T	I N E =======================================
;_hud_date_str_queue:
;		PPUQSTART
;		LDY		#$00
;		LDA		_tmp2								; REDUNDANT, this is a leftover
;		STA		_tmp4								; from using an extnt draw routine
;		LDA		_tmp3								; below, to keep vars for it
;		STA		_tmp5								; as soon as in unused, we don't need
;		PPUQMOVB	_ptr0,OP_NOP,0					; these vars as well...
;loc_58ED9:
;		PPUQMOVB	_ptr0+1,OP_ORA,#$20
;		PPUQMOVB	_tmp2,OP_NOP,0
;loc_58EDA:
;		LDA		_pre_ppu_buf,Y
;		INY
;		PPUQMOVA
;		DEC		_tmp2
;		BNE		loc_58EDA
;		LDA		_tmp3
;		BEQ		loc_58F09
;		STA		_tmp2
;		LDA		#$00
;		STA		_tmp3
;		PPUQMOVB	_ptr0,OP_AND,#$E0
;		JMP		loc_58ED9
;loc_58F09:
;		PPUQEND
;		RTS

; =============== S U B	R O U T	I N E =======================================
; unused/unref by the game, they do this somewhere else at once for all hud
; no need to put extnt here at all
;
;_cur_date_extnt_queue_unref:
;		EXTQSTART
;		LDA		_ptr0
;		STA		_extnt_queue_buf,X
;		INX
;		LDA		_ptr0+1
;		ORA		#>[_MMC5_EXRAM]
;		STA		_extnt_queue_buf,X
;		INX
;		LDA		_tmp2
;		STA		_extnt_queue_buf,X
;		INX
;loc_58F22:
;		LDA		#$02
;		STA		_extnt_queue_buf,X
;		INX
;		DEC		_tmp2
;		BNE		loc_58F22
;		LDA		_tmp3
;		BEQ		loc_58F4F
;		STA		_tmp2
;		LDA		#$00
;		STA		_tmp3
;		LDA		_ptr0
;		AND		#$E0
;		STA		_extnt_queue_buf,X
;		INX
;		LDA		_ptr0+1
;		ORA		#>[_MMC5_EXRAM]
;		STA		_extnt_queue_buf,X
;		INX
;		LDA		_tmp2
;		STA		_extnt_queue_buf,X
;		INX
;		JMP		loc_58F22
;loc_58F4F:
;		EXTQEND
;		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_popul_draw:
		JSR		_hud_popul_to_str
;		JSR		_hud_popul_pos_calc					; OPTIMIZED, merged
		LDA		#$0E
		STA		_tmp2
		LDA		#$1A
		STA		_tmp3
		LDA		#$06
		STA		_tmp4
		JMP		_hud_str_pos_calc_and_queue
;		JSR		_pre_buf_str_queue
;		RTS

; =============== S U B	R O U T	I N E =======================================
; much simpler than money calc routine, no need to put extra symbols anywhere
; so you just need to track the first non-zero digit to trim first zeroes
;
_hud_popul_to_str:
		MOVD	_ptr0,_popul_cur
		LDA		#$00
		STA		_tmp3
		LDY		#$00
		LDX		#$05
loc_59A88:
		JSR		_get_dec_digit24
		CMP		#$00
		BNE		loc_59A9E
		LDA		_tmp3
		BNE		loc_59AA8
		CPX		#$00
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
		LDA		#$00
		STA		_pre_ppu_buf,Y
		BEQ		loc_59AA3

; =============== S U B	R O U T	I N E =======================================
_get_dec_digit24:
		LDA		#$00
		STA		_tmp4
loc_5805A:
;		SEC									; FIX, lets use less opcodes
;		LDA		_tmp0
;		SBC		_dec_digit_tbl0,X
;		STA		_tmp0
;		LDA		_tmp1
;		SBC		_dec_digit_tbl1,X
;		STA		_tmp1
;		LDA		_tmp2
;		SBC		_dec_digit_tbl2,X
;		STA		_tmp2
		LDA		_tmp0						; OPTIMIZED
		CMP		_dec_digit_tbl0,X			; -
		LDA		_tmp1						; -
		SBC		_dec_digit_tbl1,X			; -
		LDA		_tmp2						; -
		SBC		_dec_digit_tbl2,X			; -
		BCC		loc_58076
		LDA		_tmp0						; FIX, we has to sub here
		SBC		_dec_digit_tbl0,X			; also C already set as well
		STA		_tmp0
		LDA		_tmp1
		SBC		_dec_digit_tbl1,X
		STA		_tmp1
		LDA		_tmp2
		SBC		_dec_digit_tbl2,X
		STA		_tmp2
		INC		_tmp4
		BNE		loc_5805A
loc_58076:
;		CLC									; REDUNDANT
;		LDA		_tmp0
;		ADC		_dec_digit_tbl0,X
;		STA		_tmp0
;		LDA		_tmp1
;		ADC		_dec_digit_tbl1,X
;		STA		_tmp1
;		LDA		_tmp2
;		ADC		_dec_digit_tbl2,X
;		STA		_tmp2
		LDA		_tmp4
		RTS

; FIX: moved here from bank7, now almost all routines are in it's
; home, already reformatted to use common function with other functions
;
; =============== S U B	R O U T	I N E =======================================
_hud_city_name_draw:
		JSR		_hud_city_name_to_str
		LDA		#$02							; -
		STA		_tmp2							; -
		LDA		#$1A							; -
		STA		_tmp3							; -
		LDA		#$0A							; -
		STA		_tmp4							; -
		JMP		_hud_str_pos_calc_and_queue		; -

; =============== S U B	R O U T	I N E =======================================
;_hud_city_name_draw:
;		JSR		_get_city_name_str
;		JSR		_hud_city_name_pos_set
;		JSR		_hud_city_name_str_queue
;		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_city_name_to_str:
		LDA		#$0A
		STA		_tmp2
		LDX		_city._name
		DEX
		MOVWO	word_DA,_city._name+1
		LDY		#$00
loc_B8086:
		CPX		#$00
		BNE		loc_B808F
		LDA		#$20
		BNE		loc_B8095
loc_B808F:
		LDA		(word_DA),Y
		DEX
loc_B8095:
		STA		_pre_ppu_buf,Y
		INY
		DEC		_tmp2
		BNE		loc_B8086
		RTS

; =============== S U B	R O U T	I N E =======================================
;_hud_city_name_to_str:
;		LDX		_city._name
;		DEX
;		STX		byte_51A
;		MOVWO	word_DA,_city._name+1
;		RTS

; =============== S U B	R O U T	I N E =======================================
;_hud_city_name_pos_set:
;		CIADDB	_tmp1,_cur_screen_tile._ROW,#$1A,NONE,#$1E
;		CIADDB	_tmp0,_cur_screen_tile._COL,#$02,NONE,#$20
;		WRAPA	_tmp2,_tmp3,#$0A,#$20
;		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#$20	; DST=(_tmp1*#$20+_tmp0)+#$2000
;		RTS

; =============== S U B	R O U T	I N E =======================================
;_hud_city_name_str_queue:
;		PPUQSTART
;		LDY		#$00
;		PPUQMOVB	_ptr0,OP_NOP,0
;		PPUQMOVB	_ptr0+1,OP_NOP,0
;		PPUQMOVB	_tmp2,OP_NOP,0
;loc_B8086:
;		LDA		byte_51A
;		BNE		loc_B808F
;		LDA		#$20
;		BNE		loc_B8095
;loc_B808F:
;		LDA		(word_DA),Y
;		DEC		byte_51A
;		INY
;loc_B8095:
;		PPUQMOVA
;		DEC		_tmp2
;		BNE		loc_B8086
;		LDA		_tmp3
;		BEQ		loc_B80BE
;		STA		_tmp2
;		LDA		#$00
;		STA		_tmp3
;		PPUQMOVB	_ptr0,OP_AND,#$E0
;		PPUQMOVB	_ptr0+1,OP_NOP,0
;		PPUQMOVB	_tmp2,OP_NOP,0
;		JMP		loc_B8086
;loc_B80BE:
;		PPUQEND
;		RTS

; REDUNDANT, merged to one common tables set
; in system bank
;
;_dec_digit_tbl0:
;		.BYTE	[[1 >> 0] & $FF]		; $01
;		.BYTE	[[10 >> 0] & $FF]		; $0A
;		.BYTE	[[100 >> 0] & $FF]		; $64
;		.BYTE	[[1000 >> 0] & $FF]		; $E8
;		.BYTE	[[10000 >> 0] & $FF]	; $10
;		.BYTE	[[100000 >> 0] & $FF]	; $A0
;		.BYTE	[[1000000 >> 0] & $FF]	; $40
;_dec_digit_tbl1:
;		.BYTE	[[1 >> 8] & $FF]		; $00
;		.BYTE	[[10 >> 8] & $FF]		; $00
;		.BYTE	[[100 >> 8] & $FF]		; $00
;		.BYTE	[[1000 >> 8] & $FF]		; $03
;		.BYTE	[[10000 >> 8] & $FF]	; $27
;		.BYTE	[[100000 >> 8] & $FF]	; $86
;		.BYTE	[[1000000 >> 8] & $FF]	; $42
;_dec_digit_tbl2:
;		.BYTE	[[1 >> 16] & $FF]		; $00
;		.BYTE	[[10 >> 16] & $FF]		; $00
;		.BYTE	[[100 >> 16] & $FF]		; $00
;		.BYTE	[[1000 >> 16] & $FF]	; $00
;		.BYTE	[[10000 >> 16] & $FF]	; $00
;		.BYTE	[[100000 >> 16] & $FF]	; $01
;		.BYTE	[[1000000 >> 16] & $FF] ; $0F

; =============== S U B	R O U T	I N E =======================================
;_hud_popul_pos_calc:
;		CIADDB	_tmp1,_cur_screen_tile._ROW,#$1A,NONE,#$1E
;		CIADDB	_tmp0,_cur_screen_tile._COL,#$0E,NONE,#$20
;		WRAPA	_tmp2,_tmp3,#$06,#$20
;		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#$00	; DST=(_tmp1*#$20+_tmp0)+#$2000
;		RTS

; =============== S U B	R O U T	I N E =======================================
; let's make it common for all three pos calc routines as well
; they are the same except position constants, and as soon as all
; thre pair of routines uses both in row, let's combine them in one
; sub as well.
;
; ppuY =  ((scrollX / 2) + screenX) % 32
; ppuX =  ((scrollY / 2) + screenY) % 30
; ppuLen =  (ppuX < 32) ? lineLen : (ppuX - 32 - 1)
; wrapLen =  (ppuX < 32) ? 0 : (lineLen - ppuLen)
; ppuOfs  = ((ppuY * 32) + ppuX) + 0x2000
;
_hud_str_pos_calc_and_queue:
		CIADDB	_tmp1,_cur_screen_tile._ROW,_tmp3,NONE,#$1E
		CIADDB	_tmp0,_cur_screen_tile._COL,_tmp2,NONE,#$20
		WRAPB	_tmp2,_tmp3,_tmp0,_tmp4,#$20
		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#$00	; DST=(_tmp1*#$20+_tmp0)+#$2000
		PPUQSTART
		LDY		#$00
		PPUQMOVB	_ptr0,OP_NOP,0
loc_59B65:
		PPUQMOVB	_ptr0+1,OP_ORA,#$20
		PPUQMOVB	_tmp2,OP_NOP,0
loc_59B66:
		LDA		_pre_ppu_buf,Y
		INY
		PPUQMOVA
		DEC		_tmp2
		BNE		loc_59B66
		DO_WRAP	_tmp2,_tmp3,loc_59B93
		PPUQMOVB	_ptr0,OP_AND,#$E0
		JMP		loc_59B65
loc_59B93:
		PPUQEND
		RTS

		BANK_END FD,$C000
