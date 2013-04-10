app3.elf:     file format elf32-littlearm


Disassembly of section .app:

08011000 <test>:
 8011000:	b580      	push	{r7, lr}
 8011002:	b084      	sub	sp, #16
 8011004:	af00      	add	r7, sp, #0
 8011006:	6078      	str	r0, [r7, #4]
 8011008:	f640 5380 	movw	r3, #3456	; 0xd80
 801100c:	f2c2 0300 	movt	r3, #8192	; 0x2000
 8011010:	68db      	ldr	r3, [r3, #12]
 8011012:	60fb      	str	r3, [r7, #12]
 8011014:	f241 00a0 	movw	r0, #4256	; 0x10a0
 8011018:	f6c0 0001 	movt	r0, #2049	; 0x801
 801101c:	f7f2 fb80 	bl	8003720 <print>
 8011020:	687b      	ldr	r3, [r7, #4]
 8011022:	4618      	mov	r0, r3
 8011024:	f7f2 fb1c 	bl	8003660 <printn>
 8011028:	f241 00a8 	movw	r0, #4264	; 0x10a8
 801102c:	f6c0 0001 	movt	r0, #2049	; 0x801
 8011030:	f7f2 fb76 	bl	8003720 <print>
 8011034:	68f8      	ldr	r0, [r7, #12]
 8011036:	f7f2 fb13 	bl	8003660 <printn>
 801103a:	f7f2 fb99 	bl	8003770 <printnl>
 801103e:	f107 0710 	add.w	r7, r7, #16
 8011042:	46bd      	mov	sp, r7
 8011044:	bd80      	pop	{r7, pc}
 8011046:	bf00      	nop
	...

08011050 <AppThread3>:
 8011050:	b580      	push	{r7, lr}
 8011052:	b084      	sub	sp, #16
 8011054:	af00      	add	r7, sp, #0
 8011056:	6078      	str	r0, [r7, #4]
 8011058:	f04f 33ff 	mov.w	r3, #4294967295
 801105c:	60fb      	str	r3, [r7, #12]
 801105e:	f241 00ac 	movw	r0, #4268	; 0x10ac
 8011062:	f6c0 0001 	movt	r0, #2049	; 0x801
 8011066:	f7f2 fb5b 	bl	8003720 <print>
 801106a:	f7f2 fb81 	bl	8003770 <printnl>
 801106e:	f44f 6300 	mov.w	r3, #2048	; 0x800
 8011072:	f2c4 0301 	movt	r3, #16385	; 0x4001
 8011076:	f44f 6200 	mov.w	r2, #2048	; 0x800
 801107a:	f2c4 0201 	movt	r2, #16385	; 0x4001
 801107e:	68d2      	ldr	r2, [r2, #12]
 8011080:	f082 0208 	eor.w	r2, r2, #8
 8011084:	60da      	str	r2, [r3, #12]
 8011086:	68fb      	ldr	r3, [r7, #12]
 8011088:	f103 33ff 	add.w	r3, r3, #4294967295
 801108c:	60fb      	str	r3, [r7, #12]
 801108e:	68fb      	ldr	r3, [r7, #12]
 8011090:	4618      	mov	r0, r3
 8011092:	f7ff ffb5 	bl	8011000 <test>
 8011096:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
 801109a:	f7ef ff11 	bl	8000ec0 <chThdSleep>
 801109e:	e7e6      	b.n	801106e <AppThread3+0x1e>

080110a0 <.LC0>:
 80110a0:	33505041 	.word	0x33505041
 80110a4:	0000203a 	.word	0x0000203a

080110a8 <.LC1>:
 80110a8:	00202d20 	.word	0x00202d20

080110ac <.LC2>:
 80110ac:	6c6c6548 	.word	0x6c6c6548
 80110b0:	7266206f 	.word	0x7266206f
 80110b4:	41206d6f 	.word	0x41206d6f
 80110b8:	00335050 	.word	0x00335050
