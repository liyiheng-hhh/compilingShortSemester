	.text
	.text
	.align	1
	.globl	dependent_computation
	.type	dependent_computation, @function
dependent_computation:
	addi	sp, sp, -256
	li	t0, 256
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	mv	t4, a0
	li	a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 2
	sw	a0, -64(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -72(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 4
	sw	a0, -80(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_wh_dependent_computation_0:
	mv	a0, t4
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_dependent_computation_1
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	t2, -88(s0)
	addw	a0, t2, a0
	sw	a0, -104(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	lw	t2, -112(s0)
	addw	a0, t2, a0
	sw	a0, -128(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lw	t2, -136(s0)
	addw	a0, t2, a0
	sw	a0, -152(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	mv	t2, a0
	lw	a0, -104(s0)
	addw	a0, t2, a0
	sw	a0, -168(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_dependent_computation_0
.L_whe_dependent_computation_1:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	lw	t2, -56(s0)
	addw	a0, t2, a0
	sw	a0, -72(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	t2, -72(s0)
	addw	a0, t2, a0
	sw	a0, -88(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	t2, -88(s0)
	addw	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	sext.w	a0, a0
	j	.Lreturn_dependent_computation_0
	li	a0, 0
.Lreturn_dependent_computation_0:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	dependent_computation, .-dependent_computation
	.text
	.align	1
	.globl	independent_computation
	.type	independent_computation, @function
independent_computation:
	addi	sp, sp, -320
	li	t0, 320
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	mv	t4, a0
	addi	t1, s0, -44
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 2
	sw	a0, -88(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -96(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 4
	sw	a0, -104(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -112(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_wh_independent_computation_0:
	mv	a0, t4
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_independent_computation_1
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	lw	t2, -104(s0)
	addw	a0, t2, a0
	sw	a0, -120(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	lw	t2, -128(s0)
	addw	a0, t2, a0
	sw	a0, -144(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	lw	t2, -152(s0)
	addw	a0, t2, a0
	sw	a0, -168(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	lw	t2, -176(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	addi	t1, s0, -24
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -32
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_independent_computation_0
.L_whe_independent_computation_1:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	t2, -72(s0)
	addw	a0, t2, a0
	sw	a0, -88(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	t2, -88(s0)
	addw	a0, t2, a0
	sw	a0, -104(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	lw	t2, -104(s0)
	addw	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	sext.w	a0, a0
	j	.Lreturn_independent_computation_1
	li	a0, 0
.Lreturn_independent_computation_1:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	independent_computation, .-independent_computation
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
	li	a0, 0
	sw	a0, -40(s0)
	call	getint
	sw	a0, -48(s0)
	li	a0, 40
	sw	a0, -56(s0)
	addi	sp, sp, -16
	li	a0, 40
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	dependent_computation
	sw	a0, -64(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	independent_computation
	sw	a0, -72(s0)
	li	a0, 43
	sw	a0, -80(s0)
	addi	sp, sp, -16
	li	a0, 43
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -64(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -88(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	lw	a0, -72(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -96(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	sext.w	a0, a0
	j	.Lreturn_main_2
	li	a0, 0
.Lreturn_main_2:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
