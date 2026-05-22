	.bss
	.align	2
	.globl	N
N:
	.zero	4
	.bss
	.align	2
	.globl	W
W:
	.zero	4
	.bss
	.align	2
	.globl	weight
weight:
	.zero	200
	.bss
	.align	2
	.globl	value
value:
	.zero	200
	.bss
	.align	2
	.globl	__knapsack_dp
__knapsack_dp:
	.zero	52224
	.text
	.text
	.align	1
	.globl	knapsack_naive
	.type	knapsack_naive, @function
knapsack_naive:
	addi	sp, sp, -368
	li	t0, 368
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifelse_knapsack_naive_1
	li	a0, 0
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
	j	.L_ifend_knapsack_naive_0
.L_ifelse_knapsack_naive_1:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	beqz	a0, .L_ifend_knapsack_naive_2
	li	a0, 0
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
.L_ifend_knapsack_naive_2:
.L_ifend_knapsack_naive_0:
	lla	a0, weight
	sd	a0, -40(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -72(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -80(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -88(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_ifelse_knapsack_naive_4
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	addi	sp, sp, -16
	lw	a0, -56(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	knapsack_naive
	addi	sp, sp, 16
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
	j	.L_ifend_knapsack_naive_3
.L_ifelse_knapsack_naive_4:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	addi	sp, sp, -16
	lw	a0, -56(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	knapsack_naive
	addi	sp, sp, 16
	sw	a0, -72(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	lla	a0, value
	sd	a0, -80(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	lw	a0, -88(s0)
	addiw	a0, a0, -1
	sw	a0, -104(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -128(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, -1
	sw	a0, -144(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	lla	a0, weight
	sd	a0, -160(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	addiw	a0, a0, -1
	sw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -200(s0)
	lw	t2, -152(s0)
	subw	a0, t2, a0
	sw	a0, -208(s0)
	addi	sp, sp, -16
	lw	a0, -144(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -208(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	knapsack_naive
	addi	sp, sp, 16
	sw	a0, -216(s0)
	lw	t2, -128(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -224(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_ifelse_knapsack_naive_6
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
	j	.L_ifend_knapsack_naive_5
.L_ifelse_knapsack_naive_6:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
.L_ifend_knapsack_naive_5:
.L_ifend_knapsack_naive_3:
	li	a0, 0
	j	.Lreturn_knapsack_naive_0
	li	a0, 0
.Lreturn_knapsack_naive_0:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	knapsack_naive, .-knapsack_naive
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -1216
	li	t0, 1216
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	call	getint
	sw	a0, -64(s0)
	lla	t1, N
	sw	a0, 0(t1)
	call	getint
	sw	a0, -72(s0)
	lla	t1, W
	sw	a0, 0(t1)
	lla	a0, weight
	sd	a0, -80(s0)
	ld	a0, -80(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -88(s0)
	lla	a0, value
	sd	a0, -96(s0)
	ld	a0, -96(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -104(s0)
	li	a0, 32
	sw	a0, -112(s0)
	addi	sp, sp, -16
	li	a0, 32
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -120(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -136(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
.L_wh_main_0:
	lla	t0, W
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lla	a0, __knapsack_dp
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -72(s0)
	ld	a0, -64(s0)
	sd	a0, -64(s0)
	li	a0, 1
	sw	a0, -80(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	t2, -56(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_main_1
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -64(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	mv	t1, a0
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_2:
	lla	t0, N
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	lla	t0, W
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lla	a0, weight
	sd	a0, -80(s0)
	lw	a0, -64(s0)
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	sw	a0, -104(s0)
	lw	a0, -96(s0)
	sw	a0, -112(s0)
	addi	t0, s0, -28
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
	beqz	a0, .L_whe_main_3
	addi	t1, s0, -20
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_4:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	addiw	a0, a0, -1
	sw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -160(s0)
	addiw	a0, a0, 0
	sw	a0, -168(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -192(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_whe_main_5
	ld	t2, -176(s0)
	lw	a0, 0(t2)
	sw	a0, -208(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -208(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	beqz	a0, .L_ifelse_main_7
	lla	a0, __knapsack_dp
	sd	a0, -176(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	li	a0, 1
	sw	a0, -240(s0)
	lw	a0, -232(s0)
	addiw	a0, a0, -1
	sw	a0, -248(s0)
	slliw	a0, a0, 10
	sw	a0, -256(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -264(s0)
	slliw	a0, a0, 2
	sw	a0, -272(s0)
	lw	t2, -256(s0)
	addw	a0, t2, a0
	sw	a0, -280(s0)
	ld	t2, -176(s0)
	add	a0, t2, a0
	sd	a0, -288(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -296(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	slliw	a0, a0, 10
	sw	a0, -312(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	slliw	a0, a0, 2
	sw	a0, -328(s0)
	lw	t2, -312(s0)
	addw	a0, t2, a0
	sw	a0, -336(s0)
	ld	t2, -176(s0)
	add	a0, t2, a0
	sd	a0, -344(s0)
	mv	t1, a0
	lw	a0, -296(s0)
	sw	a0, 0(t1)
	j	.L_ifend_main_6
.L_ifelse_main_7:
	li	a0, 0
	sw	a0, -352(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -360(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	lla	a0, __knapsack_dp
	sd	a0, -368(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	li	a0, 1
	sw	a0, -384(s0)
	lw	a0, -376(s0)
	addiw	a0, a0, -1
	sw	a0, -392(s0)
	slliw	a0, a0, 10
	sw	a0, -400(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -408(s0)
	slliw	a0, a0, 2
	sw	a0, -416(s0)
	lw	t2, -400(s0)
	addw	a0, t2, a0
	sw	a0, -424(s0)
	ld	t2, -368(s0)
	add	a0, t2, a0
	sd	a0, -432(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -440(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	lla	a0, value
	sd	a0, -448(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -456(s0)
	li	a0, 1
	sw	a0, -464(s0)
	lw	a0, -456(s0)
	addiw	a0, a0, -1
	sw	a0, -472(s0)
	slliw	a0, a0, 2
	sw	a0, -480(s0)
	ld	t2, -448(s0)
	add	a0, t2, a0
	sd	a0, -488(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -496(s0)
	lla	a0, __knapsack_dp
	sd	a0, -504(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -512(s0)
	addiw	a0, a0, -1
	sw	a0, -520(s0)
	slliw	a0, a0, 10
	sw	a0, -528(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -536(s0)
	lla	a0, weight
	sd	a0, -544(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -552(s0)
	addiw	a0, a0, -1
	sw	a0, -560(s0)
	slliw	a0, a0, 2
	sw	a0, -568(s0)
	ld	t2, -544(s0)
	add	a0, t2, a0
	sd	a0, -576(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -584(s0)
	lw	t2, -536(s0)
	subw	a0, t2, a0
	sw	a0, -592(s0)
	slliw	a0, a0, 2
	sw	a0, -600(s0)
	lw	t2, -528(s0)
	addw	a0, t2, a0
	sw	a0, -608(s0)
	ld	t2, -504(s0)
	add	a0, t2, a0
	sd	a0, -616(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -624(s0)
	lw	t2, -496(s0)
	addw	a0, t2, a0
	sw	a0, -632(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	lw	t2, -440(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -640(s0)
	lw	a0, -640(s0)
	beqz	a0, .L_ifelse_main_9
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -648(s0)
	lla	a0, __knapsack_dp
	sd	a0, -656(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -664(s0)
	slliw	a0, a0, 10
	sw	a0, -672(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -680(s0)
	slliw	a0, a0, 2
	sw	a0, -688(s0)
	lw	t2, -672(s0)
	addw	a0, t2, a0
	sw	a0, -696(s0)
	ld	t2, -656(s0)
	add	a0, t2, a0
	sd	a0, -704(s0)
	mv	t1, a0
	lw	a0, -648(s0)
	sw	a0, 0(t1)
	j	.L_ifend_main_8
.L_ifelse_main_9:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -712(s0)
	lla	a0, __knapsack_dp
	sd	a0, -720(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -728(s0)
	slliw	a0, a0, 10
	sw	a0, -736(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -744(s0)
	slliw	a0, a0, 2
	sw	a0, -752(s0)
	lw	t2, -736(s0)
	addw	a0, t2, a0
	sw	a0, -760(s0)
	ld	t2, -720(s0)
	add	a0, t2, a0
	sd	a0, -768(s0)
	mv	t1, a0
	lw	a0, -712(s0)
	sw	a0, 0(t1)
.L_ifend_main_8:
.L_ifend_main_6:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -776(s0)
	addiw	a0, a0, 1
	sw	a0, -784(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	lla	a0, __knapsack_dp
	sd	a0, -56(s0)
	lla	t0, N
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	slliw	a0, a0, 10
	sw	a0, -72(s0)
	lla	t0, W
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	slliw	a0, a0, 2
	sw	a0, -88(s0)
	lw	t2, -72(s0)
	addw	a0, t2, a0
	sw	a0, -96(s0)
	ld	t2, -56(s0)
	add	a0, t2, a0
	sd	a0, -104(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -112(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 34
	sw	a0, -120(s0)
	addi	sp, sp, -16
	li	a0, 34
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -136(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
