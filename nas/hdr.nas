		PROCESSOR 6502

		ORG	$0000

		.BYTE	"NES", $1A
		.BYTE	$08				; prg 16k banks
		.BYTE	$10				; chr 8k banks
		.BYTE	$53				; mapper lo / battery / mirror
		.BYTE	$00				; mapper hi
		.BYTE	0, 0, 0, 0, 0, 0, 0, 0