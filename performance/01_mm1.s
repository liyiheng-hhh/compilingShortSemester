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
	.globl	mm
	.type	mm, @function
mm:
	addi	sp, sp, -624
	addi	t0, sp, 624
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -568(s0)
	sd	s2, -576(s0)
	sd	s3, -584(s0)
	sd	s4, -592(s0)
	sd	s5, -600(s0)
	sd	s6, -608(s0)
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	sd	a3, -48(s0)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -52
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -56
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -52
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -56
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_mm_0:
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	ld	a0, -48(s0)
	sd	a0, -88(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -52(s0)
	lw	t2, -52(s0)
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_mm_1
	addi	t1, s0, -56
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_mm_2:
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	addiw	a0, a0, 0
	sw	a0, -144(s0)
	lw	a0, -56(s0)
	lw	t2, -56(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_mm_3
	lw	a0, -56(s0)
	slliw	a0, a0, 2
	sw	a0, -176(s0)
	lw	t2, -144(s0)
	lw	a0, -176(s0)
	addw	a0, t2, a0
	sw	a0, -184(s0)
	ld	t2, -88(s0)
	lw	a0, -184(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	ld	t1, -192(s0)
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	lw	a0, -56(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -56
	lw	a0, -208(s0)
	sw	a0, 0(t1)
	j	.L_wh_mm_2
.L_whe_mm_3:
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -52
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	j	.L_wh_mm_0
.L_whe_mm_1:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -52
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -56
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_mm_4:
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	ld	a0, -32(s0)
	sd	a0, -88(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -60(s0)
	lw	t2, -60(s0)
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_mm_5
	addi	t1, s0, -52
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_mm_6:
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	lw	a0, -52(s0)
	lw	t2, -52(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_mm_7
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	addiw	a0, a0, 0
	sw	a0, -176(s0)
	lw	t2, -176(s0)
	lw	a0, -136(s0)
	addw	a0, t2, a0
	sw	a0, -184(s0)
	ld	t2, -88(s0)
	lw	a0, -184(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	ld	t2, -192(s0)
	lw	a0, 0(t2)
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	addiw	a0, a0, -1
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	beqz	a0, .L_ifend_mm_8
	li	a0, 1
	sw	a0, -136(s0)
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -232(s0)
	addi	t1, s0, -52
	lw	a0, -232(s0)
	sw	a0, 0(t1)
	j	.L_wh_mm_6
.L_ifend_mm_8:
	li	a0, 0
	sw	a0, -240(s0)
	addi	t1, s0, -56
	lw	a0, -240(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -248(s0)
	addi	t1, s0, -64
	lw	a0, -248(s0)
	sw	a0, 0(t1)
	ld	a0, -32(s0)
	sd	a0, -256(s0)
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	sw	a0, -272(s0)
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	mv	s1, a0
	sw	s1, -288(s0)
	lw	t2, -272(s0)
	mv	a0, s1
	addw	a0, t2, a0
	sw	a0, -296(s0)
	ld	t2, -256(s0)
	lw	a0, -296(s0)
	add	a0, t2, a0
	sd	a0, -304(s0)
	ld	t2, -304(s0)
	lw	a0, 0(t2)
	sw	a0, -312(s0)
	addi	t1, s0, -64
	lw	a0, -312(s0)
	sw	a0, 0(t1)
.L_wh_mm_9:
	li	a0, 1
	sw	a0, -320(s0)
	ld	a0, -48(s0)
	sd	a0, -328(s0)
	ld	a0, -40(s0)
	sd	a0, -336(s0)
	lw	a0, -20(s0)
	lw	a0, -52(s0)
	mv	s5, a0
	sw	s5, -352(s0)
	mv	a0, s5
	slliw	a0, a0, 12
	sw	a0, -360(s0)
	lw	a0, -64(s0)
	lw	a0, -60(s0)
	slliw	a0, a0, 12
	sw	a0, -384(s0)
	lw	a0, -52(s0)
	slliw	a0, a0, 12
	sw	a0, -400(s0)
	lw	a0, -56(s0)
	lw	t2, -56(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s6, a0
	sw	s6, -416(s0)
	mv	a0, s6
	beqz	a0, .L_whe_mm_10
	lw	a0, -56(s0)
	mv	s4, a0
	sw	s4, -424(s0)
	mv	a0, s4
	slliw	a0, a0, 2
	mv	s3, a0
	sw	s3, -432(s0)
	lw	t2, -360(s0)
	mv	a0, s3
	addw	a0, t2, a0
	mv	s2, a0
	sw	s2, -440(s0)
	ld	t2, -328(s0)
	mv	a0, s2
	add	a0, t2, a0
	sd	a0, -448(s0)
	ld	t2, -448(s0)
	lw	a0, 0(t2)
	sw	a0, -456(s0)
	lw	t2, -456(s0)
	lw	a0, -64(s0)
	mulw	a0, t2, a0
	sw	a0, -464(s0)
	lw	a0, -56(s0)
	slliw	a0, a0, 2
	sw	a0, -480(s0)
	lw	t2, -384(s0)
	lw	a0, -480(s0)
	addw	a0, t2, a0
	sw	a0, -488(s0)
	ld	t2, -336(s0)
	lw	a0, -488(s0)
	add	a0, t2, a0
	sd	a0, -496(s0)
	ld	t2, -496(s0)
	lw	a0, 0(t2)
	sw	a0, -504(s0)
	lw	t2, -464(s0)
	lw	a0, -504(s0)
	addw	a0, t2, a0
	sw	a0, -512(s0)
	lw	a0, -56(s0)
	slliw	a0, a0, 2
	sw	a0, -528(s0)
	lw	t2, -400(s0)
	lw	a0, -528(s0)
	addw	a0, t2, a0
	sw	a0, -536(s0)
	ld	t2, -328(s0)
	lw	a0, -536(s0)
	add	a0, t2, a0
	sd	a0, -544(s0)
	ld	t1, -544(s0)
	lw	a0, -512(s0)
	sw	a0, 0(t1)
	lw	a0, -56(s0)
	addiw	a0, a0, 1
	sw	a0, -560(s0)
	addi	t1, s0, -56
	lw	a0, -560(s0)
	sw	a0, 0(t1)
	j	.L_wh_mm_9
.L_whe_mm_10:
	li	a0, 1
	sw	a0, -320(s0)
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -336(s0)
	addi	t1, s0, -52
	lw	a0, -336(s0)
	sw	a0, 0(t1)
	j	.L_wh_mm_6
.L_whe_mm_7:
	lw	a0, -60(s0)
	addiw	a0, a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	j	.L_wh_mm_4
.L_whe_mm_5:
	j	.Lreturn_mm_0
.Lreturn_mm_0:
	ld	s6, -608(s0)
	ld	s5, -600(s0)
	ld	s4, -592(s0)
	ld	s3, -584(s0)
	ld	s2, -576(s0)
	ld	s1, -568(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	mm, .-mm
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -304
	addi	t0, sp, 304
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -208(s0)
	sd	s2, -216(s0)
	sd	s3, -224(s0)
	sd	s4, -232(s0)
	sd	s5, -240(s0)
	sd	s6, -248(s0)
	sd	s7, -256(s0)
	sd	s8, -264(s0)
	sd	s9, -272(s0)
	sd	s10, -280(s0)
	sd	s11, -288(s0)
	call	getint
	sw	a0, -40(s0)
	addi	t1, s0, -20
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
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
.L_wh_main_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	a0, A
	sd	a0, -56(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_1
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_main_2:
	lw	a0, -24(s0)
	slliw	a0, a0, 12
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	addiw	a0, a0, 0
	sw	a0, -112(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_whe_main_3
	call	getint
	sw	a0, -136(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -152(s0)
	lw	t2, -112(s0)
	lw	a0, -152(s0)
	addw	a0, t2, a0
	sw	a0, -160(s0)
	ld	t2, -56(s0)
	lw	a0, -160(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	ld	t1, -168(s0)
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -28
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -24
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -28
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_main_4:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	a0, B
	sd	a0, -56(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	beqz	a0, .L_whe_main_5
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_main_6:
	lw	a0, -24(s0)
	slliw	a0, a0, 12
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	addiw	a0, a0, 0
	sw	a0, -112(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_whe_main_7
	call	getint
	sw	a0, -136(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -152(s0)
	lw	t2, -112(s0)
	lw	a0, -152(s0)
	addw	a0, t2, a0
	sw	a0, -160(s0)
	ld	t2, -56(s0)
	lw	a0, -160(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	ld	t1, -168(s0)
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -28
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_6
.L_whe_main_7:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_4
.L_whe_main_5:
	li	a0, 65
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 65
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_main_8:
	li	a0, 5
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	a0, A
	sd	a0, -56(s0)
	lla	a0, B
	sd	a0, -64(s0)
	lla	a0, C
	sd	a0, -72(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -40(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_whe_main_9
	addi	sp, sp, -32
	lw	a0, -80(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -56(s0)
	sd	a0, 8(sp)
	ld	a0, -64(s0)
	sd	a0, 16(sp)
	ld	a0, -72(s0)
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	mm
	addi	sp, sp, 32
	addi	sp, sp, -32
	lw	a0, -88(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -56(s0)
	sd	a0, 8(sp)
	ld	a0, -72(s0)
	sd	a0, 16(sp)
	ld	a0, -64(s0)
	sd	a0, 24(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	call	mm
	addi	sp, sp, 32
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -24
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_8
.L_whe_main_9:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -24
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_main_10:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lla	a0, B
	sd	a0, -56(s0)
	lw	a0, -20(s0)
	mv	s3, a0
	sw	s3, -64(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	mv	s2, a0
	sw	s2, -80(s0)
	mv	t2, s2
	sext.w	t2, t2
	mv	a0, s3
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s4, a0
	sw	s4, -88(s0)
	mv	a0, s4
	beqz	a0, .L_whe_main_11
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_main_12:
	lw	a0, -24(s0)
	mv	s8, a0
	sw	s8, -96(s0)
	mv	a0, s8
	slliw	a0, a0, 12
	mv	s9, a0
	sw	s9, -104(s0)
	mv	a0, s9
	addiw	a0, a0, 0
	sw	a0, -112(s0)
	lw	a0, -28(s0)
	mv	s5, a0
	sw	s5, -120(s0)
	mv	t2, s5
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s6, a0
	sw	s6, -128(s0)
	mv	a0, s6
	beqz	a0, .L_whe_main_13
	lw	a0, -32(s0)
	mv	s7, a0
	sw	s7, -136(s0)
	lw	a0, -28(s0)
	mv	s10, a0
	sw	s10, -144(s0)
	mv	a0, s10
	slliw	a0, a0, 2
	mv	s11, a0
	sw	s11, -152(s0)
	lw	t2, -112(s0)
	mv	a0, s11
	addw	a0, t2, a0
	sw	a0, -160(s0)
	ld	t2, -56(s0)
	lw	a0, -160(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	ld	t2, -168(s0)
	lw	a0, 0(t2)
	sw	a0, -176(s0)
	mv	t2, s7
	lw	a0, -176(s0)
	addw	a0, t2, a0
	sw	a0, -184(s0)
	addi	t1, s0, -32
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -200(s0)
	addi	t1, s0, -28
	lw	a0, -200(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_12
.L_whe_main_13:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	addi	t1, s0, -24
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	j	.L_wh_main_10
.L_whe_main_11:
	li	a0, 84
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 84
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -32(s0)
	lw	a0, -48(s0)
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -56(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	mv	s1, a0
	sw	s1, -64(s0)
	mv	a0, s1
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	s11, -288(s0)
	ld	s10, -280(s0)
	ld	s9, -272(s0)
	ld	s8, -264(s0)
	ld	s7, -256(s0)
	ld	s6, -248(s0)
	ld	s5, -240(s0)
	ld	s4, -232(s0)
	ld	s3, -224(s0)
	ld	s2, -216(s0)
	ld	s1, -208(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
