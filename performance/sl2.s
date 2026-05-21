	.bss
	.align	2
	.globl	x
x:
	.zero	62500000
	.bss
	.align	2
	.globl	y
y:
	.zero	62500000
	.text
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -1136
	addi	t0, sp, 1136
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -1040(s0)
	sd	s2, -1048(s0)
	sd	s3, -1056(s0)
	sd	s4, -1064(s0)
	sd	s5, -1072(s0)
	sd	s6, -1080(s0)
	sd	s7, -1088(s0)
	sd	s8, -1096(s0)
	sd	s9, -1104(s0)
	sd	s10, -1112(s0)
	sd	s11, -1120(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -20
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -28
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -32
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -36
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	call	getint
	sw	a0, -96(s0)
	addi	t1, s0, -36
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	call	getint
	sw	a0, -104(s0)
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	li	a0, 13
	sw	a0, -112(s0)
	addi	sp, sp, -16
	li	a0, 13
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -120(s0)
	addi	t1, s0, -20
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -24
	lw	a0, -128(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -136(s0)
	addi	t1, s0, -28
	lw	a0, -136(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	li	a0, 250000
	sw	a0, -72(s0)
	li	a0, 1000
	sw	a0, -80(s0)
	lla	a0, x
	sd	a0, -88(s0)
	lla	a0, y
	sd	a0, -96(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	lw	a0, -20(s0)
	lw	t2, -20(s0)
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	lw	a0, -24(s0)
	sw	a0, 0(t1)
.L_wh_main_2:
	lw	a0, -20(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	addiw	a0, a0, 0
	sw	a0, -160(s0)
	lw	a0, -20(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	addiw	a0, a0, 0
	sw	a0, -184(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_whe_main_3
	addi	t1, s0, -28
	lw	a0, -28(s0)
	sw	a0, 0(t1)
.L_wh_main_4:
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -216(s0)
	lw	t2, -160(s0)
	lw	a0, -216(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -240(s0)
	lw	t2, -184(s0)
	lw	a0, -240(s0)
	addw	a0, t2, a0
	sw	a0, -248(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -36(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -264(s0)
	lw	a0, -264(s0)
	beqz	a0, .L_whe_main_5
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -280(s0)
	lw	t2, -224(s0)
	lw	a0, -280(s0)
	addw	a0, t2, a0
	sw	a0, -288(s0)
	ld	t2, -88(s0)
	lw	a0, -288(s0)
	add	a0, t2, a0
	sd	a0, -296(s0)
	ld	t1, -296(s0)
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -312(s0)
	lw	t2, -248(s0)
	lw	a0, -312(s0)
	addw	a0, t2, a0
	sw	a0, -320(s0)
	ld	t2, -96(s0)
	lw	a0, -320(s0)
	add	a0, t2, a0
	sd	a0, -328(s0)
	ld	t1, -328(s0)
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -344(s0)
	addi	t1, s0, -28
	lw	a0, -344(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -24
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	lw	a0, -20(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -20
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -20
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -28
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_6:
	li	a0, 1
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	li	a0, 250000
	sw	a0, -72(s0)
	li	a0, 1000
	sw	a0, -80(s0)
	lla	a0, x
	sd	a0, -88(s0)
	lw	a0, -36(s0)
	addiw	a0, a0, -1
	sw	a0, -104(s0)
	lw	a0, -36(s0)
	addiw	a0, a0, -1
	sw	a0, -120(s0)
	lw	a0, -36(s0)
	addiw	a0, a0, -1
	sw	a0, -136(s0)
	lw	a0, -32(s0)
	lw	a0, -20(s0)
	lw	t2, -20(s0)
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_main_7
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	lw	a0, -24(s0)
	sw	a0, 0(t1)
.L_wh_main_8:
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -184(s0)
	lw	a0, -184(s0)
	addiw	a0, a0, 0
	sw	a0, -192(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	addiw	a0, a0, 0
	sw	a0, -224(s0)
	lw	a0, -20(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	addiw	a0, a0, 0
	sw	a0, -248(s0)
	lw	a0, -20(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -264(s0)
	lw	a0, -264(s0)
	addiw	a0, a0, 0
	sw	a0, -272(s0)
	lw	a0, -20(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	addiw	a0, a0, 0
	sw	a0, -296(s0)
	lw	a0, -20(s0)
	mv	s7, a0
	sw	s7, -304(s0)
	mv	a0, s7
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	mv	s8, a0
	sw	s8, -312(s0)
	mv	a0, s8
	addiw	a0, a0, 0
	sw	a0, -320(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -344(s0)
	lw	a0, -344(s0)
	addiw	a0, a0, 0
	sw	a0, -352(s0)
	lw	a0, -20(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -368(s0)
	lw	a0, -368(s0)
	addiw	a0, a0, 0
	sw	a0, -376(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -392(s0)
	lw	a0, -392(s0)
	beqz	a0, .L_whe_main_9
	addi	t1, s0, -28
	lw	a0, -28(s0)
	sw	a0, 0(t1)
.L_wh_main_10:
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -408(s0)
	lw	t2, -192(s0)
	lw	a0, -408(s0)
	addw	a0, t2, a0
	sw	a0, -416(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -432(s0)
	lw	t2, -224(s0)
	lw	a0, -432(s0)
	addw	a0, t2, a0
	sw	a0, -440(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -1
	sw	a0, -456(s0)
	lw	a0, -456(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -464(s0)
	lw	t2, -248(s0)
	lw	a0, -464(s0)
	addw	a0, t2, a0
	sw	a0, -472(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -488(s0)
	lw	a0, -488(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -496(s0)
	lw	t2, -272(s0)
	lw	a0, -496(s0)
	addw	a0, t2, a0
	sw	a0, -504(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -520(s0)
	lw	t2, -296(s0)
	lw	a0, -520(s0)
	addw	a0, t2, a0
	sw	a0, -528(s0)
	lw	a0, -24(s0)
	mv	s9, a0
	sw	s9, -536(s0)
	mv	a0, s9
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	mv	s10, a0
	sw	s10, -544(s0)
	lw	t2, -320(s0)
	mv	a0, s10
	addw	a0, t2, a0
	sw	a0, -552(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -1
	sw	a0, -568(s0)
	lw	a0, -568(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -576(s0)
	lw	t2, -352(s0)
	lw	a0, -576(s0)
	addw	a0, t2, a0
	sw	a0, -584(s0)
	lw	a0, -24(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -600(s0)
	lw	t2, -376(s0)
	lw	a0, -600(s0)
	addw	a0, t2, a0
	sw	a0, -608(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -136(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -624(s0)
	lw	a0, -624(s0)
	beqz	a0, .L_whe_main_11
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -640(s0)
	lw	t2, -416(s0)
	lw	a0, -640(s0)
	addw	a0, t2, a0
	sw	a0, -648(s0)
	ld	t2, -88(s0)
	lw	a0, -648(s0)
	add	a0, t2, a0
	sd	a0, -656(s0)
	ld	t2, -656(s0)
	lw	a0, 0(t2)
	sw	a0, -664(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -680(s0)
	lw	t2, -440(s0)
	lw	a0, -680(s0)
	addw	a0, t2, a0
	sw	a0, -688(s0)
	ld	t2, -88(s0)
	lw	a0, -688(s0)
	add	a0, t2, a0
	sd	a0, -696(s0)
	ld	t2, -696(s0)
	lw	a0, 0(t2)
	sw	a0, -704(s0)
	lw	t2, -664(s0)
	lw	a0, -704(s0)
	addw	a0, t2, a0
	sw	a0, -712(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -728(s0)
	lw	t2, -472(s0)
	lw	a0, -728(s0)
	addw	a0, t2, a0
	sw	a0, -736(s0)
	ld	t2, -88(s0)
	lw	a0, -736(s0)
	add	a0, t2, a0
	sd	a0, -744(s0)
	ld	t2, -744(s0)
	lw	a0, 0(t2)
	sw	a0, -752(s0)
	lw	t2, -712(s0)
	lw	a0, -752(s0)
	addw	a0, t2, a0
	sw	a0, -760(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -776(s0)
	lw	t2, -504(s0)
	lw	a0, -776(s0)
	addw	a0, t2, a0
	sw	a0, -784(s0)
	ld	t2, -88(s0)
	lw	a0, -784(s0)
	add	a0, t2, a0
	sd	a0, -792(s0)
	ld	t2, -792(s0)
	lw	a0, 0(t2)
	sw	a0, -800(s0)
	lw	t2, -760(s0)
	lw	a0, -800(s0)
	addw	a0, t2, a0
	sw	a0, -808(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -824(s0)
	lw	a0, -824(s0)
	slliw	a0, a0, 2
	mv	s2, a0
	sw	s2, -832(s0)
	lw	t2, -528(s0)
	mv	a0, s2
	addw	a0, t2, a0
	mv	s3, a0
	sw	s3, -840(s0)
	ld	t2, -88(s0)
	mv	a0, s3
	add	a0, t2, a0
	mv	s4, a0
	sd	s4, -848(s0)
	mv	t2, s4
	lw	a0, 0(t2)
	mv	s5, a0
	sw	s5, -856(s0)
	lw	t2, -808(s0)
	mv	a0, s5
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -864(s0)
	lw	a0, -28(s0)
	mv	s11, a0
	sw	s11, -872(s0)
	mv	a0, s11
	addiw	a0, a0, 1
	sw	a0, -880(s0)
	lw	a0, -880(s0)
	slliw	a0, a0, 2
	sw	a0, -888(s0)
	lw	t2, -552(s0)
	lw	a0, -888(s0)
	addw	a0, t2, a0
	mv	s1, a0
	sw	s1, -896(s0)
	ld	t2, -88(s0)
	mv	a0, s1
	add	a0, t2, a0
	sd	a0, -904(s0)
	ld	t2, -904(s0)
	lw	a0, 0(t2)
	sw	a0, -912(s0)
	mv	t2, s6
	lw	a0, -912(s0)
	addw	a0, t2, a0
	sw	a0, -920(s0)
	lw	a0, -28(s0)
	addiw	a0, a0, -1
	sw	a0, -936(s0)
	lw	a0, -936(s0)
	slliw	a0, a0, 2
	sw	a0, -944(s0)
	lw	t2, -584(s0)
	lw	a0, -944(s0)
	addw	a0, t2, a0
	sw	a0, -952(s0)
	ld	t2, -88(s0)
	lw	a0, -952(s0)
	add	a0, t2, a0
	sd	a0, -960(s0)
	ld	t2, -960(s0)
	lw	a0, 0(t2)
	sw	a0, -968(s0)
	lw	t2, -920(s0)
	lw	a0, -968(s0)
	addw	a0, t2, a0
	sw	a0, -976(s0)
	lw	t2, -976(s0)
	lw	a0, -32(s0)
	divw	a0, t2, a0
	sw	a0, -984(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -1000(s0)
	lw	t2, -608(s0)
	lw	a0, -1000(s0)
	addw	a0, t2, a0
	sw	a0, -1008(s0)
	ld	t2, -88(s0)
	lw	a0, -1008(s0)
	add	a0, t2, a0
	sd	a0, -1016(s0)
	ld	t1, -1016(s0)
	lw	a0, -984(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -1032(s0)
	addi	t1, s0, -28
	lw	a0, -1032(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_10
.L_whe_main_11:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -224(s0)
	addi	t1, s0, -24
	lw	a0, -224(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_8
.L_whe_main_9:
	lw	a0, -20(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -20
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_6
.L_whe_main_7:
	li	a0, 53
	sw	a0, -56(s0)
	addi	sp, sp, -16
	li	a0, 53
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lla	a0, x
	sd	a0, -64(s0)
	lw	a0, -36(s0)
	addi	sp, sp, -16
	lw	a0, -72(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -64(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 2
	sw	a0, -80(s0)
	li	a0, 250000
	sw	a0, -88(s0)
	li	a0, 1000
	sw	a0, -96(s0)
	lla	a0, x
	sd	a0, -104(s0)
	lw	a0, -36(s0)
	lw	a0, -36(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -36(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -160(s0)
	lw	t2, -136(s0)
	lw	a0, -160(s0)
	addw	a0, t2, a0
	sw	a0, -168(s0)
	ld	t2, -104(s0)
	lw	a0, -168(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	addi	sp, sp, -16
	lw	a0, -112(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -176(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 1
	sw	a0, -184(s0)
	li	a0, 250000
	sw	a0, -192(s0)
	li	a0, 1000
	sw	a0, -200(s0)
	lla	a0, x
	sd	a0, -208(s0)
	lw	a0, -36(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	mv	t2, a0
	li	a0, 250000
	mulw	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -24(s0)
	addiw	a0, a0, -1
	sw	a0, -256(s0)
	lw	a0, -256(s0)
	mv	t2, a0
	li	a0, 1000
	mulw	a0, t2, a0
	sw	a0, -264(s0)
	lw	t2, -240(s0)
	lw	a0, -264(s0)
	addw	a0, t2, a0
	sw	a0, -272(s0)
	ld	t2, -208(s0)
	lw	a0, -272(s0)
	add	a0, t2, a0
	sd	a0, -280(s0)
	addi	sp, sp, -16
	lw	a0, -216(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -280(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	sext.w	a0, a0
	j	.Lreturn_main_0
	li	a0, 0
.Lreturn_main_0:
	ld	s11, -1120(s0)
	ld	s10, -1112(s0)
	ld	s9, -1104(s0)
	ld	s8, -1096(s0)
	ld	s7, -1088(s0)
	ld	s6, -1080(s0)
	ld	s5, -1072(s0)
	ld	s4, -1064(s0)
	ld	s3, -1056(s0)
	ld	s2, -1048(s0)
	ld	s1, -1040(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
