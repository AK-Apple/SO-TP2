section .note.GNU-stack
GLOBAL cpuVendor
GLOBAL force_timer_tick
GLOBAL setup_kernel_restart
EXTERN main
EXTERN get_stack_base

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

; lo agarra time.h
GLOBAL getRTC
getRTC:
	mov rax, rdi	; recibimos por parámetro.
	out 70h, al		; 70h entrada para la informacion que quiero en 71h.
	mov rax, 0
	in al, 71h		; en al se deposita lo pedido por 70h, presente en 71h.
	ret

; lo agarra keyboard.h
GLOBAL get_key
get_key:
        push 	rbp
        mov     rbp, rsp

        mov		rax, 0
        in      al, 64h         ; indica si hay key
        and		rax, 01h

        cmp		rax, 01h
        jne		get_key_end		;rax=0

        in		al, 60h         ; 60h tiene la key id

    get_key_end:
        leave
        ret


; implementar inb y outb son requeridas para las funciones de sound.c sacadas de osDev
GLOBAL inb
inb:
    ; Inputs:
    ; rdi - port address
    ; Outputs:
    ; rax - data read from the port (only the lower 8 bits will be used)
    mov dx, di    ; Move port address from rdi to dx
    in al, dx     ; Read a byte from the port specified in DX into AL
    movzx eax, al ; Zero-extend AL to EAX (clear upper bits)
    ret           ; Return from the function

GLOBAL outb
outb:
    ; Inputs:
    ; rdi - port address
    ; rsi - data to send to the port
    mov dx, di    ; Move port address from rdi to dx
    mov al, sil   ; Move data from rsi to al (sil is the lower 8 bits of rsi)
    out dx, al    ; Send the byte in AL to the port specified in DX
    ret           ; Return from the function


force_timer_tick:
    int 0x20
    ret

; -- void acquire(uint8_t * lock);
GLOBAL acquire

acquire:
    mov al, 0
.retry:
    xchg BYTE [rdi], al
    test al, al
    jz .retry
    ret


; -- void release(uint8_t * lock);
GLOBAL release

release:
    mov BYTE [rdi], 1
    ret


setup_kernel_restart:
	call get_stack_base
	mov rsp, rax
	call main

;=============================================

section .data
    date db "00:00:00", 0
    test_int_80h_data db 0x63

section .bss
    placeholder resb 2