GLOBAL runInvalidOpcodeException
GLOBAL runZeroDivisionException

section .text

runInvalidOpcodeException:
    db 0xFF, 0xFF
    ret

runZeroDivisionException:
    xor rax, rax   ; rax=0
    xor rdx, rdx   ; rdx=0
    div rdx        ; rax/rdx
    ret
