
		ORG $A000

		BANK_START $FB

; ---------------------------------------------------------------------------
; ---------C-I-T-Y--C-O-R-E--M-A-I-N--E-N-G-I-N-E--R-O-U-T-I-N-E-S-----------
; ---------------------------------------------------------------------------

		SECTION_START	B00

; =============== S U B	R O U T	I N E =======================================
; Here is the main loop for city engine. Performs continuously calculations
; on a different city areas and updating statuses.
;
; at the beginning we do initial calcs without entering critical section,
; so at the beginning of the game we may see a black screen sometime longer
; than usual. However, you still have a priority with controls, so this may
; be fixed by pressing buttons oc controller.
;
_THREAD1_MAIN:
		FJSR	_RCI_meters_recalc,SRAM,PRGA	; probably should be after clearing values here?
		JSR		_overall_stats_clear			; clear statistic counters and average values
		JSR		_overall_stats_recalc			; collect statistic for current map
		JSR		_interpolate_police_fire_maps	; police and fire maps are initially updated in prev routine
		FJSR	_power_grid_recalc,SRAM,PRGA	; power connections and consumption recalc
		JSR		_land_value_recalc				; land map and average recalc
		JSR		_crime_recalc			; crime map and average recalc
		JSR		_population_dencity_recalc		; population dencity and with migration point
		LDA		#$FF
		STA		_thread1_flags					; disable all ctitical sections, now all priorities are
		STA		_thread1_flags+1				; reset
		FJSR	_city_stats_recalc,SRAM,PRGA	; must be the city evaluation recalc but at this point
		LDA		#$00							; but most time levels here are zero, so calc not performed
		STA		_city._cur_week

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_THREAD1_LOOP:
		JSR		_screen_invalidate				; priority as usual in THREAD0
		INC		_city._cur_week					; one loop - one week here
		LDA		_city._cur_week
		LSR
		BCC		_rci_meters_calc_skip
		FJSR	_RCI_meters_recalc,SRAM,PRGA	; recalc RCI once in two weeks
_rci_meters_calc_skip:
		JSR		_map_corruption_tiles_animate	; animate destroyed map tiles
		LDA		_frames_counter					; reseed RNG, so we have different behaviour
		STA		_rand_seed						; on a different starts
		ADDWB	_city._tax_year_sum,_city._tax_rate	; summing tax rates every week
		MOVWI	_power_nodes_cnt,0				; reset power grid nodes list
		LDA		#$07
		STA		_thread0_flags+1				; take some priority
		JSR		_overall_stats_clear			; clear stats again
		JSR		_overall_stats_recalc			; collect stats one more time
		LDA		#$03
		STA		_thread0_flags+1
		JSR		_interpolate_police_fire_maps	; do the same as on start, but with some
		LDA		#$00							; priority masking
		STA		_thread0_flags+1
		JSR		_map_corruption_tiles_animate	; animate map again. a lot of them here
		LDA		_city._cur_week
		AND		#$03
		BNE		.default_city_calcs				; one month = 4 weeks, do additional calcs
		LDA		#$03
		STA		_thread1_flags+1
		INC		_city._cur_month				; yep, increase months
		LDA		_city._cur_month
		CMP		#$0C
		BNE		.default_month_work				; year = 12 months
		LDA		#$00
		STA		_city._cur_month				; reset monts, increase years
		LDA		_city._cur_year+1				; unless it not 9999, or...
		CMP		#$27
		BNE		.do_year_increase
		LDA		_city._cur_year
		CMP		#$0F
		BEQ		.skip_year_increase				; ...the end of the time
.do_year_increase:
		INCW	_city._cur_year
.skip_year_increase:
		FJSR	_graphs_10_year_data_update,SRAM,PRGA	; update graphs per year
		FJSR	_fiscal_year_results_calc,SRAM,PRGA		; recalc taxes and display budget screen
		FJSR	_city_stats_recalc,SRAM,PRGA	; update city evaluation stats and traffic map!
		JSR		_map_corruption_tiles_animate	; animate again, to much time takes this loop
		FJSR	_test_for_new_decade,SRAM,PRGA	; test for the last decimal nibble of year
		LDA		_tmpE6							; if 0, then upadte 100 year graphs as well
		BNE		.default_month_work
		FJSR	_graphs_100_year_data_update,SRAM,PRGA	; currently just copy last 10year stats
.default_month_work:
		JSR		_population_growth_recalc		; we have pop growth recalc here for the first time
		JSR		_map_corruption_tiles_animate	; and animate again. if not animate it will stay forever
.default_city_calcs:
		JSR		_traffic_smooth			; soften traffic dencity map, TODO: test me, maybe too soft?
		JSR		_map_corruption_tiles_animate	; ANIMATE AGAIN!!!
		FJSR	_events_selector,SRAM,PRGA		; now chek if the events messages need to be displayed
.game_stop_loop:
		LDY		_city._game_speed				; when city is in pause mode, we will stop here
		CPY		#$03							; and loop forever. means no other calcs on town anymore
		BNE		.game_core_calc
		JSR		_screen_invalidate
		JMP		.game_stop_loop
.game_core_calc:
		LDA		_power_grid_recalc_delay_mask,Y	; another speed limit option goes here
		AND		_city._cur_week					; for fast mode power connection and consumption
		BNE		.skip_power_grid_calc			; performed only once per month, very time spending
		LDA		#$02							; procedure. slow and normal speeds are takes
		JSR		_critical_section_enter			; full power scan every week
		FJSR	_power_grid_recalc,SRAM,PRGA
		LDA		#$02
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate	; also animate tiles too
.skip_power_grid_calc:
		LDY		_city._game_speed				; for the rest of recalculations we have three
		LDA		_game_core_speed_mask_list,Y	; three speed options. all recalcs for fast mode go
		AND		_city._cur_week					; every 16 weeks, normal - every 8, slow - every 4.
;		CMP		_land_value_recalc_delay_list,Y	; land values calculated first within each delay
		BNE		loc_11E135						; period, at counter 0
		LDA		#$A0							; all these recalcs are time consuming, so time for
		JSR		_critical_section_enter			; complete this loop increased with a number of
		JSR		_land_value_recalc				; calcs performes, so for fast modes they just skip
		LDA		#$A0							; all except main evaluations
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate
loc_11E135:
		LDY		_city._game_speed				; crime level calculations performed at 2/4
		LDA		_game_core_speed_mask_list,Y	; of the delay period
		AND		_city._cur_week					; NOTE, when you select pause mode after
		CMP		_crime_recalc_delay_list,Y		; the stop loop above, you select fourth
		BNE		loc_11E153						; set of delay counters and masks, which
		LDA		#$40							; are always 0, so when you get here and change
		JSR		_critical_section_enter			; speed, at least two other calculations will
		JSR		_crime_recalc			; be forced
		LDA		#$40
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate
loc_11E153:
		LDY		_city._game_speed				; and finally, population recalcs performed
		LDA		_game_core_speed_mask_list,Y	; at 3/4 of the delay period.
		AND		_city._cur_week					; so all three recalcs performed once per loop
		CMP		_population_recalc_delay_list,Y	; at a different weeks, never calculated
		BNE		loc_11E171						; at once
		LDA		#$04
		JSR		_critical_section_enter
		JSR		_population_dencity_recalc
		LDA		#$04
		JSR		_critical_section_leave			; animate map corruptions every time we exit the
		JSR		_map_corruption_tiles_animate	; long routine.
loc_11E171:
		JSR		_disasters_handlers_select		; and finally at the end of the week we select the
		JSR		_map_corruption_tiles_animate	; desister if enabled manually or for scenario
		JMP		_THREAD1_LOOP

_power_grid_recalc_delay_mask:
		.BYTE	$03,$00,$00,$00
_game_core_speed_mask_list:
		.BYTE	$0F,$07,$03,$00
;_land_value_recalc_delay_list:
;		.BYTE	$00,$00,$00,$00					; REDUNDAT, actually this not needed, because all zeroes
_crime_recalc_delay_list:
		.BYTE	$04,$02,$01,$00
_population_recalc_delay_list:
		.BYTE	$08,$04,$02,$00

; =============== S U B	R O U T	I N E =======================================
_screen_invalidate:
		LDA		#$FF							; set flag to the chain of redraw functions in THREAD0
		STA		_hud_redraw_req					; redraws upper hud, tool palette, and game screen
		LDA		#$01
		STA		_game_core_state				; switch to the THREAD0 on next nmi
loc_11E198:
		LDA		_hud_redraw_req					; wait for request is accepted
		BNE		loc_11E198
		RTS

; =============== S U B	R O U T	I N E =======================================
_overall_stats_recalc:
		LDY		#$00							; collect info about city
loc_11E1A0:
		LDX		#$00
loc_11E1A2:
		TXA
		PHA
		TYA
		PHA
		STX		_tmp_map_pos._COL
		STY		_tmp_map_pos._ROW
		JSR		_get_cur_building_tile			; read the actual tile index
		JSR		_map_tile_stats_collect			; collect info
		PLA										; now we have stats about
		TAY										; how much buildings/transport etc.
		PLA										; and ready to calculate next
		TAX										; city state and stats
		INX
		CPX		#$4C
		BNE		loc_11E1A2
		INY
		CPY		#$4C
		BNE		loc_11E1A0
		LDY		_airports_count					; if any airport built
		BEQ		.no_airports
		LDA		_obj_active_flags				; test if obj already deployed
		STA		_tmpE6
		LSR		_tmpE6
		BCS		loc_11E1F2
		ORA		#$01							; init deploy of helicopter object
		STA		_obj_active_flags
		LDY		_last_airport_pos._COL			; starting point is last checked
		INY										; airport on map (so it will be always
		INY										; bottom-rightmost airport)
		STY		_obj_pos._heli._COL
		INY
		STY		_obj_dst_pos._heli._COL
		LDY		_last_airport_pos._ROW
		INY
		INY
		STY		_obj_pos._heli._ROW
		INY
		STY		_obj_dst_pos._heli._ROW
		LDA		#$03
		STA		_obj_cur_spr_idx._heli
		BNE		loc_11E230
loc_11E1F2:
		LSR		_tmpE6
		BCS		loc_11E23F						; test if airplane in flight as well
;		LDY		_airports_count					; redundant, already checked
;		BEQ		.no_airports
		ORA		#$02
		STA		_obj_active_flags				; start to take off animation
		LDA		#$84
		STA		_obj_plane_anim_idx
		LDA		#$20
		STA		_obj_plane_anim_timer
		LDA		_last_airport_pos._COL
		STA		_obj_dst_pos._plane._COL
		CLC
		ADC		#$03
		STA		_obj_pos._plane._COL
		LDY		_last_airport_pos._ROW
		STY		_obj_pos._plane._ROW
		INY
		INY
		STY		_obj_dst_pos._plane._ROW
		LDA		#$06
		STA		_obj_cur_spr_idx._plane
		BNE		loc_11E230
.no_airports:
		LDA		_obj_active_flags				; no airports - no flying objects
		AND		#$FC							; remove existing if airport destroyed
		STA		_obj_active_flags
; NEW: re-implement another ship starting procedures, not intended to be used in original version
; -
loc_11E23F:
		LDA		_sea_ports_count
		BEQ		._no_sea_ports
		LDA		_obj_active_flags				; test if obj already deployed
		AND		#$04
		BNE		loc_11E230						; test if already deployed
		LDA		_obj_active_flags
		ORA		#$04
		STA		_obj_active_flags
		LDA		_obj_pos._ship._COL				; set start pos to end pos
		STA		_obj_dst_pos._ship._COL			; usually bottom-rightmost water tile
		LDY		_obj_pos._ship._ROW
		STY		_obj_dst_pos._ship._ROW
		LDA		#$07							; upper left direction
		STA		_obj_cur_spr_idx._ship			; select right oriented tile
		BNE		loc_11E230						; test if already deployed
._no_sea_ports:
		LDA		_obj_active_flags				; no sea ports - no floating objects
		AND		#$FB							; remove existing if airport destroyed
		STA		_obj_active_flags
; -
loc_11E230:
		LDA		_city._bank_flags				; test for bank states
		BPL		loc_11E23D
		AND		#$01
		BNE		.bank_break						; if bank built and message displayed, skip
		LDA		#$00
		BEQ		.bank_re_set					; if bank destroyed, clean flags
loc_11E23D:
		AND		#$01
		BEQ		.bank_break						; bank no set, no acctivated, exit
		LDA		#$1F
		STA		_game_msg_idx					; when bank built, enable loans and display msg
		LDA		#$01							; this may take seconds to appear if town is huge
		STA		_game_msg_awaits_flag
		LDA		#$80							; set loan enable flag
.bank_re_set:
		STA		_city._bank_flags
.bank_break:
		MADDW	_tmpE6,_RCI_counts._R,_RCI_counts._C	; calculate overall data
; BUG! copy-paste, second CLC wrong here!
; -
;		LDA		_tmpE6
;		CLC
;		ADC		_RCI_counts._I
;		STA		_total_RCI_counts
;		LDA		_tmpE7
;		CLC
;		ADC		_RCI_counts._I+1
;		STA		_total_RCI_counts+1
; -
		MADDW	_total_RCI_counts,_tmpE6,_RCI_counts._I	; FIX, total sum of areas
; REDUNDANT, will change after end of the fiscal year, but used in some further calcs
; in event manager. not used anywhere near here after all, can be freely removed
; -
;		LDA		_nuke_power_station_count
;		CLC
;		ADC		_coal_power_station_count
;		STA		_tmp6CC
; -
		LSADDW	_tmpE6,_city._level._C,_city._level._I,3	; calculate population value
		MADDW	_tmpE6,_tmpE6,_city._level._R	; CAUTION! optimized code
		PHA										; PHA here pushes INT part of E6
		PUSHB	_tmpE6							; after MADDW instruction.
		PUSHB	#$14
		JSR		_mmc5_mul16to8					; fp8_8 mul to int result fp16_8, but threated as an integer, so
		POPD	_popul_cur						; 16_8 with shifted mantissa popul_cur = ((L_C+L_I)*8 + L_R) * 20 * 256
		MADDW	_stats._total,_stats._developed,_stats._undeveloped
		MADDW	_stats._total,_stats._total,_stats._other
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_tile_stats_collect:
		LDY		_last_map_tile_read
;		CPY		#_MAP_BRIDGE_WATER_LR			; UNFINISHED: here they planned to animate
;		BNE		loc_11E2EA						; drawbridges
;		NOP
;loc_11E2EA:
		CPY		#_MAP_GROUND					; calc electric communications
		BCC		loc_11E2FB						; skip buildings, they are counted separately
		LDA		_map_tiles_flags_tbl,Y			; now all conductor tiles below ground are electric wires
		BPL		loc_11E2FB						; skip the rest
		INCW	_electrics_count				; used in overwiew screen only
loc_11E2FB:
		CPY		#_MAP_GROUND					; no need to do somerhing with
		BNE		loc_11E308
		INCW	_ground_count		; count bulldozered grounds for owerview screen
		RTS
loc_11E308:
;		LDA		_last_map_tile_read				; REDUNDANT, _last_map_tile_read here still in Y
;		CMP		#_MAP_FIRE
		CPY		#_MAP_FIRE
		BNE		loc_11E312
		JMP		_fire_tile_collect				; fire area animate
loc_11E312:
;		CMP		#_MAP_FLOOD						; REDUNDANT
		CPY		#_MAP_FLOOD
		BNE		loc_11E319
		JMP		_flooded_tile_collect			; flooded area animate
loc_11E319:
;		CMP		#_MAP_POLLUTION					; REDUNDANT
		CPY		#_MAP_POLLUTION
		BNE		loc_11E320
		JMP		_polluted_tile_collect			; polluted area animate
loc_11E320:
;		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_ROAD
		BEQ		loc_11E32B
		JMP		_road_tile_collect				; roads collect
loc_11E32B:
		LDA		_map_tiles_flags_tbl,Y			; not a building, skip
		AND		#_FL_BUILDING					; also skip if inside the building
		BEQ		loc_11E33A						; calculate only real building indexes
		LDA		_cur_map_tile_is_inside_a_building_flag
		BNE		loc_11E33A
		JMP		_general_buildings_collect		; here goes buildings stats
loc_11E33A:
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_RAIL
		BEQ		loc_11E344
		JMP		_rails_collect					; rails collect
loc_11E344:
		LDA		_map_tiles_flags_tbl,Y			; all protected but not the buildings
		AND		#_FL_PROTECTED					; are water, sea shores and special tiles
;		BEQ		loc_11E34E						; REDUNDANT
;		JMP		_non_destructable_collect
;loc_11E34E:
		BNE		_non_destructable_collect		; FIX
		CPY		#_MAP_CORRUPT0
		BNE		locret_11E355
		STY		_corruption_anim_flag			; animate corrupted tile request
locret_11E355:
		RTS

; =============== S U B	R O U T	I N E =======================================
_non_destructable_collect:
		LDA		_last_map_tile_read
		CMP		#_MAP_GRASS						; look for park grass/threes
		BCC		locret_11E387
		CMP		#_MAP_CORRUPTED
		BCS		loc_11E36A
		INCW	_parks_count					; this is parks counter
		RTS
loc_11E36A:
		CMP		#_MAP_WATER						; this is for water and seashores
		BCC		locret_11E387
; NEW: new code to handle properly ship deploy, still need better solution
; TODO, look for _MAP_WATER_ROUTE here
; -
		BNE		.no_open_water
		MOVW	_last_water_tile_pos,_tmp_map_pos	; store last water tile position
.no_open_water:
; -
		CMP		#_MAP_WOODS0
		BCS		loc_11E37B
		INCW	_water_count					; collect sea shores
		RTS
loc_11E37B:
		CMP		#_MAP_WATER_ROAD_LR				; between sea shores are woods
		BCS		locret_11E387
		INCW	_woods_count					; calc unbulldozered/woods
locret_11E387:
		RTS

; =============== S U B	R O U T	I N E =======================================
_fire_tile_collect:
		JSR		_rand							; the probability of fire stops
		CMP		#$F0							; 16/256
		BCC		loc_11E394
		LDA		#_MAP_CORRUPTED					; if hit the treshold, cease flame
		JMP		_map_tile_write					; the burned ground left
loc_11E394:
		INCW	_fires_count					; FIX, calculate an actual value.
		LDX		#$03							; now check for 4 surroinding tiles
loc_11E396:
		JSR		_jump_to_near_to_tmp_map_pos_tile
		BCS		loc_11E3B5						; skip if at the map edge
; BUG! this routine isn't X safe! it can reset X to 0
; sometimes, so we will skip a lot of near tiles for fires!
; FIX, workaround for X issue
		TXA
		PHA
; -
		JSR		_get_cur_building_tile			; read near tile index
; FIX, workaround for X issue finish
		PLA
		TAX
; -
		LDY		_last_map_tile_read				; test if flammable
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_DESTRUCTIBLE
		BEQ		loc_11E3B2						; if no, then return back to the
		JSR		_rand							; current tile
		CMP		#$F4							; or with probability 12/256
		BCC		loc_11E3B2						; burn this near tile
		JSR		_fire_cur_map_tile
loc_11E3B2:
		JSR		_back_from_near_tmp_map_pos_tile; return to to the cur tile, repeat
loc_11E3B5:										; for all 4 surrounding tiles
		DEX
		BPL		loc_11E396
		RTS

; =============== S U B	R O U T	I N E =======================================
_flooded_tile_collect:
		LDA		_disaster_tiles_to_flood_count	; this counter is set by disaster
		BEQ		.cease_flood					; start routine. the flood will end
		DEC		_disaster_tiles_to_flood_count	; when counter drops zero
		LDY		#$03
.do_flood:
		TYA										; do test floods for all surrounding tiles
		PHA										; store counter for now
		JSR		_rand							; with probability 1/8 do flood
		AND		#$07
		BNE		.no_flood
		PUSHB	_tmp_map_pos._COL				; NOTE, as seen above, the _get_cur_building_tile
		CLC										; routine not X safe, they overlooked it earlier
		ADC		_near_tile_pos_delta_col_list,Y	; but made some workaround for this problem here
		STA		_tmp_map_pos._COL				; by implementing another inline method of
		PUSHB	_tmp_map_pos._ROW				; retrieving the near tile positions with boundary test
		CLC
		ADC		_near_tile_pos_delta_row_list,Y
		STA		_tmp_map_pos._ROW
		JSR		_tmp_map_pos_test_max			; if not exceeded the map boundaries, read near tile
		BCS		loc_11E3F7
		JSR		_get_cur_building_tile			; read actual nearest tile
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y			; test if we may flood this tile
		AND		#_FL_FLOODABLE
		BEQ		loc_11E3F7
		JSR		_flood_cur_map_tile				; if yes, then do flood
loc_11E3F7:
		POPB	_tmp_map_pos._ROW
		POPB	_tmp_map_pos._COL
.no_flood:
		PLA
		TAY
		DEY
		BPL		.do_flood
		RTS
.cease_flood:
		JSR		_rand							; 1/8 probability
		AND		#$07							; to remove flooded ground
		BNE		locret_11E411
		LDA		#_MAP_GROUND
		JMP		_map_tile_write
locret_11E411:
		RTS

; =============== S U B	R O U T	I N E =======================================
_polluted_tile_collect:
		JSR		_rand							; pollution slowly fade sometimes
		BNE		locret_11E423					; we can't control this process
		JSR		_rand
		CMP		#$10
		BCS		locret_11E423					; (1/256)*(16/256) probability it fade
		LDA		#_MAP_GROUND
		JMP		_map_tile_write
locret_11E423:
		RTS

; =============== S U B	R O U T	I N E =======================================
_road_tile_collect:
		INCW	_roads_count					; statistic counter

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_transport_deterogate_test:
		LDA		_map_tiles_flags_tbl,Y			; NOTE, for some reason we skip roads
		BMI		locret_11E456					; if intersected with electrics
		LDA		_fund_rate_trans				; here we test for roads deterogating
		CMP		#$1E							; funding above 94% is fine
		BCS		locret_11E456					; else 1/256 chance to deterogate
		JSR		_rand
		BNE		locret_11E456
		JSR		_rand							; the lower funding, more frequently
		AND		#$3F							; roads destroyed
		CMP		_fund_rate_trans				; (1/256)*((64-funding)/64) probability
		BCC		locret_11E456					;
		LDX		#_MAP_WATER						; test for water roads again
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_WATER
		BNE		loc_11E452
		LDX		#_MAP_CORRUPTED
loc_11E452:
		TXA
		JMP		_map_tile_write					; destroy road
locret_11E456:
		RTS

; =============== S U B	R O U T	I N E =======================================
_rails_collect:
		INCW	_rails_count					; statistics collect
		LDA		_obj_active_flags				; test if train object is active
		AND		#$08
		BNE		.no_trains_start				; the rest of the code is for deploying
		LDA		_last_map_tile_read				; the train object
		CMP		#_MAP_ROAD_RAIL_LR				; no start at road interceptions
		BEQ		.no_trains_start
		CMP		#_MAP_ROAD_RAIL_UD
		BEQ		.no_trains_start
		CMP		#_MAP_ELECTRO_RAIL_LR
		BEQ		.no_trains_start
		CMP		#_MAP_ELECTRO_RAIL_UD
		BEQ		.no_trains_start
		LDA		_tmp_map_pos._COL				; write start position
		STA		_obj_pos._train._COL			; and dst position to the same point
		STA		_obj_dst_pos._train._COL
		LDA		_tmp_map_pos._ROW
		STA		_obj_pos._train._ROW
		STA		_obj_dst_pos._train._ROW
		LDA		_tmp_map_pos._COL				; one more test for map edges
		CMP		#$4B
		BCS		.no_trains_start
		LDA		_tmp_map_pos._ROW
		CMP		#$4B
		BCS		.no_trains_start				; if we not at the edges, look forward
		LDX		#$02							; for train destination
		INC		_tmp_map_pos._COL				; it moves one tile at once
loc_11E49F:
		JSR		_map_tile_read					; look if forward tile is a rail
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_RAIL
		BEQ		loc_11E4BA						; if not rails, search next
		CPY		#_MAP_ROAD_RAIL_LR				; if special rails, search next
		BEQ		loc_11E4BA						; we not start at interceptions
		CPY		#_MAP_ROAD_RAIL_UD				; for some reason, but we may cross it
		BEQ		loc_11E4BA
		CPY		#_MAP_ELECTRO_RAIL_LR
		BEQ		loc_11E4BA
		CPY		#_MAP_ELECTRO_RAIL_UD
		BNE		.train_deploy
loc_11E4BA:
		CPX		#$04							; try tile to the right
		BEQ		.no_trains_start				; then tile to the bottom
		LDX		#$04							; only two dest directions here
		DEC		_tmp_map_pos._COL
		INC		_tmp_map_pos._ROW
		BPL		loc_11E49F
.train_deploy:
		STX		_obj_cur_spr_idx._train			; so select one of sprite types
		STX		_obj_dst_spr_idx._train			; hor or vert
		LDA		#$00
		STA		_obj_shift._train._COL			; fine tuning position reset
		STA		_obj_shift._train._ROW
		LDA		_obj_active_flags				; enable train object
		ORA		#$08
		STA		_obj_active_flags
.no_trains_start:
		JMP		_transport_deterogate_test		; common transport deterogation test

; =============== S U B	R O U T	I N E =======================================
_general_buildings_collect:
		JSR		_test_power_grid_map_bit		; test if building is powered
		STA		_tmp_last_read_map_tile_power_state	; remember this
		BEQ		loc_11E4F1
		INCW	_powered_areas					; collect powered areas
loc_11E4F1:
		LDA		_last_map_tile_read				; all up to _MAP_POLICE_STATION are
		CMP		#_MAP_POLICE_STATION			; RCI buildings
		BCC		.RCI_buildings_stats
		LDX		#OTHER_WIDX
		JSR		_inc_areas_stats				; increment other buildings counter
		JMP		_collect_other_buildings_stats	; all non living areas goes here
.RCI_buildings_stats:
		CMP		#_MAP_C_AREA_DEF
		BCS		loc_11E51F
		CMP		#_MAP_R_HOSPITAL
		BNE		loc_11E510
		LDX		#OTHER_WIDX						; hospitals are other buildings too
		JSR		_inc_areas_stats				; but resides in R areas
		JMP		.inc_hospital
loc_11E510:
		CMP		#_MAP_R_SCHOOL
		BNE		loc_11E51C
		LDX		#OTHER_WIDX
		JSR		_inc_areas_stats
		JMP		.inc_school
loc_11E51C:
		JMP		_R_areas_develop				; the rest is a normal RCI areas
loc_11E51F:
		CMP		#_MAP_I_AREA_DEF				; let's develop them
		BCS		loc_11E526
		JMP		_C_areas_develop
loc_11E526:
		JMP		_I_areas_develop
; NOTE, even when we destroy school here it stay counted for a while!
;
.inc_hospital:
		INCW	_hospitals_count				; additional test for removing the
		LDA		_hospitals_delta				; hospitals. but they grow as regular
		BPL		locret_11E558					; R areas
		LDA		#$15
		JSR		_rand_clamp_A					; 1/16 probability to remove
;		BNE		locret_11E558
		BEQ		loc_11E553
		RTS										; OPTIMIZED
.inc_school:
		INCW	_schools_count					; the same with schools
		LDA		_schools_delta					; if delta is negative, attempt
		BPL		locret_11E558					; to destroy hospital
		LDA		#$15
		JSR		_rand_clamp_A
		BNE		locret_11E558
loc_11E553:
		LDA		#_MAP_R_AREA_DEF				; reset R area to 0
		JMP		_map_tile_write
locret_11E558:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_areas_develop:
		LDX		#R_AREA_WIDX					; increment counter for R
		JSR		_inc_RCI_counts
		LDY		_last_map_tile_read				; get current dev state
		LDA		_R_area_develop_levels_list,Y
		STA		_cur_area_develop_state
		JSR		_adc_RCI_level					; add it to the overall level
		LDX		#DEVELOPED_WIDX
		LDA		_last_map_tile_read				; collect development state stats
		BNE		loc_11E573
		LDX		#UNDEVELOPED_WIDX
;		INX
;		INX
loc_11E573:
		JSR		_inc_areas_stats
		JSR		_R_twin_areas_cleanup; special test for double areas
		LDA		#$24
		JSR		_rand_clamp_A					; probability dev_state/36
		CMP		_cur_area_develop_state			; higher dev state, higher traffic
		BCS		loc_11E590
		LDA		#R_AREA_BIDX
		JSR		_traffic_calc					; calculate traffic around the building
		BCS		loc_11E590
		JSR		_get_life_ratio
		JMP		_R_develop_down
loc_11E590:
		JSR		_get_cur_building_tile			; while driving we change current tile
		LDA		_last_map_tile_read				; read it again
		BEQ		loc_11E59F						; if R area undeveloped, force it to develop
		JSR		_rand							; or 1/3 of chance to change the state
		AND		#$03
		BNE		locret_11E5FA					; otherwise do nothing
loc_11E59F:
		JSR		_rand							; get random value
		STA		_tmpE9
		LDA		_city._delta._R					; shift delta to 0-255 range
		CLC
		ADC		#$80
		PHA
		PUSHB	#$96							; soften and shift to signed range again
		JSR		_mmc5_mul8to8
		PLA
		PLA
		SEC
		SBC		#$4B
		STA		_tmpE8							; E8 = ((delta+128)*0.6)-0.3
		JSR		_get_growth_ratio				; E* append with new growth ratio
		CLC
		ADC		_tmpE8							; now we have full range -128 to +127
		CLC
		ADC		#$80							; shift it to 0-255 range again
		LDY		_tmp_last_read_map_tile_power_state
		BNE		loc_11E5C8						; if not powered, drop down instantly
		LDA		#$48
loc_11E5C8:
		STA		_tmpE8							; now check what we have now
		CMP		#$7F
		BCC		.drop_R_down					; if we have negative lever now, drop growth down
		SBC		#$67							; or else ratio in range 127-255 we have
		CMP		_tmpE9							; ratio-106)/256 chance to grow
		BCC		.drop_R_down					; higher ration, higher chance, however we
		LDA		_cur_area_develop_state			; will drop here anyway
		BNE		.grow_R_up						; if already developed, force grow
		LDA		_tmpE9							; if not, the 1/3 chance to
		AND		#$03							; attempt to spawn hospital or school
;		BNE		.grow_R_up						; REDUNDANT
;		JMP		.spawn_hopital_or_school
		BEQ		.spawn_hopital_or_school		; OPTIMIZED
.grow_R_up:
		JSR		_get_life_ratio
		JMP		_R_develop_up
.drop_R_down:
		LDA		_tmpE8							; when negative or small positive
		CMP		#$82							; add 106 and test against random again
		BCS		locret_11E5FA					; to decide to develop down or not
		ADC		#$67
		CMP		_tmpE9
		BCS		locret_11E5FA					; chance 20/256 you skip it
		JSR		_get_life_ratio
		JMP		_R_develop_down
locret_11E5FA:
		RTS
.spawn_hopital_or_school:
		JSR		_rand							; 50/50 select either hospital or shcool
		BPL		loc_11E612
		LDA		_hospitals_delta				; if we need them, delta is positive and not zero
		BMI		locret_11E623
		BEQ		locret_11E623
		LDA		#_MAP_R_HOSPITAL				; spawn one, clear request delta
		JSR		_map_tile_write
		LDA		#$00
		STA		_hospitals_delta
		RTS
loc_11E612:
		LDA		_schools_delta					; or else spawn
		BMI		locret_11E623
		BEQ		locret_11E623
		LDA		#_MAP_R_SCHOOL
		JSR		_map_tile_write
		LDA		#$00
		STA		_schools_delta
locret_11E623:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_twin_areas_cleanup:
		LDA		_last_map_tile_read				; test if we currently at
		CMP		#_MAP_R_AREA_TWINA0				; double area
		BCC		locret_11E72B
		LDX		#$01
		JSR		_tmp_map_pos_backup				; backup curpos slot 1
		LDA		_last_map_tile_read
		SEC
		SBC		#_MAP_R_AREA_TWINA0				; calc what twin area exactly
		TAY										; out of four
		LDA		_tmp_map_pos._COL				; calculate where the other side
		CLC										; of the building according
		ADC		_twin_area_col_list,Y			; to possible positions,
		STA		_tmp_map_pos._COL				; move to that area
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		_twin_area_row_list,Y
		STA		_tmp_map_pos._ROW
		TYA										; backup cur idx
		PHA
		JSR		_map_tile_read					; then test if twin area has
		TAX										; correct counterpart (if not damaged)
		PLA
		TAY
		TXA										; restore Y, keep A in X meanwhile, then restore
		CMP		_R_twin_idx_test_list,Y			; here is the counterparts list
		BEQ		loc_11E726						; if all is right, do nothing
		LDX		#$01
		JSR		_tmp_map_pos_restore			; if other part is broken, remove
		LDA		#_MAP_R_AREA14					; current part as well
		JMP		_map_tile_write
loc_11E726:
		LDX		#$01
		JMP		_tmp_map_pos_restore
locret_11E72B:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_develop_down:
		JSR		_map_tile_read					; if twin buildings, destroy them
		CMP		#_MAP_R_AREA_TWINA0				; they destroyed instantly if develop
		BNE		loc_11F4C9						; goes down
		LDA		#_MAP_R_AREA14					; TODO, optimize me
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		STA		_tmp_map_pos._ROW
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		RTS
loc_11F4C9:
		CMP		#_MAP_R_AREA_TWINA1
		BNE		loc_11F4EA
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		STA		_tmp_map_pos._ROW
		RTS
loc_11F4EA:
		CMP		#_MAP_R_AREA_TWINB0
		BNE		loc_11F50B
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		STA		_tmp_map_pos._COL
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		RTS
loc_11F50B:
		CMP		#_MAP_R_AREA_TWINB1
		BNE		loc_11F52C
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		LDA		#_MAP_R_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		STA		_tmp_map_pos._COL
		RTS
loc_11F52C:
		LDX		_life_ratio_idx					; if life ratio is low
		BEQ		locret_11F550					; or low development
		LDA		_cur_area_develop_state			; nothing to develop down
		BEQ		locret_11F550
		CMP		#$18
		BCC		loc_11F548						; low developed degrades fast
		SEC										; high developer decrades slow
		SBC		#$18							; with growth rate down
		LSR
		LSR
		LSR
		JSR		_R_area_new_state_set			; new_level = (level-24)/8
		LDA		#$FF
		JMP		_pop_growth_map_update			; decrease population growth
loc_11F548:
		LSR
		LSR										; new_level = (level/4)-1
		SEC
		SBC		#$01
		JMP		_map_tile_write					; may degrade to 0 here
locret_11F550:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_develop_up:
		JSR		_read_pollution_map_nibble
		CMP		#$08
		BCS		locret_11F3BE
		JSR		_map_tile_read
		CMP		#$05
		BCS		loc_11F393
		LDA		_cur_area_develop_state
		CMP		#$09
		BCS		loc_11F382
		LDY		_last_map_tile_read
		INY
		TYA
		JMP		_map_tile_write
loc_11F382:
		JSR		_read_pop_dencity_map_nibble
		CMP		#$04
		BCC		locret_11F3BE
		LDA		#$00
		JSR		_R_area_new_state_set
		LDA		#$01
		JMP		_pop_growth_map_update
loc_11F393:
		CMP		#$15
		BCS		locret_11F40D
		CMP		#$14
		BNE		loc_11F3F5
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		STA		_tmp_map_pos._ROW
		JSR		_map_tile_read
		CMP		#$14
		BNE		loc_11F3BF
		LDA		#$16
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		LDA		#$15
		JMP		_map_tile_write
locret_11F3BE:
		RTS
loc_11F3BF:
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		STA		_tmp_map_pos._COL
		JSR		_map_tile_read
		CMP		#$14
		BNE		loc_11F3EB
		LDA		#$18
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		LDA		#$17
		JMP		_map_tile_write
loc_11F3EB:
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		RTS
loc_11F3F5:
		LDA		_cur_area_develop_state
		CMP		#$28
		BCS		locret_11F40D
		LDA		_cur_area_develop_state
		LSR
		LSR
		LSR
		SEC
		SBC		#$01
		JSR		_R_area_new_state_set
		LDA		#$01
		JMP		_pop_growth_map_update
locret_11F40D:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_area_new_state_set:
		STA		_tmpE6							; state = _MAP_R_AREA05 + life_idx*4 + new_state
		LDA		_life_ratio_idx
		ASL
		ASL
		ADC		_tmpE6
		ADC		#_MAP_R_AREA05
		JMP		_map_tile_write

; =============== S U B	R O U T	I N E =======================================
_C_areas_develop:
		LDX		#C_AREA_WIDX
		JSR		_inc_RCI_counts					; count areas number
		LDA		_last_map_tile_read				; get current the development state
		SEC
		SBC		#_MAP_C_AREA_DEF
		TAY
		LDA		_C_area_develop_levels_list,Y	; increment overall dev sum for C
		STA		_cur_area_develop_state
		JSR		_adc_RCI_level					; beginning is the same as for R
		LDX		#DEVELOPED_WIDX
		LDA		_last_map_tile_read
		CMP		#_MAP_C_AREA_DEF
		BNE		loc_11E644
		LDX		#UNDEVELOPED_WIDX
;		INX
;		INX
loc_11E644:
		JSR		_inc_areas_stats				; so we collected statistics, cleanup twins...
		JSR		_C_twin_areas_cleanup			; time to developement
		LDA		#$06							; develop range here is smaller, so less
		JSR		_rand_clamp_A					; random test, 6/dev_state
		CMP		_cur_area_develop_state			; skip traffic test for low developed areas
		BCS		loc_11E661
		LDA		#C_AREA_BIDX					; drive from C to R or I areas now
		JSR		_traffic_calc
		BCS		loc_11E661						; if drive not success, drop down developement here
		JSR		_get_life_ratio
		JMP		_C_develop_down
loc_11E661:
		JSR		_rand							; 1/3 chance to develop up
		AND		#$03
		BNE		locret_11E6A1
		JSR		_rand
		STA		_tmpE9
;		LDA		#$00							; REDUNDANT, does nothing, maybe were non-zero
;		CLC										; earlier to make constant growth
;		ADC		_city._delta._C
		LDA		_city._delta._C					; FIX
		CLC
		ADC		#$80							; shift to 0-255 range
		LDY		_tmp_last_read_map_tile_power_state
		BNE		loc_11E67D						; if not powered, set negative value
		LDA		#$48
loc_11E67D:
		STA		_tmpE8
		CMP		#$7F
		BCC		loc_11E68F						; if negative, then do develop down
		SBC		#$67							; chance to develop up for positive
		CMP		_tmpE9							; delta-103/256 {25-153/256 range}
		BCC		loc_11E68F						; more delta, more percent
		JSR		_get_life_ratio
		JMP		_C_develop_up
loc_11E68F:
		LDA		_tmpE8							; also for negative or small positive
		CMP		#$82							; values add 106 and drop the dice again
		BCS		locret_11E6A1					; there is a small chance to skip it
		ADC		#$67							; also as for R when you miss the chance
		CMP		_tmpE9							; to gro up with large delta, you won't
		BCS		locret_11E6A1					; develop at all.
		JSR		_get_life_ratio
		JMP		_C_develop_down
locret_11E6A1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_C_twin_areas_cleanup:
		LDA		_last_map_tile_read				; the same as for R areas
		CMP		#_MAP_C_AREA_TWINA0				; Commerce can be twin as well
		BCC		locret_11E6E6
		LDX		#$01
		JSR		_tmp_map_pos_backup
		LDA		_last_map_tile_read
		SEC
		SBC		#_MAP_C_AREA_TWINA0
		TAY
		LDA		_tmp_map_pos._COL
		CLC
		ADC		_twin_area_col_list,Y
		STA		_tmp_map_pos._COL
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		_twin_area_row_list,Y
		STA		_tmp_map_pos._ROW
		TYA
		PHA
		JSR		_map_tile_read
		TAX
		PLA
		TAY
		TXA
		CMP		_C_twin_idx_test_list,Y			; keep it or remove completely
		BEQ		loc_11E6E1
		LDX		#$01
		JSR		_tmp_map_pos_restore
		LDA		#_MAP_C_AREA14
		JMP		_map_tile_write
loc_11E6E1:
		LDX		#$01
		JMP		_tmp_map_pos_restore
locret_11E6E6:
		RTS

; =============== S U B	R O U T	I N E =======================================
_C_develop_down:
		JSR		_map_tile_read					; remove twins
		CMP		#_MAP_C_AREA_TWINA0
		BNE		loc_11F575
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		STA		_tmp_map_pos._ROW
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		RTS
loc_11F575:
		CMP		#_MAP_C_AREA_TWINA1
		BNE		loc_11F596
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		STA		_tmp_map_pos._ROW
		RTS
loc_11F596:
		CMP		#_MAP_C_AREA_TWINB0
		BNE		loc_11F5B7
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		STA		_tmp_map_pos._COL
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		RTS
loc_11F5B7:
		CMP		#_MAP_C_AREA_TWINB1
		BNE		loc_11F5D8
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		LDA		#_MAP_C_AREA14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		STA		_tmp_map_pos._COL
		RTS
loc_11F5D8:
		LDA		_cur_area_develop_state			; for low C developement
		SEC										; not decrease growth
		SBC		#$02
		BCC		loc_11F5E8
		JSR		_C_area_new_state_set
		LDA		#$FF
		JMP		_pop_growth_map_update
loc_11F5E8:
		LDA		_cur_area_develop_state			; less than 1 only 0
		CMP		#$01
		BNE		locret_11F5F4
		LDA		#_MAP_C_AREA_DEF
		JMP		_map_tile_write
locret_11F5F4:
		RTS

; =============== S U B	R O U T	I N E =======================================
_C_develop_up:
		JSR		_map_tile_read
		CMP		#$35
		BCS		locret_11F48C
		CMP		#$34
		BNE		loc_11F474
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		STA		_tmp_map_pos._ROW
		JSR		_map_tile_read
		CMP		#$34
		BNE		loc_11F43D
		LDA		#$36
		JSR		_map_tile_write
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		LDA		#$35
		JMP		_map_tile_write
loc_11F43D:
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$02
		STA		_tmp_map_pos._ROW
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		STA		_tmp_map_pos._COL
		JSR		_map_tile_read
		CMP		#$34
		BNE		loc_11F46A
		LDA		#$38
		JSR		_map_tile_write
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		LDA		#$37
		JMP		_map_tile_write

loc_11F46A:
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$02
		STA		_tmp_map_pos._COL
		RTS
loc_11F474:
		JSR		_read_land_val_map_nibble
		LSR
		CMP		_cur_area_develop_state
		BCC		locret_11F48C
		LDA		_cur_area_develop_state
		CMP		#$05
		BCS		locret_11F48C
		JSR		_C_area_new_state_set
		LDA		#$01
		JMP		_pop_growth_map_update
locret_11F48C:
		RTS

; =============== S U B	R O U T	I N E =======================================
_C_area_new_state_set:
		STA		_tmpE6
		LDA		_life_ratio_idx					; state = _MAP_C_AREA01 +life_idx*5 + new_state
		ASL
		ASL
		ADC		_life_ratio_idx
		ADC		_tmpE6
		ADC		#_MAP_C_AREA01
		JMP		_map_tile_write

_twin_area_col_list:
		.BYTE	$00,$00,$02,$FE
_twin_area_row_list:
		.BYTE	$02,$FE,$00,$00
_C_twin_idx_test_list:
		.BYTE	_MAP_C_AREA_TWINA1,_MAP_C_AREA_TWINA0,_MAP_C_AREA_TWINB1,_MAP_C_AREA_TWINB0
_R_twin_idx_test_list:
		.BYTE	_MAP_R_AREA_TWINA1,_MAP_R_AREA_TWINA0,_MAP_R_AREA_TWINB1,_MAP_R_AREA_TWINB0

; =============== S U B	R O U T	I N E =======================================
_I_areas_develop:
		LDX		#I_AREA_WIDX
		JSR		_inc_RCI_counts
		LDA		_last_map_tile_read
		SEC
		SBC		#$40
		TAY
		LDA		_I_area_develop_levels_list,Y
		STA		_cur_area_develop_state
		JSR		_adc_RCI_level
		LDX		#DEVELOPED_WIDX
		LDA		_last_map_tile_read
		CMP		#$40
		BNE		loc_11E75C
		LDX		#UNDEVELOPED_WIDX
;		INX
;		INX
loc_11E75C:
		JSR		_inc_areas_stats
		LDA		#$06
		JSR		_rand_clamp_A
		CMP		_cur_area_develop_state
		BCS		loc_11E776
		LDA		#I_AREA_BIDX
		JSR		_traffic_calc
		BCS		loc_11E776
		JSR		_get_life_ratio
		JMP		_I_develop_down
loc_11E776:
		JSR		_rand
		AND		#$03
		BNE		locret_11E7B7
		JSR		_rand
		STA		_tmpE9
		JSR		sub_11F362
		CLC
		ADC		_city._delta._I
		CLC
		ADC		#$80
		LDY		_tmp_last_read_map_tile_power_state
		BNE		loc_11E793
		LDA		#$48
loc_11E793:
		STA		_tmpE8
		CMP		#$7F
		BCC		loc_11E7A5
		SBC		#$67
		CMP		_tmpE9
		BCC		loc_11E7A5
		JSR		_get_life_ratio
		JMP		_I_develop_up
loc_11E7A5:
		LDA		_tmpE8
		CMP		#$81
		BCS		locret_11E7B7
		ADC		#$67
		CMP		_tmpE9
		BCS		locret_11E7B7
		JSR		_get_life_ratio
		JMP		_I_develop_down
locret_11E7B7:
		RTS

; =============== S U B	R O U T	I N E =======================================
_I_develop_down:
		LDA		#$02							; the only area with no twins
		JSR		_rand_clamp_A					; the only area with pure random life ratio
		STA		_life_ratio_idx
		LDA		_cur_area_develop_state			; so it will grow or raize randomly
		SEC
		SBC		#$02
		BCC		loc_11F60D						; the rest is the same as for C
		JSR		_I_area_new_state_set
		LDA		#$FF
		JMP		_pop_growth_map_update
loc_11F60D:
		LDA		_cur_area_develop_state
		CMP		#$01
		BNE		locret_11F619
		LDA		#_MAP_I_AREA_DEF
		JMP		_map_tile_write
locret_11F619:
		RTS

; =============== S U B	R O U T	I N E =======================================
_I_develop_up:
		LDA		#$02
		JSR		_rand_clamp_A
		STA		_life_ratio_idx
		LDA		_cur_area_develop_state
		CMP		#$04
		BCS		locret_11F4A4
		JSR		_I_area_new_state_set
		LDA		#$01
		JMP		_pop_growth_map_update
locret_11F4A4:
		RTS

; =============== S U B	R O U T	I N E =======================================
_I_area_new_state_set:
		STA		_tmpE6
		LDA		_life_ratio_idx					; state = _MAP_I_AREA01 + life_idx*4 + new_state
		ASL
		ASL
		ADC		_tmpE6
		ADC		#_MAP_I_AREA01
		JMP		_map_tile_write

; =============== S U B	R O U T	I N E =======================================
_collect_other_buildings_stats:
		CMP		#_MAP_COAL_POWER
		BNE		loc_11E7CC
		LDA		_coal_power_station_count		; coal power station building
		BMI		loc_11E7C4						; no greater than 128
		INC		_coal_power_station_count
loc_11E7C4:
		FJSR	_power_node_insert,SRAM,PRGA	; insert power station as a power node
		RTS
loc_11E7CC:
		CMP		#_MAP_NUCLEAR_POWER
		BNE		loc_11E7E0
		LDA		_nuke_power_station_count		; nuclear power stations
		BMI		loc_11E7D8						; no greater than 128
		INC		_nuke_power_station_count
loc_11E7D8:
		FJSR	_power_node_insert,SRAM,PRGA	; the same for nuclear station
		RTS
loc_11E7E0:
		CMP		#_MAP_POLICE_STATION
		BNE		loc_11E803
		LDA		_police_stations_count			; police stations
		CMP		#$63							; no greater than 100
		BCS		loc_11E7EE
		INC		_police_stations_count
loc_11E7EE:
		LDA		_fund_rate_police				; if police have no roads nearby
		STA		_tmpE7							; its radius less by half
		JSR		_get_near_road_tile				; C=1 no roads nearby
		BCS		loc_11E7FA
		LSR		_tmpE7
loc_11E7FA:
		JSR		_read_police_radius_map_nibble	; also radius depends on funding
		CLC										; so we have such small values here
		ADC		_tmpE7							; put police center on map here will be
		JMP		_write_police_radius_map_nibble	; max value, will interpolate later
loc_11E803:
		CMP		#_MAP_NEW_POLICE				; the present police station
		BNE		loc_11E80E						; has maximum range no matter why
		LDA		#$0F							; new police station
		STA		_tmpE7
		JMP		loc_11E7FA
loc_11E80E:
		CMP		#_MAP_FIRE_STATION				; same for fire stations
		BNE		loc_11E831
		LDA		_fire_stations_count
		CMP		#$63
		BCS		loc_11E81C
		INC		_fire_stations_count
loc_11E81C:
		LDA		_fund_rate_fire
		STA		_tmpE7
		JSR		_get_near_road_tile
		BCS		loc_11E828
		LSR		_tmpE7
loc_11E828:
		JSR		_read_fire_radius_map_nibble
		CLC
		ADC		_tmpE7
		JMP		_write_fire_radius_map_nibble
loc_11E831:
		CMP		#_MAP_NEW_FIRE
		BNE		loc_11E83C
		LDA		#$0F							; new fire stations
		STA		_tmpE7
		JMP		loc_11E828
loc_11E83C:
		CMP		#_MAP_STADIUM					; also stadiums
		BNE		loc_11E854
		LDA		_stadiums_count
		BMI		loc_11E848
		INC		_stadiums_count
loc_11E848:
		LDY		#$00
		LDA		_city._delta._R					; if R delta positive, set
		BMI		loc_11E850						; additional bonus flag
		INY
loc_11E850:
		STY		_stadium_R_delta_positive_bonus_flag
		RTS
loc_11E854:
		CMP		#_MAP_AIR_PORT
		BNE		loc_11E86D
		LDA		_airports_count					; airports count
		BMI		loc_11E860
		INC		_airports_count
loc_11E860:
		LDA		_tmp_map_pos._COL				; here we remember las airport position
		STA		_last_airport_pos._COL			; for plane and heli object deploy routines
		LDA		_tmp_map_pos._ROW
		STA		_last_airport_pos._ROW
		RTS
loc_11E86D:
		CMP		#_MAP_SEA_PORT
		BNE		loc_11E8AD
		LDA		_sea_ports_count				; sea ports count
		BMI		loc_11E879
		INC		_sea_ports_count
loc_11E879:
		LDA		_obj_active_flags				; also test for ship deploy should be here
		AND		#$04
		BNE		locret_11E8AC
; UNFINISHED
; -
;		LDY		#>[_city_map]					; UNFINISHED very briefly written code
;		STY		_tmpE7							; to search a special symbol in the sea
;		LDY		#<[_city_map]					; which should be inserted on map to mark
;		STY		_tmpE6							; a special ship routes, ship not intended
;loc_11E888:									; to sail on free water here
;		LDA		(_tmpE6),Y
;		CMP		#_MAP_WATER_ROUTE				; special sea tile
;		BEQ		loc_11E8A0
;		INC		_tmpE6
;		BNE		loc_11E894
;		INC		_tmpE7
loc_11E894:
;		LDA		_tmpE6
;		CMP		#<[_city_map_size]				; BUG: here test value not the end of the buffer ptr
;		CMP		#<[_city_map_end]				; FIX: correct
;		LDA		_tmpE7							; BUG: but buffer size
;		SBC		#>[_city_map_size]				; FIX: correct
;		CMP		#>[_city_map_end]
;		BNE		loc_11E888
;		BEQ		locret_11E8AC
;loc_11E8A0:
;		LDA		_tmp_map_pos._COL				; store the seaport location as a astarting position
;		STA		_obj_pos._ship._COL				; for the ship which isn't quite correct, because
;		LDA		_tmp_map_pos._ROW				; ship goes only on special tiles and without a special
;		STA		_obj_pos._ship._ROW				; test, it can just stuck on the ground
;-
; NEW
; there should be a code to store the starting ship position on any FE tile instead
; then whip will move on the fixed route. the only route exists on the DEtroit scenario map!
; never used anything else.
		MOVW	_obj_pos._ship,_last_water_tile_pos
; -
locret_11E8AC:
		RTS
loc_11E8AD:
		CMP		#_MAP_BANK						; detect bank building here for some reason.
		BNE		locret_11E8B9
		LDA		_city._bank_flags
		ORA		#$01
		STA		_city._bank_flags
locret_11E8B9:
		RTS

; =============== S U B	R O U T	I N E =======================================
_inc_RCI_counts:
		INC		_RCI_counts,X
		BNE		locret_11E8C2
		INC		_RCI_counts+1,X
locret_11E8C2:
		RTS

; =============== S U B	R O U T	I N E =======================================
_inc_areas_stats:
		INC		_stats,X
		BNE		locret_11E8CB
		INC		_stats+1,X
locret_11E8CB:
		RTS

; =============== S U B	R O U T	I N E =======================================
; add fixed point value 0.XXfp to livel meters
;
_adc_RCI_level:
		CLC
		ADC		_city._level.FRAC,X
		STA		_city._level.FRAC,X
		BCC		locret_11E8D8
		INC		_city._level.INT,X
locret_11E8D8:
		RTS

; =============== S U B	R O U T	I N E =======================================
; FIXME: when space is enough, merge me with my copy
; copy of _power_grid_read in PRGA
;
_test_power_grid_map_bit:
; REDUNDANT
;		PUSHB	_tmp_map_pos._ROW
;		PUSHB	#$4C
;		JSR		_mmc5_mul8to8
;		PLA
;		CLC
;		ADC		_tmp_map_pos._COL
;		STA		_tmpE6
;		TAY
;		PLA
;		ADC		#$00
; -
; OPTIMIZED
		LDA		_tmp_map_pos._ROW				; calculate map buf offset
		MULAI	#$4C
		LDA		_MMC5_MUL0
		CLC
		ADC		_tmp_map_pos._COL
		STA		_tmpE6							; store low nibble
		TAY										; backup it for now
		LDA		_MMC5_MUL1						; adjust high nibble in A
		ADC		#$00
; -
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		LSR
		ROR		_tmpE6
		STA		_tmpE7
		LDA		_tmpE6
		CLC
		ADC		#<[_power_map]
		STA		_tmpE6
		LDA		_tmpE7
		ADC		#>[_power_map]
		STA		_tmpE7
		TYA
		AND		#$07
		TAX
		LDY		#$00
		LDA		(_tmpE6),Y
; BUG: here were used $0A3B6, an inv_bitmasks array in BANKA
; FIX: all bitmask arrays are in system bank now
		AND		_inv_bitmasks,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_traffic_calc:
		STA		_tmp6CC							; store selected RCI index
		LDX		#$01							; backup cur pos slot 1
		JSR		_tmp_map_pos_backup
		LDA		#$00
		STA		_tmp6D2							; reset buffer pos of driven roads
		JSR		_get_near_road_tile
		BCC		.return_false					; if C=0 then no roads/rails here
		JSR		_drive_test						; attempt to drive somewhere
		BCS		.return_false					; at the position found previously
		JSR		_traffic_map_update				; if drive is success update traffic map
		LDX		#$01
		JSR		_tmp_map_pos_restore			; restore and exit with TRUE
		SEC
		RTS
.return_false:
		LDX		#$01							; restore and exit
		JSR		_tmp_map_pos_restore			; result of function FALSE
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
; this routine look for any road near the current building, get the first
; available and rties to make random path to any location. It allowed
; to drive forward or turn left or right if drive forward is impossible
; it is forbidden to drive back unless you have no more options
; drive one tile at once, keep track of 1 of two tiles driven, test the
; destination for any new tile driven. if destination found, exith with
; current path stored in stack. if destination isn't found, tries to drive
; further. if there is no path forward, remove last dst pos and try to
; find another in any other direction including the opposite one.
;
_drive_test:
		LDA		#$05
		STA		_tmp6CD							; tmp value for opposite direction,
		LDY		#$00							; impossible value by start, no drive yet
loc_11E941:
		STY		_tmp6D1
		JSR		_drive_somewhere				; drive one tile, store it sometimes
		BCS		loc_11E94F						; look for destination
		JSR		_destination_test				; if we reach any useful destination
		BCS		loc_11E965						; return C=0
		RTS
loc_11E94F:
		LDA		_tmp6D2							; if buffers empty, then exit
		BEQ		loc_11E96D
		DEC		_tmp6D2							; retry one tile back
		LDA		_tmp6D1							; and try another turn
		CLC										; (or maybe the same one randomly)
		ADC		#$03
		STA		_tmp6D1							; retarget
		LDA		#$05
		STA		_tmp6CD
loc_11E965:
		LDY		_tmp6D1
		INY
		CPY		#$1C
		BCC		loc_11E941
loc_11E96D:
		SEC										; up to 27 tiles forward
		RTS										; with no success C=1

; =============== S U B	R O U T	I N E =======================================
; from the first nearest road tile select randomly direction
; and tries to drive there, if succesfull remember the opposite
; direction. if there is nowhere to drive, return C=1
; every odd iteration stores the driven position
;
_drive_somewhere:
		LDA		#$04
		JSR		_rand_clamp_A					; select one of the near tiles randomly
		STA		_tmp6CF							; remember direction
;		CMP		#$04							; REDUNDANT, debug assert for rand out of range
;		BCC		loc_11E97E
;loc_11E97B:
;		JMP		loc_11E97B
loc_11E97E:
		STA		_tmp6CE							; direction chosen
		CMP		_tmp6CD							; compare with opposite direction
		BEQ		loc_11E9B4						; so we make sure we wont drive back and forth
		LDA		_tmp6CE
		JSR		_get_near_building_tile			; read next tile in chosen direction
		BCS		loc_11E9B4
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_TRANSPORT
		BEQ		loc_11E9B4						; if transport system also
		LDX		_tmp6CE
		JSR		_jump_to_near_to_tmp_map_pos_tile	; jump it
		LDA		_tmp6CE							; calculate and store new opposite direction
		CLC										; we must drive only forward or left or right
		ADC		#$02							; from current direction
		AND		#$03
		STA		_tmp6CD
		LDA		_tmp6D1
		LSR
		BCC		loc_11E9B2						; remember path every two tiles
		JSR		_drive_stack_push
loc_11E9B2:
		CLC
		RTS
loc_11E9B4:
;		LDA		_tmp6CF							; REDUNDANT, one more debug assert
;		CMP		#$04
;		BCC		loc_11E9BE
;loc_11E9BB:
;		JMP		loc_11E9BB
;loc_11E9BE:
		INC		_tmp6CE							; look for all 4 directions except
		LDA		_tmp6CE							; opposite one if selected
		AND		#$03							; exit if more roads found
		CMP		_tmp6CF
		BNE		loc_11E97E
		SEC										; nothing to drive
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_near_building_tile:
		PHA
		LDX		#$02
		JSR		_tmp_map_pos_backup
		PLA
		TAX
		CPX		#$04							; assert check
		BCS		loc_11E9E8						; only 4 near tiles allowed
		JSR		_jump_to_near_to_tmp_map_pos_tile	; jump to near tile, read
		BCS		loc_11E9E8						; near tile index, return
		JSR		_get_cur_building_tile			; store near tile
		LDX		#$02
		JSR		_tmp_map_pos_restore
		CLC
		RTS
loc_11E9E8:
		LDX		#$02
		JSR		_tmp_map_pos_restore
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_drive_stack_push:
		JSR		_sram_write_enable				; push route to the stack
		LDY		_tmp6D2
		INY
		LDA		_tmp_map_pos._COL
		STA		_tmp_map-1,Y
		LDA		_tmp_map_pos._ROW
		STA		_tmp_map+$1B,Y
		STY		_tmp6D2
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_drive_stack_pop:
		LDY		_tmp6D2							; retrieve stack of routes
		LDA		_tmp_map-1,Y
		STA		_tmp_map_pos._COL
		LDA		_tmp_map+$1B,Y
		STA		_tmp_map_pos._ROW
		DEY
		STY		_tmp6D2
		RTS

; =============== S U B	R O U T	I N E =======================================
; NOTE, here is the copy of the original Micropolis code which tries to drive
; from R to all other destinations including stadiums, etc. except R itself.
; C however drives to any area including C itself but not to ports/police
; stations or stadiums, which odd because C depends on Airports and also odd
; since drive woll stop immediately at the start because it's already at C.
; I drives to R only.
;
; ok, as we could see here, the C drive tests will be succesfull any time
; even if no other areas near it. it will be satisfied by itself, return
; back to the starting point. This prevents the proper traffic being generated
; by C areas most of the time.
;
; FIX, let's fix it and see what will happen
;
_destination_test:
		LDX		#$00							; here we will test if our current road
loc_11EA1E:										; touched any useful destination point
		STX		_tmp6D0
		TXA
		JSR		_get_near_building_tile			; get nearest tile
		LDA		_last_map_tile_read
		LDX		_tmp6CC							; here is idx for currently selected
		BNE		loc_11EA37						; area
		CMP		#_MAP_C_AREA_DEF				; for area R test if we reached C or I
		BCC		.look_for_next_area				; areas or police station or
;		CMP		#_MAP_STADIUM					; sea port or any power station
; FIX, lets expand the destination points for R to all
; possible buildings
		CMP		#_MAP_INTERNAL_BUILD0
; -
		BCC		.reached_destination
		BCS		.look_for_next_area
loc_11EA37:
		DEX
		BNE		loc_11EA40
; FIX, let's prevent C to C destinations also
		CMP		#_MAP_C_AREA_DEF				; drive to R
		BCC		.reached_destination
		CMP		#_MAP_I_AREA_DEF				; skip C areas
		BCC		.look_for_next_area
; -
		CMP		#_MAP_POLICE_STATION			; the rest is I only
		BCS		.look_for_next_area
		BCC		.reached_destination
loc_11EA40:
		CMP		#_MAP_C_AREA_DEF				; for I areas look if we reached
		BCS		.look_for_next_area				; R areas only
.reached_destination:
		CLC
		RTS
.look_for_next_area:
		LDX		_tmp6D0
		INX
		CPX		#$04
		BNE		loc_11EA1E						; for all near tiles around
		SEC										; no destinations arrived
		RTS

; =============== S U B	R O U T	I N E =======================================
_traffic_map_update:
		LDX		_tmp6D2							; test if stack isn't empty
		BEQ		locret_11EA7C
loc_11EA55:
		STX		_tmp6D0							; remember total number of items
		JSR		_drive_stack_pop				; retrive drive position
		JSR		_tmp_map_pos_test_max			; for some reason test the edges
		BCS		locret_11EA7C
		JSR		_get_cur_building_tile			; check what kind of transport
		LDY		_last_map_tile_read				; we take
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_RAIL
		BNE		loc_11EA76						; if rails, no traffic applied
		JSR		_read_traffic_map_nibble		; if roads, update traffic map
		CLC
		ADC		#$02
		JSR		_write_traffic_map_nibble
loc_11EA76:
		LDX		_tmp6D0							; loop untill all items over
		DEX
		BNE		loc_11EA55
locret_11EA7C:
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_pos_test_max:
		LDA		_tmp_map_pos._COL
		CMP		#$4C
		BCS		locret_11EA89
		LDA		_tmp_map_pos._ROW
		CMP		#$4C
locret_11EA89:
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_near_road_tile:
		LDX		#$02							; backup cur pos to slot 2
		JSR		_tmp_map_pos_backup
		LDY		#$07
loc_11EA91:
		STY		_tmpE6							; for all surroinding tiles
		LDA		_tmp_map_pos._COL				; around 2x2 building
		CLC
		ADC		_small_building_surrond_tiles_col_list,Y
		STA		_tmp_map_pos._COL				;  24
		CMP		#$4C							; 0**6
		BCS		loc_11EABA						; 1**7
		LDA		_tmp_map_pos._ROW				;  35
		CLC										;
		ADC		_small_building_surrond_tiles_row_list,Y
		STA		_tmp_map_pos._ROW
		CMP		#$4C
		BCS		loc_11EABA
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y			; stop if we found at least one
		AND		#_FL_TRANSPORT					; rail or road
		BNE		loc_11EAC6						; and return position of new tile
loc_11EABA:
		LDX		#$02
		JSR		_tmp_map_pos_restore			; or restore backup and tray again
		LDY		_tmpE6
		DEY
		BPL		loc_11EA91
		CLC
		RTS
loc_11EAC6:
		SEC
		RTS

_small_building_surrond_tiles_col_list:
		.BYTE	$FF,$FF,$00,$00,$01,$01,$02,$02
_small_building_surrond_tiles_row_list:
		.BYTE	$00,$01,$FF,$02,$FF,$02,$00,$01

; =============== S U B	R O U T	I N E =======================================
_land_value_recalc:
		JSR		_clear_tmp_map_B5
		LDA		#$00
		STA		_tmp6CC
		STA		_tmp6CC+1
		STA		_tmp6CE
		STA		_tmp6CE+1
		STA		_tmp_area_pos._ROW
loc_11EAEC:
		LDA		#$00
		STA		_tmp_area_pos._COL
loc_11EAF1:
		JSR		_pollution_recalc
		LDA		_tmpE6
		BEQ		loc_11EB70
		JSR		_tmp_area_pos_to_tmp_map_pos_convert
		LDA		_tmpE7
		JSR		_write_tmp_map_div4_mul13_nibble
		JSR		sub_11F10B
		STA		_tmpE6
		LDA		#$0F
		SEC
		SBC		_tmpE6
		ASL
		ASL
		ASL
		ASL
		STA		_tmpE8
		JSR		_surronding_map_are_non_destructable_tile_sum
		JSR		_tmp_area_pos_to_tmp_map_pos_convert
		JSR		_read_pollution_map_nibble
		ASL
		ASL
		ASL
		ASL
		STA		_tmpE6
		LDA		_tmpE8
		SEC
		SBC		_tmpE6
		BCS		loc_11EB28
		LDA		#$00
loc_11EB28:
		STA		_tmpE8
		JSR		_tmp_area_pos_to_tmp_map_pos_convert
		JSR		_read_crime_map_nibble
		CMP		#$0B
		BCC		loc_11EB3E
		LDA		_tmpE8
		SBC		#$14
		BCS		loc_11EB3C
		LDA		#$00
loc_11EB3C:
		STA		_tmpE8
loc_11EB3E:
		LDA		_tmpE8
		CLC
		ADC		_tmp6D3
		BCC		loc_11EB48
		LDA		#$FF
loc_11EB48:
		LSR
		LSR
		LSR
		LSR
		LSR
		STA		_tmpE8
		LSR
		CLC
		ADC		_tmpE8
		BNE		loc_11EB57
		LDA		#$01
loc_11EB57:
		PHA
		CLC
		ADC		_tmp6CC
		STA		_tmp6CC
		BCC		loc_11EB64
		INC		_tmp6CC+1
loc_11EB64:
		INCW	_tmp6CE
		PLA
		JSR		_write_land_val_map_nibble
loc_11EB70:
		INC		_tmp_area_pos._COL
		LDA		_tmp_area_pos._COL
		CMP		#$13
		BEQ		loc_11EB7D
		JMP		loc_11EAF1
loc_11EB7D:
		INC		_tmp_area_pos._ROW
		LDA		_tmp_area_pos._ROW
		CMP		#$13
		BEQ		loc_11EB8A
		JMP		loc_11EAEC
loc_11EB8A:
		LDA		_tmp6CE
		ORA		_tmp6CE+1
		BNE		loc_11EB9A
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE7
		BEQ		loc_11EBB7
loc_11EB9A:
; BUG, WRONG AVERAGE CALC!
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		MOVW	_tmpE7,_tmp6CC
; -
; FIX, rescale average
		LDA		_tmp6CE							; fast divide to 8 in FIXED POINT!
		ASL										; you may think i multiply, NO!
		ROL		_tmp6CE+1						; result is fp8_8
		ASL										; so we cam have results in more wider range
		ROL		_tmp6CE+1
		ASL
		ROL		_tmp6CE+1
		STA		_tmp6CE
; -
		MOVW	_tmpEA,_tmp6CE
		JSR		_div_32to16
loc_11EBB7:
		MOVW	_city._land_val_avr,_tmpE6
		LDA		#$00
		STA		_tmp6C4
		STA		_tmp6D0
		STA		_tmp6D1
		STA		_tmp6D2
		STA		_tmp6D3
		JSR		_interpolate_tmp_to_select_poll_popg_popd_crime_land_maps
		JSR		_interpolate_tmp_with_select_poll_popg_popd_crime_land_to_tmp_map
		LDA		#$00
loc_11EBDA:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11EBDF:
		STA		_tmp_map_pos._COL
		JSR		_read_tmp_map_div4_mul13_nibble
		PHA
		CMP		#$00
		BEQ		loc_11EBFE
		INCW	_tmp6D0
		CLC
		ADC		_tmp6D2
		STA		_tmp6D2
		BCC		loc_11EBFE
		INC		_tmp6D3
loc_11EBFE:
		PLA
		JSR		_write_pollution_map_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EBDF
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EBDA
; BUG, another wrong average calc
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		MOVW	_tmpE7,_tmp6D2
; -
; FIX, rescale average
		LDA		_tmp6D0							; fast divide to 8 in FIXED POINT!
		ASL										; you may think i multiply, NO!
		ROL		_tmp6D0+1						; result is fp8_8
		ASL										; so we cam have results in more wider range
		ROL		_tmp6D0+1
		ASL
		ROL		_tmp6D0+1
		STA		_tmp6D0
; -
		MOVW	_tmpEA,_tmp6D0
		JSR		_div_32to16
		BCC		loc_11EC3B
		MOVWI	_tmpE6,$0000
loc_11EC3B:
		MOVW	_city._poll_lvl_avr,_tmpE6
		LDA		#$A0							; FIXME, section set and leave outside of this proc
		JMP		_critical_section_leave

; =============== S U B	R O U T	I N E =======================================
_pollution_recalc:
		LDA		#$00							; calculate pollution level
		STA		_tmpE6							; for currently selected area 4x4 tile
		STA		_tmpE7
		LDA		_tmp_area_pos._ROW
		ASL
		ASL
		STA		_tmp_map_pos._ROW
loc_11EC58:
		LDA		_tmp_area_pos._COL
		ASL
		ASL
		STA		_tmp_map_pos._COL
loc_11EC60:
		JSR		_get_cur_building_tile			; scan area for roads/buildings
		LDY		_last_map_tile_read				; rails also included as value entity
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_TRANSPORT|_FL_BUILDING
		BEQ		loc_11EC71
		LDA		#$01
		STA		_tmpE6							; when any of these are present, set flag
loc_11EC71:
		JSR		_get_pollution_ratio			; increment pollution ratio
		TYA
		CLC
		ADC		_tmpE7
		BCC		loc_11EC7C						; clamp to 255
		LDA		#$FF
loc_11EC7C:
		STA		_tmpE7
		INC		_tmp_map_pos._COL
		LDA		_tmp_map_pos._COL
		AND		#$03
		BNE		loc_11EC60
		INC		_tmp_map_pos._ROW
		LDA		_tmp_map_pos._ROW
		AND		#$03
		BNE		loc_11EC58
		RTS

; =============== S U B	R O U T	I N E =======================================
_population_dencity_recalc:
		JSR		_clear_tmp_map_B5
		LDY		#$00
		STY		_tmp6CC
		STY		_tmp6CC+1
		STY		_tmp6CE
		STY		_tmp6CE+1
		STY		_tmp6D0
		STY		_tmp6D0+1
loc_11ECAA:
		STY		_tmp_map_pos._ROW
		LDY		#$00
loc_11ECAF:
		STY		_tmp_map_pos._COL
		JSR		_map_tile_read
		STA		_last_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$08
		BEQ		loc_11ECF4
		JSR		_get_RCI_area_develop_state
		STA		_tmpE7
		JSR		_read_tmp_map_div4_mul13_nibble
		CLC
		ADC		_tmpE7
		JSR		_write_tmp_map_div4_mul13_nibble
		ADDWB	_tmp6CC,_tmp_map_pos._COL
		ADDWB	_tmp6CE,_tmp_map_pos._ROW
		INCW	_tmp6D0
loc_11ECF4:
		LDY		_tmp_map_pos._COL
		INY
		CPY		#$4C
		BNE		loc_11ECAF
		LDY		_tmp_map_pos._ROW
		INY
		CPY		#$4C
		BNE		loc_11ECAA
		LDY		#$02							; population dencity select
		STY		_tmp6C4
		JSR		_interpolate_tmp_to_select_poll_popg_popd_crime_land_maps
		JSR		_interpolate_tmp_with_select_poll_popg_popd_crime_land_to_tmp_map
		JSR		_interpolate_tmp_to_select_poll_popg_popd_crime_land_maps
		LDA		#$04
		JSR		_critical_section_leave			; FIXME, section set and leave outside of this proc
		LDA		_tmp6D0
		ORA		_tmp6D0+1
		BEQ		loc_11ED64
		MOVW	_tmpE7,_tmp6CC
		MOVW	_tmpEA,_tmp6D0
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		JSR		_div_32to16
		LDA		_tmpE7
		STA		_max_population_dencity_pos._COL
		MOVW	_tmpE7,_tmp6CE
		MOVW	_tmpEA,_tmp6D0
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		JSR		_div_32to16
		LDA		_tmpE7
		STA		_max_population_dencity_pos._ROW
		RTS
loc_11ED64:
		LDA		#$26
		STA		_max_population_dencity_pos._COL
		STA		_max_population_dencity_pos._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_RCI_area_develop_state:
		LDA		_last_map_tile_read
		CMP		#_MAP_C_AREA_DEF
		BCS		loc_11ED7C
		TAY
		LDA		_R_area_develop_levels_list,Y
		LSR
		LSR
		LSR
		RTS
loc_11ED7C:
		CMP		#_MAP_I_AREA_DEF
		BCS		loc_11ED88
		SEC
		SBC		#_MAP_C_AREA_DEF
		TAY
		LDA		_C_area_develop_levels_list,Y
		RTS
loc_11ED88:
		CMP		#_MAP_POLICE_STATION
		BCS		loc_11ED94
		SEC
		SBC		#_MAP_I_AREA_DEF
		TAY
		LDA		_I_area_develop_levels_list,Y
		RTS
loc_11ED94:
		LDA		#$00
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_police_fire_maps:
		LDA		#$00
		STA		_tmp6C4
		JSR		_interpolate_select_police_fire_to_tmp_map_div2
		JSR		_interpolate_tmp_to_select_police_fire_map_div4
		LDA		#$01
		STA		_tmp6C4
		JSR		_interpolate_select_police_fire_to_tmp_map_div2
		JMP		_interpolate_tmp_to_select_police_fire_map_div4

; =============== S U B	R O U T	I N E =======================================
; all maps here are 16 times smaller than game map
; calculation done on areas 4x4 tiles combined
; land value, population dencity and police radius value for each area
; are calculated previously
;
; start from the beginning of the map in 0,0 position
;
_crime_recalc:
		LDA		#$00
		STA		_tmp6CC							; CRIME_SUM here is overall sum of crime levels on map
		STA		_tmp6CC+1
		STA		_tmp6D0							; LAND_CNT here is the counter for areas with land values>0
		STA		_tmp6D0+1
loc_11EDBB:
		STA		_tmp_map_pos._ROW				; set cur ROW
		LDA		#$00
loc_11EDC0:
		STA		_tmp_map_pos._COL				; set cur COL
		JSR		_read_land_val_map_nibble	; read LAND value nibble [0-15]
		BEQ		loc_11EDF1						; skip if zero value, no crime
		STA		_tmpE7
		INCW	_tmp6D0							; increment lands counter
		LDA		#$0F							; calc complement of land value 15-LAND
		SEC										; so we have reverse proportion, less
		SBC		_tmpE7							; land value, higher the parameter here
		STA		_tmpE7
		JSR		_read_pop_dencity_map_nibble	; add here the POPUL dencity value [0-15]
		CLC										; sum would be [0-30]
		ADC		_tmpE7
		STA		_tmpE7
		JSR		_read_police_radius_map_nibble	; and then subtract the POLICE radius value times 4
		ASL										; value [0-60]
		ASL										; as a result, we will have slightly less crime
		STA		_tmpE6							; inside most of the police station area
		LDA		_tmpE7							; and less only on its edges and outside it
		SEC
		SBC		_tmpE6
		BCS		loc_11EDF1						; negative values are discarded
		LDA		#$00
loc_11EDF1:
		PHA										; backup crime level value
		JSR		_write_crime_map_nibble			; write new value, auto clamped to F
		PLA										; restore and add to sum
		CLC										; NOTE, we write here full sum with overflows
		ADC		_tmp6CC							; summing crime values for all areas
		STA		_tmp6CC							; max value here may be 19*19*15 = 5415
		BCC		loc_11EE02
		INC		_tmp6CC+1
loc_11EE02:
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EDC0
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EDBB
; BUG, this is not average again!
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		MOVW	_tmpE7,_tmp6CC					; now load divident
; -
; FIX, rescale average
		LDA		_tmp6D0							; fast divide to 8 in FIXED POINT!
		ASL										; you may think i multiply, NO!
		ROL		_tmp6D0+1						; result is fp8_8
		ASL										; so we cam have results in more wider range
		ROL		_tmp6D0+1
		ASL
		ROL		_tmp6D0+1
		STA		_tmp6D0
; -
		MOVW	_tmpEA,_tmp6D0
		JSR		_div_32to16
		BCC		loc_11EE3B
		MOVWI	_tmpE6,0						; default value 0
loc_11EE3B:
		MOVW	_city._crime_lvl_avr,_tmpE6		;
		LDA		#$40							; FIXME, section set and leave outside of this proc
		JMP		_critical_section_leave

; =============== S U B	R O U T	I N E =======================================
_interpolate_tmp_to_select_poll_popg_popd_crime_land_maps:
		LDA		#$00
loc_11EE4C:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11EE51:
		STA		_tmp_map_pos._COL
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_tmp_map_div4_mul13_surrond_sum
		JSR		_read_tmp_map_div4_mul13_nibble
		CLC
		ADC		_tmpE7
		LSR
		LSR
		CMP		#$0F
		BCC		loc_11EE69
		LDA		#$0F
loc_11EE69:
		JSR		_write_select_poll_popg_popd_crime_land_tmp_maps_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EE51
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EE4C
		RTS

; REDUNDANT, exact copy of _calc_tmp_map_div4_mul13_surrond_sum
; with _add_tmp_map_div4_mul13_nibble as well. merged
; =============== S U B	R O U T	I N E =======================================
;_calc_tmp_map_div4_mul13_surrond_sum_0:
;		LDA		_tmp_map_pos._COL
;		CMP		#$04
;		BCC		loc_11EE99
;		SBC		#$04
;		STA		_tmp_map_pos._COL
;		JSR		_add_tmp_map_div4_mul13_nibble_0
;		LDA		_tmp_map_pos._COL
;		CLC
;		ADC		#$04
;		STA		_tmp_map_pos._COL
;loc_11EE99:
;		LDA		_tmp_map_pos._COL
;		CMP		#$48
;		BCS		loc_11EEB1
;		ADC		#$04
;		STA		_tmp_map_pos._COL
;		JSR		_add_tmp_map_div4_mul13_nibble_0
;		LDA		_tmp_map_pos._COL
;		SEC
;		SBC		#$04
;		STA		_tmp_map_pos._COL
;loc_11EEB1:
;		LDA		_tmp_map_pos._ROW
;		CMP		#$04
;		BCC		loc_11EEC9
;		SBC		#$04
;		STA		_tmp_map_pos._ROW
;		JSR		_add_tmp_map_div4_mul13_nibble_0
;		LDA		_tmp_map_pos._ROW
;		CLC
;		ADC		#$04
;		STA		_tmp_map_pos._ROW
;loc_11EEC9:
;		LDA		_tmp_map_pos._ROW
;		CMP		#$48
;		BCS		locret_11EEE1
;		ADC		#$04
;		STA		_tmp_map_pos._ROW
;		JSR		_add_tmp_map_div4_mul13_nibble_0
;		LDA		_tmp_map_pos._ROW
;		SEC
;		SBC		#$04
;		STA		_tmp_map_pos._ROW
;locret_11EEE1:
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;_add_tmp_map_div4_mul13_nibble:
;		JSR		_read_tmp_map_div4_mul13_nibble_0
;		CLC
;		ADC		_tmpE7
;		STA		_tmpE7
;		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_tmp_with_select_poll_popg_popd_crime_land_to_tmp_map:
		LDA		#$00
loc_11EEED:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11EEF2:
		STA		_tmp_map_pos._COL
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_tmp_map_div4_mul13_surrond_sum
		JSR		_read_select_pol_popg_popd_crime_land_tmp_maps_nibble
		CLC
		ADC		_tmpE7
		LSR
		LSR
		CMP		#$0F
		BCC		loc_11EF0A
		LDA		#$0F
loc_11EF0A:
		JSR		_write_tmp_map_div4_mul13_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EEF2
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EEED
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_tmp_map_div4_mul13_surrond_sum:
		LDA		_tmp_map_pos._COL
		CMP		#$04
		BCC		loc_11EF3A
		SBC		#$04
		STA		_tmp_map_pos._COL
		JSR		_add_tmp_map_div4_mul13_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$04
		STA		_tmp_map_pos._COL
loc_11EF3A:
		LDA		_tmp_map_pos._COL
		CMP		#$48
		BCS		loc_11EF52
		ADC		#$04
		STA		_tmp_map_pos._COL
		JSR		_add_tmp_map_div4_mul13_nibble
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$04
		STA		_tmp_map_pos._COL
loc_11EF52:
		LDA		_tmp_map_pos._ROW
		CMP		#$04
		BCC		loc_11EF6A
		SBC		#$04
		STA		_tmp_map_pos._ROW
		JSR		_add_tmp_map_div4_mul13_nibble
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$04
		STA		_tmp_map_pos._ROW
loc_11EF6A:
		LDA		_tmp_map_pos._ROW
		CMP		#$48
		BCS		locret_11EF82
		ADC		#$04
		STA		_tmp_map_pos._ROW
		JSR		_add_tmp_map_div4_mul13_nibble
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$04
		STA		_tmp_map_pos._ROW
locret_11EF82:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_tmp_map_div4_mul13_nibble:
		JSR		_read_tmp_map_div4_mul13_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_tmp_to_select_police_fire_map_div4:
		LDA		#$00
loc_11EF8E:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11EF93:
		STA		_tmp_map_pos._COL
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_tmp_map_div8_mulA_surrond_sum
		JSR		_read_tmp_map_div8_mulA_nibble
		CLC
		ADC		_tmpE7
		LSR
		LSR
		CMP		#$0F
		BCC		loc_11EFAB
		LDA		#$0F
loc_11EFAB:
		JSR		_write_select_police_fire_tmp_maps_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11EF93
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11EF8E
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_tmp_map_div8_mulA_surrond_sum:
		LDA		_tmp_map_pos._COL
		CMP		#$08
		BCC		loc_11EFDB
		SBC		#$08
		STA		_tmp_map_pos._COL
		JSR		_add_tmp_map_div8_mulA_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$08
		STA		_tmp_map_pos._COL
loc_11EFDB:
		LDA		_tmp_map_pos._COL
		CMP		#$44
		BCS		loc_11EFF3
		ADC		#$08
		STA		_tmp_map_pos._COL
		JSR		_add_tmp_map_div8_mulA_nibble
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$08
		STA		_tmp_map_pos._COL
loc_11EFF3:
		LDA		_tmp_map_pos._ROW
		CMP		#$08
		BCC		loc_11F00B
		SBC		#$08
		STA		_tmp_map_pos._ROW
		JSR		_add_tmp_map_div8_mulA_nibble
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$08
		STA		_tmp_map_pos._ROW
loc_11F00B:
		LDA		_tmp_map_pos._ROW
		CMP		#$48
		BCS		locret_11F023
		ADC		#$08
		STA		_tmp_map_pos._ROW
		JSR		_add_tmp_map_div8_mulA_nibble
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$08
		STA		_tmp_map_pos._ROW
locret_11F023:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_tmp_map_div8_mulA_nibble:
		JSR		_read_tmp_map_div8_mulA_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_select_police_fire_to_tmp_map_div2:
		LDA		#$00
loc_11F02F:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11F034:
		STA		_tmp_map_pos._COL
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_select_police_fire_tmp_surrond_sum
		JSR		_read_select_police_fire_tmp_map_nibble
		CLC
		ADC		_tmpE7
		LSR
		CMP		#$0F
		BCC		loc_11F04B
		LDA		#$0F
loc_11F04B:
		JSR		_write_tmp_map_div8_mulA_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11F034
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11F02F
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_select_police_fire_tmp_surrond_sum:
		LDA		_tmp_map_pos._COL
		CMP		#$08
		BCC		loc_11F07B
		SBC		#$08
		STA		_tmp_map_pos._COL
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$08
		STA		_tmp_map_pos._COL
loc_11F07B:
		LDA		_tmp_map_pos._COL
		CMP		#$44
		BCS		loc_11F093
		ADC		#$08
		STA		_tmp_map_pos._COL
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._COL
		SEC
		SBC		#$08
		STA		_tmp_map_pos._COL
loc_11F093:
		LDA		_tmp_map_pos._ROW
		CMP		#$08
		BCC		loc_11F0AB
		SBC		#$08
		STA		_tmp_map_pos._ROW
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$08
		STA		_tmp_map_pos._ROW
loc_11F0AB:
		LDA		_tmp_map_pos._ROW
		CMP		#$44
		BCS		locret_11F0C3
		ADC		#$08
		STA		_tmp_map_pos._ROW
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		#$08
		STA		_tmp_map_pos._ROW
locret_11F0C3:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_select_police_fire_recalc:
		JSR		_read_select_police_fire_tmp_map_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_clear_tmp_map_B5:
		JSR		_sram_write_enable
		LDY		#$00
		TYA
loc_11F0D3:
		STA		_tmp_map,Y
		INY
		CPY		#$B5
		BNE		loc_11F0D3
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
; return
; 2 - if developed I area (41-4F)
; 3 - if coal power or sea/air port, or fires
; F - if flooded ground
; 0 - other
;
; NOTE, there is no dependency on roads/traffic values, because
; we work with raw map indexes, not the traffic indexes
;
; TODO, add roads to overall value
;
_get_pollution_ratio:
		LDY		#$02
		LDA		_last_map_tile_read
		CMP		#_MAP_POLICE_STATION			; upper tile range for developed I
		BCS		loc_11F0F0
		CMP		#_MAP_I_AREA_DEF				; lower tile range for dev I
		BCC		loc_11F0F0
; REDUNDANT
;		BNE		locret_11F10A					; skip very first undeveloped
;		LDY		#$00
; -
; OPTIMIZED
		BEQ		locret_11F109					; return Y = 0
; -
		RTS										; return current Y = 2 if dev I area
loc_11F0F0:
		LDY		#$03
		CMP		#_MAP_FIRE
		BEQ		locret_11F10A
		CMP		#_MAP_SEA_PORT
		BEQ		locret_11F10A
		CMP		#_MAP_AIR_PORT
		BEQ		locret_11F10A
		CMP		#_MAP_COAL_POWER
		BEQ		locret_11F10A
		LDY		#$0F
;		CMP		#_MAP_FLOOD						; BUG! FLOOD tile idx
		CMP		#_MAP_POLLUTION					; FIX, nuclear waste pollution, like in Micropolis
		BEQ		locret_11F10A
locret_11F109:
		LDY		#$00
locret_11F10A:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F10B:
		LDA		_max_population_dencity_pos._COL
		LSR
		LSR
		SEC
		SBC		_tmp_area_pos._COL
		BCS		loc_11F11B
		EOR		#$FF
		CLC
		ADC		#$01
loc_11F11B:
		STA		_tmpE6
		LDA		_max_population_dencity_pos._ROW
		LSR
		LSR
		SEC
		SBC		_tmp_area_pos._ROW
		BCS		loc_11F12D
		EOR		#$FF
		CLC
		ADC		#$01
loc_11F12D:
		CLC
		ADC		_tmpE6
		CMP		#$0E
		BCC		locret_11F136
		LDA		#$0E
locret_11F136:
		RTS

; =============== S U B	R O U T	I N E =======================================
; get the actual index of the area if the area is a large building
; or get the individual tile number for single tiles
;
_get_cur_building_tile:
		JSR		_map_tile_read					; read currently selected tile
		STA		_last_map_tile_read				; store read value for further usage
		LDY		#$00
		STY		_cur_map_tile_is_inside_a_building_flag
		AND		#$F0
		CMP		#$80
		BNE		locret_11F177					; skip if a sigle-tile index
		LDX		#$00
		JSR		_tmp_map_pos_backup				; backup current tile position in slot 0
		LDA		_last_map_tile_read
		AND		#$0F							; calculate position of an actual
		TAY										; index tile of the building
; REDUNDANT, weird, why not just subtract here?
;		LDX		_internal_building_tile_deltaX_list,Y
;		BEQ		loc_11F15E
;loc_11F158:
;		DEC		_tmp_map_pos._COL
;		DEX
;		BNE		loc_11F158
;loc_11F15E:
;		LDX		_internal_building_tile_deltaY_list,Y
;		BEQ		loc_11F169
;loc_11F163:
;		DEC		_tmp_map_pos._ROW
;		DEX
;		BNE		loc_11F163
;loc_11F169:
; OPTIMIZED, they used the same code in other places but with X indexing
		LDA		_tmp_map_pos._COL
		SEC
		SBC		_internal_building_tile_deltaX_list,Y
		STA		_tmp_map_pos._COL
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		_internal_building_tile_deltaY_list,Y
		STA		_tmp_map_pos._ROW
; -
		JSR		_map_tile_read					; read the actual index tile
		STA		_last_map_tile_read				; store it instead
		INC		_cur_map_tile_is_inside_a_building_flag	; set the special flag
		LDX		#$00
		JMP		_tmp_map_pos_restore			; restore current map position
locret_11F177:
		RTS

_internal_building_tile_deltaX_list:
		.BYTE	$00,$01,$01,$00,$01,$02,$02,$02,$00,$01,$02,$03,$03,$03,$03
_internal_building_tile_deltaY_list:
		.BYTE	$01,$01,$00,$02,$02,$02,$01,$00,$03,$03,$03,$03,$02,$01,$00

; =============== S U B	R O U T	I N E =======================================
_map_tile_write:
		PHA
; REDUNDANT, too slow for such frequently used routine
;		PUSHB	_tmp_map_pos._ROW
;		PUSHB	#$4C
;		JSR		_mmc5_mul8to8
;		PLA
;		CLC
;		ADC		#<[_city_map]
;		STA		word_EC
;		PLA
;		ADC		#>[_city_map]
;		STA		word_EC+1
;		LDY		_tmp_map_pos._COL
; -
; OPTIMIZED, FASTER!
		BUFPOS	word_EC,_tmp_map_pos._COL,_tmp_map_pos._ROW,#$4C,#>[_city_map]
; -
		JSR		_sram_write_enable
		PLA
		LDY		#$00
		STA		(word_EC),Y
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_tmp_map_pos_backup:
		LDA		_tmp_map_pos._ROW
		STA		_tmp_map_pos_backup_buf_Y,X
		LDA		_tmp_map_pos._COL
		STA		_tmp_map_pos_backup_buf_X,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_pos_restore:
		LDA		_tmp_map_pos_backup_buf_Y,X
		STA		_tmp_map_pos._ROW
		LDA		_tmp_map_pos_backup_buf_X,X
		STA		_tmp_map_pos._COL
		RTS

; =============== S U B	R O U T	I N E =======================================
_overall_stats_clear:
		LDX		#$00
		TXA
loc_11F242:
		STA		_total_RCI_counts,X				; TODO: make sure when RAM vars are
		INX										; moved, all these vars whould be here
		CPX		#$2C							; maybe we should make a structure here
		BNE		loc_11F242
		STA		_city._level._R.FRAC
		STA		_city._level._R.INT
		STA		_city._level._C.FRAC
		STA		_city._level._C.INT
		STA		_city._level._I.FRAC
		STA		_city._level._I.INT
		LDA		_city._bank_flags
		AND		#$C0
		STA		_city._bank_flags
		JSR		_sram_write_enable
		LDY		#$00
		TYA
loc_11F26A:
		STA		_fire_map,Y
		STA		_police_map,Y
		INY
		CPY		#$32
		BNE		loc_11F26A
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
_population_growth_recalc:
		LDA		#$00
loc_11F27A:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11F27F:
		STA		_tmp_map_pos._COL
		JSR		_read_pop_growth_map_nibble
		CMP		#$08
		BEQ		loc_11F296
		BCS		loc_11F290
		CLC
		ADC		#$01
		BNE		loc_11F293
loc_11F290:
		SEC
		SBC		#$01
loc_11F293:
		JSR		_write_pop_growth_map_nibble
loc_11F296:
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11F27F
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11F27A
		RTS

; =============== S U B	R O U T	I N E =======================================
_traffic_smooth:
		LDA		#$00
loc_11F2AD:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11F2B2:
		STA		_tmp_map_pos._COL
		JSR		_read_traffic_map_nibble
		CMP		#$03
		BCS		loc_11F2C0
		LDA		#$00
		BEQ		loc_11F2CD
loc_11F2C0:
		CMP		#$0A
		BCS		loc_11F2CA
		SEC
		SBC		#$03
		JMP		loc_11F2CD
loc_11F2CA:
		SEC
		SBC		#$04
loc_11F2CD:
		JSR		_write_traffic_map_nibble
		LDA		_tmp_map_pos._COL
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11F2B2
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11F2AD
		RTS

; =============== S U B	R O U T	I N E =======================================
_critical_section_leave:
		PHA										; clear thread 0 mask
		EOR		#$FF
		AND		_thread0_flags
		STA		_thread0_flags
		PLA
		ORA		_thread1_flags					; set thread 1 mask
		STA		_thread1_flags
		RTS

; =============== S U B	R O U T	I N E =======================================
_critical_section_enter:
		ORA		_thread0_flags					; set thread 0 mask
		STA		_thread0_flags
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_corruption_tiles_animate:
		LDA		_corruption_anim_flag			; end of loop signal, if equal
		BEQ		loc_11F33A						; then no tiles to animate left
		JSR		_sram_write_enable				; enable access to sram
		MOVWO	_tmpE6,_city_map				; pointer to the beginning
		LDY		#$00
		STY		_corruption_anim_flag
loc_11F310:
		LDA		(_tmpE6),Y						; scan, look for corrupt tiles
		SEC
		SBC		#_MAP_CORRUPT0					; start with _MAP_CORRUPT0
		BCC		loc_11F324						; skip if less than index
		CMP		#$04
		BCS		loc_11F324						; there are 4 total anim tiles tile
		TAX										; and one common corrupted ground
		LDA		_corrupt_ground_tiles_list,X	; then change to the next in list
		STA		(_tmpE6),Y
		STA		_corruption_anim_flag			; set flag
loc_11F324:
		INCW	_tmpE6
		CMPWI	_tmpE6,_city_map_end			; test for end of buffer
		BNE		loc_11F310
		JSR		_screen_invalidate				; update the screen
		JMP		_map_corruption_tiles_animate
loc_11F33A:
		JMP		_sram_write_disable				; disable writes back

_corrupt_ground_tiles_list:
		.BYTE	_MAP_CORRUPT1,_MAP_CORRUPT2,_MAP_CORRUPT3,_MAP_CORRUPTED

; =============== S U B	R O U T	I N E =======================================
; {land_value-pollution}*7-52
;
_get_growth_ratio:
		JSR		_read_pollution_map_nibble
		STA		_tmpE7
		JSR		_read_land_val_map_nibble
		SEC
		SBC		_tmpE7
		BCS		loc_11F350
		LDA		#$00
loc_11F350:
		PHA
		PUSHB	#$07
		JSR		_mmc5_mul8to8					; multiply to 7 this time and
		PLA										; shift to signed range
		TAX
		PLA
		TXA
		SEC
		SBC		#$34
		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref_6:
;		JMP		_unused_11

; =============== S U B	R O U T	I N E =======================================
sub_11F362:
		LDA		#$00
		RTS

; =============== S U B	R O U T	I N E =======================================
_pop_growth_map_update:
		STA		_tmpE7
		JSR		_read_pop_growth_map_nibble
		CLC
		ADC		_tmpE7
		JMP		_write_pop_growth_map_nibble

; =============== S U B	R O U T	I N E =======================================
; a = land_value - pollution
;  if a < 2    ratio 0
;  if a 2 to 4 ratio 1
;  if a 5 to 8 ratio 2
;  if a > 8    ratio 3
;
_get_life_ratio:
		JSR		_read_pollution_map_nibble
		STA		_tmpE7
		JSR		_read_land_val_map_nibble
		SEC
		SBC		_tmpE7
		LDX		#$00
		CMP		#$02
		BCC		loc_11F66E
		INX
		CMP		#$05
		BCC		loc_11F66E
		INX
		CMP		#$09
		BCC		loc_11F66E
		INX
loc_11F66E:
		STX		_life_ratio_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_read_select_pol_popg_popd_crime_land_tmp_maps_nibble:
		LDY		_tmp6C4
		BEQ		_read_pollution_map_nibble
		DEY
		BEQ		_read_pop_growth_map_nibble
		DEY
		BEQ		_read_pop_dencity_map_nibble
		DEY
		BEQ		_read_crime_map_nibble
		DEY
		BEQ		_read_land_val_map_nibble
		BNE		_read_tmp_map_div4_mul13_nibble

; =============== S U B	R O U T	I N E =======================================
_read_pollution_map_nibble:
		LDXY	_pollution_map
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_pop_growth_map_nibble:
		LDXY	_pop_growth_map
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_pop_dencity_map_nibble:
		LDXY	_pop_dencity_map
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_crime_map_nibble:
		LDXY	_crime_map
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_land_val_map_nibble:
		LDXY	_land_val_map
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_tmp_map_div4_mul13_nibble:
		LDXY	_tmp_map

; =============== S U B	R O U T	I N E =======================================
_read_buf_pos_div4_mul13_4bit_nibble:
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._ROW
		LSR
		LSR
		MULAI	#$13
		LDA		_tmp_map_pos._COL
		LSR
		LSR
		JMP		_read_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_read_traffic_map_nibble:
		LDXY	_traffic_map
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._ROW
		LSR
		MULAI	#$26
		LDA		_tmp_map_pos._COL
		LSR
		JMP		_read_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_read_select_police_fire_tmp_map_nibble:
		LDY		_tmp6C4
		BEQ		_read_police_radius_map_nibble
		DEY
		BEQ		_read_fire_radius_map_nibble

; =============== S U B	R O U T	I N E =======================================
_read_tmp_map_div8_mulA_nibble:
		LDXY	_tmp_map
		BNE		_read_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_police_radius_map_nibble:
		LDXY	_police_map
		BNE		_read_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_fire_radius_map_nibble:
		LDXY	_fire_map

; =============== S U B	R O U T	I N E =======================================
_read_buf_pos_div8_mulA_4bit_nibble:
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._ROW
		LSR
		LSR
		LSR
		MULAI	#$0A
		LDA		_tmp_map_pos._COL
		LSR
		LSR
		LSR

; =============== S U B	R O U T	I N E =======================================
_read_buf_pos_4bit_nibble_ex:
		CLC
		ADC		_MMC5_MUL0
		STA		word_EE
		LDA		_MMC5_MUL1
		ADC		#$00
		STA		word_EE+1
		LSR		word_EE+1
		ROR		word_EE
		ROR		_tmpE6
		PLA
		CLC
		ADC		word_EE
		STA		word_EE
		PLA
		ADC		word_EE+1
		STA		word_EE+1
		LDY		#$00
		LDA		(word_EE),Y
		BIT		_tmpE6
		BMI		loc_11F748
		LSR
		LSR
		LSR
		LSR
		RTS
loc_11F748:
		AND		#$0F
		RTS

; =============== S U B	R O U T	I N E =======================================
_surronding_map_are_non_destructable_tile_sum:
		LDA		#$00
		STA		_tmp6D3
		JSR		_map_area_non_destructable_tiles_count
		ASL		_tmp6D3
		ASL		_tmp6D3
		DEC		_tmp_area_pos._ROW
		BMI		loc_11F761
		JSR		_map_area_non_destructable_tiles_count
loc_11F761:
		INC		_tmp_area_pos._ROW
		INC		_tmp_area_pos._ROW
		LDA		_tmp_area_pos._ROW
		CMP		#$13
		BCS		loc_11F771
		JSR		_map_area_non_destructable_tiles_count
loc_11F771:
		DEC		_tmp_area_pos._ROW
		DEC		_tmp_area_pos._COL
		BMI		loc_11F77C
		JSR		_map_area_non_destructable_tiles_count
loc_11F77C:
		INC		_tmp_area_pos._COL
		INC		_tmp_area_pos._COL
		LDA		_tmp_area_pos._COL
		CMP		#$13
		BCS		loc_11F78C
		JSR		_map_area_non_destructable_tiles_count
loc_11F78C:
		DEC		_tmp_area_pos._COL
		ASL		_tmp6D3
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_area_non_destructable_tiles_count:
		LDA		_tmp_area_pos._ROW
		ASL
		ASL
		STA		_tmp_map_pos._ROW
loc_11F79B:
		LDA		_tmp_area_pos._COL
		ASL
		ASL
		STA		_tmp_map_pos._COL
loc_11F7A3:
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$04
		BEQ		loc_11F7B1
		INC		_tmp6D3
loc_11F7B1:
		INC		_tmp_map_pos._COL
		LDA		_tmp_map_pos._COL
		AND		#$03
		BNE		loc_11F7A3
		INC		_tmp_map_pos._ROW
		LDA		_tmp_map_pos._ROW
		AND		#$03
		BNE		loc_11F79B
		RTS

; =============== S U B	R O U T	I N E =======================================
_write_select_poll_popg_popd_crime_land_tmp_maps_nibble:
		LDY		_tmp6C4
		BEQ		_write_pollution_map_nibble
		DEY
		BEQ		_write_pop_growth_map_nibble
		DEY
		BEQ		_write_pop_dencity_map_nibble
		DEY
		BEQ		_write_crime_map_nibble
		DEY
		BEQ		_write_land_val_map_nibble
		BNE		_write_tmp_map_div4_mul13_nibble

; =============== S U B	R O U T	I N E =======================================
_write_pollution_map_nibble:
		LDXY	_pollution_map
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_pop_growth_map_nibble:
		LDXY	_pop_growth_map
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_pop_dencity_map_nibble:
		LDXY	_pop_dencity_map
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_crime_map_nibble:
		LDXY	_crime_map
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_land_val_map_nibble:
		LDXY	_land_val_map
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_tmp_map_div4_mul13_nibble:
		LDXY	_tmp_map

; =============== S U B	R O U T	I N E =======================================
_write_buf_pos_div4_mul13_4bit_nibble:
		CMP		#$10
		BCC		loc_11F801
		LDA		#$0F
loc_11F801:
		STA		byte_E5
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._ROW
		LSR
		LSR
;		STA		_mmc5_mul1_shadow
;		STA		_MMC5_MUL1
;		LDA		#$13
;		STA		_mmc5_mul1_shadow				; BUG: copy-paste, same shadow var, may be critical
;		STA		_MMC5_MUL0
		MULAI	#$13							; FIX: uses correct macro
		LDA		_tmp_map_pos._COL
		LSR
		LSR
		JMP		_write_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_write_traffic_map_nibble:
		LDXY	_traffic_map
		CMP		#$10
		BCC		loc_11F82C
		LDA		#$0F
loc_11F82C:
		STA		byte_E5
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._ROW
		LSR
		MULAI	#$26
		LDA		_tmp_map_pos._COL
		LSR
		JMP		_write_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_write_select_police_fire_tmp_maps_nibble:
		LDY		_tmp6C4
		BEQ		_write_police_radius_map_nibble
		DEY
		BEQ		_write_fire_radius_map_nibble

; =============== S U B	R O U T	I N E =======================================
_write_tmp_map_div8_mulA_nibble:
		LDXY	_tmp_map
		BNE		_write_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_police_radius_map_nibble:
		LDXY	_police_map
		BNE		_write_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_fire_radius_map_nibble:
		LDXY	_fire_map

; =============== S U B	R O U T	I N E =======================================
_write_buf_pos_div8_mulA_4bit_nibble:
		CMP		#$10
		BCC		loc_11F869
		LDA		#$0F
loc_11F869:
		STA		byte_E5
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._ROW
		LSR
		LSR
		LSR
		MULAI	#$0A
		LDA		_tmp_map_pos._COL
		LSR
		LSR
		LSR

; =============== S U B	R O U T	I N E =======================================
_write_buf_pos_4bit_nibble_ex:
		CLC
		ADC		_MMC5_MUL0
		STA		word_EE
		LDA		_MMC5_MUL1
		ADC		#$00
		STA		word_EE+1
		LSR		word_EE+1
		ROR		word_EE
		ROR		_tmpE6
		PLA
		CLC
		ADC		word_EE
		STA		word_EE
		PLA
		ADC		word_EE+1
		STA		word_EE+1
		LDY		#$00
		LDA		#$F0
		BIT		_tmpE6
		BMI		loc_11F8BB
		ASL		byte_E5
		ASL		byte_E5
		ASL		byte_E5
		ASL		byte_E5
		LSR
		LSR
		LSR
		LSR
loc_11F8BB:
		AND		(word_EE),Y
		ORA		byte_E5
		JSR		_sram_write_enable
		STA		(word_EE),Y
		JMP		_sram_write_disable

; =============== S U B	R O U T	I N E =======================================
;_unused_11:
;		LDA		_max_population_dencity_pos._COL
;		SEC
;		SBC		_tmp_map_pos._COL
;		BCS		loc_11F8D5
;		EOR		#$FF
;		CLC
;		ADC		#$01
;loc_11F8D5:
;		LSR
;		LSR
;		LSR
;		STA		_tmpE6
;		LDA		_max_population_dencity_pos._ROW
;		SEC
;		SBC		_tmp_map_pos._ROW
;		BCS		loc_11F8E8
;		EOR		#$FF
;		CLC
;		ADC		#$01
;loc_11F8E8:
;		LSR
;		LSR
;		LSR
;		CLC
;		ADC		_tmpE6
;		CMP		#$0F
;		BCC		locret_11F8F4
;		LDA		#$0F
;locret_11F8F4:
;		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_area_pos_to_tmp_map_pos_convert:
		LDA		_tmp_area_pos._COL
		ASL
		ASL
		STA		_tmp_map_pos._COL
		LDA		_tmp_area_pos._ROW
		ASL
		ASL
		STA		_tmp_map_pos._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
_heavy_traffic_report_test:
		PUSHW	_tmp_map_pos
		PUSHW	word_EE
		PUSHB	_tmpE6
		PUSHB	_mmc5_mul1_shadow
		PUSHB	_mmc5_mul0_shadow
		STX		_tmp_map_pos._COL
		STY		_tmp_map_pos._ROW
		JSR		_read_traffic_map_nibble
		CMP		#$0A
		BCC		loc_11F93E
		LDA		#$18
		STA		_scroll_msg_idx
		LDY		#$00
		STY		_scroll_msg_start_pos
		DEY
		STY		_scroll_msg_active_flag
		APUA_SE	_SE_IDX_TRAFFIC
loc_11F93E:
		POPB	_mmc5_mul0_shadow
		POPB	_mmc5_mul1_shadow
		POPB	_tmpE6
		POPW	word_EE
		POPW	_tmp_map_pos
		RTS

_R_area_develop_levels_list:
		.BYTE	$00,$04,$06,$08,$0C,$10,$18,$20,$28,$10,$18,$20,$28,$10,$18,$20
		.BYTE	$28,$10,$18,$20,$28,$28,$28,$28,$28
_C_area_develop_levels_list:
		.BYTE	$00,$01,$02,$03,$04,$05,$01,$02,$03,$04,$05,$01,$02,$03,$04,$05
		.BYTE	$01,$02,$03,$04,$05,$05,$05,$05,$05
_I_area_develop_levels_list:
		.BYTE	$00,$01,$02,$03,$04,$01,$02,$03,$04

; =============== S U B	R O U T	I N E =======================================
_disasters_handlers_select:
		JSR		_disasters_trigger_for_scenario_mode
		LDA		_disasters_flags
		BEQ		locret_11FB47
		LSR
		BCC		loc_11FB2C
		JMP		_fire_disaster_start
loc_11FB2C:
		LSR
		BCC		loc_11FB32
		JMP		_flood_disaster_start
loc_11FB32:
		LSR
		BCC		loc_11FB38
		JMP		_airplane_disaster_active_set
loc_11FB38:
		LSR
		BCC		loc_11FB3E
		JMP		_tornado_disaster_start
loc_11FB3E:
		LSR
		BCC		loc_11FB44
		JMP		_earthquake_disaster_start
loc_11FB44:
		JMP		_monster_disaster_start
locret_11FB47:
		RTS

; =============== S U B	R O U T	I N E =======================================
; UNUSED
;_fire_disaster_small_start_unref:
;		LDY		#$14
;		BNE		_fire_disaster_start_common

; =============== S U B	R O U T	I N E =======================================
; The Fire disaster is the only disaster that occurs only by manual selecting
; from the disasterS menu.
; There are two types of Fire disaster were planned, but the weaker one isn't used
;
_fire_disaster_start:
		LDY		#$28							; how much tiles we will damage

; =============== S U B	R O U T	I N E =======================================
_fire_disaster_start_common:
		TYA
		PHA
		LDA		#$4C
		JSR		_rand_clamp_A					; select random tile to burn
		STA		_tmp_map_pos._COL				; through the whole map
		LDA		#$4C							; so fire can start at many points
		JSR		_rand_clamp_A					; at the same time
		STA		_tmp_map_pos._ROW
		JSR		_map_tile_read					; read cur tile under the selection
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$48
		CMP		#$48
		BNE		loc_11FB73						; check for non-destructable tiles
		JSR		_fire_cur_map_tile				; destroy the rest
		JSR		_goto_alert_start				; points to very last burnt tile only
loc_11FB73:
		PLA
		TAY
		DEY
		BNE		_fire_disaster_start_common
		LDA		_disasters_flags				; the rest as usual
		AND		#$FE
		STA		_disasters_flags
		LDY		#$06
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$20
		STA		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
		JMP		_screen_invalidate				; if goto icon is used

; =============== S U B	R O U T	I N E =======================================
_flood_disaster_start:
		LDA		#$00
		STA		_tmp6CC
		LDY		#$FA
loc_11FB9A:
		TYA
		PHA
		LDA		#$4C
		JSR		_rand_clamp_A
		STA		_tmp_map_pos._COL
		LDA		#$4C
		JSR		_rand_clamp_A
		STA		_tmp_map_pos._ROW
		JSR		_map_tile_read
		CMP		#$BE
		BCC		loc_11FC04
		CMP		#$CE
		BCS		loc_11FC04
		LDY		#$03
loc_11FBB9:
		TYA
		PHA
		PUSHB	_tmp_map_pos._COL
		CLC
		ADC		_near_tile_pos_delta_col_list,Y
		STA		_tmp_map_pos._COL
		PUSHB	_tmp_map_pos._ROW
		CLC
		ADC		_near_tile_pos_delta_row_list,Y
		STA		_tmp_map_pos._ROW
		JSR		_tmp_map_pos_test_max
		BCS		loc_11FBF2
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$02
		BEQ		loc_11FBF2
		JSR		_flood_cur_map_tile
		LDA		_disasters_flags
		AND		#$FD
		STA		_disasters_flags
		INC		_tmp6CC
		LDA		#$1E
		STA		_disaster_tiles_to_flood_count
loc_11FBF2:
		POPB	_tmp_map_pos._ROW
		POPB	_tmp_map_pos._COL
		PLA
		TAY
		LDA		_tmp6CC
		BNE		loc_11FC04
		DEY
		BPL		loc_11FBB9
loc_11FC04:
		PLA
		TAY
		LDA		_tmp6CC
		BNE		loc_11FC0E
		DEY
		BNE		loc_11FB9A
loc_11FC0E:
		LDY		#$07
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$21
		STA		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
		JSR		_screen_invalidate
		JMP		_goto_alert_start
_near_tile_pos_delta_col_list:
		.BYTE	$00,$01,$00,$FF
_near_tile_pos_delta_row_list:
		.BYTE	$FF,$00,$01,$00

; =============== S U B	R O U T	I N E =======================================
_earthquake_disaster_start:
		JSR		_rand
		CLC
		ADC		#$64
		BCC		loc_11FC38
		LDA		#$FF
loc_11FC38:
		STA		_tmp6CE
		STA		_earthquake_shake_req
loc_11FC3E:
		LDA		#$4C
		JSR		_rand_clamp_A
		STA		_tmp_map_pos._COL
		LDA		#$4C
		JSR		_rand_clamp_A
		STA		_tmp_map_pos._ROW
		JSR		_get_cur_building_tile
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$08
		BEQ		loc_11FC69
		LDA		_tmp6CE
		AND		#$03
		BEQ		loc_11FC64
		LDA		#_MAP_CORRUPT0
		BNE		loc_11FC66
loc_11FC64:
		LDA		#_MAP_FIRE
loc_11FC66:
		JSR		_corrupt_cur_map_tile
loc_11FC69:
		DEC		_tmp6CE
		BNE		loc_11FC3E
loc_11FC6E:
		JSR		_screen_invalidate
		LDA		_earthquake_shake_req
		BNE		loc_11FC6E
		LDY		#$0A
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$0A
		STA		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
		LDA		_disasters_flags
		AND		#$EF
		STA		_disasters_flags
		JMP		_goto_alert_start

; =============== S U B	R O U T	I N E =======================================
_airplane_disaster_active_set:
		LDA		#$01
		STA		_airplane_crash_req				; for some reason, airplane crash
		LDA		_disasters_flags				; handler in another place in system
		AND		#$FB							; handlers manager loop of THREAD0
		STA		_disasters_flags				; don't know why
		RTS

; =============== S U B	R O U T	I N E =======================================
_tornado_disaster_start:
		LDA		_obj_active_flags				; tornado and monster can't go together
		AND		#$30
		BNE		locret_11FCFC
		LDA		_obj_active_flags				; active flag
		ORA		#$20
		STA		_obj_active_flags
		LDA		#$4C
		STA		_mmc5_chr_banks_shadow+1
		STA		_MMC5_CHR_BANKSA+1				; CHR bank init
		LDA		#$4C
		JSR		_rand_clamp_A					; select random map starting pos
		STA		_obj_pos._tornado._COL
		STA		_tmp_map_pos._COL
		LDA		#$4C
		JSR		_rand_clamp_A
		STA		_obj_pos._tornado._ROW
		STA		_tmp_map_pos._ROW
		LDA		#$4C
		JSR		_rand_clamp_A					; and absolutely random dst pos on map
		STA		_obj_dst_pos._tornado._COL
		LDA		#$4C
		JSR		_rand_clamp_A
		STA		_obj_dst_pos._tornado._ROW
		LDA		_disasters_flags				; clear disaster flag as well
		AND		#$F7
		STA		_disasters_flags
		LDY		#$09
		FJSR	_city_history_write,SRAM,PRGA	; in memories
		LDA		#$23
		STA		_game_msg_idx					; draw message
		LDA		#$01
		STA		_game_msg_awaits_flag
		JSR		_screen_invalidate
		APUA_SE	_SE_IDX_TORNADO
		JMP		_goto_alert_start
locret_11FCFC:
		RTS

; =============== S U B	R O U T	I N E =======================================
; one of two different routines to select starting map position to monster
; this routine select two random map positions, then set the dst coordinates
; to the opposite side of the map which is farthest from the current point.
; +--|+---+ so we always will take the longer path both directions, this
; |y V|<-x| results in the monster may travel only to the corners of the map
; +---+---+ 0:0, 0:4B, 4B:0, 4B:4B. final portion of the routine will change
; |x->|^ y| randomly one of the dest coordinates to something other than
; +---+|--+ corner positions. so the monster will walk to one of the sides
; of the map, but with random exit coordinate.
;
_monster_disaster_start:
		LDA		_obj_active_flags				; monster and tornado cannot walk
		AND		#$30							; together, they used the same CHR
		BNE		locret_11FD73					; bank
		LDY		#$00
		LDA		#$4C
		JSR		_rand_clamp_A					; appear at random map position
		STA		_obj_pos._monster._COL
		STA		_tmp_map_pos._COL
		CMP		#$26
		BCS		loc_11FD17						; walk to farther side of the map
		LDY		#$4B							; from one half of to another by X
loc_11FD17:
		STY		_obj_dst_pos._monster._COL
		LDY		#$00
		LDA		#$4C
		JSR		_rand_clamp_A					; the same for Y
		STA		_obj_pos._monster._ROW
		STA		_tmp_map_pos._ROW
		CMP		#$26
		BCS		loc_11FD2D
		LDY		#$4B
loc_11FD2D:
;		STY		_obj_dst_pos._monster._COL		; BUG, seems another copypaste here
		STY		_obj_dst_pos._monster._ROW		; FIX
		LDA		#$97
		JSR		_rand_clamp_A					; if this code change X position,
		LSR										; the Y will be uninitialized actually
		BCS		loc_11FD3D						; due to BUG above, but seems it's not
		STA		_obj_dst_pos._monster._COL		; much important, since we have it initialized to
		BCC		_monster_disaster_start_ex		; 0 at the beginning and then have value
loc_11FD3D:										; from previous moster appearance.
		STA		_obj_dst_pos._monster._ROW

; =============== S U B	R O U T	I N E =======================================
_monster_disaster_start_ex:
		LDA		_obj_active_flags				; monster activity flag set
		ORA		#$10
		STA		_obj_active_flags
		LDA		#$35							; WRONG: don't know why, this is not initial
		LDA		#$44							; FIX
		STA		_mmc5_chr_banks_shadow+1
		STA		_MMC5_CHR_BANKSA+1				; monster CHR bank
		LDA		_disasters_flags				; removes disaster request flag
		AND		#$DF
		STA		_disasters_flags
		LDA		#$01
		STA		_monster_in_water_flag
		LDY		#$0B
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$09
		STA		_game_msg_idx					; game message display
		LDA		#$01
		STA		_game_msg_awaits_flag
		JSR		_screen_invalidate
		JMP		_goto_alert_start
locret_11FD73:
		RTS

; =============== S U B	R O U T	I N E =======================================
_disaster_destroy_tile:
		PUSHW	_tmp_map_pos					; additional backup for critical vars
		PUSHW	word_EC							; note, we call this from NMI
		PUSHB	_backup_var
		PUSHB	_mmc5_mul1_shadow
		PUSHB	_mmc5_mul0_shadow
		PUSHB	_tmp6CD
		STX		_tmp_map_pos._COL
		STY		_tmp_map_pos._ROW
		LDY		#_MAP_CORRUPT0
		JSR		_rand							; probability of fire is 64/256
		CMP		#$40
		BCS		loc_11FDA3
		LDY		#_MAP_FIRE
loc_11FDA3:
		TYA
		JSR		_corrupt_cur_map_tile			; common corruption function
		LDY		#$00
		BCC		loc_11FDAC						; if C=1 on return, then we tried to
		INY										; destroy water. we couldn't do it
loc_11FDAC:
		STY		_monster_in_water_flag			; also we need to know where water is
		POPB	_tmp6CD							; to make proper monster animation
		POPB	_mmc5_mul0_shadow
		POPB	_mmc5_mul1_shadow				; restore and return
		POPB	_backup_var
		POPW	word_EC
		POPW	_tmp_map_pos
		RTS

; =============== S U B	R O U T	I N E =======================================
_pollute_cur_map_tile:
		LDA		#_MAP_POLLUTION					; put pollution tile on map
		BNE		_corrupt_cur_map_tile

; =============== S U B	R O U T	I N E =======================================
_fire_cur_map_tile:
		LDA		#_MAP_FIRE						; put fire tile on map
		BNE		_corrupt_cur_map_tile

; =============== S U B	R O U T	I N E =======================================
_flood_cur_map_tile:
		LDA		#_MAP_FLOOD						; put flood tile on map

; =============== S U B	R O U T	I N E =======================================
_corrupt_cur_map_tile:
		STA		_tmp6CD							; tmp storage for desired tile idx
		JSR		_map_tile_read					; read currently selected tile
		TAY
		LDA		_map_tiles_flags_tbl,Y			; test if water
		AND		#_FL_WATER						; exit with C=1
		BNE		.corrupt_water
		LDA		_map_tiles_flags_tbl,Y
		AND		#_FL_DESTRUCTIBLE
		BEQ		.corrupt_non_destructible
		LDA		_tmp6CD							; reload tile we want to put
		CMP		#_MAP_FLOOD						; flood is a special case, will pollute
		BEQ		.do_corrupt						; even the raw grounds
		CPY		#_MAP_GROUND					; the rest of disasters can't harm grounds
		BNE		.do_corrupt						; TODO, why not? especially for pollution
.corrupt_non_destructible:
		CLC
		RTS
.corrupt_water:
		SEC
		RTS
.do_corrupt:
		TYA										; NOTE, very unoptimal when we already
		AND		#$F0							; already get it in _get_cur_building_tile
		CMP		#$80							; beforehead, but not save it there.
		BNE		.skip_build_tile_adjust			; now we must set building real tile position
		TYA
		AND		#$0F
		TAY
		LDA		_tmp_map_pos._COL
		SEC
		SBC		_internal_building_tile_deltaX_list,Y
		STA		_tmp_map_pos._COL
		LDA		_tmp_map_pos._ROW
		SEC
		SBC		_internal_building_tile_deltaY_list,Y
		STA		_tmp_map_pos._ROW
.skip_build_tile_adjust:
		JSR		_map_tile_read					; read it
		TAY
		LDA		_map_tiles_flags_tbl,Y			; if not a building, burn it raw
		AND		#_FL_BUILDING
		BEQ		.do_destroy
		CPY		#_MAP_AIR_PORT					; calculate size of the building
		BNE		loc_11FE2E
		LDX		#$0E							; airport 4x4 biggest building
		BNE		.destroy_building_loop
loc_11FE2E:
		CPY		#_MAP_COAL_POWER				; power stations and all presents are
		BCC		loc_11FE3A						; 3x3 large
		CPY		#_MAP_INTERNAL_BUILD0
		BCS		loc_11FE3A
		LDX		#$07
		BNE		.destroy_building_loop
loc_11FE3A:
		LDX		#$02							; the rest of buildings are 2x2
; NOTE, similar code can be found in map unpack routines but
; better optimized. here we have very briefly nailed unoptimal
; copy of the high-level language routines.
;
.destroy_building_loop:
		TXA										; now we will destroy all tiles one by one
		PHA										; X is a counter, count backwards
		PUSHB	_tmp_map_pos._COL				; start to destroy from farthest tile
		CLC										; finish the actual building tile
		ADC		_internal_building_tile_deltaX_list,X
		STA		_tmp_map_pos._COL
		PUSHB	_tmp_map_pos._ROW
		CLC
		ADC		_internal_building_tile_deltaY_list,X
		STA		_tmp_map_pos._ROW
		LDA		_tmp6CD
		JSR		_map_tile_write					; NOTE, every time we get here we recalc buf offset!
		POPB	_tmp_map_pos._ROW				; restore base tile pos to start from the
		POPB	_tmp_map_pos._COL				; beginning
		PLA
		TAX
		DEX
		BPL		.destroy_building_loop
		APUA_SE	_SE_IDX_DESTROY					; when done, set alert position and enable button
		JSR		_goto_alert_start				; also play sound for destroying building
		LDA		_tmp6CD
		JSR		_map_tile_write					; also don't need to check for water, because
		CLC										; building can't be on water anyway
		RTS
.do_destroy:
		LDA		_tmp6CD							; get the destroy tile in left hand
		CPY		#_MAP_WATER_ROAD_LR				; compare ground tile type in right hand
		BCC		loc_11FE83						; for bridges on water will
		CPY		#_MAP_FIRE						; put water instead of grounds!
		BCS		loc_11FE83
		LDA		#_MAP_WATER
loc_11FE83:
		JSR		_map_tile_write					; finally destroy our tile.
; BUG, little logic flaw here, we set C=1 if on water, we test for water before, but
; here may be all other tiles, not water. So we always signal here we in water and
; only the reason this has no any effect, the monster used this flag does tile
; test constantly on the same tile, when it destroy it the first time, it set
; flag, but at the next iteration clears it so we not see any problems
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_nuclear_meltdown_disaster_start:
		LDA		#$00
loc_11FE8A:
		STA		_tmp_map_pos._ROW
		LDA		#$00
loc_11FE8F:
		STA		_tmp_map_pos._COL
		JSR		_get_cur_building_tile
		LDA		_last_map_tile_read
		CMP		#_MAP_NUCLEAR_POWER
		BNE		loc_11FE9F
		JMP		loc_11FEB4
loc_11FE9F:
		INC		_tmp_map_pos._COL
		LDA		_tmp_map_pos._COL
		CMP		#$4C
		BNE		loc_11FE8F
		INC		_tmp_map_pos._ROW
		LDA		_tmp_map_pos._ROW
		CMP		#$4C
		BNE		loc_11FE8A
		RTS
loc_11FEB4:
		JSR		_fire_cur_map_tile
		LDX		#$00
loc_11FEB9:
		TXA
		PHA
		PUSHB	_tmp_map_pos._COL
		PUSHB	_tmp_map_pos._ROW
		LDA		#$1F
		JSR		_rand_clamp_A
		CLC
		ADC		_tmp_map_pos._COL
		SBC		#$0F
		CMP		#$4B
		BCS		loc_11FEEA
		STA		_tmp_map_pos._COL
		LDA		#$1F
		JSR		_rand_clamp_A
		CLC
		ADC		_tmp_map_pos._ROW
		SBC		#$0F
		CMP		#$4B
		BCS		loc_11FEEA
		STA		_tmp_map_pos._ROW
		JSR		_pollute_cur_map_tile
loc_11FEEA:
		POPB	_tmp_map_pos._ROW
		POPB	_tmp_map_pos._COL
		PLA
		TAX
		INX
		CPX		#$50
		BNE		loc_11FEB9
		LDY		#$0C
		FJSR	_city_history_write,SRAM,PRGA

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_goto_alert_start:
		LDA		_tmp_map_pos._COL
		STA		_goto_dst_tile._COL
		LDA		_tmp_map_pos._ROW
		STA		_goto_dst_tile._ROW
		LDA		#$01
		STA		_goto_alert_req
		RTS

; =============== S U B	R O U T	I N E =======================================
_disasters_trigger_for_scenario_mode:
		LDA		_cur_game_mode					; only for scenario mode
		CMP		#$02
		BNE		locret_11FF87
		LDA		_city._didaster_delay		; all disasters are stopped, exit now
		BEQ		locret_11FF87
		DEC		_city._didaster_delay		; decrease
		LDX		_scenario_idx					; X = _scenario_idx
		CPX		#$05
		BNE		loc_11FF34
		LDA		_city._didaster_delay		; scenario 5 - "RIO", flood occurs 15 times
		AND		#$0F							; until it stop
		BNE		locret_11FF87
		JMP		_flood_disaster_start
loc_11FF34:
		LDA		_city._didaster_delay		; one another scenarios we need here extra test
		BNE		locret_11FF87					; if delay still not 0, skip
		CPX		#$00
		BNE		loc_11FF40						; finally test for disaster start by scenario idx
		JMP		_earthquake_disaster_start		; scenario 0 - "CISCO"
loc_11FF40:
		DEX
		DEX
		DEX
		BNE		loc_11FF81
loc_11FF45:
		LDA		_obj_active_flags				; scenario 3 - "TOKYO", start monster
		AND		#$30							; wait for tornado disappear if any.
		BNE		loc_11FF45						; as a side effect, town freezed by this time
		LDA		#$1A
		JSR		_rand_clamp_A					; as against manually selected monster disaster
		ADC		#$2D							; scenario starting point is area near the center
		STA		_obj_pos._monster._COL			; of the map within square in 26 tiles
		STA		_tmp_map_pos._COL
		LDA		#$1A
		JSR		_rand_clamp_A
		STA		_obj_pos._monster._ROW
		STA		_tmp_map_pos._ROW
		LDA		#$00
		STA		_obj_dst_pos._monster._COL		; by default it will travel always to bottom-left corner
		LDA		#$4B
		STA		_obj_dst_pos._monster._ROW
		LDA		#$97
		JSR		_rand_clamp_A					; but rnd will change one of the coordinates, so
		LSR										; it will travel either to the leftmost of the bottom side
		BCS		loc_11FF7B						; in the direction of the sea
		STA		_obj_dst_pos._monster._COL
		BCC		loc_11FF7E
loc_11FF7B:
		STA		_obj_dst_pos._monster._ROW
loc_11FF7E:
		JMP		_monster_disaster_start_ex
loc_11FF81:
		DEX
		BNE		locret_11FF87
		JMP		_nuclear_meltdown_disaster_start; scenario 4 - "BOSTON"
locret_11FF87:
		RTS										; the rest of scenarios does not need a special disasters

; =============== S U B	R O U T	I N E =======================================
; called once at the end of the year when city RCI levels not zero from PRGA
; although this is not the same average calulation as for other values,
; it's also wrong and does not the average value and not the same as in
; original Micropolis code.
; here we have average calculated by formula:
; total_sum/(1.00+(count/256)), which mean we have the same equation as for
; when we need to calc some price without taxes.
; (price+taxes)/1.rate = price
; if price for example 100, and tax is 25%, then price without taxes would be
; 100/1.25 = 80. here we have the same equation for traffic, but out of full
; sum of traffic values we have remove the percent of areas with traffix summarized
; instead of integer value it treated as FRAC portion of 1.xx fixed point number.
;
_traf_lvl_avr_recalc:
		LDY		#$00
		STY		_tmp6CC							; sum of traffic dencity values for
		STY		_tmp6CC+1						; valuable only map areas
		STY		_tmpEA							; counter of traffic areas summed
		INY										; default is 1.0 for simplify following calculations
		STY		_tmpEB
		LDA		#$00
loc_11FF97:
		STA		_tmp_map_pos._ROW				; rows first
		LDA		#$00
loc_11FF9C:
		STA		_tmp_map_pos._COL				; columns
		JSR		_read_land_val_map_nibble		; read land value nibble here (4x4 blocks)
;		CMP		#$00							; REDUNDANT, flag already set here
		BEQ		loc_11FFBB
		JSR		_read_traffic_map_nibble		; read traffic dencity value for area (2x2 blocks)
		CLC
		ADC		_tmp6CC							; sum traffic map values
		STA		_tmp6CC
		BCC		loc_11FFB5
		INC		_tmp6CC+1
loc_11FFB5:
		INCW	_tmpEA							; also increas 2x2 tiles counter (1.00+0.01)
loc_11FFBB:
		LDA		_tmp_map_pos._COL				; for all blocks 2x2
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11FF9C
		LDA		_tmp_map_pos._ROW
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11FF97
; NOTE, traffic average here calculated different from other places
; this gives more reliable results somehow, but still not correct average
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		MOVW	_tmpE7,_tmp6CC					; divide integer value to fp8_8, result is integer
		JSR		_div_32to16						; traffic_value = [traffic_sum/(1.0+(tiles_count/256))]
		PUSHWI	$0266							; coeff = 2.4
		PUSHW	_tmpE6							; multiply integer result to fp8_8, result integer
		JSR		_mmc5_mul16to16					; result traffic_level = [traffic_value * 2.4]
		PLA										; or may be treated as fp8_8 with lower nibble of integer
		POPW	_city._traf_lvl_avr				; value as FRAC part and higher nibble as INT part
		PLA
		RTS

		SECTION_ENDS	B00, "PRGB CITY CORE ENGINE"

		BANK_END FB,$C000
