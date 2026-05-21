	.text
	.text
	.align	1
	.globl	dependent_computation
	.type	dependent_computation, @function
dependent_computation:
	addi	sp, sp, -240
	addi	t0, sp, 240
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -192(s0)
	sd	s2, -200(s0)
	sd	s3, -208(s0)
	sd	s4, -216(s0)
	sd	s5, -224(s0)
	sd	s6, -232(s0)
	sw	a0, -20(s0)
	li	a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 2
	sw	a0, -64(s0)
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -72(s0)
	addi	t1, s0, -32
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 4
	sw	a0, -80(s0)
	addi	t1, s0, -36
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -40
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_dependent_computation_0:
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_dependent_computation_1
	lw	a0, -24(s0)
	lw	a0, -28(s0)
	lw	t2, -24(s0)
	addw	a0, t2, a0
	sw	a0, -104(s0)
	addi	t1, s0, -24
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	lw	a0, -32(s0)
	lw	t2, -28(s0)
	addw	a0, t2, a0
	sw	a0, -128(s0)
	addi	t1, s0, -28
	lw	a0, -128(s0)
	sw	a0, 0(t1)
	lw	a0, -32(s0)
	mv	s1, a0
	sw	s1, -136(s0)
	lw	a0, -36(s0)
	mv	t2, s1
	addw	a0, t2, a0
	sw	a0, -152(s0)
	addi	t1, s0, -32
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	lw	a0, -36(s0)
	lw	t2, -36(s0)
	lw	a0, -24(s0)
	addw	a0, t2, a0
	sw	a0, -168(s0)
	addi	t1, s0, -36
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	lw	a0, -40(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -40
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	j	.L_wh_dependent_computation_0
.L_whe_dependent_computation_1:
	lw	a0, -24(s0)
	lw	a0, -28(s0)
	lw	t2, -24(s0)
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -72(s0)
	lw	a0, -32(s0)
	mv	s5, a0
	sw	s5, -80(s0)
	mv	t2, s6
	mv	a0, s5
	addw	a0, t2, a0
	mv	s4, a0
	sw	s4, -88(s0)
	lw	a0, -36(s0)
	mv	s3, a0
	sw	s3, -96(s0)
	mv	t2, s4
	mv	a0, s3
	addw	a0, t2, a0
	mv	s2, a0
	sw	s2, -104(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_dependent_computation_0
	li	a0, 0
.Lreturn_dependent_computation_0:
	ld	s6, -232(s0)
	ld	s5, -224(s0)
	ld	s4, -216(s0)
	ld	s3, -208(s0)
	ld	s2, -200(s0)
	ld	s1, -192(s0)
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
	li	a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 2
	sw	a0, -80(s0)
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -88(s0)
	addi	t1, s0, -32
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 4
	sw	a0, -96(s0)
	addi	t1, s0, -36
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -40
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_independent_computation_0:
	li	a0, 1
	sw	a0, -72(s0)
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_independent_computation_1
	lw	a0, -24(s0)
	lw	a0, -28(s0)
	lw	t2, -24(s0)
	addw	a0, t2, a0
	sw	a0, -120(s0)
	addi	t1, s0, -44
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	lw	a0, -32(s0)
	lw	t2, -28(s0)
	addw	a0, t2, a0
	sw	a0, -144(s0)
	addi	t1, s0, -48
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	lw	a0, -32(s0)
	lw	a0, -36(s0)
	mv	s1, a0
	sw	s1, -160(s0)
	lw	t2, -32(s0)
	mv	a0, s1
	addw	a0, t2, a0
	sw	a0, -168(s0)
	addi	t1, s0, -52
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	lw	a0, -36(s0)
	lw	a0, -24(s0)
	lw	t2, -36(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	addi	t1, s0, -56
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -24
	lw	a0, -44(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -32
	lw	a0, -52(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	lw	a0, -40(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -40
	lw	a0, -208(s0)
	sw	a0, 0(t1)
	j	.L_wh_independent_computation_0
.L_whe_independent_computation_1:
	lw	a0, -24(s0)
	lw	a0, -28(s0)
	lw	t2, -24(s0)
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -88(s0)
	lw	a0, -32(s0)
	mv	s5, a0
	sw	s5, -96(s0)
	mv	t2, s6
	mv	a0, s5
	addw	a0, t2, a0
	mv	s4, a0
	sw	s4, -104(s0)
	lw	a0, -36(s0)
	mv	s3, a0
	sw	s3, -112(s0)
	mv	t2, s4
	mv	a0, s3
	addw	a0, t2, a0
	mv	s2, a0
	sw	s2, -120(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_independent_computation_1
	li	a0, 0
.Lreturn_independent_computation_1:
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
	.size	independent_computation, .-independent_computation
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -256
	addi	t0, sp, 256
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -160(s0)
	sd	s2, -168(s0)
	sd	s3, -176(s0)
	sd	s4, -184(s0)
	sd	s5, -192(s0)
	sd	s6, -200(s0)
	sd	s7, -208(s0)
	sd	s8, -216(s0)
	sd	s9, -224(s0)
	sd	s10, -232(s0)
	sd	s11, -240(s0)
	li	a0, 0
	mv	s1, a0
	sw	s1, -40(s0)
	addi	t1, s0, -20
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	li	a0, 0
	mv	s2, a0
	sw	s2, -48(s0)
	addi	t1, s0, -24
	mv	a0, s2
	sw	a0, 0(t1)
	mv	s2, a0
	li	a0, 0
	mv	s3, a0
	sw	s3, -56(s0)
	addi	t1, s0, -28
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	call	getint
	mv	s4, a0
	sw	s4, -64(s0)
	addi	t1, s0, -28
	mv	a0, s4
	sw	a0, 0(t1)
	mv	s4, a0
	li	a0, 40
	mv	s5, a0
	sw	s5, -72(s0)
	addi	sp, sp, -16
	li	a0, 40
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	lw	a0, -28(s0)
	mv	s6, a0
	sw	s6, -80(s0)
	mv	a0, s6
	sext.w	a0, a0
	mv	a0, t4
	call	dependent_computation
	mv	s7, a0
	sw	s7, -88(s0)
	addi	t1, s0, -20
	mv	a0, s7
	sw	a0, 0(t1)
	mv	s7, a0
	lw	a0, -28(s0)
	mv	s8, a0
	sw	s8, -96(s0)
	mv	a0, s8
	sext.w	a0, a0
	mv	a0, t4
	call	independent_computation
	mv	s9, a0
	sw	s9, -104(s0)
	addi	t1, s0, -24
	mv	a0, s9
	sw	a0, 0(t1)
	mv	s9, a0
	li	a0, 43
	mv	s10, a0
	sw	s10, -112(s0)
	addi	sp, sp, -16
	li	a0, 43
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -20(s0)
	mv	s11, a0
	sw	s11, -120(s0)
	mv	a0, s11
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -128(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	lw	a0, -24(s0)
	lw	a0, -136(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -144(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	sext.w	a0, a0
	j	.Lreturn_main_2
	li	a0, 0
.Lreturn_main_2:
	ld	s11, -240(s0)
	ld	s10, -232(s0)
	ld	s9, -224(s0)
	ld	s8, -216(s0)
	ld	s7, -208(s0)
	ld	s6, -200(s0)
	ld	s5, -192(s0)
	ld	s4, -184(s0)
	ld	s3, -176(s0)
	ld	s2, -168(s0)
	ld	s1, -160(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
