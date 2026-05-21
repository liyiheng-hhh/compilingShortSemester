	.bss
	.align	2
	.globl	matrix
matrix:
	.zero	80000000
	.bss
	.align	2
	.globl	a
a:
	.zero	400000
	.text
	.text
	.align	1
	.globl	transpose
	.type	transpose, @function
transpose:
	addi	sp, sp, -512
	addi	t0, sp, 512
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -456(s0)
	sd	s2, -464(s0)
	sd	s3, -472(s0)
	sd	s4, -480(s0)
	sd	s5, -488(s0)
	sd	s6, -496(s0)
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sw	a2, -36(s0)
	lw	a0, -20(s0)
	lw	a0, -36(s0)
	lw	t2, -20(s0)
	divw	a0, t2, a0
	sw	a0, -88(s0)
	addi	t1, s0, -40
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -44
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -48
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_transpose_0:
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	lw	a0, -40(s0)
	lw	a0, -36(s0)
	lw	a0, -44(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -40(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	beqz	a0, .L_whe_transpose_1
	addi	t1, s0, -48
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_transpose_2:
	lw	a0, -44(s0)
	lw	a0, -48(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -36(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_whe_transpose_3
	lw	a0, -48(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_ifend_transpose_4
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -152(s0)
	addi	t1, s0, -48
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	j	.L_wh_transpose_2
.L_ifend_transpose_4:
	ld	a0, -32(s0)
	sd	a0, -160(s0)
	lw	a0, -44(s0)
	lw	a0, -36(s0)
	lw	t2, -44(s0)
	mulw	a0, t2, a0
	sw	a0, -184(s0)
	lw	a0, -48(s0)
	lw	t2, -184(s0)
	addw	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	slliw	a0, a0, 2
	sw	a0, -208(s0)
	ld	t2, -160(s0)
	lw	a0, -208(s0)
	add	a0, t2, a0
	sd	a0, -216(s0)
	ld	t2, -216(s0)
	lw	a0, 0(t2)
	sw	a0, -224(s0)
	addi	t1, s0, -52
	lw	a0, -224(s0)
	sw	a0, 0(t1)
	ld	a0, -32(s0)
	sd	a0, -232(s0)
	lw	a0, -44(s0)
	lw	a0, -36(s0)
	mv	s2, a0
	sw	s2, -248(s0)
	lw	t2, -44(s0)
	mv	a0, s2
	mulw	a0, t2, a0
	mv	s3, a0
	sw	s3, -256(s0)
	lw	a0, -48(s0)
	mv	s4, a0
	sw	s4, -264(s0)
	mv	t2, s3
	mv	a0, s4
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -272(s0)
	mv	a0, s5
	slliw	a0, a0, 2
	mv	s6, a0
	sw	s6, -280(s0)
	ld	t2, -232(s0)
	mv	a0, s6
	add	a0, t2, a0
	sd	a0, -288(s0)
	ld	t2, -288(s0)
	lw	a0, 0(t2)
	sw	a0, -296(s0)
	lw	a0, -48(s0)
	lw	a0, -40(s0)
	lw	t2, -48(s0)
	mulw	a0, t2, a0
	sw	a0, -320(s0)
	lw	a0, -44(s0)
	lw	t2, -320(s0)
	addw	a0, t2, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	ld	t2, -232(s0)
	lw	a0, -344(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	ld	t1, -352(s0)
	lw	a0, -296(s0)
	sw	a0, 0(t1)
	ld	a0, -32(s0)
	mv	s1, a0
	sd	s1, -360(s0)
	lw	a0, -44(s0)
	lw	a0, -36(s0)
	lw	t2, -44(s0)
	mulw	a0, t2, a0
	sw	a0, -384(s0)
	lw	a0, -48(s0)
	lw	t2, -384(s0)
	addw	a0, t2, a0
	sw	a0, -400(s0)
	lw	a0, -400(s0)
	slliw	a0, a0, 2
	sw	a0, -408(s0)
	mv	t2, s1
	lw	a0, -408(s0)
	add	a0, t2, a0
	sd	a0, -416(s0)
	ld	t1, -416(s0)
	lw	a0, -52(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -424(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -440(s0)
	addi	t1, s0, -48
	lw	a0, -440(s0)
	sw	a0, 0(t1)
	j	.L_wh_transpose_2
.L_whe_transpose_3:
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -448(s0)
	addi	t1, s0, -44
	lw	a0, -448(s0)
	sw	a0, 0(t1)
	j	.L_wh_transpose_0
.L_whe_transpose_1:
	li	a0, -1
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	j	.Lreturn_transpose_0
	li	a0, 0
.Lreturn_transpose_0:
	ld	s6, -496(s0)
	ld	s5, -488(s0)
	ld	s4, -480(s0)
	ld	s3, -472(s0)
	ld	s2, -464(s0)
	ld	s1, -456(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	transpose, .-transpose
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -272
	addi	t0, sp, 272
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -184(s0)
	sd	s2, -192(s0)
	sd	s3, -200(s0)
	sd	s4, -208(s0)
	sd	s5, -216(s0)
	sd	s6, -224(s0)
	sd	s7, -232(s0)
	sd	s8, -240(s0)
	sd	s9, -248(s0)
	sd	s10, -256(s0)
	sd	s11, -264(s0)
	call	getint
	sw	a0, -40(s0)
	addi	t1, s0, -20
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	lla	a0, a
	sd	a0, -48(s0)
	ld	a0, -48(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 28
	sw	a0, -64(s0)
	addi	sp, sp, -16
	li	a0, 28
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -28
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 4
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	a0, matrix
	sd	a0, -56(s0)
	lw	a0, -20(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_main_1
	lw	a0, -28(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -56(s0)
	lw	a0, -104(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	ld	t1, -112(s0)
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 3
	addw	a0, a0, t1
	sraiw	a0, a0, 2
	li	t1, 4
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_ifend_main_2
	li	a0, 4
	sw	a0, -40(s0)
	lla	a0, matrix
	sd	a0, -56(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -152(s0)
	ld	t2, -56(s0)
	lw	a0, -152(s0)
	add	a0, t2, a0
	sd	a0, -160(s0)
	ld	t1, -160(s0)
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_ifend_main_2:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -176(s0)
	addi	t1, s0, -28
	lw	a0, -176(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_main_3:
	li	a0, 1
	sw	a0, -40(s0)
	lla	a0, matrix
	sd	a0, -48(s0)
	lla	a0, a
	sd	a0, -56(s0)
	lw	a0, -24(s0)
	lw	a0, -20(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -24(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_4
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -56(s0)
	lw	a0, -104(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	ld	t2, -112(s0)
	lw	a0, 0(t2)
	sw	a0, -120(s0)
	addi	sp, sp, -32
	lw	a0, -72(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -48(s0)
	sd	a0, 8(sp)
	lw	a0, -120(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	transpose
	addi	sp, sp, 32
	sw	a0, -128(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -28
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_3
.L_whe_main_4:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -28
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_main_5:
	li	a0, 1
	sw	a0, -40(s0)
	lla	a0, matrix
	sd	a0, -48(s0)
	lw	a0, -24(s0)
	mv	s3, a0
	sw	s3, -56(s0)
	lw	a0, -28(s0)
	mv	s2, a0
	sw	s2, -64(s0)
	mv	t2, s2
	sext.w	t2, t2
	mv	a0, s3
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s4, a0
	sw	s4, -72(s0)
	mv	a0, s4
	beqz	a0, .L_whe_main_6
	lw	a0, -32(s0)
	mv	s5, a0
	sw	s5, -80(s0)
	lw	a0, -28(s0)
	mv	s6, a0
	sw	s6, -88(s0)
	lw	a0, -28(s0)
	mv	s7, a0
	sw	s7, -96(s0)
	mv	t2, s6
	mv	a0, s7
	mulw	a0, t2, a0
	mv	s8, a0
	sw	s8, -104(s0)
	lw	a0, -28(s0)
	mv	s9, a0
	sw	s9, -112(s0)
	mv	a0, s9
	slliw	a0, a0, 2
	mv	s10, a0
	sw	s10, -120(s0)
	ld	t2, -48(s0)
	mv	a0, s10
	add	a0, t2, a0
	mv	s11, a0
	sd	s11, -128(s0)
	mv	t2, s11
	lw	a0, 0(t2)
	sw	a0, -136(s0)
	mv	t2, s8
	lw	a0, -136(s0)
	mulw	a0, t2, a0
	sw	a0, -144(s0)
	mv	t2, s5
	lw	a0, -144(s0)
	addw	a0, t2, a0
	sw	a0, -152(s0)
	addi	t1, s0, -32
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -168(s0)
	addi	t1, s0, -28
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_5
.L_whe_main_6:
	li	a0, 0
	sw	a0, -40(s0)
	lw	a0, -32(s0)
	lw	t2, -32(s0)
	sext.w	t2, t2
	lw	a0, -40(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_main_7
	lw	a0, -32(s0)
	negw	a0, a0
	sw	a0, -48(s0)
	addi	t1, s0, -32
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_ifend_main_7:
	li	a0, 49
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 49
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -32(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -56(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	mv	s1, a0
	sw	s1, -64(s0)
	mv	a0, s1
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	s11, -264(s0)
	ld	s10, -256(s0)
	ld	s9, -248(s0)
	ld	s8, -240(s0)
	ld	s7, -232(s0)
	ld	s6, -224(s0)
	ld	s5, -216(s0)
	ld	s4, -208(s0)
	ld	s3, -200(s0)
	ld	s2, -192(s0)
	ld	s1, -184(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
