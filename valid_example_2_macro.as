; Second example file with Macro defining

mcro A
    inc r0
endmcro

mcro B
    dec r1
    A
endmcro

A
B

