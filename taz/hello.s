con
	rx_pin = 63
	tx_pin = 62
	clock_freq = 80_000_000
	baud_rate = 115_200
	lr = $1f6
	'init_stack_ptr = 64*1024 - 32*4
	init_stack_ptr = 384*1024

dat
	orgh	0

'*******************************************************************************
'  COG Code
'*******************************************************************************
	org

start	jmp 	#prefix_setup
p2start_address
	long	_p2start
	long	0
	long	0
	long	0
        long    80000000 ' p2clkfreq
        long    19146744 ' p2clkconfig
  	long	115200   ' baudrate
r0	long	0
r1	long	0
r2	long	0
r3	long	0
r4	long	0
r5	long	0
r6	long	0
r7	long	0
r8	long	0
r9	long	0
r10	long	0
r11	long	0
r12	long	0
r13	long	0
r14	long	0
sp	long	0

temp	long    0
temp1	long	0
temp2	long	0

__DIVSI	mov	temp, #0
	abs	r0, r0 wc
 if_c	mov	temp, #1
	abs	r1, r1 wc
 if_c	xor	temp, #1
	call	#__UDIVSI
	cmp	temp, #0 wz
 if_nz	neg	r0, r0
	ret

'__LONGFILL
'        wrfast  #0, r0
'        rep     #1, r2
'        wflong  r1
'        ret

__LONGFILL
        mov     __LONG1, r1
        shr     __LONG1, #9
        or      __LONG1, ##$ff800000
        setd    __LONG2, r1
        sub     r2, #1
        setq    r2
__LONG1 augd    #0
__LONG2 wrlong  #0, r0
        ret

__MEMCPY
        rdbyte  r3, r1
        wrbyte  r3, r0
        add     r0, #1
        add     r1, #1
        djnz    r2, #__MEMCPY
        ret

__has_cordic
	long	0

' Used CORDIC multiply if available
__MULSI cmp	__has_cordic, #0 wz
 if_z   jmp	#__MULSI0
	qmul	r0, r1
	getqx	r0
	getqy	r1
	ret
' else, do shift and add method
__MULSI0
        mov     temp1,#0
        mov     temp2,#32
        shr     r0,#1        wc
__MULSI1
 if_c   add     temp1,r1     wc
        rcr     temp1,#1     wc
        rcr     r0,#1        wc
        djnz    temp2,#__MULSI1
        mov     r1, temp1
        ret

' Used CORDIC divide if available
__UDIVSI
	cmp	__has_cordic, #0 wz
 if_z   jmp	#__UDIVSI0
	qdiv	r0, r1
	getqx	r0
	getqy	r1
	ret
' else, do shift and subtract method
__UDIVSI0
        mov     temp2,#32
        mov     temp1,#0
        cmp     r1, #0       wz
 if_nz  jmp     #__UDIVSI1
        mov     r0, #0
        ret
__UDIVSI1
        shr     r1,#1        wcz
        rcr     temp1,#1
 if_nz  djnz    temp2,#__UDIVSI1
__UDIVSI2
        cmpsub  r0,temp1     wc
        rcl     r1,#1
        shr     temp1,#1
        djnz    temp2,#__UDIVSI2
        mov     temp1, r1
	mov	r1, r0
	mov	r0, temp1
        ret

__CMPSWAPSI
	mov	temp1,r0	'' save value to set
	rdlong	r0,r2		'' fetch original value
	cmp	r0,r1 wz	'' compare with desired original value
 if_z   wrlong  temp1,r2	'' if match, save new value
	ret

prefix_setup
        mov     save_r0, r0
        mov     save_r1, r1
        cmp     ptra, #0 wz     ' Check if ptra is zero
 if_z   jmp     #prefix_setup1  ' If zero set sp to init_stack_ptr
        mov     sp, ptra
        rdlong  p2start_address, sp
        add     sp, #4
        rdlong  r0, sp
        add     sp, #4
        jmp     #prefix_setup2
prefix_setup1
	mov	sp, ##init_stack_ptr
	drvh    #tx_pin
prefix_setup2
	mov	r1, #1
        qmul    r1, r1
        getqx   __has_cordic
        mov     r0, save_r0
        mov     r1, save_r1
	calld	lr, p2start_address
	cogid	r1
        mov     r2, #0
        wrlong  r2, r2
	cogstop	r1
save_r0 long    0
save_r1 long    0

'*******************************************************************************
'  Program HUB Code
'*******************************************************************************
	orgh	$400


' void main(void)
        .global _main
_main   
        sub     sp, #4
        wrlong  lr, sp

' {
'     int i;
' 
'     for (i = 1; i <= 10; i++)

        sub     sp, #4
        mov     r0, #1
        mov     r1, #0
        add     r1, sp
        wrlong  r0, r1
label0001
        mov     r0, #0
        add     r0, sp
        rdlong  r0, r0
        mov     r1, #10
        cmps    r1, r0 wc
 if_nc  mov     r0, #1
 if_c   mov     r0, #0
        cmp     r0, #0  wz
 if_nz  jmp     #label0003
        jmp     #label0004
label0002
        mov     r2, #0
        add     r2, sp
        rdlong  r0, r2
        add     r0, #1
        wrlong  r0, r2

'     {
        jmp     #label0001
label0003

'         msleep(500);
        mov     r0, #500
        calld   lr, #_msleep

'         printf("Hello World - %d\n", i);
        calld   lr, #label0006
        byte    "Hello World - %d", 10, 0
        alignl
label0006
        mov     r0, lr
        mov     r1, #0
        add     r1, sp
        rdlong  r1, r1
        sub     sp, #4
        wrlong  r1, sp
        sub     sp, #4
        wrlong  r0, sp
        calld   lr, #_printf
        add     sp, #8

'     }
'     printf("Goodbye\n");
        jmp     #label0002
label0004
        calld   lr, #label0008
        byte    "Goodbye", 10, 0
        alignl
label0008
        mov     r0, lr
        sub     sp, #4
        wrlong  r0, sp
        calld   lr, #_printf
        add     sp, #4

' }
        add     sp, #4
        rdlong  lr, sp
        add     sp, #4
        jmp     lr

' EOF
