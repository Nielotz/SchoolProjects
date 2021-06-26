.globl leibniz

.data
    cw: .word 0
.text

.type leibniz, @function


# long double leibniz(unsigned long x);
leibniz:
    finit
    fstcw cw
    andw $0xFFFF, cw  # 11
    fldcw cw

    # Calculate loop counter.
    mov %rdi, %rax
    mov $2, %rdi
    xor %rdx, %rdx  # Clear rdx.
    div %rdi  # %rdx = remainder = whether do additional operation
    # %rax = number of double additions
    
    # Prepare ST:
    #   0(temp), 1(const=2), 2(const=1), 3(result), 4(counter).
    fld1  # Stack: 1.
    fld1  # Stack: 1, 1.
    fld1  # Stack: 1, 1, 1.
    fld1  # Stack: 1, 1, 1, 1.
    fld1  # Stack: 1, 1, 1, 1, 1.
    fadd %ST(0), %ST(1)  # Stack: 1, 2, 1, 1, 1.

do_loop:
    fadd %ST(1)  # Add 2 to temp counter.
    fst %ST(4)  # Update main counter.
    fdivr %ST(2), %ST(0)  # ST(0) = div 1(ST(2)) / temp_counter(ST(0)).

    fsubrp %st(0), %st(3)  # Sub from result.
    fld %st(3)  # Copy counter to ST(0). 
    
    # # #
    
    fadd %ST(1)  # Add 2 to temp counter.
    fst %ST(4)  # Update main counter.
    fdivr %ST(2), %ST(0)  # ST(0) = div 1(ST(2)) / temp_counter(ST(0)).

    faddp %st(0), %st(3)  # Add to result.
    fld %st(3)   # Copy counter to ST(0). 

    dec %rax
    jnz do_loop

    or %rdx, %rdx
    jz return_result
    fadd %ST(1)  # Add 2 to temp counter.
    fst %ST(4)  # Update main counter.
    fdivr %ST(2), %ST(0)  # ST(0) = div 1(ST(2)) / temp_counter(ST(0)).

    fsubrp %st(0), %st(3)  # Sub from result.
    
return_result:
    faddp %ST(0)
    faddp %ST(0)
    faddp %ST(0)

    ret
