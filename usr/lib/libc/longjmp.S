#ifdef __ARM__

.syntax unified
.global _longjmp
.global longjmp
.type _longjmp,%function
.type longjmp,%function
_longjmp:
longjmp:
	mov ip,r0
	movs r0,r1
	moveq r0,#1
	ldmia ip!, {v1,v2,v3,v4,v5,v6,sl,fp}
	ldmia ip!, {r2,lr}
	mov sp,r2

	adr r1,1f
	ldr r2,1f
	ldr r1,[r1,r2]

#if __ARM_ARCH < 8
	tst r1,#0x260
	beq 3f
	// HWCAP_ARM_FPA
	tst r1,#0x20
	beq 2f
	ldc p2, cr4, [ip], #48
#endif
2:	tst r1,#0x40
	beq 2f
	.fpu vfp
	vldmia ip!, {d8-d15}
	.fpu softvfp
	.eabi_attribute 10, 0
	.eabi_attribute 27, 0
#if __ARM_ARCH < 8
	// HWCAP_ARM_IWMMXT
2:	tst r1,#0x200
	beq 3f
	ldcl p1, cr10, [ip], #8
	ldcl p1, cr11, [ip], #8
	ldcl p1, cr12, [ip], #8
	ldcl p1, cr13, [ip], #8
	ldcl p1, cr14, [ip], #8
	ldcl p1, cr15, [ip], #8
#endif
2:
3:	bx lr

.hidden __hwcap
.align 2
1:	.word __hwcap-1b

#else /* __ARM64__ */

.global _longjmp
.global longjmp
.type _longjmp,%function
.type longjmp,%function
_longjmp:
longjmp:
	// IHI0055B_aapcs64.pdf 5.1.1, 5.1.2 callee saved registers
	ldp x19, x20, [x0,#0]
	ldp x21, x22, [x0,#16]
	ldp x23, x24, [x0,#32]
	ldp x25, x26, [x0,#48]
	ldp x27, x28, [x0,#64]
	ldp x29, x30, [x0,#80]
	ldr x2, [x0,#104]
	mov sp, x2
	ldp d8 , d9, [x0,#112]
	ldp d10, d11, [x0,#128]
	ldp d12, d13, [x0,#144]
	ldp d14, d15, [x0,#160]

	cmp w1, 0
	csinc w0, w1, wzr, ne
	br x30

#endif /* __ARM__ */
