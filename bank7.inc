
		ORG $8000

		BANK_START $F7

		SECTION_START C03

; =============== S U B	R O U T	I N E =======================================
_constr_palette_state_update:
		LDA		#$00
		STA		_tmp0
		STA		_tmp1
		STA		_tmp2
		STA		_tmp3
		TAX
.pal_loop:
		JSR		_test_if_city_money_enough_to_build	; C=0 if available
		PHP
		ROL		_tmp0							; shift value
		ROL		_tmp1							; 0 - enabled, 1 - blocked
		ROL		_tmp2
		ROL		_tmp3
		PLP
		BCC		.pal_enabled
		CLC
		BCC		.push_activity_flag
.pal_enabled:
		CPX		_cur_const_palette_selected_idx	; if current icon isn't pressed
		BNE		.push_no_active					; then push second bit 0
		SEC
		BCS		.push_activity_flag				; push 1 otherwise
.push_no_active:
		CLC
.push_activity_flag:
		ROL		_tmp0							; now we have pairs of bits for every
		ROL		_tmp1							; constr button: 10 - blocked
		ROL		_tmp2							; 00 - enabled, not pressed
		ROL		_tmp3							; 01 - enabled, pressed
		INX
		CPX		#$0E
		BCC		.pal_loop						; 14 items total, including prize ones
		LDA		_city._presents_list			; is a special case
		ORA		_city._presents_list+1
		ORA		_city._presents_list+2
		ORA		_city._presents_list+3			; test if any
		BEQ		.no_prizes
		LDA		_tmp0							; force enable the prize button
		AND		#$FD
		STA		_tmp0
		JMP		loc_B8110
.no_prizes:
		LDA		_tmp0							; or force disable
		ORA		#$02
		STA		_tmp0
loc_B8110:
		LDA		_tmp0
		CMP		_constr_palette_flags			; test with already calculated bits before
		BNE		.pal_changed
		LDA		_tmp1
		CMP		_constr_palette_flags+1
		BNE		.pal_changed
		LDA		_tmp2
		CMP		_constr_palette_flags+2
		BNE		.pal_changed
		LDA		_tmp3
		CMP		_constr_palette_flags+3
		BNE		.pal_changed
		RTS
.pal_changed:
		LDA		_tmp0							; if changed, apply settings
		STA		_constr_palette_flags
		LDA		_tmp1
		STA		_constr_palette_flags+1
		LDA		_tmp2
		STA		_constr_palette_flags+2
		LDA		_tmp3
		STA		_constr_palette_flags+3
		LDA		#$FF							; set flag to update
		STA		_constr_palette_redraw_req
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_if_city_money_enough_to_build:
		LDA		_city._money+2					; more than needed
		BNE		loc_B815D
		TXA										; backup X reg
		PHA
		ASL
		TAX
		LDA		_buildings_prices_list+1,X		; compare for cur building
		CMP		_city._money+1
		BCC		loc_B815B
		BNE		loc_B815F
		LDA		_buildings_prices_list,X
		CMP		_city._money
		BCC		loc_B815B
		BNE		loc_B815F
loc_B815B:
		PLA										; restore X reg
		TAX
loc_B815D:
		CLC										; money enough C=0
		RTS
loc_B815F:
		PLA										; restore X reg as well
		TAX
		SEC										; money not enough C=1
		RTS

; REDUNDANT, duplicated list of building costs here
;
;_buildings_prices_list:
;		.WORD	1
;		.WORD	10
;		.WORD	5
;		.WORD	20
;		.WORD	10
;		.WORD	100
;		.WORD	100
;		.WORD	100
;		.WORD	500
;		.WORD	500
;		.WORD	3000
;		.WORD	3000
;		.WORD	5000
;		.WORD	0								; <- here is some trick to be sure the prize
;		.WORD	5000							; button won't be disabled when no money
;		.WORD	10000							; now we disabled it because using the full table
												; no money - no prizes, get off

; =============== S U B	R O U T	I N E =======================================
; input: _cur_build_sample_tile_idx - current tile "sample" under construction cursor.
; output: C=1    - can't build here
;		  C=0    - success
;		  _cur_build_store_tile_idx - index of tile to build
;
_get_valid_1X1_building_tile:
		LDA		_cur_build_sample_tile_idx
		CMP		#_MAP_GROUND0
		BEQ		.do_money_sub				; regular buldozered ground at this point
		CMP		#_MAP_WATER
		BEQ		.j_water_tile_build_select	; special case water, no need to bulldozer
		CMP		#_MAP_WATER_ROUTE
		BNE		loc_B8194					; special water tile HERE!
;		SEC									; water roads! should be something special
;		BCS		.j_water_tile_build_select	; but C=1 means nothing currently there
; -
.j_water_tile_build_select:
		JMP		_water_tile_build_select
loc_B8194:
		LDA		_city._game_flags			; test autobuldozer flag
		LSR
		BCC		.no_auto_bulldoze
		LDA		_cur_build_sample_tile_idx	; check for what we can bulldoze
		CMP		#_MAP_GRASS
		BCC		.no_auto_bulldoze			; can't autobulldoze any normal buildings
		CMP		#_MAP_FLOOD
		BCC		.do_auto_bulldoze			; can autobulldoze parks or corrupted grounds
		CMP		#_MAP_SHORE0
		BCC		.no_auto_bulldoze			; can't autobulldoze flood/pollution, other transport systems
		CMP		#_MAP_WATER_ROAD_LR
		BCC		.do_auto_bulldoze			; can autobulldoze sea shores
		CMP		#_MAP_CORRUPT0
		BCC		.no_auto_bulldoze			; more transport tiles
		CMP		#_MAP_BRIDGE_WATER_LR
		BCS		.no_auto_bulldoze			; the rest of transport tiles
.do_auto_bulldoze:
		INCW	_cur_building_price			; yes, autobulldoze it
.do_money_sub:
		JSR		_money_subtract				; charge money
		BCS		locret_B81C4				; can't build something is low money
		LDA		#$FF						; common flag to signal to destroy something
		STA		_cur_build_store_tile_idx	; then build a tile of the default type (horizontal)
		CLC									; the actual bending/joints will be done later
locret_B81C4:
		RTS
.no_auto_bulldoze:
		LDA		_const_palette_selected_idx	; when no autobulldoze, we couldn't build anything
		CMP		#$01						; except special cases of intersections, let's find any
		BNE		loc_B81D1
		JMP		_select_road_intersection_tile
loc_B81D1:
		CMP		#$03
		BNE		loc_B81D8
		JMP		_select_rails_intersection_tile
loc_B81D8:
		JMP		_select_electro_intersection_tile

; =============== S U B	R O U T	I N E =======================================
_select_road_intersection_tile:
		LDA		_cur_build_sample_tile_idx	; special intersection tiles select
		CMP		#_MAP_RAIL_LR
		BEQ		.build_storeB8
;		BNE		loc_B81E5					; REDUNDANT
;		LDA		#_MAP_ROAD_RAIL_UD			; road+rail horizontal
;		BNE		.build_store1
loc_B81E5:
		CMP		#_MAP_RAIL_UD
		BEQ		.build_storeB7				; OPTIMIZED
;		BNE		loc_B81ED					; REDUNDANT
;		LDA		#_MAP_ROAD_RAIL_LR			; road+rail vertical
;		BNE		.build_store1
loc_B81ED:
		CMP		#_MAP_ELECTRO_LR
		BEQ		.build_storeB9				; OPTIMIZED
;		BNE		loc_B81F5					; REDUNDANT
;		LDA		#_MAP_ELECTRO_ROAD_LR		; road+electric horizontal
;		BNE		.build_store1
loc_B81F5:
		CMP		#_MAP_ELECTRO_UD
		BEQ		.build_storeBA				; OPTIMIZED
;		BEQ		loc_B81FC					; REDUNDANT
		BNE		.build_forbidden0			; nothing to build
;loc_B81FC:
;		LDA		#_MAP_ELECTRO_ROAD_UD		; road+electric vertical
;		BNE		.build_store1

; =============== S U B	R O U T	I N E =======================================
_select_rails_intersection_tile:
		LDA		_cur_build_sample_tile_idx
		CMP		#_MAP_ROAD_LR
		BEQ		.build_storeB7				; OPTIMIZED
;		BNE		loc_B820A					; REDUNDANT
;		LDA		#_MAP_ROAD_RAIL_LR			; rail+road horizontal
;		BNE		.build_store1
loc_B820A:
		CMP		#_MAP_ROAD_UD
		BEQ		.build_storeB8				; OPTIMIZED
;		BNE		loc_B8212					; REDUNDANT
;		LDA		#_MAP_ROAD_RAIL_UD			; rail+road vertical
;		BNE		.build_store1
loc_B8212:
		CMP		#_MAP_ELECTRO_LR
		BEQ		.build_storeBB				; OPTIMIZED
;		BNE		loc_B821A					; REDUNDANT
;		LDA		#_MAP_ELECTRO_RAIL_LR		; rail+electro horizontal
;		BNE		.build_store1
loc_B821A:
		CMP		#_MAP_ELECTRO_UD
		BEQ		.build_storeBC
		BNE		.build_forbidden0
;		LDA		#_MAP_ELECTRO_RAIL_UD		; rail+electro vertical
;		BNE		.build_store1

; =============== S U B	R O U T	I N E =======================================
_select_electro_intersection_tile:
		LDA		_cur_build_sample_tile_idx
		CMP		#_MAP_ROAD_LR
		BEQ		.build_storeBA				; OPTIMIZED
;		BNE		loc_B822F					; REDUNDANT
;		LDA		#_MAP_ELECTRO_ROAD_UD		; electro+road horizontal
;		BNE		.build_store1
loc_B822F:
		CMP		#_MAP_ROAD_UD
		BEQ		.build_storeB9				; OPTIMIZED
;		BNE		loc_B8237					; REDUNDANT
;		LDA		#_MAP_ELECTRO_ROAD_LR		; electro+road vertical
;		BNE		.build_store1
loc_B8237:
		CMP		#_MAP_RAIL_LR
		BEQ		.build_storeBC				; OPTIMIZED
;		BNE		loc_B823F					; REDUNDANT
;		LDA		#_MAP_ELECTRO_RAIL_UD		; electro+rail horizontal
;		BNE		.build_store1
loc_B823F:
		CMP		#_MAP_RAIL_UD
		BNE		.build_forbidden0
.build_storeBB:
		LDA		#_MAP_ELECTRO_RAIL_LR		; electro+rail vertical
		.BYTE	$2C
.build_storeB7:
		LDA		#_MAP_ROAD_RAIL_LR			; rail+road horizontal
		.BYTE	$2C
.build_storeB8:
		LDA		#_MAP_ROAD_RAIL_UD			; rail+road vertical
		.BYTE	$2C
.build_storeB9:
		LDA		#_MAP_ELECTRO_ROAD_LR		; road+electric horizontal
		.BYTE	$2C
.build_storeBA:
		LDA		#_MAP_ELECTRO_ROAD_UD		; road+electric vertical
		.BYTE	$2C
.build_storeBC:
		LDA		#_MAP_ELECTRO_RAIL_UD		; electro+rail horizontal
		STA		_cur_build_store_tile_idx
		JMP		_money_subtract

; B3 B4 B5
; FF 00 01
; 4B 4C 4D

; =============== S U B	R O U T	I N E =======================================
_water_tile_build_select:
		DEC		_tmp_ptrC1+1					; get four surrounding tiles
		LDY		#$B4							; upper, lower, left, right
		LDA		(_tmp_ptrC1),Y
		PHA										; push upper
		LDY		#$FF
		LDA		(_tmp_ptrC1),Y
		TAX										; X left
		INC		_tmp_ptrC1+1
		LDY		#$4C
		LDA		(_tmp_ptrC1),Y					; push lower
		PHA
		TXA
		PHA										; push left
		LDY		#$01
		LDA		(_tmp_ptrC1),Y
		PHA										; push right
		LDA		_const_palette_selected_idx
		CMP		#$01
		BNE		.no_roads_on_water				; here we build roads on water
		PLA										; pop surrounding tiles in reverse order
		JSR		_test_for_valid_hor_road_water_joint
		BCS		.valid_left_road_water_joint
		PLA
		JSR		_test_for_valid_hor_road_water_joint
		BCS		.valid_right_road_water_joint
		PLA
		JSR		_test_for_valid_vert_road_water_joint
		BCS		.valid_bottom_road_water_joint
		PLA
		JSR		_test_for_valid_vert_road_water_joint
		BCS		.valid_upper_road_water_joint
.build_forbidden0:
		SEC
		RTS
.no_roads_on_water:
		CMP		#$03
		BNE		.rails_water_joint_tests		; here we built electrics
		PLA
		JSR		_test_for_valid_hor_electric_water_joint
		BCS		.valid_left_electro_water_joint
		PLA
		JSR		_test_for_valid_hor_electric_water_joint
		BCS		.valid_right_electro_water_joint
		PLA
		JSR		_test_for_valid_vert_electric_water_joint
		BCS		.valid_bottom_electro_water_joint
		PLA
		JSR		_test_for_valid_vert_electric_water_joint
		BCS		.valid_upper_electro_water_joint
		BCC		.build_forbidden0
.rails_water_joint_tests:
		PLA
		JSR		_test_for_valid_hor_rail_water_joint
		BCS		.valid_left_rail_water_joint
		PLA
		JSR		_test_for_valid_hor_rail_water_joint
		BCS		.valid_right_rail_water_joint
		PLA
		JSR		_test_for_valid_vert_rail_water_joint
		BCS		.valid_bottom_rail_water_joint
		PLA
		JSR		_test_for_valid_vert_rail_water_joint
		BCS		.valid_upper_rail_water_joint
		BCC		.build_forbidden0
.valid_left_road_water_joint:
		PLA
.valid_right_road_water_joint:
		PLA
		PLA
		LDA		_cur_build_sample_tile_idx		; special water tiles
		CMP		#_MAP_WATER_ROUTE
		BNE		loc_B82CA
		LDA		#_MAP_BRIDGE_WATER_LR			; set the special "bridge" road tile
		BNE		loc_B82CC
loc_B82CA:
		LDA		#_MAP_WATER_ROAD_LR				; let's build a regular hor road
loc_B82CC:
;		STA		_cur_build_store_tile_idx
;		JMP		_money_subtract
		BNE		.build_store					; OPTIMIZED
.valid_bottom_road_water_joint:
		PLA
.valid_upper_road_water_joint:
		LDA		_cur_build_sample_tile_idx
		CMP		#_MAP_WATER_ROUTE
		BNE		loc_B82DD
		LDA		#_MAP_BRIDGE_WATER_UD			; another bridge road
		BNE		loc_B82DF
loc_B82DD:
		LDA		#_MAP_WATER_ROAD_UD
loc_B82DF:
;		STA		_cur_build_store_tile_idx
;		JMP		_money_subtract
		BNE		.build_store					; OPTIMIZED
; NOTE: there is no sailable electrics or rails on water at all!
;
.valid_left_electro_water_joint:
		PLA
.valid_right_electro_water_joint:
		PLA										; there is no special bridger nor for a electrics...
		PLA
		LDA		#_MAP_WATER_RAIL_LR
;		STA		_cur_build_store_tile_idx
;		JMP		_money_subtract
		BNE		.build_store					; OPTIMIZED
.valid_bottom_electro_water_joint:
		PLA
.valid_upper_electro_water_joint:
		LDA		#_MAP_WATER_RAIL_UD
;		STA		_cur_build_store_tile_idx
;		JMP		_money_subtract
		BNE		.build_store					; OPTIMIZED
.valid_left_rail_water_joint:
		PLA
.valid_right_rail_water_joint:
		PLA										; ...nor for a rail lines on the water
		PLA
		LDA		#_MAP_WATER_ELECTRIC_LR
;		STA		_cur_build_store_tile_idx
;		JMP		_money_subtract
		BNE		.build_store					; OPTIMIZED
.valid_bottom_rail_water_joint:
		PLA
.valid_upper_rail_water_joint:
		LDA		#_MAP_WATER_ELECTRIC_UD
.build_store:
		STA		_cur_build_store_tile_idx
		JMP		_money_subtract


; =============== S U B	R O U T	I N E =======================================
_test_for_valid_hor_road_water_joint:
		CMP		#_MAP_ROAD_LR
		BCC		.invalid_water_joint			; bulldozered ground and all other buildings
		CMP		#_MAP_RAIL_LR
		BCC		.valid_water_joint				; all roads here
		CMP		#_MAP_ROAD_RAIL_LR
		BEQ		.valid_water_joint				; hor road+rail
		CMP		#_MAP_ELECTRO_ROAD_UD
		BEQ		.valid_water_joint				; hor road+electric
		CMP		#_MAP_WATER_ROAD_LR
		BEQ		.valid_water_joint				; hor road+water
		CMP		#_MAP_BRIDGE_WATER_LR
		BEQ		.valid_water_joint				; hor road+water copy, unused
.invalid_water_joint:
		CLC
		RTS
.valid_water_joint:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_for_valid_vert_road_water_joint:
		CMP		#_MAP_ROAD_LR
		BCC		.invalid_water_joint			; the same for vert tiles here
		CMP		#_MAP_RAIL_LR
		BCC		.valid_water_joint				; regular roards are valid all
		CMP		#_MAP_ROAD_RAIL_UD
		BEQ		.valid_water_joint
		CMP		#_MAP_ELECTRO_ROAD_LR
		BEQ		.valid_water_joint
		CMP		#_MAP_WATER_ROAD_UD
		BEQ		.valid_water_joint
		CMP		#_MAP_BRIDGE_WATER_UD
		BEQ		.valid_water_joint
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_for_valid_hor_electric_water_joint:
		CMP		#_MAP_RAIL_LR
		BCC		loc_B8357
		CMP		#_MAP_ELECTRO_LR
		BCC		loc_B8359
		CMP		#_MAP_ROAD_RAIL_UD
		BEQ		loc_B8359
		CMP		#_MAP_ELECTRO_RAIL_UD
		BEQ		loc_B8359
		CMP		#_MAP_WATER_RAIL_LR
		BEQ		loc_B8359
loc_B8357:
		CLC
		RTS
loc_B8359:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_for_valid_vert_electric_water_joint:
		CMP		#_MAP_RAIL_LR
		BCC		loc_B8357
		CMP		#_MAP_ELECTRO_LR
		BCC		loc_B8359
		CMP		#_MAP_ROAD_RAIL_LR
		BEQ		loc_B8359
		CMP		#_MAP_ELECTRO_RAIL_LR
		BEQ		loc_B8359
		CMP		#_MAP_WATER_RAIL_UD
		BEQ		loc_B8359
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_for_valid_hor_rail_water_joint:
		CMP		#_MAP_GROUND0
		BCC		loc_B838B
		CMP		#_MAP_ELECTRO_LR
		BCC		loc_B8389
		CMP		#_MAP_GRASS
		BCC		loc_B838B
		CMP		#_MAP_ELECTRO_ROAD_LR
		BEQ		loc_B838B
		CMP		#_MAP_ELECTRO_RAIL_LR
		BEQ		loc_B838B
		CMP		#_MAP_WATER_ELECTRIC_LR
		BEQ		loc_B838B
loc_B8389:
		CLC
		RTS
loc_B838B:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_for_valid_vert_rail_water_joint:
		CMP		#_MAP_GROUND0
		BCC		loc_B838B
		CMP		#_MAP_ELECTRO_LR
		BCC		loc_B8389
		CMP		#_MAP_GRASS
		BCC		loc_B838B
		CMP		#_MAP_ELECTRO_ROAD_UD
		BEQ		loc_B838B
		CMP		#_MAP_ELECTRO_RAIL_UD
		BEQ		loc_B838B
		CMP		#_MAP_WATER_ELECTRIC_UD
		BEQ		loc_B838B
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_is_building_available_here:
		LDA		#$FF
		STA		_cur_build_store_tile_idx		; set default building flag
		LDA		_city._game_flags
		AND		#$10
		BEQ		loc_B83B4						; no test if debug mode
		CLC										; C=0 - allowed
		RTS
loc_B83B4:
		LDY		#$00							; test all tiles under the future building
loc_B83B6:
		STY		_ptr0
		LDA		(_tmp_ptrC5),Y					; according to scheme
		CMP		#$80
		BEQ		loc_B83CA
		TAY
		LDA		(_tmp_ptrC1),Y					; read and test if we can build here something
		JSR		_test_build_available_tile
		LDY		_ptr0
		INY
		BCC		loc_B83B6						; C here is a result of JSR!
;locret_B83C9:
		RTS										; C=1 here, forbid
loc_B83CA:
		JMP		_money_subtract					; OPTIMIZED
;		BCS		locret_B83C9					; REDUNDANT, exit anyway with correct C no need to recheck
;		RTS

; =============== S U B	R O U T	I N E =======================================
_test_build_available_tile:
		PHA
		LDA		_const_palette_selected_idx		; present buildings?
		CMP		#$0D
		BNE		.regular_test
		LDY		_cur_present_select_idx			; yes, try specual case of
		LDA		_city._presents_list,Y			; water claim present
		CMP		#$05
		BEQ		.water_claim_test
.regular_test:
		PLA
		CMP		#_MAP_GROUND0					; not grounds, forbid alwaya
		BCC		.forbid							; can't autobulldoze buildings
		CMP		#_MAP_GROUND0
		BEQ		.allow
		PHA
		LDA		_city._game_flags				; test for autobuldoze flag
		LSR
		BCC		.no_auto_bulldoze_here
		LDA		_const_palette_selected_idx		; special case for train station present
		CMP		#$0D
		BNE		loc_B8401
		LDY		_cur_present_select_idx
		LDA		_city._presents_list,Y
		CMP		#$0E
		BEQ		.train_station_test
loc_B8401:
		PLA
.regular_bulldoze:
		LDY		_const_palette_selected_idx
		CPY		#$04
		BEQ		.common_bulldoze_test			; skip additional check for parks
		CMP		#_MAP_ELECTRO_LR				; buildings can be build above electic wires
		BCC		.forbid
		CMP		#_MAP_GRASS
		BCC		.electo_wires_bulldoze			; bulldoze wires
.common_bulldoze_test:
		CMP		#_MAP_CORRUPTED					; all including grass and trees are forbidden
		BCC		loc_B842E
		BEQ		.allow_with_bulldoze			; can bulldoze corrupted map
		CMP		#_MAP_SHORE0					; mixed tiles are forbinned
		BCC		.forbid
		CMP		#_MAP_WATER_ROAD_LR
		BCC		.allow_with_bulldoze			; here is the reange for sea shores and grounds
		CMP		#_MAP_CORRUPT0
		BCC		.forbid
		CMP		#_MAP_BRIDGE_WATER_LR			; the rest of forbiddent tiles are for fire, etc
		BCS		.forbid
.allow_with_bulldoze:
		INCW	_cur_building_price				; increment bulldoze price
.allow:
		CLC
		RTS
loc_B842E:
		LDA		_const_palette_selected_idx		; can bulldoze parks as well, but can't rebulldoze it
		CMP		#$04
		BNE		.allow_with_bulldoze
		BEQ		.forbid
.electo_wires_bulldoze:
		LDA		_const_palette_selected_idx		; the same here for electrics
		CMP		#$02
		BNE		.allow_with_bulldoze
		BEQ		.forbid
.no_auto_bulldoze_here:
		PLA
.forbid:
		SEC
		RTS
.water_claim_test:
		PLA
		CMP		#_MAP_WATER						; water claims only full watered area including shores
		BCC		.regular_bulldoze
		CMP		#_MAP_GROUND01
		BCC		.false
		CMP		#_MAP_WATER_ROUTE
		BNE		.regular_bulldoze
.false:
		CLC
		RTS
.train_station_test:
		PLA
		CMP		#_MAP_RAIL_LR
		BCC		.regular_bulldoze
		CMP		#_MAP_ELECTRO_LR
		BCS		.regular_bulldoze
;		CLC										; REDUNDANT, C=0 already here
		RTS

; =============== S U B	R O U T	I N E =======================================
; subtracts the delta amount from money if possible, return C=0 if sucess
; return subtracted back, return C=1 if money isn't enough
;
_money_subtract:
		LDA		_city._game_flags			; debug cheat, free construction
		AND		#$10
		BNE		loc_B8483
		LDA		_city._money				; subtract money with delta
		SEC
		SBC		_cur_building_price
		STA		_city._money
		LDA		_city._money+1
		SBC		_cur_building_price+1
		STA		_city._money+1
		LDA		_city._money+2
		SBC		#$00
		STA		_city._money+2
		BCC		loc_B8485
loc_B8483:
		CLC
		RTS
loc_B8485:
		LDA		_city._money				; if exceeded, add back
		ADC		_cur_building_price
		STA		_city._money
		LDA		_city._money+1
		ADC		_cur_building_price+1
		STA		_city._money+1
		LDA		_city._money+2
		ADC		#$00
		STA		_city._money+2
		SEC
		RTS

		SECTION_END C03

		SECTION_START F05

; =============== S U B	R O U T	I N E =======================================
_title_screen:
		LDA		_title_sub_idx
		CMP		#$02
		BEQ		loc_B8514
		INC		_tmp118
		DEC		_mmc5_operation_in_progress_flag
		LDA		#$FF
		STA		_game_core_state
		JSR		_render_off
		JSR		_spr_clear
		LDA		#$08
		STA		_mmc5_nt_mode_shadow
		STA		_MMC5_NT_MODE
		JSRXY	_tlm_res_unpack,_tlm_res_title_main
		JSR		_wait_for_nmi
		JSR		_ppu_reset
		LDX		#$00
		STX		_game_wnd_spr_mode_flag
		STX		_tmp118
		STX		_title_sub_idx
		STX		_cur_game_mode
		INX
		STX		_apu_mus_idx_req
		LDX		#$03
		STX		_scr_mode_idx
		LDA		#$07
		STA		_screen_pal_cur_bg_idx
		LDA		#$03
		STA		_ppu_pal_dequeue_req
		JSR		_wait_for_nmi
		LDA		#$83
		JSR		_pal_load_lib
		LDA		#$00
		STA		_mmc5_nt_mode_shadow
		STA		_MMC5_NT_MODE
		LDA		#$03
		STA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
		LDA		#$90
		STA		_ppu_ctrl_shadow
		LDA		_ppu_mask_shadow
		AND		#$FC
		STA		_ppu_mask_shadow
		INC		_mmc5_operation_in_progress_flag
		JSR		_render_on
		JSR		_pal_fade_in
loc_B8514:
		JSR		_wait_for_nmi
		LDA		_pad1_prev_held
		AND		#$CA
		CMP		#$CA
		BNE		loc_B8526
		FJSRA	_sram_reset,SRAM,PRG3
loc_B8526:
		JSR		_title_sub_exec
		BIT		_title_sub_idx
		BMI		loc_B8534
		JSR		_spr_finish
		JMP		loc_B8514
loc_B8534:
		JSR		_name_bufs_reset
		JSR		_spr_clear
		LDA		#$00
		STA		_MMC5_NT_MODE
		RTS

; =============== S U B	R O U T	I N E =======================================
_title_sub_exec:
		LDA		_title_sub_idx
		JSR		_switch
		.WORD	_title_sub0_init
		.WORD	_title_sub1_title_input
		.WORD	_title_sub2_menu_draw
		.WORD	_title_sub3_menu_input

; =============== S U B	R O U T	I N E =======================================
_title_sub0_init:
		INC		_title_sub_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_title_sub1_title_input:
		LDA		_pad0_autorep_press
		AND		#$90
		BEQ		locret_B855E
		INC		_title_sub_idx
		JMP		_pal_fade_out
locret_B855E:
		RTS

; =============== S U B	R O U T	I N E =======================================
_title_sub2_menu_draw:
		INC		_title_sub_idx
		DEC		_mmc5_operation_in_progress_flag
		LDA		#$01
		STA		_game_core_state
		JSR		_render_off
		JSR		_spr_clear
		LDA		#$08
		STA		_mmc5_nt_mode_shadow
		STA		_MMC5_NT_MODE
		LDA		#$03
		STA		_mmc5_chr_banks_shadow+3
		STA		_MMC5_CHR_BANKSA+3
		LDXY	_tlm_res_title_menu_default
		LDA		_has_saved_city_data_flag
		BNE		loc_B858C
		LDXY	_tlm_res_title_menu_continue
loc_B858C:
		JSR		_tlm_res_unpack
		LDA		_has_saved_city_data_flag
		BNE		loc_B859E
		LDA		_cur_game_mode
		BNE		loc_B859E
		LDX		#$03
		STX		_cur_game_mode
loc_B859E:
		JSR		_title_cursor_draw
		LDX		#$03
		STX		_scr_mode_idx
		LDX		#$05
		STX		_screen_pal_cur_spr_idx
		LDA		#$07
		STA		_screen_pal_cur_bg_idx
		LDA		#$83
		JSR		_pal_load_lib
		LDA		#$00
		STA		_mmc5_nt_mode_shadow
		STA		_MMC5_NT_MODE
		LDA		_apu_mus_pulse2_shadow+2
		CMP		#$01
		BEQ		loc_B85CB
		CMP		#$02
		BEQ		loc_B85CB
		LDA		#$01
		STA		_apu_mus_idx_req
loc_B85CB:
		LDA		#$90
		STA		_ppu_ctrl_shadow
		LDA		_ppu_mask_shadow
		AND		#$FC
		STA		_ppu_mask_shadow
		INC		_mmc5_operation_in_progress_flag
		JSR		_render_on
		JMP		_pal_fade_in

; =============== S U B	R O U T	I N E =======================================
_title_sub3_menu_input:
		LDA		_pad0_autorep_press
		AND		#$90
		BNE		loc_B8624
		LDA		_pad0_autorep_press
		AND		#$0C
		BEQ		loc_B8631
		LDA		#$01
		STA		_apu_se_flags
		LDX		_cur_game_mode
		LDA		_title_menu_item_select_up,X
		TAX
		LDA		_pad0_autorep_press
		AND		#$0C
		BEQ		loc_B8631
		CMP		#$08
		BNE		loc_B860F
		LDA		_has_saved_city_data_flag
		BNE		loc_B8608
		DEX
		BNE		loc_B861C
		LDX		#$04
loc_B8608:
		DEX
		BPL		loc_B861C
		LDX		#$03
		BNE		loc_B861C
loc_B860F:
		INX
		CPX		#$04
		BCC		loc_B861C
		LDX		#$00
		LDA		_has_saved_city_data_flag
		BNE		loc_B861C
		INX
loc_B861C:
		LDA		_title_menu_item_select_down,X
		STA		_cur_game_mode
		BPL		loc_B8631
loc_B8624:
		LDA		#$01
		STA		_apu_se_flags
		JSR		_title_cursor_draw
		LDA		#$80
		STA		_title_sub_idx
		RTS
loc_B8631:
		JMP		_title_cursor_draw

;_unused_2:
;		.BYTE	$03,$04
_title_menu_item_select_up:
		.BYTE	$00,$02,$03,$01
_title_menu_item_select_down:
		.BYTE	$00,$03,$01,$02

; =============== S U B	R O U T	I N E =======================================
_title_cursor_draw:
		LDX		_cur_game_mode
		LDA		_title_cursor_pos_list,X
		LDX		_has_saved_city_data_flag
		BNE		loc_B864D
		SEC
		SBC		#$10
loc_B864D:
		STA		_tmp2
		LDA		#$00
		STA		_tmp3
		STA		_tmp4
		MOVWO	_ptr0,_spr_raw_title_cursor
		JMP		_spr_raw_insert

_spr_raw_title_cursor:
		.BYTE	$00,$F4,$03,$20
		.BYTE	$00,$F5,$03,$28
		.BYTE	$00,$F6,$03,$30
		.BYTE	$08,$F7,$03,$20
		.BYTE	$08,$F8,$03,$28
		.BYTE	$FF

_title_cursor_pos_list:
		.BYTE	$68,$A8,$C8,$88
_tlm_res_title_main:
		.WORD	$2000
		.BYTE	4
		.WORD	_tlm_rle_title_header0
		.WORD	_tlm_rle_title1
		.WORD	_tlm_rle_title2
		.WORD	_tlm_rle_title3
_tlm_res_title_menu_continue:
		.WORD	$2000
		.BYTE	4
		.WORD	_tlm_rle_title_header0
		.WORD	_tlm_rle_title1A
		.WORD	_tlm_rle_title2
		.WORD	_tlm_rle_title3A
_tlm_res_title_menu_default:
		.WORD	$2000
		.BYTE	4
		.WORD	_tlm_rle_title_header0
		.WORD	_tlm_rle_title1B
		.WORD	_tlm_rle_title2
		.WORD	_tlm_rle_title3B
_tlm_rle_title_header0:
		.BYTE	$E4,$44,$FF,$67,$02,$23,$FF,$01,$0E,$0F,$65,$80,$03,$FF,$FF,$7A
		.BYTE	$7B,$27,$FF,$67,$12,$22,$FF,$62,$1D,$66,$90,$02,$FF,$7C,$7D,$26
		.BYTE	$FF,$6E,$21,$69,$A0,$25,$FF,$6F,$30,$69,$B0,$25,$FF,$6F,$40,$69
		.BYTE	$C0,$25,$FF,$6F,$50,$68,$D0,$26,$FF,$6F,$60,$67,$E0,$28,$FF,$63
		.BYTE	$71,$2F,$FF,$62,$F5,$3B,$FF,$63,$8B,$3C,$FF,$01,$9C,$9D,$25,$FF
		.BYTE	$FF
_tlm_rle_title1:
		.BYTE	$2F,$FF,$00,$9E,$83,$00,$0D,$27,$FF,$00,$04,$2A,$FF,$64,$00,$02
		.BYTE	$FF,$9F,$AB,$62,$BA,$00,$AA,$63,$AC,$64,$CB,$8A,$00,$1A,$64,$09
		.BYTE	$03,$FF,$9F,$AA,$AC,$62,$BD,$66,$D9,$2C,$FF,$65,$19,$00,$FF,$66
		.BYTE	$E8,$01,$9A,$9B,$8F,$00,$55,$65,$29,$03,$FF,$FF,$21,$22,$30,$FF
		.BYTE	$84,$00,$1D,$65,$39,$03,$FF,$FF,$31,$32,$24,$FF,$62,$28,$24,$FF
		.BYTE	$00,$40,$27,$FF,$65,$49,$03,$FF,$FF,$41,$42,$84,$00,$1B,$62,$48
		.BYTE	$24,$FF,$01,$50,$51,$22,$FF,$62,$55,$00,$FF,$65,$59,$00,$FF,$62
		.BYTE	$50,$24,$FF,$62,$58,$01,$FF,$FF,$64,$5D,$00,$FF,$6C,$63,$64,$60
		.BYTE	$00,$FF,$64,$66,$01,$FF,$FF,$72,$6D,$7F,$70,$7F,$80,$7F,$90,$7F
		.BYTE	$A0,$7F,$B0,$7F,$C0,$7F,$D0,$72,$E0,$23,$FF,$03,$F7,$F8,$FF,$FF
		.BYTE	$64,$FB,$6D,$F0,$E4,$41,$FF,$FF
_tlm_rle_title1A:
		.BYTE	$E4,$6A,$FF,$07,$8E,$AD,$AC,$C1,$AB,$C2,$C1,$AF,$37,$FF,$07,$9E
		.BYTE	$BD,$BC,$D1,$BB,$D2,$D1,$BF,$E4,$54,$FF,$63,$AA,$09,$AB,$FF,$AE
		.BYTE	$AF,$C0,$FF,$C1,$C2,$AB,$7B,$31,$FF,$63,$BA,$09,$BB,$FF,$BE,$BF
		.BYTE	$D0,$FF,$D1,$D2,$BB,$8B,$3E,$FF,$00,$9B,$E4,$31,$FF,$0E,$AA,$AF
		.BYTE	$C9,$AF,$C1,$AB,$FF,$AA,$C1,$AF,$8C,$AC,$AD,$C2,$8D,$30,$FF,$0E
		.BYTE	$BA,$BF,$D9,$BF,$D1,$BB,$FF,$BA,$D1,$BF,$9C,$BC,$BD,$D2,$9D,$E4
		.BYTE	$E8,$FF,$FF
_tlm_rle_title1B:
		.BYTE	$E4,$26,$FF,$01,$C3,$AF,$62,$C4,$0B,$AF,$FF,$AA,$AC,$C7,$AF,$C8
		.BYTE	$FF,$C1,$C2,$AB,$7B,$2E,$FF,$01,$D3,$BF,$62,$D4,$0B,$BF,$FF,$BA
		.BYTE	$BC,$D7,$BF,$D8,$FF,$D1,$D2,$BB,$8B,$3E,$FF,$00,$9B,$E4,$32,$FF
		.BYTE	$07,$8E,$AD,$AC,$C1,$AB,$C2,$C1,$AF,$37,$FF,$07,$9E,$BD,$BC,$D1
		.BYTE	$BB,$D2,$D1,$BF,$E4,$54,$FF,$63,$AA,$04,$AB,$FF,$AE,$AF,$C0,$93
		.BYTE	$00,$33,$22,$FF,$63,$BA,$04,$BB,$FF,$BE,$BF,$D0,$F0,$56,$00,$53
		.BYTE	$0E,$AA,$AF,$C9,$AF,$C1,$AB,$FF,$AA,$C1,$AF,$8C,$AC,$AD,$C2,$8D
		.BYTE	$30,$FF,$0E,$BA,$BF,$D9,$BF,$D1,$BB,$FF,$BA,$D1,$BF,$9C,$BC,$BD
		.BYTE	$D2,$9D,$E4,$A8,$FF,$FF
_tlm_rle_title2:
		.BYTE	$E4,$50,$14,$27,$54,$03,$14,$14,$94,$94,$32,$14,$29,$54,$9F,$00
		.BYTE	$5A,$02,$54,$54,$54,$32,$14,$9F,$00,$90,$F0,$2B,$00,$90,$33,$14
		.BYTE	$9E,$00,$F1,$08,$14,$14,$14,$14,$14,$14,$14,$14,$14,$9F,$01,$18
		.BYTE	$F0,$21,$01,$18,$25,$14,$FF
_tlm_rle_title3:
		.BYTE	$2F,$14,$00,$D4,$83,$00,$0D,$27,$14,$00,$D7,$2A,$14,$24,$D7,$00
		.BYTE	$14,$2E,$D4,$03,$14,$14,$14,$D7,$99,$00,$22,$0D,$14,$14,$14,$14
		.BYTE	$14,$14,$14,$14,$14,$14,$14,$14,$14,$D7,$94,$00,$29,$00,$D4,$8F
		.BYTE	$00,$60,$02,$14,$D6,$D6,$30,$14,$00,$D7,$92,$00,$85,$00,$D6,$86
		.BYTE	$00,$91,$00,$97,$93,$00,$81,$00,$D7,$8A,$00,$B6,$00,$D7,$85,$00
		.BYTE	$26,$08,$14,$D7,$D7,$D7,$D7,$D7,$D7,$14,$D6,$8B,$00,$B1,$22,$D6
		.BYTE	$02,$97,$97,$14,$2B,$D7,$00,$97,$24,$D6,$02,$14,$96,$96,$87,$00
		.BYTE	$F8,$02,$D7,$D7,$D7,$91,$01,$03,$07,$D6,$96,$96,$D6,$D6,$D6,$D6
		.BYTE	$D6,$91,$01,$1D,$05,$D7,$D6,$D6,$D6,$96,$96,$93,$01,$35,$22,$97
		.BYTE	$9F,$01,$4C,$F0,$29,$01,$4C,$01,$D6,$D6,$9A,$01,$58,$2C,$D6,$83
		.BYTE	$01,$69,$2B,$97,$02,$96,$96,$96,$85,$01,$36,$26,$96,$92,$01,$40
		.BYTE	$01,$D6,$D6,$8D,$01,$B5,$85,$00,$25,$01,$14,$14,$92,$02,$0B,$E4
		.BYTE	$41,$14,$FF
_tlm_rle_title3A:
		.BYTE	$E4,$6A,$14,$00,$81,$26,$80,$37,$14,$9F,$00,$6B,$E4,$3C,$14,$85
		.BYTE	$00,$6E,$83,$00,$70,$22,$80,$00,$81,$31,$14,$9F,$00,$E8,$0C,$14
		.BYTE	$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$14,$9F,$01,$15,$32
		.BYTE	$14,$00,$80,$88,$00,$E8,$00,$81,$94,$00,$F2,$9F,$01,$68,$E4,$D7
		.BYTE	$14,$FF
_tlm_rle_title3B:
		.BYTE	$E4,$26,$14,$25,$80,$85,$00,$26,$83,$00,$26,$00,$81,$9F,$00,$18
		.BYTE	$3E,$14,$9F,$00,$57,$33,$14,$00,$81,$26,$80,$37,$14,$9F,$00,$AB
		.BYTE	$E4,$3C,$14,$05,$80,$80,$80,$80,$80,$14,$99,$00,$50,$9F,$01,$28
		.BYTE	$F0,$3F,$00,$6A,$89,$00,$27,$00,$81,$94,$00,$54,$9F,$01,$A8,$E4
		.BYTE	$97,$14,$FF

		SECTION_END F05

		SECTION_START 903

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl6_A_bank_draw:
		LDA		#$80
		STA		_tmp118+7
		LDA		#$00
		BIT		_city._bank_flags
		BVC		loc_B8A11
		LDA		#$3F
		JSR		_bank_scr_common_spr_draw
		JSR		_bank_scr_sub7_already_loan_draw
		LDA		#$07
loc_B8A11:
		STA		_tmp118+2
		LDA		#$80
		STA		_game_core_state
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl6_B_bank_input:
		JSR		_bank_scr_sub_exec
		BIT		_tmp118+2
		BPL		locret_B8A34
		LDA		#$40
		STA		_scr_res_control_flags
		LDA		#$02
		STA		_scr_res_idx
		LDA		#$10
		STA		_scr_res_control_flags
		JMP		_pal_fade_out
locret_B8A34:
		RTS

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub_exec:
		LDA		_tmp118+2
		JSR		_switch
		.WORD	_bank_scr_sub0_init
		.WORD	_bank_scr_sub0_init
		.WORD	_bank_scr_sub2_loan_select
		.WORD	_bank_scr_sub3_loan_done_init
		.WORD	_bank_scr_sub4_loan_done_animate
		.WORD	_bank_scr_sub5_exit_wait_init
		.WORD	_bank_scr_sub6_exit_wait_input
		.WORD	_bank_scr_sub7_already_loan_draw
		.WORD	_bank_scr_sub8_already_load_input

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub0_init:
		LDA		#$00
		STA		_tmp118+4
		INC		_tmp118+2
		RTS

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub2_loan_select:
		LDA		#$A0
		LDY		_tmp118+4
		BEQ		loc_B8A5F
		LDA		#$D0
loc_B8A5F:
		STA		_spr_insert_args._pos._COL
		LDA		#$D9
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._idx
		JSR		_spr_lib_attr_insert
		BIT		_pad0_autorep_press
		BMI		loc_B8A8A
		BVC		loc_B8A79
		LDA		#$01
		STA		_tmp118+4
		BNE		loc_B8AA2
loc_B8A79:
		LDA		_pad0_autorep_press
		AND		#$03
		BEQ		locret_B8AA7
		LDA		_tmp118+4
		EOR		#$01
		STA		_tmp118+4
		JMP		locret_B8AA7
loc_B8A8A:
		LDA		_tmp118+4
		BNE		loc_B8AA2
		LDA		#$15
		STA		_city._loan_period					; just only for 21 year
; REDUNDANT, never used here, the loan amount fixed to 500
; and calculated manually at _fiscal_year_over_calculations
;		MOVWI	_tax_alloc._loan_fee,500			; 500 a year
		INC		_tmp118+2
		RTS
loc_B8AA2:
		LDA		#$05
		STA		_tmp118+2
locret_B8AA7:
		RTS

; REDUNDANT, two copies of the same routine here
;
; =============== S U B	R O U T	I N E =======================================
;_spr_raw_insert:
;		LDX		_spr_buf_pos
;		LDY		#$00
;loc_B8AAC:
;		LDA		(_ptr0),Y
;		CMP		#$FF
;		BEQ		loc_B8AD6
;		CLC
;		ADC		_tmp2
;		STA		_spr_buf,X
;		INX
;		INY
;		LDA		(_ptr0),Y
;		STA		_spr_buf,X
;		INX
;		INY
;		LDA		(_ptr0),Y
;		ORA		_tmp3
;		STA		_spr_buf,X
;		INX
;		INY
;		LDA		(_ptr0),Y
;		CLC
;		ADC		_tmp4
;		STA		_spr_buf,X
;		INX
;		INY
;		BNE		loc_B8AAC
;loc_B8AD6:
;		STX		_spr_buf_pos
;		RTS

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub3_loan_done_init:
		LDA		_city._bank_flags
		ORA		#$C0
		STA		_city._bank_flags
		LDA		_city._money
		CLC
		ADC		#$10
		STA		_city._money
		LDA		_city._money+1
		ADC		#$27
		STA		_city._money+1
		BCC		loc_B8AF7
		INC		_city._money+2
loc_B8AF7:
		LDA		#$00
		STA		_tmp118
		LDA		#$58
		JSR		_bank_scr_common_spr_draw
		INC		_tmp118+2
		MOVWO	_ptr0,byte_B8DC8
		MOVWO	_tmp2,byte_B8DFF
		JMP		_ppu_res_queue

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub4_loan_done_animate:
		JSR		sub_B8B92
		INC		_tmp118
		LDA		_tmp118
		LSR
		TAX
		LDA		byte_B8D7E,X
		BMI		loc_B8B89
		JSR		_bank_scr_common_spr_draw
		LDX		_tmp118
		CPX		#$12
		BEQ		loc_B8B63
		CPX		#$22
		BEQ		loc_B8B76
		CPX		#$32
		BEQ		loc_B8B50
		CPX		#$42
		BNE		locret_B8B91
		MOVWO	_ptr0,byte_B8DD3
		MOVWO	_tmp2,byte_B8E0A
		JMP		_ppu_res_queue
loc_B8B50:
		MOVWO	_ptr0,byte_B8DBD
		MOVWO	_tmp2,byte_B8DF4
		JMP		_ppu_res_queue
loc_B8B63:
		MOVWO	_ptr0,byte_B8DA7
		MOVWO	_tmp2,byte_B8DDE
		JMP		_ppu_res_queue
loc_B8B76:
		MOVWO	_ptr0,byte_B8DB2
		MOVWO	_tmp2,byte_B8DE9
		JMP		_ppu_res_queue
loc_B8B89:
		LDA		#$3F
		JSR		_bank_scr_common_spr_draw
		INC		_tmp118+2
locret_B8B91:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B8B92:
		LDA		#$00
		STA		_tmp2
		STA		_tmp4
		STA		_tmp3
		LDA		_tmp118+4
		BNE		loc_B8BAA
		MOVWO	_ptr0,byte_B8BB5
		JMP		_spr_raw_insert
loc_B8BAA:
		MOVWO	_ptr0,byte_B8BD6
		JMP		_spr_raw_insert

byte_B8BB5:
		.BYTE	$CF,$D0,$02,$88
		.BYTE	$CF,$D1,$02,$90
		.BYTE	$CF,$D2,$02,$98
		.BYTE	$CF,$D3,$02,$A0
		.BYTE	$D7,$D4,$02,$88
		.BYTE	$D7,$D5,$02,$90
		.BYTE	$D7,$D6,$02,$98
		.BYTE	$D7,$D7,$02,$A0
		.BYTE	$FF
byte_B8BD6:
		.BYTE	$CF,$D8,$02,$B8
		.BYTE	$CF,$D9,$02,$C0
		.BYTE	$CF,$DA,$02,$C8
		.BYTE	$CF,$DB,$02,$D0
		.BYTE	$D7,$DC,$02,$B8
		.BYTE	$D7,$DD,$02,$C0
		.BYTE	$D7,$DE,$02,$C8
		.BYTE	$D7,$DF,$02,$D0
		.BYTE	$FF

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub5_exit_wait_init:
		LDA		#$40
		STA		_tmp118+7
		INC		_tmp118+2

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub6_exit_wait_input:
		JSR		sub_B8B92
		LDA		_tmp118+4
		BNE		loc_B8C0C
		LDA		#$3F
		JSR		_bank_scr_common_spr_draw
loc_B8C0C:
		DEC		_tmp118+7
		BNE		locret_B8C16
		LDA		#$80
		STA		_tmp118+2
locret_B8C16:
		RTS

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub7_already_loan_draw:
		LDX		#$00
		LDA		_city._loan_period
loc_B8C1C:
		CMP		#$0A							; quick and dirty decimal conversion
		BCC		loc_B8C25						; number cannot be higher than 20
		SBC		#$0A
		INX
		BNE		loc_B8C1C
loc_B8C25:
		STA		_ptr0
		STX		_ptr0+1
		PPUQSTART
		PPUQMOVB	#$F6,OP_NOP,0
		PPUQMOVB	#$26,OP_NOP,0
		PPUQMOVB	#$02,OP_NOP,0
		LDY		_ptr0+1
		LDA		_loan_font_tiles_top_list,Y
		PPUQMOVA
		LDY		_ptr0
		LDA		_loan_font_tiles_top_list,Y
		PPUQMOVA
		PPUQMOVB	#$16,OP_NOP,0
		PPUQMOVB	#$27,OP_NOP,0
		PPUQMOVB	#$02,OP_NOP,0
		LDY		_ptr0+1
		LDA		_loan_font_tiles_bottom_list,Y
		PPUQMOVA
		LDY		_ptr0
		LDA		_loan_font_tiles_bottom_list,Y
		PPUQMOVA
		PPUQEND
		INC		_tmp118+2
		EXTQSTART
		EXTQMOVB	#$F6,OP_NOP,0				; set extnt as well
		EXTQMOVB	#$5E,OP_NOP,0
		EXTQMOVB	#$02,OP_NOP,0
		EXTQMOVB	#$D8,OP_NOP,0
		EXTQMOVA
		EXTQMOVB	#$16,OP_NOP,0
		EXTQMOVB	#$5F,OP_NOP,0
		EXTQMOVB	#$02,OP_NOP,0
		EXTQMOVB	#$D8,OP_NOP,0
		EXTQMOVA
		EXTQEND
		JMP		_bank_scr_common_spr_draw_pos3F

_loan_font_tiles_top_list:
		.BYTE	$CA,$CB,$CC,$CD,$CE,$CF,$EA,$EB,$EC,$ED,$EE,$EF
_loan_font_tiles_bottom_list:
		.BYTE	$DA,$DB,$DC,$DD,$DE,$DF,$FA,$FB,$FC,$FD,$FE,$FF

; =============== S U B	R O U T	I N E =======================================
_bank_scr_sub8_already_load_input:
		LDA		_pad0_autorep_press
		AND		#$C0
		BNE		_bank_scr_return

; =============== S U B	R O U T	I N E =======================================
_bank_scr_common_spr_draw_pos3F:
		LDA		#$3F
		JMP		_bank_scr_common_spr_draw

; =============== S U B	R O U T	I N E =======================================
_bank_scr_return:
		LDA		#$80
		STA		_tmp118+2
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_res_queue:
		LDY		#$00
		PPUQSTART
loc_B8CE4:
		LDA		(_ptr0),Y
		BNE		loc_B8CF0
		INY
		LDA		(_ptr0),Y
		BEQ		loc_B8CF7
		DEY
		LDA		(_ptr0),Y
loc_B8CF0:
		PPUQMOVA
		INY
		BNE		loc_B8CE4
loc_B8CF7:
		PPUQEND
		LDY		#$00
		EXTQSTART
loc_B8CFD:
		LDA		(_tmp2),Y
		BNE		loc_B8D09
		INY
		LDA		(_tmp2),Y
		BEQ		loc_B8D10
		DEY
		LDA		(_tmp2),Y
loc_B8D09:
		EXTQMOVA
		INY
		BNE		loc_B8CFD
loc_B8D10:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_bank_scr_common_spr_draw:
		STA		_tmp2
		LDX		#$00
		STX		_tmp4
		CMP		#$49
		BCS		loc_B8D1F
		LDX		#$20
loc_B8D1F:
		STX		_tmp3
		MOVWO	_ptr0,byte_B8D5C
		JSR		_spr_raw_insert
		LDA		#$00
		STA		_tmp3
		MOVWO	_ptr0,byte_B8D75
		JSR		_spr_raw_insert
		LDA		_tmp2
		CMP		#$47
		BCS		loc_B8D51
		LDX		_spr_buf_pos
		LDY		#$00
loc_B8D45:
		LDA		byte_B8D54,Y
		STA		_spr_buf,X
		INX
		INY
		CPY		#$08
		BCC		loc_B8D45
loc_B8D51:
		STX		_spr_buf_pos
		RTS
byte_B8D54:
		.BYTE	$37,$F4,$00,$80
		.BYTE	$37,$F5,$00,$88
byte_B8D5C:
		.BYTE	$F0,$FD,$23,$68
		.BYTE	$F0,$FD,$03,$70
		.BYTE	$F0,$FD,$03,$78
		.BYTE	$F0,$FD,$03,$80
		.BYTE	$F0,$FD,$03,$88
		.BYTE	$F0,$FD,$23,$90
		.BYTE	$FF
byte_B8D75:
		.BYTE	$F8,$FB,$00,$78
		.BYTE	$F8,$FC,$00,$80
		.BYTE	$FF
byte_B8D7E:
		.BYTE	$58,$59,$5A,$5B
		.BYTE	$5C,$5D,$5E,$5F
		.BYTE	$5F,$5E,$5D,$5C
		.BYTE	$5B,$5A,$59,$58
		.BYTE	$57,$56,$55,$54
		.BYTE	$53,$52,$51,$50
		.BYTE	$4F,$4E,$4D,$4C
		.BYTE	$4B,$4A,$49,$48
		.BYTE	$47,$46,$45,$44
		.BYTE	$43,$42,$41,$40
		.BYTE	$FF
byte_B8DA7:
		.BYTE	$4D,$25,$06,$DF,$DF,$C9,$CA,$CB,$5A,$00,$00
byte_B8DB2:
		.BYTE	$2D,$25,$06,$DF,$B8,$B9,$BA,$BB,$DF,$00,$00
byte_B8DBD:
		.BYTE	$0D,$25,$06,$DF,$A8,$A9,$AA,$AB,$DF,$00,$00
byte_B8DC8:
		.BYTE	$0D,$25,$06,$DC,$DC,$DC,$DC,$DC,$DC,$00,$00
byte_B8DD3:
		.BYTE	$ED,$24,$06,$35,$DF,$DF,$DF,$DF,$3A,$00,$00
byte_B8DDE:
		.BYTE	$4D,$5D,$06,$01,$01,$C8,$C8,$C8,$08,$00,$00
byte_B8DE9:
		.BYTE	$2D,$5D,$06,$01,$C8,$C8,$C8,$C8,$01,$00,$00
byte_B8DF4:
		.BYTE	$0D,$5D,$06,$01,$C8,$C8,$C8,$C8,$01,$00,$00
byte_B8DFF:
		.BYTE	$0D,$5D,$06,$C8,$C8,$C8,$C8,$C8,$C8,$00,$00
byte_B8E0A:
		.BYTE	$ED,$5C,$06,$48,$01,$01,$01,$01,$48,$00,$00

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl1_A_graphs_draw:
		JSR		_graphs_period_start_year_draw
		FJSRA	_graphs_period_button_redraw,PRG6,PRG7	; FIX uninitialized button state
		JMP		_graphs_data_draw

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl1_B_graphs_input:
		JSR		_graphs_period_start_year_draw
		BIT		_scr_res_control_flags
		BMI		loc_B8E30
		BIT		_pad0_autorep_press
		BVS		loc_B8E36
		BPL		locret_B8E3F
		LDA		#$20
		STA		_apu_se_flags+1
		JMP		_graphs_get_hover_button_idx
loc_B8E30:
		JSR		_flush_test_wait
		JMP		sub_B8EFB
loc_B8E36:
		LDA		#$20
		STA		_apu_se_flags+1
		LDA		#$40
		STA		_scr_res_control_flags
locret_B8E3F:
		RTS

; =============== S U B	R O U T	I N E =======================================
_graphs_period_start_year_draw:
		JSR		_get_graphs_scr_period_range
		LDA		_spr_buf_pos
		STA		_tmp5
		LDA		#$03
		STA		_tmp4
		LDY		#$00
loc_B8E4D:
		LDA		_spr_graphs_period_start_year,Y
		CMP		#$B0
		BNE		loc_B8E5B
		LDX		_tmp4
		CLC
		ADC		_ptr0,X
		DEC		_tmp4
loc_B8E5B:
		LDX		_tmp5
		STA		_spr_buf,X
		INC		_tmp5
		INY
		CPY		#$10
		BCC		loc_B8E4D
		LDX		_tmp5
		STX		_spr_buf_pos
		RTS
_spr_graphs_period_start_year:
		.BYTE	$C7,$B0,$00,$3D
		.BYTE	$C7,$B0,$00,$41
		.BYTE	$C7,$B0,$00,$45
		.BYTE	$C7,$B0,$00,$49

; =============== S U B	R O U T	I N E =======================================
_graphs_get_hover_button_idx:
		LDA		_cur_game_cursor_scr_pos._COL
		CMP		#$18
		BCC		nullsub_9
		CMP		#$28
		BCS		nullsub_9
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$48
		BCC		nullsub_9
		CMP		#$A8
		BCC		loc_B8EA5
		LDX		#$06
		CMP		#$B0
		BCC		nullsub_9
		CMP		#$B8
		BCC		loc_B8EB4
		CMP		#$C0
		BCC		nullsub_9
		CMP		#$C8
		BCS		nullsub_9
		INX
		BCC		loc_B8EB4
loc_B8EA5:
		LDX		#$00
		SEC
		SBC		#$48
		BEQ		loc_B8EB4
		DEX
loc_B8EAD:
		INX
		SBC		#$10
		BEQ		loc_B8EB4
		BCS		loc_B8EAD
loc_B8EB4:
		CPX		#$06
		BCC		sub_B8ED8
		TXA
		SEC
		SBC		#$06
		CMP		_graphs_scr_period_mode
		BEQ		nullsub_9
		LDA		_graphs_scr_period_mode
		EOR		#$01
		STA		_graphs_scr_period_mode
		LDA		#$80
		STA		_scr_res_control_flags
		FJSRA	_graphs_period_button_redraw,PRG6,PRG7
		JMP		_graphs_data_draw

; =============== S U B	R O U T	I N E =======================================
sub_B8ED8:
		CPX		_cur_graphs_button_press_idx
		BEQ		nullsub_9
		LDY		_prev_graphs_button_press_idx
		LDA		_cur_graphs_button_press_idx
		STA		_prev_graphs_button_press_idx
		STX		_cur_graphs_button_press_idx
		LDA		#$80
		STA		_scr_res_control_flags
		FJSRA	_graphs_buttons_redraw,PRG6,PRG7
		LDA		#$00
		STA		byte_59B

; =============== S U B	R O U T	I N E =======================================
nullsub_9:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B8EFB:
		LDA		byte_59B
		AND		#$01
		TAX
		LDA		_cur_graphs_button_press_idx,X
		BMI		loc_B8F1A
		JSR		sub_B8F2D
		PHA
		LDA		byte_59B
		AND		#$01
		TAY
		LDX		_cur_graphs_button_press_idx,Y
		JSR		sub_B8F82
		PLA
		JSR		sub_B8F9E
loc_B8F1A:
		INC		byte_59B
		LDA		byte_59B
		CMP		#$14
		BCC		locret_B8F2C
		LDA		#$00
		STA		_scr_res_control_flags
		STA		byte_59B
locret_B8F2C:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B8F2D:
		TAY
		LDA		byte_59B
		LSR
		STA		_tmp2
		TYA
		STA		_tmp3
		LDX		_graphs_scr_period_mode
		BEQ		loc_B8F3E
		LDX		#$3C
loc_B8F3E:
		STX		_ptr0+1
		ASL
		STA		_ptr0
		ASL
		ASL
		CLC
		ADC		_ptr0
		ADC		_ptr0+1
		TAY
		PHA
		LDX		#$03
		LDA		_tmp3
		CMP		#$04
		BEQ		loc_B8F75
		LDA		#$00
		STA		_ptr0
		LDX		#$0A
loc_B8F5A:
		LDA		_wram_map_buf+$7ED,Y
		CMP		_ptr0
		BCC		loc_B8F63
		STA		_ptr0
loc_B8F63:
		INY
		DEX
		BNE		loc_B8F5A
		LDX		#$01
		LDA		_ptr0
		CMP		#$40
		BCC		loc_B8F75
		INX
		CMP		#$80
		BCC		loc_B8F75
		INX
loc_B8F75:
		PLA
		CLC
		ADC		_tmp2
		TAY
		LDA		_wram_map_buf+$7ED,Y
loc_B8F7D:
		LSR
		DEX
		BNE		loc_B8F7D
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B8F82:
		STY		_ptr0
		LDA		byte_B8F92,X
		CLC
		ADC		_ptr0
		STA		_tmp2
		LDA		byte_B8F98,X
		STA		_tmp3
		RTS
byte_B8F92:
		.BYTE	$C8,$C8,$C8,$B8,$C8,$A8
byte_B8F98:
		.BYTE	$81,$01,$41,$C1,$C1,$01

; =============== S U B	R O U T	I N E =======================================
sub_B8F9E:
		STA		_ptr0
		LDA		#$20
		SEC
		SBC		_ptr0
		LSR
		STA		_ptr0+1
		PPUQSTART
		LDA		byte_59B
		CLC
		ADC		#$08
		STA		_tmp4
		PPUQMOVA
		PPUQMOVB	#$25,OP_NOP,0
		PPUQMOVB	#$90,OP_NOP,0
		LDY		_ptr0+1
		BEQ		loc_B8FCF
		LDA		#$DD
loc_B8FC8:
		PPUQMOVA
		DEY
		BNE		loc_B8FC8
loc_B8FCF:
		LDA		_ptr0
		LSR
		BCC		loc_B8FE1
		TAY
		LDA		_tmp2
		CLC
		ADC		#$02
		PPUQMOVA
		TYA
		BNE		loc_B8FE4
loc_B8FE1:
		BEQ		loc_B8FED
		TAY
loc_B8FE4:
		LDA		_tmp2
loc_B8FE6:
		PPUQMOVA
		DEY
		BNE		loc_B8FE6
loc_B8FED:
		PPUQEND
		EXTQSTART
		EXTQMOVB	_tmp4,OP_NOP,0
		EXTQMOVB	#$5D,OP_NOP,0
		EXTQMOVB	#$90,OP_NOP,0
		LDY		_ptr0+1
		BEQ		loc_B9010
		LDA		#$81
loc_B9009:
		EXTQMOVA
		DEY
		BNE		loc_B9009
loc_B9010:
		LDA		_ptr0
		LSR
		BCC		loc_B9018
		CLC
		ADC		#$01
loc_B9018:
		BEQ		loc_B9024
		TAY
		LDA		_tmp3
loc_B901D:
		EXTQMOVA
		DEY
		BNE		loc_B901D
loc_B9024:
		EXTQEND
		LDA		_cur_graphs_button_press_idx
		CMP		#$04
		BEQ		loc_B9034
		LDA		_prev_graphs_button_press_idx
		CMP		#$04
		BNE		locret_B9082
loc_B9034:
		LDA		_ptr0+1
		CMP		#$09
		LDX		#$B7
		LDY		#$81
		BCS		loc_B9046
		LDA		_tmp2
		CLC
		ADC		#$04
		TAX
		LDY		_tmp3
loc_B9046:
		STY		_tmp3
		STX		_tmp2
		PPUQSTART
		PPUQMOVB	_tmp4,OP_NOP,0
		PPUQMOVB	#$26,OP_NOP,0
		PPUQMOVB	#$01,OP_NOP,0
		PPUQMOVB	_tmp2,OP_NOP,0
		PPUQEND
		EXTQSTART
		EXTQMOVB	_tmp4,OP_NOP,0
		EXTQMOVB	#$5E,OP_NOP,0
		EXTQMOVB	#$01,OP_NOP,0
		EXTQMOVB	_tmp3,OP_NOP,0
		EXTQEND
locret_B9082:
		RTS

; =============== S U B	R O U T	I N E =======================================
_graphs_data_draw:
		JSR		_get_graphs_scr_period_range
		PPUQSTART
		LDY		#$02
loc_B908A:
		LDA		byte_B9117,Y
		PPUQMOVA
		DEY
		BPL		loc_B908A
		LDA		#$09
		STA		_tmp4
		LDA		_graphs_scr_period_mode
		BNE		loc_B90AD
		LDA		_city._cur_year
		LSR
		BCC		loc_B90AD
		LDA		#$01
		LDY		#$00
		JSR		sub_B914A
		JMP		loc_B90C0
loc_B90AD:
		PPUQMOVB	#$DD,OP_NOP,0
		PPUQMOVA
		LDA		#$02
		JSR		sub_B9147
		DEC		_tmp4
		BEQ		loc_B90D6
loc_B90C0:
		LDA		_ptr0+1
		CLC
		ADC		#$B0
		PPUQMOVA
		LDY		_ptr0
		LDA		byte_B9163,Y
		PPUQMOVA
		DEC		_tmp4
		BNE		loc_B90AD
loc_B90D6:
		PPUQEND
		EXTQSTART
		LDY		#$02
loc_B90DC:
		LDA		byte_B9114,Y
		EXTQMOVA
		DEY
		BPL		loc_B90DC
		LDA		#$09
		STA		_tmp4
		LDA		_graphs_scr_period_mode
		BNE		loc_B90F5
		LDA		_city._cur_year
		LSR
		BCS		loc_B9103
loc_B90F5:
		EXTQMOVB	#$81,OP_NOP,0
		EXTQMOVA
		DEC		_tmp4
		BEQ		loc_B9111
loc_B9103:
		EXTQMOVB	#$06,OP_NOP,0
		EXTQMOVA
		DEC		_tmp4
		BNE		loc_B90F5
loc_B9111:
		EXTQEND
		RTS
byte_B9114:
		.BYTE	$12,$5F,$2A
byte_B9117:
		.BYTE	$12,$27,$2A

; =============== S U B	R O U T	I N E =======================================
_get_graphs_scr_period_range:
		LDA		#$0A
		LDX		_graphs_scr_period_mode
		BEQ		loc_B9123
		LDA		#$64
loc_B9123:
		STA		_ptr0
		LDA		_city._cur_year
		SEC
		SBC		_ptr0
		TAY
		LDA		_city._cur_year+1
		SBC		#$00
		BCS		loc_B9136
		LDA		#$00
		TAY
loc_B9136:
		STA		_ptr0+1
		STY		_ptr0
		JSR		_hex_to_dec9999999_0
		LDA		_graphs_scr_period_mode
		BEQ		locret_B9146
		LDA		#$00
		STA		_ptr0
locret_B9146:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B9147:
		LDY		_graphs_scr_period_mode

; =============== S U B	R O U T	I N E =======================================
sub_B914A:
		CLC
		ADC		$00,Y
		CMP		#$0A
		BCC		loc_B915F
		LDA		#$00
		STA		$00,Y
		LDA		#$01
		INY
		CPY		#$04
		BCC		sub_B914A
		RTS
loc_B915F:
		STA		$00,Y
		RTS
byte_B9163:
		.BYTE	$BA,$DD,$BB,$DD,$BC,$DD,$BD,$DD,$BE,$DD

; =============== S U B	R O U T	I N E =======================================
_flush_wait:
		JSR		_wait_for_nmi

; =============== S U B	R O U T	I N E =======================================
_flush_test_wait:
		LDA		_extnt_queue_pos_head
		CMP		_extnt_queue_pos_tail
		BNE		_flush_wait
		LDA		_ppu_queue_pos_head
		CMP		_ppu_queue_pos_tail
		BNE		_flush_wait
		RTS

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl2_B_budget_input:
		JSR		_budget_tax_change_input
		LDA		_pad0_held
		BNE		loc_B920A
		LDA		byte_596
		CLC
		ADC		#$01
		AND		#$7F
		STA		byte_596
		BEQ		loc_B91F7
		JMP		loc_B927C
loc_B91F7:
		LDA		byte_597
		CLC
		ADC		#$01
		STA		byte_597
		CMP		#$07
		BCC		loc_B927C
		LDA		#$80
		STA		_pad0_held
		BNE		loc_B9243
loc_B920A:
		LDA		#$00
		STA		byte_596
		STA		byte_597
		JSR		_budget_wnd_cursor_draw
		LDA		_pad0_autorep_release
		AND		#$0F
		BEQ		loc_B9221
		JSR		sub_B92DA
		JMP		loc_B927C
loc_B9221:
		BIT		_pad0_autorep_release
		BPL		loc_B927C
		LDA		#$01
		STA		_apu_se_flags
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCS		loc_B9238
		FJSRA	_budget_funds_recalc,PRG7,PRG9
		RTS
loc_B9238:
		BNE		loc_B9243
		BIT		_city._bank_flags
		BPL		loc_B9243
		JMP		_bank_screen_init

loc_B9243:
		LDA		#$00
		STA		byte_596
		STA		byte_597
		JSR		_current_funds_calc
		BCC		loc_B927C
		LDA		_budget_screen_return_to_info_maps_flag
		BEQ		.budget_normal_exit
		JSR		_pal_fade_out					; exit from budget screen to info maps screen
		LDA		#$00
		STA		_scr_res_idx					; set the info maps index, NOT CLEAR IT HERE!
		STA		_fiscal_year_results_draw_req
		STA		_budget_screen_return_to_info_maps_flag
		LDA		#$48
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$28
		STA		_cur_game_cursor_scr_pos._COL
		LDA		#$10
		STA		_scr_res_control_flags
		RTS
.budget_normal_exit:
		LDA		#$00
		STA		_fiscal_year_results_draw_req
		LDA		#$40
		STA		_scr_res_control_flags
		RTS
loc_B927C:
		JSR		_budget_wnd_cursor_pos_set
		JSR		sub_B9473
		JMP		_budget_wnd_cursor_draw

; =============== S U B	R O U T	I N E =======================================
_bank_screen_init:
		LDA		#$06
		STA		_scr_res_idx
		LDA		#$10
		STA		_scr_res_control_flags
		JMP		_pal_fade_out


; =============== S U B	R O U T	I N E =======================================
_current_funds_calc:
		LDY		#$00
		LDX		#$03
		SEC
loc_B9298:
		LDA		_money_plus_taxes_collected,Y
		SBC		_total_funds,Y
		INY
		DEX
		BNE		loc_B9298
		RTS

; =============== S U B	R O U T	I N E =======================================
_save_taxes_data:
		LDA		_tax_alloc._tax
		STA		_city._tax_rate
		MOVW	_fire_fund_alloc,_tax_alloc._fire
		MOVW	_police_fund_alloc,_tax_alloc._police
		MOVW	_road_fund_alloc,_tax_alloc._trans
		MOVW	_education_fund_alloc,_tax_alloc._education
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B92DA:
		BIT		_city._bank_flags
		BPL		loc_B9312
		LDA		_pad0_autorep_release
		AND		#$0C
		BEQ		loc_B9312
		LDX		_budget_tax_cur_pos
		CMP		#$08
		BNE		loc_B92FE
		CPX		#$02
		BCC		loc_B92F8
		CPX		#$0A
		BCC		loc_B9312
		DEC		_budget_tax_cur_pos
		RTS
loc_B92F8:
		LDA		#$0B
		STA		_budget_tax_cur_pos
		RTS
loc_B92FE:
		CPX		#$08
		BCC		loc_B9312
		BEQ		loc_B930C
		CPX		#$0B
		BEQ		loc_B9312
		INC		_budget_tax_cur_pos
		RTS
loc_B930C:
		LDA		#$0A
		STA		_budget_tax_cur_pos
		RTS
loc_B9312:
		LDA		_pad0_autorep_release
		AND		#$0F
		CMP		#$04
		AND		#$05
		BCS		loc_B9334
		BNE		loc_B9329
		LDA		_budget_tax_cur_pos
		AND		#$01
		BEQ		locret_B9365
		LDA		#$FF
		BNE		loc_B935A
loc_B9329:
		LDA		_budget_tax_cur_pos
		AND		#$01
		BNE		locret_B9365
		LDA		#$01
		BNE		loc_B935A
loc_B9334:
		BNE		loc_B9349
		LDA		_budget_tax_cur_pos
		CMP		#$02
		BCS		loc_B9345
		AND		#$01
		CLC
		ADC		#$0C
		STA		_budget_tax_cur_pos
loc_B9345:
		LDA		#$FE
		BNE		loc_B935A
loc_B9349:
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCC		loc_B9358
		AND		#$01
		CLC
		ADC		#$FE
		STA		_budget_tax_cur_pos
loc_B9358:
		LDA		#$02
loc_B935A:
		CLC
		ADC		_budget_tax_cur_pos
		STA		_budget_tax_cur_pos
		LDX		#$40
		STX		_apu_se_flags+1
locret_B9365:
		RTS

; =============== S U B	R O U T	I N E =======================================
_budget_wnd_cursor_pos_set:
		BIT		_city._bank_flags
		BPL		loc_B937D
		LDA		_budget_tax_cur_pos
		CMP		#$0A
		BCC		loc_B937D
		BNE		loc_B937D
		LDA		#$B8
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$D4
		STA		_cur_game_cursor_scr_pos._COL
		RTS
loc_B937D:
		LDX		#$00
		LDA		_budget_tax_cur_pos
		LSR
		BCC		loc_B9386
		INX
loc_B9386:
		TAY
		LDA		byte_B9392,Y
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		byte_B9399,X
		STA		_cur_game_cursor_scr_pos._COL
		RTS
byte_B9392:
		.BYTE	$3C,$6C,$74,$7C,$84,$D4,$00
byte_B9399:
		.BYTE	$AC,$B4

; =============== S U B	R O U T	I N E =======================================
_budget_wnd_cursor_draw:
		LDX		#$10
		STX		_ptr0
		LDX		_spr_buf_pos
		LDA		byte_597
		ASL
		ASL
		ASL
		ASL
		TAY
loc_B93A9:
		LDA		byte_B93D7,Y
		STA		_spr_buf,X
		INY
		INX
		DEC		_ptr0
		BNE		loc_B93A9
		LDA		byte_597
		CMP		#$06
		BEQ		loc_B93D4
		LDA		#$04
		STA		_ptr0
		LDA		byte_596
		AND		#$30
		LSR
		LSR
		TAY
loc_B93C8:
		LDA		byte_B9447,Y
		STA		_spr_buf,X
		INY
		INX
		DEC		_ptr0
		BNE		loc_B93C8
loc_B93D4:
		STX		_spr_buf_pos
		RTS
byte_B93D7:
		.BYTE	$38,$B0,$02,$18,$38,$B0,$42,$1F,$02,$FA,$02,$FA,$02,$FA,$42,$FA
		.BYTE	$38,$B1,$02,$18,$38,$B1,$42,$1F,$42,$B2,$02,$18,$42,$B2,$42,$1F
		.BYTE	$38,$B3,$02,$18,$38,$B3,$42,$1F,$42,$B4,$02,$18,$42,$B4,$42,$1F
		.BYTE	$38,$B5,$02,$18,$38,$B5,$42,$1F,$42,$B6,$02,$18,$42,$B6,$42,$1F
		.BYTE	$38,$B7,$02,$18,$38,$B7,$42,$1F,$42,$B8,$02,$18,$42,$B8,$42,$1F
		.BYTE	$38,$B9,$02,$18,$38,$B9,$42,$1F,$42,$BA,$02,$18,$42,$BA,$42,$1F
		.BYTE	$02,$FA,$02,$FA,$02,$FA,$42,$FA,$42,$BB,$02,$18,$42,$BB,$42,$1F
byte_B9447:
		.BYTE	$40,$BC,$02,$18,$41,$BC,$02,$18,$42,$BC,$02,$18,$43,$BC,$02,$18

; =============== S U B	R O U T	I N E =======================================
_budget_tax_change_input:
		LDA		_pad0_held
		BMI		loc_B9461
		LDA		#$00
		STA		_budget_tax_adjust_value
		RTS
loc_B9461:
		LDA		_pad0_autorep_release
		BPL		locret_B9472
		INC		_budget_tax_adjust_value
		LDA		_budget_tax_adjust_value
		CMP		#$40
		BCC		locret_B9472
		DEC		_budget_tax_adjust_value
locret_B9472:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B9473:
		LDA		_city._bank_flags
		BEQ		locret_B94B4
		LDA		#$00
		STA		_tmp2
		STA		_tmp4
		INC		_tmp118+2
		LDA		_tmp118+2
		CMP		#$30
		BCC		loc_B948A
		LDA		#$00
loc_B948A:
		STA		_tmp118+2
		LSR
		LSR
		LSR
		TAX
		LDA		byte_B94C7,X
		STA		_tmp3
		CPX		#$03
		BEQ		loc_B94A9
		CPX		#$06
		BEQ		loc_B94A9
		MOVWO	_ptr0,byte_B94B5
		JMP		_spr_raw_insert
loc_B94A9:
		MOVWO	_ptr0,byte_B94BE
		JMP		_spr_raw_insert
locret_B94B4:
		RTS

byte_B94B5:
		.BYTE	$A0,$43,$00,$BB
		.BYTE	$A0,$44,$00,$C3
		.BYTE	$FF
byte_B94BE:
		.BYTE	$A0,$41,$00,$BB
		.BYTE	$A0,$42,$00,$C3
		.BYTE	$FF
byte_B94C7:
		.BYTE	$02,$02,$02,$00,$00,$02

; =============== S U B	R O U T	I N E =======================================
_info_maps_spr_rect_draw:
		LDA		#$00
		STA		_tmp3
		LDA		_info_maps_rect_pos._ROW
		STA		_tmp2
		LDA		_info_maps_rect_pos._COL
		STA		_tmp4
		MOVWO	_ptr0,_spr_map_info_rect
		JMP		_spr_raw_insert

; =============== S U B	R O U T	I N E =======================================
_info_maps_spr_arrows_draw:
		LDA		_info_maps_rect_pos._ROW
		STA		_tmp2
		LDA		_info_maps_rect_pos._COL
		STA		_tmp4
		LDA		#$00
		STA		_tmp3
		MOVWO	_ptr0,_spr_map_info_arrows

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_spr_raw_insert:
		LDY		#$00
		LDX		_spr_buf_pos
loc_B9500:
		LDA		(_ptr0),Y
		CMP		#$FF
		BEQ		loc_B952A
		CLC
		ADC		_tmp2
		STA		_spr_buf,X
		INX
		INY
		LDA		(_ptr0),Y
		STA		_spr_buf,X
		INX
		INY
		LDA		(_ptr0),Y
		ORA		_tmp3
		STA		_spr_buf,X
		INX
		INY
		LDA		(_ptr0),Y
		CLC
		ADC		_tmp4
		STA		_spr_buf,X
		INX
		INY
		BNE		loc_B9500
loc_B952A:
		STX		_spr_buf_pos
		RTS

_spr_map_info_rect:
		.BYTE	$00,$04,$00,$00
		.BYTE	$00,$05,$00,$2F
		.BYTE	$28,$06,$00,$00
		.BYTE	$28,$07,$00,$2F
		.BYTE	$FF
_spr_map_info_arrows:
		.BYTE	$F0,$8B,$03,$14
		.BYTE	$F0,$8C,$03,$1C
		.BYTE	$F8,$8D,$03,$18
		.BYTE	$38,$8B,$83,$14
		.BYTE	$38,$8C,$83,$1C
		.BYTE	$30,$8D,$83,$18
		.BYTE	$10,$8F,$03,$3F
		.BYTE	$18,$9F,$03,$3F
		.BYTE	$14,$8E,$03,$37
		.BYTE	$10,$8F,$43,$F0
		.BYTE	$18,$9F,$43,$F0
		.BYTE	$14,$8E,$43,$F8
		.BYTE	$FF

; =============== S U B	R O U T	I N E =======================================
_scr_res_hndl5_A_history_draw:
		LDA		_scr_res_idx
		CMP		#$0A
		BNE		locret_B95EC
		LDA		#$02
		STA		_MMC5_CHR_MODE
		LDA		#$48
		STA		_tmp118+7
		LDA		#$00
		STA		_tmp118+8
		STA		_tmp118+9
		STA		_tmp118+$A
loc_B958B:
		LDX		_tmp118+8
		CPX		#$15
		BCS		loc_B95E1
		LDA		_city._history_list,X
		BEQ		loc_B95E1
		INC		_tmp118+8
		TAX
		DEX
		LDA		_tmp118+$A
		CLC
		ADC		#$06
		FJSRA	_printf_txt_lib,PRG7,PRG9
		FJSRA	_printf_history_year,PRG7,PRG9
		JSR		_buffers_force_flush
		PPUQSTART
		LDY		_tmp118+9
		LDA		word_B95ED,Y
		INY
		PPUQMOVA
		LDA		word_B95ED,Y
		INY
		PPUQMOVA
		PPUQMOVB	#$01,OP_NOP,0
		PPUQMOVB	#$3A,OP_NOP,0
		PPUQEND
		STY		_tmp118+9
		INC		_tmp118+7
		INC		_tmp118+$A
		BNE		loc_B958B
loc_B95E1:
		INC		_scr_res_idx
		JSR		_buffers_force_flush
		LDA		#$01
		STA		_MMC5_CHR_MODE
locret_B95EC:
		RTS
word_B95ED:
		.WORD	$24C8
		.WORD	$2528
		.WORD	$2588
		.WORD	$25E8
		.WORD	$2648
		.WORD	$26A8
		.WORD	$2708
;_unused_10:
;		.BYTE	$00,$06,$07,$08,$09,$0A,$0B,$0C

; =============== S U B	R O U T	I N E =======================================
_name_bufs_reset:
		LDY		#$0A
		LDA		#$7C
loc_B9607:
		STA		_city._name,Y
		STA		_city._sister_name,Y
		STA		_city._bank_name,Y
		STA		_city._unused_name3,Y
		STA		_city._unused_name4,Y
		DEY
		BNE		loc_B9607
		LDY		#$01
		STY		_city._name
		STY		_city._sister_name
		STY		_city._bank_name
		STY		_city._unused_name3
		STY		_city._unused_name4
		DEY
		STY		byte_6DC+2
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_B962F:
		LDY		_cur_info_maps_button_idx
		LDA		byte_B9665,Y
		TAY
		LDA		_info_maps_city_map_modes_idx,Y
		STA		_ptr0
		TYA
		ASL
		ASL
		CLC
		ADC		_ptr0
		TAX
		LDA		byte_B9659,X
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		byte_B964D,X
		STA		_cur_game_cursor_scr_pos._COL
		RTS
byte_B964D:
		.BYTE	$60,$90,$60,$90,$60,$98,$60,$98,$60,$90,$60,$90
byte_B9659:
		.BYTE	$50,$50,$78,$78,$80,$80,$80,$80,$B8,$B8,$B8,$B8
byte_B9665:
		.BYTE	$00,$FF,$01,$FF,$FF,$FF,$FF,$FF,$02

; =============== S U B	R O U T	I N E =======================================
sub_B966E:
		LDY		#$00
		LDX		#$60
loc_B9672:
		LDA		byte_B9681,Y
		STA		_spr_buf,X
		INX
		INY
		CPY		#$20
		BCC		loc_B9672
		STX		_spr_buf_pos
		RTS
byte_B9681:
		.BYTE	$80,$90,$00,$40
		.BYTE	$80,$91,$00,$48
		.BYTE	$80,$92,$00,$50
		.BYTE	$80,$93,$00,$58
		.BYTE	$80,$94,$00,$60
		.BYTE	$80,$A0,$00,$68
		.BYTE	$80,$A1,$00,$70
		.BYTE	$80,$A2,$00,$78

; =============== S U B	R O U T	I N E =======================================
sub_B96A1:
		PHA
		LDY		#$FF
		BIT		byte_15E
		BPL		loc_B96AB
		LDY		#$02
loc_B96AB:
		LDA		byte_15E
loc_B96AE:
		LSR
		INY
		BCC		loc_B96AE
		LDA		byte_B96BB,Y
		TAX
		PLA
		STA		_spr_buf,X
		RTS
byte_B96BB:
		.BYTE	$1D,$21,$25,$11,$15,$19

; =============== S U B	R O U T	I N E =======================================
_wnd_header_year_draw:
		MOVW	_ptr0,_city._cur_year
		JSR		_hex_to_dec9999999_0
		PPUQSTART
		LDA		_scr_res_idx
		AND		#$FE
		CMP		#$04
		BNE		loc_B96F9
		LDA		#$A1
		STA		_tmp4
		LDA		#$67
		JSR		sub_B9754
		LDA		#$B1
		STA		_tmp4
		LDA		#$87
		JSR		sub_B9754
		PPUQEND
		MOVWO	_tmp2,byte_B9774
		JMP		_info_wnd_year_extnt_queue

loc_B96F9:
		LDA		#$00
		STA		_tmp4
		LDA		#$66
		JSR		_info_wnd_year_nt_queue
		LDA		#$40
		STA		_tmp4
		LDA		#$86
		JSR		_info_wnd_year_nt_queue
		PPUQEND
		MOVWO	_tmp2,off_B9745
		JMP		_info_wnd_year_extnt_queue


; =============== S U B	R O U T	I N E =======================================
_info_wnd_year_nt_queue:
		PPUQMOVA
		PPUQMOVB	#$24,OP_NOP,0
		PPUQMOVB	#$04,OP_NOP,0
		LDY		#$03
loc_B972B:
		LDA		_ptr0,Y
		CLC
		ADC		#$1B
		CMP		#$20
		BCC		loc_B973B
		LDA		_ptr0,Y
		CLC
		ADC		#$36
loc_B973B:
		ADC		_tmp4
		PPUQMOVA
		DEY
		BPL		loc_B972B
		RTS
off_B9745:
		.WORD	_MMC5_EXRAM+$66
		.BYTE	$04,$98,$98,$98,$98
		.WORD	_MMC5_EXRAM+$86
		.BYTE	$04,$98,$98,$98,$98
		.BYTE	0

; =============== S U B	R O U T	I N E =======================================
sub_B9754:
		PPUQMOVA
		PPUQMOVB	#$24,OP_NOP,0
		PPUQMOVB	#$04,OP_NOP,0
		LDY		#$03
loc_B9766:
		LDA		_ptr0,Y
		CLC
		ADC		_tmp4
		PPUQMOVA
		DEY
		BPL		loc_B9766
		RTS
byte_B9774:
		.BYTE	$67,$5C,$04,$4B,$4B,$4B,$4B,$87,$5C,$04,$4B,$4B,$4B,$4B,$00

; =============== S U B	R O U T	I N E =======================================
_info_wnd_year_extnt_queue:
		EXTQSTART
		LDY		#$00
loc_B9787:
		LDA		(_tmp2),Y
		BEQ		loc_B9792
		EXTQMOVA
		INY
		BNE		loc_B9787
loc_B9792:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_common_cursor_spr_clear:
		LDX		#$0F
		LDA		#$FA
loc_B97FC:
		STA		_spr_buf,X
		DEX
		BPL		loc_B97FC
		RTS

; =============== S U B	R O U T	I N E =======================================
_hex_to_dec9999999_0:
		LDY		#$00
		STY		_tmp2
		LDX		#$03
		LDY		#$00
		STY		_tmp3
		STY		_tmp4
		STY		_tmp5
		STY		_tmp6
		STY		_tmp7
loc_B97A7:
		LDY		#$00
loc_B97A9:
		LDA		_tmp0
		CMP		_hex_to_dec_tbl0,X
		LDA		_tmp1
		SBC		_hex_to_dec_tbl1,X
		LDA		_tmp2
		SBC		_hex_to_dec_tbl2,X
		BCC		loc_B97D2
		LDA		_tmp0
		SBC		_hex_to_dec_tbl0,X
		STA		_tmp0
		LDA		_tmp1
		SBC		_hex_to_dec_tbl1,X
		STA		_tmp1
		LDA		_tmp2
		SBC		_hex_to_dec_tbl2,X
		STA		_tmp2
		INY
		BNE		loc_B97A9
loc_B97D2:
		STY		_tmp2,X
		DEX
		BPL		loc_B97A7
		LDX		#$00
loc_B97D9:
		LDA		_tmp2,X
		STA		_tmp1,X
		INX
		CPX		#$06
		BNE		loc_B97D9
		RTS

; REDUNDANT, merged in system bank
;
;_hex_to_dec_tbl2:
;		.BYTE	[[10 >> 16] & $FF]		; $00
;		.BYTE	[[100 >> 16] & $FF]		; $00
;		.BYTE	[[1000 >> 16] & $FF]	; $00
;		.BYTE	[[10000 >> 16] & $FF]	; $00
;		.BYTE	[[100000 >> 16] & $FF]	; $01
;		.BYTE	[[1000000 >> 16] & $FF] ; $0F
;		.BYTE	[[10000000 >> 16] & $FF]; $98
;_hex_to_dec_tbl1:
;		.BYTE	[[10 >> 8] & $FF]		; $00
;		.BYTE	[[100 >> 8] & $FF]		; $00
;		.BYTE	[[1000 >> 8] & $FF]		; $03
;		.BYTE	[[10000 >> 8] & $FF]	; $27
;		.BYTE	[[100000 >> 8] & $FF]	; $86
;		.BYTE	[[1000000 >> 8] & $FF]  ; $42
;		.BYTE	[[10000000 >> 8] & $FF] ; $96
;_hex_to_dec_tbl0:
;		.BYTE	[[10 >> 0] & $FF]		; $0A
;		.BYTE	[[100 >> 0] & $FF]		; $64
;		.BYTE	[[1000 >> 0] & $FF]		; $E8
;		.BYTE	[[10000 >> 0] & $FF]	; $10
;		.BYTE	[[100000 >> 0] & $FF]	; $A0
;		.BYTE	[[1000000 >> 0] & $FF]  ; $40
;		.BYTE	[[10000000 >> 0] & $FF] ; $80

; REDUNDANT, two identical copy-pasted hex to str routines with 9999999 range
; used here twice to convert year to string lol
; first, we remove copy and not needed data, then we merge it with other
; commont str convert routines
;
; =============== S U B	R O U T	I N E =======================================
;_hex_to_dec9999999_0:
;		LDY		#$00
;		STY		_tmp2
;		LDX		#$03
;		LDY		#$00
;		STY		_tmp3
;		STY		_tmp4
;		STY		_tmp5
;		STY		_tmp6
;		STY		_tmp6+1
;loc_B917F:
;		LDY		#$00
;loc_B9181:
;		LDA		_ptr0
;		CMP		_hex_to_dec_tbl0,X
;		LDA		_ptr0+1
;		SBC		_hex_to_dec_tbl1,X
;		LDA		_tmp2
;		SBC		_hex_to_dec_tbl2,X
;		BCC		loc_B91AA
;		LDA		_ptr0
;		SBC		_hex_to_dec_tbl0,X
;		STA		_ptr0
;		LDA		_ptr0+1
;		SBC		_hex_to_dec_tbl1,X
;		STA		_ptr0+1
;		LDA		_tmp2
;		SBC		_hex_to_dec_tbl2,X
;		STA		_tmp2
;		INY
;		BNE		loc_B9181
;loc_B91AA:
;		STY		_tmp2,X
;		DEX
;		BPL		loc_B917F
;		LDX		#$00
;loc_B91B1:
;		LDA		_tmp2,X
;		STA		_ptr0+1,X
;		INX
;		CPX		#$06
;		BNE		loc_B91B1
;		RTS
;
;_hex_to_dec_tbl2:
;		.BYTE	[[10 >> 16] & $FF]		; $00
;		.BYTE	[[100 >> 16] & $FF]		; $00
;		.BYTE	[[1000 >> 16] & $FF]	; $00
;		.BYTE	[[10000 >> 16] & $FF]	; $00
;		.BYTE	[[100000 >> 16] & $FF]	; $01
;		.BYTE	[[1000000 >> 16] & $FF] ; $0F
;		.BYTE	[[10000000 >> 16] & $FF]; $98
;_hex_to_dec_tbl1:
;		.BYTE	[[10 >> 8] & $FF]		; $00
;		.BYTE	[[100 >> 8] & $FF]		; $00
;		.BYTE	[[1000 >> 8] & $FF]		; $03
;		.BYTE	[[10000 >> 8] & $FF]	; $27
;		.BYTE	[[100000 >> 8] & $FF]	; $86
;		.BYTE	[[1000000 >> 8] & $FF]  ; $42
;		.BYTE	[[10000000 >> 8] & $FF] ; $96
;_hex_to_dec_tbl0:
;		.BYTE	[[10 >> 0] & $FF]		; $0A
;		.BYTE	[[100 >> 0] & $FF]		; $64
;		.BYTE	[[1000 >> 0] & $FF]		; $E8
;		.BYTE	[[10000 >> 0] & $FF]	; $10
;		.BYTE	[[100000 >> 0] & $FF]	; $A0
;		.BYTE	[[1000000 >> 0] & $FF]  ; $40
;		.BYTE	[[10000000 >> 0] & $FF] ; $80

		SECTION_END 903

		BANK_END F7,$A000
