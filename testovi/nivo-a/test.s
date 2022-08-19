.section my_test
.skip 4
rasta:
b: .word a
jmp *0xA2
#jmp rasta
#jmp 0x51
#jmp 5
jeq *[r5 + 0xA1]
jmp *[r5 + rasta]
.word b
.skip 3