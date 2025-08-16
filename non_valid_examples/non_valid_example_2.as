; Non-valid example 2 - Directive and operation errors (no macros)

; Error 201: Line too long (over 80 characters)
MAIN: mov #123456789012345678901234567890123456789012345678901234567890, r1

; Error 231: .data has no parameters
.data

; Error 225: .data has illegal comma following the directive name
.data, 1, 2, 3

; Error 228: .data has multiple consecutive commas
.data 1,, 2, 3

; Error 229: .data expects integer after last comma
.data 1, 2, 3,

; Error 233: .string has no parameter
.string

; Error 232: .string expects one string parameter in quotes
.string hello world

; Error 234: .entry and .extern must have label declaration
.entry

; Error 235: .entry and .extern allow one label at a time
.extern LABEL1 LABEL2

; Error 244: Operation has illegal comma after operation name
mov, r1, r2

; Error 241: Operation has missing operand
add r1

; Error 245: Operation has extraneous text (only two operands required)
sub r1, r2, r3, r4

; Error 246: Operation has multiple consecutive commas
cmp r1,, r2

; Error 247: Operation has missing comma
inc r1 r2

; Error 258: Invalid register for register addressing method
mov r8, r1

; Error 255: Invalid operand for immediate addressing method
add #abc, r2
