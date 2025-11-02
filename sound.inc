; SONIDOS DEL JUEGO - SOUND.INC
; Usando el PC Speaker (INT 61H y timer)

.CODE

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
    MOV CX, 1
    MOV DX, 0
    MOV AH, 86H
    INT 15H
    
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
    
    ; Nota 1
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 2280
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    IN AL, 61H
    OR AL, 03H
    OUT 61H, AL
    
    MOV CX, 1
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    ; Nota 2
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1810
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 1
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    ; Nota 3
    MOV AL, 182
    OUT 43H, AL
    MOV AX, 1521
    OUT 42H, AL
    MOV AL, AH
    OUT 42H, AL
    
    MOV CX, 2
    MOV DX, 0
    MOV AH, 86H
    INT 15H
    
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
    
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    IN AL, 61H
    AND AL, 0FCH
    OUT 61H, AL
    
    POP DX
    POP CX
    POP AX
    RET
SONIDO_BEEP ENDP


