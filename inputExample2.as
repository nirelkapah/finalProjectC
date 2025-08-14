; Second input example file with 2 scenerios of macro defining

mcro A
    inc r0
endmcro

mcro B
    dec r1
    A
endmcro

A
B

