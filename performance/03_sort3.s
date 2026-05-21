	.bss
	.align	2
	.globl	a
a:
	.zero	120000040
	.bss
	.align	2
	.globl	ans
ans:
	.zero	4
	.text
	.text
	.align	1
	.globl	getMaxNum
	.type	getMaxNum, @function
getMaxNum:
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
	sd	a1, -32(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -36
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -40
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_getMaxNum_0:
	li	a0, 1
	sw	a0, -56(s0)
	ld	a0, -32(s0)
	sd	a0, -64(s0)
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_getMaxNum_1
	lw	a0, -40(s0)
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -64(s0)
	lw	a0, -104(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	ld	t2, -112(s0)
	lw	a0, 0(t2)
	sw	a0, -120(s0)
	lw	a0, -36(s0)
	mv	s1, a0
	sw	s1, -128(s0)
	mv	t2, s1
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_ifend_getMaxNum_2
	ld	a0, -32(s0)
	sd	a0, -64(s0)
	lw	a0, -40(s0)
	slliw	a0, a0, 2
	sw	a0, -152(s0)
	ld	t2, -64(s0)
	lw	a0, -152(s0)
	add	a0, t2, a0
	mv	s6, a0
	sd	s6, -160(s0)
	mv	t2, s6
	lw	a0, 0(t2)
	mv	s5, a0
	sw	s5, -168(s0)
	addi	t1, s0, -36
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
.L_ifend_getMaxNum_2:
	lw	a0, -40(s0)
	mv	s4, a0
	sw	s4, -176(s0)
	mv	a0, s4
	addiw	a0, a0, 1
	mv	s3, a0
	sw	s3, -184(s0)
	addi	t1, s0, -40
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh_getMaxNum_0
.L_whe_getMaxNum_1:
	lw	a0, -36(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_getMaxNum_0
	li	a0, 0
.Lreturn_getMaxNum_0:
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
	.size	getMaxNum, .-getMaxNum
	.text
	.align	1
	.globl	getNumPos
	.type	getNumPos, @function
getNumPos:
	addi	sp, sp, -160
	addi	t0, sp, 160
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -112(s0)
	sd	s2, -120(s0)
	sd	s3, -128(s0)
	sd	s4, -136(s0)
	sd	s5, -144(s0)
	sd	s6, -152(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 1
	sw	a0, -40(s0)
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -32
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_getNumPos_0:
	li	a0, 16
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	lw	a0, -32(s0)
	mv	s1, a0
	sw	s1, -64(s0)
	mv	t2, s1
	sext.w	t2, t2
	mv	a0, s2
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s3, a0
	sw	s3, -72(s0)
	mv	a0, s3
	beqz	a0, .L_whe_getNumPos_1
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -80(s0)
	mv	a0, s4
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 15
	addw	a0, a0, t1
	sraiw	a0, a0, 4
	mv	s5, a0
	sw	s5, -88(s0)
	addi	t1, s0, -20
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -32(s0)
	mv	s6, a0
	sw	s6, -96(s0)
	mv	a0, s6
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	j	.L_wh_getNumPos_0
.L_whe_getNumPos_1:
	li	a0, 16
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 15
	addw	a0, a0, t1
	sraiw	a0, a0, 4
	li	t1, 16
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_getNumPos_1
	li	a0, 0
.Lreturn_getNumPos_1:
	ld	s6, -152(s0)
	ld	s5, -144(s0)
	ld	s4, -136(s0)
	ld	s3, -128(s0)
	ld	s2, -120(s0)
	ld	s1, -112(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	getNumPos, .-getNumPos
	.text
	.align	1
	.globl	radixSort
	.type	radixSort, @function
radixSort:
	addi	sp, sp, -256
	addi	t0, sp, 256
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sw	a2, -36(s0)
	sw	a3, -40(s0)
	addi	t3, s0, -104
	li	t1, 64
.Lzero_array_3:
	beqz	t1, .Lzero_array_end_4
	sw	zero, 0(t3)
	addi	t3, t3, 4
	addi	t1, t1, -4
	j	.Lzero_array_3
.Lzero_array_end_4:
	addi	t3, s0, -168
	li	t1, 64
.Lzero_array_5:
	beqz	t1, .Lzero_array_end_6
	sw	zero, 0(t3)
	addi	t3, t3, 4
	addi	t1, t1, -4
	j	.Lzero_array_5
.Lzero_array_end_6:
	addi	t3, s0, -232
	li	t1, 64
.Lzero_array_7:
	beqz	t1, .Lzero_array_end_8
	sw	zero, 0(t3)
	addi	t3, t3, 4
	addi	t1, t1, -4
	j	.Lzero_array_7
.Lzero_array_end_8:
	lw	a0, -20(s0)
	li	t1, -1
	beq	a0, t1, .Lif_then_9
	j	.Lif_else_10
.Lif_then_9:
	j	.Lreturn_radixSort_2
	j	.Lif_end_11
.Lif_else_10:
	lw	a0, -36(s0)
	addiw	a0, a0, 1
	mv	t4, a0
	lw	a0, -40(s0)
	blt	t4, a0, .Lif_end_14
	j	.Lreturn_radixSort_2
.Lif_end_14:
.Lif_end_11:
	lw	a0, -36(s0)
	sw	a0, -236(s0)
.Lwhile_cond_15:
	lw	a0, -236(s0)
	mv	t4, a0
	lw	a0, -40(s0)
	bge	t4, a0, .Lwhile_end_17
.Lwhile_body_16:
	addi	a0, s0, -232
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	sp, sp, -16
	ld	a0, -32(s0)
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	lw	a0, -20(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	getNumPos
	addi	sp, sp, 16
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -232
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	sp, sp, -16
	ld	a0, -32(s0)
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 0(sp)
	lw	a0, -20(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	getNumPos
	addi	sp, sp, 16
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 1
	addiw	a0, t4, 1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -236(s0)
	addiw	a0, a0, 1
	sw	a0, -236(s0)
	j	.Lwhile_cond_15
.Lwhile_end_17:
	addi	a0, s0, -104
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -36(s0)
	sw	a0, 0(t4)
	addi	a0, s0, -168
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -232
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	lw	a0, -36(s0)
	addw	a0, a0, t4
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	li	a0, 1
	sw	a0, -236(s0)
.Lwhile_cond_18:
	lw	a0, -236(s0)
	li	t1, 16
	bge	a0, t1, .Lwhile_end_20
.Lwhile_body_19:
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -168
	mv	t5, a0
	lw	a0, -236(s0)
	addiw	a0, a0, -1
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	addi	a0, s0, -168
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -232
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	lw	a0, -236(s0)
	addiw	a0, a0, 1
	sw	a0, -236(s0)
	j	.Lwhile_cond_18
.Lwhile_end_20:
	li	a0, 0
	sw	a0, -236(s0)
.Lwhile_cond_21:
	lw	a0, -236(s0)
	li	t1, 16
	bge	a0, t1, .Lwhile_end_23
.Lwhile_body_22:
.Lwhile_cond_24:
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -168
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	slt	a0, a0, a1
	bnez	a0, .Lwhile_body_25
	j	.Lwhile_end_26
.Lwhile_body_25:
	ld	a0, -32(s0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	lw	a0, 0(a0)
	sw	a0, -240(s0)
.Lwhile_cond_27:
	lw	a0, -20(s0)
	mv	t5, a0
	lw	a0, -240(s0)
	mv	a1, t5
	call	getNumPos
	mv	t4, a0
	lw	a0, -236(s0)
	beq	t4, a0, .Lwhile_end_29
.Lwhile_body_28:
	lw	a0, -240(s0)
	sw	a0, -244(s0)
	ld	a0, -32(s0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -104
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	lw	a0, -20(s0)
	mv	t5, a0
	lw	a0, -244(s0)
	mv	a1, t5
	call	getNumPos
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	lw	a0, 0(a0)
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	lw	a0, 0(a0)
	sw	a0, -240(s0)
	ld	a0, -32(s0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -104
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	lw	a0, -20(s0)
	mv	t5, a0
	lw	a0, -244(s0)
	mv	a1, t5
	call	getNumPos
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	lw	a0, 0(a0)
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	mv	t4, a0
	lw	a0, -244(s0)
	sw	a0, 0(t4)
	addi	a0, s0, -104
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	lw	a0, -20(s0)
	mv	t5, a0
	lw	a0, -244(s0)
	mv	a1, t5
	call	getNumPos
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -104
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	lw	a0, -20(s0)
	mv	t5, a0
	lw	a0, -244(s0)
	mv	a1, t5
	call	getNumPos
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 1
	addiw	a0, t4, 1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	j	.Lwhile_cond_27
.Lwhile_end_29:
	ld	a0, -32(s0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	li	a0, 0
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	slliw	a0, a0, 2
	ld	t2, 0(sp)
	add	t2, t2, a0
	sd	t2, 0(sp)
	ld	t2, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	add	a0, a0, t2
	mv	t4, a0
	lw	a0, -240(s0)
	sw	a0, 0(t4)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -236(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	li	a0, 1
	addiw	a0, t4, 1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	j	.Lwhile_cond_24
.Lwhile_end_26:
	lw	a0, -236(s0)
	addiw	a0, a0, 1
	sw	a0, -236(s0)
	j	.Lwhile_cond_21
.Lwhile_end_23:
	lw	a0, -36(s0)
	sw	a0, -248(s0)
	addi	a0, s0, -104
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	lw	a0, -36(s0)
	sw	a0, 0(t4)
	addi	a0, s0, -168
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -232
	mv	t5, a0
	li	a0, 0
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	mv	t4, a0
	lw	a0, -36(s0)
	addw	a0, a0, t4
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	li	a0, 0
	sw	a0, -248(s0)
.Lwhile_cond_30:
	lw	a0, -248(s0)
	li	t1, 16
	bge	a0, t1, .Lwhile_end_32
.Lwhile_body_31:
	lw	a0, -248(s0)
	bge	zero, a0, .Lif_end_35
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -248(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -168
	mv	t5, a0
	lw	a0, -248(s0)
	addiw	a0, a0, -1
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
	addi	a0, s0, -168
	mv	t5, a0
	lw	a0, -248(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	mv	t4, a0
	addi	sp, sp, -16
	sd	t4, 0(sp)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -248(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	addi	a0, s0, -232
	mv	t5, a0
	lw	a0, -248(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	addi	sp, sp, -16
	sd	a0, 0(sp)
	ld	a1, 0(sp)
	addi	sp, sp, 16
	ld	a0, 0(sp)
	addi	sp, sp, 16
	addw	a0, a0, a1
	ld	t4, 0(sp)
	addi	sp, sp, 16
	sw	a0, 0(t4)
.Lif_end_35:
	addi	sp, sp, -32
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sd	a0, 0(sp)
	ld	a0, -32(s0)
	sd	a0, 8(sp)
	addi	a0, s0, -104
	mv	t5, a0
	lw	a0, -248(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 16(sp)
	addi	a0, s0, -168
	mv	t5, a0
	lw	a0, -248(s0)
	slliw	t2, a0, 2
	add	a0, t5, t2
	lw	a0, 0(a0)
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	radixSort
	addi	sp, sp, 32
	lw	a0, -248(s0)
	addiw	a0, a0, 1
	sw	a0, -248(s0)
	j	.Lwhile_cond_30
.Lwhile_end_32:
	j	.Lreturn_radixSort_2
.Lreturn_radixSort_2:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	radixSort, .-radixSort
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -304
	addi	t0, sp, 304
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -208(s0)
	sd	s2, -216(s0)
	sd	s3, -224(s0)
	sd	s4, -232(s0)
	sd	s5, -240(s0)
	sd	s6, -248(s0)
	sd	s7, -256(s0)
	sd	s8, -264(s0)
	sd	s9, -272(s0)
	sd	s10, -280(s0)
	sd	s11, -288(s0)
	lla	a0, a
	sd	a0, -40(s0)
	ld	a0, -40(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -48(s0)
	addi	t1, s0, -20
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	li	a0, 90
	sw	a0, -56(s0)
	addi	sp, sp, -16
	li	a0, 90
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 9
	sw	a0, -64(s0)
	li	a0, 0
	sw	a0, -72(s0)
	lla	a0, a
	sd	a0, -80(s0)
	lw	a0, -20(s0)
	addi	sp, sp, -32
	li	a0, 9
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -80(s0)
	sd	a0, 8(sp)
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 16(sp)
	lw	a0, -88(s0)
	sext.w	a0, a0
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	radixSort
	addi	sp, sp, 32
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -24
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 2
	sw	a0, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lla	a0, a
	sd	a0, -64(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_1
	lla	t0, ans
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	a0, -24(s0)
	lw	a0, -24(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -64(s0)
	lw	a0, -120(s0)
	add	a0, t2, a0
	mv	s11, a0
	sd	s11, -128(s0)
	mv	t2, s11
	lw	a0, 0(t2)
	mv	s10, a0
	sw	s10, -136(s0)
	lw	a0, -24(s0)
	mv	s8, a0
	sw	s8, -144(s0)
	mv	a0, s8
	addiw	a0, a0, 2
	mv	s7, a0
	sw	s7, -152(s0)
	mv	t2, s10
	mv	a0, s7
	remw	a0, t2, a0
	mv	s6, a0
	sw	s6, -160(s0)
	lw	t2, -104(s0)
	mv	a0, s6
	mulw	a0, t2, a0
	mv	s5, a0
	sw	s5, -168(s0)
	lw	t2, -96(s0)
	mv	a0, s5
	addw	a0, t2, a0
	mv	s4, a0
	sw	s4, -176(s0)
	mv	a0, s4
	addiw	a0, a0, 3
	mv	s2, a0
	sw	s2, -184(s0)
	lla	t1, ans
	mv	a0, s2
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	mv	s3, a0
	sw	s3, -192(s0)
	mv	a0, s3
	addiw	a0, a0, 1
	mv	s9, a0
	sw	s9, -200(s0)
	addi	t1, s0, -24
	mv	a0, s9
	sw	a0, 0(t1)
	mv	s9, a0
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -40(s0)
	lla	t0, ans
	lw	a0, 0(t0)
	mv	s1, a0
	sw	s1, -48(s0)
	mv	t2, s1
	sext.w	t2, t2
	lw	a0, -40(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_main_2
	mv	a0, s1
	negw	a0, a0
	sw	a0, -40(s0)
	lla	t1, ans
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_ifend_main_2:
	li	a0, 102
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 102
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lla	t0, ans
	lw	a0, 0(t0)
	sw	a0, -48(s0)
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
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	sext.w	a0, a0
	j	.Lreturn_main_36
	li	a0, 0
.Lreturn_main_36:
	ld	s11, -288(s0)
	ld	s10, -280(s0)
	ld	s9, -272(s0)
	ld	s8, -264(s0)
	ld	s7, -256(s0)
	ld	s6, -248(s0)
	ld	s5, -240(s0)
	ld	s4, -232(s0)
	ld	s3, -224(s0)
	ld	s2, -216(s0)
	ld	s1, -208(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
