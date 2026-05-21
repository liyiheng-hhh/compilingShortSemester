	.bss
	.align	2
	.globl	data
data:
	.zero	400000
	.bss
	.align	2
	.globl	size
size:
	.zero	4
	.bss
	.align	2
	.globl	out
out:
	.zero	400000
	.bss
	.align	2
	.globl	out_num
out_num:
	.zero	4
	.bss
	.align	2
	.globl	pos
pos:
	.zero	4
	.bss
	.align	2
	.globl	buf
buf:
	.zero	4
	.bss
	.align	2
	.globl	bits
bits:
	.zero	4
	.text
	.text
	.align	1
	.globl	_and
	.type	_and, @function
_and:
	addi	sp, sp, -288
	addi	t0, sp, 288
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -240(s0)
	sd	s2, -248(s0)
	sd	s3, -256(s0)
	sd	s4, -264(s0)
	sd	s5, -272(s0)
	sd	s6, -280(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh__and_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	beqz	a0, .L_whe__and_1
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
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
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -20
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	mv	s1, a0
	sw	s1, -136(s0)
	addi	t1, s0, -24
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_ifend__and_2
	li	a0, 1
	sw	a0, -160(s0)
	lw	a0, -32(s0)
	addiw	a0, a0, -1
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_ifend__and_3
	lw	a0, -40(s0)
	lw	a0, -44(s0)
	lw	t2, -40(s0)
	addw	a0, t2, a0
	sw	a0, -200(s0)
	addi	t1, s0, -40
	lw	a0, -200(s0)
	sw	a0, 0(t1)
.L_ifend__and_3:
.L_ifend__and_2:
	lw	a0, -44(s0)
	mv	s6, a0
	sw	s6, -208(s0)
	mv	a0, s6
	slliw	a0, a0, 1
	mv	s5, a0
	sw	s5, -216(s0)
	addi	t1, s0, -44
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -36(s0)
	mv	s4, a0
	sw	s4, -224(s0)
	mv	a0, s4
	addiw	a0, a0, -1
	mv	s3, a0
	sw	s3, -232(s0)
	addi	t1, s0, -36
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh__and_0
.L_whe__and_1:
	lw	a0, -40(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn__and_0
	li	a0, 0
.Lreturn__and_0:
	ld	s6, -280(s0)
	ld	s5, -272(s0)
	ld	s4, -264(s0)
	ld	s3, -256(s0)
	ld	s2, -248(s0)
	ld	s1, -240(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_and, .-_and
	.text
	.align	1
	.globl	_xor
	.type	_xor, @function
_xor:
	addi	sp, sp, -272
	addi	t0, sp, 272
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -216(s0)
	sd	s2, -224(s0)
	sd	s3, -232(s0)
	sd	s4, -240(s0)
	sd	s5, -248(s0)
	sd	s6, -256(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh__xor_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	beqz	a0, .L_whe__xor_1
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
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
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -20
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	mv	s1, a0
	sw	s1, -128(s0)
	mv	a0, s1
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -24
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	lw	t2, -28(s0)
	lw	a0, -32(s0)
	subw	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_ifend__xor_2
	lw	a0, -40(s0)
	lw	a0, -44(s0)
	lw	t2, -40(s0)
	addw	a0, t2, a0
	sw	a0, -176(s0)
	addi	t1, s0, -40
	lw	a0, -176(s0)
	sw	a0, 0(t1)
.L_ifend__xor_2:
	lw	a0, -44(s0)
	mv	s6, a0
	sw	s6, -184(s0)
	mv	a0, s6
	slliw	a0, a0, 1
	mv	s5, a0
	sw	s5, -192(s0)
	addi	t1, s0, -44
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -36(s0)
	mv	s4, a0
	sw	s4, -200(s0)
	mv	a0, s4
	addiw	a0, a0, -1
	mv	s3, a0
	sw	s3, -208(s0)
	addi	t1, s0, -36
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh__xor_0
.L_whe__xor_1:
	lw	a0, -40(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn__xor_1
	li	a0, 0
.Lreturn__xor_1:
	ld	s6, -256(s0)
	ld	s5, -248(s0)
	ld	s4, -240(s0)
	ld	s3, -232(s0)
	ld	s2, -224(s0)
	ld	s1, -216(s0)
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
	addi	sp, sp, -320
	addi	t0, sp, 320
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -272(s0)
	sd	s2, -280(s0)
	sd	s3, -288(s0)
	sd	s4, -296(s0)
	sd	s5, -304(s0)
	sd	s6, -312(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh__or_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	beqz	a0, .L_whe__or_1
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
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
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -20
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -24
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	seqz	a0, a0
	mv	s2, a0
	sw	s2, -152(s0)
	mv	a0, s2
	beqz	a0, .L_ifelse__or_3
	lw	a0, -40(s0)
	mv	s3, a0
	sw	s3, -160(s0)
	lw	a0, -44(s0)
	mv	s4, a0
	sw	s4, -168(s0)
	mv	t2, s3
	mv	a0, s4
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -176(s0)
	addi	t1, s0, -40
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	j	.L_ifend__or_2
.L_ifelse__or_3:
	li	a0, 1
	sw	a0, -184(s0)
	lw	a0, -32(s0)
	mv	s6, a0
	sw	s6, -192(s0)
	mv	a0, s6
	addiw	a0, a0, -1
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	beqz	a0, .L_ifend__or_4
	lw	a0, -40(s0)
	mv	s1, a0
	sw	s1, -216(s0)
	lw	a0, -44(s0)
	mv	t2, s1
	addw	a0, t2, a0
	sw	a0, -232(s0)
	addi	t1, s0, -40
	lw	a0, -232(s0)
	sw	a0, 0(t1)
.L_ifend__or_4:
.L_ifend__or_2:
	lw	a0, -44(s0)
	slliw	a0, a0, 1
	sw	a0, -248(s0)
	addi	t1, s0, -44
	lw	a0, -248(s0)
	sw	a0, 0(t1)
	lw	a0, -36(s0)
	addiw	a0, a0, -1
	sw	a0, -264(s0)
	addi	t1, s0, -36
	lw	a0, -264(s0)
	sw	a0, 0(t1)
	j	.L_wh__or_0
.L_whe__or_1:
	lw	a0, -40(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn__or_2
	li	a0, 0
.Lreturn__or_2:
	ld	s6, -312(s0)
	ld	s5, -304(s0)
	ld	s4, -296(s0)
	ld	s3, -288(s0)
	ld	s2, -280(s0)
	ld	s1, -272(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_or, .-_or
	.text
	.align	1
	.globl	rotrN
	.type	rotrN, @function
rotrN:
	addi	sp, sp, -128
	addi	t0, sp, 128
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -72(s0)
	sd	s2, -80(s0)
	sd	s3, -88(s0)
	sd	s4, -96(s0)
	sd	s5, -104(s0)
	sd	s6, -112(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_0
	li	a0, 2
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_0:
	li	a0, 2
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -2
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_1
	li	a0, 4
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 3
	addw	a0, a0, t1
	sraiw	a0, a0, 2
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_1:
	li	a0, 3
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -3
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_2
	li	a0, 8
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 7
	addw	a0, a0, t1
	sraiw	a0, a0, 3
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_2:
	li	a0, 4
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -4
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_3
	li	a0, 16
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 15
	addw	a0, a0, t1
	sraiw	a0, a0, 4
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_3:
	li	a0, 5
	mv	s2, a0
	sw	s2, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -5
	mv	s3, a0
	sw	s3, -56(s0)
	mv	a0, s3
	sext.w	a0, a0
	seqz	a0, a0
	mv	s4, a0
	sw	s4, -64(s0)
	mv	a0, s4
	beqz	a0, .L_ifend_rotrN_4
	li	a0, 32
	mv	s6, a0
	sw	s6, -40(s0)
	lw	a0, -20(s0)
	mv	s5, a0
	sw	s5, -48(s0)
	mv	a0, s5
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 31
	addw	a0, a0, t1
	sraiw	a0, a0, 5
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_4:
	li	a0, 6
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -6
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_5
	li	a0, 64
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 63
	addw	a0, a0, t1
	sraiw	a0, a0, 6
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_5:
	li	a0, 7
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -7
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_6
	li	a0, 128
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 127
	addw	a0, a0, t1
	sraiw	a0, a0, 7
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_6:
	li	a0, 8
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -8
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_7
	li	a0, 256
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
.L_ifend_rotrN_7:
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
	li	a0, 0
.Lreturn_rotrN_3:
	ld	s6, -112(s0)
	ld	s5, -104(s0)
	ld	s4, -96(s0)
	ld	s3, -88(s0)
	ld	s2, -80(s0)
	ld	s1, -72(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rotrN, .-rotrN
	.text
	.align	1
	.globl	rotlN
	.type	rotlN, @function
rotlN:
	addi	sp, sp, -128
	addi	t0, sp, 128
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -72(s0)
	sd	s2, -80(s0)
	sd	s3, -88(s0)
	sd	s4, -96(s0)
	sd	s5, -104(s0)
	sd	s6, -112(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_0
	lw	a0, -20(s0)
	slliw	a0, a0, 1
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_0:
	li	a0, 2
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -2
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_1
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_1:
	li	a0, 3
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -3
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_2
	lw	a0, -20(s0)
	slliw	a0, a0, 3
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_2:
	li	a0, 4
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -4
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_3
	lw	a0, -20(s0)
	slliw	a0, a0, 4
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_3:
	li	a0, 5
	mv	s2, a0
	sw	s2, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -5
	mv	s3, a0
	sw	s3, -56(s0)
	mv	a0, s3
	sext.w	a0, a0
	seqz	a0, a0
	mv	s4, a0
	sw	s4, -64(s0)
	mv	a0, s4
	beqz	a0, .L_ifend_rotlN_4
	lw	a0, -20(s0)
	mv	s5, a0
	sw	s5, -40(s0)
	mv	a0, s5
	slliw	a0, a0, 5
	mv	s6, a0
	sw	s6, -48(s0)
	mv	a0, s6
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_4:
	li	a0, 6
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -6
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_5
	lw	a0, -20(s0)
	slliw	a0, a0, 6
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_5:
	li	a0, 7
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -7
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_6
	lw	a0, -20(s0)
	slliw	a0, a0, 7
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_6:
	li	a0, 8
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -8
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_7
	lw	a0, -20(s0)
	slliw	a0, a0, 8
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_7:
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
	li	a0, 0
.Lreturn_rotlN_4:
	ld	s6, -112(s0)
	ld	s5, -104(s0)
	ld	s4, -96(s0)
	ld	s3, -88(s0)
	ld	s2, -80(s0)
	ld	s1, -72(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rotlN, .-rotlN
	.text
	.align	1
	.globl	read_bits
	.type	read_bits, @function
read_bits:
	addi	sp, sp, -320
	addi	t0, sp, 320
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -232(s0)
	sd	s2, -240(s0)
	sd	s3, -248(s0)
	sd	s4, -256(s0)
	sd	s5, -264(s0)
	sd	s6, -272(s0)
	sd	s7, -280(s0)
	sd	s8, -288(s0)
	sd	s9, -296(s0)
	sd	s10, -304(s0)
	sd	s11, -312(s0)
	sw	a0, -20(s0)
.L_wh_read_bits_0:
	li	a0, 1
	sw	a0, -40(s0)
	li	a0, 0
	sw	a0, -48(s0)
	li	a0, 8
	mv	s3, a0
	sw	s3, -56(s0)
	lla	a0, data
	sd	a0, -64(s0)
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	beqz	a0, .L_whe_read_bits_1
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	t2, -80(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_ifend_read_bits_2
	j	.L_whe_read_bits_1
.L_ifend_read_bits_2:
	lla	t0, pos
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lla	t0, size
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_ifend_read_bits_3
	j	.L_whe_read_bits_1
.L_ifend_read_bits_3:
	lla	t0, buf
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	lla	t0, pos
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	slliw	a0, a0, 2
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	addiw	a0, a0, 0
	sw	a0, -152(s0)
	ld	t2, -64(s0)
	lw	a0, -152(s0)
	add	a0, t2, a0
	sd	a0, -160(s0)
	ld	t2, -160(s0)
	lw	a0, 0(t2)
	sw	a0, -168(s0)
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	lw	a0, -168(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -176(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	mv	t2, a0
	li	t1, 8
	bgt	t2, t1, .Lir_rot_id_6
	sllw	a0, t4, t2
	j	.Lir_rot_done_7
.Lir_rot_id_6:
	mv	a0, t4
.Lir_rot_done_7:
	sw	a0, -184(s0)
	lw	a0, -128(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -184(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	or	a0, t4, a0
	sext.w	a0, a0
	mv	s11, a0
	sw	s11, -192(s0)
	lla	t1, buf
	mv	a0, s11
	sw	a0, 0(t1)
	lla	t0, bits
	lw	a0, 0(t0)
	mv	s2, a0
	sw	s2, -200(s0)
	mv	a0, s2
	addiw	a0, a0, 8
	mv	s4, a0
	sw	s4, -208(s0)
	lla	t1, bits
	mv	a0, s4
	sw	a0, 0(t1)
	lla	t0, pos
	lw	a0, 0(t0)
	mv	s5, a0
	sw	s5, -216(s0)
	mv	a0, s5
	addiw	a0, a0, 1
	mv	s6, a0
	sw	s6, -224(s0)
	lla	t1, pos
	mv	a0, s6
	sw	a0, 0(t1)
	j	.L_wh_read_bits_0
.L_whe_read_bits_1:
	li	a0, 1
	mv	s8, a0
	sw	s8, -40(s0)
	lla	t0, buf
	lw	a0, 0(t0)
	mv	s7, a0
	sw	s7, -48(s0)
	lw	a0, -20(s0)
	mv	s9, a0
	sw	s9, -56(s0)
	li	a0, 1
	mv	t4, a0
	mv	a0, s9
	sext.w	a0, a0
	sext.w	a0, a0
	mv	t2, a0
	li	t1, 8
	bgt	t2, t1, .Lir_rot_id_8
	sllw	a0, t4, t2
	j	.Lir_rot_done_9
.Lir_rot_id_8:
	mv	a0, t4
.Lir_rot_done_9:
	mv	s10, a0
	sw	s10, -64(s0)
	li	a0, 1
	mv	s1, a0
	sw	s1, -72(s0)
	mv	a0, s10
	addiw	a0, a0, -1
	sw	a0, -80(s0)
	mv	a0, s7
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -80(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	and	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -88(s0)
	addi	t1, s0, -24
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lla	t0, buf
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	a0, -20(s0)
	lw	a0, -96(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -104(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	mv	t2, a0
	li	t1, 8
	bgt	t2, t1, .Lir_rot_id_10
	srlw	a0, t4, t2
	j	.Lir_rot_done_11
.Lir_rot_id_10:
	mv	a0, t4
.Lir_rot_done_11:
	sw	a0, -112(s0)
	lla	t1, buf
	lw	a0, -112(s0)
	sw	a0, 0(t1)
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	lw	a0, -20(s0)
	lw	t2, -120(s0)
	subw	a0, t2, a0
	sw	a0, -136(s0)
	lla	t1, bits
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	j	.Lreturn_read_bits_5
	li	a0, 0
.Lreturn_read_bits_5:
	ld	s11, -312(s0)
	ld	s10, -304(s0)
	ld	s9, -296(s0)
	ld	s8, -288(s0)
	ld	s7, -280(s0)
	ld	s6, -272(s0)
	ld	s5, -264(s0)
	ld	s4, -256(s0)
	ld	s3, -248(s0)
	ld	s2, -240(s0)
	ld	s1, -232(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	read_bits, .-read_bits
	.text
	.align	1
	.globl	decode_fixed_huffman
	.type	decode_fixed_huffman, @function
decode_fixed_huffman:
	addi	sp, sp, -288
	addi	t0, sp, 288
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -200(s0)
	sd	s2, -208(s0)
	sd	s3, -216(s0)
	sd	s4, -224(s0)
	sd	s5, -232(s0)
	sd	s6, -240(s0)
	sd	s7, -248(s0)
	sd	s8, -256(s0)
	sd	s9, -264(s0)
	sd	s10, -272(s0)
	sd	s11, -280(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -20
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_decode_fixed_huffman_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_whe_decode_fixed_huffman_1
	li	a0, 1
	sext.w	a0, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -72(s0)
	j	.L_wh_decode_fixed_huffman_0
.L_whe_decode_fixed_huffman_1:
.L_wh_decode_fixed_huffman_2:
	li	a0, 1
	sw	a0, -40(s0)
	li	a0, 5
	sw	a0, -48(s0)
	li	a0, 64
	sw	a0, -56(s0)
	lw	a0, -40(s0)
	beqz	a0, .L_whe_decode_fixed_huffman_3
	li	a0, 5
	sext.w	a0, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	addiw	a0, a0, 64
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	lw	t2, -56(s0)
	sext.w	t2, t2
	lw	a0, -24(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_ifelse_decode_fixed_huffman_5
	li	a0, 144
	sw	a0, -48(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -24(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	mv	s1, a0
	sw	s1, -88(s0)
	mv	a0, s1
	beqz	a0, .L_ifelse_decode_fixed_huffman_7
	lla	a0, out
	sd	a0, -96(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -96(s0)
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	ld	t1, -120(s0)
	lw	a0, -24(s0)
	sw	a0, 0(t1)
	li	a0, 1
	mv	s10, a0
	sw	s10, -128(s0)
	lw	a0, -20(s0)
	mv	s11, a0
	sw	s11, -136(s0)
	mv	a0, s11
	addiw	a0, a0, 1
	mv	s9, a0
	sw	s9, -144(s0)
	addi	t1, s0, -20
	mv	a0, s9
	sw	a0, 0(t1)
	mv	s9, a0
	j	.L_wh_decode_fixed_huffman_2
	j	.L_ifend_decode_fixed_huffman_6
.L_ifelse_decode_fixed_huffman_7:
	lla	t0, pos
	lw	a0, 0(t0)
	mv	s8, a0
	sw	s8, -152(s0)
	lla	t0, size
	lw	a0, 0(t0)
	mv	s7, a0
	sw	s7, -160(s0)
	mv	t2, s8
	sext.w	t2, t2
	mv	a0, s7
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s6, a0
	sw	s6, -168(s0)
	mv	a0, s6
	beqz	a0, .L_ifelse_decode_fixed_huffman_9
	j	.L_wh_decode_fixed_huffman_2
	j	.L_ifend_decode_fixed_huffman_8
.L_ifelse_decode_fixed_huffman_9:
	j	.L_whe_decode_fixed_huffman_3
.L_ifend_decode_fixed_huffman_8:
.L_ifend_decode_fixed_huffman_6:
	j	.L_ifend_decode_fixed_huffman_4
.L_ifelse_decode_fixed_huffman_5:
	lla	t0, pos
	lw	a0, 0(t0)
	mv	s5, a0
	sw	s5, -176(s0)
	lla	t0, size
	lw	a0, 0(t0)
	mv	s4, a0
	sw	s4, -184(s0)
	mv	t2, s5
	sext.w	t2, t2
	mv	a0, s4
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s3, a0
	sw	s3, -192(s0)
	mv	a0, s3
	beqz	a0, .L_ifelse_decode_fixed_huffman_11
	j	.L_wh_decode_fixed_huffman_2
	j	.L_ifend_decode_fixed_huffman_10
.L_ifelse_decode_fixed_huffman_11:
	j	.L_whe_decode_fixed_huffman_3
.L_ifend_decode_fixed_huffman_10:
.L_ifend_decode_fixed_huffman_4:
	j	.L_wh_decode_fixed_huffman_2
.L_whe_decode_fixed_huffman_3:
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -40(s0)
	lla	t1, out_num
	mv	a0, s2
	sw	a0, 0(t1)
	j	.Lreturn_decode_fixed_huffman_12
.Lreturn_decode_fixed_huffman_12:
	ld	s11, -280(s0)
	ld	s10, -272(s0)
	ld	s9, -264(s0)
	ld	s8, -256(s0)
	ld	s7, -248(s0)
	ld	s6, -240(s0)
	ld	s5, -232(s0)
	ld	s4, -224(s0)
	ld	s3, -216(s0)
	ld	s2, -208(s0)
	ld	s1, -200(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	decode_fixed_huffman, .-decode_fixed_huffman
	.text
	.align	1
	.globl	output_data
	.type	output_data, @function
output_data:
	addi	sp, sp, -32
	addi	t0, sp, 32
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 1000
	bge	t1, a0, .Lif_then_14
	j	.Lif_else_15
.Lif_then_14:
	addi	sp, sp, -16
	lla	a0, out
	mv	t5, a0
	lw	a0, -20(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	putch
	addi	sp, sp, 16
	j	.Lif_end_16
.Lif_else_15:
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 1000
	blt	t1, a0, .Lif_then_17
	j	.Lif_else_18
.Lif_then_17:
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 10000
	bge	t1, a0, .Lif_then_20
	j	.Lif_else_21
.Lif_then_20:
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1717986919
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 1
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 5
	mulw	t1, a0, t1
	subw	a0, t6, t1
	bne	a0, zero, .Lif_end_25
	addi	sp, sp, -16
	lla	a0, out
	mv	t5, a0
	lw	a0, -20(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	putch
	addi	sp, sp, 16
.Lif_end_25:
	j	.Lif_end_22
.Lif_else_21:
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 10000
	blt	t1, a0, .Lland_mid_29
	j	.Lif_else_27
.Lland_mid_29:
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 100000
	bge	t1, a0, .Lif_then_26
	j	.Lif_else_27
.Lif_then_26:
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1374389535
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 4
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 50
	mulw	t1, a0, t1
	subw	a0, t6, t1
	bne	a0, zero, .Lif_end_32
	addi	sp, sp, -16
	lla	a0, out
	mv	t5, a0
	lw	a0, -20(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	putch
	addi	sp, sp, 16
.Lif_end_32:
	j	.Lif_end_28
.Lif_else_27:
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1374389535
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 5
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 100
	mulw	t1, a0, t1
	subw	a0, t6, t1
	beq	a0, zero, .Lif_then_33
	j	.Lif_else_34
.Lif_then_33:
	addi	sp, sp, -16
	lla	a0, out
	mv	t5, a0
	lw	a0, -20(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	putch
	addi	sp, sp, 16
	j	.Lif_end_35
.Lif_else_34:
	j	.Lreturn_output_data_13
.Lif_end_35:
.Lif_end_28:
.Lif_end_22:
	j	.Lif_end_19
.Lif_else_18:
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 10000
	blt	t1, a0, .Lland_mid_39
	j	.Lif_else_37
.Lland_mid_39:
	lla	a0, out_num
	lw	a0, 0(a0)
	li	t1, 100000
	bge	t1, a0, .Lif_then_36
	j	.Lif_else_37
.Lif_then_36:
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1374389535
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 4
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 50
	mulw	t1, a0, t1
	subw	a0, t6, t1
	bne	a0, zero, .Lif_end_42
	addi	sp, sp, -16
	lla	a0, out
	mv	t5, a0
	lw	a0, -20(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	putch
	addi	sp, sp, 16
.Lif_end_42:
	j	.Lif_end_38
.Lif_else_37:
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1374389535
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 5
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 100
	mulw	t1, a0, t1
	subw	a0, t6, t1
	beq	a0, zero, .Lif_then_43
	j	.Lif_else_44
.Lif_then_43:
	addi	sp, sp, -16
	lla	a0, out
	mv	t5, a0
	lw	a0, -20(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	putch
	addi	sp, sp, 16
	j	.Lif_end_45
.Lif_else_44:
	j	.Lreturn_output_data_13
.Lif_end_45:
.Lif_end_38:
.Lif_end_19:
.Lif_end_16:
.Lreturn_output_data_13:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	output_data, .-output_data
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -208
	addi	t0, sp, 208
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -112(s0)
	sd	s2, -120(s0)
	sd	s3, -128(s0)
	sd	s4, -136(s0)
	sd	s5, -144(s0)
	sd	s6, -152(s0)
	sd	s7, -160(s0)
	sd	s8, -168(s0)
	sd	s9, -176(s0)
	sd	s10, -184(s0)
	sd	s11, -192(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -20
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	lla	a0, data
	sd	a0, -64(s0)
	ld	a0, -64(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -72(s0)
	lla	t1, size
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 2000
	sw	a0, -80(s0)
	addi	t1, s0, -32
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 153
	sw	a0, -88(s0)
	addi	sp, sp, -16
	li	a0, 153
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
.L_wh_main_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	li	a0, 2
	sw	a0, -56(s0)
	lw	a0, -32(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	beqz	a0, .L_whe_main_1
	lla	t1, pos
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	lla	t1, bits
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sext.w	a0, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -80(s0)
	addi	t1, s0, -24
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 2
	sext.w	a0, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	call	decode_fixed_huffman
	lw	a0, -32(s0)
	addiw	a0, a0, -1
	mv	s1, a0
	sw	s1, -104(s0)
	addi	t1, s0, -32
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 162
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 162
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -24(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 32
	sw	a0, -56(s0)
	li	a0, 32
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	lw	a0, -28(s0)
	lw	a0, -64(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	mv	s11, a0
	sw	s11, -72(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
.L_wh_main_2:
	li	a0, 1
	mv	s10, a0
	sw	s10, -40(s0)
	mv	a0, s10
	beqz	a0, .L_whe_main_3
	lw	a0, -20(s0)
	mv	s9, a0
	sw	s9, -48(s0)
	lla	t0, out_num
	lw	a0, 0(t0)
	mv	s8, a0
	sw	s8, -56(s0)
	mv	t2, s9
	sext.w	t2, t2
	mv	a0, s8
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s7, a0
	sw	s7, -64(s0)
	mv	a0, s7
	beqz	a0, .L_ifelse_main_5
	lw	a0, -20(s0)
	mv	s6, a0
	sw	s6, -72(s0)
	mv	a0, s6
	sext.w	a0, a0
	mv	a0, t4
	call	output_data
	lw	a0, -20(s0)
	mv	s5, a0
	sw	s5, -80(s0)
	mv	a0, s5
	addiw	a0, a0, 1
	mv	s4, a0
	sw	s4, -88(s0)
	addi	t1, s0, -20
	mv	a0, s4
	sw	a0, 0(t1)
	mv	s4, a0
	j	.L_ifend_main_4
.L_ifelse_main_5:
	j	.L_whe_main_3
.L_ifend_main_4:
	j	.L_wh_main_2
.L_whe_main_3:
	li	a0, 10
	mv	s3, a0
	sw	s3, -40(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	mv	s2, a0
	sw	s2, -48(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_main_46
	li	a0, 0
.Lreturn_main_46:
	ld	s11, -192(s0)
	ld	s10, -184(s0)
	ld	s9, -176(s0)
	ld	s8, -168(s0)
	ld	s7, -160(s0)
	ld	s6, -152(s0)
	ld	s5, -144(s0)
	ld	s4, -136(s0)
	ld	s3, -128(s0)
	ld	s2, -120(s0)
	ld	s1, -112(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
