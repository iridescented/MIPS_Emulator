main:
    addi $a1, $zero, 1
    addi $a3, $zero, 401
    sw $a2, 0($t1)
loop:
    add $a2, $a2, $a1
    addi $a1, $a1, 2
    sw $a2, 4($t1)
    addi $t1, $t1, 4
    beq $a1, $a3, end
    j loop
end:
