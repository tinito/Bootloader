app3.elf:     file format elf32-littlearm


Disassembly of section .app:

08011000 <test>:
 8011000:	b580      	push	{r7, lr}
 8011002:	b084      	sub	sp, #16
 8011004:	af00      	add	r7, sp, #0
 8011006:	6078      	str	r0, [r7, #4]
 8011008:	4b0d      	ldr	r3, [pc, #52]	; (8011040 <test+0x40>)
 801100a:	f859 3003 	ldr.w	r3, [r9, r3]
 801100e:	68db      	ldr	r3, [r3, #12]
 8011010:	60fb      	str	r3, [r7, #12]
 8011012:	4b0c      	ldr	r3, [pc, #48]	; (8011044 <test+0x44>)
 8011014:	447b      	add	r3, pc
 8011016:	4618      	mov	r0, r3
 8011018:	f7f2 fb82 	bl	8003720 <print>
 801101c:	687b      	ldr	r3, [r7, #4]
 801101e:	4618      	mov	r0, r3
 8011020:	f7f2 fb1e 	bl	8003660 <printn>
 8011024:	4b08      	ldr	r3, [pc, #32]	; (8011048 <test+0x48>)
 8011026:	447b      	add	r3, pc
 8011028:	4618      	mov	r0, r3
 801102a:	f7f2 fb79 	bl	8003720 <print>
 801102e:	68f8      	ldr	r0, [r7, #12]
 8011030:	f7f2 fb16 	bl	8003660 <printn>
 8011034:	f7f2 fb9c 	bl	8003770 <printnl>
 8011038:	f107 0710 	add.w	r7, r7, #16
 801103c:	46bd      	mov	sp, r7
 801103e:	bd80      	pop	{r7, pc}
 8011040:	00000000 	.word	0x00000000
 8011044:	00000098 	.word	0x00000098
 8011048:	0000008e 	.word	0x0000008e
 801104c:	00000000 	.word	0x00000000

08011050 <AppThread3>:
 8011050:	b580      	push	{r7, lr}
 8011052:	b084      	sub	sp, #16
 8011054:	af00      	add	r7, sp, #0
 8011056:	6078      	str	r0, [r7, #4]
 8011058:	f04f 33ff 	mov.w	r3, #4294967295
 801105c:	60fb      	str	r3, [r7, #12]
 801105e:	4b10      	ldr	r3, [pc, #64]	; (80110a0 <AppThread3+0x50>)
 8011060:	447b      	add	r3, pc
 8011062:	4618      	mov	r0, r3
 8011064:	f7f2 fb5c 	bl	8003720 <print>
 8011068:	f7f2 fb82 	bl	8003770 <printnl>
 801106c:	f44f 6300 	mov.w	r3, #2048	; 0x800
 8011070:	f2c4 0301 	movt	r3, #16385	; 0x4001
 8011074:	f44f 6200 	mov.w	r2, #2048	; 0x800
 8011078:	f2c4 0201 	movt	r2, #16385	; 0x4001
 801107c:	68d2      	ldr	r2, [r2, #12]
 801107e:	f082 0208 	eor.w	r2, r2, #8
 8011082:	60da      	str	r2, [r3, #12]
 8011084:	68fb      	ldr	r3, [r7, #12]
 8011086:	f103 33ff 	add.w	r3, r3, #4294967295
 801108a:	60fb      	str	r3, [r7, #12]
 801108c:	68fb      	ldr	r3, [r7, #12]
 801108e:	4618      	mov	r0, r3
 8011090:	f7ff ffb6 	bl	8011000 <test>
 8011094:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
 8011098:	f7ef ff12 	bl	8000ec0 <chThdSleep>
 801109c:	e7e6      	b.n	801106c <AppThread3+0x1c>
 801109e:	bf00      	nop
 80110a0:	00000058 	.word	0x00000058
	...
 80110b0:	33505041 	.word	0x33505041
 80110b4:	0000203a 	.word	0x0000203a
 80110b8:	00202d20 	.word	0x00202d20
 80110bc:	6c6c6548 	.word	0x6c6c6548
 80110c0:	7266206f 	.word	0x7266206f
 80110c4:	41206d6f 	.word	0x41206d6f
 80110c8:	00335050 	.word	0x00335050
