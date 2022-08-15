.section my_test
.skip 4
a:
jmp a
b: .word a
jmp b
jmp *b
jeq $a
jmp [b]
jeq 3