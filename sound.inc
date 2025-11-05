; SONIDOS DEL JUEGO - SOUND.INC
; Usando el PC Speaker (INT 61H y timer)

.CODE

; DELAY CON LOOP (más compatible que INT 15H)
DELAY_LOOP PROC
    PUSH CX
    PUSH AX
DELAY_OUTER:
    MOV AX, 0FFFFH
DELAY_INNER:
    DEC AX
    JNZ DELAY_INNER
    LOOP DELAY_OUTER
    POP AX
    POP CX
    RET
DELAY_LOOP ENDP

; REPRODUCIR SONIDO DE RECOLECCIÓN

SONIDO_RECOLECTAR PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
    ; Beep corto y agudo
    MOV AL, 182         ; Preparar el speaker
    OUT 43H, AL         ; Enviar comando al timer
    
    MOV AX, 1193        ; Frecuencia aproximada 1000 Hz
    OUT 42H, AL         ; Enviar byte bajo
    MOV AL, AH
    OUT 42H, AL         ; Enviar byte alto
    
    IN AL, 61H          ; Obtener estado actual
    OR AL, 03H          ; Activar speaker
    OUT 61H, AL
    
    ; Esperar corto
    MOV CX, 2
    CALL DELAY_LOOP
    
    ; Apagar speaker
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    POP DX
    POP CX
    POP AX
    RET
SONIDO_RECOLECTAR ENDP


; REPRODUCIR SONIDO DE VICTORIA

SONIDO_VICTORIA PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
LOOP_MUSICA_VICTORIA:
    ; Do Mayor (C5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2280
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Mi (E5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1810
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Do Alto (C6) - primer clímax
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1140
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 12
    CALL DELAY_LOOP
    
    ; Si (B5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1277
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; La (A5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1355
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Do Alto (C6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1140
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 10
    CALL DELAY_LOOP
    
    ; Pausa dramática
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    MOV CX, 3
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Si (B5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1277
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Re (D6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1015
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Sol Alto (G6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 760
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 20
    CALL DELAY_LOOP
    
    ; Retumbar final
    ; Do bajo (C5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2280
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 25
    CALL DELAY_LOOP
    
    ; Pausa antes de repetir
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    MOV CX, 4
    CALL DELAY_LOOP
    
    ; Verificar si hay tecla presionada (para salir del loop)
    MOV AH, 01H
    INT 16H
    JNZ SALIR_LOOP_VICTORIA    ; Si HAY tecla, salir
    JMP LOOP_MUSICA_VICTORIA   ; Si no hay tecla, continuar loop
    
SALIR_LOOP_VICTORIA:
    ; Limpiar buffer y salir
    MOV AH, 00H
    INT 16H
    
    ; Apagar speaker
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    POP DX
    POP CX
    POP AX
    RET
SONIDO_VICTORIA ENDP


; REPRODUCIR BEEP SIMPLE

SONIDO_BEEP PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1493
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    POP DX
    POP CX
    POP AX
    RET
SONIDO_BEEP ENDP


; MÚSICA DE GAME OVER

MUSICA_GAME_OVER PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
LOOP_MUSICA_GAMEOVER:
    ; Mi alto (E5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1810
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 4
    CALL DELAY_LOOP
    
    ; Re# (D#5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1917
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 4
    CALL DELAY_LOOP
    
    ; La (A4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2711
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP
    
    ; Sol (G4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3043
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP
    
    ; Fa (F4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3405
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP
    
    ; Mi (E4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3619
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 10
    CALL DELAY_LOOP
    
    ; Re (D4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 4063
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Mi (E4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3619
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Fa (F4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3405
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Mi (E4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3619
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP
    
    ; Pausa dramática
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP

    ; Do grave (C3)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 9121
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 25
    CALL DELAY_LOOP
    
    ; Pausa antes de repetir
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    MOV CX, 4
    CALL DELAY_LOOP
    
    ; Verificar si hay tecla presionada (para salir del loop)
    MOV AH, 01H
    INT 16H
    JNZ SALIR_LOOP_GAMEOVER    ; Si HAY tecla, salir
    JMP LOOP_MUSICA_GAMEOVER   ; Si no hay tecla, continuar loop
    
SALIR_LOOP_GAMEOVER:
    ; Limpiar buffer y salir
    MOV AH, 00H
    INT 16H
    
    ; Apagar speaker
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    POP DX
    POP CX
    POP AX
    RET
MUSICA_GAME_OVER ENDP


; MÚSICA DEL MENÚ DE INICIO

MUSICA_MENU_INICIO PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
    ; Loop infinito de música
LOOP_MUSICA_MENU:
    ; Do (C5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2280
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Mi (E5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1810
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Do Alto (C6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1140
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP

    ; Si (B5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1277
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Mi (E5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1810
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP
 
    ; La (A5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1355
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Fa# (F#5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1607
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Re (D5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2031
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP

    ; Do Alto (C6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1140
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Si (B5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1277
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; La (A5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1355
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 6
    CALL DELAY_LOOP
    
    ; Sol (G5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 10
    CALL DELAY_LOOP

    ; Fa (F5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1703
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; La (A5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1355
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Do Alto (C6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1140
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Fa Alto (F6)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 851
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 8
    CALL DELAY_LOOP

    ; Mi (E5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1810
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Do (C5)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2280
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Sol grave (G4)
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 3043
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 5
    CALL DELAY_LOOP
    
    ; Do (C5) - nota final larga
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2280
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 10
    CALL DELAY_LOOP
    
    ; Pausa antes de repetir
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    MOV CX, 4
    CALL DELAY_LOOP
    
    ; Verificar si hay tecla presionada (para salir del loop)
    MOV AH, 01H
    INT 16H
    JNZ SALIR_LOOP_MENU    ; Si HAY tecla, salir
    JMP LOOP_MUSICA_MENU   ; Si no hay tecla, continuar loop
    
SALIR_LOOP_MENU:
    ; Limpiar buffer y salir
    MOV AH, 00H
    INT 16H
    
    ; Apagar speaker
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    POP DX
    POP CX
    POP AX
    RET
MUSICA_MENU_INICIO ENDP


