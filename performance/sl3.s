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
	addi	sp, sp, -1792
	li	t0, 1792
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	call	getint
	sw	a0, -96(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	call	getint
	sw	a0, -104(s0)
	addi	t1, s0, -32
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
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -136(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_0:
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	sw	a0, -80(s0)
	lla	a0, x
	sd	a0, -88(s0)
	lla	a0, y
	sd	a0, -96(s0)
	li	a0, 1
	sw	a0, -104(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	lw	a0, -104(s0)
	sw	a0, -120(s0)
	lw	a0, -64(s0)
	sw	a0, -128(s0)
	li	a0, 250000
	sw	a0, -136(s0)
	li	a0, 1000
	sw	a0, -144(s0)
	lw	a0, -64(s0)
	sw	a0, -152(s0)
	lw	a0, -64(s0)
	sw	a0, -160(s0)
	lw	a0, -136(s0)
	sw	a0, -168(s0)
	lw	a0, -144(s0)
	sw	a0, -176(s0)
	lw	a0, -104(s0)
	sw	a0, -184(s0)
	lw	a0, -104(s0)
	sw	a0, -192(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	ld	a0, -88(s0)
	sd	a0, -88(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_2:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -224(s0)
	addiw	a0, a0, 0
	sw	a0, -232(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -248(s0)
	addiw	a0, a0, 0
	sw	a0, -256(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -264(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -272(s0)
	lw	a0, -272(s0)
	beqz	a0, .L_whe_main_3
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_4:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -280(s0)
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
	sw	a0, -288(s0)
	lw	t2, -232(s0)
	addw	a0, t2, a0
	sw	a0, -296(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -304(s0)
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
	lw	t2, -256(s0)
	addw	a0, t2, a0
	sw	a0, -320(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	beqz	a0, .L_whe_main_5
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	slliw	a0, a0, 2
	sw	a0, -352(s0)
	lw	t2, -296(s0)
	addw	a0, t2, a0
	sw	a0, -360(s0)
	ld	t2, -88(s0)
	add	a0, t2, a0
	sd	a0, -368(s0)
	mv	t1, a0
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	slliw	a0, a0, 2
	sw	a0, -384(s0)
	lw	t2, -320(s0)
	addw	a0, t2, a0
	sw	a0, -392(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -400(s0)
	mv	t1, a0
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -408(s0)
	addiw	a0, a0, 1
	sw	a0, -416(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	addiw	a0, a0, 1
	sw	a0, -256(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_6:
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lw	a0, -56(s0)
	addiw	a0, a0, -1
	sw	a0, -72(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	addiw	a0, a0, -1
	sw	a0, -88(s0)
	lw	a0, -64(s0)
	sw	a0, -96(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	lw	a0, -64(s0)
	sw	a0, -112(s0)
	lw	a0, -104(s0)
	addiw	a0, a0, -1
	sw	a0, -120(s0)
	lla	a0, x
	sd	a0, -128(s0)
	li	a0, 0
	sw	a0, -136(s0)
	lw	a0, -64(s0)
	sw	a0, -144(s0)
	lw	a0, -64(s0)
	sw	a0, -152(s0)
	li	a0, 250000
	sw	a0, -160(s0)
	li	a0, 1000
	sw	a0, -168(s0)
	ld	a0, -128(s0)
	sd	a0, -176(s0)
	lw	a0, -136(s0)
	sw	a0, -184(s0)
	lw	a0, -64(s0)
	sw	a0, -192(s0)
	lw	a0, -160(s0)
	sw	a0, -200(s0)
	lw	a0, -168(s0)
	sw	a0, -208(s0)
	ld	a0, -128(s0)
	sd	a0, -216(s0)
	lw	a0, -136(s0)
	sw	a0, -224(s0)
	lw	a0, -160(s0)
	sw	a0, -232(s0)
	lw	a0, -64(s0)
	sw	a0, -240(s0)
	lw	a0, -168(s0)
	sw	a0, -248(s0)
	ld	a0, -128(s0)
	sd	a0, -256(s0)
	lw	a0, -136(s0)
	sw	a0, -264(s0)
	lw	a0, -160(s0)
	sw	a0, -272(s0)
	lw	a0, -64(s0)
	sw	a0, -280(s0)
	lw	a0, -168(s0)
	sw	a0, -288(s0)
	ld	a0, -128(s0)
	sd	a0, -296(s0)
	lw	a0, -136(s0)
	sw	a0, -304(s0)
	lw	a0, -160(s0)
	sw	a0, -312(s0)
	lw	a0, -168(s0)
	sw	a0, -320(s0)
	lw	a0, -64(s0)
	sw	a0, -328(s0)
	ld	a0, -128(s0)
	sd	a0, -336(s0)
	lw	a0, -136(s0)
	sw	a0, -344(s0)
	lw	a0, -160(s0)
	sw	a0, -352(s0)
	lw	a0, -168(s0)
	sw	a0, -360(s0)
	lw	a0, -64(s0)
	sw	a0, -368(s0)
	ld	a0, -128(s0)
	sd	a0, -376(s0)
	lw	a0, -136(s0)
	sw	a0, -384(s0)
	lw	a0, -64(s0)
	sw	a0, -392(s0)
	lw	a0, -160(s0)
	sw	a0, -400(s0)
	lw	a0, -64(s0)
	sw	a0, -408(s0)
	lw	a0, -168(s0)
	sw	a0, -416(s0)
	lw	a0, -64(s0)
	sw	a0, -424(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -432(s0)
	ld	a0, -128(s0)
	sd	a0, -440(s0)
	lw	a0, -136(s0)
	sw	a0, -448(s0)
	lw	a0, -160(s0)
	sw	a0, -456(s0)
	lw	a0, -168(s0)
	sw	a0, -464(s0)
	lw	a0, -64(s0)
	sw	a0, -472(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -480(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -488(s0)
	lw	a0, -488(s0)
	beqz	a0, .L_whe_main_7
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_8:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -496(s0)
	addiw	a0, a0, -1
	sw	a0, -504(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -512(s0)
	addiw	a0, a0, 0
	sw	a0, -520(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -528(s0)
	addiw	a0, a0, 1
	sw	a0, -536(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -544(s0)
	addiw	a0, a0, 0
	sw	a0, -552(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -560(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -568(s0)
	addiw	a0, a0, 0
	sw	a0, -576(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -584(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -592(s0)
	addiw	a0, a0, 0
	sw	a0, -600(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -608(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -616(s0)
	addiw	a0, a0, 0
	sw	a0, -624(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -632(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -640(s0)
	addiw	a0, a0, 0
	sw	a0, -648(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -656(s0)
	addiw	a0, a0, -1
	sw	a0, -664(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -672(s0)
	addiw	a0, a0, 0
	sw	a0, -680(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -688(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -696(s0)
	addiw	a0, a0, 0
	sw	a0, -704(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -712(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -720(s0)
	lw	a0, -720(s0)
	beqz	a0, .L_whe_main_9
	addi	t1, s0, -28
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_main_10:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -728(s0)
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
	sw	a0, -736(s0)
	lw	t2, -520(s0)
	addw	a0, t2, a0
	sw	a0, -744(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -752(s0)
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
	sw	a0, -760(s0)
	lw	t2, -552(s0)
	addw	a0, t2, a0
	sw	a0, -768(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -776(s0)
	addiw	a0, a0, -1
	sw	a0, -784(s0)
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
	sw	a0, -792(s0)
	lw	t2, -576(s0)
	addw	a0, t2, a0
	sw	a0, -800(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -808(s0)
	addiw	a0, a0, 1
	sw	a0, -816(s0)
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
	sw	a0, -824(s0)
	lw	t2, -600(s0)
	addw	a0, t2, a0
	sw	a0, -832(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -840(s0)
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
	sw	a0, -848(s0)
	lw	t2, -624(s0)
	addw	a0, t2, a0
	sw	a0, -856(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -864(s0)
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
	sw	a0, -872(s0)
	lw	t2, -648(s0)
	addw	a0, t2, a0
	sw	a0, -880(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -888(s0)
	addiw	a0, a0, -1
	sw	a0, -896(s0)
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
	sw	a0, -904(s0)
	lw	t2, -680(s0)
	addw	a0, t2, a0
	sw	a0, -912(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -920(s0)
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
	sw	a0, -928(s0)
	lw	t2, -704(s0)
	addw	a0, t2, a0
	sw	a0, -936(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -944(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -952(s0)
	lw	a0, -952(s0)
	beqz	a0, .L_whe_main_11
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -960(s0)
	slliw	a0, a0, 2
	sw	a0, -968(s0)
	lw	t2, -744(s0)
	addw	a0, t2, a0
	sw	a0, -976(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -984(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -992(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1000(s0)
	slliw	a0, a0, 2
	sw	a0, -1008(s0)
	lw	t2, -768(s0)
	addw	a0, t2, a0
	sw	a0, -1016(s0)
	ld	t2, -176(s0)
	add	a0, t2, a0
	sd	a0, -1024(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -1032(s0)
	lw	t2, -992(s0)
	addw	a0, t2, a0
	sw	a0, -1040(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1048(s0)
	slliw	a0, a0, 2
	sw	a0, -1056(s0)
	lw	t2, -800(s0)
	addw	a0, t2, a0
	sw	a0, -1064(s0)
	ld	t2, -216(s0)
	add	a0, t2, a0
	sd	a0, -1072(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -1080(s0)
	lw	t2, -1040(s0)
	addw	a0, t2, a0
	sw	a0, -1088(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1096(s0)
	slliw	a0, a0, 2
	sw	a0, -1104(s0)
	lw	t2, -832(s0)
	addw	a0, t2, a0
	sw	a0, -1112(s0)
	ld	t2, -256(s0)
	add	a0, t2, a0
	sd	a0, -1120(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -1128(s0)
	lw	t2, -1088(s0)
	addw	a0, t2, a0
	sw	a0, -1136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1144(s0)
	addiw	a0, a0, -1
	sw	a0, -1152(s0)
	slliw	a0, a0, 2
	sw	a0, -1160(s0)
	lw	t2, -856(s0)
	addw	a0, t2, a0
	sw	a0, -1168(s0)
	ld	t2, -296(s0)
	add	a0, t2, a0
	sd	a0, -1176(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -1184(s0)
	lw	t2, -1136(s0)
	addw	a0, t2, a0
	sw	a0, -1192(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1200(s0)
	addiw	a0, a0, 1
	sw	a0, -1208(s0)
	slliw	a0, a0, 2
	sw	a0, -1216(s0)
	lw	t2, -880(s0)
	addw	a0, t2, a0
	sw	a0, -1224(s0)
	ld	t2, -336(s0)
	add	a0, t2, a0
	sd	a0, -1232(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -1240(s0)
	lw	t2, -1192(s0)
	addw	a0, t2, a0
	sw	a0, -1248(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1256(s0)
	addiw	a0, a0, -1
	sw	a0, -1264(s0)
	slliw	a0, a0, 2
	sw	a0, -1272(s0)
	lw	t2, -912(s0)
	addw	a0, t2, a0
	sw	a0, -1280(s0)
	ld	t2, -376(s0)
	add	a0, t2, a0
	sd	a0, -1288(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -1296(s0)
	lw	t2, -1248(s0)
	addw	a0, t2, a0
	sw	a0, -1304(s0)
	mv	t2, a0
	lw	a0, -432(s0)
	divw	a0, t2, a0
	sw	a0, -1312(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1320(s0)
	slliw	a0, a0, 2
	sw	a0, -1328(s0)
	lw	t2, -936(s0)
	addw	a0, t2, a0
	sw	a0, -1336(s0)
	ld	t2, -440(s0)
	add	a0, t2, a0
	sd	a0, -1344(s0)
	mv	t1, a0
	lw	a0, -1312(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -1352(s0)
	addiw	a0, a0, 1
	sw	a0, -1360(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_10
.L_whe_main_11:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -520(s0)
	addiw	a0, a0, 1
	sw	a0, -552(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_8
.L_whe_main_9:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -20
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
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	lla	a0, x
	sd	a0, -72(s0)
	addi	sp, sp, -16
	lw	a0, -64(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -72(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	lla	a0, x
	sd	a0, -88(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	li	a0, 2
	sw	a0, -104(s0)
	lw	a0, -96(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -112(s0)
	li	a0, 250000
	sw	a0, -120(s0)
	lw	a0, -112(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -128(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -144(s0)
	li	a0, 1000
	sw	a0, -152(s0)
	lw	a0, -144(s0)
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
	lw	t2, -128(s0)
	addw	a0, t2, a0
	sw	a0, -168(s0)
	ld	t2, -88(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	addi	sp, sp, -16
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -176(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	lla	a0, x
	sd	a0, -192(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	li	a0, 1
	sw	a0, -208(s0)
	lw	a0, -200(s0)
	addiw	a0, a0, -1
	sw	a0, -216(s0)
	li	a0, 250000
	sw	a0, -224(s0)
	lw	a0, -216(s0)
	slliw	t2, a0, 4
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	slliw	t1, a0, 14
	addw	t2, t2, t1
	slliw	t1, a0, 15
	addw	t2, t2, t1
	slliw	t1, a0, 16
	addw	t2, t2, t1
	slliw	t1, a0, 17
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -232(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	addiw	a0, a0, -1
	sw	a0, -248(s0)
	li	a0, 1000
	sw	a0, -256(s0)
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
	sw	a0, -264(s0)
	lw	t2, -232(s0)
	addw	a0, t2, a0
	sw	a0, -272(s0)
	ld	t2, -192(s0)
	add	a0, t2, a0
	sd	a0, -280(s0)
	addi	sp, sp, -16
	lw	a0, -184(s0)
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
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
