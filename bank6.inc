
		ORG $8000

		BANK_START $F6

		SECTION_START 7904

; =============== S U B	R O U T	I N E =======================================
_info_maps_button_redraw:
		EXTQSTART
;		LDY		_cur_info_maps_button_idx		; REDUNDANT as fuck
;		LDA		_info_maps_button_idx_list,Y	; special value for disabling
;		TAY										; released button draw, never used
;		CMP		#$08							; default value here is B, so we draw
;		BEQ		loc_A0199						; dummy release always
		LDY		_prev_info_maps_button_idx
		CPY		#$FF							; FIX: let's fix logic, skip release here instead
		BEQ		loc_A0199						; -
		LDA		_info_maps_button_idx_list,Y
		JSR		_button_released_extnt_queue	; draw released button
loc_A0199:
		LDY		_cur_info_maps_button_idx
;		CPY		#$0B							; REDUNDANT, cur info never gets 0B value,
;		BEQ		_button_press_exit				; never, here we have logic flaw or mistake
		LDA		_info_maps_button_idx_list,Y	; now draw new pushed button or default 0

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_button_pressed_extnt_queue:
		ASL
		PHA
		TAY
		LDA		_info_buttons_extnt_lib,Y		; get the released button extnt
		AND		#$C0							; and change it to pressed extnt
		ORA		#$04
		STA		_tmp2
		LDA		_info_buttons_extnt_lib+1,Y
		AND		#$C0
		ORA		#$04
		STA		_tmp3
		PLA
		JSR		_button_extnt_data_queue		; send data
_button_press_exit:
		EXTQEND
		RTS

; =============== S U B	R O U T	I N E =======================================
; as against the info maps buttons, graphs buttons are unpressed by default
; and you may press two buttons at the same time
;
_graphs_buttons_redraw:
		EXTQSTART
		CPY		#$FF							; get index of redraw button in Y
		BEQ		loc_A01CC						; skip release redraw on enter
		TYA
		CLC
;		ADC		#$0A							; REDUNDANT, removed unused lines
		ADC		#$08							; FIX, new lib offset
		JSR		_button_released_extnt_queue	; use common functions
loc_A01CC:
		LDY		_cur_graphs_button_press_idx
;		BPL		loc_A01D4						; the same for push button
;		EXTQEND
;		RTS
		BMI		_button_press_exit				; OPTIMIZED
;loc_A01D4:
		TYA
		CLC
;		ADC		#$0A							; REDUNDANT
		ADC		#$08							; FIX
		JMP		_button_pressed_extnt_queue

; =============== S U B	R O U T	I N E =======================================
_button_released_extnt_queue:
		ASL										; FIX: swap this, do one shift less later
		PHA
		TAY
		LDA		_info_buttons_extnt_lib,Y		; get released button exnt pattern
		STA		_tmp2
		LDA		_info_buttons_extnt_lib+1,Y
		STA		_tmp3
		PLA

; =============== S U B	R O U T	I N E =======================================
_button_extnt_data_queue:
;		ASL										; FIX one shift less here get extnt lib ofset
		ASL
		ASL
		TAY
		MOVWO	_ptr0,_info_buttons_extnt_hdr_lib
		JSR		_button_extnt_line_queue

; !FALLTHROUGH!

; FIX
; -
; =============== S U B	R O U T	I N E =======================================
_button_extnt_line_queue:
		LDA		(_ptr0),Y						; header is over?
		BNE		loc_A01FD
		EXTQMOVB	_tmp2,OP_NOP,0				; send an actual data calculated
		EXTQMOVB	_tmp3,OP_NOP,0				; before
		INY
		RTS
loc_A01FD:
		INY										; send header
		EXTQMOVA
		JMP		_button_extnt_line_queue
; -
; REDUNDANT
; -
; =============== S U B	R O U T	I N E =======================================
;_button_extnt_line_queue:
;		LDA		#$03
;		STA		_tmp5
;loc_A01FC:
;		LDA		(_ptr0),Y						; send header, 3 bytes ofs+size
;		INY										; predefined in the table below
;		EXTQMOVA
;		DEC		_tmp5
;		BNE		loc_A01FC
;		INY										; skip fourt byte
;		EXTQMOVB	_tmp2,OP_NOP,0				; send an actual data calculated
;		EXTQMOVB	_tmp3,OP_NOP,0				; before
;		RTS

_info_maps_button_idx_list:
		.BYTE	$00,$01,$02,$02,$03,$04,$05,$06,$07,$07	;,$08,$09	REDUNDANT

; here some more redundancy in this lib. every button correspond to
; 8 bytes of this lib, however, only first 3 bytes of every 4 are used
; because this is dummy data only for easier calculation of lib offset
;
; actual data to store as extnt is in _info_buttons_extnt_lib table
;
;_info_buttons_extnt_hdr_lib:
;			;	+	+	+	-	+	+	+	-
;		.BYTE	$04,$5D,$02,$43,$24,$5D,$02,$43	; 00 info map button 0
;		.BYTE	$44,$5D,$02,$43,$64,$5D,$02,$43	; 01 info map button 1
;		.BYTE	$84,$5D,$02,$03,$A4,$5D,$02,$03	; 02 info map button 2
;		.BYTE	$C4,$5D,$02,$03,$E4,$5D,$02,$03	; 03 info map button 3
;		.BYTE	$04,$5E,$02,$03,$24,$5E,$02,$03	; 04 info map button 4
;		.BYTE	$44,$5E,$02,$43,$64,$5E,$02,$43	; 05 info map button 5
;		.BYTE	$84,$5E,$02,$43,$A4,$5E,$02,$43	; 06 info map button 6
;		.BYTE	$C4,$5E,$02,$03,$E4,$5E,$02,$03	; 07 info map button 7
;		.BYTE	$02,$5D,$02,$04,$22,$5D,$02,$04	; 08 info map dummy button push, *unused*
;		.BYTE	$42,$5D,$02,$04,$62,$5D,$02,$04	; 09 info map dummy button release
;		.BYTE	$23,$5D,$02,$83,$43,$5D,$02,$83	; 08 -0A graphs button 0
;		.BYTE	$63,$5D,$02,$03,$83,$5D,$02,$03	; 09 -0B graphs button 1
;		.BYTE	$A3,$5D,$02,$43,$C3,$5D,$02,$43	; 0A -0C graphs button 2
;		.BYTE	$E3,$5D,$02,$83,$03,$5E,$02,$83	; 0B -0D graphs button 3
;		.BYTE	$23,$5E,$02,$C3,$43,$5E,$02,$C3	; 0C -0E graphs button 4
;		.BYTE	$63,$5E,$02,$03,$83,$5E,$02,$03	; 0D -0F graphs button 5
;		.BYTE	$A3,$5E,$02,$03,$C3,$5E,$02,$03	; 10 graphs dummy button push, *unused*
;		.BYTE	$E3,$5E,$02,$03,$03,$5F,$02,$03	; 11 graphs dummy button release, *unused*

; FIX
; let's reuse them on purpose now.
;
_info_buttons_extnt_hdr_lib:
		.BYTE	$04,$5D,$02,$00,$24,$5D,$02,$00	; 00 info map button 0
		.BYTE	$44,$5D,$02,$00,$64,$5D,$02,$00	; 01 info map button 1
		.BYTE	$84,$5D,$02,$00,$A4,$5D,$02,$00	; 02 info map button 2
		.BYTE	$C4,$5D,$02,$00,$E4,$5D,$02,$00	; 03 info map button 3
		.BYTE	$04,$5E,$02,$00,$24,$5E,$02,$00	; 04 info map button 4
		.BYTE	$44,$5E,$02,$00,$64,$5E,$02,$00	; 05 info map button 5
		.BYTE	$84,$5E,$02,$00,$A4,$5E,$02,$00	; 06 info map button 6
		.BYTE	$C4,$5E,$02,$00,$E4,$5E,$02,$00	; 07 info map button 7
		.BYTE	$23,$5D,$02,$00,$43,$5D,$02,$00	; 08 graphs button 0
		.BYTE	$63,$5D,$02,$00,$83,$5D,$02,$00	; 09 graphs button 1
		.BYTE	$A3,$5D,$02,$00,$C3,$5D,$02,$00	; 0A graphs button 2
		.BYTE	$E3,$5D,$02,$00,$03,$5E,$02,$00	; 0B graphs button 3
		.BYTE	$23,$5E,$02,$00,$43,$5E,$02,$00	; 0C graphs button 4
		.BYTE	$63,$5E,$02,$00,$83,$5E,$02,$00	; 0D graphs button 5

_info_buttons_extnt_lib:
		.BYTE	$43,$43	; 00
		.BYTE	$43,$43	; 01
		.BYTE	$83,$43	; 02
		.BYTE	$83,$83 ; 03
		.BYTE	$83,$83 ; 04
		.BYTE	$43,$43 ; 05
		.BYTE	$43,$43 ; 06
		.BYTE	$83,$83 ; 07
		.BYTE	$43,$43 ; 08
		.BYTE	$83,$83 ; 09
		.BYTE	$83,$83 ; 0A
		.BYTE	$03,$03 ; 0B
		.BYTE	$43,$43 ; 0C
		.BYTE	$83,$83 ; 0D
		.BYTE	$C3,$C3 ; 0E
		.BYTE	$03,$03 ; 0F
		.BYTE	$03,$03 ; 10
		.BYTE	$03,$83 ; 11

; REDUNDANT, returned back to bank 9 where it belongs
;
; =============== S U B	R O U T	I N E =======================================
;_scr_res_hndl1_A_graphs_draw_init:
;		LDY		#$FF
;		STY		_cur_graphs_button_press_idx
;		STY		_prev_graphs_button_press_idx
;		RTS

; =============== S U B	R O U T	I N E =======================================
_graphs_period_button_redraw:
		LDY		_graphs_scr_period_mode			; swap two cmall buttons here
		LDA		_graphs_period_buttons_extnt_data,Y
		LDY		#$00
		JSR		_graphs_period_button_extnt_send
		LDA		_graphs_scr_period_mode
		EOR		#$01
		TAY
		LDA		_graphs_period_buttons_extnt_data,Y
		LDY		#$04

; !FALLTHROUGH!

; =============== S U B	R O U T	I N E =======================================
_graphs_period_button_extnt_send:
		STA		_tmp2
		EXTQSTART
loc_A0238:
		LDA		_graphs_period_buttons_extnt_hdrs,Y	; very smart solution here
		BNE		loc_A0245						; and very dumb before now
		EXTQMOVB	_tmp2,OP_NOP,0
		LDY		#$FE
loc_A0245:
		EXTQMOVA								; queue is 2 byte long
		INY										; the second byte is 0
		CPY		#$FF
		BNE		loc_A0238
		EXTQEND
		RTS

_graphs_period_buttons_extnt_data:
		.BYTE	$84,$83
_graphs_period_buttons_extnt_hdrs:
		.BYTE	$C3,$5E,$02,$00,$03,$5F,$02,$00

; =============== S U B	R O U T	I N E =======================================
_info_maps_header_and_legend_redraw:
		JSR		_buffers_wait_flush				; routine from BANK9!
		LDA		_cur_info_maps_button_idx		; convert cur menu idx into
		BEQ		loc_A0262						; if map header idx = 0
		CMP		#$0D							; special case
		BEQ		loc_A0266
		CLC										; all bove 0 are 3 shifted now then
		ADC		#$03
		BPL		loc_A0266
loc_A0262:
		CLC										; then select one of 4 maps idxes
		ADC		_info_maps_menu_extra_idx		; adding index of city map menu
loc_A0266:
		PHA										; OPTIMIZED
		MOVWO	_ptr0,_info_screen_header_nt_lib; OPTIMIZED
		MOVWO	_tmp2,_ppu_queue_buf
		PLA										; OPTIMIZED
		PHA										; -
		PPUQSTART
		STX		_tmp4
		JSR		_strcpy_ex
		PPUQEND
		MOVWO	_ptr0,_info_screen_legends_nt_lib
		PPUQSTART								; because of additional list for nt idxes
		JSR		_strcpy_lib
		PPUQEND
;		JSR		_buffers_wait_flush				; FIX, one wait just enough, or else you will see how it draw
		MOVWO	_ptr0,_info_screen_header_extnt_lib
		MOVWO	_tmp2,_extnt_queue_buf
		PLA										; OPTIMIZED
		EXTQSTART
		STX		_tmp4
		JSR		_strcpy_ex
		EXTQEND
		MOVWO	_ptr0,_info_screen_legends_extnt_lib
		EXTQSTART
		JSR		_strcpy_lib
		EXTQEND
		JMP		_buffers_wait_flush

; =============== S U B	R O U T	I N E =======================================
_strcpy_lib:
		STX		_tmp4
		LDX		_cur_info_maps_button_idx
		LDA		_info_screen_legends_idx_list,X

; =============== S U B	R O U T	I N E =======================================
_strcpy_ex:
		ASL										; calc idx
		TAY
		LDA		(_ptr0),Y						; fetch ptr
		TAX
		INY
		LDA		(_ptr0),Y
		STX		_ptr0							; store ptr
		STA		_ptr0+1
		LDY		#$00
		STY		_tmp5							; set src pos to 0
		BEQ		loc_A02FD
loc_A02F1:
		LDY		_tmp4
		STA		(_tmp2),Y						; store dst byte
		INC		_tmp4							; increment dst, can't be more than 256
		INC		_tmp5							; increment src, and hi ptr nibble if >256
		BNE		loc_A02FD
		INC		_ptr0+1
loc_A02FD:
		LDY		_tmp5							; repeat
		LDA		(_ptr0),Y						; read src byte
		CMP		#$FF
		BNE		loc_A02F1						; send if not end of buf
		LDX		_tmp4							; X = head pos of dst buf
		RTS

_info_screen_legends_idx_list:
		.BYTE	$00,$01,$02,$03,$02,$02,$02,$02,$02,$02,$02,$02,$02,$04
_info_screen_legends_nt_lib:
		.WORD	_info_screen_legend_nt00	; trees/rivers/buildings/clear 4-color
		.WORD	_info_screen_legend_nt01	; 2-buttons on/off
		.WORD	_info_screen_legend_nt02	; + pos -
		.WORD	_info_screen_legend_nt03	; min/max
		.WORD	_info_screen_legend_nt04	; clear
_info_screen_legends_extnt_lib:
		.WORD	_info_screen_legend_extnt00
		.WORD	_info_screen_legend_extnt01
		.WORD	_info_screen_legend_extnt02
		.WORD	_info_screen_legend_extnt03
		.WORD	_info_screen_legend_extnt04
_info_screen_legend_nt00:
		.BYTE	$82,$25,$8C,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$83
		.BYTE	$25,$8C,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$A2,$24
		.BYTE	$08,$DE,$69,$6A,$6B,$DE,$6C,$6D,$6E,$C2,$24,$09,$DE,$79,$7A,$7B
		.BYTE	$51,$7C,$7D,$7E,$7F,$FF
_info_screen_legend_extnt00:
		.BYTE	$82,$5D,$8C,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$83
		.BYTE	$5D,$8C,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$A2,$5C
		.BYTE	$08,$C1,$43,$43,$43,$01,$43,$43,$43,$C2,$5C,$09,$41,$43,$43,$43
		.BYTE	$50,$43,$43,$43,$43,$FF
_info_screen_legend_nt01:
		.BYTE	$82,$25,$8C,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$22,$32,$9D,$A4,$83
		.BYTE	$25,$8C,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$23,$33,$9E,$A5,$A2,$24
		.BYTE	$07,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$C2,$24,$07,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$FF
_info_screen_legend_extnt01:
		.BYTE	$82,$5D,$8C,$01,$01,$01,$01,$01,$01,$01,$01,$84,$84,$04,$04,$83
		.BYTE	$5D,$8C,$01,$01,$01,$01,$01,$01,$01,$01,$84,$84,$04,$04,$A2,$5C
		.BYTE	$07,$01,$01,$01,$01,$01,$01,$01,$C2,$5C,$07,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$FF
_info_screen_legend_nt02:
		.BYTE	$82,$25,$8C,$DE,$DE,$22,$50,$00,$50,$28,$50,$00,$28,$5F,$32,$83
		.BYTE	$25,$8C,$DE,$DE,$23,$50,$00,$50,$28,$50,$00,$28,$5F,$33,$A2,$24
		.BYTE	$07,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$C2,$24,$07,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$FF
_info_screen_legend_extnt02:
		.BYTE	$82,$5D,$8C,$01,$01,$83,$90,$D0,$50,$D0,$D0,$10,$10,$10,$83,$83
		.BYTE	$5D,$8C,$01,$01,$83,$90,$D0,$50,$D0,$D0,$10,$10,$10,$83,$A2,$5C
		.BYTE	$07,$01,$01,$01,$01,$01,$01,$01,$C2,$5C,$07,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$FF
_info_screen_legend_nt03:
		.BYTE	$82,$25,$8C,$DE,$DE,$8E,$50,$50,$00,$BE,$CE,$28,$50,$50,$9E,$83
		.BYTE	$25,$8C,$DE,$DE,$8F,$50,$50,$00,$BF,$CF,$28,$50,$50,$9F,$A2,$24
		.BYTE	$07,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$C2,$24,$07,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$FF
_info_screen_legend_extnt03:
		.BYTE	$82,$5D,$8C,$01,$01,$83,$90,$90,$D0,$C3,$C3,$D0,$D0,$D0,$83,$83
		.BYTE	$5D,$8C,$01,$01,$83,$90,$90,$D0,$C3,$C3,$D0,$D0,$D0,$83,$A2,$5C
		.BYTE	$07,$01,$01,$01,$01,$01,$01,$01,$C2,$5C,$07,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$FF
_info_screen_legend_nt04:
		.BYTE	$82,$25,$8C,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$83
		.BYTE	$25,$8C,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$DE,$A2,$24
		.BYTE	$08,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$C2,$24,$09,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$FF
_info_screen_legend_extnt04:
		.BYTE	$82,$5D,$8C,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$83
		.BYTE	$5D,$8C,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$A2,$5C
		.BYTE	$08,$01,$01,$01,$01,$01,$01,$01,$01,$C2,$5C,$09,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$FF
_info_screen_header_nt_lib:
		.WORD	_info_screen_header_nt00		; City Map Comprehensive
		.WORD	_info_screen_header_nt01		; City Map Residental
		.WORD	_info_screen_header_nt02		; City Map Commercial
		.WORD	_info_screen_header_nt03		; City Map Industrial
		.WORD	_info_screen_header_nt04		; Power Grid
		.WORD	_info_screen_header_nt05		; Population Dencity
		.WORD	_info_screen_header_nt06		; Population Rate of Growth
		.WORD	_info_screen_header_nt07		; Traffic Dencity
		.WORD	_info_screen_header_nt08		; Pollution Dencity
		.WORD	_info_screen_header_nt09		; Crime Rate
		.WORD	_info_screen_header_nt0A		; Land Value
		.WORD	_info_screen_header_nt0B		; City Service Fire Radius
		.WORD	_info_screen_header_nt0C		; City Service Police Radius
		.WORD	_info_screen_header_nt0D		; -blank header- used to clear line
_info_screen_header_extnt_lib:
		.WORD	_info_screen_header_extnt00
		.WORD	_info_screen_header_extnt00
		.WORD	_info_screen_header_extnt00
		.WORD	_info_screen_header_extnt00
		.WORD	_info_screen_header_extnt04
		.WORD	_info_screen_header_extnt05
		.WORD	_info_screen_header_extnt05
		.WORD	_info_screen_header_extnt07
		.WORD	_info_screen_header_extnt08
		.WORD	_info_screen_header_extnt04
		.WORD	_info_screen_header_extnt04
		.WORD	_info_screen_header_extnt00
		.WORD	_info_screen_header_extnt00
		.WORD	_info_screen_header_extnt0D
_info_screen_header_nt00:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$20,$20,$20,$43,$69,$74,$79,$20,$4D,$61
		.BYTE	$70,$20,$20,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$43,$6F,$6D,$70
		.BYTE	$72,$65,$68,$65,$6E,$73,$69,$76,$65,$DF,$DF,$DF,$FF
_info_screen_header_nt01:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$20,$20,$20,$43,$69,$74,$79,$20,$4D,$61
		.BYTE	$70,$20,$20,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$52,$65,$73
		.BYTE	$69,$64,$65,$6E,$74,$69,$61,$6C,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt02:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$20,$20,$20,$43,$69,$74,$79,$20,$4D,$61
		.BYTE	$70,$20,$20,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$20,$43,$6F
		.BYTE	$6D,$6D,$65,$72,$63,$69,$61,$6C,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt03:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$20,$20,$20,$43,$69,$74,$79,$20,$4D,$61
		.BYTE	$70,$20,$20,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$20,$49,$6E
		.BYTE	$64,$75,$73,$74,$72,$69,$61,$6C,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt0B:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$20,$43,$69,$74,$79,$20,$53,$65,$72,$76
		.BYTE	$69,$63,$65,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$46,$69,$72
		.BYTE	$65,$20,$52,$61,$64,$69,$75,$73,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt0C:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$20,$43,$69,$74,$79,$20,$53,$65,$72,$76
		.BYTE	$69,$63,$65,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$50,$6F,$6C,$69
		.BYTE	$63,$65,$20,$52,$61,$64,$69,$75,$73,$DF,$DF,$DF,$FF
_info_screen_header_extnt00:
		.BYTE	$A9,$5C,$13,$01,$01,$01,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$01,$01,$01,$C9,$5C,$13,$01,$01,$01,$0B,$0B,$0B,$0B
		.BYTE	$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$01,$01,$01,$FF
_info_screen_header_nt04:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$50,$6F,$77
		.BYTE	$65,$72,$20,$47,$72,$69,$64,$20,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt09:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$43,$72,$69
		.BYTE	$6D,$65,$20,$52,$61,$74,$65,$20,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt0A:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$DF,$20,$4C,$61,$6E
		.BYTE	$64,$20,$56,$61,$6C,$75,$65,$20,$20,$DF,$DF,$DF,$FF
_info_screen_header_extnt04:
		.BYTE	$A9,$5C,$13,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$C9,$5C,$13,$01,$01,$01,$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$01,$01,$01,$FF
_info_screen_header_nt05:
		.BYTE	$A9,$24,$13,$DF,$DF,$20,$20,$50,$6F,$70,$75,$6C,$61,$74,$69,$6F
		.BYTE	$6E,$20,$20,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$20,$20,$20,$20,$44
		.BYTE	$65,$6E,$73,$69,$74,$79,$20,$20,$20,$DF,$DF,$DF,$FF
_info_screen_header_nt06:
		.BYTE	$A9,$24,$13,$DF,$DF,$20,$20,$50,$6F,$70,$75,$6C,$61,$74,$69,$6F
		.BYTE	$6E,$20,$20,$DF,$DF,$DF,$C9,$24,$13,$DF,$DF,$52,$61,$74,$65,$20
		.BYTE	$6F,$66,$20,$47,$72,$6F,$77,$74,$68,$DF,$DF,$DF,$FF
_info_screen_header_extnt05:
		.BYTE	$A9,$5C,$13,$01,$01,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$01,$01,$01,$C9,$5C,$13,$01,$01,$0B,$0B,$0B,$0B,$0B
		.BYTE	$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$0B,$01,$01,$01,$FF
_info_screen_header_nt07:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$C9,$24,$13,$DF,$20,$54,$72,$61,$66,$66
		.BYTE	$69,$63,$20,$44,$65,$6E,$73,$69,$74,$79,$20,$DF,$FF
_info_screen_header_extnt07:
		.BYTE	$A9,$5C,$13,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$C9,$5C,$13,$01,$8B,$8B,$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$01,$FF
_info_screen_header_nt08:
		.BYTE	$A9,$24,$13,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$C9,$24,$13,$20,$50,$6F,$6C,$6C,$75,$74
		.BYTE	$69,$6F,$6E,$20,$44,$65,$6E,$73,$69,$74,$79,$20,$FF
_info_screen_header_extnt08:
		.BYTE	$A9,$5C,$13,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$C9,$5C,$13,$8B,$8B,$8B,$8B,$8B,$8B,$8B
		.BYTE	$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$8B,$FF
_info_screen_header_nt0D:
		.BYTE	$A2,$24,$1B,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$C2,$24
		.BYTE	$1B,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF
		.BYTE	$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$DF,$FF
_info_screen_header_extnt0D:
		.BYTE	$A2,$5C,$1B,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$C2,$5C
		.BYTE	$1B,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01
		.BYTE	$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$01,$FF

		SECTION_END 7904

		SECTION_START 902

; =============== S U B	R O U T	I N E =======================================
_scr_common_draw:
		LDA		_scr_res_idx					; _scr_res_idx here already *2
		BNE		loc_A0A9A
		LDA		#$10							; here is the only place with cursor reposition
		STA		_cur_game_cursor_scr_pos._COL	; other screens uses its own cursors or
		LDA		#$62							; ingame cursor position (at HUD info maps button)
		STA		_cur_game_cursor_scr_pos._ROW
		LDX		#$24							; for info maps screen
		LDA		#$DF
		JSR		_ppu_area_fill					; clear NT1 with DF (space)
		LDX		#$2C
		LDA		#$01
		JSR		_ppu_area_fill					; clear EXTNT mapper do NT3 with 01
		MOVWO	_ptr0,_tlm_info_maps_nt_common
;		JSR		_tlm_transfer					; REDUNDANT, merged
;		MOVWO	_ptr0,_tlm_info_maps_extnt_common
		JMP		_tlm_transfer
loc_A0A9A:
; OPTIMIZED, 156 bytes less
; -
		CMP		#$0C
		BNE		loc_A0A9B
		BIT		_city._bank_flags
		BVS		loc_A0A9B
		LDA		#$10
loc_A0A9B:
		TAY
		LDX		_scr_tlm_lib-2,Y
		LDA		_scr_tlm_lib-1,Y
		TAY
		PUSHB	_mmc5_nt_mode_shadow
		LDA		#$24
		STA		_MMC5_NT_MODE
		JSR		_tlm_res_unpack
		POPB	_MMC5_NT_MODE
		RTS
; -
; REDUNDANT
;		CMP		#$02
;		BNE		loc_A0AA1
;		JMP		_scr_res1_draw
;loc_A0AA1:
;		CMP		#$04
;		BNE		loc_A0AA8
;		JMP		_scr_res2_draw
;loc_A0AA8:
;		CMP		#$06
;		BNE		loc_A0AAF
;		JMP		_scr_res3_draw
;loc_A0AAF:
;		CMP		#$08
;		BNE		loc_A0AB6
;		JMP		_scr_res4_draw
;loc_A0AB6:
;		CMP		#$0A
;		BNE		loc_A0ABD
;		JMP		_scr_res5_draw
;loc_A0ABD:
;		CMP		#$0C
;		BNE		loc_A0AC4
;		JMP		_scr_res6_draw
;loc_A0AC4:
;		CMP		#$0E
;		BNE		locret_A0ACB
;		JMP		_scr_res7_draw
;locret_A0ACB:
;		RTS
;_scr_res1_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		JSRXY	_tlm_res_unpack,_tlm_res_scr1
;		POPB	_MMC5_NT_MODE
;		RTS
;_scr_res2_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		JSRXY	_tlm_res_unpack,_tlm_res_scr2
;		POPB	_MMC5_NT_MODE
;		RTS
;_scr_res3_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		JSRXY	_tlm_res_unpack,_tlm_res_scr3
;		POPB	_MMC5_NT_MODE
;		RTS
;_scr_res4_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		JSRXY	_tlm_res_unpack,_tlm_res_scr4
;		POPB	_MMC5_NT_MODE
;		RTS
;_scr_res5_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		JSRXY	_tlm_res_unpack,_tlm_res_scr5
;		POPB	_MMC5_NT_MODE
;		RTS
;_scr_res7_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		JSRXY	_tlm_res_unpack,_tlm_res_scr7
;		POPB	_MMC5_NT_MODE
;		RTS
;_scr_res6_draw:
;		PUSHB	_mmc5_nt_mode_shadow
;		LDA		#$24
;		STA		_MMC5_NT_MODE
;		BIT		_city._bank_flags
;		BVC		loc_A0B5E
;		LDXY	_tlm_res_scr6A
;		BNE		loc_A0B62
;loc_A0B5E:
;		LDXY	_tlm_res_scr6B
;loc_A0B62:
;		JSR		_tlm_res_unpack
;		POPB	_MMC5_NT_MODE
;		RTS
; -

_scr_tlm_lib:
		.WORD	_tlm_res_scr1
		.WORD	_tlm_res_scr2
		.WORD	_tlm_res_scr3
		.WORD	_tlm_res_scr4
		.WORD	_tlm_res_scr5
		.WORD	_tlm_res_scr6A
		.WORD	_tlm_res_scr7
		.WORD	_tlm_res_scr6B

_tlm_info_maps_nt_common:
		.BYTE	$24,$6D,$01,$AF,$24,$E1,$08,$D0,$D1,$D1,$D1,$D1,$D2,$DF,$D0,$24
		.BYTE	$E9,$53,$D1,$24,$FC,$01,$D2,$25,$01,$D0,$D3,$25,$02,$84,$20,$30
		.BYTE	$8C,$9C,$25,$82,$CC,$DE,$25,$03,$84,$21,$31,$8D,$9D,$25,$83,$CC
		.BYTE	$DE,$25,$04,$90,$00,$10,$02,$12,$04,$14,$06,$16,$08,$18,$0A,$1A
		.BYTE	$0C,$1C,$0E,$1E,$25,$05,$90,$01,$11,$03,$13,$05,$15,$07,$17,$09
		.BYTE	$19,$0B,$1B,$0D,$1D,$0F,$1F,$25,$06,$D0,$D4,$27,$01,$06,$D5,$D6
		.BYTE	$D6,$D6,$D6,$D7,$25,$08,$D3,$D3,$25,$1C,$D3,$D4,$27,$68,$01,$D5
		.BYTE	$27,$69,$53,$D6,$27,$7C,$01,$D7	; ,$00							; REDUNDANT
;_tlm_info_maps_extnt_common:
		.BYTE	$2C,$6D,$01,$82,$2C,$7B,$43,$42,$2C,$E8,$55,$81,$2D,$02,$84,$83	; MERGED
		.BYTE	$83,$83,$83,$2D,$03,$84,$83,$83,$83,$83,$2D,$04,$90,$43,$43,$43
		.BYTE	$43,$83,$83,$83,$83,$83,$83,$43,$43,$43,$43,$83,$83,$2D,$05,$90
		.BYTE	$43,$43,$43,$43,$43,$43,$83,$83,$83,$83,$43,$43,$43,$43,$83,$83
		.BYTE	$2D,$08,$D3,$81,$2F,$68,$55,$81,$2D,$1C,$D3,$81,$00
_tlm_res_scr2:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr2_rle0
_tlm_scr2_rle0:
		.BYTE	$E4,$3F,$DF,$00,$A5,$3D,$A6,$01,$A7,$B5,$2A,$00,$64,$AB,$01,$C6
		.BYTE	$00,$65,$C0,$25,$00,$00,$B7,$8B,$00,$60,$64,$BB,$01,$D6,$00,$65
		.BYTE	$D0,$87,$00,$79,$00,$DC,$63,$AC,$38,$DC,$02,$B7,$B5,$DC,$63,$B0
		.BYTE	$8A,$00,$B5,$25,$B8,$00,$B9,$89,$00,$B8,$63,$C0,$22,$DC,$65,$30
		.BYTE	$08,$DC,$C7,$DF,$DF,$33,$DF,$6F,$C8,$C9,$89,$00,$B8,$63,$D0,$29
		.BYTE	$DC,$00,$D7,$25,$D8,$00,$D9,$89,$00,$B8,$62,$BD,$02,$C5,$DC,$DC
		.BYTE	$68,$36,$02,$DC,$DC,$24,$8D,$00,$B4,$00,$B7,$39,$B8,$00,$B9,$83
		.BYTE	$00,$BE,$00,$C7,$25,$DF,$64,$52,$00,$DF,$64,$57,$23,$DF,$63,$5C
		.BYTE	$01,$DF,$C9,$8A,$01,$5E,$64,$62,$00,$DF,$64,$67,$23,$DF,$62,$6C
		.BYTE	$00,$DF,$86,$01,$7C,$64,$40,$05,$DF,$32,$DF,$DF,$DF,$DF,$85,$01
		.BYTE	$A8,$02,$DF,$6F,$C8,$23,$DF,$00,$33,$86,$01,$7C,$64,$45,$9A,$01
		.BYTE	$A8,$62,$4A,$62,$42,$99,$01,$A9,$64,$4D,$9A,$01,$A8,$22,$DF,$02
		.BYTE	$42,$43,$44,$33,$DF,$05,$C9,$DC,$B7,$B5,$DC,$D7,$39,$D8,$03,$D9
		.BYTE	$DC,$B7,$B5,$3D,$DC,$9F,$02,$5F,$84,$02,$5F,$65,$8A,$06,$DC,$DC
		.BYTE	$DC,$DC,$DC,$DC,$24,$92,$02,$71,$68,$90,$96,$02,$AD,$29,$DC,$28
		.BYTE	$2F,$8C,$02,$77,$67,$99,$97,$02,$AC,$22,$DC,$00,$BA,$2F,$BB,$00
		.BYTE	$BC,$8D,$02,$79,$00,$CA,$22,$DD,$6A,$A1,$02,$DD,$DD,$CC,$8D,$02
		.BYTE	$79,$00,$DA,$2F,$DB,$00,$CB,$F0,$26,$02,$79,$00,$C5,$3D,$C6,$00
		.BYTE	$C7,$32,$FF,$23,$00,$87,$03,$CF,$04,$FF,$00,$44,$3C,$30,$83,$03
		.BYTE	$E0,$27,$FF,$23,$C3,$23,$3F,$27,$FF,$E4,$3F,$06,$E4,$20,$8A,$2A
		.BYTE	$58,$25,$4B,$86,$04,$6B,$25,$58,$9F,$04,$5F,$01,$8A,$8A,$3D,$05
		.BYTE	$9F,$04,$9F,$89,$04,$9F,$25,$45,$07,$05,$05,$C5,$C5,$02,$05,$C5
		.BYTE	$C5,$F0,$31,$04,$B7,$29,$05,$00,$0B,$F0,$39,$04,$B4,$05,$05,$45
		.BYTE	$45,$45,$45,$45,$87,$04,$E5,$98,$05,$5C,$05,$85,$85,$05,$45,$45
		.BYTE	$45,$87,$04,$BB,$85,$04,$EA,$00,$02,$23,$C5,$85,$05,$A8,$84,$04
		.BYTE	$F4,$02,$85,$85,$02,$9F,$05,$9C,$8B,$05,$9C,$00,$45,$8D,$05,$A9
		.BYTE	$00,$C5,$9E,$05,$B8,$8B,$05,$F7,$01,$05,$05,$87,$05,$97,$02,$05
		.BYTE	$05,$05,$87,$05,$F0,$01,$45,$45,$F0,$38,$04,$FA,$F0,$35,$04,$B3
		.BYTE	$92,$05,$2C,$9F,$06,$9C,$F0,$53,$06,$7C,$F0,$5F,$06,$50,$90,$04
		.BYTE	$B0,$3E,$8A,$3F,$00,$02,$38,$44,$08,$83,$07,$DF,$28,$00,$87,$03
		.BYTE	$F0,$27,$00,$FF
_tlm_res_scr5:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr5_rle0
_tlm_scr5_rle0:
		.BYTE	$E4,$41,$DD,$00,$A5,$39,$A6,$00,$A7,$23,$DD,$00,$B5,$25,$D1,$0C
		.BYTE	$0C,$01,$13,$14,$D1,$3D,$D1,$05,$16,$05,$0E,$14,$13,$26,$D1,$00
		.BYTE	$B7,$8A,$00,$5E,$0C,$4C,$41,$53,$54,$D1,$7D,$D1,$45,$56,$45,$4E
		.BYTE	$54,$53,$8C,$00,$76,$39,$78,$85,$00,$7D,$39,$20,$9F,$00,$BD,$85
		.BYTE	$00,$7D,$39,$76,$F0,$5F,$00,$BD,$F0,$BF,$00,$BD,$F1,$05,$00,$BD
		.BYTE	$39,$98,$84,$00,$7D,$00,$C5,$39,$C6,$00,$C7,$E4,$41,$DD,$E4,$3F
		.BYTE	$00,$E4,$41,$01,$3B,$4A,$84,$04,$3E,$39,$18,$9F,$04,$5D,$85,$04
		.BYTE	$5D,$39,$94,$85,$04,$5D,$39,$8B,$9F,$04,$BD,$85,$04,$5D,$39,$14
		.BYTE	$F0,$5F,$04,$BD,$F0,$BF,$04,$BD,$F1,$05,$04,$BD,$39,$94,$00,$4A
		.BYTE	$F0,$23,$04,$3E,$E4,$3D,$01,$27,$00,$22,$10,$83,$07,$C7,$14,$10
		.BYTE	$00,$00,$00,$00,$00,$00,$00,$00,$10,$10,$10,$10,$10,$10,$10,$10
		.BYTE	$10,$10,$10,$10,$93,$07,$CC,$27,$00,$FF
_tlm_res_scr7:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr7_rle0
_tlm_scr7_rle0:
		.BYTE	$E4,$21,$DD,$00,$A5,$32,$A6,$00,$A7,$2A,$DD,$14,$B5,$00,$07,$25
		.BYTE	$2E,$25,$32,$21,$34,$25,$00,$0E,$25,$37,$00,$0D,$21,$30,$00,$00
		.BYTE	$B7,$8C,$00,$37,$0F,$47,$65,$6E,$65,$72,$61,$54,$65,$00,$4E,$65
		.BYTE	$77,$00,$4D,$61,$70,$8F,$00,$54,$2E,$00,$00,$B1,$8D,$00,$54,$00
		.BYTE	$C5,$32,$C6,$00,$C7,$E4,$2A,$DD,$00,$C0,$32,$C1,$02,$C2,$DD,$A8
		.BYTE	$23,$A9,$01,$AA,$DE,$22,$DD,$00,$D0,$32,$DD,$07,$D2,$DD,$B8,$4E
		.BYTE	$45,$58,$54,$BA,$9A,$00,$FE,$05,$4D,$41,$50,$20,$BA,$DD,$98,$00
		.BYTE	$FF,$00,$C8,$23,$C9,$00,$CA,$99,$01,$3E,$85,$00,$F8,$99,$01,$3E
		.BYTE	$04,$B8,$50,$4C,$41,$59,$9A,$01,$3D,$04,$B8,$47,$41,$4D,$45,$F0
		.BYTE	$3A,$01,$3D,$05,$DD,$DD,$DD,$DD,$DD,$DD,$9F,$01,$DE,$F0,$3F,$01
		.BYTE	$DE,$F0,$5A,$01,$DE,$64,$BB,$9A,$01,$DE,$64,$CB,$9A,$01,$DE,$00
		.BYTE	$DB,$22,$DC,$00,$DF,$98,$01,$3E,$63,$E9,$02,$EC,$EC,$EF,$98,$01
		.BYTE	$3E,$63,$F9,$02,$FC,$FC,$FF,$9B,$01,$DE,$01,$DF,$DF,$84,$02,$FD
		.BYTE	$01,$DD,$E0,$32,$E1,$00,$E2,$22,$DD,$22,$D9,$E4,$42,$DD,$E4,$3F
		.BYTE	$FF,$E4,$21,$46,$34,$0A,$8B,$04,$17,$32,$18,$9F,$04,$36,$9C,$04
		.BYTE	$36,$03,$01,$18,$18,$0A,$F0,$2A,$04,$17,$3F,$46,$34,$58,$00,$46
		.BYTE	$25,$4A,$00,$49,$83,$04,$DF,$32,$46,$02,$58,$46,$4A,$23,$8B,$9F
		.BYTE	$04,$FD,$01,$4A,$46,$99,$04,$FF,$03,$4A,$4A,$4A,$4A,$9F,$05,$3D
		.BYTE	$9B,$05,$3D,$9F,$05,$39,$F0,$3E,$05,$39,$05,$46,$46,$46,$46,$46
		.BYTE	$46,$9F,$05,$DE,$F0,$3F,$05,$DE,$F0,$5A,$05,$DE,$9F,$05,$D9,$9F
		.BYTE	$05,$D9,$9D,$05,$59,$00,$4A,$9E,$05,$58,$9F,$07,$17,$22,$46,$22
		.BYTE	$03,$9A,$04,$DD,$04,$46,$46,$4A,$4A,$4A,$E4,$42,$46,$E4,$3F,$00
		.BYTE	$FF
_tlm_res_scr6A:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr6A_rle0
_tlm_scr6A_rle0:
		.BYTE	$E4,$22,$DC,$00,$00,$37,$01,$00,$02,$26,$DC,$37,$11,$27,$DC,$00
		.BYTE	$21,$4A,$22,$12,$00,$23,$28,$DC,$66,$04,$00,$05,$86,$00,$86,$65
		.BYTE	$05,$00,$0C,$29,$DC,$62,$14,$01,$3F,$4F,$62,$19,$86,$00,$A6,$85
		.BYTE	$00,$A6,$00,$1C,$29,$DC,$0B,$24,$1F,$2F,$DC,$DC,$5F,$6F,$2B,$25
		.BYTE	$26,$DC,$DC,$62,$29,$85,$00,$C6,$00,$2C,$29,$DC,$01,$34,$0F,$23
		.BYTE	$DC,$08,$7F,$3B,$35,$DF,$58,$AC,$AD,$3A,$3B,$85,$00,$E6,$00,$3C
		.BYTE	$29,$DC,$00,$30,$86,$00,$FF,$00,$DF,$63,$A8,$01,$DF,$30,$8C,$00
		.BYTE	$FF,$23,$DC,$00,$31,$86,$01,$1F,$00,$DF,$63,$B8,$01,$DF,$31,$8C
		.BYTE	$01,$1F,$22,$DC,$0A,$32,$33,$DF,$DF,$58,$59,$DF,$5A,$33,$DF,$DF
		.BYTE	$62,$C9,$00,$5A,$87,$01,$45,$27,$DC,$00,$36,$24,$37,$6D,$60,$24
		.BYTE	$37,$00,$38,$25,$DC,$02,$10,$27,$28,$62,$40,$6D,$70,$62,$43,$01
		.BYTE	$27,$28,$27,$DC,$01,$27,$28,$62,$50,$6D,$80,$62,$53,$8B,$01,$9A
		.BYTE	$62,$46,$6D,$90,$62,$49,$87,$01,$9A,$00,$60,$39,$61,$00,$62,$23
		.BYTE	$DC,$17,$66,$49,$2A,$6D,$DC,$73,$6F,$72,$72,$79,$DC,$62,$75,$74
		.BYTE	$DC,$79,$6F,$75,$2A,$72,$65,$DC,$74,$6F,$22,$DC,$00,$67,$84,$01
		.BYTE	$FE,$0A,$66,$61,$72,$DC,$69,$6E,$DC,$64,$65,$70,$74,$83,$02,$17
		.BYTE	$0A,$67,$65,$74,$DC,$61,$6E,$6F,$74,$68,$65,$72,$85,$02,$1D,$17
		.BYTE	$6C,$6F,$61,$6E,$2E,$DC,$DC,$43,$6F,$6D,$65,$DC,$62,$61,$63,$6B
		.BYTE	$DC,$61,$6E,$64,$DC,$73,$65,$65,$87,$02,$1B,$06,$75,$73,$DC,$77
		.BYTE	$68,$65,$6E,$83,$02,$10,$84,$02,$31,$83,$02,$6B,$03,$68,$65,$61
		.BYTE	$64,$87,$02,$1B,$0B,$61,$62,$6F,$76,$65,$DC,$77,$61,$74,$65,$72
		.BYTE	$2E,$2D,$DC,$85,$02,$1D,$39,$DC,$9F,$02,$9D,$89,$02,$9D,$0D,$F0
		.BYTE	$0C,$0F,$01,$0E,$13,$F1,$DC,$EE,$CF,$CA,$CA,$DC,$EF,$92,$02,$D5
		.BYTE	$0C,$4C,$4F,$41,$4E,$53,$F1,$DC,$FE,$DF,$DA,$DA,$DC,$FF,$8A,$02
		.BYTE	$95,$01,$DD,$DD,$9B,$02,$A2,$03,$DD,$DD,$CC,$CC,$9B,$02,$A2,$04
		.BYTE	$CC,$CD,$DD,$DD,$63,$39,$64,$07,$65,$CD,$CC,$DD,$CD,$CD,$CC,$DD
		.BYTE	$41,$DD,$CC,$00,$CC,$83,$03,$82,$04,$DD,$DD,$CD,$DD,$CC,$83,$03
		.BYTE	$88,$01,$CC,$CC,$26,$DD,$84,$03,$97,$00,$CD,$86,$03,$9B,$84,$03
		.BYTE	$95,$85,$03,$98,$01,$CD,$CD,$83,$03,$AF,$01,$CC,$CC,$25,$00,$01
		.BYTE	$FF,$FF,$27,$DF,$87,$03,$C0,$07,$00,$01,$07,$1F,$01,$04,$10,$40
		.BYTE	$23,$FF,$23,$00,$27,$DF,$8F,$03,$E0,$E4,$43,$88,$37,$08,$9E,$04
		.BYTE	$3B,$9F,$04,$3A,$8C,$04,$3A,$03,$48,$48,$08,$08,$86,$04,$A5,$04
		.BYTE	$08,$08,$08,$48,$48,$8D,$04,$78,$05,$48,$48,$C8,$C8,$48,$48,$86
		.BYTE	$04,$C5,$06,$08,$48,$48,$C8,$C8,$48,$48,$8C,$04,$BA,$23,$C8,$0D
		.BYTE	$48,$08,$48,$01,$08,$C8,$C8,$48,$08,$48,$C8,$C8,$C8,$C8,$8C,$04
		.BYTE	$D9,$25,$C8,$01,$08,$01,$23,$C8,$07,$01,$08,$C8,$C8,$C8,$C8,$C8
		.BYTE	$C8,$9F,$04,$FA,$8B,$04,$7A,$07,$01,$01,$08,$08,$01,$08,$08,$01
		.BYTE	$83,$05,$0D,$08,$08,$08,$01,$01,$08,$08,$01,$08,$08,$E4,$28,$88
		.BYTE	$95,$05,$59,$01,$08,$08,$9F,$05,$7C,$F0,$25,$05,$7C,$3B,$06,$84
		.BYTE	$05,$DE,$22,$CB,$06,$C8,$CB,$CB,$CB,$CB,$CB,$C8,$88,$06,$03,$83
		.BYTE	$06,$05,$22,$C8,$8B,$05,$FD,$05,$C8,$CB,$CB,$CB,$CB,$C8,$8C,$06
		.BYTE	$0A,$00,$CB,$88,$05,$FD,$08,$CB,$CB,$C8,$C8,$CB,$CB,$CB,$CB,$C8
		.BYTE	$8B,$06,$08,$07,$C8,$C8,$06,$88,$88,$88,$88,$06,$8F,$06,$4C,$04
		.BYTE	$CB,$CB,$CB,$C8,$CB,$8C,$06,$58,$09,$CB,$CB,$CB,$C8,$CB,$CB,$CB
		.BYTE	$CB,$CB,$CB,$2D,$C8,$85,$05,$FD,$39,$C8,$9F,$06,$9D,$89,$06,$9D
		.BYTE	$00,$D8,$24,$18,$01,$D8,$C8,$23,$D8,$01,$C8,$D8,$9F,$06,$D5,$F0
		.BYTE	$4D,$06,$95,$3A,$06,$E4,$41,$88,$87,$03,$C0,$27,$01,$88,$03,$D0
		.BYTE	$0D,$03,$0F,$3F,$02,$08,$20,$80,$FF,$FF,$FF,$FF,$00,$00,$00,$88
		.BYTE	$00,$23,$8F,$07,$E0,$FF
_tlm_res_scr6B:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr6B_rle0
_tlm_scr6B_rle0:
		.BYTE	$E4,$22,$DC,$00,$00,$37,$01,$00,$02,$26,$DC,$37,$11,$27,$DC,$00
		.BYTE	$21,$4A,$22,$12,$00,$23,$28,$DC,$66,$04,$00,$05,$86,$00,$86,$65
		.BYTE	$05,$00,$0C,$29,$DC,$62,$14,$01,$3F,$4F,$62,$19,$86,$00,$A6,$85
		.BYTE	$00,$A6,$00,$1C,$29,$DC,$07,$24,$1F,$2F,$DC,$DC,$5F,$6F,$2B,$86
		.BYTE	$00,$C6,$85,$00,$C6,$00,$2C,$29,$DC,$01,$34,$0F,$23,$DC,$01,$7F
		.BYTE	$3B,$86,$00,$E6,$85,$00,$E6,$00,$3C,$29,$DC,$00,$30,$86,$00,$FF
		.BYTE	$06,$24,$31,$30,$30,$30,$30,$30,$8C,$00,$FF,$23,$DC,$00,$31,$86
		.BYTE	$01,$1F,$8D,$01,$1F,$28,$DC,$07,$32,$33,$DF,$DF,$58,$59,$DF,$5A
		.BYTE	$86,$01,$45,$87,$01,$45,$27,$DC,$00,$36,$24,$37,$6D,$60,$24,$37
		.BYTE	$00,$38,$25,$DC,$02,$10,$27,$28,$62,$40,$6D,$70,$62,$43,$01,$27
		.BYTE	$28,$27,$DC,$01,$27,$28,$62,$50,$6D,$80,$62,$53,$8B,$01,$9A,$62
		.BYTE	$46,$6D,$90,$62,$49,$87,$01,$9A,$00,$60,$39,$61,$00,$62,$23,$DC
		.BYTE	$11,$66,$48,$65,$6C,$6C,$6F,$2C,$74,$68,$69,$73,$DC,$69,$73,$DC
		.BYTE	$74,$68,$65,$28,$DC,$00,$67,$84,$01,$FE,$29,$DC,$0F,$62,$61,$6E
		.BYTE	$6B,$2E,$DC,$48,$6F,$77,$DC,$63,$61,$6E,$DC,$77,$65,$85,$02,$1D
		.BYTE	$0C,$68,$65,$6C,$70,$DC,$79,$6F,$75,$3F,$DC,$DC,$49,$66,$83,$02
		.BYTE	$47,$04,$DC,$6E,$65,$65,$64,$89,$02,$19,$07,$6D,$6F,$6E,$65,$79
		.BYTE	$DC,$77,$65,$84,$02,$36,$07,$6C,$6F,$61,$6E,$DC,$79,$6F,$75,$8A
		.BYTE	$02,$18,$85,$01,$0D,$05,$2E,$DC,$DC,$41,$6C,$6C,$84,$02,$50,$06
		.BYTE	$68,$61,$76,$65,$DC,$74,$6F,$87,$02,$1B,$01,$64,$6F,$83,$02,$0D
		.BYTE	$12,$70,$61,$79,$DC,$24,$35,$30,$30,$DC,$65,$76,$65,$72,$79,$DC
		.BYTE	$79,$65,$61,$72,$86,$02,$1C,$0A,$66,$6F,$72,$DC,$6F,$6E,$6C,$79
		.BYTE	$DC,$32,$31,$84,$02,$B7,$09,$73,$2E,$DC,$DC,$57,$68,$61,$74,$DC
		.BYTE	$61,$86,$02,$9D,$12,$65,$61,$6C,$21,$DC,$59,$6F,$75,$DC,$77,$6F
		.BYTE	$6E,$2A,$74,$DC,$66,$69,$6E,$64,$84,$02,$6F,$00,$73,$85,$02,$1D
		.BYTE	$08,$6C,$69,$6B,$65,$DC,$74,$68,$69,$73,$86,$02,$B1,$08,$64,$61
		.BYTE	$79,$2E,$DC,$57,$68,$61,$74,$89,$02,$9C,$83,$02,$51,$03,$73,$61
		.BYTE	$79,$3F,$2E,$DC,$05,$67,$DC,$DC,$DD,$DD,$66,$2D,$DC,$63,$E8,$01
		.BYTE	$DC,$DC,$63,$EC,$06,$DC,$DC,$67,$DD,$DD,$CC,$CC,$8E,$03,$42,$63
		.BYTE	$F8,$01,$DC,$DC,$63,$FC,$04,$DC,$DC,$67,$CC,$CD,$90,$03,$40,$8C
		.BYTE	$03,$31,$04,$CD,$CC,$DD,$CD,$63,$39,$64,$03,$65,$DD,$DD,$FF,$22
		.BYTE	$00,$03,$FF,$FF,$00,$00,$27,$DF,$87,$03,$C0,$87,$03,$C0,$05,$FF
		.BYTE	$00,$03,$03,$FF,$FE,$8A,$03,$C6,$00,$FF,$83,$03,$C4,$89,$03,$C6
		.BYTE	$E4,$43,$88,$37,$08,$9E,$04,$3B,$9F,$04,$3A,$8C,$04,$3A,$03,$48
		.BYTE	$48,$08,$08,$86,$04,$A5,$04,$08,$08,$08,$48,$48,$8D,$04,$78,$05
		.BYTE	$48,$48,$C8,$C8,$48,$48,$86,$04,$C5,$06,$08,$48,$48,$C8,$C8,$48
		.BYTE	$48,$8C,$04,$BA,$23,$C8,$00,$48,$86,$04,$E5,$05,$08,$48,$C8,$C8
		.BYTE	$C8,$C8,$8C,$04,$D9,$25,$C8,$00,$08,$25,$CB,$06,$08,$C8,$C8,$C8
		.BYTE	$C8,$C8,$C8,$92,$04,$FA,$05,$C8,$C8,$C8,$C8,$C8,$C8,$92,$05,$13
		.BYTE	$05,$01,$01,$08,$08,$01,$08,$86,$05,$45,$07,$08,$01,$01,$08,$08
		.BYTE	$01,$08,$08,$E4,$28,$88,$95,$05,$59,$01,$08,$08,$9F,$05,$7C,$F0
		.BYTE	$25,$05,$7C,$3B,$06,$84,$05,$DE,$29,$CB,$06,$C8,$CB,$CB,$C8,$CB
		.BYTE	$CB,$CB,$28,$C8,$85,$05,$FD,$29,$C8,$87,$06,$08,$85,$06,$0F,$01
		.BYTE	$CB,$CB,$89,$05,$FD,$84,$06,$2C,$01,$C8,$C8,$89,$06,$30,$0A,$CB
		.BYTE	$C8,$C8,$C8,$C8,$06,$88,$88,$88,$88,$06,$8C,$06,$08,$08,$CB,$CB
		.BYTE	$CB,$CB,$C8,$CB,$CB,$CB,$C8,$8E,$06,$59,$04,$CB,$CB,$C8,$C8,$CB
		.BYTE	$8D,$06,$69,$07,$C8,$C8,$06,$88,$88,$88,$88,$06,$91,$06,$66,$0D
		.BYTE	$CB,$CB,$C8,$CB,$CB,$CB,$CB,$C8,$06,$88,$88,$88,$88,$06,$8B,$06
		.BYTE	$90,$8A,$06,$84,$0E,$CB,$C8,$CB,$06,$88,$88,$88,$88,$06,$CB,$CB
		.BYTE	$CB,$CB,$CB,$C8,$8E,$06,$AE,$02,$CB,$CB,$CB,$91,$06,$3B,$05,$CB
		.BYTE	$CB,$CB,$CB,$CB,$C8,$91,$06,$B3,$0E,$C8,$CB,$CB,$CB,$C8,$CB,$CB
		.BYTE	$CB,$CB,$C8,$C8,$C8,$C8,$C8,$C8,$98,$06,$14,$84,$06,$19,$22,$06
		.BYTE	$85,$07,$4F,$9F,$07,$3B,$95,$07,$3B,$91,$07,$31,$3A,$06,$01,$88
		.BYTE	$88,$85,$03,$C0,$01,$FF,$FF,$27,$01,$87,$07,$C0,$87,$07,$C0,$07
		.BYTE	$FF,$01,$03,$03,$FF,$FC,$F8,$F8,$88,$07,$C8,$05,$FF,$FF,$FF,$00
		.BYTE	$00,$00,$88,$00,$23,$FF
_tlm_res_scr1:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr1_rle0
_tlm_scr1_rle0:
		.BYTE	$E4,$49,$DC,$00,$A0,$2D,$A1,$00,$A2,$2F,$DC,$00,$B0,$23,$DE,$05
		.BYTE	$07,$12,$01,$10,$08,$13,$23,$DE,$00,$B2,$94,$00,$5A,$05,$47,$52
		.BYTE	$41,$50,$68,$53,$94,$00,$75,$00,$C0,$2D,$C1,$00,$C2,$E4,$2C,$DC
		.BYTE	$00,$C3,$33,$C4,$00,$C5,$24,$DC,$05,$D0,$D1,$D1,$D2,$DC,$D3,$33
		.BYTE	$DD,$00,$D5,$24,$DC,$03,$D3,$80,$81,$D4,$9C,$01,$06,$01,$90,$91
		.BYTE	$9D,$01,$25,$01,$82,$83,$9D,$01,$25,$01,$92,$93,$9D,$01,$25,$01
		.BYTE	$84,$85,$9D,$01,$25,$01,$94,$95,$9D,$01,$25,$01,$86,$87,$9D,$01
		.BYTE	$25,$01,$96,$97,$9D,$01,$25,$01,$88,$89,$9D,$01,$25,$01,$98,$99
		.BYTE	$9D,$01,$25,$01,$8A,$8B,$9D,$01,$25,$01,$9A,$9B,$9D,$01,$25,$01
		.BYTE	$DE,$DE,$9D,$01,$25,$01,$A0,$A1,$9D,$01,$25,$01,$AA,$AB,$9D,$01
		.BYTE	$25,$05,$A2,$A3,$D4,$DC,$D3,$C6,$48,$C6,$C7,$00,$C6,$85,$01,$1C
		.BYTE	$03,$D5,$D6,$D6,$D7,$9B,$01,$06,$24,$DC,$00,$E3,$33,$E4,$00,$E5
		.BYTE	$E4,$62,$DC,$2F,$FF,$04,$00,$FF,$FF,$00,$00,$8B,$03,$C5,$06,$FC
		.BYTE	$FC,$7C,$00,$FE,$FE,$BE,$8C,$03,$C4,$00,$00,$89,$03,$D3,$E4,$49
		.BYTE	$01,$2F,$41,$90,$04,$3A,$23,$81,$25,$98,$23,$81,$9F,$04,$59,$91
		.BYTE	$04,$59,$9E,$04,$4B,$3C,$01,$35,$18,$24,$01,$23,$81,$01,$01,$18
		.BYTE	$33,$01,$86,$04,$FC,$01,$83,$83,$9F,$05,$05,$9D,$05,$05,$01,$03
		.BYTE	$03,$9F,$05,$45,$9D,$05,$05,$01,$43,$43,$9F,$05,$85,$9D,$05,$05
		.BYTE	$01,$C3,$C3,$9F,$05,$C5,$F0,$3F,$05,$C5,$F0,$5D,$05,$45,$9F,$05
		.BYTE	$03,$9F,$05,$63,$F0,$24,$05,$43,$33,$41,$F0,$25,$04,$FC,$9F,$04
		.BYTE	$E2,$E4,$5D,$01,$2F,$00,$00,$FF,$85,$03,$D1,$28,$00,$22,$FC,$04
		.BYTE	$7C,$02,$FE,$FE,$3E,$8A,$07,$C8,$00,$FF,$2B,$00,$FF
_tlm_res_scr4:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr4_rle0
_tlm_scr4_rle0:
		.BYTE	$E4,$40,$B6,$00,$A5,$3B,$A6,$03,$A7,$B6,$B6,$B5,$27,$00,$0C,$AE
		.BYTE	$AC,$C5,$CA,$00,$C9,$C8,$C4,$C7,$C8,$AC,$C4,$CB,$26,$00,$00,$B7
		.BYTE	$8A,$00,$5F,$0C,$BE,$BC,$D5,$DA,$00,$D9,$D8,$D4,$D7,$D8,$BC,$D4
		.BYTE	$DB,$8A,$00,$77,$3B,$7C,$85,$00,$9E,$05,$5A,$4F,$4E,$49,$4E,$47
		.BYTE	$33,$7C,$93,$00,$9E,$62,$21,$23,$7C,$00,$5B,$88,$00,$BA,$64,$62
		.BYTE	$00,$5B,$26,$7C,$01,$5A,$7C,$64,$24,$8B,$00,$F7,$64,$67,$89,$01
		.BYTE	$08,$00,$29,$62,$48,$8C,$00,$F6,$64,$6C,$89,$01,$08,$01,$71,$43
		.BYTE	$63,$29,$01,$7C,$5B,$97,$00,$DA,$02,$72,$3D,$3E,$63,$29,$98,$01
		.BYTE	$59,$62,$75,$00,$55,$8C,$00,$F6,$64,$0A,$89,$01,$08,$03,$21,$4F
		.BYTE	$74,$51,$8C,$00,$F6,$01,$0F,$10,$62,$0B,$00,$4F,$88,$01,$09,$00
		.BYTE	$72,$64,$2E,$99,$01,$58,$90,$00,$B2,$62,$11,$0A,$7C,$7C,$5B,$7C
		.BYTE	$7C,$7C,$7C,$7C,$7C,$7C,$5A,$9F,$00,$D1,$64,$33,$84,$00,$F8,$62
		.BYTE	$5F,$84,$00,$9E,$62,$14,$02,$7C,$7C,$5B,$33,$7C,$00,$5A,$92,$00
		.BYTE	$9E,$63,$38,$08,$36,$37,$5B,$7C,$7C,$7C,$5F,$60,$61,$9E,$00,$9E
		.BYTE	$85,$02,$5D,$62,$17,$85,$00,$F7,$04,$5E,$7C,$5C,$5D,$7C,$65,$3C
		.BYTE	$00,$78,$8A,$02,$38,$0C,$1F,$20,$7C,$7C,$7C,$5B,$7C,$7C,$7C,$5E
		.BYTE	$7C,$5C,$5D,$92,$02,$50,$64,$1A,$88,$02,$A8,$65,$43,$F0,$2B,$02
		.BYTE	$77,$3A,$7C,$F0,$22,$00,$9E,$00,$C5,$3B,$C6,$00,$C7,$E4,$40,$B6
		.BYTE	$E4,$3F,$7C,$E4,$40,$8A,$3D,$4A,$02,$8A,$8A,$4A,$27,$D8,$23,$CB
		.BYTE	$00,$D8,$27,$CB,$26,$D8,$9F,$04,$5E,$83,$04,$5E,$3B,$15,$85,$04
		.BYTE	$9E,$25,$D9,$33,$15,$93,$04,$9E,$22,$95,$84,$04,$EE,$88,$04,$BA
		.BYTE	$05,$95,$95,$95,$95,$95,$95,$87,$04,$EB,$05,$15,$95,$95,$95,$95
		.BYTE	$95,$9B,$04,$F7,$22,$06,$9C,$04,$F6,$06,$06,$95,$95,$95,$95,$15
		.BYTE	$95,$98,$04,$DA,$05,$06,$06,$95,$95,$95,$95,$99,$05,$59,$01,$95
		.BYTE	$95,$9F,$05,$35,$9D,$05,$35,$00,$95,$9D,$05,$54,$90,$04,$B2,$89
		.BYTE	$05,$14,$03,$15,$15,$15,$95,$9F,$04,$D1,$89,$05,$D3,$22,$95,$91
		.BYTE	$05,$FE,$8D,$04,$E5,$97,$06,$1E,$06,$95,$95,$15,$15,$15,$95,$95
		.BYTE	$93,$06,$5D,$91,$06,$51,$88,$05,$14,$03,$95,$15,$95,$95,$92,$06
		.BYTE	$70,$0C,$95,$95,$15,$15,$15,$95,$15,$15,$15,$95,$15,$95,$95,$95
		.BYTE	$06,$90,$01,$95,$95,$9A,$06,$A8,$39,$15,$9F,$06,$7D,$F0,$24,$05
		.BYTE	$DD,$3C,$4A,$E4,$40,$8A,$E4,$3F,$15,$FF
_tlm_res_scr3:
		.WORD	$2400
		.BYTE	1
		.WORD	_tlm_scr3_rle0
_tlm_scr3_rle0:
		.BYTE	$4F,$5B,$5C,$4F,$5C,$5B,$00,$C3,$3D,$C4,$01,$C5,$D3,$29,$00,$0E
		.BYTE	$03,$09,$14,$19,$00,$05,$16,$01,$0C,$15,$01,$14,$09,$0F,$0E,$24
		.BYTE	$00,$00,$D5,$8A,$00,$60,$0E,$43,$49,$54,$59,$00,$45,$76,$41,$4C
		.BYTE	$55,$41,$54,$49,$4F,$4E,$86,$00,$7A,$3D,$DD,$03,$D5,$D3,$20,$2D
		.BYTE	$2B,$1E,$01,$2E,$19,$8B,$00,$C2,$E0,$24,$2E,$21,$D5,$D3,$22,$50
		.BYTE	$55,$42,$4C,$49,$43,$DC,$4F,$50,$49,$4E,$49,$4F,$4E,$1A,$DC,$53
		.BYTE	$54,$41,$54,$49,$53,$54,$49,$43,$53,$DC,$DC,$23,$D5,$D3,$22,$2D
		.BYTE	$DC,$00,$1A,$2C,$DC,$83,$00,$FE,$6C,$70,$01,$6D,$1A,$65,$E0,$04
		.BYTE	$DC,$DC,$DC,$DC,$DC,$F0,$25,$00,$FC,$24,$DC,$04,$25,$DC,$22,$0E
		.BYTE	$1C,$84,$01,$0C,$66,$E6,$02,$DC,$DC,$24,$95,$01,$3B,$62,$ED,$62
		.BYTE	$CD,$91,$01,$57,$01,$17,$18,$95,$01,$0B,$02,$83,$81,$82,$8C,$01
		.BYTE	$04,$68,$F0,$86,$01,$1A,$02,$DD,$83,$84,$2A,$1F,$0F,$2F,$1B,$DC
		.BYTE	$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$23,$90,$00,$9F
		.BYTE	$0E,$22,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$DC,$23
		.BYTE	$90,$00,$BF,$00,$1C,$63,$8C,$00,$9F,$8B,$01,$16,$6D,$60,$96,$01
		.BYTE	$50,$89,$01,$07,$66,$A6,$8C,$01,$38,$00,$25,$F0,$20,$01,$46,$89
		.BYTE	$01,$07,$2C,$6F,$93,$02,$7E,$69,$B0,$9B,$00,$FC,$00,$BA,$97,$02
		.BYTE	$79,$67,$C0,$F0,$2B,$01,$39,$8B,$02,$85,$67,$C8,$87,$01,$19,$01
		.BYTE	$24,$29,$2B,$1F,$01,$2F,$1D,$8B,$03,$62,$01,$2F,$25,$F0,$20,$00
		.BYTE	$9F,$00,$E3,$3D,$E4,$00,$E5,$32,$FF,$23,$00,$87,$03,$CF,$04,$FF
		.BYTE	$00,$44,$3C,$30,$83,$03,$E0,$27,$FF,$23,$C3,$23,$3F,$27,$FF,$E4
		.BYTE	$3F,$C6,$E4,$20,$58,$3D,$98,$9F,$04,$5F,$01,$58,$58,$3D,$83,$01
		.BYTE	$58,$58,$3D,$86,$02,$58,$58,$86,$25,$0B,$00,$05,$26,$0B,$01,$86
		.BYTE	$05,$29,$0B,$01,$05,$05,$83,$04,$DE,$2D,$05,$8D,$05,$01,$83,$04
		.BYTE	$DE,$2D,$04,$86,$05,$01,$00,$C5,$25,$85,$99,$04,$FE,$8E,$05,$38
		.BYTE	$00,$8B,$23,$85,$8D,$05,$0C,$00,$0B,$9C,$05,$3B,$03,$05,$05,$05
		.BYTE	$05,$8A,$04,$FC,$00,$0B,$99,$05,$08,$01,$86,$86,$9C,$05,$04,$0E
		.BYTE	$83,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$86,$91
		.BYTE	$05,$D0,$2D,$83,$93,$05,$B0,$2C,$86,$24,$04,$F0,$20,$05,$16,$04
		.BYTE	$05,$05,$05,$05,$05,$94,$04,$FC,$06,$04,$04,$04,$04,$04,$04,$04
		.BYTE	$8C,$05,$98,$00,$8B,$F0,$20,$05,$86,$89,$05,$07,$2C,$84,$03,$86
		.BYTE	$58,$58,$86,$8B,$05,$A4,$03,$05,$05,$86,$85,$29,$04,$9B,$04,$FC
		.BYTE	$05,$04,$05,$05,$05,$05,$05,$92,$06,$BE,$07,$04,$04,$04,$04,$04
		.BYTE	$04,$04,$04,$97,$06,$59,$93,$05,$91,$9C,$07,$05,$3C,$86,$F0,$21
		.BYTE	$04,$9F,$3E,$58,$3F,$00,$02,$38,$44,$08,$83,$07,$DF,$28,$00,$87
		.BYTE	$03,$F0,$27,$00,$FF

		SECTION_END 902

		BANK_END F6,$A000
