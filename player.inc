; JUGADOR Y MOVIMIENTO - PLAYER.INC


.DATA
    JUGADOR_X DW 320        ; Centro horizontal de 640
    JUGADOR_Y DW 160        ; Centro vertical para mapa 21 filas
    JUGADOR_X_ANTERIOR DW 320
    JUGADOR_Y_ANTERIOR DW 160
    DIRECCION DB 0

.CODE

; INICIALIZAR JUGADOR

INICIALIZAR_JUGADOR PROC
    MOV JUGADOR_X, 320          ; Centro de 640
    MOV JUGADOR_Y, 160          ; Centro de mapa 21 filas
    MOV JUGADOR_X_ANTERIOR, 320
    MOV JUGADOR_Y_ANTERIOR, 160
    MOV DIRECCION, 0
    RET
INICIALIZAR_JUGADOR ENDP


; DIBUJAR JUGADOR

DIBUJAR_JUGADOR PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    ; Guardar posición del jugador
    MOV AX, JUGADOR_X
    MOV BX, JUGADOR_Y
    MOV TILE_X, AX
    MOV TILE_Y, BX
    MOV TILE_COLOR, COLOR_ROJO
    
    MOV DI, 0       ; Fila (0-15)
    
FILA_JUGADOR:
    CMP DI, 16
    JAE FIN_JUGADOR
    
    MOV SI, 0       ; Columna (0-15)
    
COLUMNA_JUGADOR:
    CMP SI, 16
    JAE FIN_FILA_JUGADOR
    
    ; Calcular posición del pixel
    MOV DX, TILE_Y
    ADD DX, DI       ; Y = Y_jugador + fila
    
    MOV CX, TILE_X
    ADD CX, SI       ; X = X_jugador + columna
    
    ; Dibujar pixel usando función rápida
    MOV AL, TILE_COLOR
    CALL ESCRIBIR_PIXEL
    
    INC SI
    JMP COLUMNA_JUGADOR
    
FIN_FILA_JUGADOR:
    INC DI
    JMP FILA_JUGADOR
    
FIN_JUGADOR:
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_JUGADOR ENDP


; BORRAR JUGADOR ANTERIOR (pintar del color del piso)

BORRAR_JUGADOR_ANTERIOR PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    ; Usar la posición anterior
    MOV AX, JUGADOR_X_ANTERIOR
    MOV BX, JUGADOR_Y_ANTERIOR
    MOV TILE_X, AX
    MOV TILE_Y, BX
    MOV TILE_COLOR, COLOR_MARRON   ; Color del piso (tierra)
    
    MOV DI, 0       ; Fila (0-15)
    
FILA_BORRAR:
    CMP DI, 16
    JAE FIN_BORRAR
    
    MOV SI, 0       ; Columna (0-15)
    
COLUMNA_BORRAR:
    CMP SI, 16
    JAE FIN_FILA_BORRAR
    
    ; Calcular posición del pixel
    MOV DX, TILE_Y
    ADD DX, DI       ; Y = Y_anterior + fila
    
    MOV CX, TILE_X
    ADD CX, SI       ; X = X_anterior + columna
    
    ; Dibujar pixel del color del piso
    MOV AL, TILE_COLOR
    CALL ESCRIBIR_PIXEL
    
    INC SI
    JMP COLUMNA_BORRAR
    
FIN_FILA_BORRAR:
    INC DI
    JMP FILA_BORRAR
    
FIN_BORRAR:
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
BORRAR_JUGADOR_ANTERIOR ENDP


; ACTUALIZAR VIEWPORT 

ACTUALIZAR_VIEWPORT PROC
    ; Por ahora no hacer nada - usar mapa completo
    RET
ACTUALIZAR_VIEWPORT ENDP


; VERIFICAR COLISIONES

VERIFICAR_COLISION PROC
    ; Convertir posici�n a coordenadas de mapa
    MOV AX, JUGADOR_X
    MOV BX, JUGADOR_Y
    ADD AX, 8
    ADD BX, 8
    
    MOV CX, 16
    XOR DX, DX
    DIV CX
    MOV SI, AX
    
    MOV AX, BX
    XOR DX, DX
    DIV CX
    MOV DI, AX
    
    MOV AX, DI
    MOV CX, 40          ; 40 columnas
    MUL CX
    ADD AX, SI
    MOV BX, AX
    
    ; Verificar si está en agua (tipo 2) - GAME OVER
    CMP BYTE PTR MAPA_DATOS[BX], 2
    JE AGUA_DETECTADA

    ; Verificar colisiones normales (piedra, árboles)
    CMP BYTE PTR MAPA_DATOS[BX], 1
    JE COLISION_DETECTADA
    CMP BYTE PTR MAPA_DATOS[BX], 4      ; Árboles también tienen colisión
    JE COLISION_DETECTADA
    
    CLC
    RET
    
AGUA_DETECTADA:
    ; Setear flag especial para GAME OVER
    MOV AL, 2           ; Código especial: 2 = muerte por agua
    STC
    RET
    
COLISION_DETECTADA:
    MOV AL, 1           ; Código normal: 1 = colisión bloqueante
    STC
    RET
VERIFICAR_COLISION ENDP