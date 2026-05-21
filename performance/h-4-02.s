	.text
	.text
	.align	1
	.globl	max
	.type	max, @function
max:
	addi	sp, sp, -112
	addi	t0, sp, 112
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -64(s0)
	sd	s2, -72(s0)
	sd	s3, -80(s0)
	sd	s4, -88(s0)
	sd	s5, -96(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	mv	t2, s1
	sext.w	t2, t2
	mv	a0, s2
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s3, a0
	sw	s3, -56(s0)
	mv	a0, s3
	beqz	a0, .L_ifelse_max_1
	lw	a0, -24(s0)
	mv	s4, a0
	sw	s4, -40(s0)
	mv	a0, s4
	sext.w	a0, a0
	j	.Lreturn_max_0
	j	.L_ifend_max_0
.L_ifelse_max_1:
	lw	a0, -20(s0)
	mv	s5, a0
	sw	s5, -40(s0)
	mv	a0, s5
	sext.w	a0, a0
	j	.Lreturn_max_0
.L_ifend_max_0:
	li	a0, 0
	j	.Lreturn_max_0
	li	a0, 0
.Lreturn_max_0:
	ld	s5, -96(s0)
	ld	s4, -88(s0)
	ld	s3, -80(s0)
	ld	s2, -72(s0)
	ld	s1, -64(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	max, .-max
	.text
	.align	1
	.globl	f
	.type	f, @function
f:
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
	sw	a0, -20(s0)
	li	a0, 2147483647
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	mv	s11, a0
	sw	s11, -48(s0)
	lw	a0, -20(s0)
	lw	t2, -40(s0)
	subw	a0, t2, a0
	sw	a0, -64(s0)
	addi	sp, sp, -16
	mv	a0, s11
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	max
	addi	sp, sp, 16
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 1073741823
	sw	a0, -80(s0)
	lw	t2, -80(s0)
	lw	a0, -24(s0)
	subw	a0, t2, a0
	sw	a0, -88(s0)
	addi	sp, sp, -16
	lw	a0, -72(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -88(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	max
	addi	sp, sp, 16
	sw	a0, -96(s0)
	addi	t1, s0, -28
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	li	a0, 536870912
	sw	a0, -104(s0)
	lw	t2, -104(s0)
	lw	a0, -28(s0)
	subw	a0, t2, a0
	sw	a0, -112(s0)
	addi	sp, sp, -16
	lw	a0, -96(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -112(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	max
	addi	sp, sp, 16
	mv	s1, a0
	sw	s1, -120(s0)
	addi	t1, s0, -32
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	li	a0, 3
	mv	s10, a0
	sw	s10, -128(s0)
	mv	a0, s1
	slliw	t2, a0, 1
	addw	a0, t2, a0
	mv	s9, a0
	sw	s9, -136(s0)
	li	a0, 1000
	mv	s8, a0
	sw	s8, -144(s0)
	li	a0, 1001
	mv	s6, a0
	sw	s6, -152(s0)
	li	a0, 19491001
	mv	s3, a0
	sw	s3, -160(s0)
	mv	a0, s9
	sext.w	a0, a0
	li	t1, 1099511628
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 8
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	mv	s7, a0
	sw	s7, -168(s0)
	mv	a0, s7
	mv	t2, a0
	li	a0, 1001
	mulw	a0, t2, a0
	mv	s5, a0
	sw	s5, -176(s0)
	mv	t2, s1
	mv	a0, s5
	addw	a0, t2, a0
	mv	s4, a0
	sw	s4, -184(s0)
	mv	a0, s4
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1848483668
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 23
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 19491001
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s2, a0
	sw	s2, -192(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_f_1
	li	a0, 0
.Lreturn_f_1:
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
	.size	f, .-f
	.text
	.align	1
	.globl	loop_test
	.type	loop_test, @function
loop_test:
	addi	sp, sp, -256
	addi	t0, sp, 256
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -168(s0)
	sd	s2, -176(s0)
	sd	s3, -184(s0)
	sd	s4, -192(s0)
	sd	s5, -200(s0)
	sd	s6, -208(s0)
	sd	s7, -216(s0)
	sd	s8, -224(s0)
	sd	s9, -232(s0)
	sd	s10, -240(s0)
	sd	s11, -248(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	sw	a2, -28(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -32
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
.L_wh_loop_test_0:
	li	a0, 1
	sw	a0, -56(s0)
	li	a0, 998244853
	sw	a0, -64(s0)
	lw	a0, -24(s0)
	mv	s11, a0
	sw	s11, -72(s0)
	lw	a0, -28(s0)
	lw	a0, -36(s0)
	lw	t2, -36(s0)
	sext.w	t2, t2
	mv	a0, s11
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s10, a0
	sw	s10, -96(s0)
	mv	a0, s10
	beqz	a0, .L_whe_loop_test_1
	lw	a0, -32(s0)
	mv	s9, a0
	sw	s9, -104(s0)
	lw	a0, -36(s0)
	mv	s8, a0
	sw	s8, -112(s0)
	mv	a0, s8
	sext.w	a0, a0
	mv	a0, t4
	call	f
	mv	s7, a0
	sw	s7, -120(s0)
	mv	t2, s9
	mv	a0, s7
	addw	a0, t2, a0
	mv	s1, a0
	sw	s1, -128(s0)
	mv	a0, s1
	addiw	a0, a0, 1
	mv	s6, a0
	sw	s6, -136(s0)
	mv	a0, s6
	mv	t6, a0
	sext.w	a0, t6
	li	t1, -1985070077
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	addw	t2, t2, a0
	sraiw	t2, t2, 29
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 998244853
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s5, a0
	sw	s5, -144(s0)
	addi	t1, s0, -32
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -36(s0)
	mv	s4, a0
	sw	s4, -152(s0)
	mv	t2, s4
	lw	a0, -80(s0)
	addw	a0, t2, a0
	mv	s3, a0
	sw	s3, -160(s0)
	addi	t1, s0, -36
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh_loop_test_0
.L_whe_loop_test_1:
	lw	a0, -32(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_loop_test_2
	li	a0, 0
.Lreturn_loop_test_2:
	ld	s11, -248(s0)
	ld	s10, -240(s0)
	ld	s9, -232(s0)
	ld	s8, -224(s0)
	ld	s7, -216(s0)
	ld	s6, -208(s0)
	ld	s5, -200(s0)
	ld	s4, -192(s0)
	ld	s3, -184(s0)
	ld	s2, -176(s0)
	ld	s1, -168(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	loop_test, .-loop_test
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
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
	call	getint
	sw	a0, -40(s0)
	addi	t1, s0, -20
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 33
	sw	a0, -48(s0)
	addi	sp, sp, -16
	li	a0, 33
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
.L_wh_main_0:
	li	a0, 10
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -56(s0)
	mv	a0, s1
	beqz	a0, .L_whe_main_1
	call	getint
	mv	s2, a0
	sw	s2, -64(s0)
	addi	t1, s0, -24
	mv	a0, s2
	sw	a0, 0(t1)
	mv	s2, a0
	call	getint
	mv	s3, a0
	sw	s3, -72(s0)
	addi	t1, s0, -28
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	call	getint
	mv	s4, a0
	sw	s4, -80(s0)
	addi	t1, s0, -32
	mv	a0, s4
	sw	a0, 0(t1)
	mv	s4, a0
	lw	a0, -24(s0)
	mv	s5, a0
	sw	s5, -88(s0)
	lw	a0, -28(s0)
	mv	s6, a0
	sw	s6, -96(s0)
	addi	sp, sp, -32
	mv	a0, s5
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s6
	sext.w	a0, a0
	sd	a0, 8(sp)
	mv	a0, s4
	sext.w	a0, a0
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	loop_test
	addi	sp, sp, 32
	mv	s7, a0
	sw	s7, -104(s0)
	mv	a0, s7
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	lw	a0, -20(s0)
	mv	s8, a0
	sw	s8, -112(s0)
	mv	a0, s8
	addiw	a0, a0, -1
	mv	s9, a0
	sw	s9, -120(s0)
	addi	t1, s0, -20
	mv	a0, s9
	sw	a0, 0(t1)
	mv	s9, a0
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 42
	mv	s10, a0
	sw	s10, -40(s0)
	addi	sp, sp, -16
	li	a0, 42
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	li	a0, 0
	mv	s11, a0
	sw	s11, -48(s0)
	mv	a0, s11
	sext.w	a0, a0
	j	.Lreturn_main_3
	li	a0, 0
.Lreturn_main_3:
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
	.size	main, .-main
