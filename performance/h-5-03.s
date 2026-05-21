	.data
	.align	2
	.globl	n
n:
	.word	20
	.bss
	.align	2
	.globl	A
A:
	.zero	7840000
	.bss
	.align	2
	.globl	b
b:
	.zero	5600
	.bss
	.align	2
	.globl	x
x:
	.zero	5600
	.bss
	.align	2
	.globl	y
y:
	.zero	5600
	.text
	.text
	.align	1
	.globl	kernel_ludcmp
	.type	kernel_ludcmp, @function
kernel_ludcmp:
	addi	sp, sp, -944
	addi	t0, sp, 944
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -888(s0)
	sd	s2, -896(s0)
	sd	s3, -904(s0)
	sd	s4, -912(s0)
	sd	s5, -920(s0)
	sd	s6, -928(s0)
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	sd	a3, -48(s0)
	sd	a4, -56(s0)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -64
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -68
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -112(s0)
	addi	t1, s0, -72
	lw	a0, -112(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -120(s0)
	addi	t1, s0, -60
	lw	a0, -120(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_0:
	li	a0, 0
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	li	a0, 5600
	sw	a0, -104(s0)
	ld	a0, -32(s0)
	sd	a0, -112(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -60(s0)
	lw	t2, -60(s0)
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_whe_kernel_ludcmp_1
	addi	t1, s0, -64
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_2:
	lw	a0, -60(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -168(s0)
	lw	a0, -168(s0)
	addiw	a0, a0, 0
	sw	a0, -176(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	addiw	a0, a0, 0
	sw	a0, -200(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	addiw	a0, a0, 0
	sw	a0, -224(s0)
	lw	a0, -64(s0)
	lw	t2, -64(s0)
	sext.w	t2, t2
	lw	a0, -152(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_kernel_ludcmp_3
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -256(s0)
	lw	t2, -176(s0)
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
	addi	t1, s0, -72
	lw	a0, -280(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -68
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_4:
	lw	a0, -64(s0)
	lw	a0, -64(s0)
	addiw	a0, a0, -1
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	slliw	a0, a0, 2
	sw	a0, -304(s0)
	lw	a0, -68(s0)
	lw	t2, -68(s0)
	sext.w	t2, t2
	lw	a0, -176(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -320(s0)
	lw	a0, -320(s0)
	beqz	a0, .L_whe_kernel_ludcmp_5
	lw	a0, -72(s0)
	lw	a0, -68(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	lw	t2, -200(s0)
	lw	a0, -344(s0)
	addw	a0, t2, a0
	sw	a0, -352(s0)
	ld	t2, -112(s0)
	lw	a0, -352(s0)
	add	a0, t2, a0
	sd	a0, -360(s0)
	ld	t2, -360(s0)
	lw	a0, 0(t2)
	sw	a0, -368(s0)
	lw	a0, -68(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -384(s0)
	lw	a0, -384(s0)
	addiw	a0, a0, 0
	sw	a0, -392(s0)
	lw	t2, -392(s0)
	lw	a0, -304(s0)
	addw	a0, t2, a0
	sw	a0, -400(s0)
	ld	t2, -112(s0)
	lw	a0, -400(s0)
	add	a0, t2, a0
	sd	a0, -408(s0)
	ld	t2, -408(s0)
	lw	a0, 0(t2)
	sw	a0, -416(s0)
	lw	t2, -368(s0)
	lw	a0, -416(s0)
	mulw	a0, t2, a0
	sw	a0, -424(s0)
	lw	t2, -72(s0)
	lw	a0, -424(s0)
	subw	a0, t2, a0
	sw	a0, -432(s0)
	addi	t1, s0, -72
	lw	a0, -432(s0)
	sw	a0, 0(t1)
	lw	a0, -68(s0)
	addiw	a0, a0, 1
	sw	a0, -448(s0)
	addi	t1, s0, -68
	lw	a0, -448(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_4
.L_whe_kernel_ludcmp_5:
	lw	a0, -72(s0)
	lw	a0, -64(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -456(s0)
	lw	a0, -456(s0)
	addiw	a0, a0, 0
	sw	a0, -464(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -480(s0)
	lw	t2, -464(s0)
	lw	a0, -480(s0)
	addw	a0, t2, a0
	sw	a0, -488(s0)
	ld	t2, -112(s0)
	lw	a0, -488(s0)
	add	a0, t2, a0
	sd	a0, -496(s0)
	ld	t2, -496(s0)
	lw	a0, 0(t2)
	sw	a0, -504(s0)
	lw	t2, -72(s0)
	lw	a0, -504(s0)
	divw	a0, t2, a0
	sw	a0, -512(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -528(s0)
	lw	t2, -224(s0)
	lw	a0, -528(s0)
	addw	a0, t2, a0
	sw	a0, -536(s0)
	ld	t2, -112(s0)
	lw	a0, -536(s0)
	add	a0, t2, a0
	sd	a0, -544(s0)
	ld	t1, -544(s0)
	lw	a0, -512(s0)
	sw	a0, 0(t1)
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -560(s0)
	addi	t1, s0, -64
	lw	a0, -560(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_2
.L_whe_kernel_ludcmp_3:
	lw	a0, -60(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_6:
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -576(s0)
	lw	a0, -576(s0)
	addiw	a0, a0, 0
	sw	a0, -584(s0)
	lw	a0, -60(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -608(s0)
	lw	a0, -608(s0)
	addiw	a0, a0, 0
	sw	a0, -616(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -632(s0)
	lw	a0, -632(s0)
	addiw	a0, a0, 0
	sw	a0, -640(s0)
	lw	a0, -64(s0)
	lw	t2, -64(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -656(s0)
	lw	a0, -656(s0)
	beqz	a0, .L_whe_kernel_ludcmp_7
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -672(s0)
	lw	t2, -584(s0)
	lw	a0, -672(s0)
	addw	a0, t2, a0
	sw	a0, -680(s0)
	ld	t2, -112(s0)
	lw	a0, -680(s0)
	add	a0, t2, a0
	sd	a0, -688(s0)
	ld	t2, -688(s0)
	lw	a0, 0(t2)
	sw	a0, -696(s0)
	addi	t1, s0, -72
	lw	a0, -696(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -68
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_8:
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -704(s0)
	lw	a0, -68(s0)
	lw	t2, -68(s0)
	sext.w	t2, t2
	lw	a0, -592(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -720(s0)
	lw	a0, -720(s0)
	beqz	a0, .L_whe_kernel_ludcmp_9
	lw	a0, -72(s0)
	lw	a0, -68(s0)
	slliw	a0, a0, 2
	sw	a0, -744(s0)
	lw	t2, -616(s0)
	lw	a0, -744(s0)
	addw	a0, t2, a0
	sw	a0, -752(s0)
	ld	t2, -112(s0)
	lw	a0, -752(s0)
	add	a0, t2, a0
	sd	a0, -760(s0)
	ld	t2, -760(s0)
	lw	a0, 0(t2)
	sw	a0, -768(s0)
	lw	a0, -68(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -784(s0)
	lw	a0, -784(s0)
	addiw	a0, a0, 0
	sw	a0, -792(s0)
	lw	t2, -792(s0)
	lw	a0, -704(s0)
	addw	a0, t2, a0
	sw	a0, -800(s0)
	ld	t2, -112(s0)
	lw	a0, -800(s0)
	add	a0, t2, a0
	sd	a0, -808(s0)
	ld	t2, -808(s0)
	lw	a0, 0(t2)
	sw	a0, -816(s0)
	lw	t2, -768(s0)
	lw	a0, -816(s0)
	mulw	a0, t2, a0
	sw	a0, -824(s0)
	lw	t2, -72(s0)
	lw	a0, -824(s0)
	subw	a0, t2, a0
	sw	a0, -832(s0)
	addi	t1, s0, -72
	lw	a0, -832(s0)
	sw	a0, 0(t1)
	lw	a0, -68(s0)
	addiw	a0, a0, 1
	sw	a0, -848(s0)
	addi	t1, s0, -68
	lw	a0, -848(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_8
.L_whe_kernel_ludcmp_9:
	lw	a0, -72(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -704(s0)
	lw	t2, -640(s0)
	lw	a0, -704(s0)
	addw	a0, t2, a0
	sw	a0, -856(s0)
	ld	t2, -112(s0)
	lw	a0, -856(s0)
	add	a0, t2, a0
	sd	a0, -864(s0)
	ld	t1, -864(s0)
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -880(s0)
	addi	t1, s0, -64
	lw	a0, -880(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_6
.L_whe_kernel_ludcmp_7:
	lw	a0, -60(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -60
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_0
.L_whe_kernel_ludcmp_1:
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_10:
	li	a0, 0
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	li	a0, 5600
	sw	a0, -104(s0)
	ld	a0, -40(s0)
	sd	a0, -112(s0)
	ld	a0, -32(s0)
	sd	a0, -120(s0)
	ld	a0, -56(s0)
	sd	a0, -128(s0)
	lw	a0, -20(s0)
	lw	a0, -60(s0)
	lw	t2, -60(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_kernel_ludcmp_11
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	sw	a0, -168(s0)
	ld	t2, -112(s0)
	lw	a0, -168(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	ld	t2, -176(s0)
	lw	a0, 0(t2)
	sw	a0, -184(s0)
	addi	t1, s0, -72
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -64
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_12:
	lw	a0, -60(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	addiw	a0, a0, 0
	sw	a0, -208(s0)
	lw	a0, -64(s0)
	lw	t2, -64(s0)
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -224(s0)
	lw	a0, -224(s0)
	beqz	a0, .L_whe_kernel_ludcmp_13
	lw	a0, -72(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -248(s0)
	lw	t2, -208(s0)
	lw	a0, -248(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	ld	t2, -120(s0)
	lw	a0, -256(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	ld	t2, -264(s0)
	lw	a0, 0(t2)
	sw	a0, -272(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	addiw	a0, a0, 0
	sw	a0, -296(s0)
	ld	t2, -128(s0)
	lw	a0, -296(s0)
	add	a0, t2, a0
	sd	a0, -304(s0)
	ld	t2, -304(s0)
	lw	a0, 0(t2)
	sw	a0, -312(s0)
	lw	t2, -272(s0)
	lw	a0, -312(s0)
	mulw	a0, t2, a0
	sw	a0, -320(s0)
	lw	t2, -72(s0)
	lw	a0, -320(s0)
	subw	a0, t2, a0
	sw	a0, -328(s0)
	addi	t1, s0, -72
	lw	a0, -328(s0)
	sw	a0, 0(t1)
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -344(s0)
	addi	t1, s0, -64
	lw	a0, -344(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_12
.L_whe_kernel_ludcmp_13:
	lw	a0, -72(s0)
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	addiw	a0, a0, 0
	sw	a0, -352(s0)
	ld	t2, -128(s0)
	lw	a0, -352(s0)
	add	a0, t2, a0
	sd	a0, -360(s0)
	ld	t1, -360(s0)
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	lw	a0, -60(s0)
	addiw	a0, a0, 1
	sw	a0, -376(s0)
	addi	t1, s0, -60
	lw	a0, -376(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_10
.L_whe_kernel_ludcmp_11:
	li	a0, 1
	sw	a0, -88(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -104(s0)
	addi	t1, s0, -60
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_14:
	li	a0, 0
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	li	a0, 5600
	sw	a0, -104(s0)
	ld	a0, -56(s0)
	sd	a0, -112(s0)
	ld	a0, -32(s0)
	sd	a0, -120(s0)
	ld	a0, -48(s0)
	sd	a0, -128(s0)
	lw	a0, -20(s0)
	lw	a0, -60(s0)
	lw	t2, -60(s0)
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_kernel_ludcmp_15
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	sw	a0, -176(s0)
	ld	t2, -112(s0)
	lw	a0, -176(s0)
	add	a0, t2, a0
	sd	a0, -184(s0)
	ld	t2, -184(s0)
	lw	a0, 0(t2)
	sw	a0, -192(s0)
	addi	t1, s0, -72
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	lw	a0, -60(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -64
	lw	a0, -208(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_16:
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	addiw	a0, a0, 0
	sw	a0, -224(s0)
	lw	a0, -64(s0)
	mv	s1, a0
	sw	s1, -232(s0)
	mv	t2, s1
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_kernel_ludcmp_17
	lw	a0, -72(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -264(s0)
	lw	t2, -224(s0)
	lw	a0, -264(s0)
	addw	a0, t2, a0
	mv	s6, a0
	sw	s6, -272(s0)
	ld	t2, -120(s0)
	mv	a0, s6
	add	a0, t2, a0
	mv	s5, a0
	sd	s5, -280(s0)
	mv	t2, s5
	lw	a0, 0(t2)
	mv	s4, a0
	sw	s4, -288(s0)
	lw	a0, -64(s0)
	mv	s3, a0
	sw	s3, -296(s0)
	mv	a0, s3
	slliw	a0, a0, 2
	mv	s2, a0
	sw	s2, -304(s0)
	mv	a0, s2
	addiw	a0, a0, 0
	sw	a0, -312(s0)
	ld	t2, -128(s0)
	lw	a0, -312(s0)
	add	a0, t2, a0
	sd	a0, -320(s0)
	ld	t2, -320(s0)
	lw	a0, 0(t2)
	sw	a0, -328(s0)
	mv	t2, s4
	lw	a0, -328(s0)
	mulw	a0, t2, a0
	sw	a0, -336(s0)
	lw	t2, -72(s0)
	lw	a0, -336(s0)
	subw	a0, t2, a0
	sw	a0, -344(s0)
	addi	t1, s0, -72
	lw	a0, -344(s0)
	sw	a0, 0(t1)
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -360(s0)
	addi	t1, s0, -64
	lw	a0, -360(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_16
.L_whe_kernel_ludcmp_17:
	lw	a0, -72(s0)
	lw	a0, -60(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -368(s0)
	lw	a0, -368(s0)
	addiw	a0, a0, 0
	sw	a0, -376(s0)
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	sw	a0, -392(s0)
	lw	t2, -376(s0)
	lw	a0, -392(s0)
	addw	a0, t2, a0
	sw	a0, -400(s0)
	ld	t2, -120(s0)
	lw	a0, -400(s0)
	add	a0, t2, a0
	sd	a0, -408(s0)
	ld	t2, -408(s0)
	lw	a0, 0(t2)
	sw	a0, -416(s0)
	lw	t2, -72(s0)
	lw	a0, -416(s0)
	divw	a0, t2, a0
	sw	a0, -424(s0)
	lw	a0, -60(s0)
	slliw	a0, a0, 2
	sw	a0, -440(s0)
	lw	a0, -440(s0)
	addiw	a0, a0, 0
	sw	a0, -448(s0)
	ld	t2, -128(s0)
	lw	a0, -448(s0)
	add	a0, t2, a0
	sd	a0, -456(s0)
	ld	t1, -456(s0)
	lw	a0, -424(s0)
	sw	a0, 0(t1)
	lw	a0, -60(s0)
	addiw	a0, a0, -1
	sw	a0, -472(s0)
	addi	t1, s0, -60
	lw	a0, -472(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_14
.L_whe_kernel_ludcmp_15:
	j	.Lreturn_kernel_ludcmp_0
.Lreturn_kernel_ludcmp_0:
	ld	s6, -928(s0)
	ld	s5, -920(s0)
	ld	s4, -912(s0)
	ld	s3, -904(s0)
	ld	s2, -896(s0)
	ld	s1, -888(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	kernel_ludcmp, .-kernel_ludcmp
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -256
	addi	t0, sp, 256
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -168(s0)
	sd	s2, -176(s0)
	sd	s3, -184(s0)
	sd	s4, -192(s0)
	sd	s5, -200(s0)
	sd	s6, -208(s0)
	sd	s7, -216(s0)
	sd	s8, -224(s0)
	sd	s9, -232(s0)
	sd	s10, -240(s0)
	sd	s11, -248(s0)
	lla	a0, A
	mv	s10, a0
	sd	s10, -24(s0)
	mv	a0, s10
	mv	a0, t4
	call	getarray
	sw	a0, -32(s0)
	lla	a0, b
	sd	a0, -40(s0)
	ld	a0, -40(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -48(s0)
	lla	a0, x
	sd	a0, -56(s0)
	ld	a0, -56(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -64(s0)
	lla	a0, y
	sd	a0, -72(s0)
	ld	a0, -72(s0)
	mv	a0, t4
	call	getarray
	sw	a0, -80(s0)
	li	a0, 68
	mv	s11, a0
	sw	s11, -88(s0)
	addi	sp, sp, -16
	li	a0, 68
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	lla	a0, A
	mv	s9, a0
	sd	s9, -96(s0)
	lla	a0, b
	mv	s8, a0
	sd	s8, -104(s0)
	lla	a0, x
	mv	s7, a0
	sd	s7, -112(s0)
	lla	a0, y
	mv	s6, a0
	sd	s6, -120(s0)
	lla	t0, n
	lw	a0, 0(t0)
	mv	s1, a0
	sw	s1, -128(s0)
	addi	sp, sp, -48
	mv	a0, s1
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s9
	sd	a0, 8(sp)
	mv	a0, s8
	sd	a0, 16(sp)
	mv	a0, s7
	sd	a0, 24(sp)
	mv	a0, s6
	sd	a0, 32(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	ld	a4, 32(sp)
	call	kernel_ludcmp
	addi	sp, sp, 48
	li	a0, 70
	mv	s5, a0
	sw	s5, -136(s0)
	addi	sp, sp, -16
	li	a0, 70
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lla	a0, x
	mv	s3, a0
	sd	s3, -144(s0)
	lla	t0, n
	lw	a0, 0(t0)
	mv	s4, a0
	sw	s4, -152(s0)
	addi	sp, sp, -16
	mv	a0, s4
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s3
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 0
	mv	s2, a0
	sw	s2, -160(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	s11, -248(s0)
	ld	s10, -240(s0)
	ld	s9, -232(s0)
	ld	s8, -224(s0)
	ld	s7, -216(s0)
	ld	s6, -208(s0)
	ld	s5, -200(s0)
	ld	s4, -192(s0)
	ld	s3, -184(s0)
	ld	s2, -176(s0)
	ld	s1, -168(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
