	.text
	.text
	.align	1
	.globl	max
	.type	max, @function
max:
	addi	sp, sp, -64
	li	t0, 64
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	mv	a0, t4
	sw	a0, -40(s0)
	mv	a0, t5
	sw	a0, -48(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifelse_max_1
	mv	a0, t5
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_max_0
	j	.L_ifend_max_0
.L_ifelse_max_1:
	mv	a0, t4
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_max_0
.L_ifend_max_0:
	li	a0, 0
	j	.Lreturn_max_0
	li	a0, 0
.Lreturn_max_0:
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
	addi	sp, sp, -256
	li	t0, 256
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 2147483647
	sw	a0, -48(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	t2, -48(s0)
	subw	a0, t2, a0
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
	call	max
	addi	sp, sp, 16
	sw	a0, -72(s0)
	li	a0, 1073741823
	sw	a0, -80(s0)
	mv	t2, a0
	lw	a0, -72(s0)
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
	li	a0, 536870912
	sw	a0, -104(s0)
	mv	t2, a0
	lw	a0, -96(s0)
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
	sw	a0, -120(s0)
	li	a0, 3
	sw	a0, -128(s0)
	lw	a0, -120(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -136(s0)
	li	a0, 1000
	sw	a0, -144(s0)
	lw	a0, -136(s0)
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
	sw	a0, -152(s0)
	li	a0, 1001
	sw	a0, -160(s0)
	lw	a0, -152(s0)
	mv	t2, a0
	slliw	t1, a0, 3
	addw	t2, t2, t1
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -168(s0)
	lw	t2, -120(s0)
	addw	a0, t2, a0
	sw	a0, -176(s0)
	li	a0, 19491001
	sw	a0, -184(s0)
	lw	a0, -176(s0)
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
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	sext.w	a0, a0
	j	.Lreturn_f_1
	li	a0, 0
.Lreturn_f_1:
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
	addi	sp, sp, -192
	li	t0, 192
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	sw	a2, -28(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
.L_wh_loop_test_0:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	li	a0, 998244853
	sw	a0, -72(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_loop_test_1
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	f
	sw	a0, -120(s0)
	lw	t2, -104(s0)
	addw	a0, t2, a0
	sw	a0, -128(s0)
	addiw	a0, a0, 1
	sw	a0, -136(s0)
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
	sw	a0, -144(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	mv	t2, a0
	lw	a0, -80(s0)
	addw	a0, t2, a0
	sw	a0, -160(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh_loop_test_0
.L_whe_loop_test_1:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_loop_test_2
	li	a0, 0
.Lreturn_loop_test_2:
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
	addi	sp, sp, -176
	li	t0, 176
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -24
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	call	getint
	sw	a0, -48(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 33
	sw	a0, -56(s0)
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
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_whe_main_1
	call	getint
	sw	a0, -64(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	call	getint
	sw	a0, -72(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	call	getint
	sw	a0, -80(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	sp, sp, -32
	lw	a0, -88(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -96(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	loop_test
	addi	sp, sp, 32
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addiw	a0, a0, -1
	sw	a0, -120(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 42
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 42
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	j	.Lreturn_main_3
	li	a0, 0
.Lreturn_main_3:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
