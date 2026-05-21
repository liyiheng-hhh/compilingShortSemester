	.bss
	.align	2
	.globl	a
a:
	.zero	160000
	.bss
	.align	2
	.globl	b
b:
	.zero	160000
	.bss
	.align	2
	.globl	c
c:
	.zero	160000
	.text
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -672
	addi	t0, sp, 672
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -584(s0)
	sd	s2, -592(s0)
	sd	s3, -600(s0)
	sd	s4, -608(s0)
	sd	s5, -616(s0)
	sd	s6, -624(s0)
	sd	s7, -632(s0)
	sd	s8, -640(s0)
	sd	s9, -648(s0)
	sd	s10, -656(s0)
	sd	s11, -664(s0)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -20
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -24
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -28
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -32
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -36
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -112(s0)
	addi	t1, s0, -24
	lw	a0, -112(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 200
	sw	a0, -72(s0)
	li	a0, 800
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	lla	a0, a
	sd	a0, -96(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	beqz	a0, .L_whe_main_1
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -128(s0)
	ld	t2, -96(s0)
	lw	a0, -128(s0)
	add	a0, t2, a0
	sd	a0, -136(s0)
	ld	a0, -136(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -144(s0)
	addi	t1, s0, -20
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	lw	a0, -20(s0)
	addiw	a0, a0, -200
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_ifend_main_2
	lw	a0, -144(s0)
	sext.w	a0, a0
	j	.Lreturn_main_0
.L_ifend_main_2:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -24
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 23
	sw	a0, -72(s0)
	addi	sp, sp, -16
	li	a0, 23
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -40
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -40
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_main_3:
	li	a0, 200
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	li	a0, 32
	sw	a0, -96(s0)
	li	a0, 800
	sw	a0, -104(s0)
	lla	a0, a
	sd	a0, -112(s0)
	lla	a0, b
	sd	a0, -120(s0)
	lw	a0, -40(s0)
	lw	t2, -40(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_main_4
	addi	t1, s0, -44
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_5:
	lw	a0, -40(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, 32
	sw	a0, -160(s0)
	lw	a0, -44(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_main_6
	addi	t1, s0, -28
	lw	a0, -144(s0)
	sw	a0, 0(t1)
.L_wh_main_7:
	lw	a0, -44(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 32
	sw	a0, -192(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_8
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	beqz	a0, .L_ifend_main_9
	j	.L_whe_main_8
.L_ifend_main_9:
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -160(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_ifend_main_10
	j	.L_whe_main_8
.L_ifend_main_10:
	addi	t1, s0, -24
	lw	a0, -144(s0)
	sw	a0, 0(t1)
.L_wh_main_11:
	lw	a0, -28(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -248(s0)
	lw	a0, -248(s0)
	addiw	a0, a0, 0
	sw	a0, -256(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -272(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_12
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	beqz	a0, .L_ifend_main_13
	j	.L_whe_main_12
.L_ifend_main_13:
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -192(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -312(s0)
	lw	a0, -312(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -320(s0)
	lw	a0, -320(s0)
	beqz	a0, .L_ifend_main_14
	j	.L_whe_main_12
.L_ifend_main_14:
	lw	a0, -24(s0)
	slliw	a0, a0, 2
	sw	a0, -336(s0)
	lw	t2, -256(s0)
	lw	a0, -336(s0)
	addw	a0, t2, a0
	sw	a0, -344(s0)
	ld	t2, -112(s0)
	lw	a0, -344(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	ld	t2, -352(s0)
	lw	a0, 0(t2)
	sw	a0, -360(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -376(s0)
	lw	a0, -376(s0)
	addiw	a0, a0, 0
	sw	a0, -384(s0)
	lw	t2, -384(s0)
	lw	a0, -272(s0)
	addw	a0, t2, a0
	sw	a0, -392(s0)
	ld	t2, -120(s0)
	lw	a0, -392(s0)
	add	a0, t2, a0
	sd	a0, -400(s0)
	ld	t1, -400(s0)
	lw	a0, -360(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -416(s0)
	addi	t1, s0, -24
	lw	a0, -416(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_11
.L_whe_main_12:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -256(s0)
	addi	t1, s0, -28
	lw	a0, -256(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_7
.L_whe_main_8:
	lw	a0, -44(s0)
	addiw	a0, a0, 32
	sw	a0, -272(s0)
	addi	t1, s0, -44
	lw	a0, -272(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_5
.L_whe_main_6:
	lw	a0, -40(s0)
	addiw	a0, a0, 32
	sw	a0, -88(s0)
	addi	t1, s0, -40
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_3
.L_whe_main_4:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_15:
	li	a0, 200
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 1
	sw	a0, -88(s0)
	li	a0, 800
	sw	a0, -96(s0)
	li	a0, 2
	sw	a0, -104(s0)
	lla	a0, a
	sd	a0, -112(s0)
	lla	a0, c
	sd	a0, -120(s0)
	lla	a0, b
	sd	a0, -128(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_whe_main_16
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_17:
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	addiw	a0, a0, 0
	sw	a0, -168(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -184(s0)
	lw	a0, -184(s0)
	addiw	a0, a0, 0
	sw	a0, -192(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	beqz	a0, .L_whe_main_18
	addi	t1, s0, -32
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -48
	lw	a0, -32(s0)
	sw	a0, 0(t1)
.L_wh_main_19:
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -224(s0)
	lw	a0, -32(s0)
	lw	t2, -32(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_main_20
	lw	a0, -32(s0)
	slliw	a0, a0, 2
	sw	a0, -256(s0)
	lw	t2, -168(s0)
	lw	a0, -256(s0)
	addw	a0, t2, a0
	sw	a0, -264(s0)
	ld	t2, -112(s0)
	lw	a0, -264(s0)
	add	a0, t2, a0
	sd	a0, -272(s0)
	ld	t2, -272(s0)
	lw	a0, 0(t2)
	sw	a0, -280(s0)
	lw	a0, -32(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	addiw	a0, a0, 0
	sw	a0, -304(s0)
	lw	t2, -304(s0)
	lw	a0, -224(s0)
	addw	a0, t2, a0
	sw	a0, -312(s0)
	ld	t2, -128(s0)
	lw	a0, -312(s0)
	add	a0, t2, a0
	sd	a0, -320(s0)
	ld	t2, -320(s0)
	lw	a0, 0(t2)
	sw	a0, -328(s0)
	lw	t2, -280(s0)
	lw	a0, -328(s0)
	mulw	a0, t2, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -344(s0)
	lw	a0, -344(s0)
	addiw	a0, a0, 0
	sw	a0, -352(s0)
	lw	a0, -352(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -360(s0)
	lw	a0, -360(s0)
	beqz	a0, .L_ifend_main_21
	li	a0, 800
	sw	a0, -224(s0)
	lla	a0, b
	sd	a0, -368(s0)
	lla	a0, a
	sd	a0, -376(s0)
	lw	a0, -48(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -400(s0)
	lw	a0, -32(s0)
	slliw	a0, a0, 2
	sw	a0, -416(s0)
	lw	t2, -400(s0)
	lw	a0, -416(s0)
	addw	a0, t2, a0
	sw	a0, -424(s0)
	ld	t2, -368(s0)
	lw	a0, -424(s0)
	add	a0, t2, a0
	sd	a0, -432(s0)
	ld	t2, -432(s0)
	lw	a0, 0(t2)
	sw	a0, -440(s0)
	lw	a0, -32(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -456(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -472(s0)
	lw	t2, -456(s0)
	lw	a0, -472(s0)
	addw	a0, t2, a0
	sw	a0, -480(s0)
	ld	t2, -376(s0)
	lw	a0, -480(s0)
	add	a0, t2, a0
	sd	a0, -488(s0)
	ld	t2, -488(s0)
	lw	a0, 0(t2)
	sw	a0, -496(s0)
	lw	t2, -440(s0)
	lw	a0, -496(s0)
	mulw	a0, t2, a0
	sw	a0, -504(s0)
	lw	t2, -48(s0)
	lw	a0, -504(s0)
	addw	a0, t2, a0
	sw	a0, -512(s0)
	addi	t1, s0, -48
	lw	a0, -512(s0)
	sw	a0, 0(t1)
.L_ifend_main_21:
	lw	a0, -32(s0)
	addiw	a0, a0, 1
	sw	a0, -528(s0)
	addi	t1, s0, -32
	lw	a0, -528(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_19
.L_whe_main_20:
	lw	a0, -48(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -544(s0)
	lw	t2, -192(s0)
	lw	a0, -544(s0)
	addw	a0, t2, a0
	sw	a0, -552(s0)
	ld	t2, -120(s0)
	lw	a0, -552(s0)
	add	a0, t2, a0
	sd	a0, -560(s0)
	ld	t1, -560(s0)
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -576(s0)
	addi	t1, s0, -28
	lw	a0, -576(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_17
.L_whe_main_18:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -24
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_15
.L_whe_main_16:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_22:
	li	a0, 200
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 2147483647
	sw	a0, -88(s0)
	li	a0, 800
	sw	a0, -96(s0)
	li	a0, 1
	sw	a0, -104(s0)
	lla	a0, c
	sd	a0, -112(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_whe_main_23
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -52
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_main_24:
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_main_25
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -176(s0)
	lw	t2, -144(s0)
	lw	a0, -176(s0)
	addw	a0, t2, a0
	sw	a0, -184(s0)
	ld	t2, -112(s0)
	lw	a0, -184(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	ld	t2, -192(s0)
	lw	a0, 0(t2)
	sw	a0, -200(s0)
	lw	a0, -52(s0)
	lw	t2, -200(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	beqz	a0, .L_ifend_main_26
	li	a0, 800
	sw	a0, -144(s0)
	lla	a0, c
	sd	a0, -224(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -256(s0)
	lw	t2, -240(s0)
	lw	a0, -256(s0)
	addw	a0, t2, a0
	sw	a0, -264(s0)
	ld	t2, -224(s0)
	lw	a0, -264(s0)
	add	a0, t2, a0
	sd	a0, -272(s0)
	ld	t2, -272(s0)
	lw	a0, 0(t2)
	sw	a0, -280(s0)
	addi	t1, s0, -52
	lw	a0, -280(s0)
	sw	a0, 0(t1)
.L_ifend_main_26:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -296(s0)
	addi	t1, s0, -28
	lw	a0, -296(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_24
.L_whe_main_25:
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_27:
	lw	a0, -52(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -320(s0)
	lw	a0, -320(s0)
	addiw	a0, a0, 0
	sw	a0, -328(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -344(s0)
	lw	a0, -344(s0)
	beqz	a0, .L_whe_main_28
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -360(s0)
	lw	t2, -328(s0)
	lw	a0, -360(s0)
	addw	a0, t2, a0
	sw	a0, -368(s0)
	ld	t2, -112(s0)
	lw	a0, -368(s0)
	add	a0, t2, a0
	sd	a0, -376(s0)
	ld	t1, -376(s0)
	lw	a0, -52(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -392(s0)
	addi	t1, s0, -28
	lw	a0, -392(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_27
.L_whe_main_28:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -24
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_22
.L_whe_main_23:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_29:
	li	a0, 200
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 2147483647
	sw	a0, -88(s0)
	li	a0, 800
	sw	a0, -96(s0)
	li	a0, 1
	sw	a0, -104(s0)
	lla	a0, c
	sd	a0, -112(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_whe_main_30
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -56
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_main_31:
	lw	a0, -24(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	addiw	a0, a0, 0
	sw	a0, -160(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_main_32
	lw	a0, -28(s0)
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	addiw	a0, a0, 0
	sw	a0, -200(s0)
	lw	t2, -200(s0)
	lw	a0, -136(s0)
	addw	a0, t2, a0
	sw	a0, -208(s0)
	ld	t2, -112(s0)
	lw	a0, -208(s0)
	add	a0, t2, a0
	sd	a0, -216(s0)
	ld	t2, -216(s0)
	lw	a0, 0(t2)
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	negw	a0, a0
	sw	a0, -232(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -248(s0)
	lw	t2, -160(s0)
	lw	a0, -248(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	ld	t2, -112(s0)
	lw	a0, -256(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	ld	t1, -264(s0)
	lw	a0, -232(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -280(s0)
	addi	t1, s0, -28
	lw	a0, -280(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_31
.L_whe_main_32:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -24
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_29
.L_whe_main_30:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_33:
	li	a0, 200
	mv	s3, a0
	sw	s3, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 2147483647
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	li	a0, 800
	sw	a0, -104(s0)
	lla	a0, c
	sd	a0, -112(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -120(s0)
	mv	t2, s2
	sext.w	t2, t2
	mv	a0, s3
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s4, a0
	sw	s4, -128(s0)
	mv	a0, s4
	beqz	a0, .L_whe_main_34
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_main_35:
	lw	a0, -24(s0)
	mv	s8, a0
	sw	s8, -88(s0)
	mv	a0, s8
	mv	t2, a0
	li	a0, 800
	mulw	a0, t2, a0
	mv	s9, a0
	sw	s9, -136(s0)
	mv	a0, s9
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	lw	a0, -28(s0)
	mv	s5, a0
	sw	s5, -152(s0)
	mv	t2, s5
	sext.w	t2, t2
	mv	a0, s3
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s6, a0
	sw	s6, -160(s0)
	mv	a0, s6
	beqz	a0, .L_whe_main_36
	lw	a0, -36(s0)
	mv	s7, a0
	sw	s7, -168(s0)
	lw	a0, -28(s0)
	mv	s10, a0
	sw	s10, -176(s0)
	mv	a0, s10
	slliw	a0, a0, 2
	mv	s11, a0
	sw	s11, -184(s0)
	lw	t2, -144(s0)
	mv	a0, s11
	addw	a0, t2, a0
	sw	a0, -192(s0)
	ld	t2, -112(s0)
	lw	a0, -192(s0)
	add	a0, t2, a0
	sd	a0, -200(s0)
	ld	t2, -200(s0)
	lw	a0, 0(t2)
	sw	a0, -208(s0)
	mv	t2, s7
	lw	a0, -208(s0)
	addw	a0, t2, a0
	sw	a0, -216(s0)
	addi	t1, s0, -36
	lw	a0, -216(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -232(s0)
	addi	t1, s0, -28
	lw	a0, -232(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_35
.L_whe_main_36:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -24
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_33
.L_whe_main_34:
	li	a0, 93
	sw	a0, -72(s0)
	addi	sp, sp, -16
	li	a0, 93
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -36(s0)
	lw	a0, -80(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 0
	mv	s1, a0
	sw	s1, -88(s0)
	mv	a0, s1
	sext.w	a0, a0
	j	.Lreturn_main_0
	li	a0, 0
.Lreturn_main_0:
	ld	s11, -664(s0)
	ld	s10, -656(s0)
	ld	s9, -648(s0)
	ld	s8, -640(s0)
	ld	s7, -632(s0)
	ld	s6, -624(s0)
	ld	s5, -616(s0)
	ld	s4, -608(s0)
	ld	s3, -600(s0)
	ld	s2, -592(s0)
	ld	s1, -584(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
