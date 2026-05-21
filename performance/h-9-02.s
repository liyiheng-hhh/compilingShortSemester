	.bss
	.align	2
	.globl	seed
seed:
	.zero	4
	.text
	.text
	.align	1
	.globl	rand
	.type	rand, @function
rand:
	addi	sp, sp, -144
	addi	t0, sp, 144
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -96(s0)
	sd	s2, -104(s0)
	sd	s3, -112(s0)
	sd	s4, -120(s0)
	sd	s5, -128(s0)
	sd	s6, -136(s0)
	li	a0, 19980130
	mv	s2, a0
	sw	s2, -24(s0)
	li	a0, 23333
	mv	s4, a0
	sw	s4, -32(s0)
	li	a0, 100000007
	mv	s6, a0
	sw	s6, -40(s0)
	lla	t0, seed
	lw	a0, 0(t0)
	mv	s1, a0
	sw	s1, -48(s0)
	mv	a0, s1
	mv	t2, a0
	li	a0, 19980130
	mulw	a0, t2, a0
	mv	s3, a0
	sw	s3, -56(s0)
	mv	t2, s3
	mv	a0, s4
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -64(s0)
	mv	a0, s5
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1441151780
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 25
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 100000007
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -72(s0)
	lla	t1, seed
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	lw	a0, -80(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_ifend_rand_0
	li	a0, 100000007
	sw	a0, -24(s0)
	lw	t2, -72(s0)
	lw	a0, -24(s0)
	addw	a0, t2, a0
	sw	a0, -32(s0)
	lla	t1, seed
	lw	a0, -32(s0)
	sw	a0, 0(t1)
.L_ifend_rand_0:
	lla	t0, seed
	lw	a0, 0(t0)
	sw	a0, -24(s0)
	lw	a0, -24(s0)
	sext.w	a0, a0
	j	.Lreturn_rand_0
	li	a0, 0
.Lreturn_rand_0:
	ld	s6, -136(s0)
	ld	s5, -128(s0)
	ld	s4, -120(s0)
	ld	s3, -112(s0)
	ld	s2, -104(s0)
	ld	s1, -96(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rand, .-rand
	.text
	.align	1
	.globl	gcd
	.type	gcd, @function
gcd:
	addi	sp, sp, -144
	addi	t0, sp, 144
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -88(s0)
	sd	s2, -96(s0)
	sd	s3, -104(s0)
	sd	s4, -112(s0)
	sd	s5, -120(s0)
	sd	s6, -128(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
.L_wh_gcd_0:
	lw	a0, -24(s0)
	mv	s1, a0
	sw	s1, -40(s0)
	mv	a0, s1
	sext.w	a0, a0
	snez	a0, a0
	mv	s2, a0
	sw	s2, -48(s0)
	mv	a0, s2
	beqz	a0, .L_whe_gcd_1
	lw	a0, -24(s0)
	mv	s3, a0
	sw	s3, -56(s0)
	addi	t1, s0, -28
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	lw	a0, -24(s0)
	mv	s5, a0
	sw	s5, -72(s0)
	mv	t2, s4
	mv	a0, s5
	remw	a0, t2, a0
	mv	s6, a0
	sw	s6, -80(s0)
	addi	t1, s0, -24
	mv	a0, s6
	sw	a0, 0(t1)
	mv	s6, a0
	addi	t1, s0, -20
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh_gcd_0
.L_whe_gcd_1:
	lw	a0, -20(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_gcd_1
	li	a0, 0
.Lreturn_gcd_1:
	ld	s6, -128(s0)
	ld	s5, -120(s0)
	ld	s4, -112(s0)
	ld	s3, -104(s0)
	ld	s2, -96(s0)
	ld	s1, -88(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	gcd, .-gcd
	.text
	.align	1
	.globl	prime_factors
	.type	prime_factors, @function
prime_factors:
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
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -24
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -48(s0)
	addi	t1, s0, -28
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_prime_factors_0:
	li	a0, 2
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
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
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	beqz	a0, .L_whe_prime_factors_1
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -20
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -24
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	j	.L_wh_prime_factors_0
.L_whe_prime_factors_1:
.L_wh_prime_factors_2:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	li	a0, 2
	sw	a0, -56(s0)
	lw	a0, -28(s0)
	lw	a0, -28(s0)
	lw	t2, -64(s0)
	mulw	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -20(s0)
	lw	t2, -20(s0)
	sext.w	t2, t2
	lw	a0, -80(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_prime_factors_3
.L_wh_prime_factors_4:
	lw	a0, -28(s0)
	mv	s2, a0
	sw	s2, -112(s0)
	lw	a0, -28(s0)
	lw	a0, -20(s0)
	lw	t2, -20(s0)
	mv	a0, s2
	remw	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	mv	s3, a0
	mv	s3, a0
	sw	s3, -144(s0)
	mv	a0, s3
	sext.w	a0, a0
	seqz	a0, a0
	mv	s4, a0
	sw	s4, -152(s0)
	mv	a0, s4
	beqz	a0, .L_whe_prime_factors_5
	lw	a0, -20(s0)
	mv	s5, a0
	sw	s5, -160(s0)
	mv	t2, s5
	lw	a0, -28(s0)
	divw	a0, t2, a0
	mv	s6, a0
	sw	s6, -168(s0)
	addi	t1, s0, -20
	mv	a0, s6
	sw	a0, 0(t1)
	mv	s6, a0
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -24
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	j	.L_wh_prime_factors_4
.L_whe_prime_factors_5:
	lw	a0, -28(s0)
	addiw	a0, a0, 2
	mv	s1, a0
	sw	s1, -48(s0)
	addi	t1, s0, -28
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	j	.L_wh_prime_factors_2
.L_whe_prime_factors_3:
	li	a0, 2
	sw	a0, -40(s0)
	lw	a0, -20(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_prime_factors_6
	li	a0, 1
	sw	a0, -40(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_ifend_prime_factors_6:
	lw	a0, -24(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_prime_factors_2
	li	a0, 0
.Lreturn_prime_factors_2:
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
	.size	prime_factors, .-prime_factors
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -352
	addi	t0, sp, 352
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -256(s0)
	sd	s2, -264(s0)
	sd	s3, -272(s0)
	sd	s4, -280(s0)
	sd	s5, -288(s0)
	sd	s6, -296(s0)
	sd	s7, -304(s0)
	sd	s8, -312(s0)
	sd	s9, -320(s0)
	sd	s10, -328(s0)
	sd	s11, -336(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -20
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	call	getint
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	call	getint
	sw	a0, -64(s0)
	lla	t1, seed
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 52
	sw	a0, -72(s0)
	addi	sp, sp, -16
	li	a0, 52
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
.L_wh_main_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	li	a0, 256
	sw	a0, -56(s0)
	li	a0, 10
	sw	a0, -64(s0)
	li	a0, 1
	sw	a0, -72(s0)
	lw	a0, -28(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -28(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_ifelse_main_3
	lw	a0, -20(s0)
	call	rand
	sw	a0, -48(s0)
	li	a0, 10007
	sw	a0, -128(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	li	t1, 1757988013
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 12
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	sw	a0, -136(s0)
	lw	t2, -40(s0)
	lw	a0, -136(s0)
	subw	a0, t2, a0
	sw	a0, -144(s0)
	addi	t1, s0, -20
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	lw	a0, -144(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	prime_factors
	mv	s1, a0
	sw	s1, -152(s0)
	addi	t1, s0, -32
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	j	.L_ifend_main_2
.L_ifelse_main_3:
	lw	a0, -20(s0)
	call	rand
	sw	a0, -168(s0)
	li	a0, -10007
	sw	a0, -176(s0)
	lw	a0, -168(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, -1757988013
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 12
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, -10007
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -184(s0)
	lw	t2, -160(s0)
	lw	a0, -184(s0)
	addw	a0, t2, a0
	mv	s11, a0
	sw	s11, -192(s0)
	addi	t1, s0, -20
	mv	a0, s11
	sw	a0, 0(t1)
	mv	a0, s11
	sext.w	a0, a0
	mv	a0, t4
	call	prime_factors
	mv	s10, a0
	sw	s10, -200(s0)
	addi	t1, s0, -32
	mv	a0, s10
	sw	a0, 0(t1)
	mv	s10, a0
.L_ifend_main_2:
	lw	a0, -20(s0)
	mv	s9, a0
	sw	s9, -208(s0)
	lw	a0, -32(s0)
	mv	s8, a0
	sw	s8, -216(s0)
	mv	t2, s9
	mv	a0, s8
	addw	a0, t2, a0
	mv	s7, a0
	sw	s7, -224(s0)
	mv	a0, s7
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s6, a0
	sw	s6, -232(s0)
	addi	t1, s0, -20
	mv	a0, s6
	sw	a0, 0(t1)
	mv	a0, s6
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	lw	a0, -24(s0)
	mv	s5, a0
	sw	s5, -240(s0)
	mv	a0, s5
	addiw	a0, a0, 1
	mv	s4, a0
	sw	s4, -248(s0)
	addi	t1, s0, -24
	mv	a0, s4
	sw	a0, 0(t1)
	mv	s4, a0
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 76
	mv	s3, a0
	sw	s3, -40(s0)
	addi	sp, sp, -16
	li	a0, 76
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	li	a0, 0
	mv	s2, a0
	sw	s2, -48(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_main_3
	li	a0, 0
.Lreturn_main_3:
	ld	s11, -336(s0)
	ld	s10, -328(s0)
	ld	s9, -320(s0)
	ld	s8, -312(s0)
	ld	s7, -304(s0)
	ld	s6, -296(s0)
	ld	s5, -288(s0)
	ld	s4, -280(s0)
	ld	s3, -272(s0)
	ld	s2, -264(s0)
	ld	s1, -256(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
