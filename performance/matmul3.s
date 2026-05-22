	.bss
	.align	2
	.globl	a
a:
	.zero	250000
	.bss
	.align	2
	.globl	b
b:
	.zero	250000
	.bss
	.align	2
	.globl	c
c:
	.zero	250000
	.text
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -1840
	li	t0, 1840
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -40
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -112(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 250
	sw	a0, -72(s0)
	lla	a0, a
	sd	a0, -80(s0)
	li	a0, 1000
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	beqz	a0, .L_whe_main_1
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -128(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -136(s0)
	ld	a0, -136(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -144(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	addiw	a0, a0, -250
	sw	a0, -152(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_ifend_main_2
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	sext.w	a0, a0
	j	.Lreturn_main_0
.L_ifend_main_2:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addiw	a0, a0, 1
	sw	a0, -168(s0)
	addi	t1, s0, -24
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
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_wh_main_3:
	li	a0, 250
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	lw	a0, -72(s0)
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	lw	a0, -72(s0)
	sw	a0, -104(s0)
	li	a0, 32
	sw	a0, -112(s0)
	sw	a0, -120(s0)
	lw	a0, -112(s0)
	sw	a0, -128(s0)
	lw	a0, -96(s0)
	sw	a0, -136(s0)
	lw	a0, -72(s0)
	sw	a0, -144(s0)
	lw	a0, -96(s0)
	sw	a0, -152(s0)
	lw	a0, -112(s0)
	sw	a0, -160(s0)
	lla	a0, a
	sd	a0, -168(s0)
	lw	a0, -80(s0)
	sw	a0, -176(s0)
	li	a0, 1000
	sw	a0, -184(s0)
	lla	a0, b
	sd	a0, -192(s0)
	lw	a0, -80(s0)
	sw	a0, -200(s0)
	lw	a0, -184(s0)
	sw	a0, -208(s0)
	lw	a0, -96(s0)
	sw	a0, -216(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	beqz	a0, .L_whe_main_4
	addi	t1, s0, -44
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_5:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addiw	a0, a0, 32
	sw	a0, -256(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -264(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -272(s0)
	lw	a0, -272(s0)
	beqz	a0, .L_whe_main_6
	addi	t1, s0, -28
	lw	a0, -240(s0)
	sw	a0, 0(t1)
.L_wh_main_7:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	addiw	a0, a0, 32
	sw	a0, -288(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_main_8
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -304(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -312(s0)
	lw	a0, -312(s0)
	beqz	a0, .L_ifend_main_9
	j	.L_whe_main_8
.L_ifend_main_9:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -256(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -328(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	beqz	a0, .L_ifend_main_10
	j	.L_whe_main_8
.L_ifend_main_10:
	addi	t1, s0, -24
	lw	a0, -240(s0)
	sw	a0, 0(t1)
.L_wh_main_11:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -344(s0)
	addiw	a0, a0, 0
	sw	a0, -352(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -360(s0)
	slliw	a0, a0, 2
	sw	a0, -368(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_main_12
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -144(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -384(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -392(s0)
	lw	a0, -392(s0)
	beqz	a0, .L_ifend_main_13
	j	.L_whe_main_12
.L_ifend_main_13:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -400(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -288(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -408(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -416(s0)
	lw	a0, -416(s0)
	beqz	a0, .L_ifend_main_14
	j	.L_whe_main_12
.L_ifend_main_14:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -424(s0)
	slliw	a0, a0, 2
	sw	a0, -432(s0)
	lw	t2, -352(s0)
	addw	a0, t2, a0
	sw	a0, -440(s0)
	ld	t2, -168(s0)
	add	a0, t2, a0
	sd	a0, -448(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -456(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -464(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -472(s0)
	addiw	a0, a0, 0
	sw	a0, -480(s0)
	mv	t2, a0
	lw	a0, -368(s0)
	addw	a0, t2, a0
	sw	a0, -488(s0)
	ld	t2, -192(s0)
	add	a0, t2, a0
	sd	a0, -496(s0)
	mv	t1, a0
	lw	a0, -456(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -504(s0)
	addiw	a0, a0, 1
	sw	a0, -512(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_11
.L_whe_main_12:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	addiw	a0, a0, 1
	sw	a0, -352(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_7
.L_whe_main_8:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	addiw	a0, a0, 32
	sw	a0, -368(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	j	.L_wh_main_5
.L_whe_main_6:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addiw	a0, a0, 32
	sw	a0, -104(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_main_3
.L_whe_main_4:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_wh_main_15:
	li	a0, 250
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	lw	a0, -72(s0)
	sw	a0, -88(s0)
	lw	a0, -80(s0)
	sw	a0, -96(s0)
	lw	a0, -80(s0)
	sw	a0, -104(s0)
	lw	a0, -72(s0)
	sw	a0, -112(s0)
	li	a0, 1
	sw	a0, -120(s0)
	lla	a0, a
	sd	a0, -128(s0)
	lw	a0, -80(s0)
	sw	a0, -136(s0)
	lla	a0, c
	sd	a0, -144(s0)
	lw	a0, -80(s0)
	sw	a0, -152(s0)
	li	a0, 1000
	sw	a0, -160(s0)
	lw	a0, -120(s0)
	sw	a0, -168(s0)
	lw	a0, -160(s0)
	sw	a0, -176(s0)
	lla	a0, b
	sd	a0, -184(s0)
	lw	a0, -80(s0)
	sw	a0, -192(s0)
	lw	a0, -160(s0)
	sw	a0, -200(s0)
	li	a0, 2
	sw	a0, -208(s0)
	lw	a0, -80(s0)
	sw	a0, -216(s0)
	lw	a0, -120(s0)
	sw	a0, -224(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_main_16
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_17:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -264(s0)
	addiw	a0, a0, 0
	sw	a0, -272(s0)
	lw	a0, -248(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -280(s0)
	addiw	a0, a0, 0
	sw	a0, -288(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -304(s0)
	lw	a0, -304(s0)
	beqz	a0, .L_whe_main_18
	addi	t1, s0, -32
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -48
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_main_19:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	slliw	a0, a0, 2
	sw	a0, -320(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	beqz	a0, .L_whe_main_20
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	slliw	a0, a0, 2
	sw	a0, -352(s0)
	lw	t2, -288(s0)
	addw	a0, t2, a0
	sw	a0, -360(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -368(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -376(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -384(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -392(s0)
	addiw	a0, a0, 0
	sw	a0, -400(s0)
	mv	t2, a0
	lw	a0, -320(s0)
	addw	a0, t2, a0
	sw	a0, -408(s0)
	ld	t2, -184(s0)
	add	a0, t2, a0
	sd	a0, -416(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -424(s0)
	lw	t2, -376(s0)
	mulw	a0, t2, a0
	sw	a0, -432(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	li	t1, 2
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -440(s0)
	addiw	a0, a0, 0
	sw	a0, -448(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -456(s0)
	lw	a0, -456(s0)
	beqz	a0, .L_ifend_main_21
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	lla	a0, b
	sd	a0, -464(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -472(s0)
	li	a0, 1000
	sw	a0, -480(s0)
	lw	a0, -472(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -488(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -496(s0)
	slliw	a0, a0, 2
	sw	a0, -504(s0)
	lw	t2, -488(s0)
	addw	a0, t2, a0
	sw	a0, -512(s0)
	ld	t2, -464(s0)
	add	a0, t2, a0
	sd	a0, -520(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -528(s0)
	lla	a0, a
	sd	a0, -536(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -544(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -552(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -560(s0)
	slliw	a0, a0, 2
	sw	a0, -568(s0)
	lw	t2, -552(s0)
	addw	a0, t2, a0
	sw	a0, -576(s0)
	ld	t2, -536(s0)
	add	a0, t2, a0
	sd	a0, -584(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -592(s0)
	lw	t2, -528(s0)
	mulw	a0, t2, a0
	sw	a0, -600(s0)
	lw	t2, -320(s0)
	addw	a0, t2, a0
	sw	a0, -608(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
.L_ifend_main_21:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -616(s0)
	addiw	a0, a0, 1
	sw	a0, -624(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_main_19
.L_whe_main_20:
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -632(s0)
	slliw	a0, a0, 2
	sw	a0, -640(s0)
	lw	t2, -272(s0)
	addw	a0, t2, a0
	sw	a0, -648(s0)
	ld	t2, -144(s0)
	add	a0, t2, a0
	sd	a0, -656(s0)
	mv	t1, a0
	lw	a0, -288(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -664(s0)
	addiw	a0, a0, 1
	sw	a0, -672(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_17
.L_whe_main_18:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_15
.L_whe_main_16:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_wh_main_22:
	li	a0, 250
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 2147483647
	sw	a0, -88(s0)
	lw	a0, -72(s0)
	sw	a0, -96(s0)
	lla	a0, c
	sd	a0, -104(s0)
	lw	a0, -80(s0)
	sw	a0, -112(s0)
	lw	a0, -80(s0)
	sw	a0, -120(s0)
	ld	a0, -104(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	sd	a0, -128(s0)
	lw	a0, -72(s0)
	sw	a0, -136(s0)
	ld	a0, -128(s0)
	sd	a0, -128(s0)
	lw	a0, -80(s0)
	sw	a0, -144(s0)
	li	a0, 1000
	sw	a0, -152(s0)
	li	a0, 1
	sw	a0, -160(s0)
	sw	a0, -168(s0)
	lw	a0, -152(s0)
	sw	a0, -176(s0)
	lw	a0, -160(s0)
	sw	a0, -184(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_whe_main_23
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -52
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_main_24:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -208(s0)
	addiw	a0, a0, 0
	sw	a0, -216(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	beqz	a0, .L_whe_main_25
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	slliw	a0, a0, 2
	sw	a0, -248(s0)
	lw	t2, -216(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	ld	t2, -104(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -272(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	lw	t2, -272(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	beqz	a0, .L_ifend_main_26
	lla	a0, c
	sd	a0, -216(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	li	a0, 1000
	sw	a0, -304(s0)
	lw	a0, -296(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -312(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	slliw	a0, a0, 2
	sw	a0, -328(s0)
	lw	t2, -312(s0)
	addw	a0, t2, a0
	sw	a0, -336(s0)
	ld	t2, -216(s0)
	add	a0, t2, a0
	sd	a0, -344(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -352(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
.L_ifend_main_26:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -360(s0)
	addiw	a0, a0, 1
	sw	a0, -368(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_24
.L_whe_main_25:
	addi	t1, s0, -28
	lw	a0, -120(s0)
	sw	a0, 0(t1)
.L_wh_main_27:
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -120(s0)
	addiw	a0, a0, 0
	sw	a0, -176(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -136(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -376(s0)
	lw	a0, -376(s0)
	beqz	a0, .L_whe_main_28
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -384(s0)
	slliw	a0, a0, 2
	sw	a0, -392(s0)
	lw	t2, -176(s0)
	addw	a0, t2, a0
	sw	a0, -400(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -408(s0)
	mv	t1, a0
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -416(s0)
	addiw	a0, a0, 1
	sw	a0, -424(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_27
.L_whe_main_28:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_22
.L_whe_main_23:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_wh_main_29:
	li	a0, 250
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 2147483647
	sw	a0, -88(s0)
	lw	a0, -72(s0)
	sw	a0, -96(s0)
	lla	a0, c
	sd	a0, -104(s0)
	lw	a0, -80(s0)
	sw	a0, -112(s0)
	li	a0, 1000
	sw	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	ld	a0, -104(s0)
	sd	a0, -136(s0)
	lw	a0, -80(s0)
	sw	a0, -144(s0)
	lw	a0, -120(s0)
	sw	a0, -152(s0)
	lw	a0, -128(s0)
	sw	a0, -160(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_main_30
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -56
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_main_31:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -200(s0)
	addiw	a0, a0, 0
	sw	a0, -208(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	beqz	a0, .L_whe_main_32
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -240(s0)
	addiw	a0, a0, 0
	sw	a0, -248(s0)
	mv	t2, a0
	lw	a0, -184(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	ld	t2, -104(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -272(s0)
	negw	a0, a0
	sw	a0, -280(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	slliw	a0, a0, 2
	sw	a0, -296(s0)
	lw	t2, -208(s0)
	addw	a0, t2, a0
	sw	a0, -304(s0)
	ld	t2, -136(s0)
	add	a0, t2, a0
	sd	a0, -312(s0)
	mv	t1, a0
	lw	a0, -280(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	addiw	a0, a0, 1
	sw	a0, -328(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_31
.L_whe_main_32:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addiw	a0, a0, 1
	sw	a0, -112(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_29
.L_whe_main_30:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_wh_main_33:
	li	a0, 250
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	li	a0, 2147483647
	sw	a0, -88(s0)
	lw	a0, -72(s0)
	sw	a0, -96(s0)
	lla	a0, c
	sd	a0, -104(s0)
	lw	a0, -80(s0)
	sw	a0, -112(s0)
	li	a0, 1
	sw	a0, -120(s0)
	li	a0, 1000
	sw	a0, -128(s0)
	lw	a0, -120(s0)
	sw	a0, -136(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_34
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_main_35:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	slliw	t2, a0, 3
	slliw	t1, a0, 5
	addw	t2, t2, t1
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 9
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -160(s0)
	addiw	a0, a0, 0
	sw	a0, -168(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -184(s0)
	lw	a0, -184(s0)
	beqz	a0, .L_whe_main_36
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	slliw	a0, a0, 2
	sw	a0, -208(s0)
	lw	t2, -168(s0)
	addw	a0, t2, a0
	sw	a0, -216(s0)
	ld	t2, -104(s0)
	add	a0, t2, a0
	sd	a0, -224(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -232(s0)
	lw	t2, -192(s0)
	addw	a0, t2, a0
	sw	a0, -240(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addiw	a0, a0, 1
	sw	a0, -256(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_35
.L_whe_main_36:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -24
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
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	sext.w	a0, a0
	j	.Lreturn_main_0
	li	a0, 0
.Lreturn_main_0:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
