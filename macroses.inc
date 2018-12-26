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
{1}._X:		.BYTE	0
{1}._Y:		.BYTE	0
	ENDM

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

	MAC	_tapu
{1}		SET	.
{1}._chnls:	.BYTE	{2}
{1}._ptr:	.WORD	{3}
{1}._var0:	.BYTE	{4}
{1}._var1:	.BYTE	{5}
{1}._var2:	.BYTE	{6}
{1}._var3:	.BYTE	{7}
	ENDM

	MAC	_tcity
{1}		SET	.
{1}._name:								.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._sister_name:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._bank_name:							.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._unused_name3:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._unused_name4:						.BYTE	0,0,0,0,0,0,0,0,0,0,0
{1}._scenarios_clear_flags:				.BYTE	0
{1}._all_scenarios_are_clear_ram_flag:	.BYTE	0
		_trcifix8_8						{1}._level
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
	; bit0 - auto buldozer
	; bit1 - auto budget (unused)
	; bit2 - auto goto
	; bit3 - music on/off
	; bit4 - debug mode (free construction)
{1}._game_speed:						.BYTE	0
{1}._events_used_flags:					.BYTE	0,0,0,0
{1}._year_to_build_bank:				.WORD	0
{1}._year_to_expo:						.WORD	0
{1}._year_to_liberty:					.WORD	0
{1}._presents_list:			.BYTE	0,0,0,0
{1}._history_list:						.BYTE	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
{1}._bank_flags:						.BYTE	0
	; bit0 - bank built, wait for loan message
	; bit6 - money loaned, wait for end of period
	; bit7 - loan message enabled, may loan now
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

	; \t\tLDA\t\t(.*)\n\t\tLDX\t\t(.*)\n\t\tMULXA\n\t\tCLC\n\t\tADC\t\t(.*)\n\t\tSTA\t\t(.*)\n\t\tTXA\n\t\tADC\t\t(.*)\n\t\tSTA\t\t(.*)\n
	; \t\tBUFPOS\t\4\,\3\,\1\,\2\,\5\t\; DST=\(\1*\2+\3\)+\500\n
	;
	; usage: BUFPOS DST,X,Y,WIDTH,PAGE
	;				4   3 1 2     5
	;
	MAC	BUFPOS
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
	ENDM


;-----------------C-O-M-M-O-N-------------------

	MAC BANK_START
CUR_PRG_BANK	SET {1}
CUR_BANK_START	SET .
;		ECHO	"bank",{1},"start ["
	ENDM

	MAC	BANK_END
_bank{1}_free	SET	[{2}-.]
	#if ${1}==$F0
_total_free	SET	_bank{1}_free
	#else
_total_free	SET _total_free+_bank{1}_free
	#endif
	#if .>{2}
		ECHO	"bank",CUR_PRG_BANK,"[ size =",[.-CUR_BANK_START],"]","exceeded =",[0-_bank{1}_free]
	#else
		ECHO	"bank",CUR_PRG_BANK,"[ size =",[.-CUR_BANK_START],"]","free =",_bank{1}_free
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
	#if .>[{2}-6]
		ECHO	"bank",CUR_PRG_BANK,"[ size =",[.-CUR_BANK_START],"]","exceeded =",[0-_bank{1}_free]
	#else
		ECHO	"bank",CUR_PRG_BANK,"[ size =",[.-CUR_BANK_START],"]","free =",_bank{1}_free
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