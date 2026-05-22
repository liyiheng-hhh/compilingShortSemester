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
	addi	sp, sp, -608
	li	t0, 608
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sw	a2, -36(s0)
	mv	t4, a0
	mv	t5, a2
	addi	t1, s0, -52
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	mv	a0, t4
	sw	a0, -80(s0)
	mv	a0, t5
	sw	a0, -88(s0)
	lw	t2, -80(s0)
	divw	a0, t2, a0
	sw	a0, -96(s0)
	addi	t1, s0, -44
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
.L_wh_transpose_0:
	li	a0, 0
	sw	a0, -72(s0)
	mv	a0, t5
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	sw	a0, -104(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_transpose_1
	addi	t1, s0, -48
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_transpose_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -80(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_transpose_3
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_ifend_transpose_4
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -160(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_transpose_2
.L_ifend_transpose_4:
	ld	a0, -32(s0)
	sd	a0, -168(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	mv	a0, t5
	sw	a0, -184(s0)
	lw	t2, -176(s0)
	mulw	a0, t2, a0
	sw	a0, -192(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	lw	t2, -192(s0)
	addw	a0, t2, a0
	sw	a0, -208(s0)
	slliw	a0, a0, 2
	sw	a0, -216(s0)
	ld	t2, -168(s0)
	add	a0, t2, a0
	sd	a0, -224(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -232(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	ld	a0, -32(s0)
	sd	a0, -240(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	mv	a0, t5
	sw	a0, -256(s0)
	lw	t2, -248(s0)
	mulw	a0, t2, a0
	sw	a0, -264(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	lw	t2, -264(s0)
	addw	a0, t2, a0
	sw	a0, -280(s0)
	slliw	a0, a0, 2
	sw	a0, -288(s0)
	ld	t2, -240(s0)
	add	a0, t2, a0
	sd	a0, -296(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -304(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	mv	t2, a0
	lw	a0, -96(s0)
	mulw	a0, t2, a0
	sw	a0, -320(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	lw	t2, -320(s0)
	addw	a0, t2, a0
	sw	a0, -336(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	ld	t2, -240(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	mv	t1, a0
	lw	a0, -304(s0)
	sw	a0, 0(t1)
	lw	a0, -232(s0)
	sw	a0, -360(s0)
	ld	a0, -32(s0)
	sd	a0, -368(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	mv	a0, t5
	sw	a0, -384(s0)
	lw	t2, -376(s0)
	mulw	a0, t2, a0
	sw	a0, -392(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -400(s0)
	lw	t2, -392(s0)
	addw	a0, t2, a0
	sw	a0, -408(s0)
	slliw	a0, a0, 2
	sw	a0, -416(s0)
	ld	t2, -368(s0)
	add	a0, t2, a0
	sd	a0, -424(s0)
	mv	t1, a0
	lw	a0, -360(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -432(s0)
	li	a0, 1
	sw	a0, -440(s0)
	lw	a0, -432(s0)
	addiw	a0, a0, 1
	sw	a0, -448(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_transpose_2
.L_whe_transpose_3:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addiw	a0, a0, 1
	sw	a0, -456(s0)
	addi	t1, s0, -44
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
	addi	sp, sp, -368
	li	t0, 368
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	call	getint
	sw	a0, -48(s0)
	lla	a0, a
	sd	a0, -56(s0)
	ld	a0, -56(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -64(s0)
	li	a0, 28
	sw	a0, -72(s0)
	addi	sp, sp, -16
	li	a0, 28
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_0:
	lla	a0, matrix
	sd	a0, -40(s0)
	li	a0, 4
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -48(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_1
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t1, a0
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 3
	addw	a0, a0, t1
	sraiw	a0, a0, 2
	li	t1, 4
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -136(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_ifend_main_2
	li	a0, 4
	sw	a0, -40(s0)
	lla	a0, matrix
	sd	a0, -56(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -160(s0)
	ld	t2, -56(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	mv	t1, a0
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_ifend_main_2:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_3:
	lla	a0, matrix
	sd	a0, -40(s0)
	lla	a0, a
	sd	a0, -56(s0)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_4
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -56(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -120(s0)
	addi	sp, sp, -32
	lw	a0, -48(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -40(s0)
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
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_3
.L_whe_main_4:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_5:
	lla	a0, matrix
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	sd	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	beqz	a0, .L_whe_main_6
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	t2, -88(s0)
	mulw	a0, t2, a0
	sw	a0, -104(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -136(s0)
	lw	t2, -104(s0)
	mulw	a0, t2, a0
	sw	a0, -144(s0)
	lw	t2, -80(s0)
	addw	a0, t2, a0
	sw	a0, -152(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	addiw	a0, a0, 1
	sw	a0, -168(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_5
.L_whe_main_6:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 0
	sw	a0, -48(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_main_7
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	negw	a0, a0
	sw	a0, -48(s0)
	addi	t1, s0, -32
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
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -56(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
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
