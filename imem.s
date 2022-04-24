test:
    add $a0, $zero, $a1
    addi $a0, $zero, 25
    sub $a0, $zero, $a1
    sw $a1, 4($a0)
    j fin

fin:
         
