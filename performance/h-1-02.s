	.bss
	.align	2
	.globl	lim
lim:
	.zero	4
	.text
	.text
	.align	1
	.globl	fun
	.type	fun, @function
fun:
	addi	sp, sp, -192
	addi	t0, sp, 192
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -104(s0)
	sd	s2, -112(s0)
	sd	s3, -120(s0)
	sd	s4, -128(s0)
	sd	s5, -136(s0)
	sd	s6, -144(s0)
	sd	s7, -152(s0)
	sd	s8, -160(s0)
	sd	s9, -168(s0)
	sd	s10, -176(s0)
	sd	s11, -184(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifelse_fun_1
	lw	a0, -24(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_fun_0
	j	.L_ifend_fun_0
.L_ifelse_fun_1:
	li	a0, 2
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifelse_fun_3
	li	a0, 2
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -64(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	sp, sp, -16
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	fun
	addi	sp, sp, 16
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	sext.w	a0, a0
	j	.Lreturn_fun_0
	j	.L_ifend_fun_2
.L_ifelse_fun_3:
	li	a0, 3
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -20(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	lla	t0, lim
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	t2, -80(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_ifend_fun_4
	li	a0, 3
	mv	s3, a0
	sw	s3, -40(s0)
	li	a0, 1
	mv	s5, a0
	sw	s5, -48(s0)
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	slliw	t2, a0, 1
	addw	a0, t2, a0
	mv	s4, a0
	sw	s4, -64(s0)
	mv	a0, s4
	addiw	a0, a0, 1
	mv	s6, a0
	sw	s6, -72(s0)
	lw	a0, -24(s0)
	mv	s7, a0
	sw	s7, -80(s0)
	mv	a0, s7
	addiw	a0, a0, 1
	mv	s8, a0
	sw	s8, -88(s0)
	addi	sp, sp, -16
	mv	a0, s6
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s8
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	fun
	addi	sp, sp, 16
	mv	s9, a0
	sw	s9, -96(s0)
	mv	a0, s9
	sext.w	a0, a0
	j	.Lreturn_fun_0
.L_ifend_fun_4:
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	mv	s10, a0
	sw	s10, -48(s0)
	mv	a0, s10
	slliw	a0, a0, 2
	mv	s11, a0
	sw	s11, -56(s0)
	mv	a0, s11
	addiw	a0, a0, 1
	mv	s1, a0
	sw	s1, -64(s0)
	lla	t0, lim
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	mv	a0, s1
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_ifend_fun_5
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	slliw	a0, a0, 2
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	addiw	a0, a0, 1
	sw	a0, -64(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	sp, sp, -16
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	fun
	addi	sp, sp, 16
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	sext.w	a0, a0
	j	.Lreturn_fun_0
.L_ifend_fun_5:
	li	a0, 7
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_fun_0
.L_ifend_fun_2:
.L_ifend_fun_0:
	li	a0, 0
	j	.Lreturn_fun_0
	li	a0, 0
.Lreturn_fun_0:
	ld	s11, -184(s0)
	ld	s10, -176(s0)
	ld	s9, -168(s0)
	ld	s8, -160(s0)
	ld	s7, -152(s0)
	ld	s6, -144(s0)
	ld	s5, -136(s0)
	ld	s4, -128(s0)
	ld	s3, -120(s0)
	ld	s2, -112(s0)
	ld	s1, -104(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	fun, .-fun
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -240
	addi	t0, sp, 240
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -152(s0)
	sd	s2, -160(s0)
	sd	s3, -168(s0)
	sd	s4, -176(s0)
	sd	s5, -184(s0)
	sd	s6, -192(s0)
	sd	s7, -200(s0)
	sd	s8, -208(s0)
	sd	s9, -216(s0)
	sd	s10, -224(s0)
	sd	s11, -232(s0)
	call	getint
	sw	a0, -40(s0)
	lla	t1, lim
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -20
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 23
	sw	a0, -64(s0)
	addi	sp, sp, -16
	li	a0, 23
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
.L_wh_main_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1000000007
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -24(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	lla	t0, lim
	lw	a0, 0(t0)
	mv	s5, a0
	sw	s5, -72(s0)
	mv	t2, s5
	sext.w	t2, t2
	mv	a0, s4
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s6, a0
	sw	s6, -80(s0)
	mv	a0, s6
	sext.w	a0, a0
	seqz	a0, a0
	mv	s7, a0
	sw	s7, -88(s0)
	mv	a0, s7
	beqz	a0, .L_whe_main_1
	lw	a0, -20(s0)
	mv	s8, a0
	sw	s8, -96(s0)
	lw	a0, -24(s0)
	mv	s9, a0
	sw	s9, -104(s0)
	addi	sp, sp, -16
	mv	a0, s9
	sext.w	a0, a0
	sd	a0, 0(sp)
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	fun
	addi	sp, sp, 16
	mv	s11, a0
	sw	s11, -112(s0)
	mv	t2, s8
	mv	a0, s11
	addw	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1152921497
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 28
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 1000000007
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -128(s0)
	addi	t1, s0, -20
	lw	a0, -128(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	mv	s10, a0
	sw	s10, -144(s0)
	addi	t1, s0, -24
	mv	a0, s10
	sw	a0, 0(t1)
	mv	s10, a0
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 28
	mv	s3, a0
	sw	s3, -40(s0)
	addi	sp, sp, -16
	li	a0, 28
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	mv	a0, s2
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 0
	mv	s1, a0
	sw	s1, -56(s0)
	mv	a0, s1
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	s11, -232(s0)
	ld	s10, -224(s0)
	ld	s9, -216(s0)
	ld	s8, -208(s0)
	ld	s7, -200(s0)
	ld	s6, -192(s0)
	ld	s5, -184(s0)
	ld	s4, -176(s0)
	ld	s3, -168(s0)
	ld	s2, -160(s0)
	ld	s1, -152(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
