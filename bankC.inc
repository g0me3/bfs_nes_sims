
		ORG $8000

		BANK_START $FC

; ---------------------------------------------------------------------------
; ----------------I-N-E-R-F-A-C-E--M-A-I-N--E-N-G-I-N-E----------------------
; ---------------------------------------------------------------------------

		SECTION_START	C00

; =============== S U B	R O U T	I N E =======================================
_game_start:
		JSR		_game_scr_init

; =============== S U B	R O U T	I N E =======================================
_MAIN_LOOP:
		LDA		_main_sub_idx
		CMP		#$02
		BEQ		loc_13600B
		RTS
loc_13600B:
		JSR		_wait_for_nmi
		JSR		_game_states_select
		LDA		_obj_active_flags
		AND		#$01
		BEQ		loc_13601B
		JSR		_far_heavy_traffic_report_test
loc_13601B:
		LDA		_game_state_idx
		JSR		_switch
		.WORD	_game_state00_hud_static_redraw
		.WORD	_game_state01_constr_palette_redraw
		.WORD	_game_state02_map_scroll
		.WORD	_game_state03_map_scroll_mode_set
		.WORD	_game_state04_constr_area_redraw
		.WORD	_game_state05_constr_palette_update
		.WORD	_game_state06_game_hud_redraw
		.WORD	_game_state07
		.WORD	_game_state08_build
		.WORD	_game_state09_presents_building_name_and_cost_hud_draw
		.WORD	_game_state0A_power_build_menu
		.WORD	_game_state0B_gui_ctrl_menu
		.WORD	_game_state0C_RCI_meter_redraw
		.WORD	_game_state0D_hud_date_redraw
		.WORD	_game_state0E
		.WORD	_game_state0F_hud_money_redraw
		.WORD	_game_state10_fiscal_year_draw
		.WORD	_game_state11_hud_popul_redraw
		.WORD	_game_state12_airport_build_menu
		.WORD	_game_state13_msg_wnd_display
		.WORD	_game_state14_presents_build_menu
		.WORD	_game_state15_game_cursor_area_switch
		.WORD	_game_state16_hud_city_name_redraw
		.WORD	_game_state17_common

; =============== S U B	R O U T	I N E =======================================
_game_state_common:
		JSR		_game_cursor_input_control		; do common background work
		JSR		_spr_finish
		JSR		_auto_goto_test
		FJSRA	_constr_palette_state_update,PRG7,PRG3
		LDA		_ppu_queue_pos_tail				; calc ppu queues buffers fill
		SEC										; then use it in overall test for
		SBC		_ppu_queue_pos_head				; idle state
		STA		_ptr0
		LDA		_extnt_queue_pos_tail
		SEC
		SBC		_extnt_queue_pos_head
		STA		_ptr0+1
		LDA		_gui_active_controls_flags		; test for idle state, checking if
		EOR		#$03							; any of critical state flags arent
		ORA		_ptr0							; clear, so mean there is some pending
		ORA		_ptr0+1							; requests still
		ORA		_hud_static_redraw_req
		ORA		_constr_palette_redraw_req
;		ORA		byte_3B							; REDUNDANT, never written
		ORA		_constr_palette_update_req
		ORA		_hud_present_name_draw_req
		ORA		_RCI_meter_redraw_req
		ORA		_hud_date_redraw_req
		ORA		_hud_popul_redraw_req
		ORA		_hud_city_name_redraw_req
		ORA		_game_core_stop_flag
		ORA		_hud_money_redraw_req
		ORA		_scroll_msg_active_flag
;		STA		byte_37							; REDUNDANT, never read!
		ORA		byte_5C
		ORA		_hud_redraw_req
		ORA		_game_msg_wnd_active_flag
		ORA		_budget_scr_draw_req
		STA		_game_core_state				; any of these flags gives priority to this thread
		JMP		_MAIN_LOOP

; =============== S U B	R O U T	I N E =======================================
_game_state00_hud_static_redraw:
		JSR		_hud_static_redraw				; redraw static part of the HUD (buttons, extnts)
		BCS		loc_1360DD
		LDA		_gui_active_controls_flags		; set one of flags for top side of HUD redraw complete
		ORA		#$01
		STA		_gui_active_controls_flags
		LDA		#$FF
		STA		_hud_date_redraw_req			; set dinamic parts of the HUD redraw requests
		STA		_hud_money_redraw_req
		STA		_hud_popul_redraw_req
		STA		_hud_city_name_redraw_req
loc_1360DD:
		JMP		_game_state_common

; FIX: moved here from bankD to collect free space in one place
;
; =============== S U B	R O U T	I N E =======================================
_game_state01_constr_palette_redraw:
		JSR		_constr_palette_redraw			; redraw leftmost part of HUD/GUI, a
		BCS		loc_150019						; cinstruction palette
		LDA		#$3A
		STA		_MMC5_CHR_BANKSA+2				; restore cursor sprites CHR bank
		LDA		#$FF
		STA		_hud_present_name_draw_req		; request to draw current
		LDA		_gui_active_controls_flags
		ORA		#$02
		STA		_gui_active_controls_flags
		LDA		#$FF
		STA		_RCI_meter_redraw_req
loc_150019:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state02_map_scroll:
		LDA		_gui_active_controls_flags		; jump to map scroll if upper-left area are
		AND		#$03							; filled with map, replacing the GUI
		BEQ		.mini_map_wait_for_scrolls
		LSR
		BCC		.mini_map_hor_queue				; or check which one still displayed
		JSR		_map_scroll_top_redraw			; top side of map ppu data queue
		BCS		.j_game_state_common
		LDA		_gui_active_controls_flags
		AND		#$FE
		STA		_gui_active_controls_flags
		JMP		_game_state_common
.mini_map_hor_queue:
		JSR		_map_scroll_left_redraw			; left side of map ppu data queue
		BCS		.j_game_state_common
		LDA		_gui_active_controls_flags
		AND		#$FD
		STA		_gui_active_controls_flags
.j_game_state_common:
		JMP		_game_state_common
.mini_map_wait_for_scrolls:
		LDA		#$36
		STA		_MMC5_CHR_BANKSA+2
		JSR		_map_scroll_mode_minimap_draw
		JSR		_map_scroll_mode_arrows_draw
		LDA		_cur_screen_tile._COL
		ORA		_cur_screen_tile._ROW
		AND		#$01
		BNE		.mini_map_continue_scroll
		LDA		_pad0_held
		AND		#$40
		BNE		.button_B_held
		LDA		#$00
		STA		_scr_mode_idx					; exit mini-map mode, when B released
		JSR		_scroll_msg_reset
		JMP		_game_state_common
.mini_map_continue_scroll:
		LDA		_tmp70
		AND		#$03
		BEQ		.mini_map_continue_vert_scroll
		LSR										; horizontal scroll needed
		BCC		.mini_map_continue_left_scroll
		INC		_cur_screen_tile._COL			; right scroll needed
		LDA		_cur_screen_tile._COL
		CMP		#$40
		BCC		.mini_map_scroll_done			; wrap to 0
		LDA		#$00
		STA		_cur_screen_tile._COL
		BEQ		.mini_map_scroll_done
.mini_map_continue_left_scroll:
		DEC		_cur_screen_tile._COL			; wrap to 3F
		BPL		.mini_map_scroll_done
		LDA		#$3F
		STA		_cur_screen_tile._COL
		JMP		.mini_map_scroll_done
.mini_map_continue_vert_scroll:
		LDA		_tmp70
		AND		#$0C
		BEQ		.mini_map_scroll_done
		AND		#$04
		BEQ		.mini_map_continue_down_scroll
		INC		_cur_screen_tile._ROW			; up scroll needed
		LDA		_cur_screen_tile._ROW
		CMP		#$3C
		BCC		.mini_map_scroll_done
		LDA		#$00
		STA		_cur_screen_tile._ROW
		BEQ		.mini_map_scroll_done
.mini_map_continue_down_scroll:
		DEC		_cur_screen_tile._ROW
		BPL		.mini_map_scroll_done
		LDA		#$3B
		STA		_cur_screen_tile._ROW
.mini_map_scroll_done:
		LDA		#$00
		STA		_tmp70
		JMP		_game_state_common
.button_B_held:
		LDA		_pad0_held
		AND		#$0F
		BEQ		loc_13617C
;		JSR		_buffers_test_flush
;		BCC		loc_13617F
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BEQ		loc_13617F
loc_13617C:
		JMP		_game_state_common
loc_13617F:
		LDA		_pad0_held
		AND		#$0F
		STA		_tmp70
		AND		#$03
		BEQ		loc_1361EA
		LSR
		BCS		loc_1361C0
		LDA		_cur_map_wnd_pos._COL
		BEQ		loc_1361BD
		LDA		_cur_game_cursor_area
		BEQ		loc_1361A4
		LDA		_constr_cursor_tile._COL
		CLC
		ADC		_cur_selected_buildings_size
		CMP		#$19
		BCS		loc_1361A4
		LDA		_cur_game_cursor_scr_pos._COL
		CLC
		ADC		#$08
		STA		_cur_game_cursor_scr_pos._COL
loc_1361A4:
		LDA		#$FF
		STA		byte_71
		LDA		#$F8
		STA		_anim_pos_col
		LDA		#$1F
		STA		byte_73
		JSR		sub_150124
		DEC		_cur_map_wnd_pos._COL
		DEC		_cur_screen_tile._COL
		BPL		loc_1361BD
		LDA		#$3F
		STA		_cur_screen_tile._COL
loc_1361BD:
		JMP		_game_state_common
loc_1361C0:
		LDA		_cur_map_wnd_pos._COL
		CMP		#$35
		BCS		loc_1361E7
		LDA		_cur_game_cursor_area
		BEQ		loc_1361D4
		LDA		_cur_game_cursor_scr_pos._COL
		CMP		#$40
		BCC		loc_1361D4
		SBC		#$08
		STA		_cur_game_cursor_scr_pos._COL
loc_1361D4:
		LDA		#$FF
		STA		byte_71
		LDA		#$19
		STA		_anim_pos_col
		LDA		#$00
		STA		byte_73
		JSR		sub_150124
		INC		_cur_map_wnd_pos._COL
		INC		_cur_screen_tile._COL
loc_1361E7:
		JMP		_game_state_common
loc_1361EA:
		LDA		_pad0_held
		AND		#$0C
		BEQ		loc_136253
		LSR
		LSR
		LSR
		BCS		loc_136229
		LDA		_cur_map_wnd_pos._ROW
		BEQ		loc_136226
		LDA		_cur_game_cursor_area
		BEQ		loc_13620D
		LDA		_constr_cursor_tile._ROW
		CLC
		ADC		_cur_selected_buildings_size
		CMP		#$15
		BCS		loc_13620D
		LDA		_cur_game_cursor_scr_pos._ROW
		CLC
		ADC		#$08
		STA		_cur_game_cursor_scr_pos._ROW
loc_13620D:
		LDA		#$00
		STA		byte_71
		LDA		#$F8
		STA		_anim_pos_col
		LDA		#$16
		STA		byte_73
		JSR		sub_150124
		DEC		_cur_map_wnd_pos._ROW
		DEC		_cur_screen_tile._ROW
		BPL		loc_136226
		LDA		#$3B
		STA		_cur_screen_tile._ROW
loc_136226:
		JMP		_game_state_common
loc_136229:
		LDA		_cur_map_wnd_pos._ROW
		CMP		#$37
		BCS		loc_136250
		LDA		_cur_game_cursor_area
		BEQ		loc_13623D
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$40
		BCC		loc_13623D
		SBC		#$08
		STA		_cur_game_cursor_scr_pos._ROW
loc_13623D:
		LDA		#$00
		STA		byte_71
		LDA		#$15
		STA		_anim_pos_col
		LDA		#$15
		STA		byte_73
		JSR		sub_150124
		INC		_cur_map_wnd_pos._ROW
		INC		_cur_screen_tile._ROW
loc_136250:
		JMP		_game_state_common
loc_136253:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state03_map_scroll_mode_set:
		LDA		#$01
		STA		_scr_mode_idx
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state07:
		LDA		_cur_game_cursor_scr_pos._ROW
		CMP		#$38
		BCS		loc_13626D
		LDA		_pad0_autorep_press
		AND		#$80
		STA		_game_msg_wnd_active_flag
		JMP		_game_state_common
loc_13626D:
		PUSHB	_constr_palette_press_idx
		JSR		_is_constr_palette_item_active
		PLA
		BCS		_j_apu_se_error
		CMP		_constr_palette_selected_idx
		BEQ		sub_1362C0
		STA		_cur_constr_palette_selected_idx
		STA		_constr_palette_selected_idx
		TAX
;		JSR		_apu_se_consrt_palette_click
		APUA_SE	_SE_IDX_BCLICK
		LDA		_cur_game_cursor_scr_pos._COL
		STA		_constr_palette_area_cursor_pos._COL
		LDA		_cur_game_cursor_scr_pos._ROW
		STA		_constr_palette_area_cursor_pos._ROW
		LDA		_building_sizes_list,X
		STA		_cur_selected_buildings_size
		LDA		#$FF
		STA		_hud_present_name_draw_req
		CPX		#$0B
		BNE		loc_13629F
		STA		_power_build_menu_req
		JMP		_game_state_common
loc_13629F:
		CPX		#$0C
		BNE		loc_1362A9
		STA		_airport_build_menu_req
		JMP		_game_state_common
loc_1362A9:
		CPX		#$0D
		BNE		_j_game_state_common
		STA		_presents_build_menu_req
		FJSRA	_presents_wnd_generate,PRGC,PRG2
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_j_apu_se_error:
;		JSR		_apu_se_error
		APUA_SE	_SE_IDX_ERROR

; =============== S U B	R O U T	I N E =======================================
_j_game_state_common:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
sub_1362C0:
		CMP		#$0B
		BNE		loc_1362CE
		LDA		#$FF
		STA		_hud_present_name_draw_req
		STA		_power_build_menu_req
		JMP		_game_state_common
loc_1362CE:
		CMP		#$0C
		BNE		loc_1362DC
		LDA		#$FF
		STA		_hud_present_name_draw_req
		STA		_airport_build_menu_req
		JMP		_game_state_common
loc_1362DC:
		CMP		#$0D
		BNE		_j_apu_se_error
		LDA		#$FF
		STA		_hud_present_name_draw_req
		STA		_presents_build_menu_req
		FJSRA	_presents_wnd_generate,PRGC,PRG2
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_is_constr_palette_item_active:
		ASL
		TAX
		LDY		_palette_activity_flags,X
		LDA		_palette_activity_flags+1,X
		AND		_constr_palette_flags,Y
		BEQ		loc_136300
		SEC
		RTS
loc_136300:
		CLC
		RTS
_palette_activity_flags:
		.BYTE	$03,$08
		.BYTE	$03,$02
		.BYTE	$02,$80
		.BYTE	$02,$20
		.BYTE	$02,$08
		.BYTE	$02,$02
		.BYTE	$01,$80
		.BYTE	$01,$20
		.BYTE	$01,$08
		.BYTE	$01,$02
		.BYTE	$00,$80
		.BYTE	$00,$20
		.BYTE	$00,$08
		.BYTE	$00,$02

; =============== S U B	R O U T	I N E =======================================
_game_state08_build:
		LDA		byte_5C
		BEQ		loc_136325
		JMP		_game_state_common
loc_136325:
		LDA		_constr_palette_selected_idx
		BNE		.other_builds
		JMP		_bulldoze_area
.other_builds:
		CMP		#$04
		BEQ		_build_garden_area				; =4 - gardens selected
		BCS		_bulid_large_buildings			; all above 4 - buildings
		LDA		_pad0_held						; here we start to build single tiled objects
		CMP		_pad0_prev_held					; areas - roads, rails, electrics
		BNE		.something_changed				; buttons changes, test if we need to do something
		LDA		_constr_cursor_tile._COL
		CMP		_prev_constr_cursor_tile._COL
		BNE		.something_changed				; as well as cursor position changed
		LDA		_constr_cursor_tile._ROW
		CMP		_prev_constr_cursor_tile._ROW
		BEQ		.no_changes
.something_changed:
		JSR		_get_cur_building_price
		JSR		_get_cur_constr_cursor_map_tile
		FJSRA	_get_valid_1X1_building_tile,PRG7,PRGD
		BCC		loc_136358
		JMP		.cant_build_anything_beep
loc_136358:
		JSR		_build_cur_constr_cursor_area
		JSR		_adjust_surround_net_tiles
		JSR		_calc_constr_area_update_params
		LDA		#$FF
		STA		_hud_money_redraw_req
		LDA		_constr_cursor_tile._COL
		STA		_prev_constr_cursor_tile._COL
		LDA		_constr_cursor_tile._ROW
;		JSR		_apu_se_small_build
		APUA_SE	_SE_IDX_SBUILD
.no_changes:
		JSR		_save_constr_cursor_pos
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_build_garden_area:
		LDA		_pad0_held
		CMP		_pad0_prev_held
		BNE		loc_136389
		LDA		_constr_cursor_tile._COL
		CMP		_prev_constr_cursor_tile._COL
		BNE		loc_136389
		LDA		_constr_cursor_tile._ROW
		CMP		_prev_constr_cursor_tile._ROW
		BEQ		.no_changes
loc_136389:
		JSR		_get_cur_building_price
		JSR		_get_cur_constr_cursor_map_tile
		FJSRA	_is_building_available_here,PRG7,PRGD
		BCS		.cant_build_anything_beep
		JSR		_build_cur_constr_cursor_area
		JSR		_adjust_surround_net_tiles
		JSR		_calc_constr_area_update_params
		LDA		#$FF
		STA		_hud_money_redraw_req
;		JSR		_apu_se_small_build
		APUA_SE	_SE_IDX_SBUILD
		JSR		_save_constr_cursor_pos
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_bulid_large_buildings:
		LDA		_pad0_autorep_press
		BPL		loc_1363F5
		LDA		_constr_palette_selected_idx
		CMP		#$0D
		BNE		loc_1363BE
		LDA		_constr_palette_flags
		AND		#$02
		BNE		.cant_build_anything_beep
loc_1363BE:
		JSR		_get_cur_building_price
		JSR		_get_cur_constr_cursor_map_tile
		FJSRA	_is_building_available_here,PRG7,PRGD
		BCS		.cant_build_anything_beep
		JSR		_build_cur_constr_cursor_area
		JSR		_adjust_surround_net_tiles
		JSR		_calc_constr_area_update_params
		LDA		#$FF
		STA		_hud_money_redraw_req
;		JSR		_apu_se_large_build
		APUA_SE	_SE_IDX_LBUILD
		JSR		_bank_build_year_init
		JSR		_present_remove_from_list
		LDA		_constr_palette_selected_idx
		CMP		#$0D
		BNE		loc_1363F5
		LDA		#$FF
		STA		_hud_present_name_draw_req
		BNE		loc_1363F5
.cant_build_anything_beep:
		JSR		_save_constr_cursor_pos
;		JSR		_apu_se_error
		APUA_SE	_SE_IDX_ERROR
loc_1363F5:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_bulldoze_area:
; REDUNDANT, this routine is actually redundant, because the bulldoze price
; is calculated later in _bulldoze_a_building routine and overwrites these ones
;
;		JSR		_get_cur_building_price			; bulldoze price ($1)
		JSR		_bulldoze_a_building
		BCS		.fail
		JSR		_adjust_surround_net_tilesCC
		JSR		sub_1509CF
		LDA		#$FF
		STA		_hud_money_redraw_req
		LDX		byte_CC
		DEX
		BEQ		.ok
		INC		_corruption_anim_flag
;		JSR		_apu_se_destroy
		APUA_SE	_SE_IDX_DESTROY
		JMP		_game_state_common
.ok:
;		JSR		_apu_se_bulldozer
		APUA_SE	_SE_IDX_BULLDOZE
		JMP		_game_state_common
.fail:
		LDA		byte_CD
		BNE		.skip							; with error sound or without
;		JSR		_apu_se_error
		APUA_SE	_SE_IDX_ERROR
.skip:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_save_constr_cursor_pos:
		LDA		_constr_cursor_tile._COL
		STA		_prev_constr_cursor_tile._COL
		LDA		_constr_cursor_tile._ROW
		STA		_prev_constr_cursor_tile._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_state04_constr_area_redraw:
		JSR		_constr_area_redraw
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state05_constr_palette_update:
		JSR		_constr_palette_update
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state06_game_hud_redraw:
;		JSR		_buffers_test_flush
;		BCS		loc_136472
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_136472
		LDA		_cur_map_wnd_pos._COL
		STA		_update_rect_left
		CLC
		ADC		#$18
		STA		_update_rect_right
		LDA		_cur_map_wnd_pos._ROW
		STA		_update_rect_top
		CLC
		ADC		#$14
		STA		_update_rect_bottom
		LDA		#$FF
		STA		_constr_area_update_req
		LDA		#$00
		STA		_hud_redraw_req
		LDA		#$FF
		STA		_RCI_meter_redraw_req
		STA		_hud_date_redraw_req
		STA		_hud_popul_redraw_req
		STA		_hud_city_name_redraw_req
		STA		_hud_money_redraw_req
		JSR		_seasonal_pal_load
loc_136472:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state0A_power_build_menu:
;		JSR		_buffers_test_flush
;		BCS		loc_136482
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_136482
		JSR		_power_build_menu
		LDA		#$00
		STA		_power_build_menu_req
loc_136482:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state0B_gui_ctrl_menu:
		JSR		_spr_clear
		LDA		#$00
		STA		_wnd_lvl_idx
		LDA		_gui_ctrl_menu_press_idx
		STA		_wnd_idx
		CMP		#$05
		BEQ		_game_msg_wnd_display
		CMP		#$06
		BNE		loc_1364A3
		JSR		_goto_start
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		JMP		_game_state_common
loc_1364A3:
		ASL
		TAX
		LDA		_city._menu_pos,X
		STA		_wnd_left_tile
		INX
		LDA		_city._menu_pos,X
		STA		_wnd_top_tile
		JSR		_gui_ctrl_menu_icon_push_draw
		JSR		_wnd_draw
		JSR		_gui_ctrl_menu_input
		BCS		loc_1364C9
		JSR		_wnd_erase
		JSR		_gui_ctrl_menu_icon_release_draw
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		JMP		_game_state_common
loc_1364C9:
		LDX		_wnd_idx
		CPX		#$03
		BEQ		loc_1364DA
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		JSR		_game_chr_common_reload
		JMP		_game_state_common
loc_1364DA:
		STA		_scr_res_idx
		LDA		#$02
		STA		_scr_mode_idx
		JSR		_pal_fade_out
		FJSRA	_scr_res_common,SRAM,PRG9
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		JSR		_game_chr_common_reload
		JSR		_game_hud_full_draw
		JSR		_scroll_msg_reset
		LDA		#$00
		STA		_scr_mode_idx
		STA		_screen_pal_cur_spr_idx
		JSR		_seasonal_pal_load
		JSR		_pal_fade_in
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state13_msg_wnd_display:
		INC		_game_msg_wnd_active_flag

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_display:
		JSR		_game_msg_wnd_pos_calc
		JSR		_wait_for_nmi
		LDA		#$05
		STA		_gui_ctrl_menu_press_idx
		JSR		_gui_ctrl_menu_icon_push_draw
		FJSRA	_game_msg_handler,PRG4,PRG3
		JSR		_gui_ctrl_menu_icon_release_draw
		JSR		_scroll_msg_reset
		LDA		_debug_scroll_msg_flags
		CMP		#$03
		BNE		loc_136530
		STA		_scroll_msg_active_flag
loc_136530:
		LDA		#$00
		STA		_game_msg_wnd_active_flag
		STA		_game_msg_awaits_flag
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_msg_wnd_pos_calc:
		LDA		_cur_screen_tile._ROW
		LSR
		CLC
		ADC		#$1B
		CMP		#$1E
		BCC		loc_1500BA
		SBC		#$1E
loc_1500BA:
		LDX		#$20
		MULXA
		STA		_ptr0
		TXA
		CLC
		ADC		#$20
		STA		_ptr0+1
		PPUQSTART
		PPUQMOVB	_ptr0,OP_NOP,0
		PPUQMOVB	_ptr0+1,OP_NOP,0
		LDA		#$20
		TAY
		PPUQMOVA
		LDA		#$DF
loc_1500E7:
		PPUQMOVA
		DEY
		BNE		loc_1500E7
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_state0C_RCI_meter_redraw:
;		JSR		_buffers_test_flush
;		BCS		loc_136548
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_136548
		JSR		_RCI_meters_draw
		LDA		#$00
		STA		_RCI_meter_redraw_req
loc_136548:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state0D_hud_date_redraw:
;		JSR		_buffers_test_flush
;		BCS		loc_13655C
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_13655C
;		FJSRA	_hud_date_draw,PRG7,PRG3
		JSR		_hud_date_draw
		LDA		#$00
		STA		_hud_date_redraw_req
loc_13655C:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state0E:
;		JSR		_buffers_test_flush
;		BCS		loc_136582
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_136582
		INC		_map_bg_anim_counter			; increase animation counter for
		LDA		_cur_map_wnd_pos._COL			; map bg objects
		STA		_update_rect_left
		CLC
		ADC		#$18
		STA		_update_rect_right
		LDA		_cur_map_wnd_pos._ROW
		STA		_update_rect_top
		CLC
		ADC		#$14
		STA		_update_rect_bottom
		LDA		#$FF
		STA		_constr_area_update_req
		LDA		#$00
		STA		_game_core_stop_flag
loc_136582:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state0F_hud_money_redraw:
;		JSR		_buffers_test_flush
;		BCS		loc_136595
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_136595
;		FJSRA	_hud_money_draw,PRGC,PRG3
		JSR		_hud_money_draw
		LDA		#$00
		STA		_hud_money_redraw_req
loc_136595:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state10_fiscal_year_draw:
;		JSR		_buffers_test_flush
;		BCS		loc_1365C8
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_1365C8
		LDA		#$02
		STA		_scr_res_idx
		STA		_scr_mode_idx
		JSR		_pal_fade_out
		FJSRA	_scr_res_common,SRAM,PRG9
		JSR		_game_hud_full_draw
		LDA		#$00
		STA		_scr_mode_idx
		STA		_screen_pal_cur_spr_idx
		JSR		_seasonal_pal_load
		JSR		_pal_fade_in
		JSR		_scroll_msg_reset
		LDA		#$00
		STA		_budget_scr_draw_req
		STA		_cur_game_cursor_area
loc_1365C8:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state11_hud_popul_redraw:
;		JSR		_buffers_test_flush
;		BCS		loc_1365DC
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_1365DC
;		FJSRA	_hud_popul_draw,PRG7,PRG3
		JSR		_hud_popul_draw
		LDA		#$00
		STA		_hud_popul_redraw_req
loc_1365DC:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state12_airport_build_menu:
;		JSR		_buffers_test_flush
;		BCS		loc_1365EC
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_1365EC
		JSR		_airport_build_menu
		LDA		#$00
		STA		_airport_build_menu_req
loc_1365EC:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state16_hud_city_name_redraw:
;		JSR		_buffers_test_flush
;		BCS		loc_136600
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_136600
;		FJSRA	_hud_city_name_draw,PRG7,PRG3
		JSR		_hud_city_name_draw
		LDA		#$00
		STA		_hud_city_name_redraw_req
loc_136600:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state17_common:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_states_select:
		LDA		_budget_scr_draw_req
		BEQ		loc_136610
		LDA		#$10
		JMP		_set_game_state_idx
loc_136610:
		LDA		_scr_mode_idx					; full map scroll mode request
		CMP		#$01
		BNE		loc_13661B
		LDA		#$02
		JMP		_set_game_state_idx
loc_13661B:
		LDA		_game_msg_wnd_active_flag
		BEQ		loc_136625
		LDA		#$0B
		JMP		_set_game_state_idx
loc_136625:
		LDA		byte_5C
		BEQ		loc_13662E
		LDA		#$04
		JMP		_set_game_state_idx
loc_13662E:
		LDA		_hud_static_redraw_req			; keep state of hud redraw till the end
		BEQ		loc_136637
		LDA		#$00
		JMP		_set_game_state_idx
loc_136637:
		LDA		_constr_palette_redraw_req		; also keep state of pal redraw till the end
		BEQ		loc_136640
		LDA		#$01
		JMP		_set_game_state_idx				; full redraw constr palette frame
loc_136640:
		LDA		_gui_active_controls_flags		; check gui flags active
		AND		#$03
		CMP		#$03
		BEQ		loc_13665D						; both flags are set, then gui drawn, skip
		LSR
		BCS		loc_136654						; bit0 clear if static hud need to be redrawn
		LDA		#$FF
		STA		_hud_static_redraw_req			; set the static hud redraw flag
		LDA		#$00
		JMP		_set_game_state_idx				; also set the state for redraw already
loc_136654:
		LDA		#$FF
		STA		_constr_palette_redraw_req		; bit 1 clear, then constr paletter full redraw request set
		LDA		#$01
		JMP		_set_game_state_idx				; also state of the game updated
loc_13665D:
		LDA		_constr_palette_update_req		; when constr paletter redrawn, need to update active icons
		BEQ		loc_136666
		LDA		#$05
		JMP		_set_game_state_idx				; update the states of const palette buttons
loc_136666:
		LDA		_power_build_menu_req
		BEQ		loc_136670
		LDA		#$0A
		JMP		_set_game_state_idx
loc_136670:
		LDA		_airport_build_menu_req
		BEQ		loc_13667A
		LDA		#$12
		JMP		_set_game_state_idx
loc_13667A:
		LDA		_presents_build_menu_req
		BEQ		loc_136684
		LDA		#$14
		JMP		_set_game_state_idx
loc_136684:
		LDA		_game_msg_awaits_flag
		BEQ		loc_13668E
		LDA		#$13
		JMP		_set_game_state_idx
loc_13668E:
		LDA		_pad0_autorep_press
		AND		#$30
		BEQ		loc_136699
		LDA		#$15
		JMP		_set_game_state_idx
loc_136699:
		LDA		_hud_city_name_redraw_req
		BEQ		loc_1366A3
		LDA		#$16
		JMP		_set_game_state_idx
loc_1366A3:
		LDA		_hud_money_redraw_req
		BEQ		loc_1366AC
		LDA		#$0F
		JMP		_set_game_state_idx
loc_1366AC:
		LDA		_hud_date_redraw_req
		BEQ		loc_1366B6
		LDA		#$0D
		JMP		_set_game_state_idx
loc_1366B6:
		LDA		_hud_popul_redraw_req
		BEQ		loc_1366C0
		LDA		#$11
		JMP		_set_game_state_idx
loc_1366C0:
		LDA		_hud_present_name_draw_req
		BEQ		loc_1366C9
		LDA		#$09
		JMP		_set_game_state_idx
loc_1366C9:
		LDA		_pad0_held
		AND		#$40
		BEQ		loc_1366D4
		LDA		#$03
		JMP		_set_game_state_idx
loc_1366D4:
		LDA		_pad0_autorep_press
		AND		#$80
		BEQ		loc_1366E4
		JSR		_scr_push_area_test
		BCS		loc_1366E4
		LDA		#$07
		JMP		_set_game_state_idx
loc_1366E4:
		LDA		_pad0_autorep_release			; push A button action in map window
		AND		#$80
		BEQ		loc_1366F3
		LDA		_cur_game_cursor_area
		BEQ		loc_1366F3
		LDA		#$08
		JMP		_set_game_state_idx
loc_1366F3:
		LDA		_constr_area_update_req
		BEQ		loc_1366FC
		LDA		#$04
		JMP		_set_game_state_idx
loc_1366FC:
		LDA		_RCI_meter_redraw_req
		BEQ		loc_136706
		LDA		#$0C
		JMP		_set_game_state_idx
loc_136706:
		LDA		_hud_redraw_req
		BEQ		loc_136710
		LDA		#$06
		JMP		_set_game_state_idx
loc_136710:
		LDA		_game_core_stop_flag
		BEQ		loc_13671A
		LDA		#$0E
		JMP		_set_game_state_idx
loc_13671A:
		LDA		#$17
_set_game_state_idx:
		STA		_game_state_idx
		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_palette_redraw:
;		JSR		_buffers_test_flush
;		BCS		locret_136745
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		locret_136745
		JSR		_constr_palette_area_pos_calc
		JSR		_constr_palette_static_nt_draw
		JSR		_constr_palette_static_extnt_draw
		LDA		#$00
		STA		_constr_palette_redraw_req
		LDA		_cur_map_wnd_pos._COL
		STA		_update_rect_left
		CLC
		ADC		#$18
		STA		_update_rect_right
		LDA		_cur_map_wnd_pos._ROW
		STA		_update_rect_top
		CLC
		ADC		#$14
		STA		_update_rect_bottom
		LDA		#$FF
		STA		_constr_area_update_req
		STA		_constr_palette_update_req
;		JSR		_constr_palette_update_req_set	; REDUNDANT
		CLC
		RTS
locret_136745:
		SEC										; FIX, flush wait optimize workaround
		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_palette_static_nt_draw:
		LDY		#$00
;		LDX		_ppu_queue_pos_tail				; BUG, load pos TAIL instead of head!
		PPUQSTART								; FIX, does not make any difference because
		MOVW	_ptr0,byte_7A					; we have both values the same at this point
		MOVW	_tmp2,byte_7E
loc_136A2D:
		PPUQMOVB	_ptr0,OP_NOP,0
		PPUQMOVB	_ptr0+1,OP_NOP,0
		LDA		byte_7C
		STA		_tmp4
		ORA		#$80							; vertical draw flag set
		PPUQMOVA
		LDA		byte_7D
		STA		_tmp5
loc_136A47:
		LDA		_tlm_constr_palette_static_nt,Y	; transposed construction palette nt
		INY
		PPUQMOVA
		DEC		_tmp4
		BNE		loc_136A47
		LDA		_tmp5
		BEQ		loc_136A75						; test for wrap
		PPUQMOVB	_ptr0,OP_AND,#$1F			; do wrap vertical
		PPUQMOVB	#$20,OP_NOP,0
		LDA		_tmp5
		STA		_tmp4
		ORA		#$80
		PPUQMOVA
		LDA		#$00
		STA		_tmp5
		BEQ		loc_136A47
loc_136A75:
		INC		_ptr0
		DEC		_tmp2
		BNE		loc_136A2D						; test for hor wrap
		DO_WRAP	_tmp2,_tmp3,loc_136A90			; do hor wrap
		DEC		_ptr0
		LDA		_ptr0
		AND		#$E0
		STA		_ptr0
		JMP		loc_136A2D
loc_136A90:
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_palette_static_extnt_draw:
		LDY		#$00
;		LDX		_extnt_queue_pos_tail			; BUG, again load TAIL! why!?
		EXTQSTART
		LDA		byte_7A							; reload ppu offset but with
		STA		_ptr0							; adjust to EXTNT RAM
		LDA		byte_7B
		CLC
		ADC		#$3C
		STA		_ptr0+1
		MOVW	_tmp2,byte_7E
loc_136AAA:
		EXTQMOVB	_ptr0,OP_NOP,0				; the same vertical mode
		EXTQMOVB	_ptr0+1,OP_NOP,0
		LDA		byte_7C
		STA		_tmp4
		ORA		#$80
		EXTQMOVA
		LDA		byte_7D
		STA		_tmp5
loc_136AC4:
		LDA		_tlm_constr_palette_static_extnt,Y
		INY
		EXTQMOVA
		DEC		_tmp4
		BNE		loc_136AC4
		LDA		_tmp5
		BEQ		loc_136AF2
		EXTQMOVB	_ptr0,OP_AND,#$1F
		EXTQMOVB	#>[_MMC5_EXRAM],OP_NOP,0
		LDA		_tmp5
		STA		_tmp4
		ORA		#$80
		EXTQMOVA
		LDA		#$00
		STA		_tmp5
		BEQ		loc_136AC4
loc_136AF2:
		INC		_ptr0
		DEC		_tmp2
		BNE		loc_136AAA
		DO_WRAP	_tmp2,_tmp3,loc_136B0D
		DEC		_ptr0
		LDA		_ptr0
		AND		#$E0
		STA		_ptr0
		JMP		loc_136AAA
loc_136B0D:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_scroll_left_redraw:
;		JSR		_buffers_test_flush
;		BCS		locret_136759
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		locret_136759
		JSR		_constr_palette_area_pos_calc
		JSR		_map_scroll_left_draw
		CLC
		RTS
locret_136759:
		SEC										; FIX, flush wait optimize workaround
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_scroll_left_draw:
		PPUQSTART								; backup PPU queue positions to tmp vars
		STX		byte_80							; which we will use instead
		EXTQSTART
		STX		byte_81
		LDA		_cur_map_wnd_pos._COL			; calculate invalidate area
		STA		_update_rect_right				; on the city map and starting position
		SEC
		SBC		#$07
		STA		_update_rect_left
		LDA		_cur_map_wnd_pos._ROW
		STA		_update_rect_top
		CLC
		ADC		#$15
		STA		_update_rect_bottom
loc_136B2A:
		LDA		_update_rect_left
		CMP		_update_rect_right
		BEQ		loc_136B56
		STA		_tmp_tile_pos._COL
		LDA		_update_rect_top
		STA		_tmp_tile_pos._ROW
		JSR		_map_scroll_left_column_draw
		INC		_update_rect_left
		DEC		byte_7E
		BEQ		loc_136B43
		INC		byte_7A
		BNE		loc_136B2A
loc_136B43:
		LDA		byte_7A
		AND		#$E0
		STA		byte_7A
		DO_WRAP	byte_7E,byte_7F,loc_136B56
		JMP		loc_136B2A
loc_136B56:
		LDX		byte_80
		PPUQEND
		LDX		byte_81
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_scroll_left_column_draw:
		LDX		byte_80
		PPUQMOVB	byte_7A,OP_NOP,0
		PPUQMOVB	byte_7B,OP_NOP,0
		PPUQMOVB	byte_7C,OP_ORA,#$80
		STX		byte_80
		LDX		byte_81
		EXTQMOVB	byte_7A,OP_NOP,0
		EXTQMOVB	byte_7B,OP_ADD,#$3C
		LDA		byte_7C
		STA		byte_82
		ORA		#$80
		EXTQMOVA
		STX		byte_81
		LDA		byte_7D
		STA		byte_83
loc_136B98:
		SEC
		JSR		_map_get_ppu_tile
		LDX		byte_80
		PPUQMOVB	word_6C,OP_NOP,0
		STX		byte_80
		LDX		byte_81
		EXTQMOVB	word_6C+1,OP_NOP,0
		STX		byte_81
		INC		_tmp_tile_pos._ROW
		CLC
		DEC		byte_82
		BNE		loc_136B98
		DO_WRAP	byte_82,byte_83,locret_136BF8
		LDX		byte_80
		PPUQMOVB	byte_7A,OP_AND,#$1F
		PPUQMOVB	#$20,OP_NOP,0
		PPUQMOVB	byte_82,OP_ORA,#$80
		STX		byte_80
		LDX		byte_81
		EXTQMOVB	byte_7A,OP_AND,#$1F
		EXTQMOVB	#>[_MMC5_EXRAM],OP_NOP,0
		EXTQMOVB	byte_82,OP_ORA,#$80
		STX		byte_81
		JMP		loc_136B98
locret_136BF8:
		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_palette_area_pos_calc:
		LDA		_cur_screen_tile._ROW			; simplified version of CIADDB/WRAP
		LSR										; code
		LDX		#$20							; row unchanged
		MULXA
		STA		_ptr0
		LDA		_cur_screen_tile._COL			; calc redraw area
		LSR
		CLC
		ADC		_ptr0
		STA		byte_7A							; 7A/7B - ppu offset
		TXA
		ADC		#$20
		STA		byte_7B
		CMP		#$23
		BNE		loc_1369E2
		LDA		byte_7A
		CMP		#$C0
		BCC		loc_1369E2
		SBC		#$C0
		STA		byte_7A
		LDA		#$20
		STA		byte_7B
loc_1369E2:
		LDA		_cur_screen_tile._ROW			; 7C - height, 7D - height wrap
		LSR
		CMP		#$0A
		BCC		loc_1369F5
		SBC		#$09
		STA		byte_7D
		LDA		#$15
		SBC		byte_7D
		STA		byte_7C
		BCS		loc_1369FD
loc_1369F5:
		LDA		#$15
		STA		byte_7C
		LDA		#$00
		STA		byte_7D
loc_1369FD:
		LDA		byte_7A							; 7F - width, 7D - width wrap
		AND		#$1F
		CMP		#$1A
		BCC		loc_136A10
		SBC		#$19
		STA		byte_7F
		LDA		#$07
		SBC		byte_7F
		STA		byte_7E
		RTS
loc_136A10:
		LDA		#$07
		STA		byte_7E
		LDA		#$00
		STA		byte_7F
		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_static_redraw:
;		JSR		_buffers_test_flush
;		BCS		locret_136774
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		locret_136774
		JSR		_game_hud_area_pos_calc
		JSR		_hud_static_nt_draw
		JSR		_hud_static_extnt_draw
		LDA		#$00
		STA		_hud_static_redraw_req
		CLC
		RTS
locret_136774:
		SEC										; FIX, flush wait optimize workaround
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_scroll_top_redraw:
;		JSR		_buffers_test_flush				; REDUNDANT
;		BCS		locret_136780
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		locret_136780
		JSR		_game_hud_area_pos_calc
		JSR		_map_scroll_top_draw
		CLC
		RTS
locret_136780:
		SEC										; FIX, flush wait optimize workaround
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_hud_area_pos_calc:
		LDA		_cur_screen_tile._ROW			; the only case when CIADDB can't
		LSR										; be used, because here we subtract
		SEC										; the current pos with constant.
		SBC		#$07
		BCS		loc_136862
		ADC		#$1E
loc_136862:
		PHA										; store screen row pos for now
		LDX		#$20
		MULXA
		STA		_ptr0
		LDA		_cur_screen_tile._COL
		LSR
		CLC
		ADC		_ptr0
		STA		byte_7A							; 7A/7B now has a pointer to ppu
		TXA
		ADC		#$20
		STA		byte_7B
		CMP		#$23
		BNE		loc_136892						; wrap vertical ppu position to 23C0
		LDA		byte_7A
		CMP		#$C0
		BCC		loc_136892
		SBC		#$C0
		STA		byte_7A
		LDA		#$20
		STA		byte_7B
loc_136892:
		PLA
		CMP		#$18							; calc screen line width
		BCC		loc_1368A3						; and line wrap width
		SBC		#$17
		STA		byte_7D
		LDA		#$07
		SBC		byte_7D
		STA		byte_7C
		BCS		loc_1368AB
loc_1368A3:
		LDA		#$07							; or line without wrap by default
		STA		byte_7C
		LDA		#$00
		STA		byte_7D
loc_1368AB:
		LDA		byte_7A
		AND		#$1F
		EOR		#$FF
		SEC
		ADC		#$20
		CMP		#$20
		BCC		loc_1368BA
		LDA		#$00
loc_1368BA:
		STA		byte_7E
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_scroll_top_draw:
		LDA		_ppu_queue_pos_head
		STA		_tmp544
		LDA		_extnt_queue_pos_head
		STA		_tmp545
		LDA		_cur_map_wnd_pos._COL
		SEC
		SBC		#$07
		STA		_tmp542
		LDA		_cur_map_wnd_pos._ROW
		SEC
		SBC		#$07
		STA		_tmp543
loc_13679B:
		JSR		sub_1367D3
		INC		_tmp543
		LDA		byte_7A
		CLC
		ADC		#$20
		STA		byte_7A
		LDA		byte_7B
		ADC		#$00
		STA		byte_7B
		DEC		byte_7C
		BNE		loc_13679B
		LDA		byte_7D
		BEQ		loc_1367C8
		STA		byte_7C
		LDA		byte_7A
		AND		#$1F
		STA		byte_7A
		LDA		#$20
		STA		byte_7B
		LDA		#$00
		STA		byte_7D
		BEQ		loc_13679B
loc_1367C8:
		LDA		_tmp544
		LDX		_tmp545
		STA		_ppu_queue_pos_head
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_1367D3:
		LDX		_tmp544
		PPUQMOVB	byte_7A,OP_AND,#$E0
		PPUQMOVB	byte_7B,OP_NOP,0
		PPUQMOVB	#$20,OP_NOP,0
		STX		_tmp544
		LDX		_tmp545
		EXTQMOVB	byte_7A,OP_AND,#$E0
		EXTQMOVB	byte_7B,OP_ADD,#$3C
		EXTQMOVB	#$20,OP_NOP,0
		STX		_tmp545
		STA		_tmp546
		LDA		_tmp542
		CLC
		ADC		byte_7E
		STA		_tmp_tile_pos._COL
		LDA		_tmp543
		STA		_tmp_tile_pos._ROW
		LDA		#$20
		SEC
		SBC		byte_7E
		STA		_tmp547
		SEC
loc_136823:
		JSR		_map_get_ppu_tile
		LDX		_tmp544
		PPUQMOVB	word_6C,OP_NOP,0
		STX		_tmp544
		LDX		_tmp545
		EXTQMOVB	word_6C+1,OP_NOP,0
		STX		_tmp545
		DEC		_tmp546
		BEQ		locret_136857
		INC		_tmp_tile_pos._COL
		DEC		_tmp547
		BNE		loc_136823
		LDA		#$20
		STA		_tmp547
		LDA		_tmp542
		STA		_tmp_tile_pos._COL
		SEC
		BCS		loc_136823
locret_136857:
		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_static_nt_draw:
		MOVWO	_ptr0,_tlm_hud_static_nt
		LDA		byte_7A
		AND		#$E0
		STA		_tmp2
		LDA		byte_7B
		STA		_tmp3
		LDA		byte_7C
		STA		_tmp4
		LDA		byte_7D
		STA		_tmp5
		PPUQSTART
		LDY		byte_7E
loc_1368DB:
		PPUQMOVB	_tmp2,OP_NOP,0
		PPUQMOVB	_tmp3,OP_NOP,0
		PPUQMOVB	#$20,OP_NOP,0
		STA		_tmp6
loc_1368EF:
		LDA		(_ptr0),Y
		PPUQMOVA
		INY
		CPY		#$20
		BCC		loc_1368FC
		LDY		#$00
loc_1368FC:
		DEC		_tmp6
		BNE		loc_1368EF
		ADDWB	_ptr0,#$20
		DEC		_tmp4
		BEQ		loc_136920
		ADDWB	_tmp2,#$20
		BNE		loc_1368DB
loc_136920:
		DO_WRAP	_tmp4,_tmp5,loc_136932
		STA		_tmp2
		LDA		#$20
		STA		_tmp3
		BNE		loc_1368DB
loc_136932:
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_hud_static_extnt_draw:
		MOVWO	_ptr0,_tlm_hud_static_extnt
		LDA		byte_7A
		AND		#$E0
		STA		_tmp2
		LDA		byte_7B
		CLC
		ADC		#$3C
		STA		_tmp3
		LDA		byte_7C
		STA		_tmp4
		LDA		byte_7D
		STA		_tmp5
		EXTQSTART
		LDY		byte_7E
loc_136956:
		EXTQMOVB	_tmp2,OP_NOP,0
		EXTQMOVB	_tmp3,OP_NOP,0
		EXTQMOVB	#$20,OP_NOP,0
		STA		_tmp6
loc_13696A:
		LDA		(_ptr0),Y
		EXTQMOVA
		INY
		CPY		#$20
		BCC		loc_136977
		LDY		#$00
loc_136977:
		DEC		_tmp6
		BNE		loc_13696A
		ADDWB	_ptr0,#$20
		DEC		_tmp4
		BEQ		loc_13699B
		ADDWB	_tmp2,#$20
		BNE		loc_136956
loc_13699B:
		DO_WRAP	_tmp4,_tmp5,loc_1369AD
		STA		_tmp2
		LDA		#>[_MMC5_EXRAM]
		STA		_tmp3
		BNE		loc_136956
loc_1369AD:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_building_name_and_cost_hud_draw:
		PUSHB	_prg_prgA000_shadow
		LDA		#$F2
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		_ppu_queue_pos_head				; backup queue buffers heads
		STA		_ptr0
		LDA		_extnt_queue_pos_head
		STA		_ptr0+1
		JSR		_building_name_and_cost_hud_pos_calc	; calc relative nt pos
		LDA		_constr_palette_selected_idx	; check for additional
		CMP		#$0B							; buildings
		BNE		loc_136C1D
		LDX		_cur_power_type_select_idx		; if power plant selected
		BEQ		loc_136C3A						; choose one of 0B or 0E
		CLC
		ADC		#$03
		BCC		loc_136C3A
loc_136C1D:
		CMP		#$0C
		BNE		loc_136C28
		LDX		_cur_port_type_select_idx		; if ports selected
		BEQ		loc_136C3A						; choose 0C or 0F
		CLC
		ADC		#$03
loc_136C28:
;		CMP		#$0D							; REDUNDANT
;		BNE		loc_136C3A						;
;		LDX		_cur_present_select_idx			; presents have the same
;		LDY		_city._presents_list,X			; name 0D, empty slot should
;		DEY										; display "NoPresent", but
;		BPL		loc_136C37						; it is not possible to select it
;		LDY		#$0F							; so it goes unused as well as
;loc_136C37:									; the whole this code is redundant
;		LDA		_present_names_length_list,Y
loc_136C3A:
		LDX		#$0C
		MULXA
		STA		_building_name_and_cost_hud_src_ofs
		JSR		_building_name_and_cost_hud_nt_queue
		JSR		_building_name_and_cost_hud_extnt_queue
		LDA		_ptr0							; resore ppu queue buffers
		STA		_ppu_queue_pos_head
		LDA		_ptr0+1
		STA		_extnt_queue_pos_head
		POPB	_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		RTS
; REDUNDANT
;_present_names_length_list:
;		.BYTE	$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$0D,$10

; =============== S U B	R O U T	I N E =======================================
_building_name_and_cost_hud_pos_calc:
		LDA		_cur_screen_tile._ROW
		LSR
		SEC
		SBC		#$01
		BCS		loc_136C79
		ADC		#$1E
loc_136C79:
		LDX		#$20
		MULXA
		STA		byte_7A
		TXA
		CLC
		ADC		#$20
		STA		byte_7B
		LDA		_cur_screen_tile._COL
		LSR
		CLC
		ADC		#$12
		AND		#$1F
		PHA
		CLC
		ADC		byte_7A
		STA		byte_7A
		PLA
		CMP		#$15
		BCC		loc_136CAF
		SEC
		SBC		#$14
		STA		byte_7F
		LDA		#$0C
		SEC
		SBC		byte_7F
		STA		byte_7E
		RTS
loc_136CAF:
		LDA		#$0C
		STA		byte_7E
		LDA		#$00
		STA		byte_7F
		RTS

; =============== S U B	R O U T	I N E =======================================
_building_name_and_cost_hud_nt_queue:
		LDX		_ptr0
		LDA		byte_7E
		STA		_tmp2
		LDA		byte_7F
		STA		_tmp3
		PPUQMOVB	byte_7A,OP_NOP,0
		PPUQMOVB	byte_7B,OP_NOP,0
		PPUQMOVB	_tmp2,OP_NOP,0
		LDY		_building_name_and_cost_hud_src_ofs
loc_136CD6:
		LDA		_tlm_nt_building_names_lib,Y
		PPUQMOVA
		INY
		DEC		_tmp2
		BNE		loc_136CD6
		DO_WRAP	_tmp2,_tmp3,loc_136D03
		PPUQMOVB	byte_7A,OP_AND,#$E0
		PPUQMOVB	byte_7B,OP_NOP,0
		PPUQMOVB	_tmp2,OP_NOP,0
		JMP		loc_136CD6
loc_136D03:
		STX		_ptr0
		RTS

; =============== S U B	R O U T	I N E =======================================
_building_name_and_cost_hud_extnt_queue:
		LDX		_ptr0+1
		EXTQMOVB	byte_7A,OP_NOP,0
		EXTQMOVB	byte_7B,OP_ADD,#$3C
		EXTQMOVB	byte_7E,OP_NOP,0
		LDY		_building_name_and_cost_hud_src_ofs
loc_136D1F:
		LDA		_tlm_extnt_building_names_lib,Y
		EXTQMOVA
		INY
		DEC		byte_7E
		BNE		loc_136D1F
		DO_WRAP	byte_7E,byte_7F,loc_136D4F
		EXTQMOVB	byte_7A,OP_AND,#$E0
		EXTQMOVB	byte_7B,OP_ADD,#$3C
		EXTQMOVB	byte_7E,OP_NOP,0
		JMP		loc_136D1F
loc_136D4F:
		STX		_ptr0+1
		RTS

; =============== S U B	R O U T	I N E =======================================
_airport_build_menu:
		JSR		_spr_clear
		JSR		_airport_build_wnd_draw
;		LDA		#$FF							; REDUNDANT, never read!
;		STA		byte_37
		JSR		_building_name_and_cost_hud_draw
loc_136D5F:
		JSR		_wait_for_nmi
		JSR		_spr_clear
		LDA		_cur_port_type_select_idx
		ASL
		ASL
		ASL
		ASL
		STA		_ptr0
		ASL
		CLC
		ADC		_ptr0
		ADC		#$3B
		STA		_spr_insert_args._pos._COL
		LDA		#$C2
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._idx
		JSR		_spr_lib_attr_insert
		LDA		_pad0_autorep_press
		AND		#$0F
		BNE		loc_136D8E
		LDA		_pad0_autorep_press
		AND		#$C0
		BEQ		loc_136D5F
		BNE		loc_136DAF
loc_136D8E:
;		JSR		_apu_se_item_select
		APUA_SE	_SE_IDX_SELECT
		LDA		_cur_port_type_select_idx
		BEQ		loc_136DA3
		LDA		#$03
		STA		_cur_selected_buildings_size
		LDA		#$00
		STA		_cur_port_type_select_idx
		JSR		_building_name_and_cost_hud_draw
		JMP		loc_136D5F
loc_136DA3:
		LDA		#$04
		STA		_cur_selected_buildings_size
		INC		_cur_port_type_select_idx
		JSR		_building_name_and_cost_hud_draw
		JMP		loc_136D5F
loc_136DAF:
		JSR		_spr_clear
		JMP		_wnd_erase


; =============== S U B	R O U T	I N E =======================================
_airport_build_wnd_draw:
		LDA		#$00
		STA		_wnd_lvl_idx
		LDA		#$0A
		STA		_wnd_idx
		LDA		#$04
		STA		_wnd_left_tile
		LDA		#$10
		STA		_wnd_top_tile
		JMP		_wnd_draw

; =============== S U B	R O U T	I N E =======================================
_power_build_menu:
		JSR		_spr_clear
		JSR		_power_build_wnd_draw
;		LDA		#$FF							; REDUNDANT, never read!
;		STA		byte_37
		JSR		_building_name_and_cost_hud_draw
loc_136DD6:
		JSR		_wait_for_nmi
		JSR		_spr_clear
		LDA		_cur_power_type_select_idx
		ASL
		ASL
		ASL
		ASL
		STA		_ptr0
		ASL
		CLC
		ADC		_ptr0
		ADC		#$4B
		STA		_spr_insert_args._pos._COL
		LDA		#$B2
		STA		_spr_insert_args._pos._ROW
		LDA		#$00
		STA		_spr_insert_args._idx
		JSR		_spr_lib_attr_insert
		LDA		_pad0_autorep_press
		AND		#$0F
		BNE		loc_136E05
		LDA		_pad0_autorep_press
		AND		#$C0
		BEQ		loc_136DD6
		BNE		loc_136E1E
loc_136E05:
;		JSR		_apu_se_item_select
		APUA_SE	_SE_IDX_SELECT
		LDA		_cur_power_type_select_idx
		BEQ		loc_136E16
		LDA		#$00
		STA		_cur_power_type_select_idx
		JSR		_building_name_and_cost_hud_draw
		JMP		loc_136DD6
loc_136E16:
		INC		_cur_power_type_select_idx
		JSR		_building_name_and_cost_hud_draw
		JMP		loc_136DD6
loc_136E1E:
		JSR		_spr_clear
		JMP		_wnd_erase


; =============== S U B	R O U T	I N E =======================================
_power_build_wnd_draw:
		LDA		#$00
		STA		_wnd_lvl_idx
		LDA		#$06
		STA		_wnd_idx
		STA		_wnd_left_tile
		LDA		#$0E
		STA		_wnd_top_tile
		JMP		_wnd_draw

; =============== S U B	R O U T	I N E =======================================
;_unref_0:
;		LDA		_cur_screen_tile._ROW
;		LSR
;		STA		_ptr0
;		CLC
;		ADC		#$0A
;		CMP		#$1E
;		BCC		loc_136E44
;		SBC		#$1E
;loc_136E44:
;		LDX		#$20
;		MUXA
;		STA		byte_7A
;		TXA
;		CLC
;		ADC		#$20
;		STA		byte_7B
;		LDA		_cur_screen_tile._COL
;		LSR
;		STA		_ptr0+1
;		CLC
;		ADC		#$07
;		CMP		#$20
;		BCC		loc_136E68
;		SBC		#$20
;loc_136E68:
;		CLC
;		ADC		byte_7A
;		STA		byte_7A
;		AND		#$1F
;		CMP		#$17
;		BCC		loc_136E81
;		SBC		#$16
;		STA		byte_7F
;		LDA		#$0A
;		SEC
;		SBC		byte_7F
;		STA		byte_7E
;		JMP		loc_136E89
;loc_136E81:
;		LDA		#$0A
;		STA		byte_7E
;		LDA		#$00
;		STA		byte_7F
;loc_136E89:
;		LDA		_ptr0
;		CMP		#$11
;		BCC		loc_136EA0
;		CMP		#$14
;		BCS		loc_136EA0
;		SEC
;		SBC		#$10
;		STA		byte_7D
;		LDA		#$04
;		SEC
;		SBC		byte_7D
;		STA		byte_7C
;		RTS
;loc_136EA0:
;		LDA		#$04
;		STA		byte_7C
;		LDA		#$00
;		STA		byte_7D
;		RTS

; =============== S U B	R O U T	I N E =======================================
;_unref_1:
;		LDA		byte_7A
;		PHA
;		LDA		byte_7B
;		PHA
;		LDX		_ptr0
;		LDY		#$00
;		LDA		byte_7C
;		STA		_tmp2
;		LDA		byte_7D
;		STA		_tmp3
;loc_136EBB:
;		LDA		byte_7E
;		STA		_tmp4
;		LDA		byte_7F
;		STA		_tmp5
;		LDA		byte_7A
;		STA		_ppu_queue_buf,X
;		INX
;		LDA		byte_7B
;		STA		_ppu_queue_buf,X
;		INX
;		LDA		_tmp4
;		STA		_ppu_queue_buf,X
;		INX
;loc_136ED5:
;		LDA		byte_136FB3,Y
;		STA		_ppu_queue_buf,X
;		INX
;		INY
;		DEC		_tmp4
;		BNE		loc_136ED5
;		LDA		_tmp5
;		BEQ		loc_136F02
;		STA		_tmp4
;		LDA		#$00
;		STA		_tmp5
;		LDA		byte_7A
;		AND		#$E0
;		STA		_ppu_queue_buf,X
;		INX
;		LDA		byte_7B
;		STA		_ppu_queue_buf,X
;		INX
;		LDA		_tmp4
;		STA		_ppu_queue_buf,X
;		INX
;		JMP		loc_136ED5
;loc_136F02:
;		LDA		byte_7A
;		CLC
;		ADC		#$20
;		STA		byte_7A
;		LDA		byte_7B
;		ADC		#$00
;		STA		byte_7B
;		DEC		_tmp2
;		BNE		loc_136EBB
;		LDA		_tmp3
;		BEQ		loc_136F29
;		STA		_tmp2
;		LDA		#$00
;		STA		_tmp3
;		LDA		byte_7A
;		AND		#$1F
;		STA		byte_7A
;		LDA		#$20
;		STA		byte_7B
;		BNE		loc_136EBB
;loc_136F29:
;		STX		_ptr0
;		PLA
;		STA		byte_7B
;		PLA
;		STA		byte_7A
;		RTS

; =============== S U B	R O U T	I N E =======================================
sub_136F32:
		LDX		_ptr0+1
		LDA		byte_7B
		CLC
		ADC		#$3C
		STA		byte_7B
		LDA		byte_7C
		STA		_tmp2
		LDA		byte_7D
		STA		_tmp3
loc_136F43:
		LDA		byte_7E
		STA		_tmp4
		LDA		byte_7F
		STA		_tmp5
		EXTQMOVB	byte_7A,OP_NOP,0
loc_136F5C:
		EXTQMOVB	byte_7B,OP_NOP,0
		EXTQMOVB	_tmp4,OP_NOP,0
loc_136F5D:
		EXTQMOVB	#$41,OP_NOP,0
		INY
		DEC		_tmp4
		BNE		loc_136F5D
		DO_WRAP	_tmp4,_tmp5,loc_136F89
		EXTQMOVB	byte_7A,OP_AND,#$E0
		JMP		loc_136F5C
loc_136F89:
		ADDWB	byte_7A,#$20
		DEC		_tmp2
		BNE		loc_136F43
		DO_WRAP	_tmp2,_tmp3,loc_136FB0
		LDA		byte_7A
		AND		#$1F
		STA		byte_7A
		LDA		#>[_MMC5_EXRAM]
		STA		byte_7B
		BNE		loc_136F43
loc_136FB0:
		STX		_ptr0+1
		RTS
byte_136FB3:
		.BYTE	$70,$74,$74,$74,$74,$74,$74,$74,$74,$76
		.BYTE	$71,$DC,$79,$7A,$7B,$7C,$DC,$DC,$DC,$77
		.BYTE	$72,$DC,$7D,$7E,$79,$7C,$7F,$7B,$80,$77
		.BYTE	$73,$75,$75,$75,$75,$75,$75,$75,$75,$78

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_input:
		LDA		#$36
		STA		_MMC5_CHR_BANKSA+3
		JSR		_wnd_rect_calc
		LDA		_gui_ctrl_menu_press_idx
		CMP		#$03
		BNE		loc_136FF3
		LDA		_cur_wnd_pos_backup._COL
		STA		_cur_wnd_pos._COL
		LDA		_cur_wnd_pos_backup._ROW
		STA		_cur_wnd_pos._ROW
		BNE		loc_136FFB
loc_136FF3:
		LDA		_wnd_left_pos
		STA		_cur_wnd_pos._COL
		LDA		_wnd_top_pos
		STA		_cur_wnd_pos._ROW
loc_136FFB:
		JSR		_wait_for_nmi
		JSR		_gui_ctrl_menu_spr_draw
		LDA		_gui_ctrl_menu_press_idx
		CMP		#$03
		BNE		loc_13700F
		LDA		_cur_wnd_pos._COL
		STA		_cur_wnd_pos_backup._COL
		LDA		_cur_wnd_pos._ROW
		STA		_cur_wnd_pos_backup._ROW
loc_13700F:
		LDA		_pad0_held
		AND		#$80
		BNE		loc_137018
		JSR		_gui_ctrl_menu_redraw
loc_137018:
		LDA		_pad0_held
		BEQ		loc_13707F
		LDA		_pad0_autorep_press
		BEQ		loc_137064
		AND		#$80
		BEQ		loc_13704E
		LDA		_cur_wnd_pos._ROW
		LSR
		LSR
		LSR
		CMP		_wnd_top_tile
		BEQ		loc_137073
		JSR		sub_137291
		BCC		loc_13707F
		PHA
		LDA		_gui_ctrl_menu_press_idx
		CMP		#$04
		BEQ		loc_13704A
		JSR		_spr_clear
		JSR		_wait_for_nmi
		JSR		_game_chr_common_reload
		LDA		#$00
		STA		_tmp514
		PLA
		SEC
		RTS
loc_13704A:
		PLA
		JMP		loc_137054
loc_13704E:
		LDA		_pad0_autorep_press
		AND		#$40
		BEQ		loc_137064
loc_137054:
		JSR		_spr_clear
		JSR		_wait_for_nmi
		JSR		_game_chr_common_reload
		LDA		#$00
		STA		_tmp514
		CLC
		RTS
loc_137064:
		LDA		_pad0_held
		AND		#$80
		BEQ		loc_13707F
		LDA		_cur_wnd_pos._ROW
		LSR
		LSR
		LSR
		CMP		_wnd_top_tile
		BNE		loc_13707F
loc_137073:
		LDA		_cur_wnd_pos._COL
		LSR
		LSR
		LSR
		CMP		_wnd_left_tile
		BNE		loc_13707F
		JSR		_menu_wnd_redraw_flag_set
loc_13707F:
		JSR		sub_13738F
		JSR		_spr_finish
		JMP		loc_136FFB

; =============== S U B	R O U T	I N E =======================================
_wnd_rect_calc:
		LDA		_wnd_left_tile
		ASL
		ASL
		ASL
		STA		_wnd_left_pos
		LDA		_wnd_top_tile
		ASL
		ASL
		ASL
		STA		_wnd_top_pos
		LDA		_wnd_width_tile
		ASL
		ASL
		ASL
		CLC
		ADC		_wnd_left_pos
		BCC		loc_1370A2
		LDA		#$FF
loc_1370A2:
		STA		_wnd_right_pos
		LDA		_wnd_height_tile
		ASL
		ASL
		ASL
		CLC
		ADC		_wnd_top_pos
		STA		_wnd_bottom_pos
		RTS

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_spr_draw:
		LDA		_gui_ctrl_menu_redraw_flag
		BEQ		_gui_ctrl_menu_cursor_spr_draw
		JMP		_gui_ctrl_menu_frame_spr_draw
_gui_ctrl_menu_cursor_spr_draw:
		LDA		_pad0_held
		AND		#$0F
		LSR
		BCC		loc_1370D1
		PHA
		LDA		_cur_wnd_pos._COL
		CLC
		ADC		#$02
		BCS		loc_1370C9
		CMP		_wnd_right_pos
		BCC		loc_1370CE
loc_1370C9:
		LDA		_wnd_right_pos
		SEC
		SBC		#$01
loc_1370CE:
		STA		_cur_wnd_pos._COL
		PLA
loc_1370D1:
		LSR
		BCC		loc_1370E5
		PHA
		LDA		_cur_wnd_pos._COL
		SEC
		SBC		#$02
		BCC		loc_1370E0
		CMP		_wnd_left_pos
		BCS		loc_1370E2
loc_1370E0:
		LDA		_wnd_left_pos
loc_1370E2:
		STA		_cur_wnd_pos._COL
		PLA
loc_1370E5:
		LSR
		BCC		loc_1370FC
		PHA
		LDA		_cur_wnd_pos._ROW
		CLC
		ADC		#$02
		BCS		loc_1370F4
		CMP		_wnd_bottom_pos
		BCC		loc_1370F9
loc_1370F4:
		LDA		_wnd_bottom_pos
		SEC
		SBC		#$01
loc_1370F9:
		STA		_cur_wnd_pos._ROW
		PLA
loc_1370FC:
		LSR
		BCC		loc_13710E
		LDA		_cur_wnd_pos._ROW
		SEC
		SBC		#$02
		BCC		loc_13710A
		CMP		_wnd_top_pos
		BCS		loc_13710C
loc_13710A:
		LDA		_wnd_top_pos
loc_13710C:
		STA		_cur_wnd_pos._ROW
loc_13710E:
		LDA		_pad0_held
		BEQ		loc_137117
		LDA		#$FF
		STA		_tmp514
loc_137117:
		LDA		_tmp514
		BMI		loc_137122
		AND		#$0F
		CMP		#$0A
		BCS		loc_137133
loc_137122:
		LDA		#$00
		STA		_spr_insert_args._idx
		STA		_spr_insert_args._attr
		LDA		_cur_wnd_pos._COL
		STA		_spr_insert_args._pos._COL
		LDA		_cur_wnd_pos._ROW
		STA		_spr_insert_args._pos._ROW
		JSR		_spr_lib_attr_insert
loc_137133:
		LDX		_tmp514
		BMI		locret_137142
		INX
		CPX		#$20
		BCC		loc_13713F
		LDX		#$00
loc_13713F:
		STX		_tmp514
locret_137142:
		RTS

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_frame_spr_draw:
		LDA		_wnd_width_tile
		ASL
		ASL
		ASL
		EOR		#$FF
		CLC
		ADC		#$01
		STA		_ptr0
		LDA		_wnd_height_tile
		ASL
		ASL
		ASL
		EOR		#$FF
		SEC
		SBC		#$1F
		STA		_ptr0+1
		LDA		_pad0_held
		AND		#$0F
		LSR
		BCC		loc_137173
		PHA
		LDA		_cur_wnd_pos._COL
		CLC
		ADC		#$02
		BCS		loc_13716E
		CMP		_ptr0
		BCC		loc_137170
loc_13716E:
		LDA		_ptr0
loc_137170:
		STA		_cur_wnd_pos._COL
		PLA
loc_137173:
		LSR
		BCC		loc_137183
		PHA
		LDA		_cur_wnd_pos._COL
		SEC
		SBC		#$02
		BCS		loc_137180
		LDA		#$00
loc_137180:
		STA		_cur_wnd_pos._COL
		PLA
loc_137183:
		LSR
		BCC		loc_137197
		PHA
		LDA		_cur_wnd_pos._ROW
		CLC
		ADC		#$02
		BCS		loc_137192
		CMP		_ptr0+1
		BCC		loc_137194
loc_137192:
		LDA		_ptr0+1
loc_137194:
		STA		_cur_wnd_pos._ROW
		PLA
loc_137197:
		LSR
		BCC		loc_1371A7
		LDA		_cur_wnd_pos._ROW
		SEC
		SBC		#$02
		CMP		#$28
		BCS		loc_1371A5
		LDA		#$28
loc_1371A5:
		STA		_cur_wnd_pos._ROW
loc_1371A7:
		LDA		#$00
		STA		_spr_insert_args._idx
		STA		_spr_insert_args._attr
		LDA		_cur_wnd_pos._COL
		STA		_spr_insert_args._pos._COL
		LDA		_cur_wnd_pos._ROW
		STA		_spr_insert_args._pos._ROW
		JSR		_spr_lib_attr_insert
		LDA		_wnd_width_tile
		SEC
		SBC		#$01
		ASL
		ASL
		ASL
		CLC
		ADC		_cur_wnd_pos._COL
		STA		_ptr0
		LDA		_wnd_height_tile
		SEC
		SBC		#$01
		ASL
		ASL
		ASL
		CLC
		ADC		_cur_wnd_pos._ROW
		STA		_ptr0+1
		LDX		_spr_buf_pos
		LDY		#$00
		LDA		_cur_wnd_pos._ROW
		STA		_spr_buf,X
		INX
		LDA		byte_137246,Y
		INY
		STA		_spr_buf,X
		INX
		LDA		#$00
		STA		_spr_buf,X
		INX
		LDA		_cur_wnd_pos._COL
		STA		_spr_buf,X
		INX
		LDA		_cur_wnd_pos._ROW
		STA		_spr_buf,X
		INX
		LDA		byte_137246,Y
		INY
		STA		_spr_buf,X
		INX
		LDA		#$00
		STA		_spr_buf,X
		INX
		LDA		_ptr0
		STA		_spr_buf,X
		INX
		LDA		_ptr0+1
		STA		_spr_buf,X
		INX
		LDA		byte_137246,Y
		INY
		STA		_spr_buf,X
		INX
		LDA		#$00
		STA		_spr_buf,X
		INX
		LDA		_cur_wnd_pos._COL
		STA		_spr_buf,X
		INX
		LDA		_ptr0+1
		STA		_spr_buf,X
		INX
		LDA		byte_137246,Y
		INY
		STA		_spr_buf,X
		INX
		LDA		#$00
		STA		_spr_buf,X
		INX
		LDA		_ptr0
		STA		_spr_buf,X
		INX
		STX		_spr_buf_pos
		RTS

; =============== S U B	R O U T	I N E =======================================
_menu_wnd_redraw_flag_set:
		LDA		#$FF
		STA		_gui_ctrl_menu_redraw_flag
		RTS
byte_137246:
		.BYTE	$E0,$E1,$E2,$E3

; =============== S U B	R O U T	I N E =======================================
_gui_ctrl_menu_redraw:
		LDA		_gui_ctrl_menu_redraw_flag
		BNE		loc_13724F
		RTS
loc_13724F:
		JSR		_spr_clear
		LDA		_cur_wnd_pos._COL
		AND		#$F8
		CMP		_wnd_left_pos
		BNE		loc_137262
		LDA		_cur_wnd_pos._ROW
		AND		#$F8
		CMP		_wnd_top_pos
		BEQ		loc_13728C
loc_137262:
		JSR		_wnd_erase
		LDA		_wnd_idx
		ASL
		TAX
		LDA		_cur_wnd_pos._COL
		AND		#$F8
		STA		_cur_wnd_pos._COL
		LSR
		LSR
		LSR
		STA		_city._menu_pos,X
		STA		_wnd_left_tile
		INX
		LDA		_cur_wnd_pos._ROW
		AND		#$F8
		STA		_cur_wnd_pos._ROW
		LSR
		LSR
		LSR
		STA		_city._menu_pos,X
		STA		_wnd_top_tile
		JSR		_wnd_draw
		JSR		_wnd_rect_calc
loc_13728C:
		LDA		#$00
		STA		_gui_ctrl_menu_redraw_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137291:
		LDA		_cur_wnd_pos._COL
		LSR
		LSR
		LSR
		SEC
		SBC		_wnd_left_tile
		STA		_ptr0
		LDA		_cur_wnd_pos._ROW
		LSR
		LSR
		LSR
		SEC
		SBC		_wnd_top_tile
		STA		_ptr0+1
		LDY		_wnd_idx
		LDX		byte_13734B,Y
		LDA		off_137351,Y
		STA		_tmp2
		LDA		off_137357,Y
		STA		_tmp3
		LDY		#$00
		STY		_tmp5
loc_1372B8:
		LDA		(_tmp2),Y
		CLC
		ADC		#$03
		STA		_tmp4
		LDA		_ptr0
		CMP		(_tmp2),Y
		BCC		loc_1372F4
		CMP		_tmp4
		BCS		loc_1372F4
		INY
		LDA		(_tmp2),Y
		CLC
		ADC		#$03
		STA		_tmp4
		LDA		_ptr0+1
		CMP		(_tmp2),Y
		BCC		loc_1372F5
		CMP		_tmp4
		BCS		loc_1372F5
		LDX		_wnd_idx
		CPX		#$04
		BEQ		loc_137331
		PHA
;		JSR		_apu_se_click
		APUA_SE	_SE_IDX_CLICK
		PLA
		CPX		#$00
		BEQ		loc_1372FD
		DEX
		BEQ		loc_137304
		DEX
		BEQ		loc_137326
		LDA		_tmp5
		SEC
		RTS
loc_1372F4:
		INY
loc_1372F5:
		INY
		INC		_tmp5
		DEX
		BNE		loc_1372B8
		CLC
		RTS
loc_1372FD:
		LDA		_tmp5
		STA		_city._game_speed
		CLC
		RTS
loc_137304:
		LDX		_tmp5
		LDA		_city._game_flags
		EOR		_bitmasks,X
		STA		_city._game_flags
		AND		#$08
		BNE		loc_137317
		LDA		#$80
		BNE		loc_13731D
loc_137317:
		LDY		_city._city_type
		LDA		_bg_music_list,Y
loc_13731D:
		CMP		_apu_mus_cur_idx
		BEQ		loc_137324
		STA		_apu_mus_idx_req
loc_137324:
		CLC
		RTS
loc_137326:
		LDX		_tmp5
		LDA		_disasters_flags
		EOR		_bitmasks,X
		STA		_disasters_flags
		CLC
		RTS
loc_137331:
		LDX		_tmp5
;		FJSRA	_game_menu_exec,PRG7,PRG3		; FIX, no code use PRG7, but SRAM
		FJSRA	_game_menu_exec,SRAM,PRG3
		LDA		#$00
		STA		_wnd_lvl_idx
		RTS

byte_13734B:
		.BYTE	$04,$04,$06,$06,$04,$01
off_137351:
		.BYTE	<[byte_13735D]
		.BYTE	<[byte_137365]
		.BYTE	<[byte_13736D]
		.BYTE	<[byte_137379]
		.BYTE	<[byte_137385]
		.BYTE	<[byte_13738D]
off_137357:
		.BYTE	>[byte_13735D]
		.BYTE	>[byte_137365]
		.BYTE	>[byte_13736D]
		.BYTE	>[byte_137379]
		.BYTE	>[byte_137385]
		.BYTE	>[byte_13738D]
byte_13735D:
		.BYTE	$02,$04,$08,$04,$02,$09,$08,$09
byte_137365:
		.BYTE	$02,$04,$08,$04,$02,$0A,$08,$0A
byte_13736D:
		.BYTE	$02,$04,$06,$04,$0A,$04,$02,$09,$06,$09,$0A,$09
byte_137379:
		.BYTE	$02,$04,$0A,$04,$02,$09,$06,$04,$06,$09,$0A,$09
byte_137385:
		.BYTE	$03,$04,$08,$04,$03,$09,$08,$09
byte_13738D:
		.BYTE	$01,$01

; =============== S U B	R O U T	I N E =======================================
sub_13738F:
		LDA		_wnd_left_tile
		ASL
		ASL
		ASL
		STA		byte_AA
		LDA		_wnd_top_tile
		ASL
		ASL
		ASL
		STA		byte_AB
		LDX		_wnd_idx
		BEQ		loc_1373A8
		DEX
		BEQ		loc_1373C6
		DEX
		BEQ		loc_1373F5
		RTS
loc_1373A8:
		LDX		_city._game_speed
		LDA		byte_137423,X
		CLC
		ADC		byte_AA
		STA		_spr_insert_args._pos._COL
		LDA		byte_137427,X
		CLC
		ADC		byte_AB
		STA		_spr_insert_args._pos._ROW
		LDA		#$05
		STA		_spr_insert_args._idx
		LDA		#$00
		STA		_spr_insert_args._attr
		JMP		_spr_lib_attr_insert
loc_1373C6:
		LDA		_city._game_flags
		LDX		#$00
loc_1373CB:
		LSR
		BCC		loc_1373EF
		PHA
		TXA
		PHA
		LDA		byte_13742B,X
		CLC
		ADC		byte_AA
		STA		_spr_insert_args._pos._COL
		LDA		byte_13742F,X
		CLC
		ADC		byte_AB
		STA		_spr_insert_args._pos._ROW
		LDA		#$05
		STA		_spr_insert_args._idx
		LDA		#$00
		STA		_spr_insert_args._attr
		JSR		_spr_lib_attr_insert
		PLA
		TAX
		PLA
loc_1373EF:
		INX
		CPX		#$04
		BCC		loc_1373CB
		RTS
loc_1373F5:
		LDA		_disasters_flags
		LDX		#$00
loc_1373F9:
		LSR
		BCC		loc_13741D
		PHA
		TXA
		PHA
		LDA		byte_137433,X
		CLC
		ADC		byte_AA
		STA		_spr_insert_args._pos._COL
		LDA		byte_137439,X
		CLC
		ADC		byte_AB
		STA		_spr_insert_args._pos._ROW
		LDA		#$05
		STA		_spr_insert_args._idx
		LDA		#$00
		STA		_spr_insert_args._attr
		JSR		_spr_lib_attr_insert
		PLA
		TAX
		PLA
loc_13741D:
		INX
		CPX		#$06
		BCC		loc_1373F9
		RTS
byte_137423:
		.BYTE	$14,$44,$14,$44
byte_137427:
		.BYTE	$18,$18,$40,$40
byte_13742B:
		.BYTE	$14,$44,$14,$44
byte_13742F:
		.BYTE	$18,$18,$48,$48
byte_137433:
		.BYTE	$14,$34,$54,$14,$34,$54
byte_137439:
		.BYTE	$18,$18,$18,$40,$40,$40

; =============== S U B	R O U T	I N E =======================================
_wnd_draw:
		APUA_SE	_SE_IDX_ROLLIN
		JSR		_mmc5_prgA000_bank2_set
loc_137446:
		JSR		_wnd_draw_line
		JSR		_wait_for_nmi
		LDA		_wnd_draw_cnt
		BNE		loc_137446
		JMP		_mmc5_prgA000_bank2_restore


; =============== S U B	R O U T	I N E =======================================
_wnd_draw_line:
		LDA		#$00
		STA		_wnd_ppu_data_backup_req
		LDA		_wnd_draw_cnt
		BNE		loc_137464
		LDA		#$FF
		STA		_wnd_draw_cnt
		LDA		#$00
		STA		_wnd_cur_line_num
loc_137464:
;		JSR		_buffers_test_flush
;		BCC		loc_13746A
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BEQ		loc_13746A
		RTS
loc_13746A:
		LDX		_wnd_lvl_idx
		JSR		_wnd_load_type_data
		BCS		loc_1374AB
		LDA		_wnd_type,X
		BNE		loc_137490
		LDA		_wnd_cur_line_num
		CMP		_wnd_height_tile,X
		BCC		loc_137480
		LDA		#$00
		STA		_wnd_draw_cnt
		RTS
loc_137480:
		JSR		_wnd_prequeue_data_line
		JSR		_wnd_ppu_ofs_calc0
;		JSR		_wnd_ppu_data_backup_req_set	; REDUNDANT
		LDA		#$FF							; FIX
		STA		_wnd_ppu_data_backup_req
		JSR		_wnd_nt_line_queue0
		JMP		_wnd_extnt_line_queue0

loc_137490:
		LDA		_wnd_cur_line_num
		CMP		_wnd_width_tile,X
		BCC		loc_13749B
		LDA		#$00
		STA		_wnd_draw_cnt
		RTS
loc_13749B:
		JSR		_wnd_prequeue_data_line
		JSR		_wnd_ppu_ofs_calc1
;		JSR		_wnd_ppu_data_backup_req_set	; REDUNDANT
		LDA		#$FF							; FIX
		STA		_wnd_ppu_data_backup_req
		JSR		_wnd_nt_line_queue1
		JMP		_wnd_extnt_line_queue1

loc_1374AB:
		BRK

; =============== S U B	R O U T	I N E =======================================
_wnd_erase:
		APUA_SE	_SE_IDX_ROLLOUT
		JSR		_mmc5_prgA000_bank2_set
loc_1374B3:
		JSR		_wnd_erase_line
		JSR		_wait_for_nmi
		LDA		_wnd_erase_cnt
		BNE		loc_1374B3
		JMP		_mmc5_prgA000_bank2_restore


; =============== S U B	R O U T	I N E =======================================
_wnd_erase_line:
		LDX		_wnd_lvl_idx
		LDA		#$00
		STA		_wnd_ppu_data_backup_req
		LDA		_wnd_erase_cnt
		BNE		loc_1374DC
		LDA		#$FF
		STA		_wnd_erase_cnt
		LDA		_wnd_height_tile,X
		LDY		_wnd_type,X
		BEQ		loc_1374D7
		LDA		_wnd_width_tile,X
loc_1374D7:
		SEC
		SBC		#$01
		STA		_wnd_cur_line_num
loc_1374DC:
;		JSR		_buffers_test_flush
;		BCC		loc_1374E2
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BEQ		loc_1374E2
		RTS
loc_1374E2:
		LDA		_wnd_type,X
		BNE		loc_1374F8
		JSR		_wnd_ppu_ofs_calc0
		JSR		_wnd_nt_backup_queue0
		JSR		_wnd_extnt_backup_queue0
		DEC		_wnd_cur_line_num
		BPL		locret_1374F7
		LDA		#$00
		STA		_wnd_erase_cnt
locret_1374F7:
		RTS
loc_1374F8:
		JSR		_wnd_ppu_ofs_calc1
		JSR		_wnd_nt_backup_queue1
		JSR		_wnd_extnt_backup_queue1
		DEC		_wnd_cur_line_num
		BPL		locret_137509
		LDA		#$00
		STA		_wnd_erase_cnt
locret_137509:
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_prgA000_bank2_set:
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		LDA		_prg_prgA000_shadow
		STA		_mmc5_prgA000_bank_tmp
		LDA		#$F2
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		#$00
		STA		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_mmc5_prgA000_bank2_restore:
		LDA		#$FF
		STA		_mmc5_operation_in_progress_flag
		LDA		_mmc5_prgA000_bank_tmp
		STA		_prg_prgA000_shadow
		STA		_MMC5_PRG_PAGES+2
		LDA		#$00
		STA		_mmc5_operation_in_progress_flag
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_load_type_data:
		LDY		_wnd_idx,X
		LDA		_wnd_height0_list,Y
		STA		_wnd_width_tile,X
		CMP		#$11
		BCS		loc_13754D
		LDA		_wnd_height1_list,Y
		STA		_wnd_height_tile,X
		CMP		#$11
		BCS		loc_13754D
		LDA		_wnd_type_list,Y
		STA		_wnd_type,X
		LDA		_wnd_width_list,Y
		STA		_wnd_width,X
		RTS
loc_13754D:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_prequeue_data_line:
		LDY		_wnd_idx,X
		LDA		_wnd_nt_lib_lo,Y
		STA		_ptr0
		LDA		_wnd_nt_lib_hi,Y
		STA		_ptr0+1
		LDA		_wnd_extnt_lib_lo,Y
		STA		_tmp2
		LDA		_wnd_extnt_lib_hi,Y
		STA		_tmp3
		LDA		_wnd_width,X
		STA		_tmp4
		LDY		_wnd_cur_line_num
		MULYA
		TAY
		TXA
		PHA
		LDX		#$00
loc_13757C:
		LDA		(_ptr0),Y
		STA		_pre_ppu_buf,X
		LDA		(_tmp2),Y
		INY
		STA		_pre_ppu_buf+$20,X
		INX
		DEC		_tmp4
		BNE		loc_13757C
		PLA
		TAX
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_ppu_ofs_calc0:
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_137597
		JMP		loc_1375F0
loc_137597:
		LDA		_cur_screen_tile._COL			; TODO: CIADDB but with ,X indexation
		LSR
		CLC
		ADC		_wnd_left_tile,X
		CMP		#$20
		BCC		loc_1375A3
		SBC		#$20
loc_1375A3:
		STA		_ptr0
		LDA		_cur_screen_tile._ROW
		LSR
		SEC
		SBC		#$07
		BCS		loc_1375AF
		ADC		#$1E
loc_1375AF:
		CLC
		ADC		_wnd_top_tile,X
		ADC		_wnd_cur_line_num
		CMP		#$1E
		BCC		loc_1375BA
		SBC		#$1E
loc_1375BA:
		LDY		#$20
		MULYA
		CLC
		ADC		_ptr0
		STA		_wnd_line_tmp_ppu_ofs
		STY		_wnd_line_tmp_ppu_ofs+1
		AND		#$1F
		STA		_ptr0
		ADC		_wnd_width,X
		CMP		#$20
		BCC		loc_1375E7
		LDA		#$20
		SBC		_ptr0
		STA		_wnd_line_tmp_width
		LDA		_wnd_width,X
		SEC
		SBC		_wnd_line_tmp_width
		STA		_wnd_line_tmp_height
		RTS
loc_1375E7:
		LDA		_wnd_width,X
		STA		_wnd_line_tmp_width
		LDA		#$00
		STA		_wnd_line_tmp_height
		RTS
loc_1375F0:
		LDA		_wnd_left_tile,X
		STA		_ptr0
		LDA		_wnd_top_tile,X
		CLC
		ADC		_wnd_cur_line_num
		LDY		#$20
		MULYA
		CLC
		ADC		_ptr0
		STA		_wnd_line_tmp_ppu_ofs
		TYA
		ORA		#$04
		STA		_wnd_line_tmp_ppu_ofs+1
		LDA		_wnd_width,X
		STA		_wnd_line_tmp_width
		LDA		#$00
		STA		_wnd_line_tmp_height
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_ppu_ofs_calc1:
		LDA		_game_wnd_spr_mode_flag
		BEQ		loc_137622
		JMP		loc_137681
loc_137622:
		LDA		_cur_screen_tile._COL
		LSR
		CLC
		ADC		_wnd_left_tile,X
		CMP		#$20
		BCC		loc_13762E
		SBC		#$20
loc_13762E:
		CLC
		ADC		_wnd_cur_line_num
		CMP		#$20
		BCC		loc_137637
		SBC		#$20
loc_137637:
		STA		_ptr0
		LDA		_cur_screen_tile._ROW
		LSR
		SEC
		SBC		#$07
		BCS		loc_137643
		ADC		#$1E
loc_137643:
		CLC
		ADC		_wnd_top_tile,X
		CMP		#$1E
		BCC		loc_13764C
		SBC		#$1E
loc_13764C:
		PHA
		LDY		#$20
		MULYA
		CLC
		ADC		_ptr0
		STA		_wnd_line_tmp_ppu_ofs
		STY		_wnd_line_tmp_ppu_ofs+1
		POPB	_ptr0
		LDA		#$1E
		SEC
		SBC		_ptr0
		CMP		_wnd_width,X
		BCS		loc_137678
		STA		_wnd_line_tmp_width
		LDA		_wnd_width,X
		SEC
		SBC		_wnd_line_tmp_width
		STA		_wnd_line_tmp_height
		RTS
loc_137678:
		LDA		_wnd_width,X
		STA		_wnd_line_tmp_width
		LDA		#$00
		STA		_wnd_line_tmp_height
		RTS
loc_137681:
		LDA		_wnd_left_tile,X
		CLC
		ADC		_wnd_cur_line_num
		STA		_ptr0
		LDA		_wnd_top_tile,X
		LDY		#$20
		MULYA
		CLC
		ADC		_ptr0
		STA		_wnd_line_tmp_ppu_ofs
		TYA
		ORA		#$04
		STA		_wnd_line_tmp_ppu_ofs+1
		LDA		_wnd_width,X
		STA		_wnd_line_tmp_width
		LDA		#$00
		STA		_wnd_line_tmp_height
		RTS

; REDUNDANT
; =============== S U B	R O U T	I N E =======================================
;_wnd_ppu_data_backup_req_set:
;		LDA		#$FF
;		STA		_wnd_ppu_data_backup_req
;		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_nt_line_queue0:
		LDY		_ppu_queue_pos_head				; as against other code, this one
		LDX		#$00							; keep queue head in Y instead of X
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		LDA		_wnd_line_tmp_ppu_ofs
		STA		_ppu_queue_buf,Y
		INY
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$20
		STA		_ppu_queue_buf,Y
		INY
		LDA		_ptr0
		STA		_ppu_queue_buf,Y
		INY
loc_1376D0:
		LDA		_pre_ppu_buf,X
		INX
		STA		_ppu_queue_buf,Y
		INY
		DEC		_ptr0
		BNE		loc_1376D0
		LDA		_ptr0+1
		BEQ		loc_1376FE
		STA		_ptr0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$E0
		STA		_ppu_queue_buf,Y
		INY
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$20
		STA		_ppu_queue_buf,Y
		INY
		LDA		_ptr0
		STA		_ppu_queue_buf,Y
		INY
		LDA		#$00
		STA		_ptr0+1
		BEQ		loc_1376D0
loc_1376FE:
		STY		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_nt_line_queue1:
		LDY		_ppu_queue_pos_head
		LDX		#$00
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		LDA		_wnd_line_tmp_ppu_ofs
		STA		_ppu_queue_buf,Y
		INY
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#$20
		STA		_ppu_queue_buf,Y
		INY
		LDA		_ptr0
		ORA		#$80
		STA		_ppu_queue_buf,Y
		INY
loc_137723:
		LDA		_pre_ppu_buf,X
		INX
		STA		_ppu_queue_buf,Y
		INY
		DEC		_ptr0
		BNE		loc_137723
		LDA		_ptr0+1
		BEQ		loc_137751
		STA		_ptr0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		STA		_ppu_queue_buf,Y
		INY
		LDA		#$20
		STA		_ppu_queue_buf,Y
		INY
		LDA		_ptr0
		ORA		#$80
		STA		_ppu_queue_buf,Y
		INY
		LDA		#$00
		STA		_ptr0+1
		BEQ		loc_137723
loc_137751:
		STY		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_extnt_line_queue0:
		LDY		_extnt_queue_pos_head
		LDX		#$00
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		LDA		_wnd_line_tmp_ppu_ofs
		STA		_extnt_queue_buf,Y
		INY
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,Y
		INY
		LDA		_ptr0
		STA		_extnt_queue_buf,Y
		INY
loc_137774:
		LDA		_pre_ppu_buf+$20,X
		INX
		STA		_extnt_queue_buf,Y
		INY
		DEC		_ptr0
		BNE		loc_137774
		LDA		_ptr0+1
		BEQ		loc_1377A2
		STA		_ptr0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$E0
		STA		_extnt_queue_buf,Y
		INY
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,Y
		INY
		LDA		_ptr0
		STA		_extnt_queue_buf,Y
		INY
		LDA		#$00
		STA		_ptr0+1
		BEQ		loc_137774
loc_1377A2:
		STY		_extnt_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_extnt_line_queue1:
		LDY		_extnt_queue_pos_head
		LDX		#$00
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		LDA		_wnd_line_tmp_ppu_ofs
		STA		_extnt_queue_buf,Y
		INY
		LDA		_wnd_line_tmp_ppu_ofs+1
		ORA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,Y
		INY
		LDA		_ptr0
		ORA		#$80
		STA		_extnt_queue_buf,Y
		INY
loc_1377C7:
		LDA		_pre_ppu_buf+$20,X
		INX
		STA		_extnt_queue_buf,Y
		INY
		DEC		_ptr0
		BNE		loc_1377C7
		LDA		_ptr0+1
		BEQ		loc_1377F5
		STA		_ptr0
		LDA		_wnd_line_tmp_ppu_ofs
		AND		#$1F
		STA		_extnt_queue_buf,Y
		INY
		LDA		#>[_MMC5_EXRAM]
		STA		_extnt_queue_buf,Y
		INY
		LDA		_ptr0
		ORA		#$80
		STA		_extnt_queue_buf,Y
		INY
		LDA		#$00
		STA		_ptr0+1
		BEQ		loc_1377C7
loc_1377F5:
		STY		_extnt_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_nt_backup_queue0:
		PPUQSTART
		LDA		_wnd_cur_line_num
		ASL
		ASL
		ASL
		ASL
		TAY
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		PPUQMOVB	_wnd_line_tmp_ppu_ofs,OP_NOP,0
loc_13781C:
		PPUQMOVB	_wnd_line_tmp_ppu_ofs+1,OP_ORA,#$20
		PPUQMOVB	_ptr0,OP_NOP,0
loc_13781D:
		LDA		_wnd_lvl_idx
		BEQ		loc_137827
		LDA		_nt_backup1,Y
		JMP		loc_13782A
loc_137827:
		LDA		_nt_backup0,Y
loc_13782A:
		INY
		PPUQMOVA
		DEC		_ptr0
		BNE		loc_13781D
		DO_WRAP	_ptr0,_ptr0+1,loc_137856
		PPUQMOVB	_wnd_line_tmp_ppu_ofs,OP_AND,#$E0
		JMP		loc_13781C
loc_137856:
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_nt_backup_queue1:
		PPUQSTART
		LDA		_wnd_cur_line_num
		ASL
		ASL
		ASL
		ASL
		TAY
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		PPUQMOVB	_wnd_line_tmp_ppu_ofs,OP_NOP,0
		PPUQMOVB	_wnd_line_tmp_ppu_ofs+1,OP_ORA,#$20
		PPUQMOVB	_ptr0,OP_ORA,#$80
loc_137880:
		LDA		_wnd_lvl_idx
		BEQ		loc_13788A
loc_137884:
		LDA		_nt_backup1,Y
		JMP		loc_13788D
loc_13788A:
		LDA		_nt_backup0,Y
loc_13788D:
		INY
		PPUQMOVA
		DEC		_ptr0
		BNE		loc_137880
		DO_WRAP	_ptr0,_ptr0+1,loc_1378B9
		PPUQMOVB	_wnd_line_tmp_ppu_ofs,OP_AND,#$1F
		PPUQMOVB	#$20,OP_NOP,0
		PPUQMOVB	_ptr0,OP_ORA,#$80
		JMP		loc_137880
loc_1378B9:
		PPUQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_extnt_backup_queue0:
		EXTQSTART
		LDA		_wnd_cur_line_num
		ASL
		ASL
		ASL
		ASL
		TAY
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		EXTQMOVB	_wnd_line_tmp_ppu_ofs,OP_NOP,0
loc_1378E0:
		EXTQMOVB	_wnd_line_tmp_ppu_ofs+1,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_ptr0,OP_NOP,0
loc_1378E1:
		LDA		_wnd_lvl_idx
		BEQ		loc_1378EB
		LDA		_extnt_backup1,Y
		JMP		loc_1378EE
loc_1378EB:
		LDA		_extnt_backup0,Y
loc_1378EE:
		INY
		EXTQMOVA
		DEC		_ptr0
		BNE		loc_1378E1
		DO_WRAP	_ptr0,_ptr0+1,loc_13791A
		EXTQMOVB	_wnd_line_tmp_ppu_ofs,OP_AND,#$E0
		JMP		loc_1378E0
loc_13791A:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_wnd_extnt_backup_queue1:
		EXTQSTART
		LDA		_wnd_cur_line_num
		ASL
		ASL
		ASL
		ASL
		TAY
		LDA		_wnd_line_tmp_width
		STA		_ptr0
		LDA		_wnd_line_tmp_height
		STA		_ptr0+1
		EXTQMOVB	_wnd_line_tmp_ppu_ofs,OP_NOP,0
		EXTQMOVB	_wnd_line_tmp_ppu_ofs+1,OP_ORA,#>[_MMC5_EXRAM]
		EXTQMOVB	_ptr0,OP_ORA,#$80
loc_137944:
		LDA		_wnd_lvl_idx
		BEQ		loc_13794E
		LDA		_extnt_backup1,Y
		JMP		loc_137951
loc_13794E:
		LDA		_extnt_backup0,Y
loc_137951:
		INY
		EXTQMOVA
		DEC		_ptr0
		BNE		loc_137944
		DO_WRAP	_ptr0,_ptr0+1,loc_13797D
		EXTQMOVB	_wnd_line_tmp_ppu_ofs,OP_AND,#$1F
		EXTQMOVB	#>[_MMC5_EXRAM],OP_NOP,0
		EXTQMOVB	_ptr0,OP_ORA,#$80
		JMP		loc_137944
loc_13797D:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_draw:
		LDX		#$00
		JSR		_RCI_meters_R_prequeue
		JSR		_RCI_meters_C_prequeue
		JSR		_RCI_meters_I_prequeue
		JSR		_RCI_meters_pos_calc
		JMP		_RCI_meters_queue


; =============== S U B	R O U T	I N E =======================================
_RCI_meters_R_prequeue:
		LDA		_RCI_meters_values._R
		LDY		#$80
		STY		_ptr0+1
		LDY		#$80
		BNE		_RCI_meters_prequeue

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_C_prequeue:
		LDA		_RCI_meters_values._C
		LDY		#$80
		STY		_ptr0+1
		LDY		#$81
		BNE		_RCI_meters_prequeue

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_I_prequeue:
		LDA		_RCI_meters_values._I
		LDY		#$00
		STY		_ptr0+1
		LDY		#$82

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_prequeue:
		STY		_ptr0
		AND		#$FF
		BMI		loc_1379F9
		PHA
		SEC
		SBC		#$08
		BCS		loc_1379BF
		LDA		#$00
loc_1379BF:
		TAY
		LDA		byte_151740,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		PLA
		CMP		#$09
		BCC		loc_1379D1
		LDA		#$08
loc_1379D1:
		TAY
		LDA		byte_151746,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		LDA		_ptr0
		STA		_pre_ppu_buf,X
		INX
		LDY		#$00
		LDA		byte_15174F,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		LDA		byte_151758,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		RTS
loc_1379F9:
		EOR		#$FF
		CLC
		ADC		#$01
		PHA
		PHA
		LDY		#$00
		LDA		byte_151740,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		LDA		byte_151746,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		LDA		_ptr0
		STA		_pre_ppu_buf,X
		INX
		PLA
		SEC
		SBC		#$08
		BCS		loc_137A24
		LDA		#$00
loc_137A24:
		TAY
		LDA		byte_151758,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf+1,X
		PLA
		CMP		#$09
		BCC		loc_137A35
		LDA		#$08
loc_137A35:
		TAY
		LDA		byte_15174F,Y
		CLC
		ADC		_ptr0+1
		STA		_pre_ppu_buf,X
		INX
		INX
		RTS

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_pos_calc:
		CIADDB	_tmp1,_cur_screen_tile._ROW,#$0F,NONE,#$1E
		WRAPA	_tmp2,_tmp3,#$05,#$1E
		CIADDB	_tmp0,_cur_screen_tile._COL,#$02,NONE,#$20
		WRAPA	_tmp4,_tmp5,#$03,#$20
		BUFPOS	_ptr0,_tmp0,_tmp1,#$20,#$20	; DST=(_tmp1*#$20+_tmp0)+#$2000
		RTS

; =============== S U B	R O U T	I N E =======================================
_RCI_meters_queue:
		LDX		#$00
		LDY		_ppu_queue_pos_head
loc_137AB8:
		LDA		_tmp3
		STA		_tmp6+1
		LDA		_ptr0
		STA		_ppu_queue_buf,Y
		INY
		LDA		_ptr0+1
		STA		_ppu_queue_buf,Y
		INY
		LDA		_tmp2
		STA		_tmp6
		ORA		#$80
		STA		_ppu_queue_buf,Y
		INY
loc_137AD2:
		LDA		_pre_ppu_buf,X
		INX
		STA		_ppu_queue_buf,Y
		INY
		DEC		_tmp6
		BNE		loc_137AD2
		DO_WRAP	_tmp6,_tmp6+1,loc_137B01
		LDA		_ptr0
		AND		#$1F
		STA		_ppu_queue_buf,Y
		INY
		LDA		#$20
		STA		_ppu_queue_buf,Y
		INY
		LDA		_tmp6
		ORA		#$80
		STA		_ppu_queue_buf,Y
		INY
		JMP		loc_137AD2
loc_137B01:
		DEC		_tmp4
		BEQ		loc_137B0A
		INC		_ptr0
		JMP		loc_137AB8
loc_137B0A:
		DO_WRAP	_tmp4,_tmp5,loc_137B1D
		LDA		_ptr0
		AND		#$E0
		STA		_ptr0
		JMP		loc_137AB8
loc_137B1D:
		STY		_ppu_queue_pos_head
		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_palette_update:
		LDA		_constr_palette_update_req
		BEQ		locret_137B5D
;		JSR		_buffers_test_flush
;		BCS		locret_137B5D
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		locret_137B5D
		LDA		_constr_palette_flags
		STA		_tmp4
		LDA		_constr_palette_flags+1
		STA		_tmp5
		LDA		_constr_palette_flags+2
		STA		_tmp6
		LDA		_constr_palette_flags+3
		STA		_tmp6+1
		LDA		#$0D
		STA		_tmp3
loc_137B3D:
		JSR		sub_137B5E
		BCC		loc_137B48
		JSR		sub_137B9C
		JMP		loc_137B55
loc_137B48:
		AND		#$FF
		BPL		loc_137B52
		JSR		sub_137B86
		JMP		loc_137B55
loc_137B52:
		JSR		sub_137B70
loc_137B55:
		DEC		_tmp3
		BPL		loc_137B3D
		LDA		#$00
		STA		_constr_palette_update_req
locret_137B5D:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137B5E:
		LSR		_tmp6+1
		ROR		_tmp6
		ROR		_tmp5
		ROR		_tmp4
		ROR
		LSR		_tmp6+1
		ROR		_tmp6
		ROR		_tmp5
		ROR		_tmp4
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137B70:
		LDA		_tmp3
		JSR		sub_137BB2
		LDA		_tmp3
		JSR		sub_137C04
		MOVWO	word_B2,byte_151698
		JMP		sub_137C08


; =============== S U B	R O U T	I N E =======================================
sub_137B86:
		LDA		_tmp3
		JSR		sub_137BB2
		LDA		_tmp3
		JSR		sub_137C04
		MOVWO	word_B2,byte_1516D0
		JMP		sub_137C08


; =============== S U B	R O U T	I N E =======================================
sub_137B9C:
		LDA		_tmp3
		JSR		sub_137BB2
		LDA		_tmp3
		JSR		sub_137C04
		MOVWO	word_B2,byte_151708
		JMP		sub_137C08


; =============== S U B	R O U T	I N E =======================================
sub_137BB2:
		LDY		#$00
		PHA
		AND		#$FE
		STA		_ptr0
		LDA		_cur_screen_tile._ROW
		LSR
		SEC
		ADC		_ptr0
		CMP		#$1E
		BCC		loc_137BC6
		SEC
		SBC		#$1E
loc_137BC6:
		LDX		#$20
		MULXA
		STA		_ptr0+1
		PLA
		AND		#$01
		ASL
		ADC		#$02
		STA		_tmp2
		LDA		_cur_screen_tile._COL
		LSR
		CLC
		ADC		_tmp2
		AND		#$1F
		CMP		#$1F
		BNE		loc_137BEC
		INY
		INY
loc_137BEC:
		CLC
		ADC		_ptr0+1
		STA		_tmp_extnt_dst_ofs
		TXA
		ORA		#>[_MMC5_EXRAM]
		STA		_tmp_extnt_dst_ofs+1
		CMP		#$5F
		BNE		loc_137C01
		LDA		_tmp_extnt_dst_ofs
		CMP		#$A0
		BCC		loc_137C01
		INY
loc_137C01:
		STY		byte_B6
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137C04:
		ASL
		ASL
		TAY
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137C08:
		EXTQSTART
		EXTQMOVB	_tmp_extnt_dst_ofs,OP_NOP,0
		EXTQMOVB	_tmp_extnt_dst_ofs+1,OP_NOP,0
		LDA		byte_B6
		AND		#$02
		BNE		loc_137C58
		EXTQMOVB	#$02,OP_NOP,0
		JSR		_ppu_extnt_word_queue
		LDA		byte_B6
		LSR
		BCS		loc_137C3E
		EXTQMOVB	_tmp_extnt_dst_ofs,OP_ADD,#$20
		LDA		_tmp_extnt_dst_ofs+1
		ADC		#$00
		EXTQMOVA
		JMP		loc_137C4C
loc_137C3E:
		EXTQMOVB	_tmp_extnt_dst_ofs,OP_AND,#$1F
		EXTQMOVB	#>[_MMC5_EXRAM],OP_NOP,0
loc_137C4C:
		EXTQMOVB	#$02,OP_NOP,0
		JSR		_ppu_extnt_word_queue
		EXTQEND
		RTS
loc_137C58:
		EXTQMOVB	#$01,OP_NOP,0
		JSR		_ppu_extnt_byte_queue
		EXTQMOVB	_tmp_extnt_dst_ofs,OP_AND,#$E0
		EXTQMOVB	_tmp_extnt_dst_ofs+1,OP_NOP,0
		EXTQMOVB	#$01,OP_NOP,0
		JSR		_ppu_extnt_byte_queue
		LDA		byte_B6
		LSR
		BCS		loc_137C8C
		LDA		_tmp_extnt_dst_ofs
		CLC
		ADC		#$20
		STA		_tmp_extnt_dst_ofs
		LDA		_tmp_extnt_dst_ofs+1
		ADC		#$00
		STA		_tmp_extnt_dst_ofs+1
		BNE		loc_137C96
loc_137C8C:
		LDA		_tmp_extnt_dst_ofs
		AND		#$1F
		STA		_tmp_extnt_dst_ofs
		LDA		#>[_MMC5_EXRAM]
		STA		_tmp_extnt_dst_ofs+1
loc_137C96:
		EXTQMOVB	_tmp_extnt_dst_ofs,OP_NOP,0
		EXTQMOVB	_tmp_extnt_dst_ofs+1,OP_NOP,0
		EXTQMOVB	#$01,OP_NOP,0
		JSR		_ppu_extnt_byte_queue
		EXTQMOVB	_tmp_extnt_dst_ofs,OP_AND,#$E0
		EXTQMOVB	_tmp_extnt_dst_ofs+1,OP_NOP,0
		EXTQMOVB	#$01,OP_NOP,0
		JSR		_ppu_extnt_byte_queue
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
_ppu_extnt_word_queue:
		LDA		(word_B2),Y
		INY
		EXTQMOVA

; =============== S U B	R O U T	I N E =======================================
_ppu_extnt_byte_queue:
		LDA		(word_B2),Y
		INY
		EXTQMOVA
		RTS

; =============== S U B	R O U T	I N E =======================================
_constr_area_redraw:
;		JSR		_buffers_test_flush
;		BCS		locret_137D0A
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		locret_137D0A
		LDA		_constr_area_update_req
		BEQ		locret_137D0A
		JSR		sub_137D20
		LDA		_ppu_queue_pos_head
		STA		byte_62
		LDA		_extnt_queue_pos_head
		STA		byte_63
loc_137CE8:
		JSR		sub_137D82
		JSR		sub_137DE8
		SEC
loc_137CEF:
		JSR		_map_get_ppu_tile
		JSR		sub_137E15
		JSR		sub_137E2A
		BCC		loc_137CEF
		JSR		sub_137F16
		BCC		loc_137CE8
		JSR		sub_137F22
		LDA		byte_62
		STA		_ppu_queue_pos_head
		LDA		byte_63
		STA		_extnt_queue_pos_head
locret_137D0A:
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137D0B:
		LDA		#$00
		STA		_constr_area_update_req
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137D20:
		LDA		_update_rect_top
		BPL		loc_137D26
		LDA		#$00
loc_137D26:
		CMP		_cur_map_wnd_pos._ROW
		BCS		loc_137D2C
		LDA		_cur_map_wnd_pos._ROW
loc_137D2C:
		STA		_update_rect_top
		SEC
		SBC		_cur_map_wnd_pos._ROW
		STA		byte_59
		LDA		_update_rect_bottom
		SEC
		SBC		_cur_map_wnd_pos._ROW
		CMP		#$15
		BCC		loc_137D43
		LDA		#$14
		CLC
		ADC		_cur_map_wnd_pos._ROW
		STA		_update_rect_bottom
loc_137D43:
		LDA		_update_rect_left
		BPL		loc_137D49
		LDA		#$00
loc_137D49:
		CMP		_cur_map_wnd_pos._COL
		BCS		loc_137D4F
		LDA		_cur_map_wnd_pos._COL
loc_137D4F:
		STA		_update_rect_left
		SEC
		SBC		_cur_map_wnd_pos._COL
		CLC
		ADC		#$07
		STA		byte_58
		LDA		_update_rect_right
		SEC
		SBC		_cur_map_wnd_pos._COL
		CMP		#$19
		BCC		loc_137D69
		LDA		#$18
		CLC
		ADC		_cur_map_wnd_pos._COL
		STA		_update_rect_right
loc_137D69:
		LDX		#$00
		LDA		_update_rect_bottom
		SEC
		SBC		_update_rect_top
		ADC		#$00
		CMP		#$02
		BCC		loc_137D7A
		LDA		#$01
		LDX		#$FF
loc_137D7A:
		STA		byte_64
		STX		byte_65
		CLC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137D80:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137D82:
		LDA		_cur_screen_tile._ROW
		LSR
		CLC
		ADC		byte_59
		CMP		#$1E
		BCC		loc_137D8E
		SBC		#$1E
loc_137D8E:
		LDX		#$20
		MULXA
		STA		_ptr0
		LDA		_cur_screen_tile._COL
		LSR
		CLC
		ADC		byte_58
		AND		#$1F
		PHA
		CLC
		ADC		_ptr0
		STA		byte_66
		STA		byte_68
		TXA
		ORA		#$20
		STA		byte_67
		TXA
		ORA		#>[_MMC5_EXRAM]
		STA		byte_69
		PLA
		EOR		#$FF
		SEC
		ADC		#$20
		STA		_ptr0
		LDA		_update_rect_right
		SEC
		SBC		_update_rect_left
		CLC
		ADC		#$01
		CMP		_ptr0
		BEQ		loc_137DD9
		BCC		loc_137DD9
		SBC		_ptr0
		STA		byte_6B
		LDA		_ptr0
		STA		byte_6A
		JMP		loc_137DDF
loc_137DD9:
		STA		byte_6A
		LDA		#$00
		STA		byte_6B
loc_137DDF:
		LDA		_update_rect_left
		STA		_tmp_tile_pos._COL
		LDA		_update_rect_top
		STA		_tmp_tile_pos._ROW
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137DE8:
		LDX		byte_62
		PPUQMOVB	byte_66,OP_NOP,0
		PPUQMOVB	byte_67,OP_NOP,0
		PPUQMOVB	byte_6A,OP_NOP,0
		STX		byte_62
		LDX		byte_63
		EXTQMOVB	byte_68,OP_NOP,0
		EXTQMOVB	byte_69,OP_NOP,0
		EXTQMOVB	byte_6A,OP_NOP,0
		STX		byte_63
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137E15:
		LDX		byte_62
		PPUQMOVB	word_6C,OP_NOP,0
		STX		byte_62
		LDX		byte_63
		EXTQMOVB	word_6C+1,OP_NOP,0
		STX		byte_63
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137E2A:
		INC		_tmp_tile_pos._COL
		DEC		byte_6A
		BNE		loc_137E6A
		DO_WRAP	byte_6A,byte_6B,loc_137E6C
		LDX		byte_62
		PPUQMOVB	byte_66,OP_AND,#$E0
		PPUQMOVB	byte_67,OP_NOP,0
		PPUQMOVB	byte_6A,OP_NOP,0
		STX		byte_62
		LDX		byte_63
		EXTQMOVB	byte_68,OP_AND,#$E0
		EXTQMOVB	byte_69,OP_NOP,0
		EXTQMOVB	byte_6A,OP_NOP,0
		STX		byte_63
loc_137E6A:
		CLC
		RTS
loc_137E6C:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
_map_get_ppu_tile:
		LDA		_tmp6E+1						; test if we already calculated buffer ofs
		BEQ		loc_137E7C						; if not, test out of the map cases
		BCS		loc_137E7C
		LDA		_tmp_tile_pos._COL
		CMP		#$4C
		BCS		.map_def_tile_select
		BCC		.map_skip_tile_pos_calc
loc_137E7C:
		LDA		_tmp_tile_pos._COL
		CMP		#$4C
		BCS		.map_def_tile_select
		ORA		_tmp_tile_pos._ROW
		BPL		loc_137E93
.map_def_tile_select:
		LDA		#$00
		STA		_tmp6E+1
		MOVWI	word_6C,$01DF					; return default black tile idx
		RTS
loc_137E93:
		BUFPOS	_tmp6E,_tmp_tile_pos._COL,_tmp_tile_pos._ROW,#$4C,#>[_city_map]	; DST=(_tmp_tile_pos._ROW*#$4C+_tmp_tile_pos._COL)+#>[_city_map]00
		BNE		loc_137EB5						; calc map tile offset, BNE unconditiona
.map_skip_tile_pos_calc:
		INCW	_tmp6E
loc_137EB5:
		LDX		#$00
		LDY		#$00
		LDA		(_tmp6E),Y
		BPL		.map_get_building_tile			; for all tiles less than 80, draw as building
		CMP		#_MAP_GROUND					; for internal building tiles 80-8F draw the rest of
		BCS		.map_single_tiles				; previously selected building
		JSR		_map_get_building_idx			; but first get the building idx
.map_get_building_tile:
		LDY		#$01
		STY		_tmp533							; default values for other anims
		STX		_tmp534
		JSR		_map_get_animated_areas_ppu_tile; read an actual building ppu display tile
		BCC		.map_no_anim_tiles				; if any
		BCS		loc_137ED9
.map_single_tiles:
		TAY										; OPTIMIZED, backup
		JSR		_map_get_single_ppu_tile_lib	; select lib data position for selected tiles
		TYA										; OPTIMIZED, restore
.map_no_anim_tiles:
		JSR		_map_get_still_single_tiles
loc_137ED9:
		JMP		_map_get_anim_single_tiles


; =============== S U B	R O U T	I N E =======================================
_map_get_building_idx:
		AND		#$0F							; one more of similar routines to calc
		TAX										; the real building index by selected internal
		LDA		_map_internal_tiles_ofs_list,X	; tile
		TAY										; this is a little bit optimized version
		DEC		_tmp6E+1						; with only high nibble of ptr changing
		LDA		(_tmp6E),Y
		INC		_tmp6E+1
		INX										; store the internal tile index+1
		RTS

_map_internal_tiles_ofs_list:
		.BYTE	$B4,$B3,$FF,$68,$67,$66,$B2,$FE,$1C,$1B,$1A,$19,$65,$B1,$FD

; =============== S U B	R O U T	I N E =======================================
_map_get_still_single_tiles:
		STX		_tmp2							; X = building tile idx
		TAX										; A = map tile idx
		LDA		_tlm_still_tiles_lib_lo,X
		STA		_ptr0
		LDA		_tlm_still_tiles_lib_hi,X
		STA		_ptr0+1
		LDA		_tmp2
		ASL
		TAY
		LDA		(_ptr0),Y
		STA		word_6C+1
		INY
		LDA		(_ptr0),Y
		STA		word_6C
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137F16:
		INC		_update_rect_top
		INC		byte_59
		DEC		byte_64
		BEQ		loc_137F20
		CLC
		RTS
loc_137F20:
		SEC
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_137F22:
		LDA		byte_65
		BNE		locret_137F29
		JMP		sub_15011A
locret_137F29:
		RTS

; =============== S U B	R O U T	I N E =======================================
_swap_constr_area_update_params:
		LDA		_constr_area_update_req
		LDX		_constr_area_update_req_tmp
		STA		_constr_area_update_req_tmp
		STX		_constr_area_update_req
		LDA		_update_rect_left
		LDX		_update_rect_left_tmp
		STA		_update_rect_left_tmp
		STX		_update_rect_left
		LDA		_update_rect_right
		LDX		_update_rect_right_tmp
		STA		_update_rect_right_tmp
		STX		_update_rect_right
		LDA		_update_rect_top
		LDX		_update_rect_top_tmp
		STA		_update_rect_top_tmp
		STX		_update_rect_top
		LDA		_update_rect_bottom
		LDX		_update_rect_bottom_tmp
		STA		_update_rect_bottom_tmp
		STX		_update_rect_bottom
		RTS

; =============== S U B	R O U T	I N E =======================================
sub_15011A:
		JSR		_swap_constr_area_update_params
		LDA		#$00
		STA		_constr_area_update_req_tmp
		STA		byte_5C
		RTS

; =============== S U B	R O U T	I N E =======================================
_auto_goto_test:
		LDA		_city._game_flags
		AND		#$04
		BNE		_goto_start
		RTS

; =============== S U B	R O U T	I N E =======================================
_goto_start:
		LDA		_goto_alert_req
		BEQ		locret_137F7B
		LDA		_goto_transition_counter
		BNE		locret_137F7B
		LDA		_goto_dst_tile._COL
		SEC
		SBC		#$0C
		BCS		loc_137F48
		LDA		#$00
		BEQ		loc_137F4E
loc_137F48:
		CMP		#$35
		BCC		loc_137F4E
		LDA		#$35
loc_137F4E:
		STA		_cur_map_wnd_pos._COL
		STA		_update_rect_left
		CLC
		ADC		#$18
		STA		_update_rect_right
		LDA		_goto_dst_tile._ROW
		SEC
		SBC		#$0A
		BCS		loc_137F63
		LDA		#$00
		BEQ		loc_137F69
loc_137F63:
		CMP		#$37
		BCC		loc_137F69
		LDA		#$37
loc_137F69:
		STA		_cur_map_wnd_pos._ROW
		STA		_update_rect_top
		CLC
		ADC		#$14
		STA		_update_rect_bottom
		LDA		#$1E
		STA		_goto_transition_counter
		LDA		#$FF
		STA		_constr_area_update_req
locret_137F7B:
		RTS

; =============== S U B	R O U T	I N E =======================================
_game_scr_init:
; REDUNDANT, moved to common initialization code
;		LDA		_cur_game_mode					; reinit the window starting pos
;		CMP		#$03							; all except practise start from the
;		BNE		loc_137F8D						; upper-left corner. even the practice
;		LDA		#$19							; loaded map!
;		STA		_cur_map_wnd_pos._COL			; this is not so much right
;		LDA		#$1B
;		STA		_cur_map_wnd_pos._ROW
;		BNE		loc_137F93
;loc_137F8D:
;		LDA		#$00
;		STA		_cur_map_wnd_pos._COL
;		STA		_cur_map_wnd_pos._ROW
		MOVW	_cur_map_wnd_pos,_city._wnd_pos	; for all modes except continue and practice this will be zero
loc_137F93:
;		JSR		_constr_palette_update_req_set	; REDUNDANT
		LDA		#$FF
		STA		_constr_palette_update_req
		LDA		#$00
		STA		_MMC5_NT_MODE
		STA		_game_wnd_spr_mode_flag
		STA		_scr_mode_idx
		STA		_game_msg_idx
		STA		_screen_pal_cur_spr_idx
		STA		_screen_pal_cur_bg_idx
		STA		_game_map_area_cursor_pos_backup._COL
		STA		_game_map_area_cursor_pos_backup._ROW
		STA		_game_msg_wnd_active_flag
		LDA		#$18
		STA		_cur_game_cursor_scr_pos._COL
		STA		_constr_palette_area_cursor_pos._COL
		LDA		#$48
		STA		_cur_game_cursor_scr_pos._ROW
		STA		_constr_palette_area_cursor_pos._ROW
		LDA		#$01
		STA		_cur_selected_buildings_size
		JSR		_game_hud_full_draw
		JSR		_seasonal_pal_load
		JSR		_pal_fade_in
		JMP		_scroll_msg_reset


; =============== S U B	R O U T	I N E =======================================
_seasonal_pal_load:
		LDX		_city._cur_month
		LDA		_seasonal_pal_list,X
		STA		_screen_pal_cur_bg_idx
		JSR		_pal_load_lib
		LDA		#$02
		STA		_ppu_pal_dequeue_req
		RTS
_seasonal_pal_list:
		.BYTE	$0E,$0E,$00,$00,$00,$0C,$0C,$0C,$0D,$0D,$0D,$0E

; =============== S U B	R O U T	I N E =======================================
_game_state09_presents_building_name_and_cost_hud_draw:
;		JSR		_buffers_test_flush
;		BCS		loc_150028
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_150028
		JSR		_building_name_and_cost_hud_draw
		LDA		#$00
		STA		_hud_present_name_draw_req
loc_150028:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state14_presents_build_menu:
;		JSR		_buffers_test_flush
;		BCS		loc_150055
		JSR		_buffers_test_flush				; OPTIMIZED, merged
		BNE		loc_150055
		JSR		_spr_clear
		LDA		#$00
		STA		_wnd_lvl_idx
		LDA		#$0C
		STA		_wnd_idx
		LDA		#$07
		STA		_wnd_left_tile
		LDA		#$0D
		STA		_wnd_top_tile
		JSR		_wnd_draw
		FJSRA	_presents_build_menu,PRGC,PRG2
		JSR		_wnd_erase
		LDA		#$00
		STA		_presents_build_menu_req
loc_150055:
		JMP		_game_state_common

; =============== S U B	R O U T	I N E =======================================
_game_state15_game_cursor_area_switch:
		LDA		_cur_game_cursor_area
		BNE		loc_15006C
		LDA		_game_map_area_cursor_pos_backup._COL
		BEQ		loc_150084
		STA		_cur_game_cursor_scr_pos._COL
		LDA		_game_map_area_cursor_pos_backup._ROW
		STA		_cur_game_cursor_scr_pos._ROW
		INC		_cur_game_cursor_area
		BNE		loc_150084
loc_15006C:
		LDA		_cur_game_cursor_scr_pos._COL
		STA		_game_map_area_cursor_pos_backup._COL
		LDA		_cur_game_cursor_scr_pos._ROW
		STA		_game_map_area_cursor_pos_backup._ROW
		LDA		_constr_palette_area_cursor_pos._COL
		STA		_cur_game_cursor_scr_pos._COL
		LDA		_constr_palette_area_cursor_pos._ROW
		STA		_cur_game_cursor_scr_pos._ROW
		LDA		#$00
		STA		_cur_game_cursor_area
loc_150084:
		JMP		_game_state_common

; REDUNDANT, macrocified
; =============== S U B	R O U T	I N E =======================================
;_apu_se_click:
;		LDA		#$01
;		BNE		_apu_se0_load
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_error:
;		LDA		#$02
;		BNE		_apu_se0_load
;		APUA_SE	_SE_IDX_ERROR
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_small_build:
;		LDA		#$04
;		BNE		_apu_se0_load
;		APUA_SE	_SE_IDX_SBUILD
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_large_build:
;		LDA		#$08
;
; !FALLTHROUGH!
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se0_load:
;		STA		_apu_se_idx_req
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;_unref_20:
;		STA		_apu_mus_idx_req
;		RTS
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_destroy:
;		LDA		#$04
;		BNE		_apu_se1_load
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_bulldozer:
;		LDA		#$08
;		BNE		_apu_se1_load
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_consrt_palette_click:
;		LDA		#$20
;		BNE		_apu_se1_load
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se_item_select:
;		LDA		#$40
;
; !FALLTHROUGH!
;
; =============== S U B	R O U T	I N E =======================================
;_apu_se1_load:
;		STA		_apu_se_idx_req+1
;		RTS

		SECTION_ENDS	C00, "PRGC INTERFACE MAIN ENGINE PART1"

		BANK_END FC,$A000
