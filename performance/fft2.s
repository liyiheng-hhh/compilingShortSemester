	.bss
	.align	2
	.globl	d
d:
	.zero	4
	.bss
	.align	2
	.globl	temp
temp:
	.zero	8388608
	.bss
	.align	2
	.globl	a
a:
	.zero	8388608
	.bss
	.align	2
	.globl	b
b:
	.zero	8388608
	.bss
	.align	2
	.globl	c
c:
	.zero	8388608
	.text
	.text
	.align	1
	.globl	multiply
	.type	multiply, @function
multiply:
	addi	sp, sp, -208
	li	t0, 208
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_multiply_0
	li	a0, 0
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_multiply_0
.L_ifend_multiply_0:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_multiply_1
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 998244353
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1154949188
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 28
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 998244353
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_multiply_0
.L_ifend_multiply_1:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	li	a0, 2
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -64(s0)
	addi	sp, sp, -16
	lw	a0, -40(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	slliw	a0, a0, 1
	sw	a0, -80(s0)
	li	a0, 998244353
	sw	a0, -88(s0)
	lw	a0, -80(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1154949188
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 28
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 998244353
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -96(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	li	a0, 2
	sw	a0, -112(s0)
	lw	a0, -104(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	lw	a0, -120(s0)
	addiw	a0, a0, -1
	sw	a0, -136(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_ifelse_multiply_3
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lw	t2, -96(s0)
	addw	a0, t2, a0
	sw	a0, -48(s0)
	li	a0, 998244353
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1154949188
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 28
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 998244353
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	sext.w	a0, a0
	j	.Lreturn_multiply_0
	j	.L_ifend_multiply_2
.L_ifelse_multiply_3:
	lw	a0, -96(s0)
	sext.w	a0, a0
	j	.Lreturn_multiply_0
.L_ifend_multiply_2:
	li	a0, 0
	j	.Lreturn_multiply_0
	li	a0, 0
.Lreturn_multiply_0:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	multiply, .-multiply
	.text
	.align	1
	.globl	power
	.type	power, @function
power:
	addi	sp, sp, -192
	li	t0, 192
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_power_0
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_power_1
.L_ifend_power_0:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	li	a0, 2
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -64(s0)
	addi	sp, sp, -16
	lw	a0, -40(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	power
	addi	sp, sp, 16
	sw	a0, -72(s0)
	addi	sp, sp, -16
	lw	a0, -72(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -80(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	li	a0, 2
	sw	a0, -96(s0)
	lw	a0, -88(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -104(s0)
	li	a0, 1
	sw	a0, -112(s0)
	lw	a0, -104(s0)
	addiw	a0, a0, -1
	sw	a0, -120(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_ifelse_power_2
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	addi	sp, sp, -16
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -40(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_power_1
	j	.L_ifend_power_1
.L_ifelse_power_2:
	lw	a0, -80(s0)
	sext.w	a0, a0
	j	.Lreturn_power_1
.L_ifend_power_1:
	li	a0, 0
	j	.Lreturn_power_1
	li	a0, 0
.Lreturn_power_1:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	power, .-power
	.text
	.align	1
	.globl	memmove1
	.type	memmove1, @function
memmove1:
	addi	sp, sp, -240
	li	t0, 240
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	sw	a1, -28(s0)
	sd	a2, -40(s0)
	sw	a3, -44(s0)
	mv	t4, a1
	mv	t5, a3
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
.L_wh_memmove1_0:
	mv	a0, t5
	sw	a0, -56(s0)
	ld	a0, -40(s0)
	sd	a0, -64(s0)
	ld	a0, -24(s0)
	sd	a0, -72(s0)
	mv	a0, t4
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_memmove1_1
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -64(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -136(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lw	t2, -80(s0)
	addw	a0, t2, a0
	sw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -160(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	mv	t1, a0
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_memmove1_0
.L_whe_memmove1_1:
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_memmove1_2
	li	a0, 0
.Lreturn_memmove1_2:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	memmove1, .-memmove1
	.text
	.align	1
	.globl	fft
	.type	fft, @function
fft:
	addi	sp, sp, -816
	li	t0, 816
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	sw	a1, -28(s0)
	sw	a2, -32(s0)
	sw	a3, -36(s0)
	addi	t1, s0, -40
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	lw	a0, -80(s0)
	addiw	a0, a0, -1
	sw	a0, -96(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_ifend_fft_0
	li	a0, 1
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	j	.Lreturn_fft_3
.L_ifend_fft_0:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_wh_fft_1:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	li	a0, 2
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_fft_2
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -120(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_ifelse_fft_4
	ld	a0, -24(s0)
	sd	a0, -80(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lw	t2, -136(s0)
	addw	a0, t2, a0
	sw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -160(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -176(s0)
	lla	a0, temp
	sd	a0, -184(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	li	a0, 2
	sw	a0, -200(s0)
	lw	a0, -192(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -208(s0)
	slliw	a0, a0, 2
	sw	a0, -216(s0)
	ld	t2, -184(s0)
	add	a0, t2, a0
	sd	a0, -224(s0)
	mv	t1, a0
	lw	a0, -176(s0)
	sw	a0, 0(t1)
	j	.L_ifend_fft_3
.L_ifelse_fft_4:
	ld	a0, -24(s0)
	sd	a0, -232(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	lw	t2, -240(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	slliw	a0, a0, 2
	sw	a0, -264(s0)
	ld	t2, -232(s0)
	add	a0, t2, a0
	sd	a0, -272(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -280(s0)
	lla	a0, temp
	sd	a0, -288(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	li	a0, 2
	sw	a0, -304(s0)
	lw	a0, -296(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -312(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -328(s0)
	lw	t2, -312(s0)
	addw	a0, t2, a0
	sw	a0, -336(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	ld	t2, -288(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	mv	t1, a0
	lw	a0, -280(s0)
	sw	a0, 0(t1)
.L_ifend_fft_3:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -360(s0)
	addiw	a0, a0, 1
	sw	a0, -368(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_fft_1
.L_whe_fft_2:
	ld	a0, -24(s0)
	sd	a0, -72(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lla	a0, temp
	sd	a0, -88(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	sp, sp, -32
	ld	a0, -72(s0)
	sd	a0, 0(sp)
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, -88(s0)
	sd	a0, 16(sp)
	lw	a0, -96(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	memmove1
	addi	sp, sp, 32
	sw	a0, -104(s0)
	ld	a0, -24(s0)
	sd	a0, -112(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	li	a0, 2
	sw	a0, -136(s0)
	lw	a0, -128(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	addi	sp, sp, -16
	lw	a0, -152(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -160(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -168(s0)
	addi	sp, sp, -32
	ld	a0, -112(s0)
	sd	a0, 0(sp)
	lw	a0, -120(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	lw	a0, -144(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	lw	a0, -168(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	fft
	addi	sp, sp, 32
	sw	a0, -176(s0)
	ld	a0, -24(s0)
	sd	a0, -184(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	li	a0, 2
	sw	a0, -208(s0)
	lw	a0, -200(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -216(s0)
	lw	t2, -192(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -240(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	addi	sp, sp, -16
	lw	a0, -248(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -256(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -264(s0)
	addi	sp, sp, -32
	ld	a0, -184(s0)
	sd	a0, 0(sp)
	lw	a0, -224(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	lw	a0, -240(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	lw	a0, -264(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	fft
	addi	sp, sp, 32
	sw	a0, -272(s0)
	li	a0, 0
	sw	a0, -280(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -288(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh_fft_5:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	li	a0, 2
	sw	a0, -80(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -88(s0)
	ld	a0, -24(s0)
	sd	a0, -96(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	li	a0, 998244353
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -168(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	li	a0, 1
	sw	a0, -184(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_whe_fft_6
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	lw	t2, -104(s0)
	addw	a0, t2, a0
	sw	a0, -216(s0)
	slliw	a0, a0, 2
	sw	a0, -224(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -232(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -240(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	lw	t2, -112(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	mv	t2, a0
	lw	a0, -128(s0)
	addw	a0, t2, a0
	sw	a0, -264(s0)
	slliw	a0, a0, 2
	sw	a0, -272(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -280(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -288(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	addi	sp, sp, -16
	lw	a0, -296(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -288(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -304(s0)
	lw	t2, -240(s0)
	addw	a0, t2, a0
	sw	a0, -312(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1154949188
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 28
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 998244353
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -320(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	lw	t2, -144(s0)
	addw	a0, t2, a0
	sw	a0, -336(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	mv	t1, a0
	lw	a0, -320(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -360(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -368(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	addi	sp, sp, -16
	lw	a0, -368(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -376(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -384(s0)
	lw	t2, -360(s0)
	subw	a0, t2, a0
	sw	a0, -392(s0)
	mv	t2, a0
	lw	a0, -136(s0)
	addw	a0, t2, a0
	sw	a0, -400(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1154949188
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 28
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 998244353
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -408(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -416(s0)
	lw	t2, -152(s0)
	addw	a0, t2, a0
	sw	a0, -424(s0)
	mv	t2, a0
	lw	a0, -168(s0)
	addw	a0, t2, a0
	sw	a0, -432(s0)
	slliw	a0, a0, 2
	sw	a0, -440(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -448(s0)
	mv	t1, a0
	lw	a0, -408(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -456(s0)
	addi	sp, sp, -16
	lw	a0, -456(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -176(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -464(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -472(s0)
	addiw	a0, a0, 1
	sw	a0, -480(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_fft_5
.L_whe_fft_6:
	li	a0, 0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	j	.Lreturn_fft_3
	li	a0, 0
.Lreturn_fft_3:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	fft, .-fft
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -400
	li	t0, 400
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	lla	a0, a
	sd	a0, -48(s0)
	ld	a0, -48(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -56(s0)
	lla	a0, b
	sd	a0, -64(s0)
	ld	a0, -64(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -72(s0)
	li	a0, 60
	sw	a0, -80(s0)
	addi	sp, sp, -16
	li	a0, 60
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 1
	sw	a0, -88(s0)
	lla	t1, d
	sw	a0, 0(t1)
.L_wh_main_0:
	lw	t2, -56(s0)
	lw	a0, -72(s0)
	addw	a0, t2, a0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -1
	sw	a0, -64(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_1
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	slliw	a0, a0, 1
	sw	a0, -104(s0)
	lla	t1, d
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	lla	a0, a
	sd	a0, -40(s0)
	li	a0, 0
	sw	a0, -48(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	li	a0, 5
	sw	a0, -80(s0)
	li	a0, 998244352
	sw	a0, -88(s0)
	mv	t2, a0
	lw	a0, -64(s0)
	divw	a0, t2, a0
	sw	a0, -96(s0)
	addi	sp, sp, -16
	li	a0, 5
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -96(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	power
	addi	sp, sp, 16
	sw	a0, -104(s0)
	addi	sp, sp, -32
	ld	a0, -40(s0)
	sd	a0, 0(sp)
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 8(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	lw	a0, -104(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	fft
	addi	sp, sp, 32
	sw	a0, -112(s0)
	lla	a0, b
	sd	a0, -120(s0)
	li	a0, 0
	sw	a0, -128(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	li	a0, 5
	sw	a0, -144(s0)
	li	a0, 998244352
	sw	a0, -152(s0)
	mv	t2, a0
	lw	a0, -136(s0)
	divw	a0, t2, a0
	sw	a0, -160(s0)
	addi	sp, sp, -16
	li	a0, 5
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -160(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	power
	addi	sp, sp, 16
	sw	a0, -168(s0)
	addi	sp, sp, -32
	ld	a0, -120(s0)
	sd	a0, 0(sp)
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 8(sp)
	lw	a0, -136(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	lw	a0, -168(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	fft
	addi	sp, sp, 32
	sw	a0, -176(s0)
	li	a0, 0
	sw	a0, -184(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_2:
	lla	a0, a
	sd	a0, -40(s0)
	lla	a0, b
	sd	a0, -48(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	t2, -80(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_main_3
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -128(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	slliw	a0, a0, 2
	sw	a0, -144(s0)
	ld	t2, -48(s0)
	add	a0, t2, a0
	sd	a0, -152(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -160(s0)
	addi	sp, sp, -16
	lw	a0, -128(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -160(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -168(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	mv	t1, a0
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	lla	a0, a
	sd	a0, -40(s0)
	li	a0, 0
	sw	a0, -48(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	li	a0, 5
	sw	a0, -80(s0)
	li	a0, 998244352
	sw	a0, -88(s0)
	mv	t2, a0
	lw	a0, -64(s0)
	divw	a0, t2, a0
	sw	a0, -96(s0)
	lw	t2, -88(s0)
	subw	a0, t2, a0
	sw	a0, -104(s0)
	addi	sp, sp, -16
	li	a0, 5
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -104(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	power
	addi	sp, sp, 16
	sw	a0, -112(s0)
	addi	sp, sp, -32
	ld	a0, -40(s0)
	sd	a0, 0(sp)
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 8(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	lw	a0, -112(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	fft
	addi	sp, sp, 32
	sw	a0, -120(s0)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_4:
	lla	a0, a
	sd	a0, -40(s0)
	li	a0, 998244351
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	t2, -80(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_main_5
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -128(s0)
	lla	t0, d
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addi	sp, sp, -16
	lw	a0, -136(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	li	a0, 998244351
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	power
	addi	sp, sp, 16
	sw	a0, -144(s0)
	addi	sp, sp, -16
	lw	a0, -128(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -144(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	multiply
	addi	sp, sp, 16
	sw	a0, -152(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	slliw	a0, a0, 2
	sw	a0, -168(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	mv	t1, a0
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	addiw	a0, a0, 1
	sw	a0, -192(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	li	a0, 79
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 79
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	t2, -56(s0)
	lw	a0, -72(s0)
	addw	a0, t2, a0
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -80(s0)
	lla	a0, a
	sd	a0, -88(s0)
	addi	sp, sp, -16
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -88(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	sext.w	a0, a0
	j	.Lreturn_main_4
	li	a0, 0
.Lreturn_main_4:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
