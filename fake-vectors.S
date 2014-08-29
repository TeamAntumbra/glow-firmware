.section .fake_vectors,"ax",@progbits

.global _fake_vectors
_fake_vectors:
    .equ i, 0
    .rept 43
    jmp __vectors + 4 * i
    .equ i, i + 1
    .endr
