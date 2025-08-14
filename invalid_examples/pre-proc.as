      Thisssssss lliiiinnneeeee iss wayyyyyyyy tooooo longgggggggggg      TTTTThissssss lliiiinnneeeeeeee isssss wayyyyyyyy tooooo lllllongggggggggggg!!!!!

;Invalid macro declaration, no name was defined
mcro
endmcro

;Invalid macro declaration, "mcro" command with extraneous text
e mcro macro_name
endmcro

;Invalid macro declaration, an opcode name cannot be used as a macro name
mcro mov
endmcro

;Invalid macro declaration, a register name cannot be used as a macro name
mcro r0
endmcro

;Invalid macro declaration, an instruction name cannot be used as a macro name
mcro .data
endmcro

;Invalid macro declaration, macro should be assigned with one name
mcro macro1 macro2
endmcro

;Invalid macro declaration, this macro name is already in use
mcro mymacro
    mov r1, r2
endmcro

mcro mymacro
    add r1, r3
endmcro

;Invalid macro declaration, "endmcro" command with extraneous text
mcro anothermacro
    sub r2, r4
endmcro extra_text_here

;Invalid macro declaration, macro name is too long, 31 characters max
mcro thisMacroNameIsWayTooLongAndInvalid
    clr r5
endmcro

;Invalid macro declaration, empty macro content
mcro emptyMacro

endmcro

;Invalid macro declaration, "mcro" is a reserved word
mcro mcro
    inc r1
endmcro

;Invalid macro declaration, "endmcro" is a reserved word
mcro endmcro
    dec r2
endmcro
