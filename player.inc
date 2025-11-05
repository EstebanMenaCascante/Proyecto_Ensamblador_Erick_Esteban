; JUGADOR Y MOVIMIENTO - PLAYER.INC


.DATA
    JUGADOR_X DW 320        ; Centro horizontal (columna 20 * 16)
    JUGADOR_Y DW 185        ; Fila 10 * 16 + 25 offset = 185
    JUGADOR_X_ANTERIOR DW 320
    JUGADOR_Y_ANTERIOR DW 185
    DIRECCION DB 0

.CODE

; INICIALIZAR JUGADOR

INICIALIZAR_JUGADOR PROC
    MOV JUGADOR_X, 320          ; Centro horizontal
    MOV JUGADOR_Y, 185          ; Fila 10 del mapa + offset del HUD
    MOV JUGADOR_X_ANTERIOR, 320
    MOV JUGADOR_Y_ANTERIOR, 185
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
    
    MOV SI, 0       ; Fila (0-15)
    
FILA_JUGADOR:
    CMP SI, 16
    JB CONTINUAR_FILA_JUGADOR
    JMP FIN_JUGADOR
CONTINUAR_FILA_JUGADOR:
    
    MOV BX, 0       ; Columna (0-15)
    
COLUMNA_JUGADOR:
    CMP BX, 16
    JB CONTINUAR_COLUMNA_JUGADOR
    JMP FIN_FILA_JUGADOR
CONTINUAR_COLUMNA_JUGADOR:
    
    ; Calcular posición del pixel
    MOV DX, TILE_Y
    ADD DX, SI
    
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Sprite de persona más realista
    MOV AL, 0  ; Transparente por defecto
    
    ; === CABEZA (filas 1-5, centrada) ===
    CMP SI, 1
    JB SALTAR_PIXEL_JUGADOR
    CMP SI, 6
    JAE CUERPO_PLAYER
    ; Cabeza redonda (columnas 5-10)
    CMP BX, 5
    JB SALTAR_PIXEL_JUGADOR
    CMP BX, 11
    JAE SALTAR_PIXEL_JUGADOR
    MOV AL, 14  ; Beige/piel
    ; Ojos (fila 3)
    CMP SI, 3
    JNE SEGUIR_CABEZA
    CMP BX, 6
    JE OJOS_PLAYER
    CMP BX, 9
    JNE SEGUIR_CABEZA
OJOS_PLAYER:
    MOV AL, COLOR_NEGRO
SEGUIR_CABEZA:
    ; Pelo (fila 1-2)
    CMP SI, 3
    JAE SALTAR_PIXEL_JUGADOR
    MOV AL, 6  ; Café oscuro (pelo)
    JMP SHORT SALTAR_PIXEL_JUGADOR
    
CUERPO_PLAYER:
    ; === TORSO (filas 6-10, camisa azul) ===
    CMP SI, 6
    JB SALTAR_PIXEL_JUGADOR
    CMP SI, 11
    JAE PIERNAS_PLAYER
    ; Torso (columnas 4-11)
    CMP BX, 4
    JB SALTAR_PIXEL_JUGADOR
    CMP BX, 12
    JAE SALTAR_PIXEL_JUGADOR
    MOV AL, COLOR_AZUL  ; Camisa azul
    ; Brazos a los lados
    CMP BX, 4
    JE BRAZOS_PLAYER
    CMP BX, 11
    JNE SEGUIR_TORSO
BRAZOS_PLAYER:
    MOV AL, 14  ; Brazos color piel
SEGUIR_TORSO:
    JMP SHORT SALTAR_PIXEL_JUGADOR
    
SALTAR_PIXEL_JUGADOR:
    JMP PIXEL_JUGADOR
    
PIERNAS_PLAYER:
    ; === PIERNAS (filas 11-15) ===
    CMP SI, 11
    JB PIXEL_JUGADOR
    ; Dos piernas separadas
    CMP BX, 6
    JB PIXEL_JUGADOR
    CMP BX, 7
    JBE PIERNA_IZQ
    CMP BX, 8
    JE PIXEL_JUGADOR
    CMP BX, 10
    JAE PIXEL_JUGADOR
    ; Pierna derecha
    MOV AL, 8  ; Gris (pantalón)
    JMP PIXEL_JUGADOR
PIERNA_IZQ:
    MOV AL, 8  ; Gris (pantalón)
    
PIXEL_JUGADOR:
    CMP AL, 0
    JE NO_DIBUJAR_PIXEL
    CALL ESCRIBIR_PIXEL
NO_DIBUJAR_PIXEL:
    
    INC BX
    JMP COLUMNA_JUGADOR
    
FIN_FILA_JUGADOR:
    INC SI
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


; BORRAR JUGADOR ANTERIOR (redibujar el tile del mapa)

BORRAR_JUGADOR_ANTERIOR PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    ; Convertir posición anterior a coordenadas de tile
    MOV AX, JUGADOR_X_ANTERIOR
    MOV CX, 16
    XOR DX, DX
    DIV CX              ; AX = columna del tile
    MOV SI, AX
    
    MOV AX, JUGADOR_Y_ANTERIOR
    SUB AX, 25          ; Restar offset del HUD
    XOR DX, DX
    DIV CX              ; AX = fila del tile
    MOV DI, AX
    
    ; Calcular índice en MAPA_DATOS: fila * 40 + columna
    MOV AX, DI
    MOV CX, 40
    MUL CX
    ADD AX, SI
    MOV BX, AX
    
    ; Obtener tipo de tile
    MOV BL, BYTE PTR MAPA_DATOS[BX]
    
    ; Calcular TILE_X y TILE_Y (con offset del HUD)
    MOV AX, SI
    MOV CX, 16
    MUL CX
    MOV TILE_X, AX
    
    MOV AX, DI
    MUL CX
    ADD AX, 25          ; Agregar offset del HUD
    MOV TILE_Y, AX
    
    ; Redibujar el tile según su tipo
    CMP BL, 0
    JE REDIBUJAR_PASTO_BORRAR
    CMP BL, 1
    JE REDIBUJAR_ROCA_BORRAR
    CMP BL, 2
    JE REDIBUJAR_AGUA_BORRAR
    JMP REDIBUJAR_PASTO_BORRAR  ; Por defecto pasto
    
; BORRAR SPRITE DEL JUGADOR PIXEL POR PIXEL

BORRAR_SPRITE_JUGADOR PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI

    MOV AX, JUGADOR_X_ANTERIOR
    MOV BX, JUGADOR_Y_ANTERIOR
    MOV TILE_X, AX
    MOV TILE_Y, BX

    MOV SI, 0

BORRAR_FILA:
    CMP SI, 16
    JGE FIN_BORRAR
    MOV DI, 0

BORRAR_COLUMNA:
    CMP DI, 16
    JGE SIGUIENTE_FILA

    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, DI

    ; Color de fondo según el mapa actual
    MOV AL, COLOR_SUELO_ACTUAL
    CALL ESCRIBIR_PIXEL

    INC DI
    JMP BORRAR_COLUMNA

SIGUIENTE_FILA:
    INC SI
    JMP BORRAR_FILA

FIN_BORRAR:
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
BORRAR_SPRITE_JUGADOR ENDP


REDIBUJAR_PASTO_BORRAR:
    CALL DIBUJAR_SPRITE_PASTO
    JMP FIN_BORRAR_JUGADOR
    
REDIBUJAR_ROCA_BORRAR:
    CALL DIBUJAR_SPRITE_ROCA
    JMP FIN_BORRAR_JUGADOR
    
REDIBUJAR_AGUA_BORRAR:
    CALL DIBUJAR_SPRITE_AGUA
    
FIN_BORRAR_JUGADOR:
    ; Ahora verificar si hay un recurso en esa posición y redibujarlo
    CALL REDIBUJAR_RECURSOS_EN_POSICION_ANTERIOR
    
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
BORRAR_JUGADOR_ANTERIOR ENDP


; REDIBUJAR RECURSOS QUE PUEDAN ESTAR EN LA POSICIÓN ANTERIOR
REDIBUJAR_RECURSOS_EN_POSICION_ANTERIOR PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    ; Recorrer todos los recursos
    MOV SI, OFFSET RECURSOS
    MOV CX, 45              ; 45 recursos
    
VERIFICAR_RECURSO_LOOP:
    ; Leer X, Y, activo, tipo
    MOV BX, WORD PTR [SI]       ; X
    MOV DX, WORD PTR [SI+2]     ; Y
    MOV AL, BYTE PTR [SI+4]     ; Activo
    MOV DI, WORD PTR [SI+6]     ; Tipo
    
    ; Si no está activo, saltar
    CMP AL, 0
    JE SIGUIENTE_RECURSO_BORRAR
    
    ; Verificar si está en la posición del jugador anterior
    ; (considerando que recursos son 8x8 en el centro del tile)
    MOV AX, JUGADOR_X_ANTERIOR
    ADD AX, 4               ; Centro del jugador
    SUB AX, 4               ; Margen
    CMP BX, AX
    JL SIGUIENTE_RECURSO_BORRAR
    ADD AX, 16
    CMP BX, AX
    JG SIGUIENTE_RECURSO_BORRAR
    
    MOV AX, JUGADOR_Y_ANTERIOR
    ADD AX, 4
    SUB AX, 4
    CMP DX, AX
    JL SIGUIENTE_RECURSO_BORRAR
    ADD AX, 16
    CMP DX, AX
    JG SIGUIENTE_RECURSO_BORRAR
    
    ; Está en la posición, redibujar
    PUSH SI
    PUSH CX
    MOV TILE_X, BX
    MOV TILE_Y, DX
    
    CMP DI, 1
    JE REDIBUJAR_PIEDRA_ANTERIOR
    CMP DI, 2
    JE REDIBUJAR_MADERA_ANTERIOR
    CMP DI, 3
    JE REDIBUJAR_FRUTA_ANTERIOR
    JMP FIN_REDIBUJAR_RECURSO_ANTERIOR
    
REDIBUJAR_PIEDRA_ANTERIOR:
    CALL DIBUJAR_SPRITE_PIEDRA
    JMP FIN_REDIBUJAR_RECURSO_ANTERIOR
    
REDIBUJAR_MADERA_ANTERIOR:
    CALL DIBUJAR_SPRITE_ARBOL
    JMP FIN_REDIBUJAR_RECURSO_ANTERIOR
    
REDIBUJAR_FRUTA_ANTERIOR:
    CALL DIBUJAR_SPRITE_FRUTA
    
FIN_REDIBUJAR_RECURSO_ANTERIOR:
    POP CX
    POP SI
    
SIGUIENTE_RECURSO_BORRAR:
    ADD SI, 8               ; Siguiente recurso (4 words)
    LOOP VERIFICAR_RECURSO_LOOP
    
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
REDIBUJAR_RECURSOS_EN_POSICION_ANTERIOR ENDP


; ACTUALIZAR VIEWPORT 

ACTUALIZAR_VIEWPORT PROC
    ; Por ahora no hacer nada - usar mapa completo
    RET
ACTUALIZAR_VIEWPORT ENDP


; VERIFICAR COLISIONES

VERIFICAR_COLISION PROC
    ; Convertir posición a coordenadas de mapa
    MOV AX, JUGADOR_X
    MOV BX, JUGADOR_Y
    ADD AX, 8
    ADD BX, 8
    SUB BX, 25              ; Restar offset del HUD
    
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