section .text

%macro pushState 0
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
%endmacro

GLOBAL syscall

syscall:
    pushState
    mov     rax, rcx
    mov     rcx, rdx    ; pongo rcx
    mov     rdx, rax    ; pongo rdx
    mov     rax, rdi    ; pongo rax
    mov     rbx, rsi    ; pongo rbx

    int     80h
    popState
    ret


