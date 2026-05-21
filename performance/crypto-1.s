	.data
	.align	2
	.globl	state
state:
	.word	19260817
	.bss
	.align	2
	.globl	buffer
buffer:
	.zero	131072
	.text
	.text
	.align	1
	.globl	get_random
	.type	get_random, @function
get_random:
	addi	sp, sp, -144
	addi	t0, sp, 144
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -88(s0)
	sd	s2, -96(s0)
	sd	s3, -104(s0)
	sd	s4, -112(s0)
	sd	s5, -120(s0)
	sd	s6, -128(s0)
	lla	t0, state
	lw	a0, 0(t0)
	mv	s1, a0
	sw	s1, -24(s0)
	mv	a0, s1
	slliw	a0, a0, 13
	mv	s2, a0
	sw	s2, -32(s0)
	mv	t2, s1
	mv	a0, s2
	addw	a0, t2, a0
	mv	s3, a0
	sw	s3, -40(s0)
	lla	t1, state
	mv	a0, s3
	sw	a0, 0(t1)
	li	a0, 131072
	mv	s4, a0
	sw	s4, -48(s0)
	mv	a0, s3
	sext.w	a0, a0
	sraiw	t1, a0, 31
	li	t2, 131071
	and	t1, t1, t2
	addw	a0, a0, t1
	sraiw	a0, a0, 17
	mv	s5, a0
	sw	s5, -56(s0)
	mv	t2, s3
	mv	a0, s5
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -64(s0)
	lla	t1, state
	mv	a0, s6
	sw	a0, 0(t1)
	mv	a0, s6
	slliw	a0, a0, 5
	sw	a0, -72(s0)
	mv	t2, s6
	lw	a0, -72(s0)
	addw	a0, t2, a0
	sw	a0, -80(s0)
	lla	t1, state
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	lw	a0, -80(s0)
	sext.w	a0, a0
	j	.Lreturn_get_random_0
	li	a0, 0
.Lreturn_get_random_0:
	ld	s6, -128(s0)
	ld	s5, -120(s0)
	ld	s4, -112(s0)
	ld	s3, -104(s0)
	ld	s2, -96(s0)
	ld	s1, -88(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	get_random, .-get_random
	.text
	.align	1
	.globl	rotl1
	.type	rotl1, @function
rotl1:
	addi	sp, sp, -144
	addi	t0, sp, 144
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -88(s0)
	sd	s2, -96(s0)
	sd	s3, -104(s0)
	sd	s4, -112(s0)
	sd	s5, -120(s0)
	sd	s6, -128(s0)
	sw	a0, -20(s0)
	li	a0, 2
	mv	s2, a0
	sw	s2, -40(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -48(s0)
	mv	a0, s1
	slliw	a0, a0, 1
	mv	s3, a0
	sw	s3, -56(s0)
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	mv	a0, s4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s5, a0
	sw	s5, -72(s0)
	mv	t2, s3
	mv	a0, s5
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -80(s0)
	mv	a0, s6
	sext.w	a0, a0
	j	.Lreturn_rotl1_1
	li	a0, 0
.Lreturn_rotl1_1:
	ld	s6, -128(s0)
	ld	s5, -120(s0)
	ld	s4, -112(s0)
	ld	s3, -104(s0)
	ld	s2, -96(s0)
	ld	s1, -88(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rotl1, .-rotl1
	.text
	.align	1
	.globl	rotl5
	.type	rotl5, @function
rotl5:
	addi	sp, sp, -144
	addi	t0, sp, 144
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -88(s0)
	sd	s2, -96(s0)
	sd	s3, -104(s0)
	sd	s4, -112(s0)
	sd	s5, -120(s0)
	sd	s6, -128(s0)
	sw	a0, -20(s0)
	li	a0, 32
	mv	s2, a0
	sw	s2, -40(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -48(s0)
	mv	a0, s1
	slliw	a0, a0, 5
	mv	s3, a0
	sw	s3, -56(s0)
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	mv	a0, s4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 31
	addw	a0, a0, t1
	sraiw	a0, a0, 5
	li	t1, 32
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s5, a0
	sw	s5, -72(s0)
	mv	t2, s3
	mv	a0, s5
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -80(s0)
	mv	a0, s6
	sext.w	a0, a0
	j	.Lreturn_rotl5_2
	li	a0, 0
.Lreturn_rotl5_2:
	ld	s6, -128(s0)
	ld	s5, -120(s0)
	ld	s4, -112(s0)
	ld	s3, -104(s0)
	ld	s2, -96(s0)
	ld	s1, -88(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rotl5, .-rotl5
	.text
	.align	1
	.globl	rotl30
	.type	rotl30, @function
rotl30:
	addi	sp, sp, -144
	addi	t0, sp, 144
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -88(s0)
	sd	s2, -96(s0)
	sd	s3, -104(s0)
	sd	s4, -112(s0)
	sd	s5, -120(s0)
	sd	s6, -128(s0)
	sw	a0, -20(s0)
	li	a0, 1073741824
	mv	s2, a0
	sw	s2, -40(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -48(s0)
	mv	a0, s1
	slliw	a0, a0, 30
	mv	s3, a0
	sw	s3, -56(s0)
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	mv	a0, s4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	li	t2, 1073741823
	and	t1, t1, t2
	addw	a0, a0, t1
	sraiw	a0, a0, 30
	li	t1, 1073741824
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s5, a0
	sw	s5, -72(s0)
	mv	t2, s3
	mv	a0, s5
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -80(s0)
	mv	a0, s6
	sext.w	a0, a0
	j	.Lreturn_rotl30_3
	li	a0, 0
.Lreturn_rotl30_3:
	ld	s6, -128(s0)
	ld	s5, -120(s0)
	ld	s4, -112(s0)
	ld	s3, -104(s0)
	ld	s2, -96(s0)
	ld	s1, -88(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rotl30, .-rotl30
	.text
	.align	1
	.globl	_and
	.type	_and, @function
_and:
	addi	sp, sp, -96
	addi	t0, sp, 96
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -64(s0)
	sd	s2, -72(s0)
	sd	s3, -80(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	mv	t2, s1
	mv	a0, s2
	addw	a0, t2, a0
	mv	s3, a0
	sw	s3, -56(s0)
	mv	a0, s3
	sext.w	a0, a0
	j	.Lreturn__and_4
	li	a0, 0
.Lreturn__and_4:
	ld	s3, -80(s0)
	ld	s2, -72(s0)
	ld	s1, -64(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_and, .-_and
	.text
	.align	1
	.globl	_not
	.type	_not, @function
_not:
	addi	sp, sp, -96
	addi	t0, sp, 96
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -64(s0)
	sd	s2, -72(s0)
	sd	s3, -80(s0)
	sw	a0, -20(s0)
	li	a0, -1
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	mv	t2, s1
	mv	a0, s2
	subw	a0, t2, a0
	mv	s3, a0
	sw	s3, -56(s0)
	mv	a0, s3
	sext.w	a0, a0
	j	.Lreturn__not_5
	li	a0, 0
.Lreturn__not_5:
	ld	s3, -80(s0)
	ld	s2, -72(s0)
	ld	s1, -64(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_not, .-_not
	.text
	.align	1
	.globl	_xor
	.type	_xor, @function
_xor:
	addi	sp, sp, -224
	addi	t0, sp, 224
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -128(s0)
	sd	s2, -136(s0)
	sd	s3, -144(s0)
	sd	s4, -152(s0)
	sd	s5, -160(s0)
	sd	s6, -168(s0)
	sd	s7, -176(s0)
	sd	s8, -184(s0)
	sd	s9, -192(s0)
	sd	s10, -200(s0)
	sd	s11, -208(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	lw	a0, -24(s0)
	mv	s3, a0
	sw	s3, -56(s0)
	addi	sp, sp, -16
	mv	a0, s2
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s3
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	mv	s4, a0
	sw	s4, -64(s0)
	mv	t2, s1
	mv	a0, s4
	subw	a0, t2, a0
	mv	s5, a0
	sw	s5, -72(s0)
	lw	a0, -24(s0)
	mv	s6, a0
	sw	s6, -80(s0)
	mv	t2, s5
	mv	a0, s6
	addw	a0, t2, a0
	mv	s7, a0
	sw	s7, -88(s0)
	lw	a0, -20(s0)
	mv	s8, a0
	sw	s8, -96(s0)
	lw	a0, -24(s0)
	mv	s9, a0
	sw	s9, -104(s0)
	addi	sp, sp, -16
	mv	a0, s8
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s9
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	mv	s10, a0
	sw	s10, -112(s0)
	mv	t2, s7
	mv	a0, s10
	subw	a0, t2, a0
	mv	s11, a0
	sw	s11, -120(s0)
	mv	a0, s11
	sext.w	a0, a0
	j	.Lreturn__xor_6
	li	a0, 0
.Lreturn__xor_6:
	ld	s11, -208(s0)
	ld	s10, -200(s0)
	ld	s9, -192(s0)
	ld	s8, -184(s0)
	ld	s7, -176(s0)
	ld	s6, -168(s0)
	ld	s5, -160(s0)
	ld	s4, -152(s0)
	ld	s3, -144(s0)
	ld	s2, -136(s0)
	ld	s1, -128(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_xor, .-_xor
	.text
	.align	1
	.globl	_or
	.type	_or, @function
_or:
	addi	sp, sp, -160
	addi	t0, sp, 160
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -96(s0)
	sd	s2, -104(s0)
	sd	s3, -112(s0)
	sd	s4, -120(s0)
	sd	s5, -128(s0)
	sd	s6, -136(s0)
	sd	s7, -144(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	addi	sp, sp, -16
	mv	a0, s1
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s2
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	mv	s3, a0
	sw	s3, -56(s0)
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	lw	a0, -24(s0)
	mv	s5, a0
	sw	s5, -72(s0)
	addi	sp, sp, -16
	mv	a0, s4
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s5
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	mv	s6, a0
	sw	s6, -80(s0)
	addi	sp, sp, -16
	mv	a0, s3
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s6
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	mv	s7, a0
	sw	s7, -88(s0)
	mv	a0, s7
	sext.w	a0, a0
	j	.Lreturn__or_7
	li	a0, 0
.Lreturn__or_7:
	ld	s7, -144(s0)
	ld	s6, -136(s0)
	ld	s5, -128(s0)
	ld	s4, -120(s0)
	ld	s3, -112(s0)
	ld	s2, -104(s0)
	ld	s1, -96(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_or, .-_or
	.text
	.align	1
	.globl	pseudo_md5
	.type	pseudo_md5, @function
pseudo_md5:
	addi	sp, sp, -1808
	addi	t0, sp, 1808
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -1720(s0)
	sd	s2, -1728(s0)
	sd	s3, -1736(s0)
	sd	s4, -1744(s0)
	sd	s5, -1752(s0)
	sd	s6, -1760(s0)
	sd	s7, -1768(s0)
	sd	s8, -1776(s0)
	sd	s9, -1784(s0)
	sd	s10, -1792(s0)
	sd	s11, -1800(s0)
	sd	a0, -24(s0)
	sw	a1, -28(s0)
	sd	a2, -40(s0)
	li	a0, 1732584193
	sw	a0, -696(s0)
	addi	t1, s0, -44
	lw	a0, -696(s0)
	sw	a0, 0(t1)
	li	a0, -271733879
	sw	a0, -704(s0)
	addi	t1, s0, -48
	lw	a0, -704(s0)
	sw	a0, 0(t1)
	li	a0, -1732584194
	sw	a0, -712(s0)
	addi	t1, s0, -52
	lw	a0, -712(s0)
	sw	a0, 0(t1)
	li	a0, 271733878
	sw	a0, -720(s0)
	addi	t1, s0, -56
	lw	a0, -720(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -728(s0)
	addi	t1, s0, -60
	lw	a0, -728(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -736(s0)
	addi	t1, s0, -64
	lw	a0, -736(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -744(s0)
	addi	t1, s0, -68
	lw	a0, -744(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -752(s0)
	addi	t1, s0, -72
	lw	a0, -752(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -760(s0)
	addi	t1, s0, -76
	lw	a0, -760(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -768(s0)
	addi	t1, s0, -80
	lw	a0, -768(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -776(s0)
	addi	t1, s0, -84
	lw	a0, -776(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -784(s0)
	addi	t1, s0, -600
	lw	a0, -784(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_0:
	li	a0, 64
	sw	a0, -696(s0)
	li	a0, 16
	sw	a0, -704(s0)
	li	a0, 1
	sw	a0, -712(s0)
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -696(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -728(s0)
	lw	a0, -728(s0)
	beqz	a0, .L_whe_pseudo_md5_1
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -704(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -744(s0)
	lw	a0, -744(s0)
	beqz	a0, .L_ifelse_pseudo_md5_3
	li	a0, 4
	sw	a0, -704(s0)
	lw	a0, -600(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 3
	addw	a0, a0, t1
	sraiw	a0, a0, 2
	li	t1, 4
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -760(s0)
	lw	a0, -760(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -768(s0)
	lw	a0, -768(s0)
	beqz	a0, .L_ifelse_pseudo_md5_5
	li	a0, 124429432
	sw	a0, -776(s0)
	addi	a0, s0, -340
	sd	a0, -784(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -800(s0)
	ld	t2, -784(s0)
	lw	a0, -800(s0)
	add	a0, t2, a0
	sd	a0, -808(s0)
	ld	t1, -808(s0)
	lw	a0, -776(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_4
.L_ifelse_pseudo_md5_5:
	li	a0, 4
	sw	a0, -816(s0)
	li	a0, 1
	sw	a0, -824(s0)
	lw	a0, -600(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 3
	addw	a0, a0, t1
	sraiw	a0, a0, 2
	li	t1, 4
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -840(s0)
	lw	a0, -840(s0)
	addiw	a0, a0, -1
	sw	a0, -848(s0)
	lw	a0, -848(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -856(s0)
	lw	a0, -856(s0)
	beqz	a0, .L_ifelse_pseudo_md5_7
	li	a0, 147306326
	sw	a0, -864(s0)
	addi	a0, s0, -340
	sd	a0, -872(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -888(s0)
	ld	t2, -872(s0)
	lw	a0, -888(s0)
	add	a0, t2, a0
	sd	a0, -896(s0)
	ld	t1, -896(s0)
	lw	a0, -864(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_6
.L_ifelse_pseudo_md5_7:
	li	a0, 4
	sw	a0, -904(s0)
	li	a0, 2
	sw	a0, -912(s0)
	lw	a0, -600(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 3
	addw	a0, a0, t1
	sraiw	a0, a0, 2
	li	t1, 4
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -928(s0)
	lw	a0, -928(s0)
	addiw	a0, a0, -2
	sw	a0, -936(s0)
	lw	a0, -936(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -944(s0)
	lw	a0, -944(s0)
	beqz	a0, .L_ifelse_pseudo_md5_9
	li	a0, 69234907
	sw	a0, -952(s0)
	addi	a0, s0, -340
	sd	a0, -960(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -976(s0)
	ld	t2, -960(s0)
	lw	a0, -976(s0)
	add	a0, t2, a0
	sd	a0, -984(s0)
	ld	t1, -984(s0)
	lw	a0, -952(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_8
.L_ifelse_pseudo_md5_9:
	li	a0, 29216494
	sw	a0, -992(s0)
	addi	a0, s0, -340
	sd	a0, -1000(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1016(s0)
	ld	t2, -1000(s0)
	lw	a0, -1016(s0)
	add	a0, t2, a0
	sd	a0, -1024(s0)
	ld	t1, -1024(s0)
	lw	a0, -992(s0)
	sw	a0, 0(t1)
.L_ifend_pseudo_md5_8:
.L_ifend_pseudo_md5_6:
.L_ifend_pseudo_md5_4:
	li	a0, 7
	sw	a0, -1032(s0)
	addi	a0, s0, -596
	sd	a0, -1040(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1056(s0)
	ld	t2, -1040(s0)
	lw	a0, -1056(s0)
	add	a0, t2, a0
	sd	a0, -1064(s0)
	ld	t1, -1064(s0)
	lw	a0, -1032(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_2
.L_ifelse_pseudo_md5_3:
	li	a0, 32
	sw	a0, -1072(s0)
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -1072(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1088(s0)
	lw	a0, -1088(s0)
	beqz	a0, .L_ifelse_pseudo_md5_11
	li	a0, 102638946
	sw	a0, -1096(s0)
	addi	a0, s0, -340
	sd	a0, -1104(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1120(s0)
	ld	t2, -1104(s0)
	lw	a0, -1120(s0)
	add	a0, t2, a0
	sd	a0, -1128(s0)
	ld	t1, -1128(s0)
	lw	a0, -1096(s0)
	sw	a0, 0(t1)
	li	a0, 5
	sw	a0, -1136(s0)
	addi	a0, s0, -596
	sd	a0, -1144(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1160(s0)
	ld	t2, -1144(s0)
	lw	a0, -1160(s0)
	add	a0, t2, a0
	sd	a0, -1168(s0)
	ld	t1, -1168(s0)
	lw	a0, -1136(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_10
.L_ifelse_pseudo_md5_11:
	li	a0, 48
	sw	a0, -1176(s0)
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -1176(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1192(s0)
	lw	a0, -1192(s0)
	beqz	a0, .L_ifelse_pseudo_md5_13
	li	a0, 228417826
	sw	a0, -1200(s0)
	addi	a0, s0, -340
	sd	a0, -1208(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1224(s0)
	ld	t2, -1208(s0)
	lw	a0, -1224(s0)
	add	a0, t2, a0
	sd	a0, -1232(s0)
	ld	t1, -1232(s0)
	lw	a0, -1200(s0)
	sw	a0, 0(t1)
	li	a0, 4
	sw	a0, -1240(s0)
	addi	a0, s0, -596
	sd	a0, -1248(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1264(s0)
	ld	t2, -1248(s0)
	lw	a0, -1264(s0)
	add	a0, t2, a0
	sd	a0, -1272(s0)
	ld	t1, -1272(s0)
	lw	a0, -1240(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_12
.L_ifelse_pseudo_md5_13:
	li	a0, 69804612
	sw	a0, -1280(s0)
	addi	a0, s0, -340
	sd	a0, -1288(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1304(s0)
	ld	t2, -1288(s0)
	lw	a0, -1304(s0)
	add	a0, t2, a0
	sd	a0, -1312(s0)
	ld	t1, -1312(s0)
	lw	a0, -1280(s0)
	sw	a0, 0(t1)
	li	a0, 6
	sw	a0, -1320(s0)
	addi	a0, s0, -596
	sd	a0, -1328(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1344(s0)
	ld	t2, -1328(s0)
	lw	a0, -1344(s0)
	add	a0, t2, a0
	sd	a0, -1352(s0)
	ld	t1, -1352(s0)
	lw	a0, -1320(s0)
	sw	a0, 0(t1)
.L_ifend_pseudo_md5_12:
.L_ifend_pseudo_md5_10:
.L_ifend_pseudo_md5_2:
	lw	a0, -600(s0)
	addiw	a0, a0, 1
	sw	a0, -1368(s0)
	addi	t1, s0, -600
	lw	a0, -1368(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_0
.L_whe_pseudo_md5_1:
	lw	a0, -28(s0)
	slliw	a0, a0, 3
	sw	a0, -704(s0)
	addi	t1, s0, -604
	lw	a0, -704(s0)
	sw	a0, 0(t1)
	li	a0, 128
	sw	a0, -712(s0)
	ld	a0, -24(s0)
	sd	a0, -720(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -736(s0)
	ld	t2, -720(s0)
	lw	a0, -736(s0)
	add	a0, t2, a0
	sd	a0, -744(s0)
	ld	t1, -744(s0)
	lw	a0, -712(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -752(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -768(s0)
	addi	t1, s0, -28
	lw	a0, -768(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_14:
	li	a0, 64
	sw	a0, -696(s0)
	li	a0, 56
	sw	a0, -704(s0)
	li	a0, 0
	sw	a0, -712(s0)
	li	a0, 1
	sw	a0, -720(s0)
	ld	a0, -24(s0)
	sd	a0, -728(s0)
	lw	a0, -28(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 63
	addw	a0, a0, t1
	sraiw	a0, a0, 6
	li	t1, 64
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -744(s0)
	lw	a0, -744(s0)
	addiw	a0, a0, -56
	sw	a0, -752(s0)
	lw	a0, -752(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -760(s0)
	lw	a0, -760(s0)
	beqz	a0, .L_whe_pseudo_md5_15
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -776(s0)
	ld	t2, -728(s0)
	lw	a0, -776(s0)
	add	a0, t2, a0
	sd	a0, -784(s0)
	ld	t1, -784(s0)
	lw	a0, -712(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -800(s0)
	addi	t1, s0, -28
	lw	a0, -800(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_14
.L_whe_pseudo_md5_15:
	ld	a0, -24(s0)
	sd	a0, -696(s0)
	lw	a0, -604(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -720(s0)
	ld	t2, -696(s0)
	lw	a0, -720(s0)
	add	a0, t2, a0
	sd	a0, -728(s0)
	ld	t1, -728(s0)
	lw	a0, -604(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -736(s0)
	addi	t1, s0, -600
	lw	a0, -736(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_16:
	li	a0, 4
	sw	a0, -696(s0)
	li	a0, 0
	sw	a0, -704(s0)
	li	a0, 1
	sw	a0, -712(s0)
	ld	a0, -24(s0)
	sd	a0, -720(s0)
	lw	a0, -28(s0)
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -696(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -744(s0)
	lw	a0, -744(s0)
	beqz	a0, .L_whe_pseudo_md5_17
	lw	a0, -600(s0)
	lw	t2, -28(s0)
	addw	a0, t2, a0
	sw	a0, -760(s0)
	lw	a0, -760(s0)
	slliw	a0, a0, 2
	sw	a0, -768(s0)
	ld	t2, -720(s0)
	lw	a0, -768(s0)
	add	a0, t2, a0
	sd	a0, -776(s0)
	ld	t1, -776(s0)
	lw	a0, -704(s0)
	sw	a0, 0(t1)
	lw	a0, -600(s0)
	addiw	a0, a0, 1
	sw	a0, -792(s0)
	addi	t1, s0, -600
	lw	a0, -792(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_16
.L_whe_pseudo_md5_17:
	li	a0, 4
	sw	a0, -696(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, 4
	sw	a0, -712(s0)
	addi	t1, s0, -28
	lw	a0, -712(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -720(s0)
	addi	t1, s0, -608
	lw	a0, -720(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_18:
	li	a0, 0
	sw	a0, -696(s0)
	li	a0, 16
	sw	a0, -704(s0)
	li	a0, 64
	sw	a0, -712(s0)
	li	a0, 1
	sw	a0, -720(s0)
	ld	a0, -24(s0)
	sd	a0, -728(s0)
	addi	a0, s0, -340
	sd	a0, -736(s0)
	addi	a0, s0, -672
	sd	a0, -744(s0)
	addi	a0, s0, -596
	sd	a0, -752(s0)
	lw	a0, -28(s0)
	lw	a0, -608(s0)
	lw	t2, -608(s0)
	sext.w	t2, t2
	lw	a0, -28(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -776(s0)
	lw	a0, -776(s0)
	beqz	a0, .L_whe_pseudo_md5_19
	addi	t1, s0, -600
	lw	a0, -696(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_20:
	lw	a0, -608(s0)
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -704(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -800(s0)
	lw	a0, -800(s0)
	beqz	a0, .L_whe_pseudo_md5_21
	lw	a0, -600(s0)
	lw	t2, -608(s0)
	addw	a0, t2, a0
	sw	a0, -816(s0)
	lw	a0, -816(s0)
	slliw	a0, a0, 2
	sw	a0, -824(s0)
	lw	a0, -824(s0)
	addiw	a0, a0, 0
	sw	a0, -832(s0)
	ld	t2, -728(s0)
	lw	a0, -832(s0)
	add	a0, t2, a0
	sd	a0, -840(s0)
	ld	t2, -840(s0)
	lw	a0, 0(t2)
	sw	a0, -848(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -864(s0)
	lw	a0, -864(s0)
	addiw	a0, a0, 0
	sw	a0, -872(s0)
	ld	t2, -744(s0)
	lw	a0, -872(s0)
	add	a0, t2, a0
	sd	a0, -880(s0)
	ld	t1, -880(s0)
	lw	a0, -848(s0)
	sw	a0, 0(t1)
	lw	a0, -600(s0)
	addiw	a0, a0, 1
	sw	a0, -896(s0)
	addi	t1, s0, -600
	lw	a0, -896(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_20
.L_whe_pseudo_md5_21:
	lw	a0, -44(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	lw	a0, -48(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	lw	a0, -52(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	lw	a0, -56(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t1, s0, -600
	lw	a0, -696(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_22:
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -712(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -928(s0)
	lw	a0, -928(s0)
	beqz	a0, .L_whe_pseudo_md5_23
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -704(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -944(s0)
	lw	a0, -944(s0)
	beqz	a0, .L_ifelse_pseudo_md5_25
	lw	a0, -64(s0)
	lw	a0, -68(s0)
	addi	sp, sp, -16
	lw	a0, -952(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -960(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	sw	a0, -968(s0)
	lw	a0, -64(s0)
	lw	a0, -976(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	_not
	sw	a0, -984(s0)
	lw	a0, -72(s0)
	addi	sp, sp, -16
	lw	a0, -984(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -992(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	sw	a0, -1000(s0)
	addi	sp, sp, -16
	lw	a0, -968(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1000(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_or
	addi	sp, sp, 16
	sw	a0, -1008(s0)
	addi	t1, s0, -76
	lw	a0, -1008(s0)
	sw	a0, 0(t1)
	lw	a0, -600(s0)
	addi	t1, s0, -80
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_24
.L_ifelse_pseudo_md5_25:
	li	a0, 32
	sw	a0, -1024(s0)
	lw	a0, -600(s0)
	lw	t2, -600(s0)
	sext.w	t2, t2
	lw	a0, -1024(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1040(s0)
	lw	a0, -1040(s0)
	beqz	a0, .L_ifelse_pseudo_md5_27
	lw	a0, -72(s0)
	lw	a0, -64(s0)
	addi	sp, sp, -16
	lw	a0, -1048(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1056(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	sw	a0, -1064(s0)
	lw	a0, -72(s0)
	lw	a0, -1072(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	_not
	sw	a0, -1080(s0)
	lw	a0, -68(s0)
	addi	sp, sp, -16
	lw	a0, -1080(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1088(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	mv	s11, a0
	sw	s11, -1096(s0)
	addi	sp, sp, -16
	lw	a0, -1064(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s11
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_or
	addi	sp, sp, 16
	mv	s1, a0
	sw	s1, -1104(s0)
	addi	t1, s0, -76
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	li	a0, 5
	mv	s9, a0
	sw	s9, -1112(s0)
	li	a0, 1
	mv	s6, a0
	sw	s6, -1120(s0)
	li	a0, 16
	mv	s4, a0
	sw	s4, -1128(s0)
	lw	a0, -600(s0)
	mv	s8, a0
	sw	s8, -1136(s0)
	mv	a0, s8
	slliw	t2, a0, 2
	addw	a0, t2, a0
	mv	s7, a0
	sw	s7, -1144(s0)
	mv	a0, s7
	addiw	a0, a0, 1
	mv	s5, a0
	sw	s5, -1152(s0)
	mv	a0, s5
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 15
	addw	a0, a0, t1
	sraiw	a0, a0, 4
	li	t1, 16
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s3, a0
	sw	s3, -1160(s0)
	addi	t1, s0, -80
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_ifend_pseudo_md5_26
.L_ifelse_pseudo_md5_27:
	li	a0, 48
	mv	s10, a0
	sw	s10, -1168(s0)
	lw	a0, -600(s0)
	mv	s2, a0
	sw	s2, -1176(s0)
	mv	t2, s2
	sext.w	t2, t2
	mv	a0, s10
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1184(s0)
	lw	a0, -1184(s0)
	beqz	a0, .L_ifelse_pseudo_md5_29
	lw	a0, -64(s0)
	lw	a0, -68(s0)
	lw	a0, -72(s0)
	addi	sp, sp, -16
	lw	a0, -1200(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1208(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sw	a0, -1216(s0)
	addi	sp, sp, -16
	lw	a0, -1192(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1216(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sw	a0, -1224(s0)
	addi	t1, s0, -76
	lw	a0, -1224(s0)
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -1232(s0)
	li	a0, 5
	sw	a0, -1240(s0)
	li	a0, 16
	sw	a0, -1248(s0)
	lw	a0, -600(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -1264(s0)
	lw	a0, -1264(s0)
	addiw	a0, a0, 5
	sw	a0, -1272(s0)
	lw	a0, -1272(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 15
	addw	a0, a0, t1
	sraiw	a0, a0, 4
	li	t1, 16
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -1280(s0)
	addi	t1, s0, -80
	lw	a0, -1280(s0)
	sw	a0, 0(t1)
	j	.L_ifend_pseudo_md5_28
.L_ifelse_pseudo_md5_29:
	lw	a0, -68(s0)
	lw	a0, -64(s0)
	lw	a0, -72(s0)
	lw	a0, -1304(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	_not
	sw	a0, -1312(s0)
	addi	sp, sp, -16
	lw	a0, -1296(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1312(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_or
	addi	sp, sp, 16
	sw	a0, -1320(s0)
	addi	sp, sp, -16
	lw	a0, -1288(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -1320(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sw	a0, -1328(s0)
	addi	t1, s0, -76
	lw	a0, -1328(s0)
	sw	a0, 0(t1)
	li	a0, 7
	sw	a0, -1336(s0)
	li	a0, 16
	sw	a0, -1344(s0)
	lw	a0, -600(s0)
	slliw	t2, a0, 3
	subw	a0, t2, a0
	sw	a0, -1360(s0)
	lw	a0, -1360(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 15
	addw	a0, a0, t1
	sraiw	a0, a0, 4
	li	t1, 16
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -1368(s0)
	addi	t1, s0, -80
	lw	a0, -1368(s0)
	sw	a0, 0(t1)
.L_ifend_pseudo_md5_28:
.L_ifend_pseudo_md5_26:
.L_ifend_pseudo_md5_24:
	lw	a0, -60(s0)
	lw	a0, -76(s0)
	lw	t2, -60(s0)
	addw	a0, t2, a0
	sw	a0, -1392(s0)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1408(s0)
	lw	a0, -1408(s0)
	addiw	a0, a0, 0
	sw	a0, -1416(s0)
	ld	t2, -736(s0)
	lw	a0, -1416(s0)
	add	a0, t2, a0
	sd	a0, -1424(s0)
	ld	t2, -1424(s0)
	lw	a0, 0(t2)
	sw	a0, -1432(s0)
	lw	t2, -1392(s0)
	lw	a0, -1432(s0)
	addw	a0, t2, a0
	sw	a0, -1440(s0)
	lw	a0, -80(s0)
	slliw	a0, a0, 2
	sw	a0, -1456(s0)
	lw	a0, -1456(s0)
	addiw	a0, a0, 0
	sw	a0, -1464(s0)
	ld	t2, -744(s0)
	lw	a0, -1464(s0)
	add	a0, t2, a0
	sd	a0, -1472(s0)
	ld	t2, -1472(s0)
	lw	a0, 0(t2)
	sw	a0, -1480(s0)
	lw	t2, -1440(s0)
	lw	a0, -1480(s0)
	addw	a0, t2, a0
	sw	a0, -1488(s0)
	addi	t1, s0, -84
	lw	a0, -1488(s0)
	sw	a0, 0(t1)
	lw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -1504(s0)
	lw	a0, -1504(s0)
	addiw	a0, a0, 0
	sw	a0, -1512(s0)
	ld	t2, -752(s0)
	lw	a0, -1512(s0)
	add	a0, t2, a0
	sd	a0, -1520(s0)
	ld	t2, -1520(s0)
	lw	a0, 0(t2)
	sw	a0, -1528(s0)
	addi	t1, s0, -676
	lw	a0, -1528(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -680
	lw	a0, -720(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -684
	lw	a0, -680(s0)
	sw	a0, 0(t1)
.L_wh_pseudo_md5_30:
	lw	a0, -676(s0)
	lw	a0, -684(s0)
	lw	t2, -684(s0)
	sext.w	t2, t2
	lw	a0, -676(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1552(s0)
	lw	a0, -1552(s0)
	beqz	a0, .L_whe_pseudo_md5_31
	lw	a0, -84(s0)
	lw	a0, -1560(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	rotl1
	sw	a0, -1568(s0)
	addi	t1, s0, -84
	lw	a0, -1568(s0)
	sw	a0, 0(t1)
	lw	a0, -684(s0)
	addiw	a0, a0, 1
	sw	a0, -1584(s0)
	addi	t1, s0, -684
	lw	a0, -1584(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_30
.L_whe_pseudo_md5_31:
	lw	a0, -84(s0)
	lw	a0, -64(s0)
	lw	t2, -84(s0)
	addw	a0, t2, a0
	sw	a0, -1608(s0)
	addi	t1, s0, -84
	lw	a0, -1608(s0)
	sw	a0, 0(t1)
	lw	a0, -72(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	lw	a0, -68(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	lw	a0, -64(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	addi	t1, s0, -64
	lw	a0, -84(s0)
	sw	a0, 0(t1)
	lw	a0, -600(s0)
	addiw	a0, a0, 1
	sw	a0, -1648(s0)
	addi	t1, s0, -600
	lw	a0, -1648(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_22
.L_whe_pseudo_md5_23:
	lw	a0, -44(s0)
	lw	a0, -60(s0)
	lw	t2, -44(s0)
	addw	a0, t2, a0
	sw	a0, -720(s0)
	addi	t1, s0, -44
	lw	a0, -720(s0)
	sw	a0, 0(t1)
	lw	a0, -48(s0)
	lw	a0, -64(s0)
	lw	t2, -48(s0)
	addw	a0, t2, a0
	sw	a0, -752(s0)
	addi	t1, s0, -48
	lw	a0, -752(s0)
	sw	a0, 0(t1)
	lw	a0, -52(s0)
	lw	a0, -68(s0)
	lw	t2, -52(s0)
	addw	a0, t2, a0
	sw	a0, -1672(s0)
	addi	t1, s0, -52
	lw	a0, -1672(s0)
	sw	a0, 0(t1)
	lw	a0, -56(s0)
	lw	a0, -72(s0)
	lw	t2, -56(s0)
	addw	a0, t2, a0
	sw	a0, -1696(s0)
	addi	t1, s0, -56
	lw	a0, -1696(s0)
	sw	a0, 0(t1)
	lw	a0, -608(s0)
	addiw	a0, a0, 64
	sw	a0, -1712(s0)
	addi	t1, s0, -608
	lw	a0, -1712(s0)
	sw	a0, 0(t1)
	j	.L_wh_pseudo_md5_18
.L_whe_pseudo_md5_19:
	ld	a0, -40(s0)
	sd	a0, -696(s0)
	lw	a0, -44(s0)
	ld	t1, -696(s0)
	sw	a0, 0(t1)
	li	a0, 4
	sw	a0, -712(s0)
	ld	a0, -40(s0)
	sd	a0, -720(s0)
	ld	a0, -720(s0)
	addi	a0, a0, 4
	sd	a0, -728(s0)
	lw	a0, -48(s0)
	ld	t1, -728(s0)
	sw	a0, 0(t1)
	li	a0, 8
	sw	a0, -744(s0)
	ld	a0, -40(s0)
	sd	a0, -752(s0)
	ld	a0, -752(s0)
	addi	a0, a0, 8
	sd	a0, -760(s0)
	lw	a0, -52(s0)
	ld	t1, -760(s0)
	sw	a0, 0(t1)
	li	a0, 12
	sw	a0, -776(s0)
	ld	a0, -40(s0)
	sd	a0, -784(s0)
	ld	a0, -784(s0)
	addi	a0, a0, 12
	sd	a0, -792(s0)
	lw	a0, -56(s0)
	ld	t1, -792(s0)
	sw	a0, 0(t1)
	j	.Lreturn_pseudo_md5_8
.Lreturn_pseudo_md5_8:
	ld	s11, -1800(s0)
	ld	s10, -1792(s0)
	ld	s9, -1784(s0)
	ld	s8, -1776(s0)
	ld	s7, -1768(s0)
	ld	s6, -1760(s0)
	ld	s5, -1752(s0)
	ld	s4, -1744(s0)
	ld	s3, -1736(s0)
	ld	s2, -1728(s0)
	ld	s1, -1720(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	pseudo_md5, .-pseudo_md5
	.text
	.align	1
	.globl	pseudo_sha1
	.type	pseudo_sha1, @function
pseudo_sha1:
	addi	sp, sp, -432
	addi	t0, sp, 432
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	sw	a1, -28(s0)
	sd	a2, -40(s0)
	li	a0, 1732584193
	sw	a0, -44(s0)
	li	a0, -271733879
	sw	a0, -48(s0)
	li	a0, -1732584194
	sw	a0, -52(s0)
	li	a0, 271733878
	sw	a0, -56(s0)
	li	a0, -1009589776
	sw	a0, -60(s0)
	sw	zero, -64(s0)
	sw	zero, -68(s0)
	sw	zero, -72(s0)
	sw	zero, -76(s0)
	sw	zero, -80(s0)
	sw	zero, -84(s0)
	sw	zero, -88(s0)
	lw	a0, -28(s0)
	sw	a0, -92(s0)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -28(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 128
	sw	a0, 0(t4)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -28(s0)
.Lwhile_cond_10:
	lw	a0, -28(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 63
	addw	a0, a0, t1
	sraiw	a0, a0, 6
	li	t1, 64
	mulw	t1, a0, t1
	subw	a0, t6, t1
	li	t1, 60
	beq	a0, t1, .Lwhile_end_12
.Lwhile_body_11:
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -28(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 0
	sw	a0, 0(t4)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -28(s0)
	j	.Lwhile_cond_10
.Lwhile_end_12:
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -28(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	lw	a0, -92(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	li	t2, 16777215
	and	t1, t1, t2
	addw	a0, a0, t1
	sraiw	a0, a0, 24
	mv	t4, a0
	li	a0, 256
	mv	a0, t4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	lw	a0, -92(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	li	t2, 65535
	and	t1, t1, t2
	addw	a0, a0, t1
	sraiw	a0, a0, 16
	mv	t4, a0
	li	a0, 256
	mv	a0, t4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -28(s0)
	addiw	a0, a0, 2
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	lw	a0, -92(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	mv	t4, a0
	li	a0, 256
	mv	a0, t4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -28(s0)
	addiw	a0, a0, 3
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	lw	a0, -92(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -28(s0)
	addiw	a0, a0, 4
	sw	a0, -28(s0)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t3, s0, -416
	li	t1, 320
.Lzero_array_13:
	beqz	t1, .Lzero_array_end_14
	sw	zero, 0(t3)
	addi	t3, t3, 4
	addi	t1, t1, -4
	j	.Lzero_array_13
.Lzero_array_end_14:
	li	a0, 0
	mv	t4, a0
	addi	a1, s0, -416
	sw	t4, 0(a1)
.Lwhile_cond_15:
	lw	a0, -96(s0)
	mv	t4, a0
	lw	a0, -28(s0)
	bge	t4, a0, .Lwhile_end_17
.Lwhile_body_16:
	lw	a0, -44(s0)
	sw	a0, -64(s0)
	lw	a0, -48(s0)
	sw	a0, -68(s0)
	lw	a0, -52(s0)
	sw	a0, -72(s0)
	lw	a0, -56(s0)
	sw	a0, -76(s0)
	lw	a0, -60(s0)
	sw	a0, -80(s0)
	li	a0, 0
	sw	a0, -420(s0)
.Lwhile_cond_18:
	lw	a0, -420(s0)
	li	t1, 16
	bge	a0, t1, .Lwhile_end_20
.Lwhile_body_19:
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	a0, a0, 2
	mv	t4, a0
	lw	a0, -96(s0)
	addw	a0, a0, t4
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 16777216
	mv	a0, t4
	slliw	a0, a0, 24
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	a0, a0, 2
	mv	t4, a0
	lw	a0, -96(s0)
	addw	a0, a0, t4
	mv	t4, a0
	li	a0, 1
	addiw	a0, t4, 1
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 65536
	mv	a0, t4
	slliw	a0, a0, 16
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	a0, a0, 2
	mv	t4, a0
	lw	a0, -96(s0)
	addw	a0, a0, t4
	mv	t4, a0
	li	a0, 2
	addiw	a0, t4, 2
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 256
	mv	a0, t4
	slliw	a0, a0, 8
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a0, -24(s0)
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	a0, a0, 2
	mv	t4, a0
	lw	a0, -96(s0)
	addw	a0, a0, t4
	mv	t4, a0
	li	a0, 3
	addiw	a0, t4, 3
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 1
	mv	a0, t4
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -420(s0)
	addiw	a0, a0, 1
	sw	a0, -420(s0)
	j	.Lwhile_cond_18
.Lwhile_end_20:
.Lwhile_cond_21:
	lw	a0, -420(s0)
	li	t1, 80
	bge	a0, t1, .Lwhile_end_23
.Lwhile_body_22:
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	sp, sp, -16
	addi	sp, sp, -16
	addi	sp, sp, -16
	addi	sp, sp, -16
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	addiw	a0, a0, -3
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	addiw	a0, a0, -8
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sd	a0, 0(sp)
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	addiw	a0, a0, -14
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sd	a0, 0(sp)
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	addiw	a0, a0, -16
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	rotl1
	addi	sp, sp, 16
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -420(s0)
	addiw	a0, a0, 1
	sw	a0, -420(s0)
	j	.Lwhile_cond_21
.Lwhile_end_23:
	li	a0, 0
	sw	a0, -420(s0)
.Lwhile_cond_24:
	lw	a0, -420(s0)
	li	t1, 80
	bge	a0, t1, .Lwhile_end_26
.Lwhile_body_25:
	lw	a0, -420(s0)
	li	t1, 20
	blt	a0, t1, .Lif_then_27
	j	.Lif_else_28
.Lif_then_27:
	addi	sp, sp, -16
	lw	a0, -68(s0)
	mv	t0, a0
	lw	a0, -72(s0)
	addw	a0, t0, a0
	sd	a0, 0(sp)
	addi	sp, sp, -16
	li	a0, -1
	mv	t0, a0
	lw	a0, -68(s0)
	subw	a0, t0, a0
	sd	a0, 0(sp)
	lw	a0, -76(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_and
	addi	sp, sp, 16
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_or
	addi	sp, sp, 16
	sw	a0, -84(s0)
	li	a0, 1518500249
	sw	a0, -88(s0)
	j	.Lif_end_29
.Lif_else_28:
	lw	a0, -420(s0)
	li	t1, 40
	blt	a0, t1, .Lif_then_30
	j	.Lif_else_31
.Lif_then_30:
	addi	sp, sp, -16
	lw	a0, -72(s0)
	mv	t5, a0
	lw	a0, -68(s0)
	mv	a1, t5
	call	_xor
	sd	a0, 0(sp)
	lw	a0, -76(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sw	a0, -84(s0)
	li	a0, 1859775361
	sw	a0, -88(s0)
	j	.Lif_end_32
.Lif_else_31:
	lw	a0, -420(s0)
	li	t1, 60
	blt	a0, t1, .Lif_then_33
	j	.Lif_else_34
.Lif_then_33:
	addi	sp, sp, -16
	addi	sp, sp, -16
	lw	a0, -68(s0)
	mv	t0, a0
	lw	a0, -72(s0)
	addw	a0, t0, a0
	sd	a0, 0(sp)
	lw	a0, -68(s0)
	mv	t0, a0
	lw	a0, -76(s0)
	addw	a0, t0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_or
	addi	sp, sp, 16
	sd	a0, 0(sp)
	lw	a0, -72(s0)
	mv	t0, a0
	lw	a0, -76(s0)
	addw	a0, t0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_or
	addi	sp, sp, 16
	sw	a0, -84(s0)
	li	a0, -1894007588
	sw	a0, -88(s0)
	j	.Lif_end_35
.Lif_else_34:
	lw	a0, -420(s0)
	li	t1, 80
	bge	a0, t1, .Lif_end_38
	addi	sp, sp, -16
	lw	a0, -72(s0)
	mv	t5, a0
	lw	a0, -68(s0)
	mv	a1, t5
	call	_xor
	sd	a0, 0(sp)
	lw	a0, -76(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	sw	a0, -84(s0)
	li	a0, -899497722
	sw	a0, -88(s0)
.Lif_end_38:
.Lif_end_35:
.Lif_end_32:
.Lif_end_29:
	lw	a0, -64(s0)
	slliw	a0, a0, 5
	addi	sp, sp, -16
	sd	a0, 0(sp)
	lw	a0, -64(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 31
	addw	a0, a0, t1
	sraiw	a0, a0, 5
	li	t1, 32
	mulw	t1, a0, t1
	subw	a0, t6, t1
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	mv	t4, a0
	lw	a0, -84(s0)
	addw	a0, t4, a0
	mv	t4, a0
	lw	a0, -80(s0)
	addw	a0, t4, a0
	mv	t4, a0
	lw	a0, -88(s0)
	addw	a0, t4, a0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -416
	mv	t5, a0
	lw	a0, -420(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	sw	a0, -424(s0)
	lw	a0, -76(s0)
	sw	a0, -80(s0)
	lw	a0, -72(s0)
	sw	a0, -76(s0)
	lw	a0, -68(s0)
	slliw	a0, a0, 30
	addi	sp, sp, -16
	sd	a0, 0(sp)
	lw	a0, -68(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	li	t2, 1073741823
	and	t1, t1, t2
	addw	a0, a0, t1
	sraiw	a0, a0, 30
	li	t1, 1073741824
	mulw	t1, a0, t1
	subw	a0, t6, t1
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	sw	a0, -68(s0)
	lw	a0, -424(s0)
	sw	a0, -64(s0)
	lw	a0, -420(s0)
	addiw	a0, a0, 1
	sw	a0, -420(s0)
	j	.Lwhile_cond_24
.Lwhile_end_26:
	lw	a0, -44(s0)
	mv	t0, a0
	lw	a0, -64(s0)
	addw	a0, t0, a0
	sw	a0, -44(s0)
	lw	a0, -48(s0)
	mv	t0, a0
	lw	a0, -68(s0)
	addw	a0, t0, a0
	sw	a0, -48(s0)
	lw	a0, -52(s0)
	mv	t0, a0
	lw	a0, -72(s0)
	addw	a0, t0, a0
	sw	a0, -52(s0)
	lw	a0, -56(s0)
	mv	t0, a0
	lw	a0, -76(s0)
	addw	a0, t0, a0
	sw	a0, -56(s0)
	lw	a0, -60(s0)
	mv	t0, a0
	lw	a0, -80(s0)
	addw	a0, t0, a0
	sw	a0, -60(s0)
	lw	a0, -96(s0)
	addiw	a0, a0, 64
	sw	a0, -96(s0)
	j	.Lwhile_cond_15
.Lwhile_end_17:
	ld	a0, -40(s0)
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -44(s0)
	sw	a0, 0(t4)
	ld	a0, -40(s0)
	mv	t5, a0
	li	a0, 1
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -48(s0)
	sw	a0, 0(t4)
	ld	a0, -40(s0)
	mv	t5, a0
	li	a0, 2
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -52(s0)
	sw	a0, 0(t4)
	ld	a0, -40(s0)
	mv	t5, a0
	li	a0, 3
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -56(s0)
	sw	a0, 0(t4)
	ld	a0, -40(s0)
	mv	t5, a0
	li	a0, 4
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -60(s0)
	sw	a0, 0(t4)
.Lreturn_pseudo_sha1_9:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	pseudo_sha1, .-pseudo_sha1
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -80
	addi	t0, sp, 80
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	li	a0, 12
	sw	a0, -20(s0)
	li	a0, 0
	sw	a0, -24(s0)
	li	a0, 0
	mv	t4, a0
	addi	a1, s0, -44
	sw	t4, 0(a1)
	li	a0, 0
	mv	t4, a0
	addi	a1, s0, -40
	sw	t4, 0(a1)
	li	a0, 0
	mv	t4, a0
	addi	a1, s0, -36
	sw	t4, 0(a1)
	li	a0, 0
	mv	t4, a0
	addi	a1, s0, -32
	sw	t4, 0(a1)
	li	a0, 0
	mv	t4, a0
	addi	a1, s0, -28
	sw	t4, 0(a1)
	addi	t3, s0, -64
	li	t1, 20
.Lzero_array_40:
	beqz	t1, .Lzero_array_end_41
	sw	zero, 0(t3)
	addi	t3, t3, 4
	addi	t1, t1, -4
	j	.Lzero_array_40
.Lzero_array_end_41:
	call	getint
	lla	t4, state
	sw	a0, 0(t4)
	call	getint
	sw	a0, -20(s0)
	addi	sp, sp, -16
	li	a0, 261
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	addi	a0, s0, -64
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 0
	sw	a0, 0(t4)
	addi	a0, s0, -64
	mv	t5, a0
	li	a0, 1
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 0
	sw	a0, 0(t4)
	addi	a0, s0, -64
	mv	t5, a0
	li	a0, 2
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 0
	sw	a0, 0(t4)
	addi	a0, s0, -64
	mv	t5, a0
	li	a0, 3
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 0
	sw	a0, 0(t4)
	addi	a0, s0, -64
	mv	t5, a0
	li	a0, 4
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	li	a0, 0
	sw	a0, 0(t4)
.Lwhile_cond_42:
	lw	a0, -20(s0)
	bge	zero, a0, .Lwhile_end_44
.Lwhile_body_43:
	li	a0, 32000
	sw	a0, -68(s0)
	li	a0, 0
	sw	a0, -72(s0)
.Lwhile_cond_45:
	lw	a0, -72(s0)
	mv	t4, a0
	lw	a0, -68(s0)
	bge	t4, a0, .Lwhile_end_47
.Lwhile_body_46:
	lla	a0, buffer
	mv	t5, a0
	lw	a0, -72(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	call	get_random
	mv	t4, a0
	li	a0, 256
	mv	a0, t4
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	j	.Lwhile_cond_45
.Lwhile_end_47:
	lla	a0, buffer
	mv	t4, a0
	lw	a0, -68(s0)
	mv	t5, a0
	addi	a0, s0, -44
	mv	t6, a0
	mv	a0, t4
	mv	a1, t5
	mv	a2, t6
	call	pseudo_md5
	li	a0, 0
	sw	a0, -72(s0)
.Lwhile_cond_48:
	lw	a0, -72(s0)
	li	t1, 5
	bge	a0, t1, .Lwhile_end_50
.Lwhile_body_49:
	addi	a0, s0, -64
	mv	t5, a0
	lw	a0, -72(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	sp, sp, -16
	addi	a0, s0, -64
	mv	t5, a0
	lw	a0, -72(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	addi	a0, s0, -44
	mv	t5, a0
	lw	a0, -72(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	_xor
	addi	sp, sp, 16
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	j	.Lwhile_cond_48
.Lwhile_end_50:
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -20(s0)
	j	.Lwhile_cond_42
.Lwhile_end_44:
	addi	sp, sp, -16
	li	a0, 284
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	addi	a0, s0, -64
	mv	t5, a0
	li	a0, 5
	mv	a1, t5
	call	putarray
	li	a0, 0
	j	.Lreturn_main_39
	li	a0, 0
.Lreturn_main_39:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
