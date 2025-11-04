; RUTINAS GRAFICAS - GRAPHICS.INC


.DATA
    COLOR_NEGRO EQU 0
    COLOR_AZUL EQU 1
    COLOR_VERDE EQU 2
    COLOR_ROJO EQU 4
    COLOR_MARRON EQU 6
    COLOR_VERDE_OSCURO EQU 2  ; Verde oscuro para madera (distinto del piso)
    COLOR_GRIS EQU 8
    COLOR_AMARILLO EQU 14
    COLOR_BLANCO EQU 15
    
    ; Variables temporales para dibujar tiles
    TILE_X DW 0
    TILE_Y DW 0
    TILE_COLOR DB 0
    FILA_ACTUAL DW 0
    COLUMNA_ACTUAL DW 0
    
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
    CMP AX, 21          ; 21 filas
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


; OBTENER COLOR Y DIBUJAR TILE
; BL = tipo de tile, DI = columna, BP = fila

OBTENER_COLOR_Y_DIBUJAR_TILE PROC
    PUSH AX
    PUSH BX
    
    ; Elegir color según tipo de tile
    MOV AL, COLOR_MARRON    ; Tierra (marrón)
    
    CMP BL, 1
    JNE VERIFICAR_AGUA
    MOV AL, COLOR_GRIS      ; Piedra
    JMP SHORT TIENE_COLOR
    
VERIFICAR_AGUA:
    CMP BL, 2
    JNE VERIFICAR_HIERBA
    MOV AL, COLOR_AZUL      ; Agua
    JMP SHORT TIENE_COLOR
    
VERIFICAR_HIERBA:
    CMP BL, 3
    JNE TIENE_COLOR
    MOV AL, COLOR_VERDE     ; Hierba
    
TIENE_COLOR:
    ; Guardar color en memoria
    MOV TILE_COLOR, AL
    
    ; Calcular posición X e Y del tile
    MOV AX, DI
    MOV BX, 16
    MUL BX
    MOV TILE_X, AX          ; X = columna * 16
    
    MOV AX, BP
    MOV BX, 16
    MUL BX
    MOV TILE_Y, AX          ; Y = fila * 16
    
    ; Dibujar el tile 16x16
    CALL DIBUJAR_TILE_16X16
    
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