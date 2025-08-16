; Example 4: no extern/entry declarations; simple data, string, and loop

 add #10, r1
 lea DATA, r2
 mov *r2, r3
CALC: cmp r3, #0
 sub #1, r3
 inc r1
 prn r1
 jmp CALC
FINISH: clr r1
 mov #-5, r2
 add r2, DATA
 stop
DATA: .data 25, -12, 0
MSG: .string "Hello World"
TEMP: .data 100
