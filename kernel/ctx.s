# PingPongOS - PingPong Operating System
# © Prof. Carlos A. Maziero, DINF UFPR
# Versão 2.1 -- 06/2026

# ATENÇÃO: ESTE ARQUIVO NÃO DEVE SER ALTERADO;
# ALTERAÇÕES SERÃO DESCARTADAS NA CORREÇÃO.

# Rotinas de manipulação de contextos no userspace, detalhes em ctx.h.
# Escrito em assembly com sintaxe AT&T (cmd source, dest)

#-----------------------------------------------------------------------

# register offsets in the ctx_t struct (defined in ctx.h)
.equ OFF_RIP, 8*0   # registers
.equ OFF_RSP, 8*1
.equ OFF_RBP, 8*2
.equ OFF_RDI, 8*3
.equ OFF_RSI, 8*4
.equ OFF_RAX, 8*5
.equ OFF_RBX, 8*6
.equ OFF_RCX, 8*7
.equ OFF_RDX, 8*8
.equ OFF_R8,  8*9
.equ OFF_R9,  8*10
.equ OFF_R10, 8*11
.equ OFF_R11, 8*12
.equ OFF_R12, 8*13
.equ OFF_R13, 8*14
.equ OFF_R14, 8*15
.equ OFF_R15, 8*16
.equ OFF_SSA, 8*17  # stack start address
.equ OFF_SSZ, 8*18  # stack size in bytes

#-----------------------------------------------------------------------

# saves the current context in ctx1 and transfers control to ctx2
.type ctx_switch, @function
.global ctx_switch

ctx_switch:
    # Arguments:
    # rdi: *ctx1
    # rsi: *ctx2

    # if ctx1 == ctx2, goto error
    cmp %rdi, %rsi
    jz ctx_switch_error

    # if ctx1 == NULL, do not save current context
    cmp $0, %rdi
    jz ctx_switch_load_ctx2

    # save CPU registers in ctx1 struct
    mov %r8,  OFF_R8(%rdi)    # ctx->r8 = %r8
    mov %r9,  OFF_R9(%rdi)    # ...
    mov %r10, OFF_R10(%rdi)
    mov %r11, OFF_R11(%rdi)
    mov %r12, OFF_R12(%rdi)
    mov %r13, OFF_R13(%rdi)
    mov %r14, OFF_R14(%rdi)
    mov %r15, OFF_R15(%rdi)
    mov %rax, OFF_RAX(%rdi)
    mov %rbx, OFF_RBX(%rdi)
    mov %rcx, OFF_RCX(%rdi)
    mov %rdx, OFF_RDX(%rdi)
    mov %rdi, OFF_RDI(%rdi)
    mov %rsi, OFF_RSI(%rdi)
    mov %rbp, OFF_RBP(%rdi)

    # Current stack state:
    #  :                     :
    #  :                     :
    #  +---------------------+
    #  | caller stack bottom | <- stack pointer before calling ctx_switch
    #  +---------------------+
    #  | ret addr (8 bytes)  | <- stack pointer now (%rsp)
    #  +---------------------+

    # save the return address in ctx1->rip
    mov (%rsp), %rcx          # %rcx = *(%rsp)
    mov %rcx, OFF_RIP(%rdi)   # *(%rdi + OFF_RIP) = %rcx

    # save the previous stack pointer in ctx1->rsp
    lea 8(%rsp), %rcx         # %rcx = *(%rsp + 8)
    mov %rcx, OFF_RSP(%rdi)   # *(%rdi + OFF_RSP) = %rcx

ctx_switch_load_ctx2:
    # if ctx2 == NULL, do not load ctx2 into the CPU
    cmp $0, %rsi
    jz ctx_switch_noerror

    # load CPU registers from ctx2 struct, except %rsi because
    # its current value is being used
    mov OFF_R8(%rsi),  %r8    # %r8 = *(%rsi + OFF_R8)
    mov OFF_R9(%rsi),  %r9    # ...
    mov OFF_R10(%rsi), %r10
    mov OFF_R11(%rsi), %r11
    mov OFF_R12(%rsi), %r12
    mov OFF_R13(%rsi), %r13
    mov OFF_R14(%rsi), %r14
    mov OFF_R15(%rsi), %r15
    mov OFF_RAX(%rsi), %rax
    mov OFF_RBX(%rsi), %rbx
    mov OFF_RCX(%rsi), %rcx
    mov OFF_RDX(%rsi), %rdx
    mov OFF_RDI(%rsi), %rdi
    # mov OFF_RSI(%rsi), %rsi  # do this later
    mov OFF_RBP(%rsi), %rbp
    mov OFF_RSP(%rsi), %rsp

    # load the saved RIP and push it in the stack;
    # it will be the return address when calling ret below
    push OFF_RIP(%rsi)

    # now we can load %rsi with value stored in ctx2->rsi
    mov OFF_RSI(%rsi), %rsi

ctx_switch_noerror:
    # no errors, return 0
    xor %eax, %eax
    ret

ctx_switch_error:
    # error, return -1
    movl $-1, %eax
    ret

#-----------------------------------------------------------------------

# set entry point, arg, and stack for the context given as parameter
.type ctx_create, @function
.global ctx_create

ctx_create:
    # Arguments:
    # %rdi: *ctx
    # %rsi: *entry
    # %rdx: *arg
    # %rcx: *stack
    # %r8 : size

    # if ctx == NULL, goto error
    cmp $0, %rdi
    jz ctx_create_error

    # if entry == NULL, goto error
    cmp $0, %rsi
    jz ctx_create_error

    # if stack == NULL, goto error
    cmp $0, %rcx
    jz ctx_create_error

    # if stack is not 16-byte aligned, goto error
    # (last 4 bits of %rcx should be zero)
    test $0x0F, %rcx   # 0x0F = 00001111
    jnz ctx_create_error

    # if stack size < 4K, goto error
    cmp $0x1000, %r8   # 0x1000 = 4096
    jl ctx_create_error

    # save current CPU registers in ctx struct
    mov %r8,  OFF_R8(%rdi)    # ctx->r8 = %r8
    mov %r9,  OFF_R9(%rdi)    # ...
    mov %r10, OFF_R10(%rdi)
    mov %r11, OFF_R11(%rdi)
    mov %r12, OFF_R12(%rdi)
    mov %r13, OFF_R13(%rdi)
    mov %r14, OFF_R14(%rdi)
    mov %r15, OFF_R15(%rdi)
    mov %rax, OFF_RAX(%rdi)
    mov %rbx, OFF_RBX(%rdi)
    mov %rcx, OFF_RCX(%rdi)
    mov %rdx, OFF_RDX(%rdi)
    mov %rdi, OFF_RDI(%rdi)
    mov %rsi, OFF_RSI(%rdi)
    mov %rbp, OFF_RBP(%rdi)

    # save context's stack address and size
    mov %rcx, OFF_SSA(%rdi)       # ctx->stack = stack
    mov %r8,  OFF_SSZ(%rdi)       # ctx->size  = size

    # set context's stack base and pointer; both should point to
    # the top of the stack area provided and should be 16-byte
    # aligned (stack + size - 16)
    add %r8,  %rcx                # stack += size
    sub $0x10, %rcx               # stack -= 16
    mov %rcx, OFF_RBP(%rdi)       # ctx->rbp = stack
    mov %rcx, OFF_RSP(%rdi)       # ctx->rsp = stack

    # ctx->rsp should be decremented by 8 to keep it 16-byte aligned
    # when the ctx_switch function returns, because the ret instruction
    # pops an 8-byte return address from the stack
    subq $0x8, OFF_RSP(%rdi)      # ctx->rsp -= 8

    # set context's entry point (function)
    mov %rsi, OFF_RIP(%rdi)       # ctx->rip = entry

    # set entry point function argument
    mov %rdx, OFF_RDI(%rdi)       # ctx->rdi = arg

    # no errors, return 0
    xor %eax, %eax
    ret

ctx_create_error:
    # errors, return -1
    movl $-1, %eax
    ret

#-----------------------------------------------------------------------s
