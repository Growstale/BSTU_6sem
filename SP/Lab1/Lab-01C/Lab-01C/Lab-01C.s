	.text
	.def	@feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 1
	.file	"Lab-01C.c"
	.def	_sprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_sprintf
	.globl	_sprintf                        # -- Begin function sprintf
	.p2align	4, 0x90
_sprintf:                               # @sprintf
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$24, %esp
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	leal	16(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %esi
	movl	12(%ebp), %edx
	movl	8(%ebp), %ecx
	movl	%esp, %eax
	movl	%esi, 12(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	movl	$0, 8(%eax)
	calll	__vsprintf_l
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	addl	$24, %esp
	popl	%esi
	popl	%ebp
	retl
                                        # -- End function
	.def	_vsprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vsprintf
	.globl	_vsprintf                       # -- Begin function vsprintf
	.p2align	4, 0x90
_vsprintf:                              # @vsprintf
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$20, %esp
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	movl	16(%ebp), %eax
	movl	12(%ebp), %ecx
	movl	8(%ebp), %edx
	xorl	%esi, %esi
	movl	%edx, (%esp)
	movl	$-1, 4(%esp)
	movl	%ecx, 8(%esp)
	movl	$0, 12(%esp)
	movl	%eax, 16(%esp)
	calll	__vsnprintf_l
	addl	$20, %esp
	popl	%esi
	popl	%ebp
	retl
                                        # -- End function
	.def	__snprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__snprintf
	.globl	__snprintf                      # -- Begin function _snprintf
	.p2align	4, 0x90
__snprintf:                             # @_snprintf
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	subl	$24, %esp
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	leal	20(%ebp), %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %edi
	movl	16(%ebp), %esi
	movl	12(%ebp), %edx
	movl	8(%ebp), %ecx
	movl	%esp, %eax
	movl	%edi, 12(%eax)
	movl	%esi, 8(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	calll	__vsnprintf
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	addl	$24, %esp
	popl	%esi
	popl	%edi
	popl	%ebp
	retl
                                        # -- End function
	.def	__vsnprintf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__vsnprintf
	.globl	__vsnprintf                     # -- Begin function _vsnprintf
	.p2align	4, 0x90
__vsnprintf:                            # @_vsnprintf
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	subl	$20, %esp
	movl	20(%ebp), %eax
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	movl	20(%ebp), %eax
	movl	16(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	8(%ebp), %esi
	xorl	%edi, %edi
	movl	%esi, (%esp)
	movl	%edx, 4(%esp)
	movl	%ecx, 8(%esp)
	movl	$0, 12(%esp)
	movl	%eax, 16(%esp)
	calll	__vsnprintf_l
	addl	$20, %esp
	popl	%esi
	popl	%edi
	popl	%ebp
	retl
                                        # -- End function
	.def	_main;
	.scl	2;
	.type	32;
	.endef
	.text
	.globl	_main                           # -- Begin function main
	.p2align	4, 0x90
_main:                                  # @main
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$20, %esp
	movl	$0, -4(%ebp)
	movl	$0, -12(%ebp)
	leal	"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@", %eax
	movl	%eax, (%esp)
	calll	_printf
	leal	"??_C@_02DPKJAMEF@?$CFd?$AA@", %ecx
	leal	-8(%ebp), %eax
	movl	%ecx, (%esp)
	movl	%eax, 4(%esp)
	calll	_scanf
	cmpl	$100, -8(%ebp)
	jl	LBB4_2
# %bb.1:
	cmpl	$999, -8(%ebp)                  # imm = 0x3E7
	jle	LBB4_3
LBB4_2:
	leal	"??_C@_0CN@NLFIELDG@Error?3?5a?5non?9three?9digit?5number?5@", %eax
	movl	%eax, (%esp)
	calll	_printf
	jmp	LBB4_7
LBB4_3:
	jmp	LBB4_4
LBB4_4:                                 # =>This Inner Loop Header: Depth=1
	cmpl	$0, -8(%ebp)
	jle	LBB4_6
# %bb.5:                                #   in Loop: Header=BB4_4 Depth=1
	movl	-8(%ebp), %eax
	movl	$10, %ecx
	cltd
	idivl	%ecx
	addl	-12(%ebp), %edx
	movl	%edx, -12(%ebp)
	movl	-8(%ebp), %eax
	movl	$10, %ecx
	cltd
	idivl	%ecx
	movl	%eax, -8(%ebp)
	jmp	LBB4_4
LBB4_6:
	movl	-12(%ebp), %eax
	leal	"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@", %ecx
	movl	%ecx, (%esp)
	movl	%eax, 4(%esp)
	calll	_printf
LBB4_7:
	leal	"??_C@_0BI@OGMALBCI@Press?5Enter?5to?5exit?4?4?4?6?$AA@", %eax
	movl	%eax, (%esp)
	calll	_printf
	calll	_getchar
	calll	_getchar
	xorl	%eax, %eax
	addl	$20, %esp
	popl	%ebp
	retl
                                        # -- End function
	.def	_printf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_printf
	.globl	_printf                         # -- Begin function printf
	.p2align	4, 0x90
_printf:                                # @printf
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$28, %esp
	movl	8(%ebp), %eax
	leal	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %esi
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)                 # 4-byte Spill
	movl	%esp, %eax
	movl	$1, (%eax)
	calll	___acrt_iob_func
	movl	-16(%ebp), %edx                 # 4-byte Reload
	movl	%eax, %ecx
	movl	%esp, %eax
	movl	%esi, 12(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	movl	$0, 8(%eax)
	calll	__vfprintf_l
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	addl	$28, %esp
	popl	%esi
	popl	%ebp
	retl
                                        # -- End function
	.def	_scanf;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_scanf
	.globl	_scanf                          # -- Begin function scanf
	.p2align	4, 0x90
_scanf:                                 # @scanf
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$28, %esp
	movl	8(%ebp), %eax
	leal	12(%ebp), %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %esi
	movl	8(%ebp), %eax
	movl	%eax, -16(%ebp)                 # 4-byte Spill
	movl	%esp, %eax
	movl	$0, (%eax)
	calll	___acrt_iob_func
	movl	-16(%ebp), %edx                 # 4-byte Reload
	movl	%eax, %ecx
	movl	%esp, %eax
	movl	%esi, 12(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	movl	$0, 8(%eax)
	calll	__vfscanf_l
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	addl	$28, %esp
	popl	%esi
	popl	%ebp
	retl
                                        # -- End function
	.def	__vsprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__vsprintf_l
	.globl	__vsprintf_l                    # -- Begin function _vsprintf_l
	.p2align	4, 0x90
__vsprintf_l:                           # @_vsprintf_l
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%esi
	subl	$20, %esp
	movl	20(%ebp), %eax
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	movl	20(%ebp), %eax
	movl	16(%ebp), %ecx
	movl	12(%ebp), %edx
	movl	8(%ebp), %esi
	movl	%esi, (%esp)
	movl	$-1, 4(%esp)
	movl	%edx, 8(%esp)
	movl	%ecx, 12(%esp)
	movl	%eax, 16(%esp)
	calll	__vsnprintf_l
	addl	$20, %esp
	popl	%esi
	popl	%ebp
	retl
                                        # -- End function
	.def	__vsnprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__vsnprintf_l
	.globl	__vsnprintf_l                   # -- Begin function _vsnprintf_l
	.p2align	4, 0x90
__vsnprintf_l:                          # @_vsnprintf_l
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	subl	$48, %esp
	movl	24(%ebp), %eax
	movl	20(%ebp), %eax
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	movl	24(%ebp), %eax
	movl	%eax, -28(%ebp)                 # 4-byte Spill
	movl	20(%ebp), %eax
	movl	%eax, -24(%ebp)                 # 4-byte Spill
	movl	16(%ebp), %ebx
	movl	12(%ebp), %edi
	movl	8(%ebp), %esi
	calll	___local_stdio_printf_options
	movl	-28(%ebp), %ecx                 # 4-byte Reload
	movl	%eax, %edx
	movl	(%edx), %eax
	movl	4(%edx), %edx
	orl	$1, %eax
	movl	%eax, -20(%ebp)                 # 4-byte Spill
	movl	%esp, %eax
	movl	%ecx, 24(%eax)
	movl	-24(%ebp), %ecx                 # 4-byte Reload
	movl	%ecx, 20(%eax)
	movl	-20(%ebp), %ecx                 # 4-byte Reload
	movl	%ebx, 16(%eax)
	movl	%edi, 12(%eax)
	movl	%esi, 8(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	calll	___stdio_common_vsprintf
	movl	%eax, -16(%ebp)
	cmpl	$0, -16(%ebp)
	jge	LBB8_2
# %bb.1:
	movl	$4294967295, %eax               # imm = 0xFFFFFFFF
	movl	%eax, -32(%ebp)                 # 4-byte Spill
	jmp	LBB8_3
LBB8_2:
	movl	-16(%ebp), %eax
	movl	%eax, -32(%ebp)                 # 4-byte Spill
LBB8_3:
	movl	-32(%ebp), %eax                 # 4-byte Reload
	addl	$48, %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	popl	%ebp
	retl
                                        # -- End function
	.def	___local_stdio_printf_options;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,___local_stdio_printf_options
	.globl	___local_stdio_printf_options   # -- Begin function __local_stdio_printf_options
	.p2align	4, 0x90
___local_stdio_printf_options:          # @__local_stdio_printf_options
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	leal	___local_stdio_printf_options._OptionsStorage, %eax
	popl	%ebp
	retl
                                        # -- End function
	.def	__vfprintf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__vfprintf_l
	.globl	__vfprintf_l                    # -- Begin function _vfprintf_l
	.p2align	4, 0x90
__vfprintf_l:                           # @_vfprintf_l
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	subl	$32, %esp
	movl	20(%ebp), %eax
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	movl	20(%ebp), %eax
	movl	%eax, -20(%ebp)                 # 4-byte Spill
	movl	16(%ebp), %ebx
	movl	12(%ebp), %edi
	movl	8(%ebp), %esi
	calll	___local_stdio_printf_options
	movl	-20(%ebp), %ecx                 # 4-byte Reload
	movl	(%eax), %edx
	movl	%edx, -16(%ebp)                 # 4-byte Spill
	movl	4(%eax), %edx
	movl	%esp, %eax
	movl	%ecx, 20(%eax)
	movl	-16(%ebp), %ecx                 # 4-byte Reload
	movl	%ebx, 16(%eax)
	movl	%edi, 12(%eax)
	movl	%esi, 8(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	calll	___stdio_common_vfprintf
	addl	$32, %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	popl	%ebp
	retl
                                        # -- End function
	.def	__vfscanf_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__vfscanf_l
	.globl	__vfscanf_l                     # -- Begin function _vfscanf_l
	.p2align	4, 0x90
__vfscanf_l:                            # @_vfscanf_l
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%edi
	pushl	%esi
	subl	$32, %esp
	movl	20(%ebp), %eax
	movl	16(%ebp), %eax
	movl	12(%ebp), %eax
	movl	8(%ebp), %eax
	movl	20(%ebp), %eax
	movl	%eax, -20(%ebp)                 # 4-byte Spill
	movl	16(%ebp), %ebx
	movl	12(%ebp), %edi
	movl	8(%ebp), %esi
	calll	___local_stdio_scanf_options
	movl	-20(%ebp), %ecx                 # 4-byte Reload
	movl	(%eax), %edx
	movl	%edx, -16(%ebp)                 # 4-byte Spill
	movl	4(%eax), %edx
	movl	%esp, %eax
	movl	%ecx, 20(%eax)
	movl	-16(%ebp), %ecx                 # 4-byte Reload
	movl	%ebx, 16(%eax)
	movl	%edi, 12(%eax)
	movl	%esi, 8(%eax)
	movl	%edx, 4(%eax)
	movl	%ecx, (%eax)
	calll	___stdio_common_vfscanf
	addl	$32, %esp
	popl	%esi
	popl	%edi
	popl	%ebx
	popl	%ebp
	retl
                                        # -- End function
	.def	___local_stdio_scanf_options;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,___local_stdio_scanf_options
	.globl	___local_stdio_scanf_options    # -- Begin function __local_stdio_scanf_options
	.p2align	4, 0x90
___local_stdio_scanf_options:           # @__local_stdio_scanf_options
# %bb.0:
	pushl	%ebp
	movl	%esp, %ebp
	leal	___local_stdio_scanf_options._OptionsStorage, %eax
	popl	%ebp
	retl
                                        # -- End function
	.section	.rdata,"dr",discard,"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@"
	.globl	"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@" # @"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@"
"??_C@_0BN@KLGEJOLC@Enter?5a?5three?9digit?5number?3?5?$AA@":
	.asciz	"Enter a three-digit number: "

	.section	.rdata,"dr",discard,"??_C@_02DPKJAMEF@?$CFd?$AA@"
	.globl	"??_C@_02DPKJAMEF@?$CFd?$AA@"   # @"??_C@_02DPKJAMEF@?$CFd?$AA@"
"??_C@_02DPKJAMEF@?$CFd?$AA@":
	.asciz	"%d"

	.section	.rdata,"dr",discard,"??_C@_0CN@NLFIELDG@Error?3?5a?5non?9three?9digit?5number?5@"
	.globl	"??_C@_0CN@NLFIELDG@Error?3?5a?5non?9three?9digit?5number?5@" # @"??_C@_0CN@NLFIELDG@Error?3?5a?5non?9three?9digit?5number?5@"
"??_C@_0CN@NLFIELDG@Error?3?5a?5non?9three?9digit?5number?5@":
	.asciz	"Error: a non-three-digit number was entered\n"

	.section	.rdata,"dr",discard,"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@"
	.globl	"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@" # @"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@"
"??_C@_0CH@INOANIJB@The?5sum?5of?5the?5digits?5of?5a?5numbe@":
	.asciz	"The sum of the digits of a number: %d\n"

	.section	.rdata,"dr",discard,"??_C@_0BI@OGMALBCI@Press?5Enter?5to?5exit?4?4?4?6?$AA@"
	.globl	"??_C@_0BI@OGMALBCI@Press?5Enter?5to?5exit?4?4?4?6?$AA@" # @"??_C@_0BI@OGMALBCI@Press?5Enter?5to?5exit?4?4?4?6?$AA@"
"??_C@_0BI@OGMALBCI@Press?5Enter?5to?5exit?4?4?4?6?$AA@":
	.asciz	"Press Enter to exit...\n"

	.lcomm	___local_stdio_printf_options._OptionsStorage,8,8 # @__local_stdio_printf_options._OptionsStorage
	.lcomm	___local_stdio_scanf_options._OptionsStorage,8,8 # @__local_stdio_scanf_options._OptionsStorage
	.addrsig
	.addrsig_sym __vsnprintf
	.addrsig_sym _printf
	.addrsig_sym _scanf
	.addrsig_sym _getchar
	.addrsig_sym __vsprintf_l
	.addrsig_sym __vsnprintf_l
	.addrsig_sym ___stdio_common_vsprintf
	.addrsig_sym ___local_stdio_printf_options
	.addrsig_sym __vfprintf_l
	.addrsig_sym ___acrt_iob_func
	.addrsig_sym ___stdio_common_vfprintf
	.addrsig_sym __vfscanf_l
	.addrsig_sym ___stdio_common_vfscanf
	.addrsig_sym ___local_stdio_scanf_options
	.addrsig_sym ___local_stdio_printf_options._OptionsStorage
	.addrsig_sym ___local_stdio_scanf_options._OptionsStorage
