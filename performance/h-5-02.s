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
	addi	sp, sp, -1344
	li	t0, 1344
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	sd	a3, -48(s0)
	sd	a4, -56(s0)
	mv	t4, a0
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -112(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -120(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_0:
	mv	a0, t4
	sw	a0, -88(s0)
	li	a0, 0
	sw	a0, -96(s0)
	ld	a0, -32(s0)
	sd	a0, -104(s0)
	lw	a0, -96(s0)
	sw	a0, -112(s0)
	mv	a0, t4
	sw	a0, -120(s0)
	ld	a0, -104(s0)
	sd	a0, -128(s0)
	lw	a0, -96(s0)
	sw	a0, -136(s0)
	li	a0, 1
	sw	a0, -144(s0)
	li	a0, 5600
	sw	a0, -152(s0)
	lw	a0, -96(s0)
	sw	a0, -160(s0)
	ld	a0, -104(s0)
	sd	a0, -168(s0)
	lw	a0, -96(s0)
	sw	a0, -176(s0)
	lw	a0, -152(s0)
	sw	a0, -184(s0)
	ld	a0, -104(s0)
	sd	a0, -192(s0)
	lw	a0, -96(s0)
	sw	a0, -200(s0)
	lw	a0, -152(s0)
	sw	a0, -208(s0)
	lw	a0, -144(s0)
	sw	a0, -216(s0)
	lw	a0, -144(s0)
	sw	a0, -224(s0)
	ld	a0, -104(s0)
	sd	a0, -232(s0)
	lw	a0, -96(s0)
	sw	a0, -240(s0)
	lw	a0, -152(s0)
	sw	a0, -248(s0)
	ld	a0, -104(s0)
	sd	a0, -256(s0)
	lw	a0, -96(s0)
	sw	a0, -264(s0)
	lw	a0, -152(s0)
	sw	a0, -272(s0)
	lw	a0, -144(s0)
	sw	a0, -280(s0)
	lw	a0, -152(s0)
	sw	a0, -288(s0)
	lw	a0, -96(s0)
	sw	a0, -296(s0)
	ld	a0, -104(s0)
	sd	a0, -304(s0)
	lw	a0, -96(s0)
	sw	a0, -312(s0)
	ld	a0, -104(s0)
	sd	a0, -320(s0)
	lw	a0, -96(s0)
	sw	a0, -328(s0)
	lw	a0, -152(s0)
	sw	a0, -336(s0)
	lw	a0, -144(s0)
	sw	a0, -344(s0)
	lw	a0, -152(s0)
	sw	a0, -352(s0)
	ld	a0, -104(s0)
	sd	a0, -360(s0)
	lw	a0, -96(s0)
	sw	a0, -368(s0)
	lw	a0, -152(s0)
	sw	a0, -376(s0)
	lw	a0, -144(s0)
	sw	a0, -384(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -392(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -400(s0)
	lw	a0, -400(s0)
	beqz	a0, .L_whe_kernel_ludcmp_1
	addi	t1, s0, -64
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_2:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -408(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -416(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -424(s0)
	addiw	a0, a0, 0
	sw	a0, -432(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -440(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -448(s0)
	addiw	a0, a0, 0
	sw	a0, -456(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -464(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -472(s0)
	addiw	a0, a0, 0
	sw	a0, -480(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -488(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -408(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -496(s0)
	lw	a0, -496(s0)
	beqz	a0, .L_whe_kernel_ludcmp_3
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -504(s0)
	slliw	a0, a0, 2
	sw	a0, -512(s0)
	lw	t2, -432(s0)
	addw	a0, t2, a0
	sw	a0, -520(s0)
	ld	t2, -104(s0)
	add	a0, t2, a0
	sd	a0, -528(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -536(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t1, s0, -68
	lw	a0, -160(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_4:
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -432(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -544(s0)
	addiw	a0, a0, -1
	sw	a0, -552(s0)
	slliw	a0, a0, 2
	sw	a0, -560(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -568(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -432(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -576(s0)
	lw	a0, -576(s0)
	beqz	a0, .L_whe_kernel_ludcmp_5
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -584(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -592(s0)
	slliw	a0, a0, 2
	sw	a0, -600(s0)
	lw	t2, -456(s0)
	addw	a0, t2, a0
	sw	a0, -608(s0)
	ld	t2, -168(s0)
	add	a0, t2, a0
	sd	a0, -616(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -624(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -632(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -640(s0)
	addiw	a0, a0, 0
	sw	a0, -648(s0)
	mv	t2, a0
	lw	a0, -560(s0)
	addw	a0, t2, a0
	sw	a0, -656(s0)
	ld	t2, -192(s0)
	add	a0, t2, a0
	sd	a0, -664(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -672(s0)
	lw	t2, -624(s0)
	mulw	a0, t2, a0
	sw	a0, -680(s0)
	lw	t2, -584(s0)
	subw	a0, t2, a0
	sw	a0, -688(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -696(s0)
	addiw	a0, a0, 1
	sw	a0, -704(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_4
.L_whe_kernel_ludcmp_5:
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -456(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -560(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -712(s0)
	addiw	a0, a0, 0
	sw	a0, -720(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -728(s0)
	slliw	a0, a0, 2
	sw	a0, -736(s0)
	lw	t2, -720(s0)
	addw	a0, t2, a0
	sw	a0, -744(s0)
	ld	t2, -232(s0)
	add	a0, t2, a0
	sd	a0, -752(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -760(s0)
	lw	t2, -456(s0)
	divw	a0, t2, a0
	sw	a0, -768(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -776(s0)
	slliw	a0, a0, 2
	sw	a0, -784(s0)
	lw	t2, -480(s0)
	addw	a0, t2, a0
	sw	a0, -792(s0)
	ld	t2, -256(s0)
	add	a0, t2, a0
	sd	a0, -800(s0)
	mv	t1, a0
	lw	a0, -768(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -808(s0)
	addiw	a0, a0, 1
	sw	a0, -816(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_2
.L_whe_kernel_ludcmp_3:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_6:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -160(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -168(s0)
	addiw	a0, a0, 0
	sw	a0, -176(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -208(s0)
	addiw	a0, a0, 0
	sw	a0, -216(s0)
	lw	a0, -192(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -224(s0)
	addiw	a0, a0, 0
	sw	a0, -232(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -248(s0)
	lw	a0, -248(s0)
	beqz	a0, .L_whe_kernel_ludcmp_7
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	slliw	a0, a0, 2
	sw	a0, -264(s0)
	lw	t2, -176(s0)
	addw	a0, t2, a0
	sw	a0, -272(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -280(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -480(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t1, s0, -68
	lw	a0, -296(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_8:
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	slliw	a0, a0, 2
	sw	a0, -824(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -832(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -184(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -840(s0)
	lw	a0, -840(s0)
	beqz	a0, .L_whe_kernel_ludcmp_9
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -848(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -856(s0)
	slliw	a0, a0, 2
	sw	a0, -864(s0)
	lw	t2, -232(s0)
	addw	a0, t2, a0
	sw	a0, -872(s0)
	ld	t2, -304(s0)
	add	a0, t2, a0
	sd	a0, -880(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -888(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -896(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -904(s0)
	addiw	a0, a0, 0
	sw	a0, -912(s0)
	mv	t2, a0
	lw	a0, -824(s0)
	addw	a0, t2, a0
	sw	a0, -920(s0)
	ld	t2, -360(s0)
	add	a0, t2, a0
	sd	a0, -928(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -936(s0)
	lw	t2, -888(s0)
	mulw	a0, t2, a0
	sw	a0, -944(s0)
	lw	t2, -848(s0)
	subw	a0, t2, a0
	sw	a0, -952(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -960(s0)
	addiw	a0, a0, 1
	sw	a0, -968(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_8
.L_whe_kernel_ludcmp_9:
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	a0, a0, 2
	sw	a0, -824(s0)
	lw	t2, -216(s0)
	addw	a0, t2, a0
	sw	a0, -976(s0)
	ld	t2, -320(s0)
	add	a0, t2, a0
	sd	a0, -984(s0)
	mv	t1, a0
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -992(s0)
	addiw	a0, a0, 1
	sw	a0, -1000(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_6
.L_whe_kernel_ludcmp_7:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	addiw	a0, a0, 1
	sw	a0, -128(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_0
.L_whe_kernel_ludcmp_1:
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_10:
	mv	a0, t4
	sw	a0, -88(s0)
	ld	a0, -40(s0)
	sd	a0, -96(s0)
	li	a0, 0
	sw	a0, -104(s0)
	ld	a0, -32(s0)
	sd	a0, -112(s0)
	lw	a0, -104(s0)
	sw	a0, -120(s0)
	ld	a0, -56(s0)
	sd	a0, -128(s0)
	lw	a0, -104(s0)
	sw	a0, -136(s0)
	li	a0, 1
	sw	a0, -144(s0)
	li	a0, 5600
	sw	a0, -152(s0)
	ld	a0, -128(s0)
	sd	a0, -160(s0)
	lw	a0, -104(s0)
	sw	a0, -168(s0)
	lw	a0, -144(s0)
	sw	a0, -176(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	beqz	a0, .L_whe_kernel_ludcmp_11
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	slliw	a0, a0, 2
	sw	a0, -208(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -216(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -224(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t1, s0, -64
	lw	a0, -104(s0)
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_12:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -240(s0)
	addiw	a0, a0, 0
	sw	a0, -248(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -264(s0)
	lw	a0, -264(s0)
	beqz	a0, .L_whe_kernel_ludcmp_13
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	slliw	a0, a0, 2
	sw	a0, -288(s0)
	lw	t2, -248(s0)
	addw	a0, t2, a0
	sw	a0, -296(s0)
	ld	t2, -112(s0)
	add	a0, t2, a0
	sd	a0, -304(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -312(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	slliw	a0, a0, 2
	sw	a0, -328(s0)
	addiw	a0, a0, 0
	sw	a0, -336(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -344(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -352(s0)
	lw	t2, -312(s0)
	mulw	a0, t2, a0
	sw	a0, -360(s0)
	lw	t2, -272(s0)
	subw	a0, t2, a0
	sw	a0, -368(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	addiw	a0, a0, 1
	sw	a0, -384(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_12
.L_whe_kernel_ludcmp_13:
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	slliw	a0, a0, 2
	sw	a0, -152(s0)
	addiw	a0, a0, 0
	sw	a0, -160(s0)
	ld	t2, -128(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	mv	t1, a0
	lw	a0, -112(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	addiw	a0, a0, 1
	sw	a0, -248(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_10
.L_whe_kernel_ludcmp_11:
	mv	a0, t4
	sw	a0, -88(s0)
	li	a0, 1
	sw	a0, -96(s0)
	lw	a0, -88(s0)
	addiw	a0, a0, -1
	sw	a0, -104(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_14:
	li	a0, 0
	sw	a0, -88(s0)
	ld	a0, -56(s0)
	sd	a0, -96(s0)
	li	a0, 1
	sw	a0, -104(s0)
	mv	a0, t4
	sw	a0, -112(s0)
	ld	a0, -32(s0)
	sd	a0, -120(s0)
	lw	a0, -88(s0)
	sw	a0, -128(s0)
	ld	a0, -120(s0)
	sd	a0, -136(s0)
	lw	a0, -88(s0)
	sw	a0, -144(s0)
	li	a0, 5600
	sw	a0, -152(s0)
	ld	a0, -48(s0)
	sd	a0, -160(s0)
	lw	a0, -88(s0)
	sw	a0, -168(s0)
	lw	a0, -104(s0)
	sw	a0, -176(s0)
	lw	a0, -152(s0)
	sw	a0, -184(s0)
	ld	a0, -160(s0)
	sd	a0, -192(s0)
	lw	a0, -88(s0)
	sw	a0, -200(s0)
	lw	a0, -104(s0)
	sw	a0, -208(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -224(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	beqz	a0, .L_whe_kernel_ludcmp_15
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	slliw	a0, a0, 2
	sw	a0, -248(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -256(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -264(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addiw	a0, a0, 1
	sw	a0, -280(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
.L_wh_kernel_ludcmp_16:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -288(s0)
	addiw	a0, a0, 0
	sw	a0, -296(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -112(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -312(s0)
	lw	a0, -312(s0)
	beqz	a0, .L_whe_kernel_ludcmp_17
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -328(s0)
	slliw	a0, a0, 2
	sw	a0, -336(s0)
	lw	t2, -296(s0)
	addw	a0, t2, a0
	sw	a0, -344(s0)
	ld	t2, -120(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -360(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -368(s0)
	slliw	a0, a0, 2
	sw	a0, -376(s0)
	addiw	a0, a0, 0
	sw	a0, -384(s0)
	ld	t2, -192(s0)
	add	a0, t2, a0
	sd	a0, -392(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -400(s0)
	lw	t2, -360(s0)
	mulw	a0, t2, a0
	sw	a0, -408(s0)
	lw	t2, -320(s0)
	subw	a0, t2, a0
	sw	a0, -416(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -424(s0)
	addiw	a0, a0, 1
	sw	a0, -432(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_16
.L_whe_kernel_ludcmp_17:
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	t2, a0, 5
	slliw	t1, a0, 6
	addw	t2, t2, t1
	slliw	t1, a0, 7
	addw	t2, t2, t1
	slliw	t1, a0, 8
	addw	t2, t2, t1
	slliw	t1, a0, 10
	addw	t2, t2, t1
	slliw	t1, a0, 12
	addw	t2, t2, t1
	mv	a0, t2
	sw	a0, -184(s0)
	addiw	a0, a0, 0
	sw	a0, -192(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	slliw	a0, a0, 2
	sw	a0, -208(s0)
	lw	t2, -192(s0)
	addw	a0, t2, a0
	sw	a0, -296(s0)
	ld	t2, -136(s0)
	add	a0, t2, a0
	sd	a0, -440(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -448(s0)
	lw	t2, -112(s0)
	divw	a0, t2, a0
	sw	a0, -456(s0)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -464(s0)
	slliw	a0, a0, 2
	sw	a0, -472(s0)
	addiw	a0, a0, 0
	sw	a0, -480(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -488(s0)
	mv	t1, a0
	lw	a0, -456(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -496(s0)
	addiw	a0, a0, -1
	sw	a0, -504(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
	j	.L_wh_kernel_ludcmp_14
.L_whe_kernel_ludcmp_15:
	j	.Lreturn_kernel_ludcmp_0
.Lreturn_kernel_ludcmp_0:
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
	addi	sp, sp, -176
	li	t0, 176
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	lla	a0, A
	sd	a0, -24(s0)
	ld	a0, -24(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -32(s0)
	lla	a0, b
	sd	a0, -40(s0)
	ld	a0, -40(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -48(s0)
	lla	a0, x
	sd	a0, -56(s0)
	ld	a0, -56(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -64(s0)
	lla	a0, y
	sd	a0, -72(s0)
	ld	a0, -72(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -80(s0)
	li	a0, 68
	sw	a0, -88(s0)
	addi	sp, sp, -16
	li	a0, 68
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	lla	t0, n
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lla	a0, A
	sd	a0, -104(s0)
	lla	a0, b
	sd	a0, -112(s0)
	lla	a0, x
	sd	a0, -120(s0)
	lla	a0, y
	sd	a0, -128(s0)
	addi	sp, sp, -48
	lw	a0, -96(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -104(s0)
	sd	a0, 8(sp)
	ld	a0, -112(s0)
	sd	a0, 16(sp)
	ld	a0, -120(s0)
	sd	a0, 24(sp)
	ld	a0, -128(s0)
	sd	a0, 32(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	ld	a3, 24(sp)
	ld	a4, 32(sp)
	call	kernel_ludcmp
	addi	sp, sp, 48
	li	a0, 70
	sw	a0, -136(s0)
	addi	sp, sp, -16
	li	a0, 70
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lla	t0, n
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lla	a0, x
	sd	a0, -152(s0)
	addi	sp, sp, -16
	lw	a0, -144(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -152(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
