.text

start:
	ldi A 2
	sta %b
	ldi A 5
	sta %a
	lda %a
	push A
	ldi A 5
	pop B
	cmp
	jnc %L0
	jnz %L0
	jmp %L1
L0:
	lda %a
	sta %c
L1:
	hlt


.data

b = 0
a = 0
c = 0
