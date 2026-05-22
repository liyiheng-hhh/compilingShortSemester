	.bss
	.align	2
	.globl	crc32table
crc32table:
	.zero	1024
	.bss
	.align	2
	.globl	a
a:
	.zero	400080
	.text
	.text
	.align	1
	.globl	_and
	.type	_and, @function
_and:
	addi	sp, sp, -432
	li	t0, 432
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh__and_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	sw	a0, -80(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe__and_1
	mv	a0, t4
	sw	a0, -96(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -104(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	mv	a0, t5
	sw	a0, -112(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -120(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -128(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	mv	a0, t5
	sw	a0, -144(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -152(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	mv	t5, a0
	lw	a0, -104(s0)
	addiw	a0, a0, -1
	sw	a0, -160(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	beqz	a0, .L_ifend__and_2
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	li	a0, 1
	sw	a0, -184(s0)
	lw	a0, -176(s0)
	addiw	a0, a0, -1
	sw	a0, -192(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_ifend__and_3
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	lw	t2, -208(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_ifend__and_3:
.L_ifend__and_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	a0, a0, 1
	sw	a0, -240(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addiw	a0, a0, -1
	sw	a0, -256(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh__and_0
.L_whe__and_1:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn__and_0
	li	a0, 0
.Lreturn__and_0:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_and, .-_and
	.text
	.align	1
	.globl	_xor
	.type	_xor, @function
_xor:
	addi	sp, sp, -352
	li	t0, 352
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh__xor_0:
	li	a0, 2
	sw	a0, -56(s0)
	sw	a0, -64(s0)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe__xor_1
	mv	a0, t4
	sw	a0, -88(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -96(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	mv	a0, t5
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
	addi	t1, s0, -32
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -120(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	mv	t4, a0
	mv	a0, t5
	sw	a0, -136(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	mv	t5, a0
	lw	t2, -96(s0)
	lw	a0, -112(s0)
	subw	a0, t2, a0
	sw	a0, -152(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_ifend__xor_2
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	lw	t2, -168(s0)
	addw	a0, t2, a0
	sw	a0, -184(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_ifend__xor_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	slliw	a0, a0, 1
	sw	a0, -200(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addiw	a0, a0, -1
	sw	a0, -216(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh__xor_0
.L_whe__xor_1:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn__xor_1
	li	a0, 0
.Lreturn__xor_1:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	_xor, .-_xor
	.text
	.align	1
	.globl	rotr8
	.type	rotr8, @function
rotr8:
	addi	sp, sp, -64
	li	t0, 64
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	mv	t4, a0
	mv	a0, t4
	sw	a0, -40(s0)
	li	a0, 256
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_rotr8_2
	li	a0, 0
.Lreturn_rotr8_2:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	rotr8, .-rotr8
	.text
	.align	1
	.globl	crc32
	.type	crc32, @function
crc32:
	addi	sp, sp, -304
	li	t0, 304
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sw	a2, -36(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh_crc32_0:
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 255
	sw	a0, -64(s0)
	ld	a0, -32(s0)
	sd	a0, -72(s0)
	lla	a0, crc32table
	sd	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_crc32_1
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	mv	t4, a0
	li	a0, 255
	sext.w	a0, a0
	and	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -120(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -152(s0)
	lw	a0, -120(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -152(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	xor	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -160(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	rotr8
	sw	a0, -176(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	slliw	a0, a0, 2
	sw	a0, -192(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -200(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -208(s0)
	lw	a0, -176(s0)
	sext.w	a0, a0
	mv	t4, a0
	lw	a0, -208(s0)
	sext.w	a0, a0
	sext.w	a0, a0
	xor	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -216(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	addiw	a0, a0, 1
	sw	a0, -232(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_crc32_0
.L_whe_crc32_1:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_crc32_3
	li	a0, 0
.Lreturn_crc32_3:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	crc32, .-crc32
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -224
	li	t0, 224
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	lla	a0, a
	sd	a0, -40(s0)
	ld	a0, -40(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -48(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_main_0:
	lla	a0, crc32table
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
	li	a0, 256
	sw	a0, -48(s0)
	li	a0, 19260817
	sw	a0, -64(s0)
	ld	a0, -40(s0)
	sd	a0, -40(s0)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t0, s0, -32
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
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	t2, -64(s0)
	addw	a0, t2, a0
	sw	a0, -104(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	mv	t1, a0
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 68
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 68
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
.L_wh_main_2:
	lla	a0, a
	sd	a0, -40(s0)
	li	a0, 19260817
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	beqz	a0, .L_whe_main_3
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addi	sp, sp, -32
	li	a0, 0
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -40(s0)
	sd	a0, 8(sp)
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	crc32
	addi	sp, sp, 32
	sw	a0, -88(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1870574702
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 23
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 19260817
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -96(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	addiw	a0, a0, -1
	sw	a0, -112(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	li	a0, 73
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 73
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	addi	t0, s0, -28
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
	j	.Lreturn_main_4
	li	a0, 0
.Lreturn_main_4:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
