	.text
	.globl get_parity
#edi contains n	
get_parity:
	testl %edi, %edi	# set parity flag to 1 if n is even
	jpe .EVEN		# jump if parity flag is set
	movl $0x0, %eax
	ret
	
.EVEN:
	movl $0x1, %eax
	ret
