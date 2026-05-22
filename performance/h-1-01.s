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
	addi	sp, sp, -160
	li	t0, 160
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, -1
	sw	a0, -56(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifelse_fun_1
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_fun_0
	j	.L_ifend_fun_0
.L_ifelse_fun_1:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_ifelse_fun_3
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -56(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	li	a0, 1
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	sp, sp, -16
	lw	a0, -56(s0)
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
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -56(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	lla	t0, lim
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_ifend_fun_4
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -56(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addiw	a0, a0, 1
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
	call	fun
	addi	sp, sp, 16
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	sext.w	a0, a0
	j	.Lreturn_fun_0
.L_ifend_fun_4:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	slliw	a0, a0, 2
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -64(s0)
	lla	t0, lim
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_ifend_fun_5
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	slliw	a0, a0, 2
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -72(s0)
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
	addi	sp, sp, -176
	li	t0, 176
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	call	getint
	sw	a0, -40(s0)
	lla	t1, lim
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
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
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	lla	t0, lim
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_1
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	addi	sp, sp, -16
	lw	a0, -104(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	fun
	addi	sp, sp, 16
	sw	a0, -112(s0)
	lw	t2, -96(s0)
	addw	a0, t2, a0
	sw	a0, -120(s0)
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
	sw	a0, 0(t1)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 28
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 28
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
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
