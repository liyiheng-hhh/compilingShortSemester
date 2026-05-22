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
	addi	sp, sp, -112
	li	t0, 112
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	lla	t0, seed
	lw	a0, 0(t0)
	sw	a0, -24(s0)
	li	a0, 19980130
	sw	a0, -32(s0)
	lw	a0, -24(s0)
	slliw	t2, a0, 1
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 11
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 20
	addw	t2, t2, t1
	slliw	t1, a0, 21
	addw	t2, t2, t1
	slliw	t1, a0, 24
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -40(s0)
	li	a0, 23333
	sw	a0, -48(s0)
	lw	t2, -40(s0)
	addw	a0, t2, a0
	sw	a0, -56(s0)
	li	a0, 100000007
	sw	a0, -64(s0)
	lw	a0, -56(s0)
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
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_ifend_rand_0
	lla	t0, seed
	lw	a0, 0(t0)
	sw	a0, -24(s0)
	li	a0, 100000007
	sw	a0, -32(s0)
	lw	t2, -24(s0)
	addw	a0, t2, a0
	sw	a0, -40(s0)
	lla	t1, seed
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
	addi	sp, sp, -128
	li	t0, 128
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
.L_wh_gcd_0:
	li	a0, 0
	sw	a0, -40(s0)
	mv	a0, t5
	sw	a0, -48(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_whe_gcd_1
	mv	a0, t5
	sw	a0, -64(s0)
	mv	a0, t4
	sw	a0, -72(s0)
	mv	a0, t5
	sw	a0, -80(s0)
	lw	t2, -72(s0)
	remw	a0, t2, a0
	sw	a0, -88(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	mv	t5, a0
	addi	t1, s0, -20
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	mv	t4, a0
	j	.L_wh_gcd_0
.L_whe_gcd_1:
	mv	a0, t4
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_gcd_1
	li	a0, 0
.Lreturn_gcd_1:
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
	addi	sp, sp, -288
	li	t0, 288
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	mv	t4, a0
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -48(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_prime_factors_0:
	li	a0, 2
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	mv	a0, t4
	sw	a0, -56(s0)
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
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	beqz	a0, .L_whe_prime_factors_1
	mv	a0, t4
	sw	a0, -80(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -24
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
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	t2, -64(s0)
	mulw	a0, t2, a0
	sw	a0, -80(s0)
	mv	a0, t4
	sw	a0, -88(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -80(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_prime_factors_3
.L_wh_prime_factors_4:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	mv	a0, t4
	sw	a0, -128(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	remw	a0, t2, a0
	sw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_prime_factors_5
	mv	a0, t4
	sw	a0, -160(s0)
	mv	t2, a0
	lw	a0, -120(s0)
	divw	a0, t2, a0
	sw	a0, -168(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_prime_factors_4
.L_whe_prime_factors_5:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	addiw	a0, a0, 2
	sw	a0, -48(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_prime_factors_2
.L_whe_prime_factors_3:
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -40(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	beqz	a0, .L_ifend_prime_factors_6
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_ifend_prime_factors_6:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_prime_factors_2
	li	a0, 0
.Lreturn_prime_factors_2:
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
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	call	getint
	sw	a0, -56(s0)
	call	getint
	sw	a0, -64(s0)
	lla	t1, seed
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
	sw	a0, -64(s0)
	li	a0, 10
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -104(s0)
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
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_ifelse_main_3
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -40(s0)
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
	subw	a0, t2, a0
	sw	a0, -144(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	lw	a0, -144(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	prime_factors
	sw	a0, -152(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_ifend_main_2
.L_ifelse_main_3:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -160(s0)
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
	addw	a0, t2, a0
	sw	a0, -192(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	lw	a0, -192(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	prime_factors
	sw	a0, -200(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_ifend_main_2:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	lw	t2, -208(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	li	t1, 256
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -232(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	lw	a0, -232(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	addiw	a0, a0, 1
	sw	a0, -248(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 76
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 76
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
