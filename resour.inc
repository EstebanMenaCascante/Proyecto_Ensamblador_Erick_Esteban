
; RECURSOS E INVENTARIO

.DATA
    ; Recursos (X, Y, Tipo, Activo) - cada recurso usa 8 bytes
    NUM_RECURSOS EQU 45
    RECURSOS DW 45 DUP(0, 0, 0, 1)  ; X, Y, Tipo, Activo
    
    ; Copia de respaldo de recursos para reiniciar el juego
    RECURSOS_BACKUP DW 45 DUP(0, 0, 0, 1)
    RECURSOS_CARGADOS DB 0  ; Flag para saber si ya fueron generados
    
    ; Inventario
    INVENTARIO DB 3 DUP(0)
    OBJETIVO DB 15, 15, 15

.CODE

; CARGAR RECURSOS (solo la primera vez, luego se mantienen fijos)

CARGAR_RECURSOS PROC
    ; Verificar si los recursos ya fueron generados
    CMP BYTE PTR RECURSOS_CARGADOS, 1
    JE RECURSOS_YA_EXISTEN
    
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    MOV SI, 0               ; Índice en array de recursos
    MOV DI, 0               ; Contador de recursos colocados
    
    ; Colocar 15 minerales (tipo 1)
    MOV CX, 15
COLOCAR_MINERALES:
    PUSH CX
    MOV BX, 1               ; Tipo mineral
    CALL GENERAR_POSICION_RECURSO
    POP CX
    LOOP COLOCAR_MINERALES
    
    ; Colocar 15 maderas (tipo 2)
    MOV CX, 15
COLOCAR_MADERAS:
    PUSH CX
    MOV BX, 2               ; Tipo madera
    CALL GENERAR_POSICION_RECURSO
    POP CX
    LOOP COLOCAR_MADERAS
    
    ; Colocar 15 frutas (tipo 3)
    MOV CX, 15
COLOCAR_FRUTAS:
    PUSH CX
    MOV BX, 3               ; Tipo fruta
    CALL GENERAR_POSICION_RECURSO
    POP CX
    LOOP COLOCAR_FRUTAS
    
    ; Guardar copia de respaldo para poder reiniciar
    CALL GUARDAR_RECURSOS_BACKUP
    
    ; Marcar como cargados
    MOV BYTE PTR RECURSOS_CARGADOS, 1
    
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    
RECURSOS_YA_EXISTEN:
    RET
CARGAR_RECURSOS ENDP


; GUARDAR COPIA DE RESPALDO DE RECURSOS

GUARDAR_RECURSOS_BACKUP PROC
    PUSH AX
    PUSH CX
    PUSH SI
    PUSH DI
    
    ; Copiar RECURSOS a RECURSOS_BACKUP
    MOV SI, OFFSET RECURSOS
    MOV DI, OFFSET RECURSOS_BACKUP
    MOV CX, 180             ; 45 recursos * 4 words = 180 words
    CLD
    REP MOVSW
    
    POP DI
    POP SI
    POP CX
    POP AX
    RET
GUARDAR_RECURSOS_BACKUP ENDP


; RESTAURAR RECURSOS DESDE RESPALDO

RESTAURAR_RECURSOS_BACKUP PROC
    PUSH AX
    PUSH CX
    PUSH SI
    PUSH DI
    
    ; Copiar RECURSOS_BACKUP a RECURSOS
    MOV SI, OFFSET RECURSOS_BACKUP
    MOV DI, OFFSET RECURSOS
    MOV CX, 180             ; 45 recursos * 4 words = 180 words
    CLD
    REP MOVSW
    
    POP DI
    POP SI
    POP CX
    POP AX
    RET
RESTAURAR_RECURSOS_BACKUP ENDP


; REINICIAR INVENTARIO Y PUNTUACION

REINICIAR_INVENTARIO PROC
    PUSH AX
    PUSH SI
    
    MOV SI, 0
    MOV BYTE PTR INVENTARIO[SI], 0
    MOV BYTE PTR INVENTARIO[SI+1], 0
    MOV BYTE PTR INVENTARIO[SI+2], 0
    
    ; Resetear puntuación también
    MOV WORD PTR PUNTUACION_HUD, 0
    
    POP SI
    POP AX
    RET
REINICIAR_INVENTARIO ENDP


; GENERAR POSICIÓN ALEATORIA PARA UN RECURSO
; BX = tipo de recurso

GENERAR_POSICION_RECURSO PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
REINTENTAR_POSICION:
    ; Generar X aleatorio (16 a 624 para 640 pixels)
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV CX, 608             ; 624-16
    DIV CX
    ADD DX, 16
    MOV RECURSOS[SI], DX    ; Guardar X
    
    ; Generar Y aleatorio (41 a 345 para offset del HUD de 25 pixels)
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV CX, 304             ; 320-16
    DIV CX
    ADD DX, 41              ; 16 + 25 (offset del HUD)
    MOV RECURSOS[SI+2], DX  ; Guardar Y
    
    ; Verificar que no esté en un obstáculo
    PUSH BX
    PUSH SI
    CALL VERIFICAR_POSICION_RECURSO_VALIDA
    POP SI
    POP BX
    JC REINTENTAR_POSICION  ; Si carry está activo, posición inválida
    
    ; Guardar tipo y estado
    MOV RECURSOS[SI+4], BX  ; Tipo
    MOV WORD PTR RECURSOS[SI+6], 1   ; Activo
    
    ADD SI, 8               ; Siguiente recurso
    
    POP DX
    POP CX
    POP AX
    RET
GENERAR_POSICION_RECURSO ENDP


; VERIFICAR SI LA POSICIÓN DEL RECURSO ES VÁLIDA
; Entrada: RECURSOS[SI] = X, RECURSOS[SI+2] = Y
; Salida: CF = 1 si inválida, CF = 0 si válida

VERIFICAR_POSICION_RECURSO_VALIDA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Convertir coordenadas a posición de tile
    MOV AX, RECURSOS[SI]
    ADD AX, 8               ; Centro del recurso
    MOV CX, 16
    XOR DX, DX
    DIV CX
    MOV BX, AX              ; BX = columna
    
    MOV AX, RECURSOS[SI+2]
    SUB AX, 25              ; Restar offset del HUD
    ADD AX, 8
    XOR DX, DX
    MOV CX, 16
    DIV CX
    MOV DX, AX              ; DX = fila
    
    ; Calcular índice en el mapa
    MOV AX, DX
    MOV CX, 40              ; 40 columnas
    MUL CX
    ADD AX, BX
    
    ; Verificar límites (mapa es 40x20 = 800 tiles)
    CMP AX, 800
    JAE POSICION_INVALIDA
    
    MOV BX, AX
    
    ; Verificar que sea tierra (0) o hierba (3) - NO agua (2) ni roca (1)
    MOV AL, MAPA_DATOS[BX]
    CMP AL, 0
    JE POSICION_VALIDA
    CMP AL, 3
    JE POSICION_VALIDA
    
POSICION_INVALIDA:
    STC                     ; Set carry = posición inválida
    JMP FIN_VERIFICAR_REC
    
POSICION_VALIDA:
    CLC                     ; Clear carry = posición válida
    
FIN_VERIFICAR_REC:
    POP DX
    POP CX
    POP BX
    POP AX
    RET
VERIFICAR_POSICION_RECURSO_VALIDA ENDP


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
    CMP BX, 640             ; 640 pixels de ancho
    JAE SIGUIENTE_RECURSO
    CMP DX, 0
    JL SIGUIENTE_RECURSO
    CMP DX, 336             ; 336 pixels de alto
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
    
    ; Guardar posición
    MOV TILE_X, BX
    MOV TILE_Y, DX
    
    ; Dibujar según tipo de recurso
    CMP DI, 1
    JE DIBUJAR_PIEDRA_REC
    CMP DI, 2
    JE DIBUJAR_MADERA_REC
    CMP DI, 3
    JE DIBUJAR_FRUTA_REC
    JMP FIN_DIBUJAR_RECURSO
    
DIBUJAR_PIEDRA_REC:
    CALL DIBUJAR_SPRITE_PIEDRA
    JMP FIN_DIBUJAR_RECURSO
    
DIBUJAR_MADERA_REC:
    CALL DIBUJAR_SPRITE_ARBOL
    JMP FIN_DIBUJAR_RECURSO
    
DIBUJAR_FRUTA_REC:
    CALL DIBUJAR_SPRITE_FRUTA
    
FIN_DIBUJAR_RECURSO:
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
    
    ; Verificar si está cerca (16 píxeles)
    CMP AX, 16
    JG SIGUIENTE_RECOLECCION
    CMP BX, 16
    JG SIGUIENTE_RECOLECCION
    
    ; Guardar posición del recurso para el efecto
    PUSH AX
    PUSH BX
    MOV BX, RECURSOS[SI]
    MOV DX, RECURSOS[SI+2]
    
    ; BORRAR COMPLETAMENTE EL RECURSO dibujando pasto
    PUSH SI
    PUSH CX
    MOV TILE_X, BX
    MOV TILE_Y, DX
    CALL BORRAR_RECURSO_COMPLETO
    POP CX
    POP SI
    
    CALL EFECTO_RECOLECCION
    POP BX
    POP AX
    
    ; Recolectar recurso
    MOV RECURSOS[SI+6], 0
    MOV BX, RECURSOS[SI+4]
    
    ; Agregar puntos según el tipo
    PUSH BX
    CALL AGREGAR_PUNTOS_HUD
    POP BX
    
    ; Reproducir sonido de recolección
    PUSH SI
    CALL SONIDO_RECOLECTAR
    POP SI
    
    ; Actualizar inventario
    DEC BX
    INC INVENTARIO[BX]
    
    ; Actualizar HUD después de recolectar
    PUSH SI
    PUSH CX
    CALL DIBUJAR_HUD_COMPLETO
    POP CX
    POP SI
    
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


; ============ SPRITES DE RECURSOS (8x8) ============

; SPRITE DE PIEDRA (8x8)
DIBUJAR_SPRITE_PIEDRA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_PIEDRA_REC:
    CMP SI, 8
    JAE FIN_PIEDRA_REC
    
    MOV BX, 0
COL_PIEDRA_REC:
    CMP BX, 8
    JAE FIN_FILA_PIEDRA_REC
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Piedra gris con textura
    MOV AL, 7  ; Gris claro
    
    ; Bordes oscuros
    CMP SI, 0
    JE BORDE_PIEDRA_REC
    CMP SI, 7
    JE BORDE_PIEDRA_REC
    CMP BX, 0
    JE BORDE_PIEDRA_REC
    CMP BX, 7
    JNE NO_BORDE_PIEDRA_REC
BORDE_PIEDRA_REC:
    MOV AL, 8  ; Gris oscuro
    JMP PIXEL_PIEDRA_REC
    
NO_BORDE_PIEDRA_REC:
    ; Centro más claro
    CMP SI, 3
    JB PIXEL_PIEDRA_REC
    CMP SI, 5
    JA PIXEL_PIEDRA_REC
    CMP BX, 3
    JB PIXEL_PIEDRA_REC
    CMP BX, 5
    JA PIXEL_PIEDRA_REC
    MOV AL, COLOR_BLANCO
    
PIXEL_PIEDRA_REC:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COL_PIEDRA_REC
    
FIN_FILA_PIEDRA_REC:
    INC SI
    JMP FILA_PIEDRA_REC
    
FIN_PIEDRA_REC:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_PIEDRA ENDP


; SPRITE DE PALOS DE MADERA (8x8)
DIBUJAR_SPRITE_ARBOL PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_ARBOL_REC:
    CMP SI, 8
    JAE FIN_ARBOL_REC
    
    MOV BX, 0
COL_ARBOL_REC:
    CMP BX, 8
    JAE FIN_FILA_ARBOL_REC
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Palos de madera (2 palos verticales)
    MOV AL, 6  ; Café para madera
    
    ; Palo izquierdo (columnas 2-3)
    CMP BX, 2
    JB NO_DIBUJAR_ARBOL_REC
    CMP BX, 4
    JB PIXEL_ARBOL_REC
    
    ; Palo derecho (columnas 5-6)
    CMP BX, 5
    JB NO_DIBUJAR_ARBOL_REC
    CMP BX, 7
    JA NO_DIBUJAR_ARBOL_REC
    
PIXEL_ARBOL_REC:
    CALL ESCRIBIR_PIXEL
    
NO_DIBUJAR_ARBOL_REC:
    INC BX
    JMP COL_ARBOL_REC
    
FIN_FILA_ARBOL_REC:
    INC SI
    JMP FILA_ARBOL_REC
    
FIN_ARBOL_REC:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_ARBOL ENDP


; SPRITE DE FRUTA/MANZANA (8x8)
DIBUJAR_SPRITE_FRUTA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_FRUTA_REC:
    CMP SI, 8
    JAE FIN_FRUTA_REC
    
    MOV BX, 0
COL_FRUTA_REC:
    CMP BX, 8
    JAE FIN_FILA_FRUTA_REC
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Manzana redonda roja
    MOV AL, COLOR_ROJO
    
    ; Forma de manzana (redonda)
    CMP SI, 0
    JE BORDE_FRUTA
    CMP SI, 7
    JE BORDE_FRUTA
    JMP NO_BORDE_FRUTA
BORDE_FRUTA:
    CMP BX, 2
    JB NO_DIBUJAR_FRUTA
    CMP BX, 6
    JA NO_DIBUJAR_FRUTA
    JMP PIXEL_FRUTA
    
NO_BORDE_FRUTA:
    CMP BX, 1
    JB NO_DIBUJAR_FRUTA
    CMP BX, 7
    JA NO_DIBUJAR_FRUTA
    
    ; Brillo en la manzana (arriba izquierda)
    CMP SI, 2
    JNE NO_BRILLO_FRUTA
    CMP BX, 3
    JNE NO_BRILLO_FRUTA
    MOV AL, COLOR_BLANCO
    JMP PIXEL_FRUTA
    
NO_BRILLO_FRUTA:
    ; Tallo (parte superior)
    CMP SI, 1
    JA PIXEL_FRUTA
    CMP BX, 4
    JNE NO_DIBUJAR_FRUTA
    MOV AL, 6  ; Café (tallo)
    JMP PIXEL_FRUTA
    
NO_DIBUJAR_FRUTA:
    INC BX
    JMP COL_FRUTA_REC
    
PIXEL_FRUTA:
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COL_FRUTA_REC
    
FIN_FILA_FRUTA_REC:
    INC SI
    JMP FILA_FRUTA_REC
    
FIN_FRUTA_REC:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_SPRITE_FRUTA ENDP


; BORRAR RECURSO COMPLETO (dibujar pasto en su lugar 8x8)
BORRAR_RECURSO_COMPLETO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    MOV SI, 0
FILA_BORRAR_REC:
    CMP SI, 8
    JAE FIN_BORRAR_REC
    
    MOV BX, 0
COL_BORRAR_REC:
    CMP BX, 8
    JAE FIN_FILA_BORRAR_REC
    
    MOV DX, TILE_Y
    ADD DX, SI
    MOV CX, TILE_X
    ADD CX, BX
    
    ; Dibujar con el color del suelo del mapa actual
    MOV AL, COLOR_SUELO_ACTUAL

    ; Dibujar pasto verde sólido del mismo color del fondo
    MOV AL, COLOR_VERDE
    CALL ESCRIBIR_PIXEL
    
    INC BX
    JMP COL_BORRAR_REC
    
FIN_FILA_BORRAR_REC:
    INC SI
    JMP FILA_BORRAR_REC
    
FIN_BORRAR_REC:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
BORRAR_RECURSO_COMPLETO ENDP