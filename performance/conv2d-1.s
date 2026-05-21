	.bss
	.align	2
	.globl	state
state:
	.zero	4
	.bss
	.align	2
	.globl	repeat_factor
repeat_factor:
	.zero	4
	.bss
	.align	2
	.globl	N_eff
N_eff:
	.zero	4
	.bss
	.align	2
	.globl	In
In:
	.zero	16777216
	.bss
	.align	2
	.globl	Out
Out:
	.zero	16777216
	.bss
	.align	2
	.globl	K
K:
	.zero	100
	.text
	.text
	.align	1
	.globl	get_random
	.type	get_random, @function
get_random:
	addi	sp, sp, -176
	addi	t0, sp, 176
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -128(s0)
	sd	s2, -136(s0)
	sd	s3, -144(s0)
	sd	s4, -152(s0)
	sd	s5, -160(s0)
	sd	s6, -168(s0)
	li	a0, 2048
	sw	a0, -40(s0)
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	lw	a0, -48(s0)
	mv	t6, a0
	sext.w	a0, t6
	sraiw	t1, a0, 31
	andi	t1, t1, 2047
	addw	a0, a0, t1
	sraiw	a0, a0, 11
	li	t1, 2048
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -56(s0)
	addi	t1, s0, -20
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -64(s0)
	addi	t1, s0, -24
	lw	a0, -64(s0)
	sw	a0, 0(t1)
.L_wh_get_random_0:
	li	a0, 128
	sw	a0, -40(s0)
	li	a0, 65535
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -20(s0)
	lw	a0, -24(s0)
	lw	t2, -24(s0)
	sext.w	t2, t2
	lw	a0, -20(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_get_random_1
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	a0, -88(s0)
	addiw	a0, a0, 128
	mv	s1, a0
	sw	s1, -96(s0)
	lla	t1, state
	mv	a0, s1
	sw	a0, 0(t1)
	mv	a0, s1
	mv	t6, a0
	sext.w	a0, t6
	li	t1, -2147450879
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	addw	t2, t2, a0
	sraiw	t2, t2, 15
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 65535
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -104(s0)
	lla	t1, state
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	lw	a0, -24(s0)
	mv	s6, a0
	sw	s6, -112(s0)
	mv	a0, s6
	addiw	a0, a0, 1
	mv	s5, a0
	sw	s5, -120(s0)
	addi	t1, s0, -24
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	j	.L_wh_get_random_0
.L_whe_get_random_1:
	li	a0, 65535
	mv	s3, a0
	sw	s3, -40(s0)
	lla	t0, state
	lw	a0, 0(t0)
	mv	s4, a0
	sw	s4, -48(s0)
	mv	a0, s4
	mv	t6, a0
	sext.w	a0, t6
	li	t1, -2147450879
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	addw	t2, t2, a0
	sraiw	t2, t2, 15
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 65535
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s2, a0
	sw	s2, -56(s0)
	lla	t1, state
	mv	a0, s2
	sw	a0, 0(t1)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_get_random_0
	li	a0, 0
.Lreturn_get_random_0:
	ld	s6, -168(s0)
	ld	s5, -160(s0)
	ld	s4, -152(s0)
	ld	s3, -144(s0)
	ld	s2, -136(s0)
	ld	s1, -128(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	get_random, .-get_random
	.text
	.align	1
	.globl	idx
	.type	idx, @function
idx:
	addi	sp, sp, -128
	addi	t0, sp, 128
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -80(s0)
	sd	s2, -88(s0)
	sd	s3, -96(s0)
	sd	s4, -104(s0)
	sd	s5, -112(s0)
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	sw	a2, -28(s0)
	lw	a0, -20(s0)
	mv	s1, a0
	sw	s1, -40(s0)
	lw	a0, -28(s0)
	mv	s2, a0
	sw	s2, -48(s0)
	mv	t2, s1
	mv	a0, s2
	mulw	a0, t2, a0
	mv	s3, a0
	sw	s3, -56(s0)
	lw	a0, -24(s0)
	mv	s4, a0
	sw	s4, -64(s0)
	mv	t2, s3
	mv	a0, s4
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -72(s0)
	mv	a0, s5
	sext.w	a0, a0
	j	.Lreturn_idx_1
	li	a0, 0
.Lreturn_idx_1:
	ld	s5, -112(s0)
	ld	s4, -104(s0)
	ld	s3, -96(s0)
	ld	s2, -88(s0)
	ld	s1, -80(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	idx, .-idx
	.text
	.align	1
	.globl	init_matrix
	.type	init_matrix, @function
init_matrix:
	addi	sp, sp, -384
	addi	t0, sp, 384
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -296(s0)
	sd	s2, -304(s0)
	sd	s3, -312(s0)
	sd	s4, -320(s0)
	sd	s5, -328(s0)
	sd	s6, -336(s0)
	sd	s7, -344(s0)
	sd	s8, -352(s0)
	sd	s9, -360(s0)
	sd	s10, -368(s0)
	sd	s11, -376(s0)
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_init_matrix_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -28(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -72(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_init_matrix_1
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	lw	a0, -72(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -88(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_ifelse_init_matrix_3
.L_wh_init_matrix_4:
	li	a0, 65535
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	ld	a0, -24(s0)
	sd	a0, -112(s0)
	lw	a0, -28(s0)
	lw	a0, -32(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	lw	t2, -32(s0)
	sext.w	t2, t2
	lw	a0, -136(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_whe_init_matrix_5
	call	get_random
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, -2147450879
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	addw	t2, t2, a0
	sraiw	t2, t2, 15
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 65535
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -160(s0)
	lw	a0, -32(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	mv	s2, a0
	sw	s2, -176(s0)
	lw	t2, -120(s0)
	mv	t1, s2
	mulw	t2, t2, t1
	lw	a0, -168(s0)
	addw	a0, t2, a0
	mv	s3, a0
	sw	s3, -184(s0)
	mv	a0, s3
	slliw	a0, a0, 2
	mv	s10, a0
	sw	s10, -192(s0)
	ld	t2, -112(s0)
	mv	a0, s10
	add	a0, t2, a0
	mv	s4, a0
	sd	s4, -200(s0)
	mv	t1, s4
	lw	a0, -160(s0)
	sw	a0, 0(t1)
	lw	a0, -32(s0)
	mv	s5, a0
	sw	s5, -208(s0)
	mv	a0, s5
	addiw	a0, a0, 1
	mv	s6, a0
	sw	s6, -216(s0)
	addi	t1, s0, -32
	mv	a0, s6
	sw	a0, 0(t1)
	mv	s6, a0
	j	.L_wh_init_matrix_4
.L_whe_init_matrix_5:
	j	.L_ifend_init_matrix_2
.L_ifelse_init_matrix_3:
.L_wh_init_matrix_6:
	li	a0, -1
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	ld	a0, -24(s0)
	sd	a0, -112(s0)
	lw	a0, -28(s0)
	lw	a0, -32(s0)
	mv	s7, a0
	sw	s7, -224(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	mv	s8, a0
	sw	s8, -232(s0)
	mv	t2, s7
	sext.w	t2, t2
	mv	a0, s8
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s9, a0
	sw	s9, -240(s0)
	mv	a0, s9
	beqz	a0, .L_whe_init_matrix_7
	lw	a0, -32(s0)
	mv	s1, a0
	sw	s1, -248(s0)
	lw	t2, -120(s0)
	mv	t1, s8
	mulw	t2, t2, t1
	mv	a0, s1
	addw	a0, t2, a0
	mv	s11, a0
	sw	s11, -256(s0)
	mv	a0, s11
	slliw	a0, a0, 2
	sw	a0, -264(s0)
	ld	t2, -112(s0)
	lw	a0, -264(s0)
	add	a0, t2, a0
	sd	a0, -272(s0)
	ld	t1, -272(s0)
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	lw	a0, -32(s0)
	addiw	a0, a0, 1
	sw	a0, -288(s0)
	addi	t1, s0, -32
	lw	a0, -288(s0)
	sw	a0, 0(t1)
	j	.L_wh_init_matrix_6
.L_whe_init_matrix_7:
.L_ifend_init_matrix_2:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -48(s0)
	addi	t1, s0, -28
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	j	.L_wh_init_matrix_0
.L_whe_init_matrix_1:
	j	.Lreturn_init_matrix_2
.Lreturn_init_matrix_2:
	ld	s11, -376(s0)
	ld	s10, -368(s0)
	ld	s9, -360(s0)
	ld	s8, -352(s0)
	ld	s7, -344(s0)
	ld	s6, -336(s0)
	ld	s5, -328(s0)
	ld	s4, -320(s0)
	ld	s3, -312(s0)
	ld	s2, -304(s0)
	ld	s1, -296(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	init_matrix, .-init_matrix
	.text
	.align	1
	.globl	init_kernel
	.type	init_kernel, @function
init_kernel:
	addi	sp, sp, -208
	addi	t0, sp, 208
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -152(s0)
	sd	s2, -160(s0)
	sd	s3, -168(s0)
	sd	s4, -176(s0)
	sd	s5, -184(s0)
	sd	s6, -192(s0)
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_init_kernel_0:
	li	a0, 25
	mv	s2, a0
	sw	s2, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	ld	a0, -24(s0)
	sd	a0, -64(s0)
	lw	a0, -28(s0)
	mv	s1, a0
	sw	s1, -72(s0)
	mv	t2, s1
	sext.w	t2, t2
	mv	a0, s2
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s3, a0
	sw	s3, -80(s0)
	mv	a0, s3
	beqz	a0, .L_whe_init_kernel_1
	lw	a0, -28(s0)
	mv	s4, a0
	sw	s4, -88(s0)
	mv	a0, s4
	mv	t6, a0
	sext.w	a0, t6
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
	li	t1, 3
	mulw	t1, a0, t1
	subw	a0, t6, t1
	mv	s5, a0
	sw	s5, -96(s0)
	mv	a0, s5
	addiw	a0, a0, -1
	mv	s6, a0
	sw	s6, -104(s0)
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -64(s0)
	lw	a0, -120(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	ld	t1, -128(s0)
	mv	a0, s6
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -28
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	j	.L_wh_init_kernel_0
.L_whe_init_kernel_1:
	j	.Lreturn_init_kernel_3
.Lreturn_init_kernel_3:
	ld	s6, -192(s0)
	ld	s5, -184(s0)
	ld	s4, -176(s0)
	ld	s3, -168(s0)
	ld	s2, -160(s0)
	ld	s1, -152(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	init_kernel, .-init_kernel
	.text
	.align	1
	.globl	conv2d
	.type	conv2d, @function
conv2d:
	addi	sp, sp, -704
	addi	t0, sp, 704
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -616(s0)
	sd	s2, -624(s0)
	sd	s3, -632(s0)
	sd	s4, -640(s0)
	sd	s5, -648(s0)
	sd	s6, -656(s0)
	sd	s7, -664(s0)
	sd	s8, -672(s0)
	sd	s9, -680(s0)
	sd	s10, -688(s0)
	sd	s11, -696(s0)
	sd	a0, -24(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	li	a0, 2
	sw	a0, -88(s0)
	addi	t1, s0, -44
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -96(s0)
	addi	t1, s0, -48
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_0:
	li	a0, 0
	sw	a0, -88(s0)
	li	a0, 5
	sw	a0, -96(s0)
	li	a0, 1
	sw	a0, -104(s0)
	ld	a0, -32(s0)
	sd	a0, -112(s0)
	lw	a0, -44(s0)
	lw	a0, -44(s0)
	lw	a0, -48(s0)
	lla	t0, repeat_factor
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lw	t2, -48(s0)
	sext.w	t2, t2
	lw	a0, -144(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_conv2d_1
	addi	t1, s0, -52
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_2:
	lw	a0, -52(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	lw	t2, -52(s0)
	sext.w	t2, t2
	lw	a0, -168(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_conv2d_3
	addi	t1, s0, -56
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_4:
	lw	a0, -52(s0)
	lw	a0, -52(s0)
	lw	a0, -56(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	lw	t2, -56(s0)
	sext.w	t2, t2
	lw	a0, -208(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	beqz	a0, .L_whe_conv2d_5
	addi	t1, s0, -60
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -64
	lw	a0, -60(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_6:
	lw	a0, -56(s0)
	lw	a0, -64(s0)
	lw	t2, -64(s0)
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_conv2d_7
	addi	t1, s0, -68
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	lw	a0, -64(s0)
	lw	t2, -184(s0)
	addw	a0, t2, a0
	sw	a0, -256(s0)
	lw	t2, -256(s0)
	lw	a0, -120(s0)
	subw	a0, t2, a0
	sw	a0, -264(s0)
	addi	t1, s0, -72
	lw	a0, -264(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_8:
	lw	a0, -72(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	lw	a0, -68(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -280(s0)
	lw	a0, -280(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -288(s0)
	lw	a0, -68(s0)
	lw	t2, -68(s0)
	sext.w	t2, t2
	lw	a0, -96(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -304(s0)
	lw	a0, -304(s0)
	beqz	a0, .L_whe_conv2d_9
	lw	a0, -68(s0)
	lw	t2, -56(s0)
	addw	a0, t2, a0
	sw	a0, -320(s0)
	lw	t2, -320(s0)
	lw	a0, -44(s0)
	subw	a0, t2, a0
	sw	a0, -328(s0)
	addi	t1, s0, -76
	lw	a0, -328(s0)
	sw	a0, 0(t1)
	lw	a0, -288(s0)
	beqz	a0, .L_ifend_conv2d_10
	lw	a0, -72(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -336(s0)
	lw	t2, -72(s0)
	sext.w	t2, t2
	lw	a0, -336(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -344(s0)
	lw	a0, -344(s0)
	beqz	a0, .L_ifend_conv2d_11
	li	a0, 0
	sw	a0, -352(s0)
	lw	t2, -76(s0)
	sext.w	t2, t2
	lw	a0, -352(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -360(s0)
	lw	a0, -360(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -368(s0)
	lw	a0, -368(s0)
	beqz	a0, .L_ifend_conv2d_12
	lw	t2, -76(s0)
	sext.w	t2, t2
	lw	a0, -336(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -376(s0)
	lw	a0, -376(s0)
	beqz	a0, .L_ifend_conv2d_13
	ld	a0, -24(s0)
	sd	a0, -384(s0)
	lw	a0, -60(s0)
	lw	a0, -72(s0)
	lw	t2, -400(s0)
	lw	t1, -336(s0)
	mulw	t2, t2, t1
	lw	a0, -328(s0)
	addw	a0, t2, a0
	sw	a0, -408(s0)
	lw	a0, -408(s0)
	slliw	a0, a0, 2
	sw	a0, -416(s0)
	ld	t2, -384(s0)
	lw	a0, -416(s0)
	add	a0, t2, a0
	sd	a0, -424(s0)
	li	a0, 5
	mv	s8, a0
	sw	s8, -432(s0)
	ld	a0, -40(s0)
	mv	s11, a0
	sd	s11, -440(s0)
	ld	t2, -424(s0)
	lw	a0, 0(t2)
	sw	a0, -448(s0)
	lw	a0, -64(s0)
	mv	s1, a0
	sw	s1, -456(s0)
	lw	a0, -68(s0)
	mv	s9, a0
	sw	s9, -464(s0)
	mv	t2, s1
	mv	t1, s8
	mulw	t2, t2, t1
	mv	a0, s9
	addw	a0, t2, a0
	mv	s7, a0
	sw	s7, -472(s0)
	mv	a0, s7
	slliw	a0, a0, 2
	mv	s6, a0
	sw	s6, -480(s0)
	mv	t2, s11
	mv	a0, s6
	add	a0, t2, a0
	mv	s5, a0
	sd	s5, -488(s0)
	mv	t2, s5
	lw	a0, 0(t2)
	mv	s4, a0
	sw	s4, -496(s0)
	lw	t2, -448(s0)
	mv	a0, s4
	mulw	a0, t2, a0
	mv	s3, a0
	sw	s3, -504(s0)
	lw	t2, -392(s0)
	mv	a0, s3
	addw	a0, t2, a0
	mv	s2, a0
	sw	s2, -512(s0)
	addi	t1, s0, -60
	mv	a0, s2
	sw	a0, 0(t1)
	mv	s2, a0
.L_ifend_conv2d_13:
.L_ifend_conv2d_12:
.L_ifend_conv2d_11:
.L_ifend_conv2d_10:
	lw	a0, -68(s0)
	mv	s10, a0
	sw	s10, -520(s0)
	mv	a0, s10
	addiw	a0, a0, 1
	sw	a0, -528(s0)
	addi	t1, s0, -68
	lw	a0, -528(s0)
	sw	a0, 0(t1)
	j	.L_wh_conv2d_8
.L_whe_conv2d_9:
	lw	a0, -64(s0)
	addiw	a0, a0, 1
	sw	a0, -536(s0)
	addi	t1, s0, -64
	lw	a0, -536(s0)
	sw	a0, 0(t1)
	j	.L_wh_conv2d_6
.L_whe_conv2d_7:
	lw	a0, -60(s0)
	lw	a0, -56(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -552(s0)
	lw	t2, -192(s0)
	lw	t1, -552(s0)
	mulw	t2, t2, t1
	lw	a0, -544(s0)
	addw	a0, t2, a0
	sw	a0, -560(s0)
	lw	a0, -560(s0)
	slliw	a0, a0, 2
	sw	a0, -568(s0)
	lw	a0, -568(s0)
	addiw	a0, a0, 0
	sw	a0, -576(s0)
	ld	t2, -112(s0)
	lw	a0, -576(s0)
	add	a0, t2, a0
	sd	a0, -584(s0)
	ld	t1, -584(s0)
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	lw	a0, -56(s0)
	addiw	a0, a0, 1
	sw	a0, -600(s0)
	addi	t1, s0, -56
	lw	a0, -600(s0)
	sw	a0, 0(t1)
	j	.L_wh_conv2d_4
.L_whe_conv2d_5:
	lw	a0, -52(s0)
	addiw	a0, a0, 1
	sw	a0, -608(s0)
	addi	t1, s0, -52
	lw	a0, -608(s0)
	sw	a0, 0(t1)
	j	.L_wh_conv2d_2
.L_whe_conv2d_3:
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -96(s0)
	addi	t1, s0, -48
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	j	.L_wh_conv2d_0
.L_whe_conv2d_1:
	j	.Lreturn_conv2d_4
.Lreturn_conv2d_4:
	ld	s11, -696(s0)
	ld	s10, -688(s0)
	ld	s9, -680(s0)
	ld	s8, -672(s0)
	ld	s7, -664(s0)
	ld	s6, -656(s0)
	ld	s5, -648(s0)
	ld	s4, -640(s0)
	ld	s3, -632(s0)
	ld	s2, -624(s0)
	ld	s1, -616(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	conv2d, .-conv2d
	.text
	.align	1
	.globl	nonlinear
	.type	nonlinear, @function
nonlinear:
	addi	sp, sp, -288
	addi	t0, sp, 288
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -240(s0)
	sd	s2, -248(s0)
	sd	s3, -256(s0)
	sd	s4, -264(s0)
	sd	s5, -272(s0)
	sd	s6, -280(s0)
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_nonlinear_0:
	li	a0, 3
	sw	a0, -56(s0)
	li	a0, 7
	sw	a0, -64(s0)
	li	a0, 97
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	ld	a0, -24(s0)
	sd	a0, -88(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	lw	t2, -96(s0)
	lw	a0, -96(s0)
	mulw	a0, t2, a0
	sw	a0, -104(s0)
	lw	a0, -28(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -104(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_nonlinear_1
	lw	a0, -28(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	ld	t2, -88(s0)
	lw	a0, -136(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	ld	t2, -144(s0)
	lw	a0, 0(t2)
	sw	a0, -152(s0)
	addi	t1, s0, -32
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	lw	t2, -32(s0)
	lw	a0, -32(s0)
	mulw	a0, t2, a0
	sw	a0, -160(s0)
	addi	t1, s0, -36
	lw	a0, -160(s0)
	sw	a0, 0(t1)
	lw	a0, -32(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	mv	s1, a0
	sw	s1, -168(s0)
	lw	t2, -36(s0)
	mv	a0, s1
	addw	a0, t2, a0
	sw	a0, -176(s0)
	addi	t1, s0, -40
	lw	a0, -176(s0)
	sw	a0, 0(t1)
	lw	a0, -40(s0)
	addiw	a0, a0, -7
	sw	a0, -184(s0)
	addi	t1, s0, -44
	lw	a0, -184(s0)
	sw	a0, 0(t1)
	lw	a0, -44(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 1416896428
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 5
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 97
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -192(s0)
	addi	t1, s0, -48
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	mv	s6, a0
	sw	s6, -200(s0)
	mv	a0, s6
	slliw	a0, a0, 2
	mv	s5, a0
	sw	s5, -208(s0)
	ld	t2, -88(s0)
	mv	a0, s5
	add	a0, t2, a0
	mv	s4, a0
	sd	s4, -216(s0)
	mv	t1, s4
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	lw	a0, -28(s0)
	mv	s3, a0
	sw	s3, -224(s0)
	mv	a0, s3
	addiw	a0, a0, 1
	mv	s2, a0
	sw	s2, -232(s0)
	addi	t1, s0, -28
	mv	a0, s2
	sw	a0, 0(t1)
	mv	s2, a0
	j	.L_wh_nonlinear_0
.L_whe_nonlinear_1:
	j	.Lreturn_nonlinear_5
.Lreturn_nonlinear_5:
	ld	s6, -280(s0)
	ld	s5, -272(s0)
	ld	s4, -264(s0)
	ld	s3, -256(s0)
	ld	s2, -248(s0)
	ld	s1, -240(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	nonlinear, .-nonlinear
	.text
	.align	1
	.globl	row_reduce
	.type	row_reduce, @function
row_reduce:
	addi	sp, sp, -464
	addi	t0, sp, 464
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -376(s0)
	sd	s2, -384(s0)
	sd	s3, -392(s0)
	sd	s4, -400(s0)
	sd	s5, -408(s0)
	sd	s6, -416(s0)
	sd	s7, -424(s0)
	sd	s8, -432(s0)
	sd	s9, -440(s0)
	sd	s10, -448(s0)
	sd	s11, -456(s0)
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_row_reduce_0:
	li	a0, 0
	sw	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	ld	a0, -24(s0)
	sd	a0, -72(s0)
	lw	a0, -28(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lw	t2, -28(s0)
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_row_reduce_1
	addi	t1, s0, -32
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	lw	a0, -32(s0)
	sw	a0, 0(t1)
.L_wh_row_reduce_2:
	lw	a0, -28(s0)
	lw	a0, -36(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	lw	t2, -36(s0)
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_whe_row_reduce_3
	lw	a0, -32(s0)
	lw	a0, -36(s0)
	lw	t2, -104(s0)
	lw	t1, -120(s0)
	mulw	t2, t2, t1
	lw	a0, -144(s0)
	addw	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	addiw	a0, a0, 0
	sw	a0, -168(s0)
	ld	t2, -72(s0)
	lw	a0, -168(s0)
	add	a0, t2, a0
	sd	a0, -176(s0)
	ld	t2, -176(s0)
	lw	a0, 0(t2)
	sw	a0, -184(s0)
	lw	t2, -136(s0)
	lw	a0, -184(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	addi	t1, s0, -32
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	lw	a0, -36(s0)
	addiw	a0, a0, 1
	sw	a0, -208(s0)
	addi	t1, s0, -36
	lw	a0, -208(s0)
	sw	a0, 0(t1)
	j	.L_wh_row_reduce_2
.L_whe_row_reduce_3:
	addi	t1, s0, -36
	lw	a0, -56(s0)
	sw	a0, 0(t1)
.L_wh_row_reduce_4:
	lw	a0, -28(s0)
	lw	a0, -32(s0)
	lw	a0, -28(s0)
	lw	a0, -36(s0)
	mv	s2, a0
	sw	s2, -232(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	mv	s11, a0
	sw	s11, -240(s0)
	mv	t2, s2
	sext.w	t2, t2
	mv	a0, s11
	sext.w	a0, a0
	slt	a0, t2, a0
	mv	s3, a0
	sw	s3, -248(s0)
	mv	a0, s3
	beqz	a0, .L_whe_row_reduce_5
	lw	a0, -36(s0)
	mv	s4, a0
	sw	s4, -256(s0)
	lw	t2, -104(s0)
	mv	t1, s11
	mulw	t2, t2, t1
	mv	a0, s4
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -264(s0)
	mv	a0, s5
	slliw	a0, a0, 2
	mv	s6, a0
	sw	s6, -272(s0)
	mv	a0, s6
	mv	s7, a0
	mv	s7, a0
	sw	s7, -280(s0)
	ld	t2, -72(s0)
	mv	a0, s7
	add	a0, t2, a0
	mv	s8, a0
	sd	s8, -288(s0)
	mv	t2, s8
	lw	a0, 0(t2)
	mv	s9, a0
	sw	s9, -296(s0)
	mv	t2, s9
	lw	a0, -216(s0)
	subw	a0, t2, a0
	mv	s10, a0
	sw	s10, -304(s0)
	lw	a0, -36(s0)
	mv	s1, a0
	sw	s1, -312(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	lw	t2, -224(s0)
	lw	t1, -320(s0)
	mulw	t2, t2, t1
	mv	a0, s1
	addw	a0, t2, a0
	sw	a0, -328(s0)
	lw	a0, -328(s0)
	slliw	a0, a0, 2
	sw	a0, -336(s0)
	lw	a0, -336(s0)
	addiw	a0, a0, 0
	sw	a0, -344(s0)
	ld	t2, -72(s0)
	lw	a0, -344(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	ld	t1, -352(s0)
	mv	a0, s10
	sw	a0, 0(t1)
	lw	a0, -36(s0)
	addiw	a0, a0, 1
	sw	a0, -368(s0)
	addi	t1, s0, -36
	lw	a0, -368(s0)
	sw	a0, 0(t1)
	j	.L_wh_row_reduce_4
.L_whe_row_reduce_5:
	lw	a0, -28(s0)
	addiw	a0, a0, 1
	sw	a0, -72(s0)
	addi	t1, s0, -28
	lw	a0, -72(s0)
	sw	a0, 0(t1)
	j	.L_wh_row_reduce_0
.L_whe_row_reduce_1:
	j	.Lreturn_row_reduce_6
.Lreturn_row_reduce_6:
	ld	s11, -456(s0)
	ld	s10, -448(s0)
	ld	s9, -440(s0)
	ld	s8, -432(s0)
	ld	s7, -424(s0)
	ld	s6, -416(s0)
	ld	s5, -408(s0)
	ld	s4, -400(s0)
	ld	s3, -392(s0)
	ld	s2, -384(s0)
	ld	s1, -376(s0)
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	row_reduce, .-row_reduce
	.text
	.align	1
	.globl	checksum
	.type	checksum, @function
checksum:
	addi	sp, sp, -208
	addi	t0, sp, 208
	sd	s0, -16(t0)
	mv	s0, t0
	sd	s1, -152(s0)
	sd	s2, -160(s0)
	sd	s3, -168(s0)
	sd	s4, -176(s0)
	sd	s5, -184(s0)
	sd	s6, -192(s0)
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -32
	lw	a0, -48(s0)
	sw	a0, 0(t1)
.L_wh_checksum_0:
	li	a0, 1
	sw	a0, -40(s0)
	ld	a0, -24(s0)
	sd	a0, -48(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	t2, -56(s0)
	lw	a0, -56(s0)
	mulw	a0, t2, a0
	sw	a0, -64(s0)
	lw	a0, -32(s0)
	lw	t2, -32(s0)
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_checksum_1
	lw	a0, -28(s0)
	lw	a0, -32(s0)
	mv	s1, a0
	sw	s1, -96(s0)
	mv	a0, s1
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -48(s0)
	lw	a0, -104(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	ld	t2, -112(s0)
	lw	a0, 0(t2)
	mv	s6, a0
	sw	s6, -120(s0)
	lw	t2, -28(s0)
	mv	a0, s6
	addw	a0, t2, a0
	mv	s5, a0
	sw	s5, -128(s0)
	addi	t1, s0, -28
	mv	a0, s5
	sw	a0, 0(t1)
	mv	s5, a0
	lw	a0, -32(s0)
	mv	s4, a0
	sw	s4, -136(s0)
	mv	a0, s4
	addiw	a0, a0, 1
	mv	s3, a0
	sw	s3, -144(s0)
	addi	t1, s0, -32
	mv	a0, s3
	sw	a0, 0(t1)
	mv	s3, a0
	j	.L_wh_checksum_0
.L_whe_checksum_1:
	lw	a0, -28(s0)
	mv	s2, a0
	sw	s2, -40(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_checksum_7
	li	a0, 0
.Lreturn_checksum_7:
	ld	s6, -192(s0)
	ld	s5, -184(s0)
	ld	s4, -176(s0)
	ld	s3, -168(s0)
	ld	s2, -160(s0)
	ld	s1, -152(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	checksum, .-checksum
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
	lla	t1, state
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	call	getint
	sw	a0, -48(s0)
	lla	t1, repeat_factor
	lw	a0, -48(s0)
	sw	a0, 0(t1)
	li	a0, 513
	sw	a0, -56(s0)
	li	a0, 64
	sw	a0, -64(s0)
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	mv	t6, a0
	sext.w	a0, t6
	li	t1, 2143297521
	sext.w	t1, t1
	mul	t2, a0, t1
	srli	t2, t2, 32
	sext.w	t2, t2
	sraiw	t2, t2, 8
	sraiw	t3, t2, 31
	negw	t3, t3
	addw	a0, t2, t3
	li	t1, 513
	mulw	t1, a0, t1
	subw	a0, t6, t1
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	addiw	a0, a0, 64
	sw	a0, -88(s0)
	lla	t1, N_eff
	lw	a0, -88(s0)
	sw	a0, 0(t1)
	li	a0, 134
	sw	a0, -96(s0)
	addi	sp, sp, -16
	li	a0, 134
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	lla	a0, In
	sd	a0, -104(s0)
	ld	a0, -104(s0)
	mv	a0, t4
	call	init_matrix
	lla	a0, K
	sd	a0, -112(s0)
	ld	a0, -112(s0)
	mv	a0, t4
	call	init_kernel
	lla	a0, In
	mv	s1, a0
	sd	s1, -120(s0)
	lla	a0, Out
	mv	s11, a0
	sd	s11, -128(s0)
	lla	a0, K
	mv	s10, a0
	sd	s10, -136(s0)
	addi	sp, sp, -32
	mv	a0, s1
	sd	a0, 0(sp)
	mv	a0, s11
	sd	a0, 8(sp)
	mv	a0, s10
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	conv2d
	addi	sp, sp, 32
	lla	a0, Out
	mv	s9, a0
	sd	s9, -144(s0)
	mv	a0, s9
	mv	a0, t4
	call	nonlinear
	lla	a0, Out
	mv	s8, a0
	sd	s8, -152(s0)
	mv	a0, s8
	mv	a0, t4
	call	row_reduce
	lla	a0, Out
	mv	s7, a0
	sd	s7, -160(s0)
	mv	a0, s7
	mv	a0, t4
	call	checksum
	mv	s6, a0
	sw	s6, -168(s0)
	addi	t1, s0, -20
	mv	a0, s6
	sw	a0, 0(t1)
	mv	s6, a0
	li	a0, 145
	mv	s5, a0
	sw	s5, -176(s0)
	addi	sp, sp, -16
	li	a0, 145
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -20(s0)
	mv	s4, a0
	sw	s4, -184(s0)
	mv	a0, s4
	sext.w	a0, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	mv	s3, a0
	sw	s3, -192(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	mv	s2, a0
	sw	s2, -200(s0)
	mv	a0, s2
	sext.w	a0, a0
	j	.Lreturn_main_8
	li	a0, 0
.Lreturn_main_8:
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
