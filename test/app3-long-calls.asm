app3.elf:     file format elf32-littlearm


Disassembly of section .app:

08011000 <test>:
 8011000:	b580      	push	{r7, lr}
 8011002:	b084      	sub	sp, #16
 8011004:	af00      	add	r7, sp, #0
 8011006:	6078      	str	r0, [r7, #4]
 8011008:	4b12      	ldr	r3, [pc, #72]	; (8011054 <test+0x54>)
 801100a:	f859 3003 	ldr.w	r3, [r9, r3]
 801100e:	68db      	ldr	r3, [r3, #12]
 8011010:	60fb      	str	r3, [r7, #12]
 8011012:	4b11      	ldr	r3, [pc, #68]	; (8011058 <test+0x58>)
 8011014:	447b      	add	r3, pc
 8011016:	4618      	mov	r0, r3
 8011018:	4b10      	ldr	r3, [pc, #64]	; (801105c <test+0x5c>)
 801101a:	f859 3003 	ldr.w	r3, [r9, r3]
 801101e:	4798      	blx	r3
 8011020:	687b      	ldr	r3, [r7, #4]
 8011022:	4618      	mov	r0, r3
 8011024:	4b0e      	ldr	r3, [pc, #56]	; (8011060 <test+0x60>)
 8011026:	f859 3003 	ldr.w	r3, [r9, r3]
 801102a:	4798      	blx	r3
 801102c:	4b0d      	ldr	r3, [pc, #52]	; (8011064 <test+0x64>)
 801102e:	447b      	add	r3, pc
 8011030:	4618      	mov	r0, r3
 8011032:	4b0a      	ldr	r3, [pc, #40]	; (801105c <test+0x5c>)
 8011034:	f859 3003 	ldr.w	r3, [r9, r3]
 8011038:	4798      	blx	r3
 801103a:	68f8      	ldr	r0, [r7, #12]
 801103c:	4b08      	ldr	r3, [pc, #32]	; (8011060 <test+0x60>)
 801103e:	f859 3003 	ldr.w	r3, [r9, r3]
 8011042:	4798      	blx	r3
 8011044:	4b08      	ldr	r3, [pc, #32]	; (8011068 <test+0x68>)
 8011046:	f859 3003 	ldr.w	r3, [r9, r3]
 801104a:	4798      	blx	r3
 801104c:	f107 0710 	add.w	r7, r7, #16
 8011050:	46bd      	mov	sp, r7
 8011052:	bd80      	pop	{r7, pc}
 8011054:	0000000c 	.word	0x0000000c
 8011058:	000000c8 	.word	0x000000c8
 801105c:	00000004 	.word	0x00000004
 8011060:	00000010 	.word	0x00000010
 8011064:	000000b6 	.word	0x000000b6
	...

08011070 <AppThread3>:
 8011070:	b580      	push	{r7, lr}
 8011072:	b084      	sub	sp, #16
 8011074:	af00      	add	r7, sp, #0
 8011076:	6078      	str	r0, [r7, #4]
 8011078:	f04f 33ff 	mov.w	r3, #4294967295
 801107c:	60fb      	str	r3, [r7, #12]
 801107e:	4b13      	ldr	r3, [pc, #76]	; (80110cc <AppThread3+0x5c>)
 8011080:	447b      	add	r3, pc
 8011082:	4618      	mov	r0, r3
 8011084:	4b12      	ldr	r3, [pc, #72]	; (80110d0 <AppThread3+0x60>)
 8011086:	f859 3003 	ldr.w	r3, [r9, r3]
 801108a:	4798      	blx	r3
 801108c:	4b11      	ldr	r3, [pc, #68]	; (80110d4 <AppThread3+0x64>)
 801108e:	f859 3003 	ldr.w	r3, [r9, r3]
 8011092:	4798      	blx	r3
 8011094:	f44f 6300 	mov.w	r3, #2048	; 0x800
 8011098:	f2c4 0301 	movt	r3, #16385	; 0x4001
 801109c:	f44f 6200 	mov.w	r2, #2048	; 0x800
 80110a0:	f2c4 0201 	movt	r2, #16385	; 0x4001
 80110a4:	68d2      	ldr	r2, [r2, #12]
 80110a6:	f082 0208 	eor.w	r2, r2, #8
 80110aa:	60da      	str	r2, [r3, #12]
 80110ac:	68fb      	ldr	r3, [r7, #12]
 80110ae:	f103 33ff 	add.w	r3, r3, #4294967295
 80110b2:	60fb      	str	r3, [r7, #12]
 80110b4:	68fb      	ldr	r3, [r7, #12]
 80110b6:	4618      	mov	r0, r3
 80110b8:	4b07      	ldr	r3, [pc, #28]	; (80110d8 <AppThread3+0x68>)
 80110ba:	447b      	add	r3, pc
 80110bc:	4798      	blx	r3
 80110be:	f44f 707a 	mov.w	r0, #1000	; 0x3e8
 80110c2:	4b06      	ldr	r3, [pc, #24]	; (80110dc <AppThread3+0x6c>)
 80110c4:	f859 3003 	ldr.w	r3, [r9, r3]
 80110c8:	4798      	blx	r3
 80110ca:	e7e3      	b.n	8011094 <AppThread3+0x24>
 80110cc:	00000068 	.word	0x00000068
 80110d0:	00000004 	.word	0x00000004
 80110d4:	00000000 	.word	0x00000000
 80110d8:	ffffff43 	.word	0xffffff43
 80110dc:	00000008 	.word	0x00000008
 80110e0:	33505041 	.word	0x33505041
 80110e4:	0000203a 	.word	0x0000203a
 80110e8:	00202d20 	.word	0x00202d20
 80110ec:	6c6c6548 	.word	0x6c6c6548
 80110f0:	7266206f 	.word	0x7266206f
 80110f4:	41206d6f 	.word	0x41206d6f
 80110f8:	00335050 	.word	0x00335050
