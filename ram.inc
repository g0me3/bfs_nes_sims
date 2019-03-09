
		INCLUDE		"../macroses.inc"

;--------------------R-A-M----------------------

		ORG $0000

_ptr0:
		.WORD	0
_tmp0	EQU		_ptr0
_tmp1	EQU		_tmp0+1

_ptr2:
		.WORD	0
_tmp2:	EQU		_ptr2
_tmp3:	EQU		_tmp2+1

_ptr4:
		.WORD	0
_tmp4	EQU		_ptr4
_tmp5	EQU		_tmp4+1

_ptr6:
		.WORD	0
_tmp6	EQU		_ptr6
_tmp7	EQU		_tmp6+1

; variables never used in the original code, hovewer, tmp9 is
; reinitialized with 0 at the reset for unknown reason
;
; will use these regs for my own if i need some spare vars
; to be sure they doesn't interfer with anything.
;
_tmp8:
		.BYTE	0
_tmp9:
		.BYTE	0

_ppu_ctrl_shadow:
		.BYTE	0
_ppu_mask_shadow:
		.BYTE	0
_spr_buf_pos:
		.BYTE	0
_ppu_queue_pos_tail:
		.BYTE	0
_ppu_queue_pos_head:
		.BYTE	0
_extnt_queue_pos_tail:
		.BYTE	0
_extnt_queue_pos_head:
		.BYTE	0
_nmi_busy_flag:
		.BYTE	0
_nmi_flag:
		.BYTE	0
;byte_13_unused:
;		.BYTE	0
_irq_sub_idx:
		.BYTE	0
_ptr15:
		.WORD	0
_tmp17:
		.BYTE	0
_ptr18:
		.WORD	0
_mmc5_operation_in_progress_flag:
		.BYTE	0
_frames_counter:
		.BYTE	0
_frames_div8_counter:
		.BYTE	0
_rand_seed:
		.BYTE	0,0,0
_mmc5_sp_mode_shadow:
		.BYTE	0
_mmc5_sp_scroll_shadow:
		.BYTE	0
_mmc5_chr_mode_shadow:
		.BYTE	0
;_unref_18:
;		.BYTE	0,0,0,0
_cur_thread_idx:
		.BYTE	0
_thread_stack:
		.BYTE	0,0
_thread_prg8000:
		.BYTE	0,0
_thread_prgA000:
		.BYTE	0,0
_thread_sram_state:
		.BYTE	0,0
_prg_prg8000_shadow:
		.BYTE	0
_prg_prgA000_shadow:
		.BYTE	0
_sram_state_flag:
		.BYTE	0
_disasters_flags:
		.BYTE	0
_cur_building_price:
		.BYTE	0,0
_hud_money_redraw_req:
		.BYTE	0
;byte_37:
;		.BYTE	0								; REDUNDANT, unused var
_game_state_idx:
		.BYTE	0
_hud_redraw_req:
		.BYTE	0
_constr_palette_redraw_req:
		.BYTE	0
;byte_3B:
;		.BYTE	0								; another REDUNDANT var
_gui_active_controls_flags:
		.BYTE	0

		_tpos	_cur_screen_tile

;		.BYTE	0								; unused var

_scr_mode_idx:
		.BYTE	0
_pad_tmp:
		.BYTE	0
_pad0_held:
		.BYTE	0
_pad0_press:
		.BYTE	0
_pad0_autorep:
		.BYTE	0
_pad0_autorep_timer:
		.BYTE	0
_dpad0_autorep_timer:
		.BYTE	0
_pad0_prev_held:
		.BYTE	0
_pad1_prev_held:
		.BYTE	0

		_tsprarg	_spr_insert_args

		_tsprargraw	_spr_insert_raw_args

		_tpos		_cur_map_wnd_pos

_full_game_field_redraw_req:
		.BYTE	0
_update_rect_left:
		.BYTE	0
_update_rect_right:
		.BYTE	0
_update_rect_top:
		.BYTE	0
_update_rect_bottom:
		.BYTE	0
byte_58:
		.BYTE	0
byte_59:
		.BYTE	0

		_tpos	_tmp_tile_pos

_build_game_field_update_req:
		.BYTE	0
_full_game_field_redraw_req_tmp:
		.BYTE	0
_update_rect_left_tmp:
		.BYTE	0
_update_rect_right_tmp:
		.BYTE	0
_update_rect_top_tmp:
		.BYTE	0
_update_rect_bottom_tmp:
		.BYTE	0
byte_62:
		.BYTE	0
byte_63:
		.BYTE	0
byte_64:
		.BYTE	0
byte_65:
		.BYTE	0
byte_66:
		.BYTE	0
byte_67:
		.BYTE	0
byte_68:
		.BYTE	0
byte_69:
		.BYTE	0
byte_6A:
		.BYTE	0
byte_6B:
		.BYTE	0
word_6C:
		.BYTE	0,0
_tmp6E:
		.BYTE	0,0

_tmp70:
_anim_pos_is_load:
		.BYTE	0
byte_71:
_tmp71:
		.BYTE	0
_anim_pos_col:
_tmp72:
		.BYTE	0
byte_73:
_tmp73:
		.BYTE	0
_anim_pos_row:
_tmp74:
		.BYTE	0
byte_75:
_tmp75:
		.BYTE	0
byte_76:
_tmp76:
		.BYTE	0
byte_77:
_tmp77:
		.BYTE	0
byte_78:
_tmp78:
		.BYTE	0
byte_79:
_tmp79:
		.BYTE	0
byte_7A:
_tmp7A:
		.BYTE	0
byte_7B:
_tmp7B:
		.BYTE	0
byte_7C:
_tmp7C:
		.BYTE	0
byte_7D:
_tmp7D:
		.BYTE	0
byte_7E:
_tmp7E:
		.BYTE	0
byte_7F:
_tmp7F:
		.BYTE	0
byte_80:
_tmp80:
		.BYTE	0
byte_81:
_tmp81:
		.BYTE	0
byte_82:
_tmp82:
		.BYTE	0
byte_83:
_tmp83:
		.BYTE	0
_mmc5_prgA000_bank_tmp:
		.BYTE	0
_wnd_draw_cnt:
		.BYTE	0
_wnd_erase_cnt:
		.BYTE	0
_wnd_ppu_data_backup_req:
		.BYTE	0
_gui_ctrl_menu_press_idx:
		.BYTE	0
_wnd_lvl_idx:
		.BYTE	0
_wnd_idx:
		.BYTE	0,0
_wnd_cur_line_num:
		.BYTE	0
_wnd_type:
		.BYTE	0,0
_wnd_width_tile:
		.BYTE	0,0
_wnd_height_tile:
		.BYTE	0,0
_wnd_left_tile:
		.BYTE	0,0
_wnd_top_tile:
		.BYTE	0,0
_wnd_width:
		.BYTE	0,0
_wnd_line_tmp_ppu_ofs:
		.BYTE	0,0
word_9B:
		.BYTE	0,0
_wnd_line_tmp_width:
		.BYTE	0
_wnd_line_tmp_height:
		.BYTE	0
byte_9F:
		.BYTE	0
byte_A0:
		.BYTE	0

		_tpos	_cur_wnd_pos

		_tpos	_cur_wnd_pos_backup

_wnd_left_pos:
		.BYTE	0
_wnd_top_pos:
		.BYTE	0
_wnd_right_pos:
		.BYTE	0
_wnd_bottom_pos:
		.BYTE	0
_gui_ctrl_menu_redraw_flag:
		.BYTE	0
byte_AA:
		.BYTE	0
byte_AB:
		.BYTE	0
_constr_palette_update_req:
		.BYTE	0
_constr_palette_flags:
		.BYTE	0,0,0,0

; NOTE: this is a copy of _constr_palette_selected_idx and has the same
; value, used only in const palette enable test routine
;
_cur_constr_palette_selected_idx:
		.BYTE	0

word_B2:
		.BYTE	0,0
_tmp_extnt_dst_ofs:
		.BYTE	0,0
byte_B6:
		.BYTE	0
_constr_palette_selected_idx:
		.BYTE	0
_constr_palette_press_idx:
		.BYTE	0
_cur_game_cursor_area:
		.BYTE	0

		_tpos	_cur_game_cursor_scr_pos

		_tpos	_constr_cursor_tile

_pal_anim_counter:
		.BYTE	0
;_unref_10:
;		.BYTE	0
_cur_building_size:
		.BYTE	0
_tmp_ptrC1:
		.BYTE	0,0
_tmp_ptrC3:
		.BYTE	0,0
_tmp_ptrC5:
		.BYTE	0,0
_cur_build_sample_tile_idx:
		.BYTE	0
_cur_power_type_select_idx:
		.BYTE	0
_cur_port_type_select_idx:
		.BYTE	0
_cur_build_store_tile_idx:
		.BYTE	0
_bulldoze_tile_idx:
		.BYTE	0
_cur_bulldoze_size:
		.BYTE	0
_bulldoze_error_beep_flag:
		.BYTE	0
_bulldoze_start_delta_col:
		.BYTE	0
_bulldoze_start_delta_row:
		.BYTE	0
word_D0:
		.BYTE	0,0
word_D2:
		.BYTE	0,0
byte_D4:
		.BYTE	0
byte_D5:
		.BYTE	0
_hud_build_name_draw_req:
		.BYTE	0
;_hud_build_name_src_ofs:						; REDUNDANT, not used anymore
;		.BYTE	0
_game_msg_idx:
		.BYTE	0
;_prev_game_msg_idx:							; REDUNDANT, unused anymore
;		.BYTE	0
_game_msg_awaits_flag:
		.BYTE	0
word_DA:
		.BYTE	0,0
_ptrDC:
		.BYTE	0,0
;_unref_11:
;		.BYTE	0,0,0,0,0,0,0
_tmpE5:
		.BYTE	0

_tmpE6	EQU		.
_tmpE7	EQU		_tmpE6+1
_tmpE8	EQU		_tmpE6+2
_tmpE9	EQU		_tmpE6+3

		_tfix16_16	_dvdn

_tmpEA	EQU		.
_tmpEB	EQU		_tmpEA+1

		_tfix8_8	_dvdr

_ptrEC:
		.BYTE	0,0
_ptrEE:
		.BYTE	0,0

_apu_mus_idx_req:
		.BYTE	0
_apu_se_idx_req:
		.BYTE	0,0
_apu_mus_seq_ofs:
		.BYTE	0
_apu_se_loop_idx:
		.BYTE	0,0
_apu_tmpF6:
		.BYTE	0
_apu_tmpF7:
		.BYTE	0
_apu_se_loop_counters:
		.BYTE	0,0
_apu_trk_data_ptr:
		.BYTE	0,0
_apu_dpcm_disable_flag:
		.BYTE	0
_apu_se80_loop_counter:
		.BYTE	0

		_tpos	_info_maps_rect_pos

		_tpos	_map_start_pos

		_tpos	_map_pos

		_tpos	_map_tmp_pos

		_tpos	_tmp_map_pos

_tmp_map_pos_backup_buf_Y:
		.BYTE	0,0,0
_tmp_map_pos_backup_buf_X:
		.BYTE	0,0,0

		_tpos	_tmp_area_pos

		ECHO "-------------"
		ECHO "zero ram free:",[$100-.]
		ECHO "-------------"

		ORG $0100

_mmc5_nt_mode_shadow:
		.BYTE	0
_screen_refresh_req:
		.BYTE	0
_game_msg_wnd_active_flag:
		.BYTE	0
_thread0_flags:
		.BYTE	0,0
_thread_priority_req:
		.BYTE	0
_game_core_state:
		.BYTE	0
_mini_map_need_redraw_flag:
		.BYTE	0
_thread1_flags:
		.BYTE	0,0
;_unref_13:
;		.BYTE	0,0,0
_mini_map_cur_row:
		.BYTE	0
_mini_map_cur_col:
		.BYTE	0
;_unref_14:
;		.BYTE	0,0

		_tpos	_last_water_tile_pos				; NEW: new var for ship deploy

_cur_info_maps_button_idx:
		.BYTE	0
_prev_info_maps_button_idx:
		.BYTE	0
_scr_res_window_opened_flag:
		.BYTE	0
_scr_res_control_flags:
		.BYTE	0
_scr_res_idx:
		.BYTE	0
_tmp118:
		.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
byte_127:
		.BYTE	0
byte_128:
		.BYTE	0
byte_129:
		.BYTE	0
byte_12A:
		.BYTE	0
_new_map_number:
		.BYTE	0,0
_game_wnd_spr_mode_flag:
		.BYTE	0
_info_maps_menu_extra_idx:
		.BYTE	0,0,0
byte_131:
		.BYTE	0
_budget_tax_adjust_value:
		.BYTE	0
_max_tax_rate:
		.BYTE	0,0
_taxes_income:
		.BYTE	0,0,0

;		_tpos	_info_maps_rect_pos

_info_maps_menu_extra_value_backup:
		.BYTE	0
_budget_screen_return_to_info_maps_flag:
		.BYTE	0
_info_maps_button_pending:
		.BYTE	0
_city_eval_string_counter:
		.BYTE	0
_obj_active_flags:
		.BYTE	0
_frames_div4_counter:
		.BYTE	0
_new_city_screen_cur_row_idx:
		.BYTE	0
_new_city_screen_cur_col_idx:
		.BYTE	0
_title_sub_idx:
		.BYTE	0
_cur_obj_idx:
		.BYTE	0
;_unref_16:
;		.BYTE	0
_obj_tornado_counter:
		.BYTE	0
_obj_plane_anim_idx:
		.BYTE	0
_obj_plane_anim_timer:
		.BYTE	0

		_tpos	_last_airport_pos

_obj_monster_delay:
		.BYTE	0
_obj_monster_anim_phase0:
		.BYTE	0
_obj_monster_anim_phase1:
		.BYTE	0
_obj_monster_chr_idx:
		.BYTE	0
_obj_monster_spr_idx:
		.BYTE	0

		_tobj6w	_obj_shift

_new_city_map_regen_req:
		.BYTE	0
_new_city_map_regen_delay:
		.BYTE	0
_new_city_digit_spr_y_pos:
		.BYTE	0
_new_city_button_to_change_flags:
		.BYTE	0
byte_15F:
		.BYTE	0

		ORG	$160

		_tarray	_stack_upper,$50

		_tarray	_stack_lower,$50

		ORG	$200

		_tarray	_spr_buf,$100

		ORG	$300

		_tarray	_ppu_queue_buf,$100

		ORG	$400

		_tarray	_extnt_queue_buf,$100

		ORG	$500

_pad0_autorep_delay:
		.BYTE	0
_RCI_meter_redraw_req:
		.BYTE	0

		_trci _RCI_meters_values

_common_yes_no_menu_cursor_pos:
		.BYTE	0

		_tpos _cur_obj_pos

		_tpos _cur_dst_pos

_ppu_pal_dequeue_req:
		.BYTE	0
_screen_pal_cur_spr_idx:
		.BYTE	0
_screen_pal_cur_bg_idx:
		.BYTE	0

		_tpos _prev_constr_cursor_tile

		_tpos _cur_constr_cursor_map_pos

_tmp511:
		.BYTE	0
_tmp512:
		.BYTE	0
_hud_date_redraw_req:
		.BYTE	0
_tmp514:
		.BYTE	0
;_unref_5:
;		.BYTE	0
_tmp_popul_hex:
		.BYTE	0,0,0
_tmp51A:
		.BYTE	0
_tmp51B:
		.BYTE	0
_tmp_popul_decimal_buf:
		.BYTE	0,0,0,0,0,0
_tmp522:
		.BYTE	0
_tmp523:
		.BYTE	0
_nmi_seconds_timer:
		.BYTE	0
_nmi_frames_timer:
		.BYTE	0
_presents_build_menu_req:
		.BYTE	0
_cur_present_select_idx:
		.BYTE	0
_tmp528:
		.BYTE	0
_tmp529:
		.BYTE	0
_tmp52A:
		.BYTE	0
_tmp52B:
		.BYTE	0
_tmp52C:
		.BYTE	0
_tmp52D:
		.BYTE	0
_tmp52E:
		.BYTE	0
_tmp52F:
		.BYTE	0
_tmp530:
		.BYTE	0
_input_forbid_delay:
		.BYTE	0
_game_core_stop_flag:
		.BYTE	0
_tmp533:
		.BYTE	0
_tmp534:
		.BYTE	0
_map_bg_anim_counter:
		.BYTE	0
_game_core_timer:
		.BYTE	0
_debug_scroll_msg_flags:
		.BYTE	0
_stadium_anim_enable_flag:
		.BYTE	0
_scroll_msg_active_flag:
		.BYTE	0
_scroll_msg_shift_pos:
		.BYTE	0
_scroll_msg_idx:
		.BYTE	0
_scroll_msg_start_pos:
		.BYTE	0
_scroll_msg_end_char:
		.BYTE	0
_power_build_menu_req:
		.BYTE	0
_airport_build_menu_req:
		.BYTE	0
_hud_popul_redraw_req:
		.BYTE	0
_hud_city_name_redraw_req:
		.BYTE	0
_tmp542:
		.BYTE	0
_tmp543:
		.BYTE	0
_tmp544:
		.BYTE	0
_tmp545:
		.BYTE	0
_tmp546:
		.BYTE	0
_tmp547:
		.BYTE	0

		_tpos _constr_palette_area_cursor_pos

		_tpos _game_map_area_cursor_pos_backup

		_tobj6w	_obj_pos

		_tobj6w	_obj_dst_pos

		_tobj4b	_obj_cur_spr_idx

		_tobj4b	_obj_dst_spr_idx

		_tobj4b	_obj_spr_delta

;_unref?:
;		.BYTE	0
_train_hide_mode:
		.BYTE	0
_train_turns_counter:
		.BYTE	0

;		_tpos	_map_start_pos
;
;		_tpos	_map_pos
;
;		_tpos	_map_tmp_pos

_tmp579:
		.BYTE	0
_tmp57A:
		.BYTE	0
_tmp57B:
		.BYTE	0
_tmp57C:
		.BYTE	0
_tmp57D:
		.BYTE	0
_map_dir:
		.BYTE	0
_map_last_dir:
		.BYTE	0
_tmp580:
		.BYTE	0
_tmp581:
		.BYTE	0
_tmp582:
		.BYTE	0
_main_sub_idx:
		.BYTE	0

; budget calc vars
;
_loan_fee:
		.WORD	0
_budget_display_timer:
		.WORD	0
_budget_tax_cur_pos:
		.BYTE	0

		_tfund16 _budget_fund_alloc

		_tfund8	_budget_fund_rate

_budget_funds_alloc_sum:
		.BYTE	0,0,0
_budget_funds_alloc_sum_loan:
		.BYTE	0,0,0
_budget_money:
		.BYTE	0,0,0
; -

_cur_graphs_button_press_idx:
		.BYTE	0
_prev_graphs_button_press_idx:
		.BYTE	0
_graphs_scr_period_mode:
		.BYTE	0
_graphs_draw_data_pos:
		.BYTE	0
_game_setup_sub_idx:
		.BYTE	0
_scenario_result_idx:
		.BYTE	0
_mmc5_mul1_shadow:
		.BYTE	0
_mmc5_mul0_shadow:
		.BYTE	0
_backup_var:
		.BYTE	0
_mmc5_tmp_mul_result:
		.BYTE	0
_mmc5_tmp_mul_data:
		.WORD	0
_has_saved_city_data_flag:
		.BYTE	0
word_5A9:
		.BYTE	0,0
_tmp5AB:
		.BYTE	0
_tmp5AC:
		.BYTE	0
_tmp5AD:
		.BYTE	0
_tmp5AE:
		.BYTE	0
_tmp5AF:
		.BYTE	0
_mmc5_chr_banks_shadow:
		.BYTE	0,0,0,0

		_tcity _city

; FIX of SAVE/LOAD, also aliases to not to rename a lot again
_cur_game_mode	EQU	_city._cur_game_mode
_scenario_idx	EQU	_city._scenario_idx
;

_goto_alert_req:
		.BYTE	0
_goto_in_progress_delay:
		.BYTE	0

		_tpos _goto_dst_tile

_airplane_crash_req:
		.BYTE	0
_budget_scr_draw_req:
		.BYTE	0
_city_taxes_collected:
		.BYTE	0,0,0

; global city funding parameters
;
; rate of funding, recalculated twice: in THREAD1 while collecting data
; and in THREAD0 when adjusting the budget, has a default values
; at the beginning of the game
;
; ALERT! keep order the same for all these vars, it's important!
;
_fund_rate_fire:
		.BYTE	0
_fund_rate_police:
		.BYTE	0
_fund_rate_trans:
		.BYTE	0
;
; requested funding amounts recalculated every time by THREAD1
;
_fund_request_fire:
		.BYTE	0,0
_fund_request_police:
		.BYTE	0,0
_fund_request_trans:
		.BYTE	0,0
_fund_request_education:
		.BYTE	0,0
;
; actual current funding amounts
;
_fund_alloc_fire:
		.BYTE	0,0
_fund_alloc_police:
		.BYTE	0,0
_fund_alloc_trans:
		.BYTE	0,0
_fund_alloc_education:
		.BYTE	0,0
; -

_doing_a_good_job_votes:
		.BYTE	0
_doing_a_bad_job_votes:
		.BYTE	0
_popul_cur:
		.BYTE	0,0,0
_popul_net_migration:
		.BYTE	0,0,0
_assessed_value:
		.BYTE	0,0,0
_annual_score_change:
		.BYTE	0,0
_prev_city_score:
		.BYTE	0,0
_worst_problem_list:
		.BYTE	0,0,0,0
_worst_problem_percents:
		.BYTE	0,0,0,0

		_tfix8_8	_cur_RCI_levels_sum

_earthquake_shake_req:
		.BYTE	0

; -
; following are set of various statistics of the city
_total_RCI_counts:
		.BYTE	0,0

		_trciw	_RCI_counts

_fires_count:
		.BYTE	0,0

; these vars are accessed individually in the statistic
; crawler, however, for the overview screen they are put
; into ptr list and accessed via indexes, if you change
; it's order, make sure you change indexes as well
;
_hospitals_count:
		.BYTE	0,0
_schools_count:
		.BYTE	0,0
_police_stations_count:
		.BYTE	0
_fire_stations_count:
		.BYTE	0
_stadiums_count:
		.BYTE	0
_coal_power_station_count:
		.BYTE	0
_nuke_power_station_count:
		.BYTE	0
_sea_ports_count:
		.BYTE	0
_airports_count:
		.BYTE	0
_roads_count:
		.BYTE	0,0
_rails_count:
		.BYTE	0,0
_electrics_count:
		.BYTE	0,0

; structured already, access can be indirect or individual
; as well.
;
		_tdev	_stats

; NOTE, following counters are used as an array of similar
; parameters and accessed indirectly! so don't change its
; order!
;
_parks_count:
		.BYTE	0,0
_woods_count:
		.BYTE	0,0
_ground_count:
		.BYTE	0,0
_water_count:
		.BYTE	0,0

_counters_area_size	EQU	[.-_total_RCI_counts]

; end if statistic parameters
; -

_corruption_anim_req:
		.BYTE	0

;		_tpos _tmp_map_pos

_last_map_tile_read:
		.BYTE	0
_cur_map_tile_is_inside_a_building_flag:
		.BYTE	0
_cur_area_power_state:
		.BYTE	0
_powered_areas:
		.BYTE	0,0
_hospitals_delta:
		.BYTE	0
_schools_delta:
		.BYTE	0
_tmp6AA:
		.BYTE	0,0
_tmp6AC:
		.BYTE	0,0

		_treq _required

_cur_area_develop_state:
		.BYTE	0

		_tfix8_8	_prev_RCI_levels_sum

_popul_last_year:
		.BYTE	0,0,0

		_tpos	_city_center_pos

;_tmp_map_pos_backup_buf_Y:
;		.BYTE	0,0,0
;_tmp_map_pos_backup_buf_X:
;		.BYTE	0,0,0
;
;		_tpos	_tmp_area_pos

_life_ratio_idx:
		.BYTE	0
_tmp6C4:
		.BYTE	0
_power_nodes_cnt:
		.BYTE	0,0
_RCI_max_range_10_years:
		.BYTE	0
_RCI_max_range_100_years:
		.BYTE	0
_disaster_tiles_to_flood_count:
		.BYTE	0
_monster_in_water_flag:
		.BYTE	0
_city_gone_broke_flag:
		.BYTE	0

; END OF REINIT AREA

_tmp6CC:
		.BYTE	0
_tmp6CD:
		.BYTE	0

_problem_data	SET	.

_tmp6CE:
		.BYTE	0
_tmp6CF:
		.BYTE	0
_tmp6D0:
		.BYTE	0
_tmp6D1:
		.BYTE	0
_tmp6D2:
		.BYTE	0
_tmp6D3:
		.BYTE	0
_tmp6D4:
		.BYTE	0
_tmp6D5:
		.BYTE	0
_tmp6D6:
		.BYTE	0,0
_tmp6D8:
		.BYTE	0
_tmp6D9:
		.BYTE	0
_tmp6DA:
		.BYTE	0
_tmp6DB:
		.BYTE	0

_tmp6DC		EQU .
_problem_accepted:
		.BYTE	0,0,0,0,0,0,0
					; one of these vars used as tmp	value _tmp_name_input_mode_idx
					; in name input	system
_problem_percent:
		.BYTE	0,0,0,0,0,0,0

		ORG	$700

		_tarray	_pre_ppu_buf,$40

_ram_signature:
		.BYTE	0,0,0,0

		ORG	$7C0

_pal_bg_buf:
		.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
_pal_spr_buf:
		.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

_apu_mus_p1_delay:
		.BYTE	0
_apu_mus_p2_delay:
		.BYTE	0
_apu_mus_tri_delay:
		.BYTE	0

_apu_mus_p1_timer:
		.BYTE	0
_apu_mus_p2_timer:
		.BYTE	0
_apu_mus_tri_timer:
		.BYTE	0

_apu_mus_p1_data_pos:
		.BYTE	0
_apu_mus_p2_data_pos:
		.BYTE	0
_apu_mus_tri_data_pos:
		.BYTE	0

_apu_mus_v09:
		.BYTE	0
_apu_mus_tempo:
		.BYTE	0
_apu_mus_v0B:
		.BYTE	0
_apu_mus_v0C:
		.BYTE	0
_apu_mus_dmc_data_pos:
		.BYTE	0
_apu_mus_dmc_timer:
		.BYTE	0
_apu_mus_p1_tempo_delta:
		.BYTE	0

_apu_mus_p1_time_lo:
		.BYTE	0
_apu_mus_p2_tempo_delta:
		.BYTE	0
_apu_mus_p1_tempo_cnt:
		.BYTE	0
_apu_mus_p2_tempo_cnt:
		.BYTE	0
_apu_mus_p2_time_lo:
		.BYTE	0
_apu_mus_envelope:
		.BYTE	0
_apu_mus_cur_idx:
		.BYTE	0
_apu_mus_dmc_delay:
		.BYTE	0
_apu_mus_tri_time_lo:
		.BYTE	0
_apu_mus_p1_release_mode:
		.BYTE	0
_apu_mus_p2_release_mode:
		.BYTE	0
_apu_mus_fade_out_timer:
		.BYTE	0

;--------------------P-P-U----------------------

		ORG $2000

_PPU_CTRL:
		.BYTE 0
_PPU_MASK:
		.BYTE 0
_PPU_STATUS:
		.BYTE 0
_PPU_OAM_ADDR:
		.BYTE 0
_PPU_OAM_DATA:
		.BYTE 0
_PPU_SCROLL:
		.BYTE 0
_PPU_ADDR:
		.BYTE 0
_PPU_DATA:
		.BYTE 0

;--------------------A-P-U----------------------

		ORG $4000

_APU_PULSE1_ENV:
		.BYTE 0
_APU_PULSE1_SWEEP:
		.BYTE	0
_APU_PULSE1_TIME:
		.BYTE 0
_APU_PULSE1_LEN:
		.BYTE 0
_APU_PULSE2_ENV:
		.BYTE 0
_APU_PULSE2_SWEEP:
		.BYTE	0
_APU_PULSE2_TIME:
		.BYTE 0
_APU_PULSE2_LEN:
		.BYTE 0
_APU_TRI_COUNT:
		.BYTE 0
_APU_TRI_UNUSED:
		.BYTE 0
_APU_TRI_TIME:
		.BYTE 0
_APU_TRI_LEN:
		.BYTE 0
_APU_NOISE_ENV:
		.BYTE 0
_APU_NOISE_UNUSED:
		.BYTE 0
_APU_NOISE_LOOP:
		.BYTE 0
_APU_NOISE_LEN:
		.BYTE 0
_APU_DMC_FREQ:
		.BYTE 0
_APU_DMC_LOAD:
		.BYTE 0
_APU_DMC_ADDR:
		.BYTE 0
_APU_DMC_LEN:
		.BYTE 0
_PPU_OAM_DMA:
		.BYTE 0
_APU_STATUS:
		.BYTE 0
_PAD0_REG:
		.BYTE 0
_PAD1_REG:
		.BYTE 0

;		ORG $4083
;
;byte_4083:
;		.BYTE	0
;byte_4084:
;		.BYTE	0

		ORG	$5100

_MMC5_PRG_SIZE:
		.BYTE	0
_MMC5_CHR_SIZE:
		.BYTE	0
_MMC5_WRAM0_ENABLE:
		.BYTE	0
_MMC5_WRAM1_ENABLE:
		.BYTE	0
_MMC5_CHR_MODE:
		.BYTE	0
_MMC5_NT_MODE:
		.BYTE	0
_MMC5_NT_FILL:
		.BYTE	0
_MMC5_AT_FILL:
		.BYTE	0,0,0,0,0,0,0,0,0,0,0,0
_MMC5_PRG_PAGES:
		.BYTE	0,0,0,0,0
		.BYTE	0,0,0,0,0,0,0,0
_MMC5_CHR_BANKSA:
		.BYTE	0,0,0,0,0,0,0,0
_MMC5_CHR_BANKSB:
		.BYTE	0,0,0,0,0,0,0,0

		ORG	$5200

_MMC5_SP_MODE:
		.BYTE	0
_MMC5_SP_SCROLL:
		.BYTE	0
_MMC5_SP_PAGE:
		.BYTE	0,0
_MMC5_IRQ_ENABLE:
		.BYTE	0
_MMC5_MUL0:
		.BYTE	0
_MMC5_MUL1:
		.BYTE	0

		ORG	$5C00

		_tarray	_MMC5_EXRAM,$400

		ORG	$6000

		; 8-bits array, 76 rows, 76 columns
		_tarray	_city_map,76*76

		; 1-bits array, 76 rows, 76 columns
		_tarray	_power_map,(76*76)/8

		; 4-bits array, 38 rows, 38 columns (76/2)
		_tarray	_traffic_map,(38*38)/2

		ORG $7C40

		_tarray	_nt_backup0,$100
		_tarray	_nt_backup1,$E0
		_tarray	_extnt_backup0,$100
		_tarray	_extnt_backup1,$E0

		ORG	$8000

		; 4-bits array, 19 rows, 19 columns (76/4)
		_tarray	_land_val_map,((19*19)/2)+1

		; 4-bits array, 19 rows, 19 columns
		_tarray	_crime_map,((19*19)/2)+1

		; 4-bits array, 19 rows, 19 columns
		_tarray	_pollution_map,((19*19)/2)+1

		; 4-bits array, 19 rows, 19 columns
		_tarray	_pop_dencity_map,((19*19)/2)+1

		; 4-bits array, 19 rows, 19 columns
		_tarray	_pop_growth_map,((19*19)/2)+1

		; 4-bits array, 10 rows, 10 columns
		_tarray	_police_map,(10*10)/2

		; 4-bits array, 10 rows, 10 columns
		_tarray	_fire_map,(10*10)/2

		_tarray	_tmp_map,1024

		_tgraphs _graphs

;	SAVE DATA AREA

_sram_data_crc:
		.WORD	0

		_tgraphs _sram_graphs

		; 8-bits array, 76 rows, 76 columns
		_tarray	_sram_city_map,76*76

		_tcity	_sram_vars

_sram_has_saved_city_data_flag:
		.BYTE	0

		#if .>$A000
			ECHO "OUT OF SRAM=",[.-$A000]
		#endif
