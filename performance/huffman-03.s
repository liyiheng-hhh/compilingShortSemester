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
	addi	sp, sp, -432
	li	t0, 432
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh__and_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	sw	a0, -80(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe__and_1
	mv	a0, t4
	sw	a0, -96(s0)
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
	addi	t1, s0, -28
	sw	a0, 0(t1)
	mv	a0, t5
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
	addi	t1, s0, -32
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -128(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	mv	a0, t5
	sw	a0, -144(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -152(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	mv	t5, a0
	lw	a0, -104(s0)
	addiw	a0, a0, -1
	sw	a0, -160(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	beqz	a0, .L_ifend__and_2
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	li	a0, 1
	sw	a0, -184(s0)
	lw	a0, -176(s0)
	addiw	a0, a0, -1
	sw	a0, -192(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_ifend__and_3
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	lw	t2, -208(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_ifend__and_3:
.L_ifend__and_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	a0, a0, 1
	sw	a0, -240(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addiw	a0, a0, -1
	sw	a0, -256(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh__and_0
.L_whe__and_1:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn__and_0
	li	a0, 0
.Lreturn__and_0:
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
	addi	sp, sp, -352
	li	t0, 352
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh__xor_0:
	li	a0, 2
	sw	a0, -56(s0)
	sw	a0, -64(s0)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe__xor_1
	mv	a0, t4
	sw	a0, -88(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -96(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	mv	a0, t5
	sw	a0, -104(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -112(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -120(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	mv	a0, t5
	sw	a0, -136(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	mv	t5, a0
	lw	t2, -96(s0)
	lw	a0, -112(s0)
	subw	a0, t2, a0
	sw	a0, -152(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_ifend__xor_2
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	lw	t2, -168(s0)
	addw	a0, t2, a0
	sw	a0, -184(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_ifend__xor_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	slliw	a0, a0, 1
	sw	a0, -200(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addiw	a0, a0, -1
	sw	a0, -216(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh__xor_0
.L_whe__xor_1:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn__xor_1
	li	a0, 0
.Lreturn__xor_1:
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
	addi	sp, sp, -448
	li	t0, 448
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh__or_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	sw	a0, -80(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe__or_1
	mv	a0, t4
	sw	a0, -96(s0)
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
	addi	t1, s0, -28
	sw	a0, 0(t1)
	mv	a0, t5
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
	addi	t1, s0, -32
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -128(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	mv	a0, t5
	sw	a0, -144(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -152(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	mv	t5, a0
	lw	a0, -104(s0)
	addiw	a0, a0, -1
	sw	a0, -160(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	beqz	a0, .L_ifelse__or_3
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	lw	t2, -176(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_ifend__or_2
.L_ifelse__or_3:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	li	a0, 1
	sw	a0, -208(s0)
	lw	a0, -200(s0)
	addiw	a0, a0, -1
	sw	a0, -216(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	beqz	a0, .L_ifend__or_4
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	lw	t2, -232(s0)
	addw	a0, t2, a0
	sw	a0, -248(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_ifend__or_4:
.L_ifend__or_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	slliw	a0, a0, 1
	sw	a0, -264(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addiw	a0, a0, -1
	sw	a0, -280(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh__or_0
.L_whe__or_1:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn__or_2
	li	a0, 0
.Lreturn__or_2:
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
	addi	sp, sp, -80
	li	t0, 80
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	mv	a0, t5
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
	beqz	a0, .L_ifend_rotrN_0
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -2
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_1
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 4
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -3
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_2
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 8
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 4
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -4
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_3
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 16
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 5
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -5
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_4
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 32
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 6
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -6
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_5
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 64
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 7
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -7
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_6
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 128
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 8
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -8
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotrN_7
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 256
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	mv	a0, t4
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_rotrN_3
	li	a0, 0
.Lreturn_rotrN_3:
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
	addi	sp, sp, -144
	li	t0, 144
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	mv	a0, t5
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
	beqz	a0, .L_ifend_rotlN_0
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 1
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_0:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -2
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_1
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 2
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_1:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -3
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_2
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 3
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_2:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 4
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -4
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_3
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 4
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_3:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 5
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -5
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_4
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 5
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_4:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 6
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -6
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_5
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 6
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_5:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 7
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -7
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_6
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 7
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_6:
	mv	a0, t5
	sw	a0, -40(s0)
	li	a0, 8
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -8
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifend_rotlN_7
	mv	a0, t4
	sw	a0, -40(s0)
	slliw	a0, a0, 8
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
.L_ifend_rotlN_7:
	mv	a0, t4
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_rotlN_4
	li	a0, 0
.Lreturn_rotlN_4:
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
	addi	sp, sp, -256
	li	t0, 256
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
.L_wh_read_bits_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lla	a0, data
	sd	a0, -64(s0)
	lw	a0, -40(s0)
	sw	a0, -72(s0)
	li	a0, 8
	sw	a0, -80(s0)
	lw	a0, -48(s0)
	sw	a0, -88(s0)
	lw	a0, -48(s0)
	beqz	a0, .L_whe_read_bits_1
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	beqz	a0, .L_ifend_read_bits_2
	j	.L_whe_read_bits_1
.L_ifend_read_bits_2:
	lla	t0, pos
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lla	t0, size
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	lw	t2, -56(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_ifend_read_bits_3
	j	.L_whe_read_bits_1
.L_ifend_read_bits_3:
	lla	t0, buf
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lla	t0, pos
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -160(s0)
	addiw	a0, a0, 0
	sw	a0, -168(s0)
	ld	t2, -64(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -184(s0)
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	lw	a0, -184(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -192(s0)
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
	sw	a0, -200(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -200(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	or	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -208(s0)
	lla	t1, buf
	sw	a0, 0(t1)
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	addiw	a0, a0, 8
	sw	a0, -224(s0)
	lla	t1, bits
	sw	a0, 0(t1)
	lla	t0, pos
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	addiw	a0, a0, 1
	sw	a0, -240(s0)
	lla	t1, pos
	sw	a0, 0(t1)
	j	.L_wh_read_bits_0
.L_whe_read_bits_1:
	lla	t0, buf
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 1
	mv	t4, a0
	lw	a0, -56(s0)
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
	sw	a0, -64(s0)
	li	a0, 1
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	addiw	a0, a0, -1
	sw	a0, -80(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -80(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	and	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -88(s0)
	lla	t0, buf
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -104(s0)
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
	sw	a0, 0(t1)
	lla	t0, bits
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	lw	t2, -120(s0)
	subw	a0, t2, a0
	sw	a0, -136(s0)
	lla	t1, bits
	sw	a0, 0(t1)
	lw	a0, -88(s0)
	sext.w	a0, a0
	j	.Lreturn_read_bits_5
	li	a0, 0
.Lreturn_read_bits_5:
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
	li	t0, 288
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -24
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -20
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
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_whe_decode_fixed_huffman_1
	li	a0, 1
	sext.w	a0, a0
	mv	t4, a0
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
	mv	t4, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -64(s0)
	addiw	a0, a0, 64
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	lw	t2, -56(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_ifelse_decode_fixed_huffman_5
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	li	a0, 144
	sw	a0, -56(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -48(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_ifelse_decode_fixed_huffman_7
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	lla	a0, out
	sd	a0, -112(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	slliw	a0, a0, 2
	sw	a0, -128(s0)
	ld	t2, -112(s0)
	add	a0, t2, a0
	sd	a0, -136(s0)
	mv	t1, a0
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	li	a0, 1
	sw	a0, -152(s0)
	lw	a0, -144(s0)
	addiw	a0, a0, 1
	sw	a0, -160(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_wh_decode_fixed_huffman_2
	j	.L_ifend_decode_fixed_huffman_6
.L_ifelse_decode_fixed_huffman_7:
	lla	t0, pos
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	lla	t0, size
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	lw	t2, -168(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -184(s0)
	lw	a0, -184(s0)
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
	sw	a0, -192(s0)
	lla	t0, size
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	lw	t2, -192(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	beqz	a0, .L_ifelse_decode_fixed_huffman_11
	j	.L_wh_decode_fixed_huffman_2
	j	.L_ifend_decode_fixed_huffman_10
.L_ifelse_decode_fixed_huffman_11:
	j	.L_whe_decode_fixed_huffman_3
.L_ifend_decode_fixed_huffman_10:
.L_ifend_decode_fixed_huffman_4:
	j	.L_wh_decode_fixed_huffman_2
.L_whe_decode_fixed_huffman_3:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lla	t1, out_num
	sw	a0, 0(t1)
	j	.Lreturn_decode_fixed_huffman_12
.Lreturn_decode_fixed_huffman_12:
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
	li	t0, 32
	add	t0, sp, t0
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
	addi	sp, sp, -240
	li	t0, 240
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	lla	a0, data
	sd	a0, -64(s0)
	ld	a0, -64(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -72(s0)
	lla	t1, size
	sw	a0, 0(t1)
	li	a0, 2000
	sw	a0, -80(s0)
	addi	t1, s0, -32
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
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -64(s0)
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
	sw	a0, 0(t1)
	li	a0, 1
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 2
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	read_bits
	sw	a0, -88(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	call	decode_fixed_huffman
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addiw	a0, a0, -1
	sw	a0, -104(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
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
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 32
	sw	a0, -56(s0)
	li	a0, 32
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -72(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
.L_wh_main_2:
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	beqz	a0, .L_whe_main_3
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	lla	t0, out_num
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifelse_main_5
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	output_data
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addiw	a0, a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_ifend_main_4
.L_ifelse_main_5:
	j	.L_whe_main_3
.L_ifend_main_4:
	j	.L_wh_main_2
.L_whe_main_3:
	li	a0, 10
	sw	a0, -40(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_main_46
	li	a0, 0
.Lreturn_main_46:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
