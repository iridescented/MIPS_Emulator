main:
    addi $a1, $zero, 1
    addi $a2, $zero, 0
    addi $a3, $zero, 401
    addi $t1, $zero, 0
    nop
    nop
    sw $a2, 0($t1)
    nop
    nop
loop:
    nop
    nop
    add $a2, $a2, $a1
    nop
    addi $a1, $a1, 2
    sw $a2, 4($t1)
    addi $t1, $t1, 4
    beq $a1, $a3, end
    nop
    nop
    j loop
    nop
    nop
end: