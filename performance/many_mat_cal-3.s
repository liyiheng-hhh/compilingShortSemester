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
	.globl	main
	.type	main, @function
main:
	li	t6, -2560
	add	sp, sp, t6
	li	t0, 2560
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	addi	t1, s0, -32
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	addi	t1, s0, -40
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
	addi	t1, s0, -64
	sw	a0, 0(t1)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t1, s0, -76
	sw	a0, 0(t1)
	addi	t1, s0, -80
	sw	a0, 0(t1)
	addi	t1, s0, -84
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	call	getint
	sw	a0, -112(s0)
	call	getint
	sw	a0, -120(s0)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_0:
	li	a0, 2
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	li	a0, 1
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_1
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	beqz	a0, .L_ifend_main_2
	lla	a0, A
	sd	a0, -128(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	slliw	a0, a0, 12
	sw	a0, -184(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	ld	a0, -192(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -200(s0)
.L_ifend_main_2:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_3:
	li	a0, 2
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	li	a0, 1
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_4
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -168(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_ifend_main_5
	lla	a0, B
	sd	a0, -128(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	slliw	a0, a0, 12
	sw	a0, -192(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -200(s0)
	ld	a0, -200(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -208(s0)
.L_ifend_main_5:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	addiw	a0, a0, 1
	sw	a0, -216(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_3
.L_whe_main_4:
	li	a0, 25
	sw	a0, -104(s0)
	addi	sp, sp, -16
	li	a0, 25
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_6:
	li	a0, 2
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	li	a0, 1
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_7
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -168(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_ifend_main_8
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_main_9:
	lla	a0, A
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	sd	a0, -184(s0)
	li	a0, -1
	sw	a0, -192(s0)
	ld	a0, -184(s0)
	sd	a0, -184(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	slliw	a0, a0, 12
	sw	a0, -208(s0)
	li	a0, 1
	sw	a0, -216(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	beqz	a0, .L_whe_main_10
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	slliw	a0, a0, 2
	sw	a0, -248(s0)
	lw	t2, -208(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	ld	t2, -184(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	mv	t1, a0
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addiw	a0, a0, 1
	sw	a0, -280(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_main_9
.L_whe_main_10:
.L_ifend_main_8:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_6
.L_whe_main_7:
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_11:
	li	a0, 2
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -128(s0)
	li	a0, 1
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_12
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	beqz	a0, .L_ifend_main_13
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
.L_wh_main_14:
	li	a0, -1
	sw	a0, -176(s0)
	lla	a0, B
	sd	a0, -184(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	slliw	a0, a0, 12
	sw	a0, -200(s0)
	li	a0, 1
	sw	a0, -208(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	beqz	a0, .L_whe_main_15
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	a0, a0, 2
	sw	a0, -240(s0)
	lw	t2, -200(s0)
	addw	a0, t2, a0
	sw	a0, -248(s0)
	ld	t2, -184(s0)
	add	a0, t2, a0
	sd	a0, -256(s0)
	mv	t1, a0
	lw	a0, -176(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -264(s0)
	addiw	a0, a0, 1
	sw	a0, -272(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh_main_14
.L_whe_main_15:
.L_ifend_main_13:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -184(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_11
.L_whe_main_12:
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -136(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -144(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
.L_wh_main_16:
	li	a0, 0
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sw	a0, -128(s0)
	li	a0, 1
	sw	a0, -136(s0)
	lw	a0, -112(s0)
	sw	a0, -144(s0)
	li	a0, 32
	sw	a0, -152(s0)
	sw	a0, -160(s0)
	lw	a0, -152(s0)
	sw	a0, -168(s0)
	lw	a0, -136(s0)
	sw	a0, -176(s0)
	lw	a0, -112(s0)
	sw	a0, -184(s0)
	lw	a0, -136(s0)
	sw	a0, -192(s0)
	lw	a0, -152(s0)
	sw	a0, -200(s0)
	lla	a0, A
	sd	a0, -208(s0)
	lw	a0, -104(s0)
	sw	a0, -216(s0)
	li	a0, 2
	sw	a0, -224(s0)
	lla	a0, B
	sd	a0, -232(s0)
	lw	a0, -104(s0)
	sw	a0, -240(s0)
	li	a0, 3
	sw	a0, -248(s0)
	lla	a0, C
	sd	a0, -256(s0)
	lw	a0, -104(s0)
	sw	a0, -264(s0)
	lw	a0, -136(s0)
	sw	a0, -272(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	beqz	a0, .L_whe_main_17
	addi	t1, s0, -44
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_main_18:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	addiw	a0, a0, 32
	sw	a0, -312(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -328(s0)
	lw	a0, -328(s0)
	beqz	a0, .L_whe_main_19
	addi	t1, s0, -28
	lw	a0, -296(s0)
	sw	a0, 0(t1)
.L_wh_main_20:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -336(s0)
	addiw	a0, a0, 32
	sw	a0, -344(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_main_21
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -352(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -144(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -360(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -368(s0)
	lw	a0, -368(s0)
	beqz	a0, .L_ifend_main_22
	j	.L_whe_main_21
.L_ifend_main_22:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -312(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -384(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -392(s0)
	lw	a0, -392(s0)
	beqz	a0, .L_ifend_main_23
	j	.L_whe_main_21
.L_ifend_main_23:
	addi	t1, s0, -48
	lw	a0, -296(s0)
	sw	a0, 0(t1)
.L_wh_main_24:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	slliw	a0, a0, 12
	sw	a0, -400(s0)
	addiw	a0, a0, 0
	sw	a0, -408(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -416(s0)
	slliw	a0, a0, 12
	sw	a0, -424(s0)
	addiw	a0, a0, 0
	sw	a0, -432(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -440(s0)
	slliw	a0, a0, 12
	sw	a0, -448(s0)
	addiw	a0, a0, 0
	sw	a0, -456(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_main_25
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -464(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -184(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -472(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -480(s0)
	lw	a0, -480(s0)
	beqz	a0, .L_ifend_main_26
	j	.L_whe_main_25
.L_ifend_main_26:
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -488(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -344(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -496(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -504(s0)
	lw	a0, -504(s0)
	beqz	a0, .L_ifend_main_27
	j	.L_whe_main_25
.L_ifend_main_27:
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -512(s0)
	slliw	a0, a0, 2
	sw	a0, -520(s0)
	lw	t2, -408(s0)
	addw	a0, t2, a0
	sw	a0, -528(s0)
	ld	t2, -208(s0)
	add	a0, t2, a0
	sd	a0, -536(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -544(s0)
	slliw	a0, a0, 1
	sw	a0, -552(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -560(s0)
	slliw	a0, a0, 2
	sw	a0, -568(s0)
	lw	t2, -432(s0)
	addw	a0, t2, a0
	sw	a0, -576(s0)
	ld	t2, -232(s0)
	add	a0, t2, a0
	sd	a0, -584(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -592(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -600(s0)
	lw	t2, -552(s0)
	addw	a0, t2, a0
	sw	a0, -608(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -616(s0)
	slliw	a0, a0, 2
	sw	a0, -624(s0)
	lw	t2, -456(s0)
	addw	a0, t2, a0
	sw	a0, -632(s0)
	ld	t2, -256(s0)
	add	a0, t2, a0
	sd	a0, -640(s0)
	mv	t1, a0
	lw	a0, -608(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -648(s0)
	addiw	a0, a0, 1
	sw	a0, -656(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_main_24
.L_whe_main_25:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	addiw	a0, a0, 1
	sw	a0, -408(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_20
.L_whe_main_21:
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	addiw	a0, a0, 32
	sw	a0, -432(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	j	.L_wh_main_18
.L_whe_main_19:
	addi	t0, s0, -40
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	addiw	a0, a0, 32
	sw	a0, -144(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	j	.L_wh_main_16
.L_whe_main_17:
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_28:
	li	a0, 0
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sw	a0, -128(s0)
	lla	a0, C
	sd	a0, -136(s0)
	lw	a0, -104(s0)
	sw	a0, -144(s0)
	li	a0, 1
	sw	a0, -152(s0)
	li	a0, 7
	sw	a0, -160(s0)
	li	a0, 3
	sw	a0, -168(s0)
	ld	a0, -136(s0)
	sd	a0, -176(s0)
	lw	a0, -104(s0)
	sw	a0, -184(s0)
	lw	a0, -152(s0)
	sw	a0, -192(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	beqz	a0, .L_whe_main_29
	addi	t1, s0, -52
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_main_30:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	slliw	a0, a0, 12
	sw	a0, -224(s0)
	addiw	a0, a0, 0
	sw	a0, -232(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	slliw	a0, a0, 12
	sw	a0, -248(s0)
	addiw	a0, a0, 0
	sw	a0, -256(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -264(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -272(s0)
	lw	a0, -272(s0)
	beqz	a0, .L_whe_main_31
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	slliw	a0, a0, 2
	sw	a0, -288(s0)
	lw	t2, -232(s0)
	addw	a0, t2, a0
	sw	a0, -296(s0)
	ld	t2, -136(s0)
	add	a0, t2, a0
	sd	a0, -304(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -312(s0)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	mv	t2, a0
	mulw	a0, t2, a0
	sw	a0, -320(s0)
	addiw	a0, a0, 7
	sw	a0, -328(s0)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	sext.w	a0, a0
	li	t1, -1431655765
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	addw	t2, t2, a0
	sraiw	t2, t2, 1
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	sw	a0, -336(s0)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	slliw	a0, a0, 2
	sw	a0, -352(s0)
	lw	t2, -256(s0)
	addw	a0, t2, a0
	sw	a0, -360(s0)
	ld	t2, -176(s0)
	add	a0, t2, a0
	sd	a0, -368(s0)
	mv	t1, a0
	lw	a0, -336(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	addiw	a0, a0, 1
	sw	a0, -384(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	j	.L_wh_main_30
.L_whe_main_31:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_28
.L_whe_main_29:
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_main_32:
	li	a0, 0
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sw	a0, -128(s0)
	lw	a0, -104(s0)
	sw	a0, -136(s0)
	lw	a0, -104(s0)
	sw	a0, -144(s0)
	lw	a0, -112(s0)
	sw	a0, -152(s0)
	li	a0, 1
	sw	a0, -160(s0)
	lla	a0, C
	sd	a0, -168(s0)
	lw	a0, -104(s0)
	sw	a0, -176(s0)
	lla	a0, A
	sd	a0, -184(s0)
	lw	a0, -104(s0)
	sw	a0, -192(s0)
	lw	a0, -160(s0)
	sw	a0, -200(s0)
	ld	a0, -184(s0)
	sd	a0, -208(s0)
	lw	a0, -104(s0)
	sw	a0, -216(s0)
	lw	a0, -160(s0)
	sw	a0, -224(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_main_33
	addi	t1, s0, -60
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_main_34:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	slliw	a0, a0, 12
	sw	a0, -264(s0)
	addiw	a0, a0, 0
	sw	a0, -272(s0)
	lw	a0, -248(s0)
	slliw	a0, a0, 12
	sw	a0, -280(s0)
	addiw	a0, a0, 0
	sw	a0, -288(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -304(s0)
	lw	a0, -304(s0)
	beqz	a0, .L_whe_main_35
	addi	t1, s0, -64
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -68
	lw	a0, -144(s0)
	sw	a0, 0(t1)
.L_wh_main_36:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	slliw	a0, a0, 2
	sw	a0, -320(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -152(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	beqz	a0, .L_whe_main_37
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -352(s0)
	slliw	a0, a0, 2
	sw	a0, -360(s0)
	lw	t2, -288(s0)
	addw	a0, t2, a0
	sw	a0, -368(s0)
	ld	t2, -168(s0)
	add	a0, t2, a0
	sd	a0, -376(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -384(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -392(s0)
	slliw	a0, a0, 12
	sw	a0, -400(s0)
	addiw	a0, a0, 0
	sw	a0, -408(s0)
	mv	t2, a0
	lw	a0, -320(s0)
	addw	a0, t2, a0
	sw	a0, -416(s0)
	ld	t2, -208(s0)
	add	a0, t2, a0
	sd	a0, -424(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -432(s0)
	lw	t2, -384(s0)
	mulw	a0, t2, a0
	sw	a0, -440(s0)
	lw	t2, -344(s0)
	addw	a0, t2, a0
	sw	a0, -448(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -456(s0)
	addiw	a0, a0, 1
	sw	a0, -464(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	j	.L_wh_main_36
.L_whe_main_37:
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	slliw	a0, a0, 2
	sw	a0, -472(s0)
	lw	t2, -272(s0)
	addw	a0, t2, a0
	sw	a0, -480(s0)
	ld	t2, -184(s0)
	add	a0, t2, a0
	sd	a0, -488(s0)
	mv	t1, a0
	lw	a0, -288(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -496(s0)
	addiw	a0, a0, 1
	sw	a0, -504(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	j	.L_wh_main_34
.L_whe_main_35:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	addiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_32
.L_whe_main_33:
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -128(s0)
	addi	t1, s0, -76
	sw	a0, 0(t1)
.L_wh_main_38:
	li	a0, 0
	sw	a0, -104(s0)
	lw	a0, -112(s0)
	sw	a0, -128(s0)
	lw	a0, -104(s0)
	sw	a0, -136(s0)
	lw	a0, -112(s0)
	sw	a0, -144(s0)
	li	a0, 1
	sw	a0, -152(s0)
	sw	a0, -160(s0)
	lw	a0, -112(s0)
	sw	a0, -168(s0)
	li	a0, 32
	sw	a0, -176(s0)
	sw	a0, -184(s0)
	lw	a0, -176(s0)
	sw	a0, -192(s0)
	lw	a0, -152(s0)
	sw	a0, -200(s0)
	lw	a0, -112(s0)
	sw	a0, -208(s0)
	lw	a0, -152(s0)
	sw	a0, -216(s0)
	lw	a0, -176(s0)
	sw	a0, -224(s0)
	lla	a0, A
	sd	a0, -232(s0)
	lw	a0, -104(s0)
	sw	a0, -240(s0)
	ld	a0, -232(s0)
	sd	a0, -248(s0)
	lw	a0, -104(s0)
	sw	a0, -256(s0)
	lw	a0, -152(s0)
	sw	a0, -264(s0)
	addi	t0, s0, -76
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -280(s0)
	lw	a0, -280(s0)
	beqz	a0, .L_whe_main_39
	addi	t1, s0, -80
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -84
	sw	a0, 0(t1)
	addi	t1, s0, -80
	sw	a0, 0(t1)
.L_wh_main_40:
	addi	t0, s0, -80
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	beqz	a0, .L_whe_main_41
	addi	t1, s0, -84
	lw	a0, -136(s0)
	sw	a0, 0(t1)
.L_wh_main_42:
	addi	t0, s0, -80
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	addi	t0, s0, -80
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	addiw	a0, a0, 32
	sw	a0, -320(s0)
	addi	t0, s0, -84
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -144(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	beqz	a0, .L_whe_main_43
	addi	t1, s0, -28
	lw	a0, -304(s0)
	sw	a0, 0(t1)
.L_wh_main_44:
	addi	t0, s0, -84
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	addi	t0, s0, -84
	lw	a0, 0(t0)
	sw	a0, -344(s0)
	addiw	a0, a0, 32
	sw	a0, -352(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_main_45
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -360(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -168(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -368(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -376(s0)
	lw	a0, -376(s0)
	beqz	a0, .L_ifend_main_46
	j	.L_whe_main_45
.L_ifend_main_46:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -384(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -320(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -392(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -400(s0)
	lw	a0, -400(s0)
	beqz	a0, .L_ifend_main_47
	j	.L_whe_main_45
.L_ifend_main_47:
	addi	t1, s0, -48
	lw	a0, -304(s0)
	sw	a0, 0(t1)
.L_wh_main_48:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	slliw	a0, a0, 12
	sw	a0, -408(s0)
	addiw	a0, a0, 0
	sw	a0, -416(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -424(s0)
	slliw	a0, a0, 12
	sw	a0, -432(s0)
	addiw	a0, a0, 0
	sw	a0, -440(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_whe_main_49
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -448(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -208(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -456(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -464(s0)
	lw	a0, -464(s0)
	beqz	a0, .L_ifend_main_50
	j	.L_whe_main_49
.L_ifend_main_50:
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -472(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -352(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -480(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -488(s0)
	lw	a0, -488(s0)
	beqz	a0, .L_ifend_main_51
	j	.L_whe_main_49
.L_ifend_main_51:
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -496(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -504(s0)
	slliw	a0, a0, 2
	sw	a0, -512(s0)
	lw	t2, -416(s0)
	addw	a0, t2, a0
	sw	a0, -520(s0)
	ld	t2, -232(s0)
	add	a0, t2, a0
	sd	a0, -528(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -536(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -544(s0)
	slliw	a0, a0, 2
	sw	a0, -552(s0)
	lw	t2, -440(s0)
	addw	a0, t2, a0
	sw	a0, -560(s0)
	ld	t2, -248(s0)
	add	a0, t2, a0
	sd	a0, -568(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -576(s0)
	lw	t2, -536(s0)
	mulw	a0, t2, a0
	sw	a0, -584(s0)
	lw	t2, -496(s0)
	addw	a0, t2, a0
	sw	a0, -592(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -600(s0)
	addiw	a0, a0, 1
	sw	a0, -608(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_main_48
.L_whe_main_49:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -352(s0)
	addiw	a0, a0, 1
	sw	a0, -416(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_main_44
.L_whe_main_45:
	addi	t0, s0, -84
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	addiw	a0, a0, 32
	sw	a0, -440(s0)
	addi	t1, s0, -84
	sw	a0, 0(t1)
	j	.L_wh_main_42
.L_whe_main_43:
	addi	t0, s0, -80
	lw	a0, 0(t0)
	sw	a0, -616(s0)
	addiw	a0, a0, 32
	sw	a0, -624(s0)
	addi	t1, s0, -80
	sw	a0, 0(t1)
	j	.L_wh_main_40
.L_whe_main_41:
	addi	t0, s0, -76
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	addiw	a0, a0, 1
	sw	a0, -136(s0)
	addi	t1, s0, -76
	sw	a0, 0(t1)
	j	.L_wh_main_38
.L_whe_main_39:
	li	a0, 105
	sw	a0, -104(s0)
	addi	sp, sp, -16
	li	a0, 105
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -120(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
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
