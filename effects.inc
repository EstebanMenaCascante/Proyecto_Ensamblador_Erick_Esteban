; EFECTOS VISUALES - EFFECTS.INC

.DATA
    EFECTO_X_POS DW 0
    EFECTO_Y_POS DW 0

.CODE

; INICIAR EFECTO DE RECOLECCIÓN
; BX = X, DX = Y

EFECTO_RECOLECCION PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
    ; Guardar posición
    MOV EFECTO_X_POS, BX
    MOV EFECTO_Y_POS, DX
    
    ; Dibujar efecto simple
    CALL DIBUJAR_EFECTO_SIMPLE
    
    POP DX
    POP CX
    POP AX
    RET
EFECTO_RECOLECCION ENDP


; DIBUJAR EFECTO SIMPLE (4 puntos alrededor)

DIBUJAR_EFECTO_SIMPLE PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
    MOV CX, EFECTO_X_POS
    ADD CX, 4
    MOV DX, EFECTO_Y_POS
    ADD DX, 4
    
    ; Punto arriba
    PUSH CX
    PUSH DX
    SUB DX, 10
    MOV AL, 14
    CALL ESCRIBIR_PIXEL
    POP DX
    POP CX
    
    ; Punto abajo
    PUSH CX
    PUSH DX
    ADD DX, 10
    MOV AL, 14
    CALL ESCRIBIR_PIXEL
    POP DX
    POP CX
    
    ; Punto izquierda
    PUSH CX
    PUSH DX
    SUB CX, 10
    MOV AL, 14
    CALL ESCRIBIR_PIXEL
    POP DX
    POP CX
    
    ; Punto derecha
    PUSH CX
    PUSH DX
    ADD CX, 10
    MOV AL, 14
    CALL ESCRIBIR_PIXEL
    POP DX
    POP CX
    
    POP DX
    POP CX
    POP AX
    RET
DIBUJAR_EFECTO_SIMPLE ENDP

