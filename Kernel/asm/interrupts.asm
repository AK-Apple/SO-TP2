
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _int80Handler

GLOBAL saveCurrentRegisters

GLOBAL _exception00Handler
GLOBAL _exception06Handler


EXTERN main
EXTERN getStackBase
EXTERN int80Dispacher
EXTERN irqDispatcher
EXTERN exceptionDispatcher

EXTERN int80_write

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pushAndSaveState 0
	pushState
	mov [updating_regex + 8*0], rax
	mov [updating_regex + 8*1], rbx
	mov [updating_regex + 8*2], rcx
    mov [updating_regex + 8*3], rdx
    mov [updating_regex + 8*4], rsi
    mov [updating_regex + 8*5], rdi
	mov [updating_regex + 8*6], rbp
	mov [updating_regex + 8*8], r8
    mov [updating_regex + 8*9], r9
    mov [updating_regex + 8*10], r10
    mov [updating_regex + 8*11], r11
    mov [updating_regex + 8*12], r12
    mov [updating_regex + 8*13], r13
    mov [updating_regex + 8*14], r14
    mov [updating_regex + 8*15], r15

	; para registros especiales, los busco en el stack
	mov rax, [rsp + 15*8]
	mov [updating_regex + 8*16], rax	; rip
	mov rax, [rsp + 15*8 + 16]
	mov [updating_regex + 8*17], rax	; rflags
	mov rax, [rsp + 15*8 + 24]
	mov [updating_regex + 8*7], rax		; rsp

%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro pushStateWithoutRax 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popStateWithoutRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	pushAndSaveState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro exceptionHandler 1
	pushAndSaveState	; guarda registros en updating_regex

	mov rdi, %1 ; pasaje de parametros
	mov rsi, updating_regex
	call exceptionDispatcher

	call getStackBase
	mov [rsp + 15*8 + 24], rax	; rsp

	mov rax, 0x400000
	mov [rsp + 15*8], rax		; rip

	popState
	iretq

%endmacro

GLOBAL storeRegs

%macro copy_updating_regex 0		; copia updating_regex en regex. Ver diferencia en section bss
    push rsi                ; Save the RSI register (callee-saved)
    push rdi                ; Save the RDI register (callee-saved)

    mov rsi, updating_regex ; Load the source address into RSI
    mov rdi, regex          ; Load the destination address into RDI

    mov rcx, 18             ; Set the counter to 18 (number of quadwords)
.copy_loop:
    mov rax, [rsi]          ; Load a quadword from the source
    mov [rdi], rax          ; Store the quadword into the destination
    add rsi, 8              ; Move to the next quadword in the source
    add rdi, 8              ; Move to the next quadword in the destination
    loop .copy_loop         ; Decrement the counter and loop if not zero

    pop rdi                 ; Restore the RDI register
    pop rsi                 ; Restore the RSI register
%endmacro


storeRegs:
    push rbp
    mov rbp, rsp

	copy_updating_regex

    mov rax, regex

    mov rsp, rbp
    pop rbp
    ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:			; activa interrupciones enmascarables
	sti			; ver power
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5


;Zero Division Exception
_exception00Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception06Handler:
	exceptionHandler 1

;int 80 Handler
_int80Handler:
	pushStateWithoutRax

	mov 	rdi, rax ; pasaje de parametro 1
	mov 	rsi, rbx ; pasaje de parametro 2

	mov     rax, rdx ;
	mov     rdx, rcx ; pasaje de parametro 3
	mov     rcx, rax ; pasaje de parametro 4

	call int80Dispacher; llamada a syscall

	; no hacemos lo de out 20h porque es otro tipo de interrupt

	popStateWithoutRax
	iretq

haltcpu:
	cli
	hlt
	ret


SECTION .bss
	aux resq 1
	regex resq 18 			;reserva espacio para 18 qwords (cada registro para mostrarlos en las excepciones)
	updating_regex resq 18	;como regex, pero se actualiza cada tick