
; RECURSOS E INVENTARIO 

.DATA
    ; Recursos (X, Y, Tipo, Activo) - cada recurso usa 8 bytes
    NUM_RECURSOS EQU 15
    RECURSOS DW 15 DUP(0, 0, 0, 1)  ; X, Y, Tipo, Activo
    
    ; Inventario
    INVENTARIO DB 3 DUP(0)
    OBJETIVO DB 2, 2, 2

.CODE

; CARGAR RECURSOS

CARGAR_RECURSOS PROC
    ; Minerales (Tipo 1)
    MOV RECURSOS[0], 80    ; X
    MOV RECURSOS[2], 80    ; Y
    MOV RECURSOS[4], 1     ; Tipo
    MOV RECURSOS[6], 1     ; Activo
    
    MOV RECURSOS[8], 120
    MOV RECURSOS[10], 60
    MOV RECURSOS[12], 1
    MOV RECURSOS[14], 1
    
    MOV RECURSOS[16], 200
    MOV RECURSOS[18], 100
    MOV RECURSOS[20], 1
    MOV RECURSOS[22], 1
    
    ; Madera (Tipo 2)
    MOV RECURSOS[24], 100
    MOV RECURSOS[26], 150
    MOV RECURSOS[28], 2
    MOV RECURSOS[30], 1
    
    MOV RECURSOS[32], 180
    MOV RECURSOS[34], 80
    MOV RECURSOS[36], 2
    MOV RECURSOS[38], 1
    
    MOV RECURSOS[40], 240
    MOV RECURSOS[42], 180
    MOV RECURSOS[44], 2
    MOV RECURSOS[46], 1
    
    ; Fruta (Tipo 3)
    MOV RECURSOS[48], 60
    MOV RECURSOS[50], 140
    MOV RECURSOS[52], 3
    MOV RECURSOS[54], 1
    
    MOV RECURSOS[56], 140
    MOV RECURSOS[58], 200
    MOV RECURSOS[60], 3
    MOV RECURSOS[62], 1
    
    MOV RECURSOS[64], 220
    MOV RECURSOS[66], 120
    MOV RECURSOS[68], 3
    MOV RECURSOS[70], 1
    
    RET
CARGAR_RECURSOS ENDP


; DIBUJAR RECURSOS 

DIBUJAR_RECURSOS PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    PUSH BP
    
    MOV BP, NUM_RECURSOS  ; Contador de recursos
    MOV SI, 0             ; Índice en el array
    
DIBUJAR_RECURSO_LOOP:
    CMP BP, 0
    JE SALIR_DIBUJAR_RECURSOS
    
    ; Verificar si está activo
    MOV AX, RECURSOS[SI+6]
    CMP AX, 1
    JNE SIGUIENTE_RECURSO
    
    ; Obtener posición y tipo
    MOV BX, RECURSOS[SI]   ; X en BX
    MOV DX, RECURSOS[SI+2] ; Y en DX
    MOV DI, RECURSOS[SI+4] ; Tipo en DI
    
    ; Verificar si está dentro de la pantalla
    CMP BX, 0
    JL SIGUIENTE_RECURSO
    CMP BX, 320
    JAE SIGUIENTE_RECURSO
    CMP DX, 0
    JL SIGUIENTE_RECURSO
    CMP DX, 240
    JAE SIGUIENTE_RECURSO
    
    ; Dibujar recurso con su color
    CALL DIBUJAR_UN_RECURSO

SIGUIENTE_RECURSO:
    ADD SI, 8        ; Siguiente recurso (4 words = 8 bytes)
    DEC BP
    JMP DIBUJAR_RECURSO_LOOP

SALIR_DIBUJAR_RECURSOS:
    JMP FIN_DIBUJAR_RECURSOS

FIN_DIBUJAR_RECURSOS:
    POP BP
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_RECURSOS ENDP


; DIBUJAR UN RECURSO
; BX = X, DX = Y, DI = tipo

DIBUJAR_UN_RECURSO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    ; Elegir color según tipo
    MOV AL, COLOR_AMARILLO  ; Por defecto mineral
    
    CMP DI, 2
    JNE NO_ES_MADERA
    MOV AL, COLOR_MARRON    ; Madera
    JMP SHORT TIENE_COLOR_REC
    
NO_ES_MADERA:
    CMP DI, 3
    JNE TIENE_COLOR_REC
    MOV AL, COLOR_ROJO      ; Fruta

TIENE_COLOR_REC:
    ; Guardar datos del recurso
    MOV TILE_COLOR, AL
    MOV TILE_X, BX
    MOV TILE_Y, DX
    
    MOV DI, 0  ; Fila (0-7)
    
FILA_REC:
    CMP DI, 8
    JAE FIN_REC
    
    MOV SI, 0  ; Columna (0-7)
    
COLUMNA_REC:
    CMP SI, 8
    JAE FIN_FILA_REC
    
    ; Calcular posición del pixel
    MOV DX, TILE_Y
    ADD DX, DI       ; Y = Y_recurso + fila
    
    MOV CX, TILE_X
    ADD CX, SI       ; X = X_recurso + columna
    
    ; Dibujar pixel usando función rápida
    MOV AL, TILE_COLOR
    CALL ESCRIBIR_PIXEL
    
    INC SI
    JMP COLUMNA_REC
    
FIN_FILA_REC:
    INC DI
    JMP FILA_REC
    
FIN_REC:
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_UN_RECURSO ENDP


; VERIFICAR RECOLECCION

VERIFICAR_RECOLECCION PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV CX, NUM_RECURSOS
    MOV SI, 0
    
VERIFICAR_RECURSO:
    ; Verificar si est� activo
    MOV AX, RECURSOS[SI+6]
    CMP AX, 1
    JNE SIGUIENTE_RECOLECCION
    
    ; Calcular distancia al jugador
    MOV AX, JUGADOR_X
    MOV BX, JUGADOR_Y
    ADD AX, 8  ; Centro del jugador
    ADD BX, 8
    
    SUB AX, RECURSOS[SI]   ; X
    SUB BX, RECURSOS[SI+2] ; Y
    
    ; Usar valor absoluto aproximado
    CMP AX, 0
    JGE POSITIVO_X
    NEG AX
POSITIVO_X:
    CMP BX, 0
    JGE POSITIVO_Y
    NEG BX
POSITIVO_Y:
    
    ; Verificar si est� cerca (16 p�xeles)
    CMP AX, 16
    JG SIGUIENTE_RECOLECCION
    CMP BX, 16
    JG SIGUIENTE_RECOLECCION
    
    ; Recolectar recurso
    MOV RECURSOS[SI+6], 0
    MOV BX, RECURSOS[SI+4]
    DEC BX
    INC INVENTARIO[BX]
    
SIGUIENTE_RECOLECCION:
    ADD SI, 8
    LOOP VERIFICAR_RECURSO
    
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
VERIFICAR_RECOLECCION ENDP


; VERIFICAR VICTORIA

VERIFICAR_VICTORIA PROC
    MOV SI, 0
    MOV CX, 3
    
VERIFICAR_OBJETIVO:
    MOV AL, INVENTARIO[SI]
    CMP AL, OBJETIVO[SI]
    JB NO_VICTORIA
    INC SI
    LOOP VERIFICAR_OBJETIVO
    
    MOV AL, 1  ; Victoria
    RET
    
NO_VICTORIA:
    MOV AL, 0  ; No victoria
    RET
VERIFICAR_VICTORIA ENDP