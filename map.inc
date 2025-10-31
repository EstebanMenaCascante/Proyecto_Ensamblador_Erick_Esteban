; MANEJO DE MAPAS - MAP.INC


.DATA
    ; Mapa (ahora 40x21 = 840 bytes para ocupar toda la pantalla EGA)
    MAPA_ANCHO DW ?
    MAPA_ALTO DW ?
    MAPA_DATOS DB 840 DUP(?)
    NOMBRE_ARCHIVO DB 'mapa.txt', 0
    CARACTER_TEMP DB ?
    
    ; Viewport (ventana visible del mapa)
    VIEWPORT_X DW 0
    VIEWPORT_Y DW 0
    
    ; Generación aleatoria
    SEMILLA DW 0
    CONTADOR_OBSTACULOS DW 0

.CODE

; CARGAR MAPA (ahora genera aleatoriamente)

CARGAR_MAPA PROC
    ; Establecer dimensiones para modo EGA 640x350
    ; Usar tiles más grandes para aprovechar la pantalla
    ; 640 pixels / 32 = 20 columnas (tiles de 32x32)
    ; 320 pixels / 16 = 20 filas (tiles de 16x16)
    ; O mantener 20x15 con tiles de 32x23 para llenar mejor
    ; Por ahora usamos 40x21 con tiles de 16x16 = 640x336
    MOV MAPA_ANCHO, 40
    MOV MAPA_ALTO, 21
    
    ; Inicializar semilla aleatoria con el reloj del sistema
    MOV AH, 2CH
    INT 21H
    MOV AL, DL      ; Centésimas de segundo
    MOV AH, DH      ; Segundos
    ADD AX, CX      ; Agregar minutos y horas
    MOV SEMILLA, AX
    
    ; Generar mapa aleatorio
    CALL GENERAR_MAPA_ALEATORIO
    
    RET
CARGAR_MAPA ENDP


; GENERAR MAPA ALEATORIO JUGABLE

GENERAR_MAPA_ALEATORIO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; 1. Llenar todo con tierra
    MOV SI, 0
    MOV CX, 840         ; 40x21 = 840 tiles
LLENAR_TIERRA:
    MOV MAPA_DATOS[SI], 0
    INC SI
    LOOP LLENAR_TIERRA
    
    ; 2. NO crear bordes (para usar todo el espacio)
    ; CALL CREAR_BORDES  ; Comentado para ocupar toda la ventana
    
    ; 3. Colocar menos obstáculos de piedra
    MOV CONTADOR_OBSTACULOS, 7      ; 7 grupos de piedras (reducido)
    CALL COLOCAR_OBSTACULOS_PIEDRA
    
    ; 4. Colocar menos obstáculos de agua
    MOV CONTADOR_OBSTACULOS, 4      ; 4 grupos de agua (reducido)
    CALL COLOCAR_OBSTACULOS_AGUA
    
    ; 5. Colocar hierba decorativa
    MOV CONTADOR_OBSTACULOS, 35     ; Más hierba para el mapa más grande
    CALL COLOCAR_HIERBA
    
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_ALEATORIO ENDP


; CREAR BORDES DEL MAPA

CREAR_BORDES PROC
    PUSH SI
    PUSH CX
    
    ; Borde superior
    MOV SI, 0
    MOV CX, 40          ; 40 columnas
BORDE_SUP:
    MOV MAPA_DATOS[SI], 1
    INC SI
    LOOP BORDE_SUP
    
    ; Borde inferior
    MOV SI, 800         ; Fila 20 * 40
    MOV CX, 40  
BORDE_INF:
    MOV MAPA_DATOS[SI], 1
    INC SI
    LOOP BORDE_INF
    
    ; Borde izquierdo
    MOV SI, 0
BORDE_IZQ:
    MOV MAPA_DATOS[SI], 1
    ADD SI, 40
    CMP SI, 840
    JB BORDE_IZQ
    
    ; Borde derecho
    MOV SI, 39
BORDE_DER:
    MOV MAPA_DATOS[SI], 1
    ADD SI, 40
    CMP SI, 840
    JB BORDE_DER
    
    POP CX
    POP SI
    RET
CREAR_BORDES ENDP


; GENERAR NÚMERO ALEATORIO
; Retorna: AX = número aleatorio

NUMERO_ALEATORIO PROC
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Generador congruencial lineal
    MOV AX, SEMILLA
    MOV BX, 25173
    MUL BX
    ADD AX, 13849
    MOV SEMILLA, AX
    
    POP DX
    POP CX
    POP BX
    RET
NUMERO_ALEATORIO ENDP


; COLOCAR OBSTÁCULOS DE PIEDRA (grupos pequeños)

COLOCAR_OBSTACULOS_PIEDRA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
SIGUIENTE_GRUPO_PIEDRA:
    CMP CONTADOR_OBSTACULOS, 0
    JE FIN_PIEDRAS
    DEC CONTADOR_OBSTACULOS
    
    ; Generar posición aleatoria (en todo el mapa)
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 38          ; Ancho útil (40-2)
    DIV BX
    ADD DX, 1           ; +1 para dejar un poco de margen
    PUSH DX             ; Columna
    
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 19          ; Alto útil (21-2)
    DIV BX
    ADD DX, 1           ; +1 para dejar un poco de margen
    MOV BX, DX          ; Fila
    
    ; Calcular posición en el mapa
    MOV AX, BX
    MOV CX, 40          ; 40 columnas
    MUL CX
    POP DX
    ADD AX, DX
    MOV SI, AX
    
    ; Verificar que no esté muy cerca del centro (spawn del jugador)
    CMP SI, 400
    JB POSICION_OK_PIEDRA
    CMP SI, 450
    JA POSICION_OK_PIEDRA
    JMP SIGUIENTE_GRUPO_PIEDRA      ; Muy cerca del jugador
    
POSICION_OK_PIEDRA:
    ; Colocar grupo de 2x2 o 3x2 piedras
    CALL NUMERO_ALEATORIO
    AND AX, 1
    ADD AX, 2           ; 2 o 3 piedras de ancho
    MOV CX, AX
    
COLOCAR_FILA_PIEDRA:
    PUSH CX
    MOV CX, 2           ; 2 de alto
    
COLOCAR_COLUMNA_PIEDRA:
    CMP SI, 840
    JAE SIGUIENTE_PIEDRA
    MOV BYTE PTR MAPA_DATOS[SI], 1
    ADD SI, 40          ; Siguiente fila (40 columnas)
SIGUIENTE_PIEDRA:
    LOOP COLOCAR_COLUMNA_PIEDRA
    
    SUB SI, 79          ; Volver arriba y mover a la derecha
    POP CX
    LOOP COLOCAR_FILA_PIEDRA
    
    JMP SIGUIENTE_GRUPO_PIEDRA
    
FIN_PIEDRAS:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
COLOCAR_OBSTACULOS_PIEDRA ENDP


; COLOCAR OBSTÁCULOS DE AGUA (grupos pequeños)

COLOCAR_OBSTACULOS_AGUA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
SIGUIENTE_GRUPO_AGUA:
    CMP CONTADOR_OBSTACULOS, 0
    JE FIN_AGUA
    DEC CONTADOR_OBSTACULOS
    
    ; Generar posición aleatoria (en todo el mapa)
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 38
    DIV BX
    ADD DX, 1
    PUSH DX
    
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 19
    DIV BX
    ADD DX, 1
    MOV BX, DX
    
    MOV AX, BX
    MOV CX, 40          ; 40 columnas
    MUL CX
    POP DX
    ADD AX, DX
    MOV SI, AX
    
    ; Verificar spawn del jugador
    CMP SI, 400
    JB POSICION_OK_AGUA
    CMP SI, 450
    JA POSICION_OK_AGUA
    JMP SIGUIENTE_GRUPO_AGUA
    
POSICION_OK_AGUA:
    ; Colocar grupo pequeño de agua (2x2 máximo)
    MOV CX, 2
    
COLOCAR_FILA_AGUA:
    PUSH CX
    MOV CX, 2
    
COLOCAR_COLUMNA_AGUA:
    CMP SI, 840
    JAE SIGUIENTE_AGUA
    MOV BYTE PTR MAPA_DATOS[SI], 2
    INC SI
SIGUIENTE_AGUA:
    LOOP COLOCAR_COLUMNA_AGUA
    
    ADD SI, 38          ; Siguiente fila (40-2)
    POP CX
    LOOP COLOCAR_FILA_AGUA
    
    JMP SIGUIENTE_GRUPO_AGUA
    
FIN_AGUA:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
COLOCAR_OBSTACULOS_AGUA ENDP


; COLOCAR HIERBA DECORATIVA

COLOCAR_HIERBA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
SIGUIENTE_HIERBA:
    CMP CONTADOR_OBSTACULOS, 0
    JE FIN_HIERBA
    DEC CONTADOR_OBSTACULOS
    
    ; Generar posición aleatoria
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 38
    DIV BX
    ADD DX, 1
    PUSH DX
    
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 19
    DIV BX
    ADD DX, 1
    MOV BX, DX
    
    MOV AX, BX
    MOV CX, 40          ; 40 columnas
    MUL CX
    POP DX
    ADD AX, DX
    MOV SI, AX
    
    ; Colocar solo si es tierra (no sobrescribir obstáculos)
    CMP SI, 840
    JAE SIGUIENTE_HIERBA
    CMP BYTE PTR MAPA_DATOS[SI], 0
    JNE SIGUIENTE_HIERBA
    MOV BYTE PTR MAPA_DATOS[SI], 3
    
    JMP SIGUIENTE_HIERBA
    
FIN_HIERBA:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
COLOCAR_HIERBA ENDP