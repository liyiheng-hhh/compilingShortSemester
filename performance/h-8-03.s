	.data
	.align	2
	.globl	n
n:
	.word	50
	.bss
	.align	2
	.globl	seq
seq:
	.zero	5600
	.bss
	.align	2
	.globl	table
table:
	.zero	7840000
	.text
	.text
	.align	1
	.globl	kernel_nussinov
	.type	kernel_nussinov, @function
kernel_nussinov:
	li	t6, -2192
	add	sp, sp, t6
	li	t0, 2192
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -2136(s0)
	sd	s2, -2144(s0)
	sd	s3, -2152(s0)
	sd	s4, -2160(s0)
	sd	s5, -2168(s0)
	sd	s6, -2176(s0)
	sw	a0, -20(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -44
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -48
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -52
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -96(s0)
	lw	a0, -20(s0)
	addiw	a0, a0, -1
	sw	a0, -112(s0)
	addi	t1, s0, -44
	lw	a0, -112(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_0:
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	li	a0, 5600
	sw	a0, -88(s0)
	ld	a0, -40(s0)
	sd	a0, -96(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -44(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -136(s0)
	lw	a0, -136(s0)
	beqz	a0, .L_whe_kernel_nussinov_1
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -152(s0)
	addi	t1, s0, -48
	lw	a0, -152(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_2:
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -168(s0)
	lw	t2, -168(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -176(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -192(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -208(s0)
	lw	a0, -208(s0)
	addiw	a0, a0, 0
	sw	a0, -216(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	addiw	a0, a0, 0
	sw	a0, -240(s0)
	lw	a0, -48(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -256(s0)
	lw	a0, -256(s0)
	beqz	a0, .L_whe_kernel_nussinov_3
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -272(s0)
	lw	t2, -272(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -280(s0)
	lw	a0, -280(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -288(s0)
	lw	a0, -288(s0)
	beqz	a0, .L_ifend_kernel_nussinov_4
	li	a0, 5600
	sw	a0, -296(s0)
	li	a0, 1
	sw	a0, -304(s0)
	ld	a0, -40(s0)
	sd	a0, -312(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -328(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	lw	t2, -328(s0)
	lw	a0, -344(s0)
	addw	a0, t2, a0
	sw	a0, -352(s0)
	ld	t2, -312(s0)
	lw	a0, -352(s0)
	add	a0, t2, a0
	sd	a0, -360(s0)
	ld	t2, -360(s0)
	lw	a0, 0(t2)
	sw	a0, -368(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -384(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -400(s0)
	lw	a0, -400(s0)
	slliw	a0, a0, 2
	sw	a0, -408(s0)
	lw	t2, -384(s0)
	lw	a0, -408(s0)
	addw	a0, t2, a0
	sw	a0, -416(s0)
	ld	t2, -312(s0)
	lw	a0, -416(s0)
	add	a0, t2, a0
	sd	a0, -424(s0)
	ld	t2, -424(s0)
	lw	a0, 0(t2)
	sw	a0, -432(s0)
	lw	t2, -368(s0)
	sext.w	t2, t2
	lw	a0, -432(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -440(s0)
	lw	a0, -440(s0)
	beqz	a0, .L_ifend_kernel_nussinov_5
	li	a0, 5600
	sw	a0, -448(s0)
	li	a0, 1
	sw	a0, -456(s0)
	ld	a0, -40(s0)
	sd	a0, -464(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -480(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -496(s0)
	lw	a0, -496(s0)
	slliw	a0, a0, 2
	sw	a0, -504(s0)
	lw	t2, -480(s0)
	lw	a0, -504(s0)
	addw	a0, t2, a0
	sw	a0, -512(s0)
	ld	t2, -464(s0)
	lw	a0, -512(s0)
	add	a0, t2, a0
	sd	a0, -520(s0)
	ld	t2, -520(s0)
	lw	a0, 0(t2)
	sw	a0, -528(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -544(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -560(s0)
	lw	t2, -544(s0)
	lw	a0, -560(s0)
	addw	a0, t2, a0
	sw	a0, -568(s0)
	ld	t2, -464(s0)
	lw	a0, -568(s0)
	add	a0, t2, a0
	sd	a0, -576(s0)
	ld	t1, -576(s0)
	lw	a0, -528(s0)
	sw	a0, 0(t1)
.L_ifend_kernel_nussinov_5:
.L_ifend_kernel_nussinov_4:
	lw	a0, -176(s0)
	beqz	a0, .L_ifend_kernel_nussinov_6
	li	a0, 5600
	sw	a0, -176(s0)
	li	a0, 1
	sw	a0, -584(s0)
	ld	a0, -40(s0)
	sd	a0, -592(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -608(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -624(s0)
	lw	t2, -608(s0)
	lw	a0, -624(s0)
	addw	a0, t2, a0
	sw	a0, -632(s0)
	ld	t2, -592(s0)
	lw	a0, -632(s0)
	add	a0, t2, a0
	sd	a0, -640(s0)
	ld	t2, -640(s0)
	lw	a0, 0(t2)
	sw	a0, -648(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -664(s0)
	lw	a0, -664(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -672(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -688(s0)
	lw	t2, -672(s0)
	lw	a0, -688(s0)
	addw	a0, t2, a0
	sw	a0, -696(s0)
	ld	t2, -592(s0)
	lw	a0, -696(s0)
	add	a0, t2, a0
	sd	a0, -704(s0)
	ld	t2, -704(s0)
	lw	a0, 0(t2)
	sw	a0, -712(s0)
	lw	t2, -648(s0)
	sext.w	t2, t2
	lw	a0, -712(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -720(s0)
	lw	a0, -720(s0)
	beqz	a0, .L_ifend_kernel_nussinov_7
	li	a0, 1
	sw	a0, -728(s0)
	li	a0, 5600
	sw	a0, -736(s0)
	ld	a0, -40(s0)
	sd	a0, -744(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -760(s0)
	lw	a0, -760(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -768(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -784(s0)
	lw	t2, -768(s0)
	lw	a0, -784(s0)
	addw	a0, t2, a0
	sw	a0, -792(s0)
	ld	t2, -744(s0)
	lw	a0, -792(s0)
	add	a0, t2, a0
	sd	a0, -800(s0)
	ld	t2, -800(s0)
	lw	a0, 0(t2)
	sw	a0, -808(s0)
	lw	a0, -808(s0)
	slliw	a0, a0, 1
	sw	a0, -816(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -832(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -848(s0)
	lw	t2, -832(s0)
	lw	a0, -848(s0)
	addw	a0, t2, a0
	sw	a0, -856(s0)
	ld	t2, -744(s0)
	lw	a0, -856(s0)
	add	a0, t2, a0
	sd	a0, -864(s0)
	ld	t1, -864(s0)
	lw	a0, -816(s0)
	sw	a0, 0(t1)
.L_ifend_kernel_nussinov_7:
.L_ifend_kernel_nussinov_6:
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -880(s0)
	lw	t2, -880(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -888(s0)
	lw	a0, -888(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -896(s0)
	lw	a0, -896(s0)
	beqz	a0, .L_ifend_kernel_nussinov_8
	li	a0, 1
	sw	a0, -904(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -920(s0)
	lw	a0, -20(s0)
	lw	t2, -920(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -936(s0)
	lw	a0, -936(s0)
	beqz	a0, .L_ifend_kernel_nussinov_9
	li	a0, 1
	sw	a0, -944(s0)
	lw	a0, -44(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -968(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -968(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -976(s0)
	lw	a0, -976(s0)
	beqz	a0, .L_ifelse_kernel_nussinov_11
	li	a0, 0
	sw	a0, -984(s0)
	addi	t1, s0, -56
	lw	a0, -984(s0)
	sw	a0, 0(t1)
	li	a0, 3
	sw	a0, -992(s0)
	ld	a0, -32(s0)
	sd	a0, -1000(s0)
	lw	a0, -44(s0)
	slliw	a0, a0, 2
	sw	a0, -1016(s0)
	ld	t2, -1000(s0)
	lw	a0, -1016(s0)
	add	a0, t2, a0
	sd	a0, -1024(s0)
	ld	t2, -1024(s0)
	lw	a0, 0(t2)
	sw	a0, -1032(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1048(s0)
	ld	t2, -1000(s0)
	lw	a0, -1048(s0)
	add	a0, t2, a0
	sd	a0, -1056(s0)
	ld	t2, -1056(s0)
	lw	a0, 0(t2)
	sw	a0, -1064(s0)
	lw	t2, -1032(s0)
	lw	a0, -1064(s0)
	addw	a0, t2, a0
	sw	a0, -1072(s0)
	lw	a0, -1072(s0)
	addiw	a0, a0, -3
	sw	a0, -1080(s0)
	lw	a0, -1080(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -1088(s0)
	lw	a0, -1088(s0)
	beqz	a0, .L_ifend_kernel_nussinov_12
	li	a0, 3
	sw	a0, -1096(s0)
	addi	t1, s0, -56
	lw	a0, -1096(s0)
	sw	a0, 0(t1)
.L_ifend_kernel_nussinov_12:
	li	a0, 5600
	sw	a0, -1104(s0)
	li	a0, 1
	sw	a0, -1112(s0)
	ld	a0, -40(s0)
	sd	a0, -1120(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1136(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1152(s0)
	lw	t2, -1136(s0)
	lw	a0, -1152(s0)
	addw	a0, t2, a0
	sw	a0, -1160(s0)
	ld	t2, -1120(s0)
	lw	a0, -1160(s0)
	add	a0, t2, a0
	sd	a0, -1168(s0)
	ld	t2, -1168(s0)
	lw	a0, 0(t2)
	sw	a0, -1176(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -1192(s0)
	lw	a0, -1192(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1200(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -1216(s0)
	lw	a0, -1216(s0)
	slliw	a0, a0, 2
	sw	a0, -1224(s0)
	lw	t2, -1200(s0)
	lw	a0, -1224(s0)
	addw	a0, t2, a0
	sw	a0, -1232(s0)
	ld	t2, -1120(s0)
	lw	a0, -1232(s0)
	add	a0, t2, a0
	sd	a0, -1240(s0)
	ld	t2, -1240(s0)
	lw	a0, 0(t2)
	sw	a0, -1248(s0)
	lw	a0, -56(s0)
	lw	t2, -1248(s0)
	addw	a0, t2, a0
	sw	a0, -1264(s0)
	lw	t2, -1176(s0)
	sext.w	t2, t2
	lw	a0, -1264(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1272(s0)
	lw	a0, -1272(s0)
	beqz	a0, .L_ifend_kernel_nussinov_13
	li	a0, 1
	sw	a0, -1280(s0)
	li	a0, 5600
	sw	a0, -1288(s0)
	ld	a0, -40(s0)
	sd	a0, -1296(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -1312(s0)
	lw	a0, -1312(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1320(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -1336(s0)
	lw	a0, -1336(s0)
	slliw	a0, a0, 2
	sw	a0, -1344(s0)
	lw	t2, -1320(s0)
	lw	a0, -1344(s0)
	addw	a0, t2, a0
	sw	a0, -1352(s0)
	ld	t2, -1296(s0)
	lw	a0, -1352(s0)
	add	a0, t2, a0
	sd	a0, -1360(s0)
	ld	t2, -1360(s0)
	lw	a0, 0(t2)
	sw	a0, -1368(s0)
	lw	a0, -56(s0)
	lw	t2, -1368(s0)
	addw	a0, t2, a0
	sw	a0, -1384(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1400(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1416(s0)
	lw	t2, -1400(s0)
	lw	a0, -1416(s0)
	addw	a0, t2, a0
	sw	a0, -1424(s0)
	ld	t2, -1296(s0)
	lw	a0, -1424(s0)
	add	a0, t2, a0
	sd	a0, -1432(s0)
	ld	t1, -1432(s0)
	lw	a0, -1384(s0)
	sw	a0, 0(t1)
.L_ifend_kernel_nussinov_13:
	j	.L_ifend_kernel_nussinov_10
.L_ifelse_kernel_nussinov_11:
	li	a0, 5600
	sw	a0, -1440(s0)
	li	a0, 1
	sw	a0, -1448(s0)
	ld	a0, -40(s0)
	sd	a0, -1456(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1472(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1488(s0)
	lw	t2, -1472(s0)
	lw	a0, -1488(s0)
	addw	a0, t2, a0
	sw	a0, -1496(s0)
	ld	t2, -1456(s0)
	lw	a0, -1496(s0)
	add	a0, t2, a0
	sd	a0, -1504(s0)
	ld	t2, -1504(s0)
	lw	a0, 0(t2)
	sw	a0, -1512(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -1528(s0)
	lw	a0, -1528(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1536(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -1552(s0)
	lw	a0, -1552(s0)
	slliw	a0, a0, 2
	sw	a0, -1560(s0)
	lw	t2, -1536(s0)
	lw	a0, -1560(s0)
	addw	a0, t2, a0
	sw	a0, -1568(s0)
	ld	t2, -1456(s0)
	lw	a0, -1568(s0)
	add	a0, t2, a0
	sd	a0, -1576(s0)
	ld	t2, -1576(s0)
	lw	a0, 0(t2)
	sw	a0, -1584(s0)
	lw	t2, -1512(s0)
	sext.w	t2, t2
	lw	a0, -1584(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1592(s0)
	lw	a0, -1592(s0)
	beqz	a0, .L_ifend_kernel_nussinov_14
	li	a0, 1
	sw	a0, -1600(s0)
	li	a0, 5600
	sw	a0, -1608(s0)
	ld	a0, -40(s0)
	sd	a0, -1616(s0)
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -1632(s0)
	lw	a0, -1632(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1640(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, -1
	sw	a0, -1656(s0)
	lw	a0, -1656(s0)
	slliw	a0, a0, 2
	sw	a0, -1664(s0)
	lw	t2, -1640(s0)
	lw	a0, -1664(s0)
	addw	a0, t2, a0
	sw	a0, -1672(s0)
	ld	t2, -1616(s0)
	lw	a0, -1672(s0)
	add	a0, t2, a0
	sd	a0, -1680(s0)
	ld	t2, -1680(s0)
	lw	a0, 0(t2)
	sw	a0, -1688(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1704(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1720(s0)
	lw	t2, -1704(s0)
	lw	a0, -1720(s0)
	addw	a0, t2, a0
	sw	a0, -1728(s0)
	ld	t2, -1616(s0)
	lw	a0, -1728(s0)
	add	a0, t2, a0
	sd	a0, -1736(s0)
	ld	t1, -1736(s0)
	lw	a0, -1688(s0)
	sw	a0, 0(t1)
.L_ifend_kernel_nussinov_14:
.L_ifend_kernel_nussinov_10:
.L_ifend_kernel_nussinov_9:
.L_ifend_kernel_nussinov_8:
	addi	t1, s0, -52
	lw	a0, -192(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_15:
	lw	a0, -48(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1752(s0)
	lw	t2, -216(s0)
	lw	a0, -1752(s0)
	addw	a0, t2, a0
	sw	a0, -1760(s0)
	ld	t2, -96(s0)
	lw	a0, -1760(s0)
	add	a0, t2, a0
	sd	a0, -1768(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -1784(s0)
	lw	a0, -52(s0)
	lw	t2, -52(s0)
	sext.w	t2, t2
	lw	a0, -192(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1800(s0)
	lw	a0, -1800(s0)
	beqz	a0, .L_whe_kernel_nussinov_16
	ld	t2, -1768(s0)
	lw	a0, 0(t2)
	sw	a0, -1808(s0)
	lw	a0, -52(s0)
	slliw	a0, a0, 2
	sw	a0, -1824(s0)
	lw	t2, -240(s0)
	lw	a0, -1824(s0)
	addw	a0, t2, a0
	sw	a0, -1832(s0)
	ld	t2, -96(s0)
	lw	a0, -1832(s0)
	add	a0, t2, a0
	sd	a0, -1840(s0)
	ld	t2, -1840(s0)
	lw	a0, 0(t2)
	sw	a0, -1848(s0)
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -1864(s0)
	lw	a0, -1864(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1872(s0)
	lw	a0, -1872(s0)
	addiw	a0, a0, 0
	sw	a0, -1880(s0)
	lw	t2, -1880(s0)
	lw	a0, -1784(s0)
	addw	a0, t2, a0
	sw	a0, -1888(s0)
	ld	t2, -96(s0)
	lw	a0, -1888(s0)
	add	a0, t2, a0
	sd	a0, -1896(s0)
	ld	t2, -1896(s0)
	lw	a0, 0(t2)
	sw	a0, -1904(s0)
	lw	t2, -1848(s0)
	lw	a0, -1904(s0)
	addw	a0, t2, a0
	sw	a0, -1912(s0)
	lw	t2, -1808(s0)
	sext.w	t2, t2
	lw	a0, -1912(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -1920(s0)
	lw	a0, -1920(s0)
	beqz	a0, .L_ifend_kernel_nussinov_17
	li	a0, 5600
	sw	a0, -1768(s0)
	li	a0, 1
	sw	a0, -1784(s0)
	ld	a0, -40(s0)
	sd	a0, -1928(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -1944(s0)
	lw	a0, -52(s0)
	slliw	a0, a0, 2
	sw	a0, -1960(s0)
	lw	t2, -1944(s0)
	lw	a0, -1960(s0)
	addw	a0, t2, a0
	sw	a0, -1968(s0)
	ld	t2, -1928(s0)
	lw	a0, -1968(s0)
	add	a0, t2, a0
	sd	a0, -1976(s0)
	ld	t2, -1976(s0)
	lw	a0, 0(t2)
	sw	a0, -1984(s0)
	lw	a0, -1984(s0)
	slliw	a0, a0, 1
	sw	a0, -1992(s0)
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -2008(s0)
	lw	a0, -2008(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -2016(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -2032(s0)
	lw	t2, -2016(s0)
	lw	a0, -2032(s0)
	addw	a0, t2, a0
	sw	a0, -2040(s0)
	ld	t2, -1928(s0)
	lw	a0, -2040(s0)
	add	a0, t2, a0
	sd	a0, -2048(s0)
	ld	t2, -2048(s0)
	lw	a0, 0(t2)
	li	t6, -2056
	add	t6, s0, t6
	sw	a0, 0(t6)
	lw	t2, -1992(s0)
	li	t6, -2056
	add	t6, s0, t6
	lw	a0, 0(t6)
	addw	a0, t2, a0
	li	t6, -2064
	add	t6, s0, t6
	sw	a0, 0(t6)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	li	t6, -2080
	add	t6, s0, t6
	sw	a0, 0(t6)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	li	t6, -2096
	add	t6, s0, t6
	sw	a0, 0(t6)
	li	t6, -2080
	add	t6, s0, t6
	lw	t2, 0(t6)
	li	t6, -2096
	add	t6, s0, t6
	lw	a0, 0(t6)
	addw	a0, t2, a0
	li	t6, -2104
	add	t6, s0, t6
	sw	a0, 0(t6)
	ld	t2, -1928(s0)
	li	t6, -2104
	add	t6, s0, t6
	lw	a0, 0(t6)
	add	a0, t2, a0
	li	t6, -2112
	add	t6, s0, t6
	sd	a0, 0(t6)
	li	t6, -2112
	add	t6, s0, t6
	ld	t1, 0(t6)
	li	t6, -2064
	add	t6, s0, t6
	lw	a0, 0(t6)
	sw	a0, 0(t1)
.L_ifend_kernel_nussinov_17:
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	li	t6, -2128
	add	t6, s0, t6
	sw	a0, 0(t6)
	addi	t1, s0, -52
	li	t6, -2128
	add	t6, s0, t6
	lw	a0, 0(t6)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_15
.L_whe_kernel_nussinov_16:
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -240(s0)
	addi	t1, s0, -48
	lw	a0, -240(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_2
.L_whe_kernel_nussinov_3:
	lw	a0, -44(s0)
	addiw	a0, a0, -1
	sw	a0, -96(s0)
	addi	t1, s0, -44
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_0
.L_whe_kernel_nussinov_1:
	li	a0, 0
	sw	a0, -72(s0)
	addi	t1, s0, -60
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -80(s0)
	addi	t1, s0, -64
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -88(s0)
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_18:
	li	a0, 0
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	li	a0, 32
	sw	a0, -88(s0)
	li	a0, 5600
	sw	a0, -96(s0)
	li	a0, 11
	sw	a0, -104(s0)
	ld	a0, -40(s0)
	sd	a0, -112(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -20(s0)
	lw	a0, -60(s0)
	lw	t2, -60(s0)
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_kernel_nussinov_19
	addi	t1, s0, -64
	lw	a0, -72(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_20:
	lw	a0, -60(s0)
	lw	a0, -60(s0)
	mv	s1, a0
	sw	s1, -176(s0)
	mv	a0, s1
	addiw	a0, a0, 32
	sw	a0, -184(s0)
	lw	a0, -64(s0)
	lw	t2, -64(s0)
	sext.w	t2, t2
	lw	a0, -128(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	beqz	a0, .L_whe_kernel_nussinov_21
	addi	t1, s0, -44
	lw	a0, -168(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_22:
	lw	a0, -64(s0)
	lw	a0, -64(s0)
	mv	s5, a0
	sw	s5, -208(s0)
	mv	a0, s5
	addiw	a0, a0, 32
	sw	a0, -216(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_kernel_nussinov_23
	lw	a0, -44(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -136(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -232(s0)
	lw	a0, -232(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_ifend_kernel_nussinov_24
	j	.L_whe_kernel_nussinov_23
.L_ifend_kernel_nussinov_24:
	lw	a0, -44(s0)
	lw	t2, -44(s0)
	sext.w	t2, t2
	lw	a0, -184(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -256(s0)
	lw	a0, -256(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -264(s0)
	lw	a0, -264(s0)
	beqz	a0, .L_ifend_kernel_nussinov_25
	j	.L_whe_kernel_nussinov_23
.L_ifend_kernel_nussinov_25:
	addi	t1, s0, -48
	lw	a0, -168(s0)
	sw	a0, 0(t1)
.L_wh_kernel_nussinov_26:
	lw	a0, -44(s0)
	mv	s2, a0
	sw	s2, -168(s0)
	mv	a0, s2
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -272(s0)
	lw	a0, -272(s0)
	addiw	a0, a0, 0
	sw	a0, -280(s0)
	lw	a0, -44(s0)
	mv	t2, a0
	li	a0, 5600
	mulw	a0, t2, a0
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	addiw	a0, a0, 0
	sw	a0, -304(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_kernel_nussinov_27
	lw	a0, -48(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -320(s0)
	lw	a0, -320(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -328(s0)
	lw	a0, -328(s0)
	beqz	a0, .L_ifend_kernel_nussinov_28
	j	.L_whe_kernel_nussinov_27
.L_ifend_kernel_nussinov_28:
	lw	a0, -48(s0)
	mv	s6, a0
	sw	s6, -336(s0)
	mv	t2, s6
	sext.w	t2, t2
	lw	a0, -216(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s4, a0
	sw	s4, -344(s0)
	mv	a0, s4
	sext.w	a0, a0
	seqz	a0, a0
	mv	s3, a0
	sw	s3, -352(s0)
	mv	a0, s3
	beqz	a0, .L_ifend_kernel_nussinov_29
	j	.L_whe_kernel_nussinov_27
.L_ifend_kernel_nussinov_29:
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -368(s0)
	lw	t2, -280(s0)
	lw	a0, -368(s0)
	addw	a0, t2, a0
	sw	a0, -376(s0)
	ld	t2, -112(s0)
	lw	a0, -376(s0)
	add	a0, t2, a0
	sd	a0, -384(s0)
	ld	t2, -384(s0)
	lw	a0, 0(t2)
	sw	a0, -392(s0)
	lw	a0, -392(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1561806290
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 2
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 11
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -400(s0)
	lw	a0, -48(s0)
	slliw	a0, a0, 2
	sw	a0, -416(s0)
	lw	t2, -304(s0)
	lw	a0, -416(s0)
	addw	a0, t2, a0
	sw	a0, -424(s0)
	ld	t2, -112(s0)
	lw	a0, -424(s0)
	add	a0, t2, a0
	sd	a0, -432(s0)
	ld	t1, -432(s0)
	lw	a0, -400(s0)
	sw	a0, 0(t1)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -448(s0)
	addi	t1, s0, -48
	lw	a0, -448(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_26
.L_whe_kernel_nussinov_27:
	lw	a0, -44(s0)
	addiw	a0, a0, 1
	sw	a0, -280(s0)
	addi	t1, s0, -44
	lw	a0, -280(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_22
.L_whe_kernel_nussinov_23:
	lw	a0, -64(s0)
	addiw	a0, a0, 32
	sw	a0, -304(s0)
	addi	t1, s0, -64
	lw	a0, -304(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_20
.L_whe_kernel_nussinov_21:
	lw	a0, -60(s0)
	addiw	a0, a0, 32
	sw	a0, -80(s0)
	addi	t1, s0, -60
	lw	a0, -80(s0)
	sw	a0, 0(t1)
	j	.L_wh_kernel_nussinov_18
.L_whe_kernel_nussinov_19:
	j	.Lreturn_kernel_nussinov_0
.Lreturn_kernel_nussinov_0:
	ld	s6, -2176(s0)
	ld	s5, -2168(s0)
	ld	s4, -2160(s0)
	ld	s3, -2152(s0)
	ld	s2, -2144(s0)
	ld	s1, -2136(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	kernel_nussinov, .-kernel_nussinov
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -224
	addi	t0, sp, 224
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -128(s0)
	sd	s2, -136(s0)
	sd	s3, -144(s0)
	sd	s4, -152(s0)
	sd	s5, -160(s0)
	sd	s6, -168(s0)
	sd	s7, -176(s0)
	sd	s8, -184(s0)
	sd	s9, -192(s0)
	sd	s10, -200(s0)
	sd	s11, -208(s0)
	lla	a0, seq
	mv	s1, a0
	sd	s1, -24(s0)
	mv	a0, s1
	mv	a0, t4
	call	getarray
	mv	s2, a0
	sw	s2, -32(s0)
	lla	a0, table
	mv	s3, a0
	sd	s3, -40(s0)
	mv	a0, s3
	mv	a0, t4
	call	getarray
	mv	s4, a0
	sw	s4, -48(s0)
	li	a0, 79
	mv	s5, a0
	sw	s5, -56(s0)
	addi	sp, sp, -16
	li	a0, 79
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	lla	a0, seq
	mv	s7, a0
	sd	s7, -64(s0)
	lla	a0, table
	mv	s8, a0
	sd	s8, -72(s0)
	lla	t0, n
	lw	a0, 0(t0)
	mv	s6, a0
	sw	s6, -80(s0)
	addi	sp, sp, -32
	mv	a0, s6
	sext.w	a0, a0
	sd	a0, 0(sp)
	mv	a0, s7
	sd	a0, 8(sp)
	mv	a0, s8
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	kernel_nussinov
	addi	sp, sp, 32
	li	a0, 81
	mv	s9, a0
	sw	s9, -88(s0)
	addi	sp, sp, -16
	li	a0, 81
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lla	a0, table
	sd	a0, -96(s0)
	lla	t0, n
	lw	a0, 0(t0)
	mv	s10, a0
	sw	s10, -104(s0)
	mv	t2, s10
	mv	a0, s10
	mulw	a0, t2, a0
	mv	s11, a0
	sw	s11, -112(s0)
	addi	sp, sp, -16
	mv	a0, s11
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -96(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	sext.w	a0, a0
	j	.Lreturn_main_1
	li	a0, 0
.Lreturn_main_1:
	ld	s11, -208(s0)
	ld	s10, -200(s0)
	ld	s9, -192(s0)
	ld	s8, -184(s0)
	ld	s7, -176(s0)
	ld	s6, -168(s0)
	ld	s5, -160(s0)
	ld	s4, -152(s0)
	ld	s3, -144(s0)
	ld	s2, -136(s0)
	ld	s1, -128(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
