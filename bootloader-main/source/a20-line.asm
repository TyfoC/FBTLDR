bits 16

extern EnableA20Line
EnableA20Line:
	in al, 0x92
	test al, 2
	jnz .Done

	or al, 2
	and al, 0xfe
	out 0x92, al
.Done:
	retd