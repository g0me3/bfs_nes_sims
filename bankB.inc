
		ORG $A000

		BANK_START $FB

; =============== S U B	R O U T	I N E =======================================
_THREAD1_MAIN:
		FJSR	_RCI_meters_recalc,SRAM,PRGA
		JSR		_overall_stats_clear
		JSR		_overall_stats_recalc
		JSR		_interpolate_police_fire_maps
		FJSR	_power_grid_recalc,SRAM,PRGA
		JSR		_land_value_pollution_recalc
		JSR		_crime_level_recalc
		JSR		_population_dencity_recalc
		LDA		#$FF
		STA		_thread1_flags
		STA		_thread1_flags+1
		FJSR	_city_stats_recalc,SRAM,PRGA
		LDA		#$00
		STA		_city._cur_week

; =============== S U B	R O U T	I N E =======================================
_THREAD1_LOOP:
		JSR		_wait_for_hud_redraw
		INC		_city._cur_week
		LDA		_city._cur_week
		LSR
		BCC		loc_11E047
		FJSR	_RCI_meters_recalc,SRAM,PRGA
loc_11E047:
		JSR		_map_corruption_tiles_animate
		LDA		_frames_counter
		STA		_rand_seed
		ADDW8	_city._tax_year_rate,_city._tax_rate
		MOVWI	_power_stations_list_pos,$0000
		LDA		#$07
		STA		_thread0_flags+1
		JSR		_overall_stats_clear
		JSR		_overall_stats_recalc
		LDA		#$03
		STA		_thread0_flags+1
		JSR		_interpolate_police_fire_maps
		LDA		#$00
		STA		_thread0_flags+1
		JSR		_map_corruption_tiles_animate
		LDA		_city._cur_week
		AND		#$03
		BNE		loc_11E0E1
		LDA		#$03
		STA		_thread1_flags+1
		INC		_city._cur_month
		LDA		_city._cur_month
		CMP		#$0C
		BNE		loc_11E0DB
		LDA		#$00
		STA		_city._cur_month
		LDA		_city._cur_year+1
		CMP		#$27
		BNE		loc_11E0A9
		LDA		_city._cur_year
		CMP		#$0F
		BEQ		loc_11E0B1
loc_11E0A9:
		INCW	_city._cur_year
loc_11E0B1:
		FJSR	_graphs_10_year_data_update,SRAM,PRGA
		FJSR	_fiscal_year_over_calculations,SRAM,PRGA
		FJSR	_city_stats_recalc,SRAM,PRGA
		JSR		_map_corruption_tiles_animate
		FJSR	_year_to_str,SRAM,PRGA
		LDA		_tmpE6
		BNE		loc_11E0DB
		FJSR	_graphs_100_year_data_update,SRAM,PRGA
loc_11E0DB:
		JSR		_population_growth_map_recalc
		JSR		_map_corruption_tiles_animate
loc_11E0E1:
		JSR		_traffic_dencity_recalc
		JSR		_map_corruption_tiles_animate
		FJSR	_events_manager,SRAM,PRGA
_game_stop_loop:
		LDY		_city._game_speed
		CPY		#$03
		BNE		_game_core_calc
		JSR		_wait_for_hud_redraw
		JMP		_game_stop_loop
_game_core_calc:
		LDA		_game_core_recalcA_delay_list,Y
		AND		_city._cur_week
		BNE		loc_11E117
		LDA		#$02
		JSR		_critical_section_enter
		FJSR	_power_grid_recalc,SRAM,PRGA
		LDA		#$02
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate
loc_11E117:
		LDY		_city._game_speed
		LDA		_game_core_recalc_delay_mask_list,Y
		AND		_city._cur_week
		CMP		_game_core_recalcB_delay_list,Y
		BNE		loc_11E135
		LDA		#$A0
		JSR		_critical_section_enter
		JSR		_land_value_pollution_recalc
		LDA		#$A0
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate
loc_11E135:
		LDY		_city._game_speed
		LDA		_game_core_recalc_delay_mask_list,Y
		AND		_city._cur_week
		CMP		_game_core_recalcC_delay_list,Y
		BNE		loc_11E153
		LDA		#$40
		JSR		_critical_section_enter
		JSR		_crime_level_recalc
		LDA		#$40
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate
loc_11E153:
		LDY		_city._game_speed
		LDA		_game_core_recalc_delay_mask_list,Y
		AND		_city._cur_week
		CMP		_game_core_recalcD_delay_list,Y
		BNE		loc_11E171
		LDA		#$04
		JSR		_critical_section_enter
		JSR		_population_dencity_recalc
		LDA		#$04
		JSR		_critical_section_leave
		JSR		_map_corruption_tiles_animate
loc_11E171:
		JSR		_disasters_handlers_select
		JSR		_map_corruption_tiles_animate
		JMP		_THREAD1_LOOP

_game_core_recalcA_delay_list:
		.BYTE	$03,$00,$00,$00
_game_core_recalc_delay_mask_list:
		.BYTE	$0F,$07,$03,$00
_game_core_recalcB_delay_list:
		.BYTE	$00,$00,$00,$00
_game_core_recalcC_delay_list:
		.BYTE	$04,$02,$01,$00
_game_core_recalcD_delay_list:
		.BYTE	$08,$04,$02,$00

; =============== S U B	R O U T	I N E =======================================
_wait_for_hud_redraw:
		LDA		#$FF
		STA		_hud_redraw_req
		LDA		#$01
		STA		_game_core_state
loc_11E198:
		LDA		_hud_redraw_req
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
		STX		_tmp_map_pos._X
		STY		_tmp_map_pos._Y
		JSR		_get_building_map_tile			; all 76x76 map tiles are
		JSR		_collect_map_tiles_stats		; collected
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
		LDY		_last_airport_pos._X			; starting point is last checked
		INY										; airport on map (so it will be always
		INY										; bottom-rightmost airport)
		STY		_obj_pos._heli._X
		INY
		STY		_obj_next_pos._heli._X
		LDY		_last_airport_pos._Y
		INY
		INY
		STY		_obj_pos._heli._Y
		INY
		STY		_obj_next_pos._heli._Y
		LDA		#$03
		STA		_obj_spr_idx._heli
		BNE		loc_11E230
loc_11E1F2:
		LSR		_tmpE6
		BCS		loc_11E230						; test if airplane in flight as well
		LDY		_airports_count
		BEQ		.no_airports
		ORA		#$02
		STA		_obj_active_flags				; start to take off animation
		LDA		#$84
		STA		_obj_plane_anim_idx
		LDA		#$20
		STA		_obj_plane_anim_timer
		LDA		_last_airport_pos._X
		STA		_obj_next_pos._plane._X
		CLC
		ADC		#$03
		STA		_obj_pos._plane._X
		LDY		_last_airport_pos._Y
		STY		_obj_pos._plane._Y
		INY
		INY
		STY		_obj_next_pos._plane._Y
		LDA		#$06
		STA		_obj_spr_idx._plane
		BNE		loc_11E230
.no_airports:
		LDA		_obj_active_flags				; no airports - no flying objects
		AND		#$FC							; remove existing if airport destroyed
		STA		_obj_active_flags
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
		MADDW	_tmpE6,_RCI_counts._R,_RCI_counts._C
;
; BUG! seems copy-paste, second CLC seems wrong here!
;-
		LDA		_tmpE6
		CLC
		ADC		_RCI_counts._I
		STA		_total_RCI_counts
		LDA		_tmpE7
		CLC
		ADC		_RCI_counts._I+1
		STA		_total_RCI_counts+1
;-
;		MADDW	_total_RCI_counts,_tmpE6,_RCI_counts._I
;-
		LDA		_nuke_power_station_count
		CLC
		ADC		_coal_power_station_count
		STA		_tmp6CC
		LSADDW	_tmpE6,_city._level._C,_city._level._I,3
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
_collect_map_tiles_stats:
		LDY		_last_map_tile_read
		CPY		#$EB							; TODO: fix me, tile unused,
		BNE		loc_11E2EA						; there is no code, so may remove this
		NOP
loc_11E2EA:
		CPY		#$90
		BCC		loc_11E2FB
		LDA		_map_tiles_flags_tbl,Y
		BPL		loc_11E2FB
		INCW	_electrics_count
loc_11E2FB:
		CPY		#$90
		BNE		loc_11E308
		INCW	_map_buldozered_tiles_count
		RTS
loc_11E308:
		LDA		_last_map_tile_read
		CMP		#$E6
		BNE		loc_11E312
		JMP		_fire_tile_collect
loc_11E312:
		CMP		#$B5
		BNE		loc_11E319
		JMP		_flooded_tile_collect
loc_11E319:
		CMP		#$B6
		BNE		loc_11E320
		JMP		_polluted_tile_collect
loc_11E320:
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$20
		BEQ		loc_11E32B
		JMP		_road_tile_collect
loc_11E32B:
		LDA		_map_tiles_flags_tbl,Y
		AND		#$08
		BEQ		loc_11E33A
		LDA		_cur_map_tile_is_inside_a_building_flag
		BNE		loc_11E33A
		JMP		_collect_general_buildings_stats
loc_11E33A:
		LDA		_map_tiles_flags_tbl,Y
		AND		#$10
		BEQ		loc_11E344
		JMP		_rails_collect
loc_11E344:
		LDA		_map_tiles_flags_tbl,Y
		AND		#$04
		BEQ		loc_11E34E
		JMP		_non_destructable_collect
loc_11E34E:
		CPY		#$E7
		BNE		locret_11E355
		STY		_corruption_anim_flag
locret_11E355:
		RTS
_non_destructable_collect:
		LDA		_last_map_tile_read
		CMP		#$B2
		BCC		locret_11E387
		CMP		#$B4
		BCS		loc_11E36A
		INCW	_parks_counter
		RTS
loc_11E36A:
		CMP		#$BD
		BCC		locret_11E387
		CMP		#$CE
		BCS		loc_11E37B
		INCW	_map_shore_tiles_count
		RTS
loc_11E37B:
		CMP		#$E0
		BCS		locret_11E387
		INCW	_map_unbuldozered_tiles_count
locret_11E387:
		RTS
_fire_tile_collect:
		JSR		_rand
		CMP		#$F0
		BCC		loc_11E394
		LDA		#$B4
		JMP		_map_tile_write
loc_11E394:
		LDX		#$03
loc_11E396:
		JSR		_set_tmp_map_pos_to_surrounding_tile
		BCS		loc_11E3B5
		JSR		_get_building_map_tile
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y
		AND		#$40
		BEQ		loc_11E3B2
		JSR		_rand
		CMP		#$F4
		BCC		loc_11E3B2
		JSR		_fire_cur_map_tile
loc_11E3B2:
		JSR		_restore_tmp_map_pos_from_surrounding_tile
loc_11E3B5:
		DEX
		BPL		loc_11E396
		RTS
_flooded_tile_collect:
		LDA		_disaster_tiles_to_flood_count
		BEQ		loc_11E405
		DEC		_disaster_tiles_to_flood_count
		LDY		#$03
_do_flood:
		TYA
		PHA
		JSR		_rand
		AND		#$07
		BNE		loc_11E3FF
		PUSHB	_tmp_map_pos._X
		CLC
		ADC		byte_11FC26,Y
		STA		_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		CLC
		ADC		byte_11FC2A,Y
		STA		_tmp_map_pos._Y
		JSR		_tmp_map_pos_test_max
		BCS		loc_11E3F7
		JSR		_get_building_map_tile
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y
		AND		#$02
		BEQ		loc_11E3F7
		JSR		_flood_cur_map_tile
loc_11E3F7:
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
loc_11E3FF:
		PLA
		TAY
		DEY
		BPL		_do_flood
		RTS
loc_11E405:
		JSR		_rand
		AND		#$07
		BNE		locret_11E411
		LDA		#$90
		JSR		_map_tile_write
locret_11E411:
		RTS
_polluted_tile_collect:
		JSR		_rand
		BNE		locret_11E423
		JSR		_rand
		CMP		#$10
		BCS		locret_11E423
		LDA		#$90
		JSR		_map_tile_write
locret_11E423:
		RTS
_road_tile_collect:
		INCW	_roads_count
loc_11E42C:
		LDA		_map_tiles_flags_tbl,Y
		BMI		locret_11E456
		LDA		_road_fund_rate
		CMP		#$1E
		BCS		locret_11E456
		JSR		_rand
		BNE		locret_11E456
		JSR		_rand
		AND		#$3F
		CMP		_road_fund_rate
		BCC		locret_11E456
		LDX		#$BD
		LDA		_map_tiles_flags_tbl,Y
		AND		#$01
		BNE		loc_11E452
		LDX		#$B4
loc_11E452:
		TXA
		JSR		_map_tile_write
locret_11E456:
		RTS
		RTS
_rails_collect:
		INCW	_rails_count
		LDA		_obj_active_flags
		AND		#$08
		BNE		_no_train_start
		LDA		_last_map_tile_read
		CMP		#$B7
		BEQ		_no_train_start
		CMP		#$B8
		BEQ		_no_train_start
		CMP		#$BB
		BEQ		_no_train_start
		CMP		#$BC
		BEQ		_no_train_start
		LDA		_tmp_map_pos._X
		STA		_obj_pos._train._X
		STA		_obj_next_pos._train._X
		LDA		_tmp_map_pos._Y
		STA		_obj_pos._train._Y
		STA		_obj_next_pos._train._Y
		LDA		_tmp_map_pos._X
		CMP		#$4B
		BCS		_no_train_start
		LDA		_tmp_map_pos._Y
		CMP		#$4B
		BCS		_no_train_start
		LDX		#$02
		INC		_tmp_map_pos._X
loc_11E49F:
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$10
		BEQ		loc_11E4BA
		CPY		#$B7
		BEQ		loc_11E4BA
		CPY		#$B8
		BEQ		loc_11E4BA
		CPY		#$BB
		BEQ		loc_11E4BA
		CPY		#$BC
		BNE		loc_11E4C8
loc_11E4BA:
		CPX		#$04
		BEQ		_no_train_start
		LDX		#$04
		DEC		_tmp_map_pos._X
		INC		_tmp_map_pos._Y
		BPL		loc_11E49F
loc_11E4C8:
		STX		_obj_spr_idx._train
		STX		_obj_anim_max._train
		LDA		#$00
		STA		_obj_timer._train._X
		STA		_obj_timer._train._Y
		LDA		_obj_active_flags
		ORA		#$08
		STA		_obj_active_flags
_no_train_start:
		JMP		loc_11E42C
_collect_general_buildings_stats:
		JSR		_test_power_grid_map_bit
		STA		_tmp_last_read_map_tile_power_state
		BEQ		loc_11E4F1
		INCW	_power_total_consumption
loc_11E4F1:
		LDA		_last_map_tile_read
		CMP		#$50
		BCC		_collect_RCI_buildings_stats
		LDX		#OTHER_WIDX
		JSR		_inc_areas_stats
		JMP		_collect_other_buildings_stats
_collect_RCI_buildings_stats:
		CMP		#$20
		BCS		loc_11E51F
		CMP		#$19
		BNE		loc_11E510
		LDX		#OTHER_WIDX
		JSR		_inc_areas_stats
		JMP		_inc_hospital
loc_11E510:
		CMP		#$1A
		BNE		loc_11E51C
		LDX		#OTHER_WIDX
		JSR		_inc_areas_stats
		JMP		_inc_school
loc_11E51C:
		JMP		_R_areas_develop
loc_11E51F:
		CMP		#$40
		BCS		loc_11E526
		JMP		_C_areas_develop
loc_11E526:
		JMP		_I_areas_develop
_inc_hospital:
		INCW	_R_hospitals_count
		LDA		_hospitals_delta
		BPL		locret_11E558
		LDA		#$15
		JSR		_rand_clamp
		BNE		locret_11E558
		BEQ		loc_11E553
_inc_school:
		INCW	_R_schools_count
		LDA		_schools_delta
		BPL		locret_11E558
		LDA		#$15
		JSR		_rand_clamp
		BNE		locret_11E558
loc_11E553:
		LDA		#$00
		JMP		_map_tile_write
locret_11E558:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_areas_develop:
		LDX		#R_AREA_WIDX
		JSR		_inc_RCI_counts
		LDY		_last_map_tile_read
		LDA		_R_area_develop_states_list,Y
		STA		_cur_R_area_develop_state
		JSR		_adc_RCI_level
		LDX		#DEVELOPED_WIDX
		LDA		_last_map_tile_read
		BNE		loc_11E573
		INX										; TODO LDX #UNDEVELOPED_WIDX
		INX
loc_11E573:
		JSR		_inc_areas_stats
		JSR		_R_areas_twin_blocks_delete_test
		LDA		#$24
		JSR		_rand_clamp
		CMP		_cur_R_area_develop_state
		BCS		loc_11E590
		LDA		#$00
		JSR		sub_11E912
		BCS		loc_11E590
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F4A5
loc_11E590:
		JSR		_get_building_map_tile
		LDA		_last_map_tile_read
		BEQ		loc_11E59F
		JSR		_rand
		AND		#$03
		BNE		locret_11E5FA
loc_11E59F:
		JSR		_rand
		STA		_tmpE9
		LDA		_city._delta._R
		CLC
		ADC		#$80
		PHA
		PUSHB	#$96
		JSR		_mmc5_mul8to8
		PLA
		PLA
		SEC
		SBC		#$4B
		STA		_tmpE8
		JSR		sub_11F341
		CLC
		ADC		_tmpE8
		CLC
		ADC		#$80
		LDY		_tmp_last_read_map_tile_power_state
		BNE		loc_11E5C8
		LDA		#$48
loc_11E5C8:
		STA		_tmpE8
		CMP		#$7F
		BCC		loc_11E5E8
		SBC		#$67
		CMP		_tmpE9
		BCC		loc_11E5E8
		LDA		_cur_R_area_develop_state
		BNE		loc_11E5E2
		LDA		_tmpE9
		AND		#$03
		BNE		loc_11E5E2
		JMP		loc_11E5FB
loc_11E5E2:
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F365
loc_11E5E8:
		LDA		_tmpE8
		CMP		#$82
		BCS		locret_11E5FA
		ADC		#$67
		CMP		_tmpE9
		BCS		locret_11E5FA
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F4A5
locret_11E5FA:
		RTS
loc_11E5FB:
		JSR		_rand
		BPL		loc_11E612
		LDA		_hospitals_delta
		BMI		locret_11E623
		BEQ		locret_11E623
		LDA		#$19
		JSR		_map_tile_write
		LDA		#$00
		STA		_hospitals_delta
		RTS
loc_11E612:
		LDA		_schools_delta
		BMI		locret_11E623
		BEQ		locret_11E623
		LDA		#$1A
		JSR		_map_tile_write
		LDA		#$00
		STA		_schools_delta
locret_11E623:
		RTS

; =============== S U B	R O U T	I N E =======================================
_C_areas_develop:
		LDX		#C_AREA_WIDX
		JSR		_inc_RCI_counts
		LDA		_last_map_tile_read
		SEC
		SBC		#$20
		TAY
		LDA		_C_area_develop_states_list,Y
		STA		_cur_R_area_develop_state
		JSR		_adc_RCI_level
		LDX		#DEVELOPED_WIDX
		LDA		_last_map_tile_read
		CMP		#$20
		BNE		loc_11E644
		INX										; TODO: LDX UNDEVELOPED_WIDX
		INX
loc_11E644:
		JSR		_inc_areas_stats
		JSR		_C_areas_twin_blocks_delete_test
		LDA		#$06
		JSR		_rand_clamp
		CMP		_cur_R_area_develop_state
		BCS		loc_11E661
		LDA		#$01
		JSR		sub_11E912
		BCS		loc_11E661
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F551
loc_11E661:
		JSR		_rand
		AND		#$03
		BNE		locret_11E6A1
		JSR		_rand
		STA		_tmpE9
		LDA		#$00
		CLC
		ADC		_city._delta._C
		CLC
		ADC		#$80
		LDY		_tmp_last_read_map_tile_power_state
		BNE		loc_11E67D
		LDA		#$48
loc_11E67D:
		STA		_tmpE8
		CMP		#$7F
		BCC		loc_11E68F
		SBC		#$67
		CMP		_tmpE9
		BCC		loc_11E68F
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F40E
loc_11E68F:
		LDA		_tmpE8
		CMP		#$82
		BCS		locret_11E6A1
		ADC		#$67
		CMP		_tmpE9
		BCS		locret_11E6A1
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F551
locret_11E6A1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_C_areas_twin_blocks_delete_test:
		LDA		_last_map_tile_read
		CMP		#$35
		BCC		locret_11E6E6
		LDX		#$01
		JSR		_tmp_map_pos_backup
		LDA		_last_map_tile_read
		SEC
		SBC		#$35
		TAY
		LDA		_tmp_map_pos._X
		CLC
		ADC		_twin_area_ofsX_list,Y
		STA		_tmp_map_pos._X
		LDA		_tmp_map_pos._Y
		CLC
		ADC		_twin_area_ofsY_list,Y
		STA		_tmp_map_pos._Y
		TYA
		PHA
		JSR		_map_tile_read
		TAX
		PLA
		TAY
		TXA
		CMP		_C_twin_idx_test_list,Y
		BEQ		loc_11E6E1
		LDX		#$01
		JSR		_tmp_map_pos_restore
		LDA		#$34
		JMP		_map_tile_write
loc_11E6E1:
		LDX		#$01
		JMP		_tmp_map_pos_restore
locret_11E6E6:
		RTS

; =============== S U B	R O U T	I N E =======================================
_R_areas_twin_blocks_delete_test:
		LDA		_last_map_tile_read
		CMP		#$15
		BCC		locret_11E72B
		LDX		#$01
		JSR		_tmp_map_pos_backup
		LDA		_last_map_tile_read
		SEC
		SBC		#$15
		TAY
		LDA		_tmp_map_pos._X
		CLC
		ADC		_twin_area_ofsX_list,Y
		STA		_tmp_map_pos._X
		LDA		_tmp_map_pos._Y
		CLC
		ADC		_twin_area_ofsY_list,Y
		STA		_tmp_map_pos._Y
		TYA
		PHA
		JSR		_map_tile_read
		TAX
		PLA
		TAY
		TXA
		CMP		_R_twin_idx_test_list,Y
		BEQ		loc_11E726
		LDX		#$01
		JSR		_tmp_map_pos_restore
		LDA		#$14
		JMP		_map_tile_write
loc_11E726:
		LDX		#$01
		JMP		_tmp_map_pos_restore
locret_11E72B:
		RTS
_twin_area_ofsX_list:
		.BYTE	$00, $00,$02,$FE
_twin_area_ofsY_list:
		.BYTE	$02,$FE,$00, $00
_C_twin_idx_test_list:
		.BYTE	$36,$35,$38,$37
_R_twin_idx_test_list:
		.BYTE	$16,$15,$18,$17

; =============== S U B	R O U T	I N E =======================================
_I_areas_develop:
		LDX		#I_AREA_WIDX
		JSR		_inc_RCI_counts
		LDA		_last_map_tile_read
		SEC
		SBC		#$40
		TAY
		LDA		_I_area_develop_states_list,Y
		STA		_cur_R_area_develop_state
		JSR		_adc_RCI_level
		LDX		#DEVELOPED_WIDX
		LDA		_last_map_tile_read
		CMP		#$40
		BNE		loc_11E75C
		INX										; TODO: LDX UNDEVELOPED_WIDX
		INX
loc_11E75C:
		JSR		_inc_areas_stats
		LDA		#$06
		JSR		_rand_clamp
		CMP		_cur_R_area_develop_state
		BCS		loc_11E776
		LDA		#$02
		JSR		sub_11E912
		BCS		loc_11E776
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F5F5
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
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F48D
loc_11E7A5:
		LDA		_tmpE8
		CMP		#$81
		BCS		locret_11E7B7
		ADC		#$67
		CMP		_tmpE9
		BCS		locret_11E7B7
		JSR		_get_area_pollution_to_land_value_ratio
		JMP		sub_11F5F5
locret_11E7B7:
		RTS

; =============== S U B	R O U T	I N E =======================================
_collect_other_buildings_stats:
		CMP		#$60
		BNE		loc_11E7CC
		LDA		_coal_power_station_count
		BMI		loc_11E7C4
		INC		_coal_power_station_count
loc_11E7C4:
		FJSR	_power_stations_map_pos_list_insert,SRAM,PRGA
		RTS
loc_11E7CC:
		CMP		#$61
		BNE		loc_11E7E0
		LDA		_nuke_power_station_count
		BMI		loc_11E7D8
		INC		_nuke_power_station_count
loc_11E7D8:
		FJSR	_power_stations_map_pos_list_insert,SRAM,PRGA
		RTS
loc_11E7E0:
		CMP		#$50
		BNE		loc_11E803
		LDA		_police_stations_count
		CMP		#$63
		BCS		loc_11E7EE
		INC		_police_stations_count
loc_11E7EE:
		LDA		_police_fund_rate
		STA		_tmpE7
		JSR		_test_4x4_building_for_surrond_transports
		BCS		loc_11E7FA
		LSR		_tmpE7
loc_11E7FA:
		JSR		_read_police_radius_map_nibble
		CLC
		ADC		_tmpE7
		JMP		_write_police_radius_map_nibble
loc_11E803:
		CMP		#$75
		BNE		loc_11E80E
		LDA		#$0F
		STA		_tmpE7
		JMP		loc_11E7FA
loc_11E80E:
		CMP		#$51
		BNE		loc_11E831
		LDA		_fire_stations_count
		CMP		#$63
		BCS		loc_11E81C
		INC		_fire_stations_count
loc_11E81C:
		LDA		_fire_fund_rate
		STA		_tmpE7
		JSR		_test_4x4_building_for_surrond_transports
		BCS		loc_11E828
		LSR		_tmpE7
loc_11E828:
		JSR		_read_fire_radius_map_nibble
		CLC
		ADC		_tmpE7
		JMP		_write_fire_radius_map_nibble
loc_11E831:
		CMP		#$76
		BNE		loc_11E83C
		LDA		#$0F
		STA		_tmpE7
		JMP		loc_11E828
loc_11E83C:
		CMP		#$63
		BNE		loc_11E854
		LDA		_stadiums_count
		BMI		loc_11E848
		INC		_stadiums_count
loc_11E848:
		LDY		#$00
		LDA		_city._delta._R
		BMI		loc_11E850
		INY
loc_11E850:
		STY		_stadium_R_delta_positive_bonus_flag
		RTS
loc_11E854:
		CMP		#$64
		BNE		loc_11E86D
		LDA		_airports_count
		BMI		loc_11E860
		INC		_airports_count
loc_11E860:
		LDA		_tmp_map_pos._X
		STA		_last_airport_pos._X
		LDA		_tmp_map_pos._Y
		STA		_last_airport_pos._Y
		RTS
loc_11E86D:
		CMP		#$62
		BNE		loc_11E8AD
		LDA		_sea_ports_count
		BMI		loc_11E879
		INC		_sea_ports_count
loc_11E879:
		LDA		_obj_active_flags
		AND		#$04
		BNE		locret_11E8AC
		LDY		#>[_wram_map_buf]				; TODO: rearrange to macrocify
		STY		_tmpE7
		LDY		#<[_wram_map_buf]
		STY		_tmpE6
loc_11E888:
		LDA		(_tmpE6),Y
		CMP		#$FE
		BEQ		loc_11E8A0
		INC		_tmpE6
		BNE		loc_11E894
		INC		_tmpE7
loc_11E894:
		LDA		_tmpE6
		CMP		#$90
		LDA		_tmpE7
		SBC		#$16
		BNE		loc_11E888
		BEQ		locret_11E8AC
loc_11E8A0:
		LDA		_tmp_map_pos._X
		STA		_obj_pos._ship._X
		LDA		_tmp_map_pos._Y
		STA		_obj_pos._ship._Y
locret_11E8AC:
		RTS
loc_11E8AD:
		CMP		#$70
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
_test_power_grid_map_bit:
		PUSHB	_tmp_map_pos._Y
		PUSHB	#$4C
		JSR		_mmc5_mul8to8
		PLA
		CLC
		ADC		_tmp_map_pos._X
		STA		_tmpE6
		TAY
		PLA
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
sub_11E912:
		STA		_tmp6CC
		LDX		#$01
		JSR		_tmp_map_pos_backup
		LDA		#$00
		STA		_tmp6D2
		JSR		_test_4x4_building_for_surrond_transports
		BCC		loc_11E933
		JSR		sub_11E93A
		BCS		loc_11E933
		JSR		sub_11EA50
		LDX		#$01
		JSR		_tmp_map_pos_restore
		SEC
		RTS
loc_11E933:
		LDX		#$01
		JSR		_tmp_map_pos_restore
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11E93A:
		LDA		#$05
		STA		_tmp6CC+1
		LDY		#$00
loc_11E941:
		STY		_tmp6D0+1
		JSR		sub_11E96F
		BCS		loc_11E94F
		JSR		sub_11EA1C
		BCS		loc_11E965
		RTS
loc_11E94F:
		LDA		_tmp6D2
		BEQ		loc_11E96D
		DEC		_tmp6D2
		LDA		_tmp6D0+1
		CLC
		ADC		#$03
		STA		_tmp6D0+1
		LDA		#$05
		STA		_tmp6CC+1
loc_11E965:
		LDY		_tmp6D0+1
		INY
		CPY		#$1C
		BCC		loc_11E941
loc_11E96D:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11E96F:
		LDA		#$04
		JSR		_rand_clamp
		STA		_tmp6CE+1
		CMP		#$04
		BCC		loc_11E97E
loc_11E97B:
		JMP		loc_11E97B
loc_11E97E:
		STA		_tmp6CE
		CMP		_tmp6CC+1
		BEQ		loc_11E9B4
		LDA		_tmp6CE
		JSR		sub_11E9CD
		BCS		loc_11E9B4
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y
		AND		#$30
		BEQ		loc_11E9B4
		LDX		_tmp6CE
		JSR		_set_tmp_map_pos_to_surrounding_tile
		LDA		_tmp6CE
		CLC
		ADC		#$02
		AND		#$03
		STA		_tmp6CC+1
		LDA		_tmp6D0+1
		LSR
		BCC		loc_11E9B2
		JSR		sub_11E9EF
loc_11E9B2:
		CLC
		RTS
loc_11E9B4:
		LDA		_tmp6CE+1
		CMP		#$04
		BCC		loc_11E9BE
loc_11E9BB:
		JMP		loc_11E9BB
loc_11E9BE:
		INC		_tmp6CE
		LDA		_tmp6CE
		AND		#$03
		CMP		_tmp6CE+1
		BNE		loc_11E97E
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11E9CD:
		PHA
		LDX		#$02
		JSR		_tmp_map_pos_backup
		PLA
		TAX
		CPX		#$04
		BCS		loc_11E9E8
		JSR		_set_tmp_map_pos_to_surrounding_tile
		BCS		loc_11E9E8
		JSR		_get_building_map_tile
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
sub_11E9EF:
		JSR		_sram_on
		LDY		_tmp6D2
		INY
		LDA		_tmp_map_pos._X
		STA		_fire_radius_map_buf+$31,Y
		LDA		_tmp_map_pos._Y
		STA		_tmp_recalc_bufs+$1B,Y
		STY		_tmp6D2
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
sub_11EA08:
		LDY		_tmp6D2
		LDA		_fire_radius_map_buf+$31,Y
		STA		_tmp_map_pos._X
		LDA		_tmp_recalc_bufs+$1B,Y
		STA		_tmp_map_pos._Y
		DEY
		STY		_tmp6D2
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11EA1C:
		LDX		#$00
loc_11EA1E:
		STX		_tmp6D0
		TXA
		JSR		sub_11E9CD
		LDA		_last_map_tile_read
		LDX		_tmp6CC
		BNE		loc_11EA37
		CMP		#$20
		BCC		loc_11EA46
		CMP		#$64
		BCC		loc_11EA44
		BCS		loc_11EA46
loc_11EA37:
		DEX
		BNE		loc_11EA40
		CMP		#$50
		BCS		loc_11EA46
		BCC		loc_11EA44
loc_11EA40:
		CMP		#$20
		BCS		loc_11EA46
loc_11EA44:
		CLC
		RTS
loc_11EA46:
		LDX		_tmp6D0
		INX
		CPX		#$04
		BNE		loc_11EA1E
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11EA50:
		LDX		_tmp6D2
		BEQ		locret_11EA7C
loc_11EA55:
		STX		_tmp6D0
		JSR		sub_11EA08
		JSR		_tmp_map_pos_test_max
		BCS		locret_11EA7C
		JSR		_get_building_map_tile
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y
		AND		#$10
		BNE		loc_11EA76
		JSR		_read_traffic_dencity_map_nibble
		CLC
		ADC		#$02
		JSR		_write_traffic_dencity_map_nibble
loc_11EA76:
		LDX		_tmp6D0
		DEX
		BNE		loc_11EA55
locret_11EA7C:
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_pos_test_max:
		LDA		_tmp_map_pos._X
		CMP		#$4C
		BCS		locret_11EA89
		LDA		_tmp_map_pos._Y
		CMP		#$4C
locret_11EA89:
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_4x4_building_for_surrond_transports:
		LDX		#$02
		JSR		_tmp_map_pos_backup
		LDY		#$07
loc_11EA91:
		STY		_tmpE6
		LDA		_tmp_map_pos._X
		CLC
		ADC		_4x4_building_surrond_tiles_ofsX_list,Y
		STA		_tmp_map_pos._X
		CMP		#$4C
		BCS		loc_11EABA
		LDA		_tmp_map_pos._Y
		CLC
		ADC		_4x4_building_surrond_tiles_ofsY_list,Y
		STA		_tmp_map_pos._Y
		CMP		#$4C
		BCS		loc_11EABA
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$30
		BNE		loc_11EAC6
loc_11EABA:
		LDX		#$02
		JSR		_tmp_map_pos_restore
		LDY		_tmpE6
		DEY
		BPL		loc_11EA91
		CLC
		RTS
loc_11EAC6:
		SEC
		RTS
_4x4_building_surrond_tiles_ofsX_list:
		.BYTE	$FF,$FF,$00,$00,$01,$01,$02,$02
_4x4_building_surrond_tiles_ofsY_list:
		.BYTE	$00,$01,$FF,$02,$FF,$02,$00,$01

; =============== S U B	R O U T	I N E =======================================
_land_value_pollution_recalc:
		JSR		_clear_tmp_recalc_bufB5
		LDA		#$00
		STA		_tmp6CC
		STA		_tmp6CC+1
		STA		_tmp6CE
		STA		_tmp6CE+1
		STA		_tmp_area_pos._Y
loc_11EAEC:
		LDA		#$00
		STA		_tmp_area_pos._X
loc_11EAF1:
		JSR		_calc_map_area_pollution_level
		LDA		_tmpE6
		BEQ		loc_11EB70
		JSR		_tmp_area_pos_to_tmp_map_pos_calc
		LDA		_tmpE7
		JSR		_write_tmp_recalc_div4_mul13_map_nibble
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
		JSR		_tmp_area_pos_to_tmp_map_pos_calc
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
		JSR		_tmp_area_pos_to_tmp_map_pos_calc
		JSR		_read_crime_level_map_nibble
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
		JSR		_write_land_value_map_nibble
loc_11EB70:
		INC		_tmp_area_pos._X
		LDA		_tmp_area_pos._X
		CMP		#$13
		BEQ		loc_11EB7D
		JMP		loc_11EAF1
loc_11EB7D:
		INC		_tmp_area_pos._Y
		LDA		_tmp_area_pos._Y
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
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		MOVW	_tmpE7,_tmp6CC
		MOVW	_tmpEA,_tmp6CE
		JSR		_div_32to16
loc_11EBB7:
		MOVW	_city._var_5F8,_tmpE6
		LDA		#$00
		STA		_tmp6C4
		STA		_tmp6D0
		STA		_tmp6D0+1
		STA		_tmp6D2
		STA		_tmp6D3
		JSR		_interpolate_recalc_to_select_poll_popg_popd_crime_land_maps
		JSR		_interpolate_recalc_with_select_poll_popg_popd_crime_land_to_recalc_map
		LDA		#$00
loc_11EBDA:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11EBDF:
		STA		_tmp_map_pos._X
		JSR		_read_tmp_recalc_div4_mul13_map_nibble
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
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EBDF
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EBDA
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		MOVW	_tmpE7,_tmp6D2
		MOVW	_tmpEA,_tmp6D0
		JSR		_div_32to16
		BCC		loc_11EC3B
		MOVWI	_tmpE6,$0000
loc_11EC3B:
		MOVW	_city._var_5FC,_tmpE6
		LDA		#$A0
		JMP		_critical_section_leave

; =============== S U B	R O U T	I N E =======================================
_calc_map_area_pollution_level:
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE7
		LDA		_tmp_area_pos._Y
		ASL
		ASL
		STA		_tmp_map_pos._Y
loc_11EC58:
		LDA		_tmp_area_pos._X
		ASL
		ASL
		STA		_tmp_map_pos._X
loc_11EC60:
		JSR		_get_building_map_tile
		LDY		_last_map_tile_read
		LDA		_map_tiles_flags_tbl,Y
		AND		#$38
		BEQ		loc_11EC71
		LDA		#$01
		STA		_tmpE6
loc_11EC71:
		JSR		_get_map_tile_pollution_ratio
		TYA
		CLC
		ADC		_tmpE7
		BCC		loc_11EC7C
		LDA		#$FF
loc_11EC7C:
		STA		_tmpE7
		INC		_tmp_map_pos._X
		LDA		_tmp_map_pos._X
		AND		#$03
		BNE		loc_11EC60
		INC		_tmp_map_pos._Y
		LDA		_tmp_map_pos._Y
		AND		#$03
		BNE		loc_11EC58
		RTS

; =============== S U B	R O U T	I N E =======================================
_population_dencity_recalc:
		JSR		_clear_tmp_recalc_bufB5
		LDY		#$00
		STY		_tmp6CC
		STY		_tmp6CC+1
		STY		_tmp6CE
		STY		_tmp6CE+1
		STY		_tmp6D0
		STY		_tmp6D0+1
loc_11ECAA:
		STY		_tmp_map_pos._Y
		LDY		#$00
loc_11ECAF:
		STY		_tmp_map_pos._X
		JSR		_map_tile_read
		STA		_last_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$08
		BEQ		loc_11ECF4
		JSR		_get_RCI_area_develop_state
		STA		_tmpE7
		JSR		_read_tmp_recalc_div4_mul13_map_nibble
		CLC
		ADC		_tmpE7
		JSR		_write_tmp_recalc_div4_mul13_map_nibble
		ADDW8	_tmp6CC,_tmp_map_pos._X
		ADDW8	_tmp6CE,_tmp_map_pos._Y
		INCW	_tmp6D0
loc_11ECF4:
		LDY		_tmp_map_pos._X
		INY
		CPY		#$4C
		BNE		loc_11ECAF
		LDY		_tmp_map_pos._Y
		INY
		CPY		#$4C
		BNE		loc_11ECAA
		LDY		#$02
		STY		_tmp6C4
		JSR		_interpolate_recalc_to_select_poll_popg_popd_crime_land_maps
		JSR		_interpolate_recalc_with_select_poll_popg_popd_crime_land_to_recalc_map
		JSR		_interpolate_recalc_to_select_poll_popg_popd_crime_land_maps
		LDA		#$04
		JSR		_critical_section_leave			; BUG! this section set and leave outside of this proc
		LDA		_tmp6D0
		ORA		_tmp6D0+1
		BEQ		loc_11ED64
		LDA		_tmp6CC
		STA		_tmpE7
		LDA		_tmp6CC+1
		STA		_tmpE8
		LDA		_tmp6D0
		STA		_tmpEA
		LDA		_tmp6D0+1
		STA		_tmpEB
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		JSR		_div_32to16
		LDA		_tmpE7
		STA		_cur_population_dencity
		LDA		_tmp6CE
		STA		_tmpE7
		LDA		_tmp6CE+1
		STA		_tmpE8
		LDA		_tmp6D0
		STA		_tmpEA
		LDA		_tmp6D0+1
		STA		_tmpEB
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		JSR		_div_32to16
		LDA		_tmpE7
		STA		_cur_population_dencity+1
		RTS
loc_11ED64:
		LDA		#$26
		STA		_cur_population_dencity
		STA		_cur_population_dencity+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_get_RCI_area_develop_state:
		LDA		_last_map_tile_read
		CMP		#$20
		BCS		loc_11ED7C
		TAY
		LDA		_R_area_develop_states_list,Y
		LSR
		LSR
		LSR
		RTS
loc_11ED7C:
		CMP		#$40
		BCS		loc_11ED88
		SEC
		SBC		#$20
		TAY
		LDA		_C_area_develop_states_list,Y
		RTS
loc_11ED88:
		CMP		#$50
		BCS		loc_11ED94
		SEC
		SBC		#$40
		TAY
		LDA		_I_area_develop_states_list,Y
		RTS
loc_11ED94:
		LDA		#$00
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_police_fire_maps:
		LDA		#$00
		STA		_tmp6C4
		JSR		_interpolate_select_police_fire_to_recalc_map_div2
		JSR		_interpolate_recalc_to_select_police_fire_map_div4
		LDA		#$01
		STA		_tmp6C4
		JSR		_interpolate_select_police_fire_to_recalc_map_div2
		JMP		_interpolate_recalc_to_select_police_fire_map_div4

; =============== S U B	R O U T	I N E =======================================
_crime_level_recalc:
		LDA		#$00
		STA		_tmp6CC
		STA		_tmp6CC+1
		STA		_tmp6D0
		STA		_tmp6D0+1
loc_11EDBB:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11EDC0:
		STA		_tmp_map_pos._X
		JSR		_read_land_value_map_nibble
		BEQ		loc_11EDF1
		STA		_tmpE7
		INCW	_tmp6D0
		LDA		#$0F
		SEC
		SBC		_tmpE7
		STA		_tmpE7
		JSR		_read_pop_dencity_map_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		JSR		_read_police_radius_map_nibble
		ASL
		ASL
		STA		_tmpE6
		LDA		_tmpE7
		SEC
		SBC		_tmpE6
		BCS		loc_11EDF1
		LDA		#$00
loc_11EDF1:
		PHA
		JSR		_write_crime_level_map_nibble
		PLA
		CLC
		ADC		_tmp6CC
		STA		_tmp6CC
		BCC		loc_11EE02
		INC		_tmp6CC+1
loc_11EE02:
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EDC0
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EDBB
		LDA		_tmp6CC
		STA		_tmpE7
		LDA		_tmp6CC+1
		STA		_tmpE8
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		LDA		_tmp6D0
		STA		_tmpEA
		LDA		_tmp6D0+1
		STA		_tmpEB
		JSR		_div_32to16
		BCC		loc_11EE3B
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE7
loc_11EE3B:
		LDA		_tmpE6
		STA		_city._crime_level
		LDA		_tmpE7
		STA		_city._crime_level+1
		LDA		#$40
		JMP		_critical_section_leave

; =============== S U B	R O U T	I N E =======================================
_interpolate_recalc_to_select_poll_popg_popd_crime_land_maps:
		LDA		#$00
loc_11EE4C:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11EE51:
		STA		_tmp_map_pos._X
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_tmp_recalc_div4_mul13_surrond_map_nibbles_sum_0
		JSR		_read_tmp_recalc_div4_mul13_map_nibble
		CLC
		ADC		_tmpE7
		LSR
		LSR
		CMP		#$0F
		BCC		loc_11EE69
		LDA		#$0F
loc_11EE69:
		JSR		_write_select_poll_popg_popd_crime_land_recalc_maps_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EE51
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EE4C
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_tmp_recalc_div4_mul13_surrond_map_nibbles_sum_0:
		LDA		_tmp_map_pos._X
		CMP		#$04
		BCC		loc_11EE99
		SBC		#$04
		STA		_tmp_map_pos._X
		JSR		_add_tmp_recalc_div4_mul13_map_nibble_0
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		STA		_tmp_map_pos._X
loc_11EE99:
		LDA		_tmp_map_pos._X
		CMP		#$48
		BCS		loc_11EEB1
		ADC		#$04
		STA		_tmp_map_pos._X
		JSR		_add_tmp_recalc_div4_mul13_map_nibble_0
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$04
		STA		_tmp_map_pos._X
loc_11EEB1:
		LDA		_tmp_map_pos._Y
		CMP		#$04
		BCC		loc_11EEC9
		SBC		#$04
		STA		_tmp_map_pos._Y
		JSR		_add_tmp_recalc_div4_mul13_map_nibble_0
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		STA		_tmp_map_pos._Y
loc_11EEC9:
		LDA		_tmp_map_pos._Y
		CMP		#$48
		BCS		locret_11EEE1
		ADC		#$04
		STA		_tmp_map_pos._Y
		JSR		_add_tmp_recalc_div4_mul13_map_nibble_0
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$04
		STA		_tmp_map_pos._Y
locret_11EEE1:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_tmp_recalc_div4_mul13_map_nibble_0:
		JSR		_read_tmp_recalc_div4_mul13_map_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_recalc_with_select_poll_popg_popd_crime_land_to_recalc_map:
		LDA		#$00
loc_11EEED:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11EEF2:
		STA		_tmp_map_pos._X
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_tmp_recalc_div4_mul13_surrond_map_nibbles_sum
		JSR		_read_select_pol_popg_popd_crime_land_recalc_maps_nibble
		CLC
		ADC		_tmpE7
		LSR
		LSR
		CMP		#$0F
		BCC		loc_11EF0A
		LDA		#$0F
loc_11EF0A:
		JSR		_write_tmp_recalc_div4_mul13_map_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EEF2
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11EEED
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_tmp_recalc_div4_mul13_surrond_map_nibbles_sum:
		LDA		_tmp_map_pos._X
		CMP		#$04
		BCC		loc_11EF3A
		SBC		#$04
		STA		_tmp_map_pos._X
		JSR		_add_tmp_recalc_div4_mul13_map_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		STA		_tmp_map_pos._X
loc_11EF3A:
		LDA		_tmp_map_pos._X
		CMP		#$48
		BCS		loc_11EF52
		ADC		#$04
		STA		_tmp_map_pos._X
		JSR		_add_tmp_recalc_div4_mul13_map_nibble
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$04
		STA		_tmp_map_pos._X
loc_11EF52:
		LDA		_tmp_map_pos._Y
		CMP		#$04
		BCC		loc_11EF6A
		SBC		#$04
		STA		_tmp_map_pos._Y
		JSR		_add_tmp_recalc_div4_mul13_map_nibble
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		STA		_tmp_map_pos._Y
loc_11EF6A:
		LDA		_tmp_map_pos._Y
		CMP		#$48
		BCS		locret_11EF82
		ADC		#$04
		STA		_tmp_map_pos._Y
		JSR		_add_tmp_recalc_div4_mul13_map_nibble
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$04
		STA		_tmp_map_pos._Y
locret_11EF82:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_tmp_recalc_div4_mul13_map_nibble:
		JSR		_read_tmp_recalc_div4_mul13_map_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_recalc_to_select_police_fire_map_div4:
		LDA		#$00
loc_11EF8E:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11EF93:
		STA		_tmp_map_pos._X
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_tmp_recalc_div8_mulA_surrond_map_nibbles_sum
		JSR		_read_tmp_recalc_div8_mulA_nibble
		CLC
		ADC		_tmpE7
		LSR
		LSR
		CMP		#$0F
		BCC		loc_11EFAB
		LDA		#$0F
loc_11EFAB:
		JSR		_write_select_police_fire_recalc_maps_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11EF93
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11EF8E
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_tmp_recalc_div8_mulA_surrond_map_nibbles_sum:
		LDA		_tmp_map_pos._X
		CMP		#$08
		BCC		loc_11EFDB
		SBC		#$08
		STA		_tmp_map_pos._X
		JSR		_add_tmp_recalc_div8_mulA_map_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$08
		STA		_tmp_map_pos._X
loc_11EFDB:
		LDA		_tmp_map_pos._X
		CMP		#$44
		BCS		loc_11EFF3
		ADC		#$08
		STA		_tmp_map_pos._X
		JSR		_add_tmp_recalc_div8_mulA_map_nibble
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$08
		STA		_tmp_map_pos._X
loc_11EFF3:
		LDA		_tmp_map_pos._Y
		CMP		#$08
		BCC		loc_11F00B
		SBC		#$08
		STA		_tmp_map_pos._Y
		JSR		_add_tmp_recalc_div8_mulA_map_nibble
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$08
		STA		_tmp_map_pos._Y
loc_11F00B:
		LDA		_tmp_map_pos._Y
		CMP		#$48
		BCS		locret_11F023
		ADC		#$08
		STA		_tmp_map_pos._Y
		JSR		_add_tmp_recalc_div8_mulA_map_nibble
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$08
		STA		_tmp_map_pos._Y
locret_11F023:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_tmp_recalc_div8_mulA_map_nibble:
		JSR		_read_tmp_recalc_div8_mulA_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_interpolate_select_police_fire_to_recalc_map_div2:
		LDA		#$00
loc_11F02F:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11F034:
		STA		_tmp_map_pos._X
		LDA		#$00
		STA		_tmpE7
		JSR		_calc_select_police_fire_recalc_surrond_map_nibbles_sum
		JSR		_read_select_police_fire_recalc_map_nibble
		CLC
		ADC		_tmpE7
		LSR
		CMP		#$0F
		BCC		loc_11F04B
		LDA		#$0F
loc_11F04B:
		JSR		_write_tmp_recalc_div8_mulA_map_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11F034
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$08
		CMP		#$4C
		BCC		loc_11F02F
		RTS

; =============== S U B	R O U T	I N E =======================================
_calc_select_police_fire_recalc_surrond_map_nibbles_sum:
		LDA		_tmp_map_pos._X
		CMP		#$08
		BCC		loc_11F07B
		SBC		#$08
		STA		_tmp_map_pos._X
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$08
		STA		_tmp_map_pos._X
loc_11F07B:
		LDA		_tmp_map_pos._X
		CMP		#$44
		BCS		loc_11F093
		ADC		#$08
		STA		_tmp_map_pos._X
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$08
		STA		_tmp_map_pos._X
loc_11F093:
		LDA		_tmp_map_pos._Y
		CMP		#$08
		BCC		loc_11F0AB
		SBC		#$08
		STA		_tmp_map_pos._Y
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$08
		STA		_tmp_map_pos._Y
loc_11F0AB:
		LDA		_tmp_map_pos._Y
		CMP		#$44
		BCS		locret_11F0C3
		ADC		#$08
		STA		_tmp_map_pos._Y
		JSR		_add_select_police_fire_recalc
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$08
		STA		_tmp_map_pos._Y
locret_11F0C3:
		RTS

; =============== S U B	R O U T	I N E =======================================
_add_select_police_fire_recalc:
		JSR		_read_select_police_fire_recalc_map_nibble
		CLC
		ADC		_tmpE7
		STA		_tmpE7
		RTS

; =============== S U B	R O U T	I N E =======================================
_clear_tmp_recalc_bufB5:
		JSR		_sram_on
		LDY		#$00
		TYA
loc_11F0D3:
		STA		_tmp_recalc_bufs,Y
		INY
		CPY		#$B5
		BNE		loc_11F0D3
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
; return
; 2 - if developed I area (41-4F)
; 3 - if coal power or sea/air port
; F - if flooded ground
; 0 - other
_get_map_tile_pollution_ratio:
		LDY		#$02
		LDA		_last_map_tile_read
		CMP		#$50
		BCS		loc_11F0F0
		CMP		#$40
		BCC		loc_11F0F0
		BNE		locret_11F10A
		LDY		#$00
		RTS
loc_11F0F0:
		LDY		#$03
		CMP		#$E6
		BEQ		locret_11F10A
		CMP		#$62
		BEQ		locret_11F10A
		CMP		#$64
		BEQ		locret_11F10A
		CMP		#$60
		BEQ		locret_11F10A
		LDY		#$0F
		CMP		#$B5
		BEQ		locret_11F10A
		LDY		#$00
locret_11F10A:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F10B:
		LDA		_cur_population_dencity
		LSR
		LSR
		SEC
		SBC		_tmp_area_pos._X
		BCS		loc_11F11B
		EOR		#$FF
		CLC
		ADC		#$01
loc_11F11B:
		STA		_tmpE6
		LDA		_cur_population_dencity+1
		LSR
		LSR
		SEC
		SBC		_tmp_area_pos._Y
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
_get_building_map_tile:
		JSR		_map_tile_read
		STA		_last_map_tile_read
		LDY		#$00
		STY		_cur_map_tile_is_inside_a_building_flag
		AND		#$F0
		CMP		#$80
		BNE		locret_11F177
		LDX		#$00
		JSR		_tmp_map_pos_backup
		LDA		_last_map_tile_read
		AND		#$0F
		TAY
		LDX		_internal_building_tile_deltaX_list,Y
		BEQ		loc_11F15E
loc_11F158:
		DEC		_tmp_map_pos._X
		DEX
		BNE		loc_11F158
loc_11F15E:
		LDX		_internal_building_tile_deltaY_list,Y
		BEQ		loc_11F169
loc_11F163:
		DEC		_tmp_map_pos._Y
		DEX
		BNE		loc_11F163
loc_11F169:
		JSR		_map_tile_read
		STA		_last_map_tile_read
		INC		_cur_map_tile_is_inside_a_building_flag
		LDX		#$00
		JSR		_tmp_map_pos_restore
locret_11F177:
		RTS
_internal_building_tile_deltaX_list:
		.BYTE	$00,$01,$01,$00,$01,$02,$02,$02,$00,$01,$02,$03,$03,$03,$03
_internal_building_tile_deltaY_list:
		.BYTE	$01,$01,$00,$02,$02,$02,$01,$00,$03,$03,$03,$03,$02,$01,$00

; =============== S U B	R O U T	I N E =======================================
_map_tile_read:
		PUSHB	_tmp_map_pos._Y
		PUSHB	#$4C
		JSR		_mmc5_mul8to8
		PLA
		CLC
		ADC		#<[_wram_map_buf]
		STA		word_EC
		PLA
		ADC		#>[_wram_map_buf]
		STA		word_EC+1
		LDY		_tmp_map_pos._X
		LDA		(word_EC),Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_tile_write:
		PHA
		PUSHB	_tmp_map_pos._Y
		PUSHB	#$4C
		JSR		_mmc5_mul8to8
		PLA
		CLC
		ADC		#<[_wram_map_buf]
		STA		word_EC
		PLA
		ADC		#>[_wram_map_buf]
		STA		word_EC+1
		LDY		_tmp_map_pos._X
		JSR		_sram_on
		PLA
		STA		(word_EC),Y
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_restore_tmp_map_pos_from_surrounding_tile:
		CPX		#$00
		BEQ		_tmp_map_inc_pos_Y
		CPX		#$01
		BEQ		_tmp_map_dec_pos_X
		CPX		#$02
		BEQ		_tmp_map_dec_pos_Y
		CPX		#$03
		BEQ		_tmp_map_inc_pos_X
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_set_tmp_map_pos_to_surrounding_tile:
		CPX		#$00
		BEQ		_tmp_map_dec_pos_Y
		CPX		#$01
		BEQ		_tmp_map_inc_pos_X
		CPX		#$02
		BEQ		_tmp_map_inc_pos_Y
		CPX		#$03
		BEQ		_tmp_map_dec_pos_X
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_dec_pos_Y:
		LDA		_tmp_map_pos._Y
		BEQ		_tmp_map_cant_change_pos
		DEC		_tmp_map_pos._Y
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_inc_pos_X:
		LDA		_tmp_map_pos._X
		CMP		#$4B
		BEQ		_tmp_map_cant_change_pos
		INC		_tmp_map_pos._X
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_inc_pos_Y:
		LDA		_tmp_map_pos._Y
		CMP		#$4B
		BEQ		_tmp_map_cant_change_pos
		INC		_tmp_map_pos._Y
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_dec_pos_X:
		LDA		_tmp_map_pos._X
		BEQ		_tmp_map_cant_change_pos
		DEC		_tmp_map_pos._X
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_cant_change_pos:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_pos_backup:
		LDA		_tmp_map_pos._Y
		STA		_tmp_map_pos_backup_buf_Y,X
		LDA		_tmp_map_pos._X
		STA		_tmp_map_pos_backup_buf_X,X
		RTS

; =============== S U B	R O U T	I N E =======================================
_tmp_map_pos_restore:
		LDA		_tmp_map_pos_backup_buf_Y,X
		STA		_tmp_map_pos._Y
		LDA		_tmp_map_pos_backup_buf_X,X
		STA		_tmp_map_pos._X
		RTS

; =============== S U B	R O U T	I N E =======================================
_overall_stats_clear:
		LDX		#$00
		TXA
loc_11F242:
		STA		_total_RCI_counts,X
		INX
		CPX		#$2C
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
		JSR		_sram_on
		LDY		#$00
		TYA
loc_11F26A:
		STA		_fire_radius_map_buf,Y
		STA		_police_radius_map_buf,Y
		INY
		CPY		#$32
		BNE		loc_11F26A
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
_population_growth_map_recalc:
		LDA		#$00
loc_11F27A:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11F27F:
		STA		_tmp_map_pos._X
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
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11F27F
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$04
		CMP		#$4C
		BCC		loc_11F27A
		RTS

; =============== S U B	R O U T	I N E =======================================
_traffic_dencity_recalc:
		LDA		#$00
loc_11F2AD:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11F2B2:
		STA		_tmp_map_pos._X
		JSR		_read_traffic_dencity_map_nibble
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
		JSR		_write_traffic_dencity_map_nibble
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11F2B2
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11F2AD
		RTS

; =============== S U B	R O U T	I N E =======================================
_critical_section_leave:
		PHA
		EOR		#$FF
		AND		_thread0_flags
		STA		_thread0_flags
		PLA
		ORA		_thread1_flags
		STA		_thread1_flags
		RTS

; =============== S U B	R O U T	I N E =======================================
_critical_section_enter:
		ORA		_thread0_flags
		STA		_thread0_flags
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_corruption_tiles_animate:
		LDA		_corruption_anim_flag
		BEQ		loc_11F33A
		JSR		_sram_on
		LDY		#>[_wram_map_buf]				; TODO: rearrange to macrocify
		STY		_tmpE7
		LDY		#<[_wram_map_buf]
		STY		_tmpE6
		STY		_corruption_anim_flag
loc_11F310:
		LDA		(_tmpE6),Y
		SEC
		SBC		#$E7
		BCC		loc_11F324
		CMP		#$04
		BCS		loc_11F324
		TAX
		LDA		_corrupt_ground_tiles_list,X
		STA		(_tmpE6),Y
		STA		_corruption_anim_flag
loc_11F324:
		INC		_tmpE6
		BNE		loc_11F32A
		INC		_tmpE7
loc_11F32A:
		LDA		_tmpE6
		CMP		#<[_power_grid_map_buf]
		LDA		_tmpE7
		SBC		#>[_power_grid_map_buf]
		BNE		loc_11F310
		JSR		_wait_for_hud_redraw
		JMP		_map_corruption_tiles_animate
loc_11F33A:
		JMP		_sram_off
_corrupt_ground_tiles_list:
		.BYTE	$E8,$E9,$EA,$B4

; =============== S U B	R O U T	I N E =======================================
sub_11F341:
		JSR		_read_pollution_map_nibble
		STA		_tmpE7
		JSR		_read_land_value_map_nibble
		SEC
		SBC		_tmpE7
		BCS		loc_11F350
		LDA		#$00
loc_11F350:
		PHA
		PUSHB	#$07
		JSR		_mmc5_mul8to8
		PLA
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
sub_11F365:
		JSR		_read_pollution_map_nibble
		CMP		#$08
		BCS		locret_11F3BE
		JSR		_map_tile_read
		CMP		#$05
		BCS		loc_11F393
		LDA		_cur_R_area_develop_state
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
		JSR		sub_11F61A
		LDA		#$01
		JMP		sub_11F647
loc_11F393:
		CMP		#$15
		BCS		locret_11F40D
		CMP		#$14
		BNE		loc_11F3F5
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		STA		_tmp_map_pos._Y
		JSR		_map_tile_read
		CMP		#$14
		BNE		loc_11F3BF
		LDA		#$16
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		LDA		#$15
		JSR		_map_tile_write
locret_11F3BE:
		RTS
loc_11F3BF:
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		STA		_tmp_map_pos._X
		JSR		_map_tile_read
		CMP		#$14
		BNE		loc_11F3EB
		LDA		#$18
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		LDA		#$17
		JMP		_map_tile_write
loc_11F3EB:
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		RTS
loc_11F3F5:
		LDA		_cur_R_area_develop_state
		CMP		#$28
		BCS		locret_11F40D
		LDA		_cur_R_area_develop_state
		LSR
		LSR
		LSR
		SEC
		SBC		#$01
		JSR		sub_11F61A
		LDA		#$01
		JMP		sub_11F647
locret_11F40D:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F40E:
		JSR		_map_tile_read
		CMP		#$35
		BCS		locret_11F48C
		CMP		#$34
		BNE		loc_11F474
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		STA		_tmp_map_pos._Y
		JSR		_map_tile_read
		CMP		#$34
		BNE		loc_11F43D
		LDA		#$36
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		LDA		#$35
		JSR		_map_tile_write
		RTS
loc_11F43D:
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		STA		_tmp_map_pos._X
		JSR		_map_tile_read
		CMP		#$34
		BNE		loc_11F46A
		LDA		#$38
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		LDA		#$37
		JSR		_map_tile_write
		RTS
loc_11F46A:
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		RTS
loc_11F474:
		JSR		_read_land_value_map_nibble
		LSR
		CMP		_cur_R_area_develop_state
		BCC		locret_11F48C
		LDA		_cur_R_area_develop_state
		CMP		#$05
		BCS		locret_11F48C
		JSR		sub_11F628
		LDA		#$01
		JSR		sub_11F647
locret_11F48C:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F48D:
		LDA		#$02
		JSR		_rand_clamp
		STA		byte_6C3
		LDA		_cur_R_area_develop_state
		CMP		#$04
		BCS		locret_11F4A4
		JSR		sub_11F639
		LDA		#$01
		JSR		sub_11F647
locret_11F4A4:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F4A5:
		JSR		_map_tile_read
		CMP		#$15
		BNE		loc_11F4C9
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		STA		_tmp_map_pos._Y
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		RTS
loc_11F4C9:
		CMP		#$16
		BNE		loc_11F4EA
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		STA		_tmp_map_pos._Y
		RTS
loc_11F4EA:
		CMP		#$17
		BNE		loc_11F50B
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		STA		_tmp_map_pos._X
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		RTS
loc_11F50B:
		CMP		#$18
		BNE		loc_11F52C
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		LDA		#$14
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		STA		_tmp_map_pos._X
		RTS
loc_11F52C:
		LDX		byte_6C3
		BEQ		locret_11F550
		LDA		_cur_R_area_develop_state
		BEQ		locret_11F550
		CMP		#$18
		BCC		loc_11F548
		SEC
		SBC		#$18
		LSR
		LSR
		LSR
		JSR		sub_11F61A
		LDA		#$FF
		JMP		sub_11F647
loc_11F548:
		LSR
		LSR
		SEC
		SBC		#$01
		JMP		_map_tile_write
locret_11F550:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F551:
		JSR		_map_tile_read
		CMP		#$35
		BNE		loc_11F575
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		STA		_tmp_map_pos._Y
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		RTS
loc_11F575:
		CMP		#$36
		BNE		loc_11F596
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		SEC
		SBC		#$02
		STA		_tmp_map_pos._Y
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		STA		_tmp_map_pos._Y
		RTS
loc_11F596:
		CMP		#$37
		BNE		loc_11F5B7
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		STA		_tmp_map_pos._X
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		RTS
loc_11F5B7:
		CMP		#$38
		BNE		loc_11F5D8
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		SEC
		SBC		#$02
		STA		_tmp_map_pos._X
		LDA		#$34
		JSR		_map_tile_write
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		STA		_tmp_map_pos._X
		RTS
loc_11F5D8:
		LDA		_cur_R_area_develop_state
		SEC
		SBC		#$02
		BCC		loc_11F5E8
		JSR		sub_11F628
		LDA		#$FF
		JMP		sub_11F647
loc_11F5E8:
		LDA		_cur_R_area_develop_state
		CMP		#$01
		BNE		locret_11F5F4
		LDA		#$20
		JSR		_map_tile_write
locret_11F5F4:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F5F5:
		LDA		#$02
		JSR		_rand_clamp
		STA		byte_6C3
		LDA		_cur_R_area_develop_state
		SEC
		SBC		#$02
		BCC		loc_11F60D
		JSR		sub_11F639
		LDA		#$FF
		JMP		sub_11F647
loc_11F60D:
		LDA		_cur_R_area_develop_state
		CMP		#$01
		BNE		locret_11F619
		LDA		#$40
		JSR		_map_tile_write
locret_11F619:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_11F61A:
		STA		_tmpE6
		LDA		byte_6C3
		ASL
		ASL
		ADC		_tmpE6
		ADC		#$05
		JMP		_map_tile_write

; =============== S U B	R O U T	I N E =======================================
sub_11F628:
		STA		_tmpE6
		LDA		byte_6C3
		ASL
		ASL
		ADC		byte_6C3
		ADC		_tmpE6
		ADC		#$21
		JMP		_map_tile_write

; =============== S U B	R O U T	I N E =======================================
sub_11F639:
		STA		_tmpE6
		LDA		byte_6C3
		ASL
		ASL
		ADC		_tmpE6
		ADC		#$41
		JMP		_map_tile_write

; =============== S U B	R O U T	I N E =======================================
sub_11F647:
		STA		_tmpE7
		JSR		_read_pop_growth_map_nibble
		CLC
		ADC		_tmpE7
		JMP		_write_pop_growth_map_nibble

; =============== S U B	R O U T	I N E =======================================
; a = POLLUTION - LAND_VALUE
;  if a < 2    ratio 0
;  if a 2 to 4 ratio 1
;  if a 5 to 8 ratio 2
;  if a > 8    ratio 3
;
_get_area_pollution_to_land_value_ratio:
		JSR		_read_pollution_map_nibble
		STA		_tmpE7
		JSR		_read_land_value_map_nibble
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
		STX		byte_6C3
		RTS

; =============== S U B	R O U T	I N E =======================================
_read_select_pol_popg_popd_crime_land_recalc_maps_nibble:
		LDY		_tmp6C4
		BEQ		_read_pollution_map_nibble
		DEY
		BEQ		_read_pop_growth_map_nibble
		DEY
		BEQ		_read_pop_dencity_map_nibble
		DEY
		BEQ		_read_crime_level_map_nibble
		DEY
		BEQ		_read_land_value_map_nibble
		BNE		_read_tmp_recalc_div4_mul13_map_nibble

; =============== S U B	R O U T	I N E =======================================
_read_pollution_map_nibble:
		LDXY	_pollution_map_buf
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_pop_growth_map_nibble:
		LDXY	_pop_growth_map_buf
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_pop_dencity_map_nibble:
		LDXY	_pop_dencity_map_buf
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_crime_level_map_nibble:
		LDXY	_crime_level_map_buf
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_land_value_map_nibble:
		LDXY	_land_value_map_buf
		BNE		_read_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_tmp_recalc_div4_mul13_map_nibble:
		LDXY	_tmp_recalc_bufs

; =============== S U B	R O U T	I N E =======================================
_read_buf_pos_div4_mul13_4bit_nibble:
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._Y
		LSR
		LSR
		MULAI	#$13
		LDA		_tmp_map_pos._X
		LSR
		LSR
		JMP		_read_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_read_traffic_dencity_map_nibble:
		LDXY	_traffic_density_map_buf
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._Y
		LSR
		MULAI	#$26
		LDA		_tmp_map_pos._X
		LSR
		JMP		_read_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_read_select_police_fire_recalc_map_nibble:
		LDY		_tmp6C4
		BEQ		_read_police_radius_map_nibble
		DEY
		BEQ		_read_fire_radius_map_nibble

; =============== S U B	R O U T	I N E =======================================
_read_tmp_recalc_div8_mulA_nibble:
		LDXY	_tmp_recalc_bufs
		BNE		_read_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_police_radius_map_nibble:
		LDXY	_police_radius_map_buf
		BNE		_read_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_read_fire_radius_map_nibble:
		LDXY	_fire_radius_map_buf

; =============== S U B	R O U T	I N E =======================================
_read_buf_pos_div8_mulA_4bit_nibble:
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._Y
		LSR
		LSR
		LSR
		MULAI	#$0A
		LDA		_tmp_map_pos._X
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
		DEC		_tmp_area_pos._Y
		BMI		loc_11F761
		JSR		_map_area_non_destructable_tiles_count
loc_11F761:
		INC		_tmp_area_pos._Y
		INC		_tmp_area_pos._Y
		LDA		_tmp_area_pos._Y
		CMP		#$13
		BCS		loc_11F771
		JSR		_map_area_non_destructable_tiles_count
loc_11F771:
		DEC		_tmp_area_pos._Y
		DEC		_tmp_area_pos._X
		BMI		loc_11F77C
		JSR		_map_area_non_destructable_tiles_count
loc_11F77C:
		INC		_tmp_area_pos._X
		INC		_tmp_area_pos._X
		LDA		_tmp_area_pos._X
		CMP		#$13
		BCS		loc_11F78C
		JSR		_map_area_non_destructable_tiles_count
loc_11F78C:
		DEC		_tmp_area_pos._X
		ASL		_tmp6D3
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_area_non_destructable_tiles_count:
		LDA		_tmp_area_pos._Y
		ASL
		ASL
		STA		_tmp_map_pos._Y
loc_11F79B:
		LDA		_tmp_area_pos._X
		ASL
		ASL
		STA		_tmp_map_pos._X
loc_11F7A3:
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$04
		BEQ		loc_11F7B1
		INC		_tmp6D3
loc_11F7B1:
		INC		_tmp_map_pos._X
		LDA		_tmp_map_pos._X
		AND		#$03
		BNE		loc_11F7A3
		INC		_tmp_map_pos._Y
		LDA		_tmp_map_pos._Y
		AND		#$03
		BNE		loc_11F79B
		RTS

; =============== S U B	R O U T	I N E =======================================
_write_select_poll_popg_popd_crime_land_recalc_maps_nibble:
		LDY		_tmp6C4
		BEQ		_write_pollution_map_nibble
		DEY
		BEQ		_write_pop_growth_map_nibble
		DEY
		BEQ		_write_pop_dencity_map_nibble
		DEY
		BEQ		_write_crime_level_map_nibble
		DEY
		BEQ		_write_land_value_map_nibble
		BNE		_write_tmp_recalc_div4_mul13_map_nibble

; =============== S U B	R O U T	I N E =======================================
_write_pollution_map_nibble:
		LDXY	_pollution_map_buf
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_pop_growth_map_nibble:
		LDXY	_pop_growth_map_buf
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_pop_dencity_map_nibble:
		LDXY	_pop_dencity_map_buf
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_crime_level_map_nibble:
		LDXY	_crime_level_map_buf
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_land_value_map_nibble:
		LDXY	_land_value_map_buf
		BNE		_write_buf_pos_div4_mul13_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_tmp_recalc_div4_mul13_map_nibble:
		LDXY	_tmp_recalc_bufs

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
		LDA		_tmp_map_pos._Y
		LSR
		LSR
;		STA		_mmc5_mul1_shadow
;		STA		_MMC5_MUL1
;		LDA		#$13
;		STA		_mmc5_mul1_shadow				; BUG: copy-paste, same shadow var, may be critical
;		STA		_MMC5_MUL0
		MULAI	#$13							; FIX: uses correct macro
		LDA		_tmp_map_pos._X
		LSR
		LSR
		JMP		_write_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_write_traffic_dencity_map_nibble:
		LDXY	_traffic_density_map_buf
		CMP		#$10
		BCC		loc_11F82C
		LDA		#$0F
loc_11F82C:
		STA		byte_E5
		TYA
		PHA
		TXA
		PHA
		LDA		_tmp_map_pos._Y
		LSR
		MULAI	#$26
		LDA		_tmp_map_pos._X
		LSR
		JMP		_write_buf_pos_4bit_nibble_ex

; =============== S U B	R O U T	I N E =======================================
_write_select_police_fire_recalc_maps_nibble:
		LDY		_tmp6C4
		BEQ		_write_police_radius_map_nibble
		DEY
		BEQ		_write_fire_radius_map_nibble

; =============== S U B	R O U T	I N E =======================================
_write_tmp_recalc_div8_mulA_map_nibble:
		LDXY	_tmp_recalc_bufs
		BNE		_write_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_police_radius_map_nibble:
		LDXY	_police_radius_map_buf
		BNE		_write_buf_pos_div8_mulA_4bit_nibble

; =============== S U B	R O U T	I N E =======================================
_write_fire_radius_map_nibble:
		LDXY	_fire_radius_map_buf

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
		LDA		_tmp_map_pos._Y
		LSR
		LSR
		LSR
		MULAI	#$0A
		LDA		_tmp_map_pos._X
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
		JSR		_sram_on
		STA		(word_EE),Y
		JMP		_sram_off

; =============== S U B	R O U T	I N E =======================================
;_unused_11:
;		LDA		_cur_population_dencity
;		SEC
;		SBC		_tmp_map_pos._X
;		BCS		loc_11F8D5
;		EOR		#$FF
;		CLC
;		ADC		#$01
;loc_11F8D5:
;		LSR
;		LSR
;		LSR
;		STA		_tmpE6
;		LDA		_cur_population_dencity+1
;		SEC
;		SBC		_tmp_map_pos._Y
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
_tmp_area_pos_to_tmp_map_pos_calc:
		LDA		_tmp_area_pos._X
		ASL
		ASL
		STA		_tmp_map_pos._X
		LDA		_tmp_area_pos._Y
		ASL
		ASL
		STA		_tmp_map_pos._Y
		RTS

; =============== S U B	R O U T	I N E =======================================
_heavy_traffic_report_test:
		PUSHB	_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		PUSHB	word_EE
		PUSHB	word_EE+1
		PUSHB	_tmpE6
		PUSHB	_mmc5_mul1_shadow
		PUSHB	_mmc5_mul0_shadow
		STX		_tmp_map_pos._X
		STY		_tmp_map_pos._Y
		JSR		_read_traffic_dencity_map_nibble
		CMP		#$0A
		BCC		loc_11F93E
		LDA		#$18
		STA		_scroll_msg_idx
		LDY		#$00
		STY		_scroll_msg_start_pos
		DEY
		STY		_scroll_msg_active_flag
		LDA		#$02
		STA		_apu_se_flags+1
loc_11F93E:
		POPB	_mmc5_mul0_shadow
		POPB	_mmc5_mul1_shadow
		POPB	_tmpE6
		POPB	word_EE+1
		POPB	word_EE
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_select_map_tile_if_powered_up:
		PUSHB	_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		LDX		_tmp6D0+1
		JSR		_set_tmp_map_pos_to_surrounding_tile
		BCS		loc_11F977
		JSR		_test_map_tile_if_powered_up
		BCS		loc_11F977
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
		CLC
		RTS
loc_11F977:
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_test_map_tile_if_powered_up:
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		BPL		loc_11F9DA
		TYA
		CMP		#$60
		BEQ		loc_11F9DA
		CMP		#$61
		BEQ		loc_11F9DA
		LDA		_tmp_map_pos._Y
		MULAI	#$4C
		LDA		_MMC5_MUL0
		CLC
		ADC		_tmp_map_pos._X
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
		LDA		_inv_bitmasks,X
		AND		(_tmpE6),Y
		BNE		loc_11F9DA
		CLC
		RTS
loc_11F9DA:
		SEC
		RTS
_map_tiles_flags_tbl:
		.BYTE	$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA
		.BYTE	$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$00,$00,$00,$00,$00
		.BYTE	$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA
		.BYTE	$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$00,$00,$00,$00,$00,$00,$00
		.BYTE	$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$CA,$00,$00,$00,$00,$00,$00,$00
		.BYTE	$CA,$CA,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.BYTE	$CA,$CA,$CA,$CA,$CA,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
		.BYTE	$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$CE,$00
		.BYTE	$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$C0,$00
		.BYTE	$02,$62,$62,$62,$62,$62,$62,$62,$62,$62,$62,$62,$52,$52,$52,$52
		.BYTE	$52,$52,$52,$52,$52,$52,$52,$C2,$C2,$C2,$C2,$C2,$C2,$C2,$C2,$C2
		.BYTE	$C2,$C2,$46,$06,$02,$00,$00,$52,$52,$E2,$E2,$D2,$D2,$05,$06,$06
		.BYTE	$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$06,$46,$46
		.BYTE	$46,$46,$46,$46,$46,$46,$46,$46,$46,$46,$46,$46,$46,$46,$46,$46
		.BYTE	$05,$05,$15,$15,$85,$85,$00,$00,$00,$00,$00,$05,$05,$05,$05,$05
		.BYTE	$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$05,$00,$05,$00
_R_area_develop_states_list:
		.BYTE	$00,$04,$06,$08,$0C,$10,$18,$20,$28,$10,$18,$20,$28,$10,$18,$20
		.BYTE	$28,$10,$18,$20,$28,$28,$28,$28,$28
_C_area_develop_states_list:
		.BYTE	$00,$01,$02,$03,$04,$05,$01,$02,$03,$04,$05,$01,$02,$03,$04,$05
		.BYTE	$01,$02,$03,$04,$05,$05,$05,$05,$05
_I_area_develop_states_list:
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
_fire_disaster_start:
		LDY		#$28

; =============== S U B	R O U T	I N E =======================================
_fire_disaster_start_common:
		TYA
		PHA
		LDA		#$4C
		JSR		_rand_clamp
		STA		_tmp_map_pos._X
		LDA		#$4C
		JSR		_rand_clamp
		STA		_tmp_map_pos._Y
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$48
		CMP		#$48
		BNE		loc_11FB73
		JSR		_fire_cur_map_tile
		JSR		_goto_alert_start
loc_11FB73:
		PLA
		TAY
		DEY
		BNE		_fire_disaster_start_common
		LDA		_disasters_flags
		AND		#$FE
		STA		_disasters_flags
		LDY		#$06
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$20
		STA		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
		JMP		_wait_for_hud_redraw

; =============== S U B	R O U T	I N E =======================================
_flood_disaster_start:
		LDA		#$00
		STA		_tmp6CC
		LDY		#$FA
loc_11FB9A:
		TYA
		PHA
		LDA		#$4C
		JSR		_rand_clamp
		STA		_tmp_map_pos._X
		LDA		#$4C
		JSR		_rand_clamp
		STA		_tmp_map_pos._Y
		JSR		_map_tile_read
		CMP		#$BE
		BCC		loc_11FC04
		CMP		#$CE
		BCS		loc_11FC04
		LDY		#$03
loc_11FBB9:
		TYA
		PHA
		PUSHB	_tmp_map_pos._X
		CLC
		ADC		byte_11FC26,Y
		STA		_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		CLC
		ADC		byte_11FC2A,Y
		STA		_tmp_map_pos._Y
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
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
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
		JSR		_wait_for_hud_redraw
		JMP		_goto_alert_start
byte_11FC26:
		.BYTE	$00,$01,$00,$FF
byte_11FC2A:
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
		STA		byte_674
loc_11FC3E:
		LDA		#$4C
		JSR		_rand_clamp
		STA		_tmp_map_pos._X
		LDA		#$4C
		JSR		_rand_clamp
		STA		_tmp_map_pos._Y
		JSR		_get_building_map_tile
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$08
		BEQ		loc_11FC69
		LDA		_tmp6CE
		AND		#$03
		BEQ		loc_11FC64
		LDA		#$E7
		BNE		loc_11FC66
loc_11FC64:
		LDA		#$E6
loc_11FC66:
		JSR		_corrupt_cur_map_tile
loc_11FC69:
		DEC		_tmp6CE
		BNE		loc_11FC3E
loc_11FC6E:
		JSR		_wait_for_hud_redraw
		LDA		byte_674
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
		STA		_airplane_crash_req
		LDA		_disasters_flags
		AND		#$FB
		STA		_disasters_flags
		RTS

; =============== S U B	R O U T	I N E =======================================
_tornado_disaster_start:
		LDA		_obj_active_flags
		AND		#$30
		BNE		locret_11FCFC
		LDA		_obj_active_flags
		ORA		#$20
		STA		_obj_active_flags
		LDA		#$4C
		STA		_MMC5_CHR_BANKSA+1
		STA		_mmc5_chr_banks_shadow+1
		LDA		#$4C
		JSR		_rand_clamp
		STA		_obj_pos._tornado._X
		STA		_tmp_map_pos._X
		LDA		#$4C
		JSR		_rand_clamp
		STA		_obj_pos._tornado._Y
		STA		_tmp_map_pos._Y
		LDA		#$4C
		JSR		_rand_clamp
		STA		_obj_next_pos._tornado._X
		LDA		#$4C
		JSR		_rand_clamp
		STA		_obj_next_pos._tornado._Y
		LDA		_disasters_flags
		AND		#$F7
		STA		_disasters_flags
		LDY		#$09
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$23
		STA		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
		JSR		_wait_for_hud_redraw
		LDA		#$30
		STA		_apu_se_flags+1
		JMP		_goto_alert_start
locret_11FCFC:
		RTS

; =============== S U B	R O U T	I N E =======================================
_monster_disaster_start:
		LDA		_obj_active_flags
		AND		#$30
		BNE		locret_11FD73
		LDY		#$00
		LDA		#$4C
		JSR		_rand_clamp
		STA		_obj_pos._monster._X
		STA		_tmp_map_pos._X
		CMP		#$26
		BCS		loc_11FD17
		LDY		#$4B
loc_11FD17:
		STY		_obj_next_pos._monster._X
		LDY		#$00
		LDA		#$4C
		JSR		_rand_clamp
		STA		_obj_pos._monster._Y
		STA		_tmp_map_pos._Y
		CMP		#$26
		BCS		loc_11FD2D
		LDY		#$4B
loc_11FD2D:
		STY		_obj_next_pos._monster._X
		LDA		#$97
		JSR		_rand_clamp
		LSR
		BCS		loc_11FD3D
		STA		_obj_next_pos._monster._X
		BCC		_force_monster_disaster
loc_11FD3D:
		STA		_obj_next_pos._monster._Y

; =============== S U B	R O U T	I N E =======================================
_force_monster_disaster:
		LDA		_obj_active_flags
		ORA		#$10
		STA		_obj_active_flags
		LDA		#$35
		STA		_MMC5_CHR_BANKSA+1
		STA		_mmc5_chr_banks_shadow+1
		LDA		_disasters_flags
		AND		#$DF
		STA		_disasters_flags
		LDA		#$01
		STA		byte_6CA
		LDY		#$0B
		FJSR	_city_history_write,SRAM,PRGA
		LDA		#$09
		STA		_game_msg_idx
		LDA		#$01
		STA		_game_msg_awaits_flag
		JSR		_wait_for_hud_redraw
		JMP		_goto_alert_start
locret_11FD73:
		RTS

; =============== S U B	R O U T	I N E =======================================
_fire_or_destroy_map_tile_ex:
		PUSHB	_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		PUSHB	word_EC
		PUSHB	word_EC+1
		PUSHB	_backup_var
		PUSHB	_mmc5_mul1_shadow
		PUSHB	_mmc5_mul0_shadow
		PUSHB	_tmp6CC+1
		STX		_tmp_map_pos._X
		STY		_tmp_map_pos._Y
		LDY		#$E7
		JSR		_rand
		CMP		#$40
		BCS		loc_11FDA3
		LDY		#$E6
loc_11FDA3:
		TYA
		JSR		_corrupt_cur_map_tile
		LDY		#$00
		BCC		loc_11FDAC
		INY
loc_11FDAC:
		STY		byte_6CA
		POPB	_tmp6CC+1
		POPB	_mmc5_mul0_shadow
		POPB	_mmc5_mul1_shadow
		POPB	_backup_var
		POPB	word_EC+1
		POPB	word_EC
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
		RTS

; =============== S U B	R O U T	I N E =======================================
_pollute_cur_map_tile:
		LDA		#$B6
		BNE		_corrupt_cur_map_tile

; =============== S U B	R O U T	I N E =======================================
_fire_cur_map_tile:
		LDA		#$E6
		BNE		_corrupt_cur_map_tile

; =============== S U B	R O U T	I N E =======================================
_flood_cur_map_tile:
		LDA		#$B5

; =============== S U B	R O U T	I N E =======================================
_corrupt_cur_map_tile:
		STA		_tmp6CC+1
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$01
		BNE		loc_11FDFA
		LDA		_map_tiles_flags_tbl,Y
		AND		#$40
		BEQ		loc_11FDF8
		LDA		_tmp6CC+1
		CMP		#$B5
		BEQ		loc_11FDFC
		CPY		#$90
		BNE		loc_11FDFC
loc_11FDF8:
		CLC
		RTS
loc_11FDFA:
		SEC
		RTS
loc_11FDFC:
		TYA
		AND		#$F0
		CMP		#$80
		BNE		loc_11FE1B
		TYA
		AND		#$0F
		TAY
		LDA		_tmp_map_pos._X
		SEC
		SBC		_internal_building_tile_deltaX_list,Y
		STA		_tmp_map_pos._X
		LDA		_tmp_map_pos._Y
		SEC
		SBC		_internal_building_tile_deltaY_list,Y
		STA		_tmp_map_pos._Y
loc_11FE1B:
		JSR		_map_tile_read
		TAY
		LDA		_map_tiles_flags_tbl,Y
		AND		#$08
		BEQ		loc_11FE76
		CPY		#$64
		BNE		loc_11FE2E
		LDX		#$0E
		BNE		loc_11FE3C
loc_11FE2E:
		CPY		#$60
		BCC		loc_11FE3A
		CPY		#$80
		BCS		loc_11FE3A
		LDX		#$07
		BNE		loc_11FE3C
loc_11FE3A:
		LDX		#$02
loc_11FE3C:
		TXA
		PHA
		PUSHB	_tmp_map_pos._X
		CLC
		ADC		_internal_building_tile_deltaX_list,X
		STA		_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		CLC
		ADC		_internal_building_tile_deltaY_list,X
		STA		_tmp_map_pos._Y
		LDA		_tmp6CC+1
		JSR		_map_tile_write
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
		PLA
		TAX
		DEX
		BPL		loc_11FE3C
		LDA		#$04
		STA		_apu_se_flags+1
		JSR		_goto_alert_start
		LDA		_tmp6CC+1
		JSR		_map_tile_write
		CLC
		RTS
loc_11FE76:
		LDA		_tmp6CC+1
		CPY		#$E0
		BCC		loc_11FE83
		CPY		#$E6
		BCS		loc_11FE83
		LDA		#$BD
loc_11FE83:
		JSR		_map_tile_write
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_nuclear_meltdown_disaster_start:
		LDA		#$00
loc_11FE8A:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11FE8F:
		STA		_tmp_map_pos._X
		JSR		_get_building_map_tile
		LDA		_last_map_tile_read
		CMP		#$61
		BNE		loc_11FE9F
		JMP		loc_11FEB4
loc_11FE9F:
		INC		_tmp_map_pos._X
		LDA		_tmp_map_pos._X
		CMP		#$4C
		BNE		loc_11FE8F
		INC		_tmp_map_pos._Y
		LDA		_tmp_map_pos._Y
		CMP		#$4C
		BNE		loc_11FE8A
		RTS
loc_11FEB4:
		JSR		_fire_cur_map_tile
		LDX		#$00
loc_11FEB9:
		TXA
		PHA
		PUSHB	_tmp_map_pos._X
		PUSHB	_tmp_map_pos._Y
		LDA		#$1F
		JSR		_rand_clamp
		CLC
		ADC		_tmp_map_pos._X
		SBC		#$0F
		CMP		#$4B
		BCS		loc_11FEEA
		STA		_tmp_map_pos._X
		LDA		#$1F
		JSR		_rand_clamp
		CLC
		ADC		_tmp_map_pos._Y
		SBC		#$0F
		CMP		#$4B
		BCS		loc_11FEEA
		STA		_tmp_map_pos._Y
		JSR		_pollute_cur_map_tile
loc_11FEEA:
		POPB	_tmp_map_pos._Y
		POPB	_tmp_map_pos._X
		PLA
		TAX
		INX
		CPX		#$50
		BNE		loc_11FEB9
		LDY		#$0C
		FJSR	_city_history_write,SRAM,PRGA

; =============== S U B	R O U T	I N E =======================================
; !FALLTHROUGH!
_goto_alert_start:
		LDA		_tmp_map_pos._X
		STA		_goto_dest_tile._X
		LDA		_tmp_map_pos._Y
		STA		_goto_dest_tile._Y
		LDA		#$01
		STA		_goto_alert_req
		RTS

; =============== S U B	R O U T	I N E =======================================
_disasters_trigger_for_scenario_mode:
		LDA		_cur_game_mode
		CMP		#$02
		BNE		locret_11FF87
		LDA		_scenario_disaster_delay
		BEQ		locret_11FF87
		DEC		_scenario_disaster_delay
		LDX		_scenario_idx
		CPX		#$05
		BNE		loc_11FF34
		LDA		_scenario_disaster_delay
		AND		#$0F
		BNE		locret_11FF87
		JMP		_flood_disaster_start
loc_11FF34:
		LDA		_scenario_disaster_delay
		BNE		locret_11FF87
		CPX		#$00
		BNE		loc_11FF40
		JMP		_earthquake_disaster_start
loc_11FF40:
		DEX
		DEX
		DEX
		BNE		loc_11FF81
loc_11FF45:
		LDA		_obj_active_flags
		AND		#$30
		BNE		loc_11FF45
		LDA		#$1A
		JSR		_rand_clamp
		ADC		#$2D
		STA		_obj_pos._monster._X
		STA		_tmp_map_pos._X
		LDA		#$1A
		JSR		_rand_clamp
		STA		_obj_pos._monster._Y
		STA		_tmp_map_pos._Y
		LDA		#$00
		STA		_obj_next_pos._monster._X
		LDA		#$4B
		STA		_obj_next_pos._monster._Y
		LDA		#$97
		JSR		_rand_clamp
		LSR
		BCS		loc_11FF7B
		STA		_obj_next_pos._monster._X
		BCC		loc_11FF7E
loc_11FF7B:
		STA		_obj_next_pos._monster._Y
loc_11FF7E:
		JMP		_force_monster_disaster
loc_11FF81:
		DEX
		BNE		locret_11FF87
		JMP		_nuclear_meltdown_disaster_start
locret_11FF87:
		RTS

; =============== S U B	R O U T	I N E =======================================
_traffic_jam_recalc:
		LDY		#$00
		STY		_tmp6CC
		STY		_tmp6CC+1
		STY		_tmpEA
		INY
		STY		_tmpEB
		LDA		#$00
loc_11FF97:
		STA		_tmp_map_pos._Y
		LDA		#$00
loc_11FF9C:
		STA		_tmp_map_pos._X
		JSR		_read_land_value_map_nibble
		CMP		#$00
		BEQ		loc_11FFBB
		JSR		_read_traffic_dencity_map_nibble
		CLC
		ADC		_tmp6CC
		STA		_tmp6CC
		BCC		loc_11FFB5
		INC		_tmp6CC+1
loc_11FFB5:
		INC		_tmpEA
		BNE		loc_11FFBB
		INC		_tmpEB
loc_11FFBB:
		LDA		_tmp_map_pos._X
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11FF9C
		LDA		_tmp_map_pos._Y
		CLC
		ADC		#$02
		CMP		#$4C
		BCC		loc_11FF97
		LDA		#$00
		STA		_tmpE6
		STA		_tmpE9
		LDA		_tmp6CC
		STA		_tmpE7
		LDA		_tmp6CC+1
		STA		_tmpE8
		JSR		_div_32to16
		PUSHWI	$0266
		PUSHW	_tmpE6
		JSR		_mmc5_mul16to16
		PLA
		POPB	_traffic_level_value
		POPB	_traffic_level_value+1
		PLA
		RTS

		BANK_END FB,$C000
