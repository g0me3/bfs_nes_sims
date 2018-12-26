;-------------C-O-N-S-T-A-N-T-S-----------------

DEBUG	EQU		1

PRG0	EQU		$F0
PRG1	EQU		$F1
PRG2	EQU		$F2
PRG3	EQU		$F3
PRG4	EQU		$F4
PRG5	EQU		$F5
PRG6	EQU		$F6
PRG7	EQU		$F7
PRG8	EQU		$F8
PRG9	EQU		$F9
PRGA	EQU		$FA
PRGB	EQU		$FB
PRGC	EQU		$FC
PRGD	EQU		$FD
SRAM	EQU		$00

;--------------------A-P-U----------------------
;						    idxes	  flags (old)
_SE_IDX_CLICK		EQU		$0001	; $0001 - regular click
_SE_IDX_ERROR		EQU		$0002	; $0002	- error buzzer
_SE_IDX_SBUILD		EQU		$0003	; $0004	- smal build sound
_SE_IDX_LBUILD		EQU		$0004	; $0008	- large build sound
_SE_IDX_ALERT		EQU		$0005	; $0010	- alert buzzer
_SE_IDX_ROLLOUT		EQU		$0006	; $0020	- menu open
_SE_IDX_ROLLIN		EQU		$0007	; $0040	- menu close
_SE_IDX_0080		EQU		$0008	; $0080	- *unused* click
_SE_IDX_MONSTER		EQU		$0100	; $0100	; monster dpcm
_SE_IDX_TRAFFIC		EQU		$0200	; $0200	; traffic report dpcm
_SE_IDX_DESTROY		EQU		$0300	; $0400	; explode/destroy
_SE_IDX_BULLDOZE	EQU		$0400	; $0800	; bulldoze sound
_SE_IDX_1000		EQU		$0500	; $1000	; *unused* alert
_SE_IDX_BCLICK		EQU		$0600	; $2000	; button press
_SE_IDX_TORNADO		EQU		$0700	; $3000	; tornado sound
_SE_IDX_SELECT		EQU		$0800	; $4000	; selection menus sound
_SE_IDX_8000		EQU		$0900	; $8000	; *unused* cancel

	MAC	APUA_SE
	#if {1}<$100
		LDA		#{1}
		STA		_apu_se_idx_req
	#else
		LDA		#[{1}>>8]
		STA		_apu_se_idx_req+1
	#endif
	ENDM

	MAC	APUX_SE
	#if {1}<$100
		LDX		#{1}
		STX		_apu_se_idx_req
	#else
		LDX		#[{1}>>8]
		STX		_apu_se_idx_req+1
	#endif
	ENDM

;							idx		flags
_MUS_IDX_TITLE0		EQU		$01	;	$01
_MUS_IDX_TITLE1		EQU		$02	;	$02
_MUS_IDX_SETUP		EQU		$03	;	$04
_MUS_IDX_VILLAGE0	EQU		$04	;	$08
_MUS_IDX_VILLAGE1	EQU		$05	;	$10
_MUS_IDX_TOWN		EQU		$06	;	$20
_MUS_IDX_CITY		EQU		$07	;	$41
_MUS_IDX_CAPITAL	EQU		$08	;	$42
_MUS_IDX_METROPOLIS	EQU		$09	;	$44
_MUS_IDX_MEGAPOLIS0	EQU		$0A	;	$48
_MUS_IDX_MEGAPOLIS1 EQU		$0B	;	$50
_MUS_IDX_NOTICE0	EQU		$0C	;	$60
_MUS_IDX_NOTICE1	EQU		$0D	;	$40
_MUS_IDX_ALERT0		EQU		$0E	;	$21
_MUS_IDX_ALERT1		EQU		$0F	;	$22
_MUS_IDX_STOP		EQU		$80

;------------S-T-R-U-C-T-U-R-E-S----------------

	MAC	_tfix16_16
{1}		SET	.
{1}.FRAC	.WORD	0
{1}.INT		.WORD	0
	ENDM

	MAC	_tfix8_8
{1}		SET	.
{1}.FRAC	.BYTE	0
{1}.INT		.BYTE	0
	ENDM


	MAC	_tpos
{1}		SET	.
{1}._COL:		.BYTE	0
{1}._ROW:		.BYTE	0
	ENDM

R_AREA_BIDX	EQU	0
C_AREA_BIDX	EQU	1
I_AREA_BIDX	EQU	2

	MAC	_trci
{1}		SET	.
{1}._R:		.BYTE	0
{1}._C:		.BYTE	0
{1}._I:		.BYTE	0
	ENDM

R_AREA_WIDX	EQU	0
C_AREA_WIDX	EQU 2
I_AREA_WIDX	EQU 4

	MAC	_trciw
{1}		SET	.
{1}._R:		.WORD	0
{1}._C:		.WORD	0
{1}._I:		.WORD	0
	ENDM

	MAC	_trcifix8_8
{1}			SET	.
{1}.FRAC	SET	.
{1}.INT		SET [.+1]
		_tfix8_8	{1}._R
		_tfix8_8	{1}._C
		_tfix8_8	{1}._I
	ENDM

	MAC	_tcity
{1}										SET	.
{1}._name:								.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._sister_name:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._bank_name:							.BYTE	0,0,0,0,0,0,0,0,0,0,0
; REDUNDANT, remove by now, free some space for more savestates
;{1}._unused_name3:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
;{1}._unused_name4:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
;
; FIX, new vars for SAVE/LOAD state proper work
; moved here to prevent erasing at the game init
{1}._cur_game_mode:						.BYTE	0
{1}._scenario_idx:						.BYTE	0
; FIX, difficult workaround don't need anymore
{1}._difficult:							.BYTE	0
; -
{1}._scenarios_clear_flags:				.BYTE	0
{1}._all_scenarios_are_clear_ram_flag:	.BYTE	0
		_trcifix8_8						{1}._level
		_trci							{1}._delta
		_tfix8_8						{1}._traf_lvl_avr
		_tfix8_8						{1}._land_val_avr
		_tfix8_8						{1}._crime_lvl_avr
		_tfix8_8						{1}._poll_lvl_avr
{1}._city_type:							.BYTE	0
{1}._score:								.WORD	0
{1}._money:								.BYTE	0,0,0
{1}._cur_year:							.WORD	0
{1}._cur_month:							.BYTE	0
{1}._cur_week:							.BYTE	0
{1}._tax_rate:							.BYTE	0
{1}._game_flags:						.BYTE	0
	; bit0 - auto buldozer
	; bit1 - auto budget (unused)
	; bit2 - auto goto
	; bit3 - music on/off
	; bit4 - debug mode (free construction)
{1}._game_speed:						.BYTE	0
{1}._events_flags:						.BYTE	0,0,0,0
{1}._year_to_bank:						.WORD	0
{1}._year_to_expo:						.WORD	0
{1}._year_to_liberty:					.WORD	0
{1}._presents_list:						.BYTE	0,0,0,0
{1}._history_list:						.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{1}._bank_flags:						.BYTE	0
	; bit0 - bank built, wait for loan message
	; bit6 - money loaned, wait for end of period
	; bit7 - loan message enabled, may loan now
{1}._tax_year_sum:						.WORD	0
{1}._loan_period:						.BYTE	0
; FIX, reinitialized before game, or erased or loadable now
{1}._didaster_delay:					.BYTE	0
{1}._disaster_end:						.BYTE	0
										_tpos	{1}._wnd_pos
{1}._menu_pos							.BYTE	0,0,0,0,0,0,0,0,0,0,0,0
{1}_size								SET	[.-{1}]
	ENDM

	MAC	_treq
{1}		SET	.
{1}._stadium:	.BYTE	0
{1}._airport:	.BYTE	0
{1}._seaport:	.BYTE	0
	ENDM

DEVELOPED_WIDX		EQU 0
UNDEVELOPED_WIDX	EQU	2
OTHER_WIDX			EQU 4

	MAC	_tdev
{1}		SET	.
{1}._developed:		.WORD	0
{1}._undeveloped:	.WORD	0
{1}._other:			.WORD	0
{1}._total:			.WORD	0
	ENDM

	MAC	_tobj6w
{1}		SET	.
{1}._COL	SET	.
{1}._ROW	SET	[.+1]
		_tpos {1}._heli
		_tpos {1}._plane
		_tpos {1}._ship
		_tpos {1}._train
		_tpos {1}._monster
		_tpos {1}._tornado
	ENDM

	MAC	_tobj4b
{1}		SET	.
{1}._heli:		.BYTE	0
{1}._plane:		.BYTE	0
{1}._ship:		.BYTE	0
{1}._train:		.BYTE	0
	ENDM

	MAC	_tfund8
{1}		SET	.
{1}._fire:		.BYTE	0
{1}._police:	.BYTE	0
{1}._trans:		.BYTE	0
{1}._education:	.BYTE	0
{1}._unk:		.BYTE	0
	ENDM

	MAC	_tfund16
{1}		SET	.
{1}._tax:			.WORD	0
{1}._fire:			.WORD	0
{1}._police:		.WORD	0
{1}._trans:			.WORD	0
{1}._education:		.WORD	0
	ENDM

	MAC	_tsprarg
{1}		SET	.
{1}._attr:		.BYTE	0
{1}._idx:		.BYTE	0
		_tpos	{1}._pos
{1}._clip:		.BYTE	0
	ENDM

	MAC	_tsprargraw
{1}		SET	.
{1}._idx:		.BYTE	0
		_tpos	{1}._pos
	ENDM

	MAC	_tgraph
{1}		SET	.
	.BYTE	0,0,0,0,0,0,0,0,0,0
	ENDM

	MAC	_tyear
{1}		SET	.
		_tgraph	{1}._R
		_tgraph	{1}._C
		_tgraph	{1}._I
		_tgraph	{1}._pollution
		_tgraph	{1}._money
		_tgraph	{1}._crime
	ENDM

	MAC	_tgraphs
{1}		SET	.
		_tyear {1}._10y
		_tyear {1}._100y
{1}_size	SET [.-{1}]
	ENDM

	MAC	_tarray
{1}		SET	.
		DS	{2}
{1}_end	SET	.
{1}_size SET [{1}_end-{1}]
	ENDM

;---------G-A-M-E--S-P-E-C-I-F-I-C--------------

	; \t\tSTX\t\t_MMC5_MUL1\n\t\tSTA\t\t_MMC5_MUL0\n\t\tLDA\t\t_MMC5_MUL0\n\t\tLDX\t\t_MMC5_MUL1\n
	; \t\tMULXA\n
	;
	MAC	MULXA
		STX		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDX		_MMC5_MUL1
	ENDM

	; \t\tSTA\t\t_MMC5_MUL1\n\t\tSTX\t\t_MMC5_MUL0\n\t\tLDX\t\t_MMC5_MUL0\n\t\tLDA\t\t_MMC5_MUL1\n
	; \t\tMULXA\n
	;
	MAC	MULAX
		STA		_MMC5_MUL1
		STX		_MMC5_MUL0
		LDX		_MMC5_MUL0
		LDA		_MMC5_MUL1
	ENDM

	; \t\tSTY\t\t_MMC5_MUL1\n\t\tSTA\t\t_MMC5_MUL0\n\t\tLDA\t\t_MMC5_MUL0\n\t\tLDY\t\t_MMC5_MUL1\n
	; \t\tMULYA\n
	;
	MAC	MULYA
		STY		_MMC5_MUL1
		STA		_MMC5_MUL0
		LDA		_MMC5_MUL0
		LDY		_MMC5_MUL1
	ENDM

	; \t\tSTA\t\t_mmc5_mul1_shadow\n\t\tSTA\t\t_MMC5_MUL1\n\t\tLDA\t\t(.*)\n\t\tSTA\t\t_mmc5_mul0_shadow\n\t\tSTA\t\t_MMC5_MUL0\n
	; \t\tMULAI\t\1\n
	;
	MAC	MULAI
		STA		_mmc5_mul1_shadow
		STA		_MMC5_MUL1
		LDA		{0}
		STA		_mmc5_mul0_shadow
		STA		_MMC5_MUL0
	ENDM

	MAC	FJSRA
		JSR		_farcall_argsAXY_safe
		.BYTE	{2},{3}
		.WORD	{1}
	ENDM

	MAC	FJSR
		JSR		_farcall_argsXY_safe
		.BYTE	{2},{3}
		.WORD	{1}
	ENDM

	; \t\tLDX\t\t#<\[(.*)\]\n\t\tLDY\t\t#>\[\1\]\n\t\tJSR\t\t(.*)\n
	; \t\tJSRXY\t\2,\1\n
	;
	MAC JSRXY
		LDX		#<[{2}]
		LDY		#>[{2}]
		JSR		{1}
	ENDM

	; \t\tLDX\t\t#<\[(.*)\]\n\t\tLDY\t\t#>\[\1\]\n\t\tJSR\t\t(.*)\n
	; \t\tJSRXY\t\2,\1\n
	;
	MAC JMPXY
		LDX		#<[{2}]
		LDY		#>[{2}]
		JMP		{1}
	ENDM

	; \t\tLDX\t\t#<\[(.*)\]\n\t\tLDY\t\t#>\[\1\]\n
	; \t\tLDXY\t\1\n
	;
	MAC	LDXY
		LDX		#<[{1}]
		LDY		#>[{1}]
	ENDM

	; \t\tPUSHB\t(.*)\n\t\tPUSHB\t(.*)\n\t\tJSR\t\t_mmc5_mul8to8\n\t\tPOPB\t(.*)\n\t\tPOPB\t\3\+1\n
	; \t\tMMULB8\t\3,\1,\2\n
	;
	; store-multiply, input args are 8bit, result is 16bit
	;
	MAC	MMULB8
		PUSHB	{2}
		PUSHB	{3}
		JSR		_mmc5_mul8to8
		POPB	{1}
		POPB	{1}+1
	ENDM

	MAC	MULA8
	ENDM

NONE	EQU		-1

	;\t\tLDA\t\t(.*)\n\t\tLSR\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tCMP\t\t(.*)\n\t\tBCC\t\t(.*)\n\t\tSBC\t\t\3\n\4:\n\t\tSTA\t\t(.*)\n
	;\t\tCIADDB\t\5,\1,\2,NONE,\3\n
	;
	; add mem to mem with extra immediate, clamp/wrap and store
	;
	MAC	CIADDB
		LDA		{2}
		LSR
		CLC
	#if {3}!=NONE
		ADC		{3}
	#endif
	#if {4}!=NONE
		ADC		{4}
	#endif
		CMP		{5}
		BCC		.1
		SBC		{5}
.1:
		STA		{1}
	ENDM

	;
	MAC	PPUQSTART
		LDX		_ppu_queue_pos_head
	ENDM

	MAC	PPUQEND
		STX		_ppu_queue_pos_head
	ENDM

	MAC	EXTQSTART
		LDX		_extnt_queue_pos_head
	ENDM

	MAC	EXTQEND
		STX		_extnt_queue_pos_head
	ENDM

	;\t\tSTA\t\t_ppu_queue_buf\,X\n\t\tINX\n
	;\t\tPPUQMOVA\n
	;
	; TODO: macrocify non-standart usage of such opcodes by rearrange
	; to the regular pattern types without logic changes
	;
	MAC	PPUQMOVA
		STA		_ppu_queue_buf,X
		INX
	ENDM

OP_NOP	EQU	-1
OP_ORA	EQU	1
OP_AND	EQU	2

	MAC	PPUQMOVB
		LDA		{1}
	#if {2}==OP_ORA
		ORA		{3}
	#endif
	#if {2}==OP_AND
		AND		{3}
	#endif
		STA		_ppu_queue_buf,X
		INX
	ENDM

	MAC	EXTQMOVA
		STA		_extnt_queue_buf,X
		INX
	ENDM

OP_NOP	EQU	-1
OP_ORA	EQU	1
OP_AND	EQU	2
OP_ADD	EQU	3

	MAC	EXTQMOVB
		LDA		{1}
	#if {2}==OP_ORA
		ORA		{3}
	#endif
	#if {2}==OP_AND
		AND		{3}
	#endif
	#if {2}==OP_ADD
		CLC
		ADC		{3}
	#endif
		STA		_extnt_queue_buf,X
		INX
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t(.*)\n\t\tCMP\t\t(.*)\n\t\tBCS\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t\3\n\t\tSEC\n\t\tSBC\t\t\5\n\t\tSTA\t\t(.*)\n\t\tBNE\t\t(.*)\n\4\:\n\t\tLDA\t\t\3\n\t\tSTA\t\t\5\n\t\tLDA\t\t\#\$00\n\t\tSTA\t\t\6\n\7\:\n
	; \t\tWRAPB\t\5\,\6\,\2\,\3\,\1\n
	;
	; usage: WRAPB LINELEN,WRAPLEN,POS,WIDTH,BUFLEN
	; 				 \5		 \6		\2	 \3     \1
	;			     {1}     {2}    {3}  {4}    {5}
	;
	MAC	WRAPB
		LDA		{5}		; \1
		SEC
		SBC		{3}		; \2
		CMP		{4}		; \3
		BCS		.1					; ORIGINAL: we check here if line is clamped, but
		BEQ		.1					; we will get here also if the size is exactly eaqual
		STA		{1}		; \5		; ant thus recalc values by hand... there is only one
		LDA		{4}		; \3		; place in bank D, that does BEQ check
		SEC							; FIX: let's get the same for all such places.
		SBC		{1}		; \5
		STA		{2}		; \6
		BNE		.2
.1:
		LDA		{4}		; \3
		STA		{1}		; \5
		LDA		#$00
		STA		{2}		; \6
.2:
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tBEQ\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t\#\$00\n\t\tSTA\t\t\1\n
	; \t\tDO_WRAP\t\3,\1,\2\n
	;
	;
	MAC	DO_WRAP
		LDA		{2}
		BEQ		{3}
		STA		{1}
		LDA		#$00
		STA		{2}
	ENDM

	; \t\tCLC\n\t\tADC\t\t(.*)\n\t\tCMP\t\t(.*)\n\t\tBCC\t\t(.*)\n\t\tSBC\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t\5\n\t\tSTA\t\t(.*)\n\t\tBNE\t\t(.*)\n\3\:\n\t\tLDA\t\t\6\n\t\tSTA\t\t\7\n\t\tLDA\t\t\#\$00\n\t\tSTA\t\t\5\n\8\:\n
	; \t\tWRAPA\t\7\,\5\,\6\,\2\n
	;
	; usage: WRAPA LINELEN,WRAPLEN,MAXLEN,BUFLEN
	;				\7		\5		\6		\2
	;			    {1} 	{2}		{3}		{4}
	;
	; the same as WRAPA, but only for constant parameters,
	; however, WRAPB can be used with constant parameters as well.
	;
	; TODO: check speed and size, decide wether to keep both methods, or use one
	; for all cases
	;
	MAC WRAPA
	#if {3}>2
		CLC
		ADC		{3}-1	; \6-1
		CMP		{4}		; \2
		BCC		.1
		SBC		{4}-1	; \2-1
		STA		{2}		; \5
		LDA		{3}		; \6
		SEC
		SBC		{2}		; \5
		STA		{1}		; \7
		BNE		.2
.1:
		LDA		{3}		; \6
		STA		{1}		; \7
		LDA		#$00
		STA		{2}		; \5
.2:
	#else
		CMP		{4}-1
		BNE		.1
		LDA		{3}-1
		STA		{1}
		STA		{2}
		BNE		.2
.1:
		LDA		{3}
		STA		{1}
		LDA		#$00
		STA		{2}
.2:
	#endif
	ENDM



	; \t\tLDA\t\t(.*)\n\t\tLDX\t\t(.*)\n\t\tMULXA\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tTXA\n\t\tADC\t\t(.*)\n\t\tSTA\t\t(.*)\n
	; \t\tBUFPOS\t\4\,\3\,\1\,\2\,\5\t\; DST=\(\1*\2+\3\)+\500\n
	;
	; usage: BUFPOS DST,X,Y,WIDTH,PAGE
	;				4   3 1 2     5
	;
	MAC	BUFPOS
	#if {5}==[[_city_map>>8]&$FF]
		LDX		{3}
		LDA		_city_map_row_pos_list_lo,X
		CLC
		ADC		{2}
		STA		{1}
		LDA		_city_map_row_pos_list_hi,X
		ADC		#$00
		STA		{1}+1
	#else
		LDA		{3}		; 1 y
		LDX		{4}		; 2 width
		MULXA
		CLC
		ADC		{2}		; 3 x
	#if {5}!=$00
		STA		{1}		; 4 dst
		TXA
		ADC		{5}		; 5 page
		STA		{1}+1	; 6 dst+1
	#else
		BCC		.1
		INX
.1:
		STA		{1}
		STX		{1}+1
	#endif
	#endif
	ENDM

	; tmp game specific, will optimize next iteration
	; \t\tLDA\t\t_map_bg_anim_counter\n\t\tAND\t\t\#\$03\n\t\tASL\n\t\tSTA\t\t_ptr0\n\t\tTXA\n\t\tASL\n\t\tASL\n\t\tASL\n\t\tORA\t\t_ptr0\n\t\tTAX\n\t\tLDA\t\t([^\,]*)\,X\n\t\tSTA\t\tword_6C\+1\n\t\tINX\n\t\tLDA\t\t\1\,X\n\t\tSTA\t\tword_6C\n\t\tSEC\n
	; \t\tLDA\t\t_map_bg_anim_counter\n\t\tAND\t\t\#\$01\n\t\tASL\n\t\tSTA\t\t_ptr0\n\t\tTXA\n\t\tASL\n\t\tASL\n\t\tORA\t\t_ptr0\n\t\tTAX\n\t\tLDA\t\t([^\,]*)\,X\n\t\tSTA\t\tword_6C\+1\n\t\tINX\n\t\tLDA\t\t\1\,X\n\t\tSTA\t\tword_6C\n\t\tSEC\n
	; ANIM_FETCH <lib>, <delay>
	;

	MAC ANIM_FETCH
		LDA		_map_bg_anim_counter
		AND		#{2}
		ASL
		STA		_ptr0
		TXA
	#if {2}=$01
		ASL
		ASL
	#endif
	#if {2}=$03
		ASL
		ASL
		ASL
	#endif
		ORA		_ptr0
		TAX
		LDA		{1},X
		STA		word_6C+1
		INX
		LDA		{1},X
		STA		word_6C
		SEC
	ENDM

;-----------------C-O-M-M-O-N-------------------

	MAC BANK_START
CUR_PRG_BANK	SET {1}
CUR_BANK_START	SET .
		ECHO	"BANK",CUR_PRG_BANK," ORG ",[CUR_BANK_START>>12],"{"
	ENDM

	MAC	BANK_END
_bank{1}_free	SET	[{2}-.]
	#if ${1}==$F0
_total_free	SET	_bank{1}_free
	#else
_total_free	SET _total_free+_bank{1}_free
	#endif
	#if .>{2}
;		ECHO	"BANK",CUR_PRG_BANK,"[ org =",[CUR_BANK_START>>12]," s =",[.-CUR_BANK_START],"]","EXCEEDED =",[0-_bank{1}_free]
		ECHO	"} s =",[.-CUR_BANK_START],"EXCEEDED =",[0-_bank{1}_free]
	#else
;		ECHO	"BANK",CUR_PRG_BANK,"[ org =",[CUR_BANK_START>>12]," s =",[.-CUR_BANK_START],"]","FREE =",_bank{1}_free
		ECHO	"} s =",[.-CUR_BANK_START],"FREE =",_bank{1}_free
	#endif
	#if .<{2}-1
		ORG		{2}-1
		.BYTE	$FF
	#endif
	ENDM

	MAC SECTION_START
_{1}_start	SET	.
	ENDM

	MAC SECTION_END
_{1}_end	SET .
		ECHO	" * block =",_{1}_end-_{1}_start,"[",${1},"]"
	ENDM

	MAC SECTION_ENDS
_{1}_end	SET .
		ECHO	" * block =",_{1}_end-_{1}_start,"[",{2},"]"
	ENDM

	MAC	LAST_BANK_END
_bank{1}_free	SET	[{2}-.-6]
_total_free	SET _total_free+_bank{1}_free
	#if .>[{2}-6]
;		ECHO	"BANK",CUR_PRG_BANK,"[ s =",[.-CUR_BANK_START],"]","EXCEEDED =",[0-_bank{1}_free]
		ECHO	"} s =",[.-CUR_BANK_START],"EXCEEDED =",[0-_bank{1}_free]
	#else
;		ECHO	"BANK",CUR_PRG_BANK,"[ s =",[.-CUR_BANK_START],"]","FREE =",_bank{1}_free
		ECHO	"} s =",[.-CUR_BANK_START],"FREE =",_bank{1}_free
	#endif
		ECHO	"-------------";
		ECHO	"total free =",_total_free
		ECHO	""
		ORG		{2}-6
		.WORD NMI
		.WORD RESET
		.WORD IRQ
	ENDM

	; simple pair of macroses, used for exporting the routine
	; from the external banks and unconditionally call it
	; use it when there are no options (always far)
	;
	MAC PTR_OFS
{1}_pidx	SET	[[. - CUR_BANK_START] >> 1]
{1}_bank	SET CUR_PRG_BANK
		.WORD	{1}
	ENDM

	MAC	PTR_IDX
		.BYTE	{1}_bank,{1}_pidx
	ENDM

;---------------A-R-I-T-H-M---------------------

	MAC	NEGA
		EOR		#$FF
		CLC
		ADC		#$01
	ENDM

	; \t\tLDA\t\t([^\,^\n]*)\n\t\tPHA\n
	; \t\tPUSHB\t\1\n
	;
	MAC	PUSHB
		LDA		{1}
		PHA
	ENDM

	MAC	PUSHWI
		PUSHB	#[[{1}>>8]&$FF]
		PUSHB	#[[{1}>>0]&$FF]
	ENDM

	MAC PUSHW
		PUSHB	{1}+1
		PUSHB	{1}
	ENDM

	; \t\tPLA\n\t\tSTA\t\t([^\,^\n]*)\n
	; \t\tPOPB\t\1\n
	;
	MAC	POPB
		PLA
		STA		{1}
	ENDM

	MAC	POPW
		POPB	{1}
		POPB	{1}+1
	ENDM

	MAC	POPD
		POPB	{1}
		POPB	{1}+1
		POPB	{1}+2
	ENDM

	MAC POPADDD
		PLA
		CLC
		ADC		{2}
		STA		{1}
		PLA
		ADC		{2}+1
		STA		{1}+1
		PLA
		ADC		{2}+2
		STA		{1}+2
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t\1\+1\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVW\t\2,\1\n
	;
	MAC	MOVW
		LDA		{2}
		STA		{1}
		LDA		{2}+1
		STA		{1}+1
	ENDM

	MAC	MOVD
		LDA		{2}
		STA		{1}
		LDA		{2}+1
		STA		{1}+1
		LDA		{2}+2
		STA		{1}+2
	ENDM

	; \t\tLDA\t\t#<\[(.*)\]\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#>\[\1\]\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVWO\t\2,\1\n
	;
	MAC	MOVWO
		LDA		#<[{2}]
		STA		{1}
		LDA		#>[{2}]
		STA		{1}+1
	ENDM

	; \t\tLDA\t\t#\$(..)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#\$(..)\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVWI\t\2,$\3\1\n
	;
	MAC	MOVWI
	#if {2}==0
		LDA		#0
		STA		{1}
		STA		{1}+1
	#else
		LDA		#[[{2}>>0]&$FF]
		STA		{1}
		LDA		#[[{2}>>8]&$FF]
		STA		{1}+1
	#endif
	ENDM

	; \t\tLDA\t\t#\$(..)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#\$(..)\n\t\tSTA\t\t\2\+1\n\t\tLDA\t\t#\$(..)\n\t\tSTA\t\t\2\+2\n
	; \t\tMOVDI\t\2,$\4\3\1\n
	;
	MAC	MOVDI
	#if {2}==0
		LDA		#0
		STA		{1}
		STA		{1}+1
		STA		{1}+2
	#else
		LDA		#[[{2}>>0 ]&$FF]
		STA		{1}
		LDA		#[[{2}>>8 ]&$FF]
		STA		{1}+1
		LDA		#[[{2}>>16]&$FF]
		STA		{1}+2
	#endif
	ENDM

	; \t\tINC\t\t(.*)\n\t\tBNE\t\t(.*)\n\t\tINC\t\t\1\+1\n\2:\n
	; \t\tINCW\t\1\n\2:
	;
	MAC	INCW
		INC		{1}
		BNE		.1
		INC		{1}+1
.1:
	ENDM

	MAC	ADDB
	#if {2}==#$02
		INC		{1}
		INC		{1}
	#else
		LDA		{1}
		CLC
		ADC		{2}
		STA		{1}
	#endif
	ENDM

	MAC	MADDB
		LDA		{2}
		CLC
		ADC		{3}
		STA		{1}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCC\t\t(.*)\n\t\tINC\t\t\1\+1\n\3:
	; \t\tADDWB\t\1,\2\n\3:
	;
	MAC	ADDWB
		LDA		{1}
		CLC
		ADC		{2}
		STA		{1}
		BCC		.1
		INC		{1}+1
.1:
	ENDM

	; Add two 16-bit integer or fixed point values and put it
	; in third arg.
	;
	MAC	MADDW
		LDA		{2}
		CLC
		ADC		{3}
		STA		{1}
		LDA		{2}+1
		ADC		{3}+1
		STA		{1}+1
	ENDM

	; Add two 16-bit integer or fixed point values, shift the
	; result left, then store to third arg
	;
	MAC	LSADDW
		LDA		{2}
		CLC
		ADC		{3}
		STA		{1}
		LDA		{2}+1
		ADC		{3}+1
		REPEAT	{4}
		ASL		{1}
		ROL
		REPEND
		STA		{1}+1
	ENDM

	; Shift right 16bit or 8x8 fixed point value, store arg
	;
	MAC	RSMOVW
		LDA		{2}
		STA		{1}
		LDA		{2}+1
		REPEAT	{3}
		LSR
		ROR		{1}
		REPEND
		STA		{1}+1
	ENDM

	MAC CMPW
		LDA		{1}
		CMP		{2}
		LDA		{1}+1
		SBC		{2}+1
	ENDM

	MAC CMPWI
		LDA		{1}
		CMP		#[[{2}>>0]&$FF]
		LDA		{1}+1
		SBC		#[[{2}>>8]&$FF]
	ENDM

	MAC	MSUBW
		LDA		{2}
		SEC
		SBC		{3}
		STA		{1}
		LDA		{2}+1
		SBC		{3}+1
		STA		{1}+1
	ENDM

	MAC	SUBB
	#if {2}==#$02
		DEC		{1}
		DEC		{1}
	#else
		LDA		{1}
		SEC
		SBC		{2}
		STA		{1}
	#endif
	ENDM

	MAC	MSUBB
		LDA		{2}
		SEC
		SBC		{3}
		STA		{1}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t\#\$(..)\n\t\tSTA\t\t\1\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t\#\$(..)\n\t\tSTA\t\t\1\+1\n\t\tBCS\t\t(.*)\n\t\tDEC\t\t\1\+2\n\4\:\n
	; \t\tSUBDI16\t\1,\$\3\2\n\4\:\n
	;
	; Subtract 16-bit Word from 24-bit Dword
	; in third arg.
	;
	MAC	SUBDI16
		LDA		{1}
		SEC
		SBC		#<[{2}]
		STA		{1}
		LDA		{1}+1
		SBC		#>[{2}]
		STA		{1}+1
		BCS		.1
		DEC		{1}+2
.1:
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t\2\+1\n\t\tSTA\t\t\1\+1\n\t\tBCS\t\t(.*)\n\t\tDEC\t\t\1\+2\n\3\:\n
	; \t\tSUBDW\t\1,\2\n\3\:\n
	;
	MAC	SUBDW
		LDA		{1}
		SEC
		SBC		{2}
		STA		{1}
		LDA		{1}+1
		SBC		{2}+1
		STA		{1}+1
		BCS		.1
		DEC		{1}+2
.1:
	ENDM

	MAC	SUBD
		LDA		{1}
		SEC
		SBC		{2}
		STA		{1}
		LDA		{1}+1
		SBC		{2}+1
		STA		{1}+1
		LDA		{1}+2
		SBC		{2}+2
		STA		{1}+2
	ENDM

	MAC	MSUBD
		LDA		{2}
		SEC
		SBC		{3}
		STA		{1}
		LDA		{2}+1
		SBC		{3}+1
		STA		{1}+1
		LDA		{2}+2
		SBC		{3}+2
		STA		{1}+2
	ENDM

;-------------------F-O-N-T-S-------------------

_0		EQU		$00
_1		EQU		$01
_2		EQU		$02
_3		EQU		$03
_4		EQU		$04
_5		EQU		$05
_6		EQU		$06
_7		EQU		$07
_8		EQU		$08
_9		EQU		$09
_A		EQU		$0A
_B		EQU		$0B
_C		EQU		$0C
_D		EQU		$0D
_E		EQU		$0E
_F		EQU		$0F
_G		EQU		$10
_H		EQU		$11
_I		EQU		$12
_J		EQU		$13
_K		EQU		$14
_L		EQU		$15
_M		EQU		$16
_N		EQU		$17
_O		EQU		$18
_P		EQU		$19
_Q		EQU		$1A
_R		EQU		$1B
_S		EQU		$1C
_T		EQU		$1D
_U		EQU		$1E
_V		EQU		$1F
_W		EQU		$20
_X		EQU		$21
_Y		EQU		$22
_Z		EQU		$23
__		EQU		$DF


;-------------T-I-L-E--F-L-A-G-S----------------

_FL_CONDUCTOR		EQU	$80
_FL_FLAMMABLE		EQU	$40
_FL_ROAD			EQU	$20
_FL_RAIL			EQU	$10
_FL_TRANSPORT		EQU	_FL_ROAD|_FL_RAIL
_FL_BUILDING		EQU $08
_FL_TERRAIN			EQU $04
_FL_FLOODABLE		EQU	$02
_FL_WATER			EQU	$01
_FL_AREA			EQU	_FL_CONDUCTOR|_FL_FLAMMABLE|_FL_BUILDING|_FL_FLOODABLE

;--------------M-A-P--T-I-L-E-S-----------------

_MAP_R_AREA_DEF			EQU		$00
_MAP_R_AREA01			EQU		$01
_MAP_R_AREA02			EQU		$02
_MAP_R_AREA03			EQU		$03
_MAP_R_AREA04			EQU		$04
_MAP_R_AREA05			EQU		$05
_MAP_R_AREA06			EQU		$06
_MAP_R_AREA07			EQU		$07
_MAP_R_AREA08			EQU		$08
_MAP_R_AREA09			EQU		$09
_MAP_R_AREA0A			EQU		$0A
_MAP_R_AREA0B			EQU		$0B
_MAP_R_AREA0C			EQU		$0C
_MAP_R_AREA0D			EQU		$0D
_MAP_R_AREA0E			EQU		$0E
_MAP_R_AREA0F			EQU		$0F
_MAP_R_AREA10			EQU		$10
_MAP_R_AREA11			EQU		$11
_MAP_R_AREA12			EQU		$12
_MAP_R_AREA13			EQU		$13
_MAP_R_AREA14			EQU		$14
_MAP_R_AREA_TWINA0		EQU		$15
_MAP_R_AREA_TWINA1		EQU		$16
_MAP_R_AREA_TWINB0		EQU		$17
_MAP_R_AREA_TWINB1		EQU		$18
_MAP_R_HOSPITAL			EQU		$19
_MAP_R_SCHOOL			EQU		$1A
;						EQU		$1B
;						EQU		$1C
;						EQU		$1D
;						EQU		$1E
;						EQU		$1F
_MAP_C_AREA_DEF			EQU		$20
_MAP_C_AREA01			EQU		$21
_MAP_C_AREA02			EQU		$22
_MAP_C_AREA03			EQU		$23
_MAP_C_AREA04			EQU		$24
_MAP_C_AREA05			EQU		$25
_MAP_C_AREA06			EQU		$26
_MAP_C_AREA07			EQU		$27
_MAP_C_AREA08			EQU		$28
_MAP_C_AREA09			EQU		$29
_MAP_C_AREA0A			EQU		$2A
_MAP_C_AREA0B			EQU		$2B
_MAP_C_AREA0C			EQU		$2C
_MAP_C_AREA0D			EQU		$2D
_MAP_C_AREA0E			EQU		$2E
_MAP_C_AREA0F			EQU		$2F
_MAP_C_AREA10			EQU		$30
_MAP_C_AREA11			EQU		$31
_MAP_C_AREA12			EQU		$32
_MAP_C_AREA13			EQU		$33
_MAP_C_AREA14			EQU		$34
_MAP_C_AREA_TWINA0		EQU		$35
_MAP_C_AREA_TWINA1		EQU		$36
_MAP_C_AREA_TWINB0		EQU		$37
_MAP_C_AREA_TWINB1		EQU		$38
;						EQU		$39
;						EQU		$3A
;						EQU		$3B
;						EQU		$3C
;						EQU		$3D
;						EQU		$3E
;						EQU		$3F
_MAP_I_AREA_DEF			EQU		$40
_MAP_I_AREA01			EQU		$41
_MAP_I_AREA02			EQU		$42
_MAP_I_AREA03			EQU		$43
_MAP_I_AREA04			EQU		$44
_MAP_I_AREA05			EQU		$45
_MAP_I_AREA06			EQU		$46
_MAP_I_AREA07			EQU		$47
_MAP_I_AREA08			EQU		$48
;						EQU		$49
;						EQU		$4A
;						EQU		$4B
;						EQU		$4C
;						EQU		$4D
;						EQU		$4E
;						EQU		$4F
_MAP_POLICE_STATION		EQU		$50
_MAP_FIRE_STATION		EQU		$51
;						EQU		$52
;						EQU		$53
;						EQU		$54
;						EQU		$55
;						EQU		$56
;						EQU		$57
;						EQU		$58
;						EQU		$59
;						EQU		$5A
;						EQU		$5B
;						EQU		$5C
;						EQU		$5D
;						EQU		$5E
;						EQU		$5F
_MAP_COAL_POWER			EQU		$60
_MAP_NUCLEAR_POWER		EQU		$61
_MAP_SEA_PORT			EQU		$62
_MAP_STADIUM			EQU		$63
_MAP_AIR_PORT			EQU		$64
;						EQU		$65
;						EQU		$66
;						EQU		$67
;						EQU		$68
;						EQU		$69
;						EQU		$6A
;						EQU		$6B
;						EQU		$6C
;						EQU		$6D
;						EQU		$6E
;						EQU		$6F
_MAP_BANK				EQU		$70
_MAP_AMUSEMENT_PARK		EQU		$71
_MAP_ZOO				EQU		$72
_MAP_CASINO				EQU		$73
; 						EQU		$74		; "Reclaimed ground", not used by index
_MAP_NEW_POLICE			EQU		$75
_MAP_NEW_FIRE			EQU		$76
_MAP_10TH_MONUMENT		EQU		$77
_MAP_100TH_MONUMENT		EQU		$78
_MAP_EXPO_MONUMENT		EQU		$79
_MAP_LIBERTY_MONUMENT	EQU		$7A
_MAP_LIBRARY			EQU		$7B
_MAP_PARK_EXPAND		EQU		$7C
_MAP_TRAIN_STATION		EQU		$7D
_MAP_WHITE_HOUSE		EQU		$7E
;						EQU		$7F
_MAP_INTERNAL_BUILD0	EQU		$80
_MAP_INTERNAL_BUILD1	EQU		$81
_MAP_INTERNAL_BUILD2	EQU		$82
_MAP_INTERNAL_BUILD3	EQU		$83
_MAP_INTERNAL_BUILD4	EQU		$84
_MAP_INTERNAL_BUILD5	EQU		$85
_MAP_INTERNAL_BUILD6	EQU		$86
_MAP_INTERNAL_BUILD7	EQU		$87
_MAP_INTERNAL_BUILD8	EQU		$88
_MAP_INTERNAL_BUILD9	EQU		$89
_MAP_INTERNAL_BUILDA	EQU		$8A
_MAP_INTERNAL_BUILDB	EQU		$8B
_MAP_INTERNAL_BUILDC	EQU		$8C
_MAP_INTERNAL_BUILDD	EQU		$8D
_MAP_INTERNAL_BUILDE	EQU		$8E
;						EQU		$8F
_MAP_GROUND				EQU		$90
_MAP_ROAD_LR			EQU		$91
_MAP_ROAD_UD			EQU		$92
_MAP_ROAD_BEND_RD		EQU		$93
_MAP_ROAD_BEND_LD		EQU		$94
_MAP_ROAD_BEND_RU		EQU		$95
_MAP_ROAD_BEND_LU		EQU		$96
_MAP_ROAD_TCROSS_R		EQU		$97
_MAP_ROAD_TCROSS_L		EQU		$98
_MAP_ROAD_TCORSS_D		EQU		$99
_MAP_ROAD_TCROSS_U		EQU		$9A
_MAP_ROAD_CROSS			EQU		$9B
_MAP_RAIL_LR			EQU		$9C
_MAP_RAIL_UD			EQU		$9D
_MAP_RAIL_BEND_RD		EQU		$9E
_MAP_RAIL_BEND_LD		EQU		$9F
_MAP_RAIL_BEND_RU		EQU		$A0
_MAP_RAIL_BEND_LU		EQU		$A1
_MAP_RAIL_TCROSS_R		EQU		$A2
_MAP_RAIL_TCROSS_L		EQU		$A3
_MAP_RAIL_TCORSS_D		EQU		$A4
_MAP_RAIL_TCROSS_U		EQU		$A5
_MAP_RAIL_CROSS			EQU		$A6
_MAP_ELECTRO_LR			EQU		$A7
_MAP_ELECTRO_UD			EQU		$A8
_MAP_ELECTRO_BEND_RD	EQU		$A9
_MAP_ELECTRO_BEND_LD	EQU		$AA
_MAP_ELECTRO_BEND_RU	EQU		$AB
_MAP_ELECTRO_BEND_LU	EQU		$AC
_MAP_ELECTRO_TCROSS_R	EQU		$AD
_MAP_ELECTRO_TCROSS_L	EQU		$AE
_MAP_ELECTRO_TCORSS_D	EQU		$AF
_MAP_ELECTRO_TCROSS_U	EQU		$B0
_MAP_ELECTRO_CROSS		EQU		$B1
_MAP_GRASS				EQU		$B2
_MAP_THREE				EQU		$B3
_MAP_CORRUPTED			EQU		$B4
_MAP_FLOOD				EQU		$B5
_MAP_POLLUTION			EQU		$B6
_MAP_ROAD_RAIL_LR		EQU		$B7
_MAP_ROAD_RAIL_UD		EQU		$B8
_MAP_ELECTRO_ROAD_LR	EQU		$B9
_MAP_ELECTRO_ROAD_UD	EQU		$BA
_MAP_ELECTRO_RAIL_LR	EQU		$BB
_MAP_ELECTRO_RAIL_UD	EQU		$BC
_MAP_WATER				EQU		$BD
_MAP_SHORE0				EQU		$BE
_MAP_SHORE1				EQU		$BF
_MAP_SHORE2				EQU		$C0
_MAP_SHORE3				EQU		$C1
_MAP_SHORE4				EQU		$C2
_MAP_SHORE5				EQU		$C3
_MAP_SHORE6				EQU		$C4
_MAP_SHORE7				EQU		$C5
_MAP_SHORE8				EQU		$C6
_MAP_SHORE9				EQU		$C7
_MAP_SHOREA				EQU		$C8
_MAP_SHOREB				EQU		$C9
_MAP_SHOREC				EQU		$CA
_MAP_SHORED				EQU		$CB
_MAP_SHOREE				EQU		$CC
_MAP_SHOREF				EQU		$CD
_MAP_WOODS0				EQU		$CE
_MAP_WOODS1				EQU		$CF
_MAP_WOODS2				EQU		$D0
_MAP_WOODS3				EQU		$D1
_MAP_WOODS4				EQU		$D2
_MAP_WOODS5				EQU		$D3
_MAP_WOODS6				EQU		$D4
_MAP_WOODS7				EQU		$D5
_MAP_WOODS8				EQU		$D6
_MAP_WOODS9				EQU		$D7
_MAP_WOODSA				EQU		$D8
_MAP_WOODSB				EQU		$D9
_MAP_WOODSC				EQU		$DA
_MAP_WOODSD				EQU		$DB
_MAP_WOODSE				EQU		$DC
_MAP_WOODSF				EQU		$DD
_MAP_WOODS10			EQU		$DE
_MAP_WOODS11			EQU		$DF
_MAP_WATER_ROAD_LR		EQU		$E0
_MAP_WATER_ROAD_UD		EQU		$E1
_MAP_WATER_RAIL_LR		EQU		$E2
_MAP_WATER_RAIL_UD		EQU		$E3
_MAP_WATER_ELECTRO_LR	EQU		$E4
_MAP_WATER_ELECTRO_UD	EQU		$E5
_MAP_FIRE				EQU		$E6
_MAP_CORRUPT0			EQU		$E7
_MAP_CORRUPT1			EQU		$E8
_MAP_CORRUPT2			EQU		$E9
_MAP_CORRUPT3			EQU		$EA
_MAP_BRIDGE_ROAD_LR		EQU		$EB
_MAP_BRIDGE_ROAD_UD		EQU		$EC
_MAP_BRIDGE0			EQU		$ED
_MAP_BRIDGE1			EQU		$EE
_MAP_BRIDGE2			EQU		$EF
_MAP_BRIDGE3			EQU		$F0
_MAP_BRIDGE4			EQU		$F1
_MAP_BRIDGE5			EQU		$F2
_MAP_BRIDGE6			EQU		$F3
_MAP_BRIDGE7			EQU		$F4
_MAP_BRIDGE8			EQU		$F5
_MAP_BRIDGE9			EQU		$F6
_MAP_BRIDGEA			EQU		$F7
_MAP_BRIDGEB			EQU		$F8
_MAP_BRIDGEC			EQU		$F9
_MAP_BRIDGED			EQU		$FA
_MAP_BRIDGEE			EQU		$FB
_MAP_BRIDGEF			EQU		$FC
;						EQU		$FD
_MAP_WATER_ROUTE		EQU		$FE
;						EQU		$FF
