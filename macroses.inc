;-------------C-O-N-S-T-A-N-T-S-----------------

DEBUG	EQU		1

;---------G-A-M-E--S-P-E-C-I-F-I-C--------------

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
{1}._var_5F8:							.BYTE	0
{1}._var_5F9:							.BYTE	0
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
{1}._data:		.BYTE	0,0,0,0,0,0,0,0,0,0
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


;---------------M-A-C-R-O-S-E-S-----------------

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
_total_free SET _total_free+_bank{1}_free
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
_total_free SET _total_free+_bank{1}_free
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

	; conditional set of macroses, first mark the routine
	; as external by setting its bank number, then check if any
	; far routine assigned, which is occurs on the second pass
	; when far routine detects that caller and its destination
	; are in different banks. if there is no _far* calls, then
	; we don't need this entry at all, discard.
	;
	MAC EXPORT
{1}_bank	SET CUR_PRG_BANK
	#ifconst _far{1}
{1}_pidx	SET	[[. - CUR_BANK_START] >> 1]
		.WORD	{1}
	#endif
	ENDM

	; when used in system bank, will always generate a far call
	; when used in external banks, may be used in direct jumps
	; which is selected based on the bank number of the destination
	;
	; may be used if caller uses a farcall only once in a
	; particular place
	;
	; if there is a direct not far call used for this farcall
	; we abandon JMP generating at all, so we automatically clear
	; unused code.
	;
	; there is an possible case, when you may use it as OFS and direct
	; jump at the same time, so you need to be aware, bacause in
	; local mode this may cause of disappearance of needed JMPs,
	; so maybe you need to use FJSR here or make the code manually
	;
	MAC	FJMP
	#if	{1}_bank == CUR_PRG_BANK
		JMP		{1}
	#else
;_far{1}	SET .
		LDA		#{1}_bank
		LDY		#{1}_pidx
		JMP		_far_call_no_args
	#endif
	ENDM

	; used only in system banks once per call, _far* link actually
	; not needed, but kept here for compatibility with EXPORT
	; macros
	;
	MAC	FJSR
	#if	{1}_bank == CUR_PRG_BANK
		JSR		{1}
	#else
;_far{1}	SET .
		LDA		#{1}_bank
		LDY		#{1}_pidx
		JSR		_far_call_no_args
	#endif
	ENDM

	; the same, but with args (there are no JSRs of this kind of subs)
	;
	MAC	FJMPA
_far{1}	SET .
		STA		_backup_A
		STY		_backup_Y
		LDA		#{1}_bank
		LDY		#{1}_pidx
		JMP		_far_call_with_args
	ENDM

;---------------A-R-I-T-H-M---------------------

	; \t\tLDA\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t\1\+1\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVM\t\2,\1\n
	;
	MAC	MOVM
		LDA		{2}
		STA		{1}
		LDA		{2}+1
		STA		{1}+1
	ENDM

	; \t\tLDA\t\t#<\[(.*)\]\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#>\[\1\]\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVO\t\2,\1\n
	; \t\tLDA\t\t#\$(..)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#\$(..)\n\t\tSTA\t\t\2\+1\n
	; \t\tMOVO\t\2,$\3\1\n
	;
	;
	MAC	MOVO
		LDA		#<[{2}]
		STA		{1}
		LDA		#>[{2}]
		STA		{1}+1
	ENDM

	; \t\tINC\t\t(.*)\n\t\tBNE\t\t(.*)\n\t\tINC\t\t\1\+1\n\2:\n
	; \t\tINCW\t\1\n
	;
	MAC	INCW
		INC		{1}
		BNE		.1
		INC		{1}+1
.1:
	ENDM

	; \t\tINC\t\t(.*)\n\t\tBNE\t\t(.*)\n\t\tINC\t\t\1\+1\n\2:\t\tJMP\t\tloc(.*)\n
	; \t\tINC\t\t(.*)\n\t\tBNE\t\tloc\3\n\t\tINC\t\t\1\+1\n;\2:\n\t\tJMP\t\tloc\3\n
	; \t\tINC\t\t(.*)\n\t\tBNE\t\t(.*)\n\t\tINC\t\t\1\+1\n\t\tJMP\t\t\2\n
	; \t\tJINCW\t\1,\2\n
	;
	MAC	JINCW
		INC		{1}
		BNE		{2}
		INC		{1}+1
		JMP		{2}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tBNE\t\t(.*)\n\t\tDEC\t\t\1\+1\n\2:\n\t\tDEC\t\t\1\n
	; \t\tDECW\t\1\n
	;
	MAC	DECW
		LDA		{1}
		BNE		.1
		DEC		{1}+1
.1:
		DEC		{1}
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

	; \t\tLDA\t\t(.*)\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCC\t\t(.*)\n\t\tINC\t\t\1\+1\n\t\tJMP\t\t\3\n
	; \t\tJADDW8\t\1,\2,\3\n
	;
	MAC	JADDW8
		LDA		{1}
		CLC
		ADC		{2}
		STA		{1}
		BCC		{3}
		INC		{1}+1
		JMP		{3}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCS\t\t(.*)\n\t\tDEC\t\t\1\+1\n\3:\n
	; \t\tEADDW8\t\1,\2\n
	;
	MAC EADDW8
		LDA		{1}
		CLC
		ADC		{2}
		STA		{1}
		BCS		.1
		DEC		{1}+1
.1:
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCS\t\t(.*)\n\t\tDEC\t\t\1\+1\n\t\tJMP\t\t\3\n
	; \t\tJEADDW8\t\1,\2,\3\n
	;
	MAC JEADDW8
		LDA		{1}
		CLC
		ADC		{2}
		STA		{1}
		BCS		{3}
		DEC		{1}+1
		JMP		{3}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCS\t\t(.*)\n\t\tDEC\t\t\1\+1\n\3:
	; \t\tSUBW8\t\1,\2\n\3:
	;
	MAC	SUBW8
		LDA		{1}
		SEC
		SBC		{2}
		STA		{1}
		BCS		.1
		DEC		{1}+1
.1:
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tBCS\t\t(.*)\n\t\tDEC\t\t\1\+1\n\t\tJMP\t\t\3\n
	; \t\tJSUBW8\t\1,\2,\3\n
	;
	MAC	JSUBW8
		LDA		{1}
		SEC
		SBC		{2}
		STA		{1}
		BCS		{3}
		DEC		{1}+1
		JMP		{3}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tCLC\n\t\tADC\t\t#<\[(.*)\]\n\t\tSTA\t\t\1\n\t\tLDA\t\t\1\+1\n\t\tADC\t\t#>\[\2\]\n\t\tSTA\t\t\1\+1\n
	; \t\tADDWO16\t\1,\2\n
	;
	MAC	ADDWO16
		LDA		{1}
		CLC
		ADC		#<[{2}]
		STA		{1}
		LDA		{1}+1
		ADC		#>[{2}]
		STA		{1}+1
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t(.*)\n\t\tSTA\t\t\1\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t\2\+1\n\t\tSTA\t\t\1\+1\n
	; \t\tSUBM16\t\1,\2\n
	;
	MAC	SUBM16
		LDA		{1}
		SEC
		SBC		{2}
		STA		{1}
		LDA		{1}+1
		SBC		{2}+1
		STA		{1}+1
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tEOR\t\t#\$FF\n\t\tCLC\n\t\tADC\t\t#1\n\t\tSTA\t\t\1\n
	; \t\tNEGB\t\1\n
	;
	MAC	NEGB
		LDA		{1}
		EOR		#$FF
		CLC
		ADC		#1
		STA		{1}
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tEOR\t\t\#\$FF\n\t\tSTA\t\t\1\n\t\tLDA\t\t\1\+1\n\t\tEOR\t\t\#\$FF\n\t\tSTA\t\t\1\+1\n\t\tINC\t\t\1\n\t\tBNE\t\t(.*)\n\t\tINC\t\t\1\+1\n\2:
	; \t\tNEGW\t\1\n\2:
	;
	MAC	NEGW
		LDA		{1}
		EOR		#$FF
		STA		{1}
		LDA		{1}+1
		EOR		#$FF
		STA		{1}+1
		INC		{1}
		BNE		.1
		INC		{1}+1
.1:
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t#\$(..)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t#\$(.)\n
	; \t\tCMPI16\t\1,\$\3\2\n
	;
	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t#(.)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t#\$(..)\n
	; \t\tCMPI16\t\1,\$\30\2\n
	;
	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t#\$(..)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t#(.)\n
	; \t\tCMPI16\t\1,\$0\3\2\n
	;
	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t#(.)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t#(.)\n
	; \t\tCMPI16\t\1,\$0\30\2\n
	;
	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t#(.)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t#\$(.)\n
	; \t\tCMPI16\t\1,\$\30\2\n
	;
	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t#\$(..)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t#\$(..)\n
	; \t\tCMPI16\t\1,\$\3\2\n
	;
	MAC	CMPI16
		LDA		{1}
		SEC
		SBC		#<[{2}]
		LDA		{1}+1
		SBC		#>[{2}]
	ENDM

	; \t\tLDA\t\t(.*)\n\t\tSEC\n\t\tSBC\t\t(.*)\n\t\tLDA\t\t\1\+1\n\t\tSBC\t\t\2\+1\n
	; \t\tCMPM16\t\1,\2
	;
	MAC CMPM16
		LDA		{1}
		SEC
		SBC		{2}
		LDA		{1}+1
		SBC		{2}+1
	ENDM

	; \t\tLDA\t\t#0\n\t\tSTA\t\t(.*)\+1\n\t\tLDA\t\t(.*)\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tSTA\t\t\1\n
	; \t\tLSHW8\t\1,\2,5\n
	; \t\tLDA\t\t#0\n\t\tSTA\t\t(.*)\+1\n\t\tLDA\t\t(.*)\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\t\tASL\n\t\tROL\t\t\1\+1\n\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tSTA\t\t\1\n
	; \t\tLSHW8\t\1,\2,4\n
	; \t\tLDA\t\t#0\n\t\tSTA\t\t(.*)\+1\n\t\tLDA\t\t(.*)\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tSTA\t\t\1\n
	; \t\tLSHW8\t\1,\2,3\n
	; \t\tLDA\t\t#0\n\t\tSTA\t\t(.*)\+1\n\t\tLDA\t\t(.*)\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tASL\n\t\tROL\t\t\1\+1\n\t\tSTA\t\t\1\n
	; \t\tLSHW8\t\1,\2,2\n
	; \t\tLDA\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#\$00\n\t\tSTA\t\t\2\+1\n\t\tASL\t\t\2\n\t\tROL\t\t\2\+1\n\t\tASL\t\t\2\n\t\tROL\t\t\2\+1\n\t\tASL\t\t\2\n\t\tROL\t\t\2\+1\n
	; \t\tLSHW8\t\2,\1,3\n
	; \t\tLDA\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tLDA\t\t#\$00\n\t\tSTA\t\t\2\+1\n\t\tASL\t\t\2\n\t\tROL\t\t\2\+1\n\t\tASL\t\t\2\n\t\tROL\t\t\2\+1\n
	; \t\tLSHW8\t\2,\1,2\n
	;
	MAC	LSHW8
		LDA		#0
		STA		{1}+1
		LDA		{2}
		REPEAT	{3}
		ASL
		ROL		{1}+1
		REPEND
		STA		{1}
	ENDM

