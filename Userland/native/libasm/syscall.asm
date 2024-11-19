section .note.GNU-stack
section .text
GLOBAL sys_hlt
GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_set_text_color
GLOBAL sys_time
GLOBAL sys_sleep
GLOBAL sys_sound
GLOBAL sys_nosound
GLOBAL sys_clear
GLOBAL sys_put_pixel
GLOBAL sys_ticks_elapsed
GLOBAL sys_seconds_elapsed
GLOBAL sys_new_size
GLOBAL sys_get_window_size
GLOBAL sys_get_registers
GLOBAL sys_create_process
GLOBAL sys_create_process_fd
GLOBAL sys_kill_process
GLOBAL sys_get_process_status
GLOBAL sys_get_pid
GLOBAL sys_get_processes_info
GLOBAL sys_change_priority
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_yield
GLOBAL sys_wait_pid
GLOBAL sys_set_fd
GLOBAL sys_exit
GLOBAL sys_get_exit_code
GLOBAL sys_memory_alloc
GLOBAL sys_memory_free
GLOBAL sys_memory_info
GLOBAL sys_sem_open
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL sys_sem_close
GLOBAL sys_create_pipe
GLOBAL sys_request_pipe
GLOBAL sys_close_pipe
GLOBAL sys_set_foreground
GLOBAL sys_ticks_sleep
GLOBAL sys_set_stdin_options
GLOBAL sys_openMVar
GLOBAL sys_putMVar
GLOBAL sys_takeMVar

%macro Syscall 1
	push    rbp 
    mov     rbp, rsp
    push    rbx
    
	; INPUT  id  rdi  rsi  rdx  rcx  r8  r9
    ; SYSCAL rax rbx  rcx  rdx  r8   r9   
    mov     r9, r8
    mov     r8, rcx
    mov     rcx, rsi
    mov     rbx, rdi
	mov     rax, %1 ; syscall id

	int     80h

	pop     rbx
    leave   
    ret
%endmacro

sys_openMVar:
	Syscall 40

sys_putMVar:
	Syscall 41

sys_takeMVar:
	Syscall 42

sys_hlt:
	Syscall 0

sys_read:
	Syscall 3

sys_write:
	Syscall 4

sys_set_text_color:
	Syscall 14

sys_time:
	Syscall 8

sys_sleep:
	Syscall 2

sys_sound:
	Syscall 1

sys_nosound:
	Syscall 12

sys_clear:
	Syscall 5

sys_put_pixel:
	Syscall 6

sys_ticks_elapsed:
	Syscall 10

sys_seconds_elapsed:
	Syscall 9

sys_new_size:
	Syscall 7

sys_get_window_size:
	Syscall 11

sys_get_registers:
	Syscall 13

sys_create_process:
	mov rcx, std_fds
	Syscall 15

sys_create_process_fd:
	Syscall 15

sys_kill_process:
	Syscall 16

sys_get_process_status:
	Syscall 17

sys_get_pid:
	Syscall 18

sys_get_processes_info:
	Syscall 19

sys_change_priority:
	Syscall 20

sys_block:
	Syscall 21

sys_unblock:
	Syscall 22

sys_yield:
	Syscall 23

sys_wait_pid:
	Syscall 24

sys_set_fd:
	Syscall 25

sys_exit:
	Syscall 26

sys_memory_alloc:
	Syscall 27

sys_memory_free:
	Syscall 28

sys_memory_info:
	Syscall 29

sys_get_exit_code:
	Syscall 30

sys_sem_open:
	Syscall 31

sys_sem_wait:
	Syscall 32

sys_sem_post:
	Syscall 33

sys_sem_close:
	Syscall 34

sys_set_foreground:
	Syscall 35

sys_create_pipe:
	Syscall 36

sys_close_pipe:
	Syscall 37

sys_ticks_sleep:
	Syscall 38

sys_set_stdin_options:
	Syscall 39

SECTION .data
std_fds 	dq 		-1, 1, 2