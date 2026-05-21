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
	addi	sp, sp, -288
	addi	t0, sp, 288
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -240(s0)
	sd	s2, -248(s0)
	sd	s3, -256(s0)
	sd	s4, -264(s0)
	sd	s5, -272(s0)
	sd	s6, -280(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh__and_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	beqz	a0, .L_whe__and_1
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
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
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
	sw	a0, -104(s0)
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -20
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	mv	s1, a0
	sw	s1, -136(s0)
	addi	t1, s0, -24
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_ifend__and_2
	li	a0, 1
	sw	a0, -160(s0)
	lw	a0, -32(s0)
	addiw	a0, a0, -1
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_ifend__and_3
	lw	a0, -40(s0)
	lw	a0, -44(s0)
	lw	t2, -40(s0)
	addw	a0, t2, a0
	sw	a0, -200(s0)
	addi	t1, s0, -40
	lw	a0, -200(s0)
	sw	a0, 0(t1)
.L_ifend__and_3:
.L_ifend__and_2:
	lw	a0, -44(s0)
	mv	s6, a0
	sw	s6, -208(s0)
	mv	a0, s6
	slliw	a0, a0, 1
	mv	s5, a0
	sw	s5, -216(s0)
	addi	t1, s0, -44
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -36(s0)
	mv	s4, a0
	sw	s4, -224(s0)
	mv	a0, s4
	addiw	a0, a0, -1
	mv	s3, a0
	sw	s3, -232(s0)
	addi	t1, s0, -36
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh__and_0
.L_whe__and_1:
	lw	a0, -40(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn__and_0
	li	a0, 0
.Lreturn__and_0:
	ld	s6, -280(s0)
	ld	s5, -272(s0)
	ld	s4, -264(s0)
	ld	s3, -256(s0)
	ld	s2, -248(s0)
	ld	s1, -240(s0)
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
	sw	a1, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 32
	sw	a0, -72(s0)
	addi	t1, s0, -36
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh__xor_0:
	li	a0, 2
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	beqz	a0, .L_whe__xor_1
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
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
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
	sw	a0, -104(s0)
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -20
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	mv	s1, a0
	sw	s1, -128(s0)
	mv	a0, s1
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -24
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	lw	t2, -28(s0)
	lw	a0, -32(s0)
	subw	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_ifend__xor_2
	lw	a0, -40(s0)
	lw	a0, -44(s0)
	lw	t2, -40(s0)
	addw	a0, t2, a0
	sw	a0, -176(s0)
	addi	t1, s0, -40
	lw	a0, -176(s0)
	sw	a0, 0(t1)
.L_ifend__xor_2:
	lw	a0, -44(s0)
	mv	s6, a0
	sw	s6, -184(s0)
	mv	a0, s6
	slliw	a0, a0, 1
	mv	s5, a0
	sw	s5, -192(s0)
	addi	t1, s0, -44
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -36(s0)
	mv	s4, a0
	sw	s4, -200(s0)
	mv	a0, s4
	addiw	a0, a0, -1
	mv	s3, a0
	sw	s3, -208(s0)
	addi	t1, s0, -36
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh__xor_0
.L_whe__xor_1:
	lw	a0, -40(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn__xor_1
	li	a0, 0
.Lreturn__xor_1:
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
	.size	_xor, .-_xor
	.text
	.align	1
	.globl	rotr8
	.type	rotr8, @function
rotr8:
	addi	sp, sp, -96
	addi	t0, sp, 96
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -64(s0)
	sd	s2, -72(s0)
	sd	s3, -80(s0)
	sw	a0, -20(s0)
	li	a0, 256
	mv	s2, a0
	sw	s2, -40(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -48(s0)
	mv	a0, s1
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 255
	addw	a0, a0, t1
	sraiw	a0, a0, 8
	mv	s3, a0
	sw	s3, -56(s0)
	mv	a0, s3
	sext.w	a0, a0
	j	.Lreturn_rotr8_2
	li	a0, 0
.Lreturn_rotr8_2:
	ld	s3, -80(s0)
	ld	s2, -72(s0)
	ld	s1, -64(s0)
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
	addi	sp, sp, -336
	addi	t0, sp, 336
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -240(s0)
	sd	s2, -248(s0)
	sd	s3, -256(s0)
	sd	s4, -264(s0)
	sd	s5, -272(s0)
	sd	s6, -280(s0)
	sd	s7, -288(s0)
	sd	s8, -296(s0)
	sd	s9, -304(s0)
	sd	s10, -312(s0)
	sd	s11, -320(s0)
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sw	a2, -36(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -40
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -44
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_crc32_0:
	li	a0, 255
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	ld	a0, -32(s0)
	sd	a0, -72(s0)
	lla	a0, crc32table
	sd	a0, -80(s0)
	lw	a0, -36(s0)
	lw	a0, -40(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	lw	a0, -36(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_crc32_1
	lw	a0, -20(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	mv	t4, a0
	li	a0, 255
	sext.w	a0, a0
	and	a0, t4, a0
	sext.w	a0, a0
	sw	a0, -120(s0)
	lw	a0, -40(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	ld	t2, -72(s0)
	lw	a0, -136(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	ld	t2, -144(s0)
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
	mv	s1, a0
	sw	s1, -160(s0)
	addi	t1, s0, -44
	mv	a0, s1
	sw	a0, 0(t1)
	mv	s1, a0
	lw	a0, -20(s0)
	mv	s11, a0
	sw	s11, -168(s0)
	mv	a0, s11
	sext.w	a0, a0
	mv	a0, t4
	call	rotr8
	mv	s10, a0
	sw	s10, -176(s0)
	lw	a0, -44(s0)
	mv	s9, a0
	sw	s9, -184(s0)
	mv	a0, s9
	slliw	a0, a0, 2
	mv	s8, a0
	sw	s8, -192(s0)
	ld	t2, -80(s0)
	mv	a0, s8
	add	a0, t2, a0
	mv	s7, a0
	sd	s7, -200(s0)
	mv	t2, s7
	lw	a0, 0(t2)
	mv	s6, a0
	sw	s6, -208(s0)
	mv	a0, s10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, s6
	sext.w	a0, a0
	sext.w	a0, a0
	xor	a0, t4, a0
	sext.w	a0, a0
	mv	s5, a0
	sw	s5, -216(s0)
	addi	t1, s0, -20
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -40(s0)
	mv	s4, a0
	sw	s4, -224(s0)
	mv	a0, s4
	addiw	a0, a0, 1
	mv	s3, a0
	sw	s3, -232(s0)
	addi	t1, s0, -40
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh_crc32_0
.L_whe_crc32_1:
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -56(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_crc32_3
	li	a0, 0
.Lreturn_crc32_3:
	ld	s11, -320(s0)
	ld	s10, -312(s0)
	ld	s9, -304(s0)
	ld	s8, -296(s0)
	ld	s7, -288(s0)
	ld	s6, -280(s0)
	ld	s5, -272(s0)
	ld	s4, -264(s0)
	ld	s3, -256(s0)
	ld	s2, -248(s0)
	ld	s1, -240(s0)
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
	addi	sp, sp, -240
	addi	t0, sp, 240
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -144(s0)
	sd	s2, -152(s0)
	sd	s3, -160(s0)
	sd	s4, -168(s0)
	sd	s5, -176(s0)
	sd	s6, -184(s0)
	sd	s7, -192(s0)
	sd	s8, -200(s0)
	sd	s9, -208(s0)
	sd	s10, -216(s0)
	sd	s11, -224(s0)
	lla	a0, a
	sd	a0, -40(s0)
	ld	a0, -40(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -48(s0)
	addi	t1, s0, -20
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -32
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 256
	mv	s11, a0
	sw	s11, -40(s0)
	li	a0, 19260817
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lla	a0, crc32table
	sd	a0, -64(s0)
	lw	a0, -32(s0)
	lw	t2, -32(s0)
	sext.w	t2, t2
	mv	a0, s11
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_main_1
	lw	a0, -32(s0)
	lw	t2, -48(s0)
	addw	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -32(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -64(s0)
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	ld	t1, -120(s0)
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	lw	a0, -32(s0)
	addiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -32
	lw	a0, -136(s0)
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
	li	a0, 19260817
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	a0, a
	mv	s11, a0
	sd	s11, -56(s0)
	lw	a0, -24(s0)
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -72(s0)
	mv	a0, s4
	beqz	a0, .L_whe_main_3
	lw	a0, -20(s0)
	mv	s2, a0
	sw	s2, -80(s0)
	addi	sp, sp, -32
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s11
	sd	a0, 8(sp)
	mv	a0, s2
	sext.w	a0, a0
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	crc32
	addi	sp, sp, 32
	mv	s3, a0
	sw	s3, -88(s0)
	mv	a0, s3
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
	mv	s5, a0
	sw	s5, -96(s0)
	addi	t1, s0, -28
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -104(s0)
	mv	a0, s1
	addiw	a0, a0, -1
	mv	s6, a0
	sw	s6, -112(s0)
	addi	t1, s0, -20
	mv	a0, s6
	sw	a0, 0(t1)
	mv	s6, a0
	j	.L_wh_main_2
.L_whe_main_3:
	li	a0, 73
	mv	s7, a0
	sw	s7, -40(s0)
	addi	sp, sp, -16
	li	a0, 73
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -28(s0)
	mv	s8, a0
	sw	s8, -48(s0)
	mv	a0, s8
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	mv	s9, a0
	sw	s9, -56(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	mv	s10, a0
	sw	s10, -64(s0)
	mv	a0, s10
	sext.w	a0, a0
	j	.Lreturn_main_4
	li	a0, 0
.Lreturn_main_4:
	ld	s11, -224(s0)
	ld	s10, -216(s0)
	ld	s9, -208(s0)
	ld	s8, -200(s0)
	ld	s7, -192(s0)
	ld	s6, -184(s0)
	ld	s5, -176(s0)
	ld	s4, -168(s0)
	ld	s3, -160(s0)
	ld	s2, -152(s0)
	ld	s1, -144(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
