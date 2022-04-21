	.data
data1:	.word	100
data2:	.word	200
data3:	.word	0x12345678
	.text
main:								 #store(counter,"main")
	and	$17, $17, $0 				#counter++
	and	$18, $18, $0 				#counter++
	la	$8, data1  					#counter++
	la	$9, data2 					#counter++
	and	$10, $10, $0 				#counter++
lab1: 								#store(counter,"lab1")
	and	$11, $11, $0 				#counter++
lab2:
	addiu	$17, $17, 0x1 				#counter++
	addiu	$11, $11, 0x1 				#counter++
	or	$9, $9, $0 				#counter++
	bne	$11, $8, lab2 				#counter++
lab3: 								#store(counter,"lab3")
	addiu	$18, $18, 0x2 				#counter++
	addiu	$11, $11, 1 				#counter++
	sll	$18, $17, 1 				#counter++
	srl	$17, $18, 1 				#counter++
	and	$19, $17, $18 				#counter++
	bne	$11, $9, lab3 				#counter++
lab4: 								#store(counter,"lab4")
	addu	$5, $5, $31 				#counter++
	nor	$16, $17, $18 				#counter++
	beq	$10, $8, lab5 				#counter++
	j	lab1 #counter++
lab5: 								#store(counter,"lab5")
	ori	$16, $16, 0xf0f0 #counter++
