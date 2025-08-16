; Example 2: two macros (B invokes A)

mcro A
    inc r0
endmcro

mcro B
    dec r1
    A
endmcro

A
B

