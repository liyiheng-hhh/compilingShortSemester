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
	addi	sp, sp, -736
	addi	t0, sp, 736
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -680(s0)
	sd	s2, -688(s0)
	sd	s3, -696(s0)
	sd	s4, -704(s0)
	sd	s5, -712(s0)
	sd	s6, -720(s0)
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -44
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_trsm_optimized_0:
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	lla	t0, .Lfloat_1
	flw	fa0, 0(t0)
	fmv.s	fs3, fa0
	fsw	fs3, -88(s0)
	ld	a0, -40(s0)
	sd	a0, -96(s0)
	ld	a0, -32(s0)
	sd	a0, -104(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -44(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_trsm_optimized_1
	addi	t1, s0, -48
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_trsm_optimized_2:
	lw	a0, -44(s0)
	slliw	a0, a0, 12
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	addiw	a0, a0, 0
	sw	a0, -176(s0)
	lw	a0, -44(s0)
	slliw	a0, a0, 12
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	addiw	a0, a0, 0
	sw	a0, -200(s0)
	lw	a0, -44(s0)
	slliw	a0, a0, 2
	sw	a0, -216(s0)
	lw	t2, -200(s0)
	lw	a0, -216(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	ld	t2, -104(s0)
	lw	a0, -224(s0)
	add	a0, t2, a0
	sd	a0, -232(s0)
	lw	a0, -44(s0)
	slliw	a0, a0, 12
	sw	a0, -248(s0)
	lw	a0, -248(s0)
	addiw	a0, a0, 0
	sw	a0, -256(s0)
	lw	a0, -48(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -272(s0)
	lw	a0, -272(s0)
	beqz	a0, .L_whe_trsm_optimized_3
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -288(s0)
	lw	t2, -176(s0)
	lw	a0, -288(s0)
	addw	a0, t2, a0
	sw	a0, -296(s0)
	ld	t2, -96(s0)
	lw	a0, -296(s0)
	add	a0, t2, a0
	sd	a0, -304(s0)
	ld	t2, -304(s0)
	flw	fa0, 0(t2)
	fmv.s	fs0, fa0
	fsw	fs0, -312(s0)
	ld	t2, -232(s0)
	flw	fa0, 0(t2)
	fmv.s	fs1, fa0
	fsw	fs1, -320(s0)
	fmv.s	fa0, fs0
	fmv.s	ft8, fa0
	fmv.s	fa0, fs1
	fdiv.s	fa0, ft8, fa0
	fmv.s	fs2, fa0
	fsw	fs2, -328(s0)
	fmv.s	fa0, fs2
	fmv.s	ft8, fa0
	fmv.s	fa0, fs3
	fadd.s	fa0, ft8, fa0
	fmv.s	fs4, fa0
	fsw	fs4, -336(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -352(s0)
	lw	t2, -256(s0)
	lw	a0, -352(s0)
	addw	a0, t2, a0
	sw	a0, -360(s0)
	ld	t2, -96(s0)
	lw	a0, -360(s0)
	add	a0, t2, a0
	sd	a0, -368(s0)
	ld	t1, -368(s0)
	fmv.s	fa0, fs4
	fsw	fa0, 0(t1)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -384(s0)
	addi	t1, s0, -48
	lw	a0, -384(s0)
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_2
.L_whe_trsm_optimized_3:
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -52
	lw	a0, -120(s0)
	sw	a0, 0(t1)
.L_wh_trsm_optimized_4:
	lw	a0, -44(s0)
	mv	s4, a0
	sw	s4, -176(s0)
	mv	a0, s4
	slliw	a0, a0, 2
	sw	a0, -232(s0)
	lw	a0, -44(s0)
	mv	s2, a0
	sw	s2, -256(s0)
	mv	a0, s2
	slliw	a0, a0, 12
	sw	a0, -392(s0)
	lw	a0, -392(s0)
	addiw	a0, a0, 0
	sw	a0, -400(s0)
	lw	a0, -52(s0)
	lw	t2, -52(s0)
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -416(s0)
	lw	a0, -416(s0)
	beqz	a0, .L_whe_trsm_optimized_5
	addi	t1, s0, -48
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_trsm_optimized_6:
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	sw	a0, -432(s0)
	lw	a0, -432(s0)
	addiw	a0, a0, 0
	sw	a0, -440(s0)
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	mv	s6, a0
	sw	s6, -456(s0)
	mv	a0, s6
	mv	s5, a0
	mv	s5, a0
	sw	s5, -464(s0)
	mv	t2, s5
	lw	a0, -232(s0)
	addw	a0, t2, a0
	mv	s3, a0
	sw	s3, -472(s0)
	ld	t2, -104(s0)
	mv	a0, s3
	add	a0, t2, a0
	sd	a0, -480(s0)
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	sw	a0, -496(s0)
	lw	a0, -496(s0)
	addiw	a0, a0, 0
	sw	a0, -504(s0)
	lw	a0, -48(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -520(s0)
	lw	a0, -520(s0)
	beqz	a0, .L_whe_trsm_optimized_7
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	mv	s1, a0
	sw	s1, -536(s0)
	lw	t2, -440(s0)
	mv	a0, s1
	addw	a0, t2, a0
	sw	a0, -544(s0)
	ld	t2, -96(s0)
	lw	a0, -544(s0)
	add	a0, t2, a0
	sd	a0, -552(s0)
	ld	t2, -552(s0)
	flw	fa0, 0(t2)
	fmv.s	fs5, fa0
	fsw	fs5, -560(s0)
	ld	t2, -480(s0)
	flw	fa0, 0(t2)
	fmv.s	fs6, fa0
	fsw	fs6, -568(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -584(s0)
	lw	t2, -400(s0)
	lw	a0, -584(s0)
	addw	a0, t2, a0
	sw	a0, -592(s0)
	ld	t2, -96(s0)
	lw	a0, -592(s0)
	add	a0, t2, a0
	sd	a0, -600(s0)
	ld	t2, -600(s0)
	flw	fa0, 0(t2)
	fmv.s	fs7, fa0
	fsw	fs7, -608(s0)
	fmv.s	fa0, fs6
	fmv.s	ft8, fa0
	fmv.s	fa0, fs7
	fmul.s	fa0, ft8, fa0
	fmv.s	fs8, fa0
	fsw	fs8, -616(s0)
	fmv.s	fa0, fs5
	fmv.s	ft8, fa0
	fmv.s	fa0, fs8
	fsub.s	fa0, ft8, fa0
	fmv.s	fs9, fa0
	fsw	fs9, -624(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -640(s0)
	lw	t2, -504(s0)
	lw	a0, -640(s0)
	addw	a0, t2, a0
	sw	a0, -648(s0)
	ld	t2, -96(s0)
	lw	a0, -648(s0)
	add	a0, t2, a0
	sd	a0, -656(s0)
	ld	t1, -656(s0)
	fmv.s	fa0, fs9
	fsw	fa0, 0(t1)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -672(s0)
	addi	t1, s0, -48
	lw	a0, -672(s0)
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_6
.L_whe_trsm_optimized_7:
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -400(s0)
	addi	t1, s0, -52
	lw	a0, -400(s0)
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_4
.L_whe_trsm_optimized_5:
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -44
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_trsm_optimized_0
.L_whe_trsm_optimized_1:
	j	.Lreturn_trsm_optimized_0
.Lreturn_trsm_optimized_0:
	ld	s6, -720(s0)
	ld	s5, -712(s0)
	ld	s4, -704(s0)
	ld	s3, -696(s0)
	ld	s2, -688(s0)
	ld	s1, -680(s0)
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
	addi	sp, sp, -400
	addi	t0, sp, 400
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -312(s0)
	sd	s2, -320(s0)
	sd	s3, -328(s0)
	sd	s4, -336(s0)
	sd	s5, -344(s0)
	sd	s6, -352(s0)
	sd	s7, -360(s0)
	sd	s8, -368(s0)
	sd	s9, -376(s0)
	sd	s10, -384(s0)
	sd	s11, -392(s0)
	call	getint
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
	addi	t1, s0, -24
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -28
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lla	a0, A
	sd	a0, -72(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -80(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_main_2:
	lw	a0, -24(s0)
	slliw	a0, a0, 12
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	addiw	a0, a0, 0
	sw	a0, -128(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_whe_main_3
	call	getfloat
	fsw	fa0, -152(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -168(s0)
	lw	t2, -128(s0)
	lw	a0, -168(s0)
	addw	a0, t2, a0
	sw	a0, -176(s0)
	ld	t2, -72(s0)
	lw	a0, -176(s0)
	add	a0, t2, a0
	sd	a0, -184(s0)
	ld	t1, -184(s0)
	flw	fa0, -152(s0)
	fsw	fa0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -200(s0)
	addi	t1, s0, -28
	lw	a0, -200(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
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
.L_wh_main_4:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lla	a0, C
	sd	a0, -72(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -80(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_main_5
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_main_6:
	lw	a0, -24(s0)
	slliw	a0, a0, 12
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	addiw	a0, a0, 0
	sw	a0, -128(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_whe_main_7
	call	getfloat
	fsw	fa0, -152(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -168(s0)
	lw	t2, -128(s0)
	lw	a0, -168(s0)
	addw	a0, t2, a0
	sw	a0, -176(s0)
	ld	t2, -72(s0)
	lw	a0, -176(s0)
	add	a0, t2, a0
	sd	a0, -184(s0)
	ld	t1, -184(s0)
	flw	fa0, -152(s0)
	fsw	fa0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -200(s0)
	addi	t1, s0, -28
	lw	a0, -200(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_6
.L_whe_main_7:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
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
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_8:
	li	a0, 5
	sw	a0, -56(s0)
	li	a0, 0
	sw	a0, -64(s0)
	li	a0, 1
	sw	a0, -72(s0)
	lla	a0, C
	sd	a0, -80(s0)
	lla	a0, B
	sd	a0, -88(s0)
	lla	a0, A
	sd	a0, -96(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -32(s0)
	lw	t2, -32(s0)
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_main_9
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_10:
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_11
	addi	t1, s0, -28
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_main_12:
	lw	a0, -24(s0)
	slliw	a0, a0, 12
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	addiw	a0, a0, 0
	sw	a0, -176(s0)
	lw	a0, -24(s0)
	slliw	a0, a0, 12
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	addiw	a0, a0, 0
	sw	a0, -200(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	beqz	a0, .L_whe_main_13
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -232(s0)
	lw	t2, -176(s0)
	lw	a0, -232(s0)
	addw	a0, t2, a0
	sw	a0, -240(s0)
	ld	t2, -80(s0)
	lw	a0, -240(s0)
	add	a0, t2, a0
	sd	a0, -248(s0)
	ld	t2, -248(s0)
	flw	fa0, 0(t2)
	fmv.s	fs0, fa0
	fsw	fs0, -256(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -272(s0)
	lw	t2, -200(s0)
	lw	a0, -272(s0)
	addw	a0, t2, a0
	sw	a0, -280(s0)
	ld	t2, -88(s0)
	lw	a0, -280(s0)
	add	a0, t2, a0
	sd	a0, -288(s0)
	ld	t1, -288(s0)
	fmv.s	fa0, fs0
	fsw	fa0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -304(s0)
	addi	t1, s0, -28
	lw	a0, -304(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_12
.L_whe_main_13:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -200(s0)
	addi	t1, s0, -24
	lw	a0, -200(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_10
.L_whe_main_11:
	addi	sp, sp, -32
	lw	a0, -112(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -96(s0)
	sd	a0, 8(sp)
	ld	a0, -88(s0)
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	trsm_optimized
	addi	sp, sp, 32
	lw	a0, -32(s0)
	addiw	a0, a0, 1
	sw	a0, -80(s0)
	addi	t1, s0, -32
	lw	a0, -80(s0)
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
	fmv.s	fs1, fa0
	fsw	fs1, -64(s0)
	addi	t1, s0, -36
	fmv.s	fa0, fs1
	fsw	fa0, 0(t1)
	fmv.s	fs1, fa0
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_main_14:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	lla	a0, B
	sd	a0, -72(s0)
	lw	a0, -20(s0)
	mv	s3, a0
	sw	s3, -80(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -96(s0)
	mv	t2, s2
	sext.w	t2, t2
	mv	a0, s3
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s4, a0
	sw	s4, -104(s0)
	mv	a0, s4
	beqz	a0, .L_whe_main_15
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_main_16:
	lw	a0, -24(s0)
	mv	s7, a0
	sw	s7, -112(s0)
	mv	a0, s7
	slliw	a0, a0, 12
	mv	s8, a0
	sw	s8, -120(s0)
	mv	a0, s8
	addiw	a0, a0, 0
	sw	a0, -128(s0)
	lw	a0, -28(s0)
	mv	s5, a0
	sw	s5, -136(s0)
	mv	t2, s5
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s6, a0
	sw	s6, -144(s0)
	mv	a0, s6
	beqz	a0, .L_whe_main_17
	flw	fa0, -36(s0)
	fmv.s	fs2, fa0
	fsw	fs2, -152(s0)
	lw	a0, -28(s0)
	mv	s9, a0
	sw	s9, -160(s0)
	mv	a0, s9
	slliw	a0, a0, 2
	mv	s10, a0
	sw	s10, -168(s0)
	lw	t2, -128(s0)
	mv	a0, s10
	addw	a0, t2, a0
	mv	s11, a0
	sw	s11, -176(s0)
	ld	t2, -72(s0)
	mv	a0, s11
	add	a0, t2, a0
	sd	a0, -184(s0)
	ld	t2, -184(s0)
	flw	fa0, 0(t2)
	fmv.s	fs3, fa0
	fsw	fs3, -192(s0)
	fmv.s	fa0, fs2
	fmv.s	ft8, fa0
	fmv.s	fa0, fs3
	fadd.s	fa0, ft8, fa0
	fmv.s	fs4, fa0
	fsw	fs4, -200(s0)
	addi	t1, s0, -36
	fmv.s	fa0, fs4
	fsw	fa0, 0(t1)
	fmv.s	fs4, fa0
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -216(s0)
	addi	t1, s0, -28
	lw	a0, -216(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_16
.L_whe_main_17:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -24
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_14
.L_whe_main_15:
	flw	fa0, -36(s0)
	fmv.s	fs5, fa0
	fsw	fs5, -56(s0)
	fmv.s	fa0, fs5
	fmv.s	ft0, fa0
	fmv.s	fa0, ft0
	call	putfloat
	li	a0, 10
	sw	a0, -64(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	mv	s1, a0
	sw	s1, -72(s0)
	mv	a0, s1
	sext.w	a0, a0
	j	.Lreturn_main_2
	li	a0, 0
.Lreturn_main_2:
	ld	s11, -392(s0)
	ld	s10, -384(s0)
	ld	s9, -376(s0)
	ld	s8, -368(s0)
	ld	s7, -360(s0)
	ld	s6, -352(s0)
	ld	s5, -344(s0)
	ld	s4, -336(s0)
	ld	s3, -328(s0)
	ld	s2, -320(s0)
	ld	s1, -312(s0)
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
