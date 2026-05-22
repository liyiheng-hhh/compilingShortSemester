	.bss
	.align	2
	.globl	hashmod
hashmod:
	.zero	4
	.bss
	.align	2
	.globl	bucket
bucket:
	.zero	40000000
	.bss
	.align	2
	.globl	head
head:
	.zero	40000000
	.bss
	.align	2
	.globl	next
next:
	.zero	40000000
	.bss
	.align	2
	.globl	nextvalue
nextvalue:
	.zero	40000000
	.bss
	.align	2
	.globl	key
key:
	.zero	40000000
	.bss
	.align	2
	.globl	value
value:
	.zero	40000000
	.bss
	.align	2
	.globl	cnt
cnt:
	.zero	4
	.bss
	.align	2
	.globl	keys
keys:
	.zero	40000000
	.bss
	.align	2
	.globl	values
values:
	.zero	40000000
	.bss
	.align	2
	.globl	requests
requests:
	.zero	40000000
	.bss
	.align	2
	.globl	ans
ans:
	.zero	40000000
	.text
	.text
	.align	1
	.globl	hash
	.type	hash, @function
hash:
	addi	sp, sp, -64
	li	t0, 64
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	mv	t4, a0
	mv	a0, t4
	sw	a0, -40(s0)
	lla	t0, hashmod
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	lw	t2, -40(s0)
	remw	a0, t2, a0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_hash_0
	li	a0, 0
.Lreturn_hash_0:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	hash, .-hash
	.text
	.align	1
	.globl	insert
	.type	insert, @function
insert:
	addi	sp, sp, -864
	li	t0, 864
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	sw	a1, -24(s0)
	mv	t4, a0
	mv	t5, a1
	addi	t1, s0, -32
	lw	a0, -40(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -40(s0)
	mv	a0, t4
	sw	a0, -48(s0)
	lla	t0, hashmod
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	lw	t2, -48(s0)
	remw	a0, t2, a0
	sw	a0, -64(s0)
	lla	a0, head
	sd	a0, -72(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -80(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -88(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -96(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -104(s0)
	lw	a0, -104(s0)
	beqz	a0, .L_ifend_insert_0
	lla	t0, cnt
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	lla	t1, cnt
	sw	a0, 0(t1)
	lla	a0, head
	sd	a0, -72(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -80(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -88(s0)
	mv	t1, a0
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -96(s0)
	lla	a0, key
	sd	a0, -104(s0)
	lw	a0, -56(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -104(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t1, a0
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	mv	a0, t5
	sw	a0, -128(s0)
	lla	a0, value
	sd	a0, -136(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	mv	t1, a0
	lw	a0, -128(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -152(s0)
	lla	a0, next
	sd	a0, -160(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -168(s0)
	mv	t1, a0
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -176(s0)
	lla	a0, nextvalue
	sd	a0, -184(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -192(s0)
	mv	t1, a0
	lw	a0, -176(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -200(s0)
	lw	a0, -200(s0)
	sext.w	a0, a0
	j	.Lreturn_insert_1
.L_ifend_insert_0:
	lla	a0, head
	sd	a0, -40(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -48(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -56(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -72(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_insert_1:
	li	a0, 0
	sw	a0, -40(s0)
	lla	a0, key
	sd	a0, -48(s0)
	mv	a0, t4
	sw	a0, -56(s0)
	lla	a0, next
	sd	a0, -72(s0)
	lw	a0, -40(s0)
	sw	a0, -80(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -88(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -96(s0)
	lw	a0, -96(s0)
	beqz	a0, .L_whe_insert_2
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -104(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -48(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -128(s0)
	mv	t2, a0
	lw	a0, -56(s0)
	subw	a0, t2, a0
	sw	a0, -136(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -144(s0)
	lw	a0, -144(s0)
	beqz	a0, .L_ifend_insert_3
	lla	t0, cnt
	lw	a0, 0(t0)
	sw	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	lw	a0, -48(s0)
	addiw	a0, a0, 1
	sw	a0, -152(s0)
	lla	t1, cnt
	sw	a0, 0(t1)
	lla	a0, nextvalue
	sd	a0, -160(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	slliw	a0, a0, 2
	sw	a0, -176(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -184(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -192(s0)
	lw	a0, -152(s0)
	slliw	a0, a0, 2
	sw	a0, -200(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -208(s0)
	mv	t1, a0
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	lla	a0, nextvalue
	sd	a0, -216(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -224(s0)
	slliw	a0, a0, 2
	sw	a0, -232(s0)
	ld	t2, -216(s0)
	add	a0, t2, a0
	sd	a0, -240(s0)
	mv	t1, a0
	lw	a0, -152(s0)
	sw	a0, 0(t1)
	mv	a0, t5
	sw	a0, -248(s0)
	lla	a0, value
	sd	a0, -256(s0)
	mv	t2, a0
	lw	a0, -200(s0)
	add	a0, t2, a0
	sd	a0, -264(s0)
	mv	t1, a0
	lw	a0, -248(s0)
	sw	a0, 0(t1)
	li	a0, 1
	sw	a0, -272(s0)
	lw	a0, -272(s0)
	sext.w	a0, a0
	j	.Lreturn_insert_1
.L_ifend_insert_3:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -280(s0)
	slliw	a0, a0, 2
	sw	a0, -288(s0)
	addiw	a0, a0, 0
	sw	a0, -296(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -304(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -312(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_insert_1
.L_whe_insert_2:
	lla	t0, cnt
	lw	a0, 0(t0)
	sw	a0, -40(s0)
	li	a0, 1
	sw	a0, -48(s0)
	lw	a0, -40(s0)
	addiw	a0, a0, 1
	sw	a0, -56(s0)
	lla	t1, cnt
	sw	a0, 0(t1)
	lla	a0, head
	sd	a0, -72(s0)
	lw	a0, -64(s0)
	slliw	a0, a0, 2
	sw	a0, -80(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -88(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -96(s0)
	lla	a0, next
	sd	a0, -104(s0)
	lw	a0, -56(s0)
	slliw	a0, a0, 2
	sw	a0, -112(s0)
	ld	t2, -104(s0)
	add	a0, t2, a0
	sd	a0, -120(s0)
	mv	t1, a0
	lw	a0, -96(s0)
	sw	a0, 0(t1)
	lla	a0, head
	sd	a0, -128(s0)
	mv	t2, a0
	lw	a0, -80(s0)
	add	a0, t2, a0
	sd	a0, -136(s0)
	mv	t1, a0
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	mv	a0, t4
	sw	a0, -144(s0)
	lla	a0, key
	sd	a0, -152(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -160(s0)
	mv	t1, a0
	lw	a0, -144(s0)
	sw	a0, 0(t1)
	mv	a0, t5
	sw	a0, -168(s0)
	lla	a0, value
	sd	a0, -176(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -184(s0)
	mv	t1, a0
	lw	a0, -168(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -192(s0)
	lla	a0, nextvalue
	sd	a0, -200(s0)
	mv	t2, a0
	lw	a0, -112(s0)
	add	a0, t2, a0
	sd	a0, -208(s0)
	mv	t1, a0
	lw	a0, -192(s0)
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -216(s0)
	lw	a0, -216(s0)
	sext.w	a0, a0
	j	.Lreturn_insert_1
	li	a0, 0
.Lreturn_insert_1:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	insert, .-insert
	.text
	.align	1
	.globl	reduce
	.type	reduce, @function
reduce:
	addi	sp, sp, -592
	li	t0, 592
	add	t0, sp, t0
	sd	s0, -16(t0)
	mv	s0, t0
	sw	a0, -20(s0)
	mv	t4, a0
	addi	t1, s0, -32
	lw	a0, -56(s0)
	sw	a0, 0(t1)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	li	a0, 0
	sw	a0, -56(s0)
	mv	a0, t4
	sw	a0, -64(s0)
	lla	t0, hashmod
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	lw	t2, -64(s0)
	remw	a0, t2, a0
	sw	a0, -80(s0)
	addi	t1, s0, -24
	sw	a0, 0(t1)
	lla	a0, head
	sd	a0, -88(s0)
	lw	a0, -80(s0)
	slliw	a0, a0, 2
	sw	a0, -96(s0)
	ld	t2, -88(s0)
	add	a0, t2, a0
	sd	a0, -104(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -112(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
.L_wh_reduce_0:
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -56(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -64(s0)
	lw	a0, -64(s0)
	beqz	a0, .L_whe_reduce_1
	lla	a0, key
	sd	a0, -72(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	slliw	a0, a0, 2
	sw	a0, -88(s0)
	ld	t2, -72(s0)
	add	a0, t2, a0
	sd	a0, -96(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -104(s0)
	mv	a0, t4
	sw	a0, -112(s0)
	lw	t2, -104(s0)
	subw	a0, t2, a0
	sw	a0, -120(s0)
	sext.w	a0, a0
	seqz	a0, a0
	sw	a0, -128(s0)
	lw	a0, -128(s0)
	beqz	a0, .L_ifend_reduce_2
	li	a0, 0
	sw	a0, -136(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -144(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
.L_wh_reduce_3:
	lla	a0, value
	sd	a0, -152(s0)
	lla	a0, nextvalue
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	ld	a0, -152(s0)
	sd	a0, -152(s0)
	ld	a0, -160(s0)
	sd	a0, -160(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	sext.w	a0, a0
	snez	a0, a0
	sw	a0, -176(s0)
	lw	a0, -176(s0)
	beqz	a0, .L_whe_reduce_4
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -184(s0)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -192(s0)
	slliw	a0, a0, 2
	sw	a0, -200(s0)
	ld	t2, -152(s0)
	add	a0, t2, a0
	sd	a0, -208(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -216(s0)
	lw	t2, -184(s0)
	addw	a0, t2, a0
	sw	a0, -224(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	addi	t0, s0, -36
	lw	a0, 0(t0)
	sw	a0, -232(s0)
	slliw	a0, a0, 2
	sw	a0, -240(s0)
	ld	t2, -160(s0)
	add	a0, t2, a0
	sd	a0, -248(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -256(s0)
	addi	t1, s0, -36
	sw	a0, 0(t1)
	j	.L_wh_reduce_3
.L_whe_reduce_4:
	mv	a0, t4
	sw	a0, -264(s0)
	li	a0, 100
	sw	a0, -272(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -264(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -280(s0)
	lw	a0, -280(s0)
	beqz	a0, .L_ifelse_reduce_6
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -288(s0)
	slliw	a0, a0, 1
	sw	a0, -296(s0)
	lw	a0, -296(s0)
	sext.w	a0, a0
	j	.Lreturn_reduce_2
	j	.L_ifend_reduce_5
.L_ifelse_reduce_6:
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -304(s0)
	li	a0, 3
	sw	a0, -312(s0)
	lw	a0, -304(s0)
	slliw	t2, a0, 1
	addw	a0, t2, a0
	sw	a0, -320(s0)
	lw	a0, -320(s0)
	sext.w	a0, a0
	j	.Lreturn_reduce_2
.L_ifend_reduce_5:
.L_ifend_reduce_2:
	lla	a0, next
	sd	a0, -328(s0)
	addi	t0, s0, -28
	lw	a0, 0(t0)
	sw	a0, -336(s0)
	slliw	a0, a0, 2
	sw	a0, -344(s0)
	ld	t2, -328(s0)
	add	a0, t2, a0
	sd	a0, -352(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -360(s0)
	addi	t1, s0, -28
	sw	a0, 0(t1)
	j	.L_wh_reduce_0
.L_whe_reduce_1:
	li	a0, 0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_reduce_2
	li	a0, 0
.Lreturn_reduce_2:
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	reduce, .-reduce
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp, sp, -304
	li	t0, 304
	add	t0, sp, t0
	sd	ra, -8(t0)
	sd	s0, -16(t0)
	mv	s0, t0
	call	getint
	sw	a0, -40(s0)
	lla	t1, hashmod
	sw	a0, 0(t1)
	lla	a0, keys
	sd	a0, -48(s0)
	ld	a0, -48(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -56(s0)
	lla	a0, values
	sd	a0, -64(s0)
	ld	a0, -64(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -72(s0)
	lla	a0, requests
	sd	a0, -80(s0)
	ld	a0, -80(s0)
	mv	t4, a0
	mv	a0, t4
	call	getarray
	sw	a0, -88(s0)
	li	a0, 81
	sw	a0, -96(s0)
	addi	sp, sp, -16
	li	a0, 81
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_starttime
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -104(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_main_0:
	lla	a0, keys
	sd	a0, -40(s0)
	lla	a0, values
	sd	a0, -48(s0)
	li	a0, 1
	sw	a0, -64(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -72(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -56(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -80(s0)
	lw	a0, -80(s0)
	beqz	a0, .L_whe_main_1
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -96(s0)
	slliw	a0, a0, 2
	sw	a0, -104(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -112(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -120(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	ld	t2, -48(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -152(s0)
	addi	sp, sp, -16
	lw	a0, -120(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	lw	a0, -152(s0)
	sext.w	a0, a0
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	insert
	addi	sp, sp, 16
	sw	a0, -160(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -168(s0)
	addiw	a0, a0, 1
	sw	a0, -176(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_main_0
.L_whe_main_1:
	li	a0, 0
	sw	a0, -40(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
.L_wh_main_2:
	lla	a0, requests
	sd	a0, -40(s0)
	lla	a0, ans
	sd	a0, -48(s0)
	li	a0, 1
	sw	a0, -56(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -64(s0)
	mv	t2, a0
	sext.w	t2, t2
	lw	a0, -88(s0)
	sext.w	a0, a0
	slt	a0, t2, a0
	sw	a0, -72(s0)
	lw	a0, -72(s0)
	beqz	a0, .L_whe_main_3
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -80(s0)
	slliw	a0, a0, 2
	sw	a0, -96(s0)
	ld	t2, -40(s0)
	add	a0, t2, a0
	sd	a0, -104(s0)
	mv	t2, a0
	lw	a0, 0(t2)
	sw	a0, -112(s0)
	lw	a0, -112(s0)
	sext.w	a0, a0
	mv	t4, a0
	mv	a0, t4
	call	reduce
	sw	a0, -120(s0)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -128(s0)
	slliw	a0, a0, 2
	sw	a0, -136(s0)
	ld	t2, -48(s0)
	add	a0, t2, a0
	sd	a0, -144(s0)
	mv	t1, a0
	lw	a0, -120(s0)
	sw	a0, 0(t1)
	addi	t0, s0, -32
	lw	a0, 0(t0)
	sw	a0, -152(s0)
	addiw	a0, a0, 1
	sw	a0, -160(s0)
	addi	t1, s0, -32
	sw	a0, 0(t1)
	j	.L_wh_main_2
.L_whe_main_3:
	li	a0, 93
	sw	a0, -40(s0)
	addi	sp, sp, -16
	li	a0, 93
	sd	a0, 0(sp)
	ld	a0, 0(sp)
	call	_sysy_stoptime
	addi	sp, sp, 16
	lla	a0, ans
	sd	a0, -48(s0)
	addi	sp, sp, -16
	lw	a0, -88(s0)
	sext.w	a0, a0
	sd	a0, 0(sp)
	ld	a0, -48(s0)
	sd	a0, 8(sp)
	ld	a0, 0(sp)
	ld	a1, 8(sp)
	call	putarray
	addi	sp, sp, 16
	li	a0, 0
	sw	a0, -56(s0)
	lw	a0, -56(s0)
	sext.w	a0, a0
	j	.Lreturn_main_3
	li	a0, 0
.Lreturn_main_3:
	ld	ra, -8(s0)
	ld	t0, -16(s0)
	mv	sp, s0
	mv	s0, t0
	ret
	.size	main, .-main
