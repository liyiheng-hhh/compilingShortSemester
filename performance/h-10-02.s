	.bss
	.align	2
	.globl	A
A:
	.zero	4194304
	.bss
	.align	2
	.globl	B
B:
	.zero	4194304
	.bss
	.align	2
	.globl	C
C:
	.zero	4194304
	.text
	.text
	.align	1
	.globl	trsm_optimized
	.type	trsm_optimized, @function
trsm_optimized:
	addi	sp, sp, -880
	li	t0, 880
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	mv	t4, a0
	addi	t1, s0, -48
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
.L_wh_trsm_optimized_0:
	mv	a0, t4
	sw	a0, -72(s0)
	li	a0, 0
	sw	a0, -80(s0)
	mv	a0, t4
	sw	a0, -88(s0)
	ld	a0, -40(s0)
	sd	a0, -96(s0)
	lw	a0, -80(s0)
	sw	a0, -104(s0)
	ld	a0, -32(s0)
	sd	a0, -112(s0)
	lw	a0, -80(s0)
	sw	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	lla	t0, .Lfloat_1
	flw	fa0, 0(t0)
	fsw	fa0, -136(s0)
	ld	a0, -96(s0)
	sd	a0, -144(s0)
	lw	a0, -80(s0)
	sw	a0, -152(s0)
	lw	a0, -128(s0)
	sw	a0, -160(s0)
	lw	a0, -128(s0)
	sw	a0, -168(s0)
	mv	a0, t4
	sw	a0, -176(s0)
	lw	a0, -80(s0)
	sw	a0, -184(s0)
	mv	a0, t4
	sw	a0, -192(s0)
	ld	a0, -96(s0)
	sd	a0, -200(s0)
	lw	a0, -128(s0)
	sw	a0, -208(s0)
	lw	a0, -80(s0)
	sw	a0, -216(s0)
	lw	a0, -128(s0)
	sw	a0, -224(s0)
	ld	a0, -112(s0)
	sd	a0, -232(s0)
	lw	a0, -80(s0)
	sw	a0, -240(s0)
	ld	a0, -96(s0)
	sd	a0, -248(s0)
	lw	a0, -80(s0)
	sw	a0, -256(s0)
	ld	a0, -96(s0)
	sd	a0, -264(s0)
	lw	a0, -80(s0)
	sw	a0, -272(s0)
	lw	a0, -128(s0)
	sw	a0, -280(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	beqz	a0, .L_whe_trsm_optimized_1
	addi	t1, s0, -48
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_trsm_optimized_2:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	slliw	a0, a0, 12
	sw	a0, -312(s0)
	addiw	a0, a0, 0
	sw	a0, -320(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	slliw	a0, a0, 12
	sw	a0, -336(s0)
	addiw	a0, a0, 0
	sw	a0, -344(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -352(s0)
	slliw	a0, a0, 2
	sw	a0, -360(s0)
	lw	t2, -344(s0)
	addw	a0, t2, a0
	sw	a0, -368(s0)
	ld	t2, -112(s0)
	add	a0, t2, a0
	sd	a0, -376(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -384(s0)
	slliw	a0, a0, 12
	sw	a0, -392(s0)
	addiw	a0, a0, 0
	sw	a0, -400(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -408(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -416(s0)
	lw	a0, -416(s0)
	beqz	a0, .L_whe_trsm_optimized_3
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -424(s0)
	slliw	a0, a0, 2
	sw	a0, -432(s0)
	lw	t2, -320(s0)
	addw	a0, t2, a0
	sw	a0, -440(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -448(s0)
	mv	t2, a0
	flw	fa0, 0(t2)
	fsw	fa0, -456(s0)
	ld	t2, -376(s0)
	flw	fa0, 0(t2)
	fsw	fa0, -464(s0)
	flw	fa0, -456(s0)
	fmv.s	ft8, fa0
	flw	fa0, -464(s0)
	fdiv.s	fa0, ft8, fa0
	fsw	fa0, -472(s0)
	fmv.s	ft8, fa0
	flw	fa0, -136(s0)
	fadd.s	fa0, ft8, fa0
	fsw	fa0, -480(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -488(s0)
	slliw	a0, a0, 2
	sw	a0, -496(s0)
	lw	t2, -400(s0)
	addw	a0, t2, a0
	sw	a0, -504(s0)
	ld	t2, -144(s0)
	add	a0, t2, a0
	sd	a0, -512(s0)
	mv	t1, a0
	fsw	fa0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -520(s0)
	addiw	a0, a0, 1
	sw	a0, -528(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_2
.L_whe_trsm_optimized_3:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
.L_wh_trsm_optimized_4:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	slliw	a0, a0, 12
	sw	a0, -152(s0)
	addiw	a0, a0, 0
	sw	a0, -160(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -176(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -320(s0)
	lw	a0, -320(s0)
	beqz	a0, .L_whe_trsm_optimized_5
	addi	t1, s0, -48
	lw	a0, -184(s0)
	sw	a0, 0(t1)
.L_wh_trsm_optimized_6:
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	slliw	a0, a0, 12
	sw	a0, -400(s0)
	addiw	a0, a0, 0
	sw	a0, -536(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -544(s0)
	slliw	a0, a0, 12
	sw	a0, -552(s0)
	addiw	a0, a0, 0
	sw	a0, -560(s0)
	mv	t2, a0
	lw	a0, -136(s0)
	addw	a0, t2, a0
	sw	a0, -568(s0)
	ld	t2, -232(s0)
	add	a0, t2, a0
	sd	a0, -576(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -584(s0)
	slliw	a0, a0, 12
	sw	a0, -592(s0)
	addiw	a0, a0, 0
	sw	a0, -600(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -608(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -192(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -616(s0)
	lw	a0, -616(s0)
	beqz	a0, .L_whe_trsm_optimized_7
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -624(s0)
	slliw	a0, a0, 2
	sw	a0, -632(s0)
	lw	t2, -536(s0)
	addw	a0, t2, a0
	sw	a0, -640(s0)
	ld	t2, -200(s0)
	add	a0, t2, a0
	sd	a0, -648(s0)
	mv	t2, a0
	flw	fa0, 0(t2)
	fsw	fa0, -656(s0)
	ld	t2, -576(s0)
	flw	fa0, 0(t2)
	fsw	fa0, -664(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -672(s0)
	slliw	a0, a0, 2
	sw	a0, -680(s0)
	lw	t2, -160(s0)
	addw	a0, t2, a0
	sw	a0, -688(s0)
	ld	t2, -248(s0)
	add	a0, t2, a0
	sd	a0, -696(s0)
	mv	t2, a0
	flw	fa0, 0(t2)
	fsw	fa0, -704(s0)
	flw	fa0, -664(s0)
	fmv.s	ft8, fa0
	flw	fa0, -704(s0)
	fmul.s	fa0, ft8, fa0
	fsw	fa0, -712(s0)
	flw	fa0, -656(s0)
	fmv.s	ft8, fa0
	flw	fa0, -712(s0)
	fsub.s	fa0, ft8, fa0
	fsw	fa0, -720(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -728(s0)
	slliw	a0, a0, 2
	sw	a0, -736(s0)
	lw	t2, -600(s0)
	addw	a0, t2, a0
	sw	a0, -744(s0)
	ld	t2, -264(s0)
	add	a0, t2, a0
	sd	a0, -752(s0)
	mv	t1, a0
	fsw	fa0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -760(s0)
	addiw	a0, a0, 1
	sw	a0, -768(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_6
.L_whe_trsm_optimized_7:
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -160(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_4
.L_whe_trsm_optimized_5:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_0
.L_whe_trsm_optimized_1:
	j	.Lreturn_trsm_optimized_0
.Lreturn_trsm_optimized_0:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	trsm_optimized, .-trsm_optimized
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -736
	li	t0, 736
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -36
	flw	fa0, -56(s0)
	fsw	fa0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	call	getint
	sw	a0, -64(s0)
	addi	t1, s0, -20
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_0:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lla	a0, A
	sd	a0, -80(s0)
	lw	a0, -64(s0)
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	sw	a0, -104(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_2:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 12
	sw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_main_3
	call	getfloat
	fsw	fa0, -168(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	lw	t2, -144(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -200(s0)
	mv	t1, a0
	fsw	fa0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addiw	a0, a0, 1
	sw	a0, -216(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_4:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lla	a0, C
	sd	a0, -80(s0)
	lw	a0, -64(s0)
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	sw	a0, -104(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_main_5
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_6:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 12
	sw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_main_7
	call	getfloat
	fsw	fa0, -168(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	lw	t2, -144(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -200(s0)
	mv	t1, a0
	fsw	fa0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addiw	a0, a0, 1
	sw	a0, -216(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_6
.L_whe_main_7:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	li	a0, 55
	sw	a0, -56(s0)
	addi	sp, sp, -16
	li	a0, 55
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_main_8:
	li	a0, 5
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	a0, -64(s0)
	sw	a0, -80(s0)
	lla	a0, C
	sd	a0, -88(s0)
	lla	a0, B
	sd	a0, -96(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	lla	a0, A
	sd	a0, -112(s0)
	ld	a0, -96(s0)
	sd	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	lw	a0, -64(s0)
	sw	a0, -144(s0)
	lw	a0, -128(s0)
	sw	a0, -152(s0)
	lw	a0, -64(s0)
	sw	a0, -160(s0)
	lw	a0, -128(s0)
	sw	a0, -168(s0)
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
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -184(s0)
	lw	a0, -184(s0)
	beqz	a0, .L_whe_main_9
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_10:
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
	beqz	a0, .L_whe_main_11
	addi	t1, s0, -28
	lw	a0, -80(s0)
	sw	a0, 0(t1)
.L_wh_main_12:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	slliw	a0, a0, 12
	sw	a0, -216(s0)
	addiw	a0, a0, 0
	sw	a0, -224(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	a0, a0, 12
	sw	a0, -240(s0)
	addiw	a0, a0, 0
	sw	a0, -248(s0)
	ld	a0, -96(s0)
	sd	a0, -96(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -136(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -264(s0)
	lw	a0, -264(s0)
	beqz	a0, .L_whe_main_13
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	slliw	a0, a0, 2
	sw	a0, -280(s0)
	lw	t2, -224(s0)
	addw	a0, t2, a0
	sw	a0, -288(s0)
	ld	t2, -88(s0)
	add	a0, t2, a0
	sd	a0, -296(s0)
	mv	t2, a0
	flw	fa0, 0(t2)
	fsw	fa0, -304(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	slliw	a0, a0, 2
	sw	a0, -320(s0)
	lw	t2, -248(s0)
	addw	a0, t2, a0
	sw	a0, -328(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -336(s0)
	mv	t1, a0
	fsw	fa0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	addiw	a0, a0, 1
	sw	a0, -352(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_12
.L_whe_main_13:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	addiw	a0, a0, 1
	sw	a0, -248(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_10
.L_whe_main_11:
	addi	sp, sp, -32
	lw	a0, -104(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -112(s0)
	sd	a0, 8(sp)
	ld	a0, -120(s0)
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	trsm_optimized
	addi	sp, sp, 32
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_main_8
.L_whe_main_9:
	li	a0, 70
	sw	a0, -56(s0)
	addi	sp, sp, -16
	li	a0, 70
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	fmv.w.x	fa0, zero
	fsw	fa0, -64(s0)
	addi	t1, s0, -36
	fsw	fa0, 0(t1)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
.L_wh_main_14:
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t0, s0, -20
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lla	a0, B
	sd	a0, -80(s0)
	lw	a0, -64(s0)
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	sw	a0, -104(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_main_15
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_16:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 12
	sw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_main_17
	flw	fa0, -36(s0)
	fsw	fa0, -168(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -184(s0)
	lw	t2, -144(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	ld	t2, -80(s0)
	add	a0, t2, a0
	sd	a0, -200(s0)
	mv	t2, a0
	flw	fa0, 0(t2)
	fsw	fa0, -208(s0)
	flw	fa0, -168(s0)
	fmv.s	ft8, fa0
	flw	fa0, -208(s0)
	fadd.s	fa0, ft8, fa0
	fsw	fa0, -216(s0)
	addi	t1, s0, -36
	fsw	fa0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	addiw	a0, a0, 1
	sw	a0, -232(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_16
.L_whe_main_17:
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_main_14
.L_whe_main_15:
	flw	fa0, -36(s0)
	fsw	fa0, -56(s0)
	flw	fa0, -56(s0)
	fmv.s	ft0, fa0
	fmv.s	fa0, ft0
	call	putfloat
	li	a0, 10
	sw	a0, -64(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	j	.Lreturn_main_2
	li	a0, 0
.Lreturn_main_2:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
	.section	.rodata
	.align	2
.Lfloat_1:
	.word	1065353216
