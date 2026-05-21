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
	addi	sp, sp, -336
	addi	t0, sp, 336
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -248(s0)
	sd	s2, -256(s0)
	sd	s3, -264(s0)
	sd	s4, -272(s0)
	sd	s5, -280(s0)
	sd	s6, -288(s0)
	sd	s7, -296(s0)
	sd	s8, -304(s0)
	sd	s9, -312(s0)
	sd	s10, -320(s0)
	sd	s11, -328(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	beqz	a0, .L_ifelse_knapsack_naive_1
	li	a0, 0
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
	j	.L_ifend_knapsack_naive_0
.L_ifelse_knapsack_naive_1:
	lw	a0, -24(s0)
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
	li	a0, 1
	sw	a0, -40(s0)
	lla	a0, weight
	sd	a0, -48(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -72(s0)
	ld	t2, -48(s0)
	lw	a0, -72(s0)
	add	a0, t2, a0
	sd	a0, -80(s0)
	ld	t2, -80(s0)
	lw	a0, 0(t2)
	sw	a0, -88(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_ifelse_knapsack_naive_4
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	lw	a0, -24(s0)
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
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	lw	a0, -24(s0)
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
	mv	s2, a0
	sw	s2, -72(s0)
	addi	t1, s0, -28
	mv	a0, s2
	sw	a0, 0(t1)
	mv	s2, a0
	li	a0, 1
	mv	s5, a0
	sw	s5, -80(s0)
	lla	a0, value
	mv	s3, a0
	sd	s3, -88(s0)
	lla	a0, weight
	mv	s1, a0
	sd	s1, -96(s0)
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -104(s0)
	mv	a0, s4
	addiw	a0, a0, -1
	mv	s6, a0
	sw	s6, -112(s0)
	mv	a0, s6
	slliw	a0, a0, 2
	mv	s7, a0
	sw	s7, -120(s0)
	mv	t2, s3
	mv	a0, s7
	add	a0, t2, a0
	mv	s8, a0
	sd	s8, -128(s0)
	mv	t2, s8
	lw	a0, 0(t2)
	mv	s9, a0
	sw	s9, -136(s0)
	lw	a0, -20(s0)
	mv	s10, a0
	sw	s10, -144(s0)
	mv	a0, s10
	addiw	a0, a0, -1
	mv	s11, a0
	sw	s11, -152(s0)
	lw	a0, -24(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	mv	t2, s1
	lw	a0, -184(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	ld	t2, -192(s0)
	lw	a0, 0(t2)
	sw	a0, -200(s0)
	lw	t2, -24(s0)
	lw	a0, -200(s0)
	subw	a0, t2, a0
	sw	a0, -208(s0)
	addi	sp, sp, -16
	mv	a0, s11
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
	mv	t2, s9
	lw	a0, -216(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t1, s0, -32
	lw	a0, -224(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -32(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_ifelse_knapsack_naive_6
	lw	a0, -224(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
	j	.L_ifend_knapsack_naive_5
.L_ifelse_knapsack_naive_6:
	lw	a0, -28(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_knapsack_naive_0
.L_ifend_knapsack_naive_5:
.L_ifend_knapsack_naive_3:
	li	a0, 0
	j	.Lreturn_knapsack_naive_0
	li	a0, 0
.Lreturn_knapsack_naive_0:
	ld	s11, -328(s0)
	ld	s10, -320(s0)
	ld	s9, -312(s0)
	ld	s8, -304(s0)
	ld	s7, -296(s0)
	ld	s6, -288(s0)
	ld	s5, -280(s0)
	ld	s4, -272(s0)
	ld	s3, -264(s0)
	ld	s2, -256(s0)
	ld	s1, -248(s0)
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
	addi	sp, sp, -848
	addi	t0, sp, 848
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -760(s0)
	sd	s2, -768(s0)
	sd	s3, -776(s0)
	sd	s4, -784(s0)
	sd	s5, -792(s0)
	sd	s6, -800(s0)
	sd	s7, -808(s0)
	sd	s8, -816(s0)
	sd	s9, -824(s0)
	sd	s10, -832(s0)
	sd	s11, -840(s0)
	call	getint
	sw	a0, -56(s0)
	lla	t1, N
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	call	getint
	sw	a0, -64(s0)
	lla	t1, W
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	lla	a0, weight
	sd	a0, -72(s0)
	ld	a0, -72(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -80(s0)
	lla	a0, value
	sd	a0, -88(s0)
	ld	a0, -88(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -96(s0)
	li	a0, 32
	sw	a0, -104(s0)
	addi	sp, sp, -16
	li	a0, 32
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -112(s0)
	addi	t1, s0, -20
	lw	a0, -112(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -120(s0)
	addi	t1, s0, -24
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -20
	lw	a0, -128(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lla	a0, __knapsack_dp
	sd	a0, -72(s0)
	lla	t0, W
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	a0, -20(s0)
	lw	t2, -80(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_main_1
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -72(s0)
	lw	a0, -120(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	ld	t1, -128(s0)
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -20
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_2:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lla	a0, weight
	sd	a0, -72(s0)
	lla	t0, N
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lla	t0, W
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	a0, -28(s0)
	lw	t2, -80(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	beqz	a0, .L_whe_main_3
	addi	t1, s0, -20
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_main_4:
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	ld	t2, -72(s0)
	lw	a0, -144(s0)
	add	a0, t2, a0
	sd	a0, -152(s0)
	lw	a0, -20(s0)
	lw	t2, -88(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_main_5
	ld	t2, -152(s0)
	lw	a0, 0(t2)
	sw	a0, -184(s0)
	lw	a0, -20(s0)
	lw	t2, -20(s0)
	sext.w	t2, t2
	lw	a0, -184(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_ifelse_main_7
	li	a0, 1
	sw	a0, -152(s0)
	lla	a0, __knapsack_dp
	sd	a0, -208(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	slliw	a0, a0, 10
	sw	a0, -232(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -248(s0)
	lw	t2, -232(s0)
	lw	a0, -248(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	ld	t2, -208(s0)
	lw	a0, -256(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	ld	t2, -264(s0)
	lw	a0, 0(t2)
	sw	a0, -272(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 10
	sw	a0, -288(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -304(s0)
	lw	t2, -288(s0)
	lw	a0, -304(s0)
	addw	a0, t2, a0
	sw	a0, -312(s0)
	ld	t2, -208(s0)
	lw	a0, -312(s0)
	add	a0, t2, a0
	sd	a0, -320(s0)
	ld	t1, -320(s0)
	lw	a0, -272(s0)
	sw	a0, 0(t1)
	j	.L_ifend_main_6
.L_ifelse_main_7:
	li	a0, 0
	sw	a0, -328(s0)
	addi	t1, s0, -32
	lw	a0, -328(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -336(s0)
	addi	t1, s0, -36
	lw	a0, -336(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -344(s0)
	lla	a0, __knapsack_dp
	sd	a0, -352(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -368(s0)
	lw	a0, -368(s0)
	slliw	a0, a0, 10
	sw	a0, -376(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -392(s0)
	lw	t2, -376(s0)
	lw	a0, -392(s0)
	addw	a0, t2, a0
	sw	a0, -400(s0)
	ld	t2, -352(s0)
	lw	a0, -400(s0)
	add	a0, t2, a0
	sd	a0, -408(s0)
	ld	t2, -408(s0)
	lw	a0, 0(t2)
	sw	a0, -416(s0)
	addi	t1, s0, -32
	lw	a0, -416(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -424(s0)
	lla	a0, value
	sd	a0, -432(s0)
	lla	a0, __knapsack_dp
	sd	a0, -440(s0)
	lla	a0, weight
	sd	a0, -448(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -464(s0)
	lw	a0, -464(s0)
	slliw	a0, a0, 2
	sw	a0, -472(s0)
	ld	t2, -432(s0)
	lw	a0, -472(s0)
	add	a0, t2, a0
	sd	a0, -480(s0)
	ld	t2, -480(s0)
	lw	a0, 0(t2)
	sw	a0, -488(s0)
	lw	a0, -28(s0)
	mv	s1, a0
	sw	s1, -496(s0)
	mv	a0, s1
	addiw	a0, a0, -1
	sw	a0, -504(s0)
	lw	a0, -504(s0)
	slliw	a0, a0, 10
	sw	a0, -512(s0)
	lw	a0, -20(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	mv	s11, a0
	sw	s11, -536(s0)
	mv	a0, s11
	slliw	a0, a0, 2
	mv	s10, a0
	sw	s10, -544(s0)
	ld	t2, -448(s0)
	mv	a0, s10
	add	a0, t2, a0
	mv	s9, a0
	sd	s9, -552(s0)
	mv	t2, s9
	lw	a0, 0(t2)
	mv	s8, a0
	sw	s8, -560(s0)
	lw	t2, -20(s0)
	mv	a0, s8
	subw	a0, t2, a0
	mv	s7, a0
	sw	s7, -568(s0)
	mv	a0, s7
	slliw	a0, a0, 2
	mv	s6, a0
	sw	s6, -576(s0)
	lw	t2, -512(s0)
	mv	a0, s6
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -584(s0)
	ld	t2, -440(s0)
	mv	a0, s5
	add	a0, t2, a0
	mv	s4, a0
	sd	s4, -592(s0)
	mv	t2, s4
	lw	a0, 0(t2)
	mv	s3, a0
	sw	s3, -600(s0)
	lw	t2, -488(s0)
	mv	a0, s3
	addw	a0, t2, a0
	mv	s2, a0
	sw	s2, -608(s0)
	addi	t1, s0, -36
	mv	a0, s2
	sw	a0, 0(t1)
	mv	s2, a0
	lw	t2, -32(s0)
	sext.w	t2, t2
	mv	a0, s2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -616(s0)
	lw	a0, -616(s0)
	beqz	a0, .L_ifelse_main_9
	lla	a0, __knapsack_dp
	sd	a0, -624(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 10
	sw	a0, -640(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -656(s0)
	lw	t2, -640(s0)
	lw	a0, -656(s0)
	addw	a0, t2, a0
	sw	a0, -664(s0)
	ld	t2, -624(s0)
	lw	a0, -664(s0)
	add	a0, t2, a0
	sd	a0, -672(s0)
	ld	t1, -672(s0)
	mv	a0, s2
	sw	a0, 0(t1)
	j	.L_ifend_main_8
.L_ifelse_main_9:
	lla	a0, __knapsack_dp
	sd	a0, -680(s0)
	lw	a0, -32(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 10
	sw	a0, -704(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -720(s0)
	lw	t2, -704(s0)
	lw	a0, -720(s0)
	addw	a0, t2, a0
	sw	a0, -728(s0)
	ld	t2, -680(s0)
	lw	a0, -728(s0)
	add	a0, t2, a0
	sd	a0, -736(s0)
	ld	t1, -736(s0)
	lw	a0, -32(s0)
	sw	a0, 0(t1)
.L_ifend_main_8:
.L_ifend_main_6:
	lw	a0, -20(s0)
	addiw	a0, a0, 1
	sw	a0, -752(s0)
	addi	t1, s0, -20
	lw	a0, -752(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -28
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	lla	a0, __knapsack_dp
	sd	a0, -56(s0)
	lla	t0, N
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 10
	sw	a0, -72(s0)
	lla	t0, W
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	slliw	a0, a0, 2
	sw	a0, -88(s0)
	lw	t2, -72(s0)
	lw	a0, -88(s0)
	addw	a0, t2, a0
	sw	a0, -96(s0)
	ld	t2, -56(s0)
	lw	a0, -96(s0)
	add	a0, t2, a0
	sd	a0, -104(s0)
	ld	t2, -104(s0)
	lw	a0, 0(t2)
	sw	a0, -112(s0)
	addi	t1, s0, -40
	lw	a0, -112(s0)
	sw	a0, 0(t1)
	li	a0, 34
	sw	a0, -120(s0)
	addi	sp, sp, -16
	li	a0, 34
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -40(s0)
	lw	a0, -128(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -136(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	s11, -840(s0)
	ld	s10, -832(s0)
	ld	s9, -824(s0)
	ld	s8, -816(s0)
	ld	s7, -808(s0)
	ld	s6, -800(s0)
	ld	s5, -792(s0)
	ld	s4, -784(s0)
	ld	s3, -776(s0)
	ld	s2, -768(s0)
	ld	s1, -760(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
