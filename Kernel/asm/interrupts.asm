
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

GLOBAL _exception00Handler
GLOBAL _exception06Handler


EXTERN exit
EXTERN schedule
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
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

; ------------ Sección SO -----------





%macro change_process 0
	pushState

	mov rdi, 0
	call irqDispatcher

	mov rdi, rsp
	call schedule
	mov rsp, rax

	mov al, 20h ; fin de interrupcion
	out 20h, al

	popState

	iretq
%endmacro

; ------------ Fin Sección SO -----------

%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametros
	mov rsi, rsp
	call exceptionDispatcher

	popState
	
	mov rdi, -2
	mov QWORD[rsp], exit

	iretq

%endmacro

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
	change_process

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

	;rdi, rsi, rdx, rcx, r8 y r9
	mov 	rdi, rax
	mov 	rsi, rbx
	xchg 	rdx, rcx
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