; RUTINAS GRAFICAS - GRAPHICS.INC


.DATA
    COLOR_NEGRO EQU 0
    COLOR_AZUL EQU 1
    COLOR_VERDE EQU 2
    COLOR_CYAN EQU 3
    COLOR_ROJO EQU 4
    COLOR_MAGENTA EQU 5
    COLOR_MARRON EQU 6
    COLOR_GRIS_CLARO EQU 7
    COLOR_GRIS EQU 8
    COLOR_AZUL_CLARO EQU 9
    COLOR_VERDE_CLARO EQU 10
    COLOR_CYAN_CLARO EQU 11
    COLOR_ROJO_CLARO EQU 12
    COLOR_MAGENTA_CLARO EQU 13
    COLOR_AMARILLO EQU 14
    COLOR_BLANCO EQU 15
    
    COLOR_VERDE_OSCURO EQU 2  ; Verde oscuro para madera
    COLOR_ARENA EQU 14        ; Amarillo para arena del desierto
    COLOR_TIERRA EQU 6        ; Marrón para tierra de montaña
    
    ; Variables temporales para dibujar tiles
    TILE_X DW 0
    TILE_Y DW 0
    TILE_COLOR DB 0
    FILA_ACTUAL DW 0
    COLUMNA_ACTUAL DW 0
    
    ; Color del suelo según mapa actual
    COLOR_SUELO_ACTUAL DB 2   ; Por defecto verde (bosque)
    
    ; Segmento de memoria de video EGA 640x350x16
    VIDEO_SEG EQU 0A000H

.CODE

; INICIAR MODO GRAFICO EGA

INICIAR_MODO_GRAFICO PROC
    MOV AX, 0010H
    INT 10H
    RET
INICIAR_MODO_GRAFICO ENDP


; ESCRIBIR PIXEL RAPIDO
; CX = X, DX = Y, AL = Color

ESCRIBIR_PIXEL PROC
    PUSH AX
    PUSH BX
    MOV AH, 0CH      ; Función escribir pixel
    XOR BH, BH       ; Página 0
    INT 10H
    POP BX
    POP AX
    RET
ESCRIBIR_PIXEL ENDP


; LIMPIAR PANTALLA

LIMPIAR_PANTALLA PROC
    MOV AX, 0600H
    MOV BH, COLOR_NEGRO
    MOV CX, 0000H
    MOV DX, 184FH
    INT 10H
    RET
LIMPIAR_PANTALLA ENDP


; DIBUJAR FRAME COMPLETO

DIBUJAR_FRAME_COMPLETO PROC
    CALL DIBUJAR_MAPA_COMPLETO
    CALL DIBUJAR_RECURSOS
    CALL DIBUJAR_JUGADOR
    RET
DIBUJAR_FRAME_COMPLETO ENDP


; DIBUJAR MAPA COMPLETO 

DIBUJAR_MAPA_COMPLETO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    PUSH BP
    
    MOV SI, 0           ; Índice del mapa
    MOV FILA_ACTUAL, 0  ; Fila 0
    
PROXIMA_FILA_MAPA:
    MOV AX, FILA_ACTUAL
    CMP AX, 20          ; 20 filas (para caber con HUD de 25px)
    JAE FIN_MAPA_COMPLETO
    
    MOV COLUMNA_ACTUAL, 0  ; Columna 0
    
PROXIMA_COLUMNA_MAPA:
    MOV AX, COLUMNA_ACTUAL
    CMP AX, 40          ; 40 columnas
    JAE SIGUIENTE_FILA_MAPA
    
    ; Obtener tile del mapa
    MOV BL, MAPA_DATOS[SI]
    
    ; Obtener fila y columna
    MOV BP, FILA_ACTUAL
    MOV DI, COLUMNA_ACTUAL
    
    ; Calcular color y dibujar
    CALL OBTENER_COLOR_Y_DIBUJAR_TILE
    
    ; Siguiente columna
    INC SI
    INC COLUMNA_ACTUAL
    JMP PROXIMA_COLUMNA_MAPA

SIGUIENTE_FILA_MAPA:
    INC FILA_ACTUAL
    JMP PROXIMA_FILA_MAPA

FIN_MAPA_COMPLETO:
    POP BP
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_MAPA_COMPLETO ENDP


; OBTENER COLOR Y DIBUJAR TILE CON SPRITE/TEXTURA
; BL = tipo de tile, DI = columna, BP = fila

OBTENER_COLOR_Y_DIBUJAR_TILE PROC
    PUSH AX
    PUSH BX
    
    ; Calcular posición X e Y del tile primero
    MOV AX, DI
    PUSH BX
    MOV BX, 16
    MUL BX
    POP BX
    MOV TILE_X, AX          ; X = columna * 16
    
    MOV AX, BP
    PUSH BX
    MOV BX, 16
    MUL BX
    POP BX
    ADD AX, 25          ; Offset para el HUD (25 pixels de altura)
    MOV TILE_Y, AX          ; Y = fila * 16 + 25
    
    ; Dibujar según tipo de tile
    CMP BL, 0
    JE DIBUJAR_PASTO
    CMP BL, 1
    JE DIBUJAR_ROCA
    CMP BL, 2
    JE DIBUJAR_AGUA_TILE
    CMP BL, 3
    JE DIBUJAR_PASTO        ; Hierba = pasto
    CMP BL, 4
    JE DIBUJAR_ARBOL_TILE   ; Árboles del mapa
    CMP BL, 5
    JE DIBUJAR_PORTAL_TILE  ; Portales brillantes
    JMP DIBUJAR_PASTO       ; Por defecto pasto
    
DIBUJAR_PASTO:
    CALL DIBUJAR_SPRITE_PASTO
    JMP FIN_DIBUJAR_TILE_TEXTURA
    
DIBUJAR_ROCA:
    CALL DIBUJAR_SPRITE_ROCA
    JMP FIN_DIBUJAR_TILE_TEXTURA
    
DIBUJAR_ARBOL_TILE:
    CALL DIBUJAR_SPRITE_ARBOL_MAPA
    JMP FIN_DIBUJAR_TILE_TEXTURA
    
DIBUJAR_PORTAL_TILE:
    CALL DIBUJAR_PORTAL
    JMP FIN_DIBUJAR_TILE_TEXTURA
    
DIBUJAR_AGUA_TILE:
    CALL DIBUJAR_SPRITE_AGUA
    
FIN_DIBUJAR_TILE_TEXTURA:
    POP BX
    POP AX
    RET
OBTENER_COLOR_Y_DIBUJAR_TILE ENDP


; DIBUJAR UN TILE 16X16
; TILE_X, TILE_Y = posición superior izquierda
; TILE_COLOR = color a usar

DIBUJAR_TILE_16X16 PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH DI
    
    MOV DI, 0        ; Fila del tile (0-15)
    
FILA_TILE:
    CMP DI, 16
    JAE FIN_DIBUJAR_TILE
    
    MOV BX, 0        ; Columna del tile (0-15)
    
COLUMNA_TILE:
    CMP BX, 16
    JAE FIN_FILA_TILE
    
    ; Calcular posición del pixel
    MOV DX, TILE_Y
    ADD DX, DI       ; Y = Y_tile + fila
    
    MOV CX, TILE_X
    ADD CX, BX       ; X = X_tile + columna
    
    ; Dibujar pixel usando función rápida
    MOV AL, TILE_COLOR
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COLUMNA_TILE
    
FIN_FILA_TILE:
    INC DI
    JMP FILA_TILE
    
FIN_DIBUJAR_TILE:
    POP DI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_TILE_16X16 ENDP

; DIBUJAR HUD

DIBUJAR_HUD PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    MOV CX, 0
    MOV DX, 336      ; Línea debajo del área de juego
    MOV AL, COLOR_BLANCO
    
DIBUJAR_LINEA_HUD:
    CMP CX, 640      ; 640 pixels de ancho
    JAE FIN_HUD
    CALL ESCRIBIR_PIXEL
    INC CX
    JMP DIBUJAR_LINEA_HUD
    
FIN_HUD:
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_HUD ENDP


; ========== SPRITES CON TEXTURAS ==========

; DIBUJAR SPRITE DE PASTO (16x16) - Natural y suave
DIBUJAR_SPRITE_PASTO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_PASTO:
    CMP SI, 16
    JB CONTINUAR_FILA_PASTO
    JMP FIN_PASTO
CONTINUAR_FILA_PASTO:
    
    MOV BX, 0
COLUMNA_PASTO:
    CMP BX, 16
    JB CONTINUAR_COLUMNA_PASTO
    JMP FIN_FILA_PASTO
CONTINUAR_COLUMNA_PASTO:
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Usar color del suelo según el mapa actual
    MOV AL, COLOR_SUELO_ACTUAL
    
    ; Puntos de variación de tono (más oscuros)
    CMP SI, 1
    JNE NO_F1_PASTO
    CMP BX, 4
    JE OSCURO_PASTO
    CMP BX, 13
    JE OSCURO_PASTO
NO_F1_PASTO:
    
    CMP SI, 3
    JNE NO_F3_PASTO
    CMP BX, 2
    JE OSCURO_PASTO
    CMP BX, 9
    JE OSCURO_PASTO
NO_F3_PASTO:
    
    CMP SI, 6
    JNE NO_F6_PASTO
    CMP BX, 7
    JE OSCURO_PASTO
    CMP BX, 15
    JE OSCURO_PASTO
NO_F6_PASTO:
    
    CMP SI, 8
    JNE NO_F8_PASTO
    CMP BX, 0
    JE OSCURO_PASTO
    CMP BX, 11
    JE OSCURO_PASTO
NO_F8_PASTO:
    
    CMP SI, 10
    JNE NO_F10_PASTO
    CMP BX, 5
    JE OSCURO_PASTO
    CMP BX, 14
    JE OSCURO_PASTO
NO_F10_PASTO:
    
    CMP SI, 13
    JNE NO_F13_PASTO
    CMP BX, 3
    JE OSCURO_PASTO
    CMP BX, 10
    JE OSCURO_PASTO
NO_F13_PASTO:
    
    CMP SI, 15
    JNE PIXEL_PASTO
    CMP BX, 8
    JE OSCURO_PASTO
    JMP PIXEL_PASTO
    
OSCURO_PASTO:
    ; Usar un tono más oscuro del color actual
    ; Verde (2) → Verde oscuro (2, mismo)
    ; Gris (8) → Negro (0)
    ; Marrón (6) → Gris oscuro (8)
    MOV AL, COLOR_SUELO_ACTUAL
    
    CMP AL, 8               ; ¿Es gris (montaña)?
    JNE NO_ES_GRIS
    MOV AL, 0               ; Usar negro para variación lodosa
    JMP PIXEL_PASTO
NO_ES_GRIS:
    
    CMP AL, 6               ; ¿Es marrón (desierto)?
    JNE NO_ES_MARRON
    MOV AL, 8               ; Usar gris para variación de arena
    JMP PIXEL_PASTO
NO_ES_MARRON:
    
    ; Verde o cualquier otro → mantener verde oscuro
    MOV AL, 2
    
PIXEL_PASTO:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COLUMNA_PASTO
    
FIN_FILA_PASTO:
    INC SI
    JMP FILA_PASTO
    
FIN_PASTO:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_PASTO ENDP


; DIBUJAR SPRITE DE ROCA (16x16) - Estilo retro
DIBUJAR_SPRITE_ROCA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_ROCA:
    CMP SI, 16
    JAE FIN_ROCA
    
    MOV BX, 0
COLUMNA_ROCA:
    CMP BX, 16
    JAE FIN_FILA_ROCA
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Roca con forma y sombras definidas
    MOV AL, 7  ; Gris claro por defecto
    
    ; Contorno negro (forma de roca)
    CMP SI, 0
    JE BORDE_ROCA_NEGRO
    CMP SI, 15
    JE BORDE_ROCA_NEGRO
    CMP BX, 0
    JE BORDE_ROCA_NEGRO
    CMP BX, 15
    JE BORDE_ROCA_NEGRO
    
    ; Esquinas redondeadas
    CMP SI, 1
    JNE NO_ESQUINA1_ROCA
    CMP BX, 1
    JE BORDE_ROCA_NEGRO
    CMP BX, 14
    JE BORDE_ROCA_NEGRO
NO_ESQUINA1_ROCA:
    
    CMP SI, 14
    JNE NO_ESQUINA14_ROCA
    CMP BX, 1
    JE BORDE_ROCA_NEGRO
    CMP BX, 14
    JE BORDE_ROCA_NEGRO
NO_ESQUINA14_ROCA:
    
    ; Sombras internas (gris oscuro)
    CMP SI, 12
    JB NO_SOMBRA_ROCA
    CMP BX, 3
    JB NO_SOMBRA_ROCA
    MOV AL, 8  ; Gris oscuro
    JMP PIXEL_ROCA
    
NO_SOMBRA_ROCA:
    ; Luces (blanco)
    CMP SI, 3
    JA NO_LUZ_ROCA
    CMP BX, 10
    JB NO_LUZ_ROCA
    MOV AL, COLOR_BLANCO
    JMP PIXEL_ROCA
    
NO_LUZ_ROCA:
    ; Textura media (gris)
    MOV AL, COLOR_GRIS
    JMP PIXEL_ROCA
    
BORDE_ROCA_NEGRO:
    MOV AL, COLOR_NEGRO
    
PIXEL_ROCA:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COLUMNA_ROCA
    
FIN_FILA_ROCA:
    INC SI
    JMP FILA_ROCA
    
FIN_ROCA:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_ROCA ENDP


; DIBUJAR SPRITE DE AGUA (16x16) - Estilo retro con ondas
DIBUJAR_SPRITE_AGUA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_AGUA_SPRITE:
    CMP SI, 16
    JAE FIN_AGUA_SPRITE
    
    MOV BX, 0
COLUMNA_AGUA_SPRITE:
    CMP BX, 16
    JAE FIN_FILA_AGUA_SPRITE
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Agua azul con ondas horizontales definidas
    MOV AL, COLOR_AZUL  ; Azul oscuro base
    
    ; Líneas de ondas (azul claro) - horizontales cada 4 filas
    CMP SI, 3
    JE ONDA_AGUA_CLARO
    CMP SI, 7
    JE ONDA_AGUA_CLARO
    CMP SI, 11
    JE ONDA_AGUA_CLARO
    CMP SI, 15
    JE ONDA_AGUA_CLARO
    JMP PIXEL_AGUA_SPRITE
    
ONDA_AGUA_CLARO:
    MOV AL, 9  ; Azul claro
    
PIXEL_AGUA_SPRITE:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COLUMNA_AGUA_SPRITE
    
FIN_FILA_AGUA_SPRITE:
    INC SI
    JMP FILA_AGUA_SPRITE
    
FIN_AGUA_SPRITE:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_AGUA ENDP


; DIBUJAR SPRITE DE ÁRBOL DEL MAPA (16x16) - Estilo Zelda/Pokemon
DIBUJAR_SPRITE_ARBOL_MAPA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_ARBOL_MAPA:
    CMP SI, 16
    JAE FIN_ARBOL_MAPA
    
    MOV BX, 0
COLUMNA_ARBOL_MAPA:
    CMP BX, 16
    JAE FIN_FILA_ARBOL_MAPA
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Fondo de pasto
    MOV AL, COLOR_VERDE
    
    ; Copa del árbol (filas 0-10, circular)
    CMP SI, 11
    JAE TRONCO_ARBOL_MAPA
    
    ; Forma de copa verde oscuro
    MOV AL, COLOR_VERDE_OSCURO
    
    ; Fila 0-1: parte superior estrecha
    CMP SI, 2
    JAE FILA2_ARBOL
    CMP BX, 6
    JB FONDO_ARBOL_MAPA
    CMP BX, 10
    JA FONDO_ARBOL_MAPA
    JMP PIXEL_ARBOL_MAPA
    
FILA2_ARBOL:
    ; Fila 2-9: copa ancha
    CMP SI, 10
    JAE FILA10_ARBOL
    CMP BX, 3
    JB FONDO_ARBOL_MAPA
    CMP BX, 13
    JA FONDO_ARBOL_MAPA
    JMP PIXEL_ARBOL_MAPA
    
FILA10_ARBOL:
    ; Fila 10: base de copa
    CMP BX, 5
    JB FONDO_ARBOL_MAPA
    CMP BX, 11
    JA FONDO_ARBOL_MAPA
    JMP PIXEL_ARBOL_MAPA
    
TRONCO_ARBOL_MAPA:
    ; Tronco café (filas 11-15, columnas 7-8)
    CMP BX, 7
    JB FONDO_ARBOL_MAPA
    CMP BX, 9
    JA FONDO_ARBOL_MAPA
    MOV AL, 6  ; Café
    JMP PIXEL_ARBOL_MAPA
    
FONDO_ARBOL_MAPA:
    MOV AL, COLOR_VERDE  ; Pasto de fondo
    
PIXEL_ARBOL_MAPA:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COLUMNA_ARBOL_MAPA
    
FIN_FILA_ARBOL_MAPA:
    INC SI
    JMP FILA_ARBOL_MAPA
    
FIN_ARBOL_MAPA:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_ARBOL_MAPA ENDP


; ========================================
; DIBUJAR PORTAL (marco brillante con efecto mágico)
; ========================================
DIBUJAR_PORTAL PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_PORTAL:
    CMP SI, 16
    JAE FIN_PORTAL
    
    MOV BX, 0
COLUMNA_PORTAL:
    CMP BX, 16
    JB CONTINUAR_PIXEL_PORTAL
    JMP FIN_FILA_PORTAL
    
CONTINUAR_PIXEL_PORTAL:
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Color base del portal (fondo oscuro para contraste)
    MOV AL, 0           ; Negro como base
    
    ; Marco exterior (bordes del tile) - Amarillo brillante
    CMP SI, 0
    JE MARCO_PORTAL
    CMP SI, 15
    JE MARCO_PORTAL
    CMP BX, 0
    JE MARCO_PORTAL
    CMP BX, 15
    JNE CHEQUEAR_MARCO_INTERNO
    
MARCO_PORTAL:
    MOV AL, COLOR_AMARILLO      ; Amarillo brillante
    JMP PIXEL_PORTAL
    
CHEQUEAR_MARCO_INTERNO:
    ; Marco interior - Cyan brillante (efecto mágico)
    CMP SI, 2
    JE MARCO_INTERNO
    CMP SI, 13
    JE MARCO_INTERNO
    CMP BX, 2
    JE MARCO_INTERNO
    CMP BX, 13
    JNE CHEQUEAR_CENTRO
    
MARCO_INTERNO:
    MOV AL, COLOR_CYAN_CLARO    ; Cyan brillante
    JMP PIXEL_PORTAL
    
CHEQUEAR_CENTRO:
    ; Centro - Relleno con efecto mágico brillante
    ; Área central (4x4 píxeles blancos brillantes)
    CMP SI, 6
    JB RELLENO_CYAN
    CMP SI, 10
    JA RELLENO_CYAN
    CMP BX, 6
    JB RELLENO_CYAN
    CMP BX, 10
    JA RELLENO_CYAN
    
    ; Centro mágico blanco/cyan
    MOV AL, COLOR_BLANCO
    CMP SI, 7
    JNE TEST_CYAN_CENTRO
    MOV AL, COLOR_CYAN_CLARO
TEST_CYAN_CENTRO:
    CMP SI, 9
    JNE PIXEL_PORTAL
    MOV AL, COLOR_CYAN_CLARO
    JMP PIXEL_PORTAL
    
RELLENO_CYAN:
    ; Relleno intermedio entre marcos - Cyan oscuro
    MOV AL, COLOR_CYAN
    
PIXEL_PORTAL:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COLUMNA_PORTAL
    
FIN_FILA_PORTAL:
    INC SI
    JMP FILA_PORTAL
    
FIN_PORTAL:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_PORTAL ENDP
