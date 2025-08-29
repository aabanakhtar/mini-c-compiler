	.text
	.file	"main"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	.p2align	4, 0x90
.LBB0_1:                                # %while_cond
                                        # =>This Inner Loop Header: Depth=1
	movl	$.L__unnamed_1, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	jmp	.LBB0_1
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"Hello, World!\\n"
	.size	.L__unnamed_1, 16

	.section	".note.GNU-stack","",@progbits
