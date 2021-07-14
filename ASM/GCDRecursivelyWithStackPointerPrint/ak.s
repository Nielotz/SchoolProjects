.globl GCD
.globl print_call_rsp
.globl print_ret_rsp

.type GCD, @function
.type print_call_rsp, @function
.type print_ret_rsp, @function

.data
    modulo_result_print_message: .string "Result for GCD(%llu, %llu) = %llu\n"
    call_rsp_message: .string "CALL address: %llx deep level: %lld\n"
    ret_rsp_message: .string "RET address: %llx deep level: %lld\n"
    deep_level: .quad -1
    a: .quad 0
    b: .quad 0
.text
    

# Calculates Greatest Common Divider of two numbers.
#    a = %rdi
#    b = %rsi
GCD: 
    sub $8, %rsp  # Realign rsp.

    # Save args into memory.
    mov %rdi, a(%rip)
    mov %rsi, b(%rip)

    # Calculate GCD.
    xor %rax, %rax  # Use 0 vectors
    call gcd_req
    
    # Print result.
    lea modulo_result_print_message(%rip), %rdi  # 1st arg
    mov a(%rip), %rsi  # 2nd arg = a
    mov b(%rip), %rdx  # 3rd arg = b
    mov %rax, %rcx  # 4th arg = a % b
    xor %rax, %rax  # Use 0 vectors
    call printf@plt

    add $8, %rsp  # Realign rsp.

    # Return 0.
    xor %rax, %rax    
    ret

print_call_rsp:
    lea call_rsp_message(%rip), %rdi  # 1st. argument: message.

    # 3rd. argument: deep_level.
    mov deep_level(%rip), %rdx  
    inc %rdx
    mov %rdx, deep_level(%rip)   

_print_rsp:
    lea  8(%rsp), %rsi  # 2nd. argument: stack pointer.
    xor %rax, %rax
    call printf@plt
    ret
print_ret_rsp:
    lea ret_rsp_message(%rip), %rdi  # 1st. argument: message.

    # 3rd. argument: deep_level.
    mov deep_level(%rip), %rdx 
    mov %rdx, %rax
    dec %rax
    mov %rax, deep_level(%rip)  # Update deep_level.

    jmp _print_rsp   


# Calculates recursively Greatest Common Divider of two numbers.
#    a = %rdi
#    b = %rsi
gcd_req:
    # Save registers.
    push %rdi
    push %rsi
    call print_call_rsp
    # Retrieve registers.
    pop %rsi
    pop %rdi

    # When b == 0 -> return a.
    or %rsi, %rsi  # Set ZF.
    jz _return_a
    
    # b != 0 => return gcd_req(b, a % b)
    xor %rdx, %rdx  # Clean %rdx before div.
    mov %rdi, %rax  # Set a as dividend.
    div %rsi  # Divide by b, remainder: a mod b in %rdx.

    mov %rsi, %rdi  # Set b as first param.
    mov %rdx, %rsi  # Set reminder as second param.

    xor %rax, %rax  # Use 0 vectors
    call gcd_req

    # Save register.
    push %rax
    push %rax  # Keep the same amount of push and pop along every print_*_rsp call.
    call print_ret_rsp
    # Retrieve registers.
    pop %rax
    pop %rax  # Keep the same amount of push and pop along every print_*_rsp call.
    
    ret
_return_a:
    push %rdi
    push %rdi  # Keep the same amount of push and pop along every print_*_rsp call.
    call print_ret_rsp
    pop %rax
    pop %rax  # Keep the same amount of push and pop along every print_*_rsp call.

    ret
