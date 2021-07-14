.globl main

.data 
    outstr: .ascii  "NWD(%llu, %llu) = %llu\n\0"
    error_to_few_arguments: .ascii  "To few arguments!\n\0"
    error_to_many_arguments: .ascii  "To many arguments!\n\0"
    error_not_int_argument: .ascii  "Arguments have to be ints!\n\0"
    error_invalid_argument: .ascii  "Argument is invalid!\n\0"
.text

main:
    mov $5, %r14  

    call parse_arguments
    jz make_ret_with_err
    
    # Calculate GCD of %rax, %rdx and save in %rax.
    mov %rax, %r8  # Make a copy of a.
    mov %rdx, %r9  # Make a copy of b.

    mov %rdx, %r11  # Make a copy of b.
    # %r8 and %r9 are read-only
_while_start:
    # while b != 0:
    or %r11, %r11 
    jz _while_end
    
    # a, b = b, a % b
    
    # Save reminder to %rdx.
    xor %rdx, %rdx  # Clean for reminder.
    div %r11  # Now in %rdx is future b.

    mov %r11, %rax  # Save new a.
    mov %rdx, %r11  # Save new b.

    jmp _while_start
_while_end:

    mov $outstr, %rdi  # 1st arg
    mov %r8, %rsi  # 2nd arg
    mov %r9, %rdx  # 3rd arg, second return already in %rdx
    mov %rax, %rcx  # NWD result
    xor %eax, %eax  # Use 0 vectors.
    call printf

    xor %eax, %eax
    ret

# Convert str repr of numbers to actual numbers.
# Exit when arguments do not satisfy requirements:
# 2 arguments, both numbers
# INPUT:
#   %rdi - 1st argument 
#   %rsi - 2nd argument
# RETURN:
#   %rax - 1st converted argument 
#   %rdx - 2nd converted argument 
#   ZF set to 1 when error and print error message
#   
parse_arguments:
    # Test how many arguments passed to program.
    sub $3, %edi
    jnz _invalid_number_of_arguments

    # When number of arguments is ok.
    # Convert first argument.
    mov 16(%rsi), %r12  # Save 2nd argument.
    mov 8(%rsi), %rdi  # move first argument.
    xor %rax, %rax  # Use 0 vectors.
    call atoll
    or %rax, %rax
    jz _cannot_convert_or_zero

    # Convert second argument.
    mov %r12, %rdi  # Retrieve second argument as first to the atoi.
    mov %rax, %r12  # Save first converted argument.
    xor %rax, %rax  # Use 0 vectors.
    call atoll
    or %rax, %rax
    jz _cannot_convert_or_zero

    # Prepare returns.
    mov %r12, %rdx
    xchg %rdx, %rax
    ret


_invalid_number_of_arguments:
    js _to_few
    # To many.
    mov $error_to_many_arguments, %rdi
    jmp print_err

_to_few:
    mov $error_to_few_arguments, %rdi
    jmp print_err

_cannot_convert_or_zero:
    mov $error_invalid_argument, %rdi
    jmp print_err


# INPUT:
#   %rdi - message address
# Zero eax, and set ZF to 1.
print_err:
    mov $1, %eax
    call printf
    xor %eax, %eax # Set ZF to 1.
    ret

make_ret_with_err:
    mov $1, %eax
    ret
