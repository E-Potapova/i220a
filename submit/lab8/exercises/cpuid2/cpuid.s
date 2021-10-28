#Currently get_cpuid(int *eax, int *ebx, int *ecx, int *edx).
#Modify to get_cpuid(int cpuid_op, int *eax, int *ebx, int *ecx, int *edx).
#		      op = rdi,   eax = rsi, ebx = rdx, ecx = rcx, edx = r8
	.text
	.globl get_cpuid
get_cpuid:
	pushq   %rcx
	pushq   %rdx
	movl 	%edi, %eax	#setup cpuid opcode to incoming cpuid_op
	cpuid
	#largest param in %eax
	#12-char manufacturer string in ebx, edx, ecx.
	movl	%eax, (%rsi)	#store eax cpuid result
	popq	%rax		#pop address for ebxP
	movl    %ebx, (%rax)    #store ebx cpuid result
	popq    %rax		#pop address for ecxP
	movl	%ecx, (%rax)	#store ecx cpuid result
	movl	%edx, (%r8)	#store edx cpuid result
	ret
	
