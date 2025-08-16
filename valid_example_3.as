; Third example file 

.extern CHECK
.extern GLOBAL
.entry MAIN
MAIN: lea ARRAY, r0
 mov #3, r1
LOOP: cmp r1, #0
 mov *r0, r2
 add #7, r2
 prn r2
 inc r0
 dec r1
 bne LOOP
 jsr CHECK
END: mov GLOBAL, r3
.entry TEXT
 sub #20, r3
 cmp r3, #-10
 bne SKIP
 clr r3
SKIP: mov r3, RESULT
 stop
ARRAY: .data 5, 10, 15
TEXT: .string "Test"
RESULT: .data 0
COUNT: .data 42
