; Non-valid example 3 - Matrix, operand, and label errors (no macros)

; Error 212: Label name too long (over 30 characters)
VERY_LONG_LABEL_NAME_EXCEEDS_MAX: add r1, r2

; Error 220: Label name must start with alphabetic character
1INVALID_LABEL: mov r3, r4

; Error 216: Opcode name cannot be used as label name
mov: add r1, r2

; Error 217: Register name cannot be used as label name
r1: sub r3, r4

; Error 250: .mat has invalid syntax
.mat [2] 1, 2, 3, 4

; Error 250: .mat missing brackets format
.mat 2 3 1, 2, 3, 4, 5, 6

; Error 252: .mat has more values than allowed
MATRIX: .mat [2][2] 1, 2, 3, 4, 5, 6, 7, 8

; Error 251: Invalid matrix operand syntax
mov MATRIX[r1], r2

; Error 254: Operand uses immediate addressing but has no value
add #, r1

; Error 256: Operand out of range for immediate addressing method
mov #99999, r2

; Error 257: Operand uses register addressing but has no value
sub r, r3

; Error 259: Unrecognized directive (must start with dot)
data 1, 2, 3

; Error 260: Unrecognized command
invalidcommand r1, r2

; Error 261: Unrecognized operand
mov unknown_operand, r1

; Error 262: Reserved word used as operand
add mcro, r2

; Error 240: Operation has extraneous text (no operands required)
rts r1, r2
