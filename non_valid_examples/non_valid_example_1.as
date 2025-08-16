; Non-valid example 1 - Macro errors and common syntax issues

; Error 203: "mcro" command with extraneous text
mcro INVALID_MACRO extra text here
    mov r1, r2
endmcro

; Error 204: Opcode name cannot be used as macro name
mcro mov
    add r1, r2
endmcro

; Error 207: Macro name already in use
mcro DUPLICATE_MACRO
    sub r3, r4
endmcro

mcro DUPLICATE_MACRO
    inc r5
endmcro

; Error 208: "endmcro" command with extraneous text
mcro VALID_MACRO
    prn #5
endmcro extra text

; Error 209: Empty macro content
mcro EMPTY_MACRO
endmcro

; Error 201: Line too long (over 80 characters)
mcro VERY_LONG_MACRO_NAME_THAT_EXCEEDS_THE_MAXIMUM_ALLOWED_LINE_LENGTH_OF_EIGHTY_CHARACTERS
