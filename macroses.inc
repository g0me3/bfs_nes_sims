;-------------C-O-N-S-T-A-N-T-S-----------------

DEBUG	EQU		1

;------------S-T-R-U-C-T-U-R-E-S----------------

	MAC	_tpos
{1}		SET	.
{1}._X:		.BYTE	0
{1}._Y:		.BYTE	0
	ENDM

	MAC	_trci
{1}		SET	.
{1}._R:		.BYTE	0
{1}._C:		.BYTE	0
{1}._I:		.BYTE	0
	ENDM

	MAC	_trciw
{1}		SET	.
{1}._R:		.WORD	0
{1}._C:		.WORD	0
{1}._I:		.WORD	0
	ENDM

	MAC	_tapu
{1}		SET	.
{1}._chnls:	.BYTE	{2}
{1}._ptr:	.WORD	{3}
{1}._var0:	.BYTE	{4}
{1}._var1:	.BYTE	{5}
{1}._var2:	.BYTE	{6}
{1}._var3:	.BYTE	{7}
	ENDM

; _game_flags
; bit0 - auto buldozer
; bit1 - auto budget
; bit2 - auto goto
; bit3 - music on

	MAC	_tcity
{1}		SET	.
{1}._name:								.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._sister_name:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._bank_name:							.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._unused_name3:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._unused_name4:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._scenarios_clear_flags:				.BYTE	0
{1}._all_scenarios_are_clear_ram_flag:	.BYTE	0
		_trciw							{1}._level
		_trci							{1}._delta
{1}._var_5F6:							.BYTE	0
{1}._var_5F7:							.BYTE	0
{1}._var_5F8:							.WORD	0
{1}._crime_level:						.WORD	0
{1}._var_5FC:							.BYTE	0
{1}._var_5FD:							.BYTE	0
{1}._difficult:							.BYTE	0
{1}._city_type:							.BYTE	0
{1}._score:								.WORD	0
{1}._money:								.BYTE	0,0,0
{1}._cur_year:							.WORD	0
{1}._cur_month:							.BYTE	0
{1}._cur_week:							.BYTE	0
{1}._tax_rate:							.BYTE	0
{1}._game_flags:						.BYTE	0
{1}._game_speed:						.BYTE	0
{1}._events_used_flags:					.BYTE	0,0,0,0
{1}._year_to_build_bank:				.WORD	0
{1}._year_to_expo:						.WORD	0
{1}._year_to_liberty:					.WORD	0
{1}._special_buildings_list:			.BYTE	0,0,0,0
{1}._history_list:						.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{1}._bank_flags:						.BYTE	0
{1}._tax_year_rate:						.WORD	0
{1}._bank_loan_period:					.BYTE	0
{1}._unused:							.BYTE	0
	ENDM

	MAC	_treq
{1}		SET	.
{1}._stadium:	.BYTE	0
{1}._seaport:	.BYTE	0
{1}._airport:	.BYTE	0
	ENDM

	MAC	_tdev
{1}		SET	.
{1}._developed:		.WORD	0
{1}._undeveloped:	.WORD	0
{1}._other:			.WORD	0
{1}._total:			.WORD	0
	ENDM

	MAC	_tobj6w
{1}		SET	.
{1}._X	SET	.
{1}._Y	SET	[.+1]
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
{1}._bank_loan_year:.WORD	0
	ENDM

	MAC	_tsprarg
{1}		SET	.
{1}._attr:		.BYTE	0
{1}._idx:		.BYTE	0
		_tpos	{1}._pos
{1}._flip:		.BYTE	0
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
		_tgraph	{1}._crime
		_tgraph	{1}._money
		_tgraph	{1}._tax_income
	ENDM

	MAC	_tgraphs
{1}		SET	.
		_tyear {1}._10y
		_tyear {1}._100y
	ENDM

;---------G-A-M-E--S-P-E-C-I-F-I-C--------------

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

	; \t\tPUSHB\t(.*)\n\t\tPUSHB\t(.*)\n\t\tJSR\t\t_mmc5_mul8to8\n\t\tPOPB\t(.*)\n\t\tPOPB\t\3\+1\n
	; \t\tMMULB8\t\3,\1,\2\n
	;
	; multyply 8bit arguments, store result in 16bit buffer
	;
	MAC	MMULB8
		PUSHB	{2}
		PUSHB	{3}
		JSR		_mmc5_mul8to8
		POPB	{1}
		POPB	{1}+1
	ENDM

;-----------------C-O-M-M-O-N-------------------

	MAC BANK_START
CUR_PRG_BANK	SET {1}
CUR_BANK_START	SET .
		ECHO	"bank",{1},"start ["
	ENDM

	MAC	BANK_END
_bank{1}_free	SET	[{2}-.]
	#if ${1}==$0
_total_free	SET	_bank{1}_free
	#else
_total_free	SET _total_free+_bank{1}_free
	#endif
	#if .>{2}
		ECHO	"]","exceeded by=",[0-_bank{1}_free]
	#else
		ECHO	"]","free =",_bank{1}_free
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
		ECHO	" > section",${1},"size =",_{1}_end-_{1}_start
	ENDM

	MAC	LAST_BANK_END
_bank{1}_free	SET	[{2}-.-6]
_total_free	SET _total_free+_bank{1}_free
		ECHO	"]","free =",_bank{1}_free
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

	MAC PUSHWM
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

	; \t\tLDA\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t\1\+1\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVW\t\2,\1\n
	;
	MAC	MOVW
		LDA		{2}
		STA		{1}
		LDA		{2}+1
		STA		{1}+1
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
		LDA		#[[{2}>>0 ]&$FF]
		STA		{1}
		LDA		#[[{2}>>8 ]&$FF]
		STA		{1}+1
	#endif
	ENDM

	; \t\tLDA\t\t#\$(..)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#\$(..)\n\t\tSTA\t\t\2\+1\n\t\tLDA\t\t#\$(..)\n\t\tSTA\t\t\2\+2\n
	; \t\tMOVDI\t\2,$\4\3\1\n
	;
	MAC	MOVDI
		LDA		#[[{2}>>0 ]&$FF]
		STA		{1}
		LDA		#[[{2}>>8 ]&$FF]
		STA		{1}+1
		LDA		#[[{2}>>16]&$FF]
		STA		{1}+2
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

	; \t\tLDA\t\t(.*)\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCC\t\t(.*)\n\t\tINC\t\t\1\+1\n\3:
	; \t\tADDW8\t\1,\2\n\3:
	;
	MAC	ADDW8
		LDA		{1}
		CLC
		ADC		{2}
		STA		{1}
		BCC		.1
		INC		{1}+1
.1:
	ENDM
