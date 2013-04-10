Main	
	r0	0x0	
	r1	0x200012e8	
	r2	0x200012e8	
	r3	0x0	
	r4	0x8011051	
	r5	0x0	
	r6	0x0	
	r7	0x20002474	
	r8	0	
	r9	0x0	
	r10	0x0	
	r11	0x0	
	r12	1	
	sp	0x20002474	
	lr	0x08000b4d	
	pc	0x080003af	
	f0	0	
	f1	0	
	f2	0	
	f3	0	
	f4	0	
	f5	0	
	f6	0	
	f7	0	
	fps	0	
	cpsr	2164260896	






08011000:   push {r7, lr}
08011002:   sub sp, #16
08011004:   add r7, sp, #0
08011006:   str r0, [r7, #4]
08011008:   movw r3, #3312  ; 0xcf0
0801100c:   movt r3, #8192  ; 0x2000
08011010:   ldr r3, [r3, #12]
08011012:   str r3, [r7, #12]
08011014:   movw r0, #4256  ; 0x10a0
08011018:   movt r0, #2049  ; 0x801
0801101c:   bl 0x8003690 <print>
08011020:   ldr r3, [r7, #4]
08011022:   mov r0, r3
08011024:   bl 0x80035d0 <printn>
08011028:   movw r0, #4264  ; 0x10a8
0801102c:   movt r0, #2049  ; 0x801
08011030:   bl 0x8003690 <print>
08011034:   ldr r0, [r7, #12]
08011036:   bl 0x80035d0 <printn>
0801103a:   bl 0x80036d0 <printnl>
0801103e:   add.w r7, r7, #16
08011042:   mov sp, r7
08011044:   pop {r7, pc}
08011046:   nop 
08011048:   movs r0, r0
0801104a:   movs r0, r0
0801104c:   movs r0, r0
0801104e:   movs r0, r0
08011050:   push {r7, lr}
08011052:   sub sp, #16
08011054:   add r7, sp, #0
08011056:   str r0, [r7, #4]
08011058:   mov.w r3, #4294967295
0801105c:   str r3, [r7, #12]
0801105e:   movw r0, #4268  ; 0x10ac
08011062:   movt r0, #2049  ; 0x801
08011066:   bl 0x8003690 <print>
0801106a:   bl 0x80036d0 <printnl>
0801106e:   mov.w r3, #2048 ; 0x800
08011072:   movt r3, #16385 ; 0x4001
08011076:   mov.w r2, #2048 ; 0x800
0801107a:   movt r2, #16385 ; 0x4001
0801107e:   ldr r2, [r2, #12]
08011080:   eor.w r2, r2, #4
08011084:   str r2, [r3, #12]
08011086:   ldr r3, [r7, #12]
08011088:   add.w r3, r3, #4294967295
0801108c:   str r3, [r7, #12]
0801108e:   ldr r3, [r7, #12]
08011090:   mov r0, r3
08011092:   bl 0x8011000
08011096:   mov.w r0, #1000 ; 0x3e8
0801109a:   bl 0x8000eb0 <chThdSleep>
0801109e:   b.n 0x801106e
080110a0:   str r1, [r0, r1]
080110a2:   adds r2, #80    ; 0x50
080110a4:   movs r0, #58    ; 0x3a
080110a6:   movs r0, r0
080110a8:   cmp r5, #32
080110aa:   movs r0, r4
080110ac:   str r0, [r1, #84]       ; 0x54
080110ae:   ldr r4, [r5, #68]       ; 0x44
080110b0:   movs r0, #111   ; 0x6f
080110b2:   strb r6, [r4, #9]
080110b4:   ldr r7, [r5, #84]       ; 0x54
080110b6:   asrs r0, r4
080110b8:   str r0, [r2, r1]
080110ba:   movs r2, r6
080110bc:   str r0, [r1, #84]       ; 0x54
080110be:   ldr r4, [r5, #68]       ; 0x44
080110c0:   movs r0, #111   ; 0x6f
080110c2:   strb r6, [r4, #9]
080110c4:   ldr r7, [r5, #84]       ; 0x54
080110c6:   asrs r0, r4
080110c8:   str r0, [r2, r1]
080110ca:   movs r3, r6
080110cc:   lsrs r0, r6, #19
080110ce:   movs r0, #0
080110d0:   movs r0, r0
080110d2:   movs r0, r0
080110d4:   movs r0, r0
080110d6:   movs r0, r0
080110d8:   movs r0, r0
080110da:   movs r0, r0
