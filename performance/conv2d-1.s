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
	li	t0, 176
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	li	a0, 0
	sw	a0, -40(s0)
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	li	a0, 2048
	sw	a0, -56(s0)
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
	sw	a0, -64(s0)
	addi	t1, s0, -24
	lw	a0, -40(s0)
	sw	a0, 0(t1)
.L_wh_get_random_0:
	li	a0, 128
	sw	a0, -40(s0)
	li	a0, 65535
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_get_random_1
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addiw	a0, a0, 128
	sw	a0, -96(s0)
	lla	t1, state
	sw	a0, 0(t1)
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
	sw	a0, 0(t1)
	addi	t0, s0, -24
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	addiw	a0, a0, 1
	sw	a0, -120(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	j	.L_wh_get_random_0
.L_whe_get_random_1:
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 65535
	sw	a0, -48(s0)
	lw	a0, -40(s0)
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
	sw	a0, -56(s0)
	lla	t1, state
	sw	a0, 0(t1)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_get_random_0
	li	a0, 0
.Lreturn_get_random_0:
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
	addi	sp, sp, -80
	li	t0, 80
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	sw	a2, -28(s0)
	mv	t4, a0
	mv	t5, a1
	mv	t6, a2
	mv	a0, t4
	sw	a0, -40(s0)
	mv	a0, t6
	sw	a0, -48(s0)
	lw	t2, -40(s0)
	mulw	a0, t2, a0
	sw	a0, -56(s0)
	mv	a0, t5
	sw	a0, -64(s0)
	lw	t2, -56(s0)
	addw	a0, t2, a0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	sext.w	a0, a0
	j	.Lreturn_idx_1
	li	a0, 0
.Lreturn_idx_1:
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
	addi	sp, sp, -416
	li	t0, 416
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_init_matrix_0:
	li	a0, 0
	sw	a0, -40(s0)
	li	a0, 2
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	t2, -64(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_init_matrix_1
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	sext.w	a0, a0
	sraiw	t1, a0, 31
	andi	t1, t1, 1
	addw	a0, a0, t1
	sraiw	a0, a0, 1
	sw	a0, -104(s0)
	lw	t2, -88(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	beqz	a0, .L_ifelse_init_matrix_3
.L_wh_init_matrix_4:
	li	a0, 65535
	sw	a0, -40(s0)
	ld	a0, -24(s0)
	sd	a0, -48(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lw	t2, -136(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -152(s0)
	lw	a0, -152(s0)
	beqz	a0, .L_whe_init_matrix_5
	call	get_random
	sw	a0, -160(s0)
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
	sw	a0, -168(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	lw	t2, -120(s0)
	lw	t1, -184(s0)
	mulw	t2, t2, t1
	lw	a0, -176(s0)
	addw	a0, t2, a0
	sw	a0, -192(s0)
	slliw	a0, a0, 2
	sw	a0, -200(s0)
	ld	t2, -48(s0)
	add	a0, t2, a0
	sd	a0, -208(s0)
	mv	t1, a0
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	addiw	a0, a0, 1
	sw	a0, -224(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_init_matrix_4
.L_whe_init_matrix_5:
	j	.L_ifend_init_matrix_2
.L_ifelse_init_matrix_3:
.L_wh_init_matrix_6:
	li	a0, -1
	sw	a0, -40(s0)
	ld	a0, -24(s0)
	sd	a0, -48(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -240(s0)
	lw	t2, -232(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -248(s0)
	lw	a0, -248(s0)
	beqz	a0, .L_whe_init_matrix_7
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -264(s0)
	lw	t2, -120(s0)
	lw	t1, -264(s0)
	mulw	t2, t2, t1
	lw	a0, -256(s0)
	addw	a0, t2, a0
	sw	a0, -272(s0)
	slliw	a0, a0, 2
	sw	a0, -280(s0)
	ld	t2, -48(s0)
	add	a0, t2, a0
	sd	a0, -288(s0)
	mv	t1, a0
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	addiw	a0, a0, 1
	sw	a0, -304(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_init_matrix_6
.L_whe_init_matrix_7:
.L_ifend_init_matrix_2:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	addiw	a0, a0, 1
	sw	a0, -48(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_init_matrix_0
.L_whe_init_matrix_1:
	j	.Lreturn_init_matrix_2
.Lreturn_init_matrix_2:
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
	addi	sp, sp, -192
	li	t0, 192
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_init_kernel_0:
	li	a0, 25
	sw	a0, -40(s0)
	li	a0, 3
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	ld	a0, -24(s0)
	sd	a0, -64(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -40(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_init_kernel_1
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -88(s0)
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
	sw	a0, -96(s0)
	addiw	a0, a0, -1
	sw	a0, -104(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	slliw	a0, a0, 2
	sw	a0, -120(s0)
	ld	t2, -64(s0)
	add	a0, t2, a0
	sd	a0, -128(s0)
	mv	t1, a0
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_init_kernel_0
.L_whe_init_kernel_1:
	j	.Lreturn_init_kernel_3
.Lreturn_init_kernel_3:
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
	addi	sp, sp, -1424
	li	t0, 1424
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	sd	a1, -32(s0)
	sd	a2, -40(s0)
	addi	t1, s0, -52
	lw	a0, -88(s0)
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
	li	a0, 0
	sw	a0, -88(s0)
	li	a0, 2
	sw	a0, -96(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
.L_wh_conv2d_0:
	li	a0, 0
	sw	a0, -88(s0)
	sw	a0, -96(s0)
	lw	a0, -88(s0)
	sw	a0, -104(s0)
	lw	a0, -88(s0)
	sw	a0, -112(s0)
	li	a0, 5
	sw	a0, -120(s0)
	li	a0, 1
	sw	a0, -128(s0)
	lw	a0, -88(s0)
	sw	a0, -136(s0)
	lw	a0, -128(s0)
	sw	a0, -144(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	ld	a0, -32(s0)
	sd	a0, -160(s0)
	lw	a0, -88(s0)
	sw	a0, -168(s0)
	lw	a0, -128(s0)
	sw	a0, -176(s0)
	lw	a0, -120(s0)
	sw	a0, -184(s0)
	addi	t0, s0, -44
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	lw	a0, -128(s0)
	sw	a0, -200(s0)
	lw	a0, -88(s0)
	sw	a0, -208(s0)
	lw	a0, -128(s0)
	sw	a0, -216(s0)
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	lla	t0, repeat_factor
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	lw	t2, -224(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -240(s0)
	lw	a0, -240(s0)
	beqz	a0, .L_whe_conv2d_1
	addi	t1, s0, -52
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_2:
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -248(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -256(s0)
	lw	t2, -248(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -264(s0)
	lw	a0, -264(s0)
	beqz	a0, .L_whe_conv2d_3
	addi	t1, s0, -56
	lw	a0, -96(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_4:
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	addi	t0, s0, -56
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	lw	t2, -288(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -304(s0)
	lw	a0, -304(s0)
	beqz	a0, .L_whe_conv2d_5
	addi	t1, s0, -60
	lw	a0, -104(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -64
	lw	a0, -112(s0)
	sw	a0, 0(t1)
.L_wh_conv2d_6:
	addi	t0, s0, -56
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -120(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -328(s0)
	lw	a0, -328(s0)
	beqz	a0, .L_whe_conv2d_7
	addi	t1, s0, -68
	lw	a0, -136(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -336(s0)
	lw	t2, -272(s0)
	addw	a0, t2, a0
	sw	a0, -344(s0)
	mv	t2, a0
	lw	a0, -152(s0)
	subw	a0, t2, a0
	sw	a0, -352(s0)
	addi	t1, s0, -72
	sw	a0, 0(t1)
.L_wh_conv2d_8:
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -360(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -208(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -368(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -376(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -384(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -184(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -392(s0)
	lw	a0, -392(s0)
	beqz	a0, .L_whe_conv2d_9
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -400(s0)
	lw	t2, -312(s0)
	addw	a0, t2, a0
	sw	a0, -408(s0)
	mv	t2, a0
	lw	a0, -192(s0)
	subw	a0, t2, a0
	sw	a0, -416(s0)
	addi	t1, s0, -76
	sw	a0, 0(t1)
	lw	a0, -376(s0)
	beqz	a0, .L_ifend_conv2d_10
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -424(s0)
	lw	t2, -376(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -432(s0)
	lw	a0, -432(s0)
	beqz	a0, .L_ifend_conv2d_11
	addi	t0, s0, -76
	lw	a0, 0(t0)
	sw	a0, -440(s0)
	li	a0, 0
	sw	a0, -448(s0)
	lw	t2, -440(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -456(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -464(s0)
	lw	a0, -464(s0)
	beqz	a0, .L_ifend_conv2d_12
	addi	t0, s0, -76
	lw	a0, 0(t0)
	sw	a0, -472(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -480(s0)
	lw	t2, -472(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -488(s0)
	lw	a0, -488(s0)
	beqz	a0, .L_ifend_conv2d_13
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -496(s0)
	ld	a0, -24(s0)
	sd	a0, -504(s0)
	addi	t0, s0, -72
	lw	a0, 0(t0)
	sw	a0, -512(s0)
	addi	t0, s0, -76
	lw	a0, 0(t0)
	sw	a0, -520(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -528(s0)
	lw	t2, -512(s0)
	lw	t1, -528(s0)
	mulw	t2, t2, t1
	lw	a0, -520(s0)
	addw	a0, t2, a0
	sw	a0, -536(s0)
	slliw	a0, a0, 2
	sw	a0, -544(s0)
	ld	t2, -504(s0)
	add	a0, t2, a0
	sd	a0, -552(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -560(s0)
	ld	a0, -40(s0)
	sd	a0, -568(s0)
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -576(s0)
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -584(s0)
	li	a0, 5
	sw	a0, -592(s0)
	lw	t2, -576(s0)
	lw	t1, -592(s0)
	mulw	t2, t2, t1
	lw	a0, -584(s0)
	addw	a0, t2, a0
	sw	a0, -600(s0)
	slliw	a0, a0, 2
	sw	a0, -608(s0)
	ld	t2, -568(s0)
	add	a0, t2, a0
	sd	a0, -616(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -624(s0)
	lw	t2, -560(s0)
	mulw	a0, t2, a0
	sw	a0, -632(s0)
	lw	t2, -496(s0)
	addw	a0, t2, a0
	sw	a0, -640(s0)
	addi	t1, s0, -60
	sw	a0, 0(t1)
.L_ifend_conv2d_13:
.L_ifend_conv2d_12:
.L_ifend_conv2d_11:
.L_ifend_conv2d_10:
	addi	t0, s0, -68
	lw	a0, 0(t0)
	sw	a0, -648(s0)
	addiw	a0, a0, 1
	sw	a0, -656(s0)
	addi	t1, s0, -68
	sw	a0, 0(t1)
	j	.L_wh_conv2d_8
.L_whe_conv2d_9:
	addi	t0, s0, -64
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	addiw	a0, a0, 1
	sw	a0, -664(s0)
	addi	t1, s0, -64
	sw	a0, 0(t1)
	j	.L_wh_conv2d_6
.L_whe_conv2d_7:
	addi	t0, s0, -60
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addi	t0, s0, -56
	lw	a0, 0(t0)
	sw	a0, -672(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -680(s0)
	lw	t2, -280(s0)
	lw	t1, -680(s0)
	mulw	t2, t2, t1
	lw	a0, -672(s0)
	addw	a0, t2, a0
	sw	a0, -688(s0)
	slliw	a0, a0, 2
	sw	a0, -696(s0)
	addiw	a0, a0, 0
	sw	a0, -704(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -712(s0)
	mv	t1, a0
	lw	a0, -272(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -56
	lw	a0, 0(t0)
	sw	a0, -720(s0)
	addiw	a0, a0, 1
	sw	a0, -728(s0)
	addi	t1, s0, -56
	sw	a0, 0(t1)
	j	.L_wh_conv2d_4
.L_whe_conv2d_5:
	addi	t0, s0, -52
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	addiw	a0, a0, 1
	sw	a0, -736(s0)
	addi	t1, s0, -52
	sw	a0, 0(t1)
	j	.L_wh_conv2d_2
.L_whe_conv2d_3:
	addi	t0, s0, -48
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	addiw	a0, a0, 1
	sw	a0, -104(s0)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	j	.L_wh_conv2d_0
.L_whe_conv2d_1:
	j	.Lreturn_conv2d_4
.Lreturn_conv2d_4:
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
	addi	sp, sp, -400
	li	t0, 400
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	addi	t1, s0, -32
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	addi	t1, s0, -44
	sw	a0, 0(t1)
	addi	t1, s0, -48
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_nonlinear_0:
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	sw	a0, -56(s0)
	mv	t2, a0
	mulw	a0, t2, a0
	sw	a0, -64(s0)
	ld	a0, -24(s0)
	sd	a0, -72(s0)
	li	a0, 3
	sw	a0, -80(s0)
	li	a0, 7
	sw	a0, -88(s0)
	li	a0, 97
	sw	a0, -96(s0)
	li	a0, 1
	sw	a0, -104(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -112(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -64(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -120(s0)
	lw	a0, -120(s0)
	beqz	a0, .L_whe_nonlinear_1
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -152(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	mv	t2, a0
	mulw	a0, t2, a0
	sw	a0, -160(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	lw	a0, -152(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -168(s0)
	lw	t2, -160(s0)
	addw	a0, t2, a0
	sw	a0, -176(s0)
	addi	t1, s0, -40
	sw	a0, 0(t1)
	addiw	a0, a0, -7
	sw	a0, -184(s0)
	addi	t1, s0, -44
	sw	a0, 0(t1)
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
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	slliw	a0, a0, 2
	sw	a0, -208(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -216(s0)
	mv	t1, a0
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	addiw	a0, a0, 1
	sw	a0, -232(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_nonlinear_0
.L_whe_nonlinear_1:
	j	.Lreturn_nonlinear_5
.Lreturn_nonlinear_5:
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
	addi	sp, sp, -528
	li	t0, 528
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	addi	t1, s0, -32
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_row_reduce_0:
	li	a0, 0
	sw	a0, -56(s0)
	ld	a0, -24(s0)
	sd	a0, -64(s0)
	lw	a0, -56(s0)
	sw	a0, -72(s0)
	li	a0, 1
	sw	a0, -80(s0)
	lw	a0, -56(s0)
	sw	a0, -88(s0)
	ld	a0, -64(s0)
	sd	a0, -96(s0)
	lw	a0, -56(s0)
	sw	a0, -104(s0)
	lw	a0, -80(s0)
	sw	a0, -112(s0)
	ld	a0, -64(s0)
	sd	a0, -120(s0)
	lw	a0, -56(s0)
	sw	a0, -128(s0)
	lw	a0, -80(s0)
	sw	a0, -136(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	lw	t2, -144(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -160(s0)
	lw	a0, -160(s0)
	beqz	a0, .L_whe_row_reduce_1
	addi	t1, s0, -32
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	sw	a0, 0(t1)
.L_wh_row_reduce_2:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -176(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	lw	t2, -176(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	beqz	a0, .L_whe_row_reduce_3
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -200(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -208(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -216(s0)
	lw	t2, -168(s0)
	lw	t1, -216(s0)
	mulw	t2, t2, t1
	lw	a0, -208(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	slliw	a0, a0, 2
	sw	a0, -232(s0)
	addiw	a0, a0, 0
	sw	a0, -240(s0)
	ld	t2, -64(s0)
	add	a0, t2, a0
	sd	a0, -248(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -256(s0)
	lw	t2, -200(s0)
	addw	a0, t2, a0
	sw	a0, -264(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -272(s0)
	addiw	a0, a0, 1
	sw	a0, -280(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh_row_reduce_2
.L_whe_row_reduce_3:
	addi	t1, s0, -36
	lw	a0, -88(s0)
	sw	a0, 0(t1)
.L_wh_row_reduce_4:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -296(s0)
	lw	t2, -288(s0)
	sext.w	t2, t2
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -304(s0)
	lw	a0, -304(s0)
	beqz	a0, .L_whe_row_reduce_5
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -312(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -320(s0)
	lw	t2, -72(s0)
	lw	t1, -320(s0)
	mulw	t2, t2, t1
	lw	a0, -312(s0)
	addw	a0, t2, a0
	sw	a0, -328(s0)
	slliw	a0, a0, 2
	sw	a0, -336(s0)
	addiw	a0, a0, 0
	sw	a0, -344(s0)
	ld	t2, -96(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -360(s0)
	mv	t2, a0
	lw	a0, -88(s0)
	subw	a0, t2, a0
	sw	a0, -368(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -376(s0)
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -384(s0)
	lw	t2, -168(s0)
	lw	t1, -384(s0)
	mulw	t2, t2, t1
	lw	a0, -376(s0)
	addw	a0, t2, a0
	sw	a0, -392(s0)
	slliw	a0, a0, 2
	sw	a0, -400(s0)
	addiw	a0, a0, 0
	sw	a0, -408(s0)
	ld	t2, -120(s0)
	add	a0, t2, a0
	sd	a0, -416(s0)
	mv	t1, a0
	lw	a0, -368(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -424(s0)
	addiw	a0, a0, 1
	sw	a0, -432(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh_row_reduce_4
.L_whe_row_reduce_5:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	addiw	a0, a0, 1
	sw	a0, -88(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_row_reduce_0
.L_whe_row_reduce_1:
	j	.Lreturn_row_reduce_6
.Lreturn_row_reduce_6:
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
	li	t0, 208
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sd	a0, -24(s0)
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -48(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_checksum_0:
	lla	t0, N_eff
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	sw	a0, -40(s0)
	mv	t2, a0
	mulw	a0, t2, a0
	sw	a0, -48(s0)
	ld	a0, -24(s0)
	sd	a0, -56(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -48(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_checksum_1
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -56(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -120(s0)
	lw	t2, -88(s0)
	addw	a0, t2, a0
	sw	a0, -128(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -136(s0)
	addiw	a0, a0, 1
	sw	a0, -144(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_checksum_0
.L_whe_checksum_1:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	lw	a0, -40(s0)
	sext.w	a0, a0
	j	.Lreturn_checksum_7
	li	a0, 0
.Lreturn_checksum_7:
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
	addi	sp, sp, -224
	li	t0, 224
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	call	getint
	sw	a0, -40(s0)
	lla	t1, state
	sw	a0, 0(t1)
	call	getint
	sw	a0, -48(s0)
	lla	t1, repeat_factor
	sw	a0, 0(t1)
	lla	t0, state
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	li	a0, 513
	sw	a0, -64(s0)
	lw	a0, -56(s0)
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
	sw	a0, -72(s0)
	li	a0, 64
	sw	a0, -80(s0)
	lw	a0, -72(s0)
	addiw	a0, a0, 64
	sw	a0, -88(s0)
	lla	t1, N_eff
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
	mv	t4, a0
	mv	a0, t4
	call	init_matrix
	lla	a0, K
	sd	a0, -112(s0)
	ld	a0, -112(s0)
	mv	t4, a0
	mv	a0, t4
	call	init_kernel
	lla	a0, In
	sd	a0, -120(s0)
	lla	a0, Out
	sd	a0, -128(s0)
	lla	a0, K
	sd	a0, -136(s0)
	addi	sp, sp, -32
	ld	a0, -120(s0)
	sd	a0, 0(sp)
	ld	a0, -128(s0)
	sd	a0, 8(sp)
	ld	a0, -136(s0)
	sd	a0, 16(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	ld	a2, 16(sp)
	call	conv2d
	addi	sp, sp, 32
	lla	a0, Out
	sd	a0, -144(s0)
	ld	a0, -144(s0)
	mv	t4, a0
	mv	a0, t4
	call	nonlinear
	lla	a0, Out
	sd	a0, -152(s0)
	ld	a0, -152(s0)
	mv	t4, a0
	mv	a0, t4
	call	row_reduce
	lla	a0, Out
	sd	a0, -160(s0)
	ld	a0, -160(s0)
	mv	t4, a0
	mv	a0, t4
	call	checksum
	sw	a0, -168(s0)
	li	a0, 145
	sw	a0, -176(s0)
	addi	sp, sp, -16
	li	a0, 145
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lw	a0, -168(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putint
	li	a0, 10
	sw	a0, -184(s0)
	li	a0, 10
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	putch
	li	a0, 0
	sw	a0, -192(s0)
	lw	a0, -192(s0)
	sext.w	a0, a0
	j	.Lreturn_main_8
	li	a0, 0
.Lreturn_main_8:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
