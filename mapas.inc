; ========================================
; SISTEMA DE MÚLTIPLES MAPAS - MAPAS.INC
; ========================================
; Gestiona 3 mapas diferentes con transiciones:
; - Mapa Principal (centro)
; - Mapa Montañoso (izquierda)
; - Mapa Desértico (derecha)


.DATA
    ; === IDENTIFICADORES DE MAPAS ===
    MAPA_PRINCIPAL EQU 0
    MAPA_MONTANOSO EQU 1
    MAPA_DESERTICO EQU 2
    
    MAPA_ACTUAL DB 0        ; Mapa donde está el jugador actualmente
    MAPA_ANTERIOR DB 0      ; Para regresar
    
    ; === DATOS DE LOS 3 MAPAS (40x20 = 800 bytes cada uno) ===
    MAPA_PRINCIPAL_DATOS DB 800 DUP(?)
    MAPA_MONTANOSO_DATOS DB 800 DUP(?)
    MAPA_DESERTICO_DATOS DB 800 DUP(?)
    
    ; === FLAGS DE GENERACIÓN ===
    MAPA_PRINCIPAL_GENERADO DB 0
    MAPA_MONTANOSO_GENERADO DB 0
    MAPA_DESERTICO_GENERADO DB 0
    
    ; === PUNTOS DE ENTRADA/SALIDA ===
    ; Cuando entras por la izquierda, apareces en X=608 (derecha del nuevo mapa)
    ; Cuando entras por la derecha, apareces en X=16 (izquierda del nuevo mapa)
    ENTRADA_IZQUIERDA_X DW 608      ; 40*16 - 32 (margen)
    ENTRADA_DERECHA_X DW 16
    ENTRADA_Y DW 185                ; Misma Y (centro vertical)
    
    ; === LÍMITES PARA DETECTAR CAMBIO DE MAPA ===
    ; Solo cambian cuando el jugador cruza completamente (para evitar cambios accidentales)
    LIMITE_IZQ_CAMBIO DW 5          ; Si X <= 5, cambiar a mapa izquierdo
    LIMITE_DER_CAMBIO DW 620        ; Si X >= 620, cambiar a mapa derecho
    
    ; === VARIABLES DE TRANSICIÓN ===
    TRANSICION_ACTIVA DB 0
    CONTADOR_FADE DB 0

.CODE

; ========================================
; INICIALIZAR SISTEMA DE MAPAS
; ========================================
INICIALIZAR_MAPAS PROC
    ; Empezar en el mapa principal
    MOV MAPA_ACTUAL, MAPA_PRINCIPAL
    MOV MAPA_ANTERIOR, MAPA_PRINCIPAL
    
    ; Generar mapa principal la primera vez
    CMP MAPA_PRINCIPAL_GENERADO, 1
    JE YA_EXISTE_PRINCIPAL_INIT
    
    CALL GENERAR_MAPA_TIPO_PRINCIPAL
    CALL COPIAR_MAPA_DATOS_A_PRINCIPAL
    MOV MAPA_PRINCIPAL_GENERADO, 1
    
YA_EXISTE_PRINCIPAL_INIT:
    ; Cargar el mapa principal a MAPA_DATOS
    CALL COPIAR_PRINCIPAL_A_MAPA_DATOS
    
    ; Establecer color del suelo para bosque
    CALL ACTUALIZAR_COLOR_SUELO
    
    RET
INICIALIZAR_MAPAS ENDP


; ========================================
; GENERAR MAPA PRINCIPAL (Bosque/Pradera)
; ========================================
GENERAR_MAPA_PRINCIPAL PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; Verificar si ya fue generado
    CMP MAPA_PRINCIPAL_GENERADO, 1
    JE YA_GENERADO_PRINCIPAL
    
    ; Apuntar a datos del mapa principal
    LEA SI, MAPA_PRINCIPAL_DATOS
    
    ; Generar terreno (70% pasto, 15% agua, 10% rocas, 5% árboles)
    MOV CX, 800         ; 40x20 tiles
    
GEN_TILE_PRINCIPAL:
    ; Número aleatorio 0-99
    CALL RANDOM_100
    
    ; 0-69: Pasto (0)
    CMP AL, 70
    JB ES_PASTO_PRINCIPAL
    
    ; 70-84: Agua (2)
    CMP AL, 85
    JB ES_AGUA_PRINCIPAL
    
    ; 85-94: Rocas (1)
    CMP AL, 95
    JB ES_ROCA_PRINCIPAL
    
    ; 95-99: Árboles (3)
    MOV BYTE PTR [SI], 3
    JMP SIGUIENTE_TILE_PRINCIPAL
    
ES_PASTO_PRINCIPAL:
    MOV BYTE PTR [SI], 0
    JMP SIGUIENTE_TILE_PRINCIPAL
    
ES_AGUA_PRINCIPAL:
    MOV BYTE PTR [SI], 2
    JMP SIGUIENTE_TILE_PRINCIPAL
    
ES_ROCA_PRINCIPAL:
    MOV BYTE PTR [SI], 1
    
SIGUIENTE_TILE_PRINCIPAL:
    INC SI
    LOOP GEN_TILE_PRINCIPAL
    
    ; Crear camino seguro horizontal (fila 10)
    LEA SI, MAPA_PRINCIPAL_DATOS
    ADD SI, 400         ; Fila 10 * 40
    MOV CX, 40
CAMINO_HORIZONTAL_PRINCIPAL:
    MOV BYTE PTR [SI], 0    ; Pasto
    INC SI
    LOOP CAMINO_HORIZONTAL_PRINCIPAL
    
    ; Crear zona de portal IZQUIERDO (filas 8-12, columna 0)
    LEA SI, MAPA_PRINCIPAL_DATOS
    ADD SI, 320         ; Fila 8 * 40
    MOV CX, 5           ; 5 filas (8,9,10,11,12)
PORTAL_IZQ_PRINCIPAL:
    MOV BYTE PTR [SI], 0    ; Pasto (accesible)
    ADD SI, 40              ; Siguiente fila
    LOOP PORTAL_IZQ_PRINCIPAL
    
    ; Crear zona de portal DERECHO (filas 8-12, columna 39)
    LEA SI, MAPA_PRINCIPAL_DATOS
    ADD SI, 359         ; Fila 8 * 40 + 39
    MOV CX, 5
PORTAL_DER_PRINCIPAL:
    MOV BYTE PTR [SI], 0    ; Pasto (accesible)
    ADD SI, 40
    LOOP PORTAL_DER_PRINCIPAL
    
    ; Marcar como generado
    MOV MAPA_PRINCIPAL_GENERADO, 1
    
YA_GENERADO_PRINCIPAL:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_PRINCIPAL ENDP


; ========================================
; GENERAR MAPA MONTAÑOSO (Izquierda)
; ========================================
GENERAR_MAPA_MONTANOSO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; Verificar si ya fue generado
    CMP MAPA_MONTANOSO_GENERADO, 1
    JE YA_GENERADO_MONTANOSO
    
    ; Apuntar a datos del mapa montañoso
    LEA SI, MAPA_MONTANOSO_DATOS
    
    ; Generar terreno montañoso (60% rocas, 30% pasto, 10% árboles)
    MOV CX, 800
    
GEN_TILE_MONTANOSO:
    CALL RANDOM_100
    
    ; 0-59: Rocas (1)
    CMP AL, 60
    JB ES_ROCA_MONTANOSO
    
    ; 60-89: Pasto (0)
    CMP AL, 90
    JB ES_PASTO_MONTANOSO
    
    ; 90-99: Árboles (3)
    MOV BYTE PTR [SI], 3
    JMP SIGUIENTE_TILE_MONTANOSO
    
ES_ROCA_MONTANOSO:
    MOV BYTE PTR [SI], 1
    JMP SIGUIENTE_TILE_MONTANOSO
    
ES_PASTO_MONTANOSO:
    MOV BYTE PTR [SI], 0
    
SIGUIENTE_TILE_MONTANOSO:
    INC SI
    LOOP GEN_TILE_MONTANOSO
    
    ; Crear camino seguro (fila 10)
    LEA SI, MAPA_MONTANOSO_DATOS
    ADD SI, 400
    MOV CX, 40
CAMINO_MONTANOSO:
    MOV BYTE PTR [SI], 0
    INC SI
    LOOP CAMINO_MONTANOSO
    
    ; Crear zona de portal DERECHO (filas 8-12, columna 39)
    LEA SI, MAPA_MONTANOSO_DATOS
    ADD SI, 359         ; Fila 8 * 40 + 39
    MOV CX, 5
PORTAL_DER_MONTANOSO:
    MOV BYTE PTR [SI], 0    ; Pasto (accesible)
    ADD SI, 40
    LOOP PORTAL_DER_MONTANOSO
    
    ; Marcar como generado
    MOV MAPA_MONTANOSO_GENERADO, 1
    
YA_GENERADO_MONTANOSO:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_MONTANOSO ENDP


; ========================================
; GENERAR MAPA DESÉRTICO (Derecha)
; ========================================
GENERAR_MAPA_DESERTICO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; Verificar si ya fue generado
    CMP MAPA_DESERTICO_GENERADO, 1
    JE YA_GENERADO_DESERTICO
    
    ; Apuntar a datos del mapa desértico
    LEA SI, MAPA_DESERTICO_DATOS
    
    ; Generar terreno desértico (80% arena/pasto seco, 15% rocas, 5% árboles secos)
    MOV CX, 800
    
GEN_TILE_DESERTICO:
    CALL RANDOM_100
    
    ; 0-79: Arena/pasto (0)
    CMP AL, 80
    JB ES_ARENA_DESERTICO
    
    ; 80-94: Rocas (1)
    CMP AL, 95
    JB ES_ROCA_DESERTICO
    
    ; 95-99: Árboles (3)
    MOV BYTE PTR [SI], 3
    JMP SIGUIENTE_TILE_DESERTICO
    
ES_ARENA_DESERTICO:
    MOV BYTE PTR [SI], 0
    JMP SIGUIENTE_TILE_DESERTICO
    
ES_ROCA_DESERTICO:
    MOV BYTE PTR [SI], 1
    
SIGUIENTE_TILE_DESERTICO:
    INC SI
    LOOP GEN_TILE_DESERTICO
    
    ; Crear camino seguro
    LEA SI, MAPA_DESERTICO_DATOS
    ADD SI, 400
    MOV CX, 40
CAMINO_DESERTICO:
    MOV BYTE PTR [SI], 0
    INC SI
    LOOP CAMINO_DESERTICO
    
    ; Crear zona de portal IZQUIERDO (filas 8-12, columna 0)
    LEA SI, MAPA_DESERTICO_DATOS
    ADD SI, 320         ; Fila 8 * 40
    MOV CX, 5
PORTAL_IZQ_DESERTICO:
    MOV BYTE PTR [SI], 0    ; Pasto (accesible)
    ADD SI, 40
    LOOP PORTAL_IZQ_DESERTICO
    
    ; Marcar como generado
    MOV MAPA_DESERTICO_GENERADO, 1
    
YA_GENERADO_DESERTICO:
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_DESERTICO ENDP


; ========================================
; VERIFICAR CAMBIO DE MAPA
; Llamar después de cada movimiento del jugador
; ========================================
VERIFICAR_CAMBIO_MAPA PROC
    PUSH AX
    PUSH BX
    
    MOV AX, JUGADOR_X
    
    ; ¿Cruzó el límite izquierdo?
    CMP AX, LIMITE_IZQ_CAMBIO
    JBE CRUCE_IZQUIERDO
    
    ; ¿Cruzó el límite derecho?
    CMP AX, LIMITE_DER_CAMBIO
    JAE CRUCE_DERECHO
    
    ; No hay cambio
    JMP FIN_VERIFICAR_CAMBIO
    
CRUCE_IZQUIERDO:
    ; Decidir a qué mapa ir según el mapa actual
    MOV AL, MAPA_ACTUAL
    
    CMP AL, MAPA_PRINCIPAL
    JE IR_A_DESERTICO_IZQ       ; Izquierda desde Principal = Desértico
    
    CMP AL, MAPA_DESERTICO
    JE NO_SALIR_MAS_IZQ         ; Ya estás en el borde extremo
    
    CMP AL, MAPA_MONTANOSO
    JE REGRESAR_A_PRINCIPAL_DESDE_MONTANOSO
    
IR_A_DESERTICO_IZQ:
    MOV BL, MAPA_DESERTICO
    JMP CAMBIAR_MAPA_AHORA
    
CRUCE_DERECHO:
    MOV AL, MAPA_ACTUAL
    
    CMP AL, MAPA_PRINCIPAL
    JE IR_A_MONTANOSO_DER       ; Derecha desde Principal = Montañoso
    
    CMP AL, MAPA_MONTANOSO
    JE NO_SALIR_MAS_DER         ; Ya estás en el borde extremo
    
    CMP AL, MAPA_DESERTICO
    JE REGRESAR_A_PRINCIPAL_DESDE_DESERTICO
    
IR_A_MONTANOSO_DER:
    MOV BL, MAPA_MONTANOSO
    JMP CAMBIAR_MAPA_AHORA
    
REGRESAR_A_PRINCIPAL_DESDE_MONTANOSO:
    MOV BL, MAPA_PRINCIPAL
    JMP CAMBIAR_MAPA_AHORA
    
REGRESAR_A_PRINCIPAL_DESDE_DESERTICO:
    MOV BL, MAPA_PRINCIPAL
    JMP CAMBIAR_MAPA_AHORA
    
NO_SALIR_MAS_IZQ:
NO_SALIR_MAS_DER:
    ; Restaurar posición del jugador dentro de límites
    MOV AX, JUGADOR_X_ANTERIOR
    MOV JUGADOR_X, AX
    JMP FIN_VERIFICAR_CAMBIO
    
CAMBIAR_MAPA_AHORA:
    ; BL contiene el ID del nuevo mapa
    MOV MAPA_ANTERIOR, AL   ; Guardar el mapa anterior
    CALL TRANSICION_CAMBIO_MAPA
    
FIN_VERIFICAR_CAMBIO:
    POP BX
    POP AX
    RET
VERIFICAR_CAMBIO_MAPA ENDP


; ========================================
; TRANSICIÓN Y CAMBIO DE MAPA
; BL = ID del nuevo mapa
; ========================================
TRANSICION_CAMBIO_MAPA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; === FASE 1: FADE OUT (oscurecer pantalla) ===
    CALL FADE_OUT_SIMPLE
    
    ; === FASE 2: CAMBIAR MAPA ===
    ; Actualizar mapa actual
    MOV MAPA_ACTUAL, BL
    
    ; Verificar si el mapa destino ya fue generado
    CMP BL, MAPA_PRINCIPAL
    JE VERIFICAR_PRINCIPAL_GEN
    CMP BL, MAPA_MONTANOSO
    JE VERIFICAR_MONTANOSO_GEN
    CMP BL, MAPA_DESERTICO
    JE VERIFICAR_DESERTICO_GEN
    JMP CONTINUAR_TRANS
    
VERIFICAR_PRINCIPAL_GEN:
    CMP MAPA_PRINCIPAL_GENERADO, 1
    JE CARGAR_PRINCIPAL_EXISTENTE
    ; No existe, generar nuevo
    CALL GENERAR_MAPA_TIPO_PRINCIPAL
    ; Copiar a buffer permanente
    CALL COPIAR_MAPA_DATOS_A_PRINCIPAL
    MOV MAPA_PRINCIPAL_GENERADO, 1
    JMP CONTINUAR_TRANS
CARGAR_PRINCIPAL_EXISTENTE:
    ; Ya existe, cargar desde buffer
    CALL COPIAR_PRINCIPAL_A_MAPA_DATOS
    JMP CONTINUAR_TRANS
    
VERIFICAR_MONTANOSO_GEN:
    CMP MAPA_MONTANOSO_GENERADO, 1
    JE CARGAR_MONTANOSO_EXISTENTE
    CALL GENERAR_MAPA_TIPO_MONTANOSO
    CALL COPIAR_MAPA_DATOS_A_MONTANOSO
    MOV MAPA_MONTANOSO_GENERADO, 1
    JMP CONTINUAR_TRANS
CARGAR_MONTANOSO_EXISTENTE:
    CALL COPIAR_MONTANOSO_A_MAPA_DATOS
    JMP CONTINUAR_TRANS
    
VERIFICAR_DESERTICO_GEN:
    CMP MAPA_DESERTICO_GENERADO, 1
    JE CARGAR_DESERTICO_EXISTENTE
    CALL GENERAR_MAPA_TIPO_DESERTICO
    CALL COPIAR_MAPA_DATOS_A_DESERTICO
    MOV MAPA_DESERTICO_GENERADO, 1
    JMP CONTINUAR_TRANS
CARGAR_DESERTICO_EXISTENTE:
    CALL COPIAR_DESERTICO_A_MAPA_DATOS
    
CONTINUAR_TRANS:
    MOV BYTE PTR MAPA_CARGADO, 1
    
    ; Actualizar color del suelo según el mapa
    CALL ACTUALIZAR_COLOR_SUELO
    
    ; === FASE 3: POSICIONAR JUGADOR ===
    ; Si venimos de la izquierda, aparecer a la derecha
    ; Si venimos de la derecha, aparecer a la izquierda
    MOV AL, MAPA_ANTERIOR
    CMP AL, BL
    JE NO_REPOSICIONAR     ; Mismo mapa? No hacer nada
    
    ; Determinar de qué lado venimos
    MOV AX, JUGADOR_X
    CMP AX, 320             ; Centro del mapa
    JB VENIA_DE_IZQUIERDA
    
    ; Venía de la derecha
    MOV AX, ENTRADA_DERECHA_X
    MOV JUGADOR_X, AX
    JMP REPOSICIONAR_LISTO
    
VENIA_DE_IZQUIERDA:
    MOV AX, ENTRADA_IZQUIERDA_X
    MOV JUGADOR_X, AX
    
REPOSICIONAR_LISTO:
    ; Mantener Y en el centro
    MOV AX, ENTRADA_Y
    MOV JUGADOR_Y, AX
    MOV JUGADOR_Y_ANTERIOR, AX
    MOV AX, JUGADOR_X
    MOV JUGADOR_X_ANTERIOR, AX
    
NO_REPOSICIONAR:
    
    ; === FASE 4: REDIBUJAR TODO ===
    CALL DIBUJAR_FRAME_COMPLETO
    
    ; === FASE 5: FADE IN (aclarar pantalla) ===
    CALL FADE_IN_SIMPLE
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
TRANSICION_CAMBIO_MAPA ENDP


; ========================================
; CARGAR DATOS DEL MAPA ACTUAL A MAPA_DATOS
; ========================================
CARGAR_DATOS_MAPA_ACTUAL PROC
    PUSH AX
    PUSH CX
    PUSH SI
    PUSH DI
    
    ; Determinar origen según MAPA_ACTUAL
    MOV AL, MAPA_ACTUAL
    
    CMP AL, MAPA_PRINCIPAL
    JE CARGAR_PRINCIPAL
    
    CMP AL, MAPA_MONTANOSO
    JE CARGAR_MONTANOSO
    
    CMP AL, MAPA_DESERTICO
    JE CARGAR_DESERTICO
    
    JMP FIN_CARGAR_DATOS
    
CARGAR_PRINCIPAL:
    ; Generar si no existe
    CMP MAPA_PRINCIPAL_GENERADO, 1
    JE COPIAR_PRINCIPAL
    CALL GENERAR_MAPA_PRINCIPAL
COPIAR_PRINCIPAL:
    LEA SI, MAPA_PRINCIPAL_DATOS
    JMP COPIAR_A_MAPA_DATOS
    
CARGAR_MONTANOSO:
    CMP MAPA_MONTANOSO_GENERADO, 1
    JE COPIAR_MONTANOSO
    CALL GENERAR_MAPA_MONTANOSO
COPIAR_MONTANOSO:
    LEA SI, MAPA_MONTANOSO_DATOS
    JMP COPIAR_A_MAPA_DATOS
    
CARGAR_DESERTICO:
    CMP MAPA_DESERTICO_GENERADO, 1
    JE COPIAR_DESERTICO
    CALL GENERAR_MAPA_DESERTICO
COPIAR_DESERTICO:
    LEA SI, MAPA_DESERTICO_DATOS
    
COPIAR_A_MAPA_DATOS:
    ; Asegurar que ES = DS para MOVSB
    PUSH ES
    PUSH DS
    POP ES
    
    ; SI ya contiene la dirección origen (viene de arriba)
    LEA DI, [MAPA_DATOS]
    MOV CX, 800
    
    ; Copiar byte por byte
COPIAR_LOOP:
    LODSB               ; Cargar byte de [SI] a AL
    STOSB               ; Guardar AL en [DI]
    LOOP COPIAR_LOOP
    
    POP ES
    JMP FIN_CARGAR_DATOS

FIN_CARGAR_DATOS:
    POP DI
    POP SI
    POP CX
    POP AX
    RET
CARGAR_DATOS_MAPA_ACTUAL ENDP


; ========================================
; FADE OUT SIMPLE (Oscurecer pantalla)
; ========================================
FADE_OUT_SIMPLE PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Dibujar rectángulo negro sobre toda la pantalla en 3 pasos
    MOV CX, 3
    
FADE_OUT_LOOP:
    PUSH CX
    
    ; Dibujar rectángulo negro semitransparente
    MOV CX, 0           ; X inicio
    MOV DX, 0           ; Y inicio
    
FADE_ROW:
    CMP DX, 350
    JAE FADE_DELAY
    
    MOV CX, 0
FADE_COL:
    CMP CX, 640
    JAE SIGUIENTE_FADE_ROW
    
    ; Escribir pixel negro
    MOV AH, 0CH
    MOV AL, COLOR_NEGRO
    MOV BH, 0
    INT 10H
    
    ADD CX, 4           ; Saltos para acelerar
    JMP FADE_COL
    
SIGUIENTE_FADE_ROW:
    ADD DX, 4
    JMP FADE_ROW
    
FADE_DELAY:
    ; Pequeña pausa
    PUSH CX
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    POP CX
    
    POP CX
    LOOP FADE_OUT_LOOP
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
FADE_OUT_SIMPLE ENDP


; ========================================
; FADE IN SIMPLE (Aclarar pantalla)
; ========================================
FADE_IN_SIMPLE PROC
    PUSH AX
    PUSH CX
    PUSH DX
    
    ; Pausa corta para efecto de transición
    MOV CX, 0
    MOV DX, 8000
    MOV AH, 86H
    INT 15H
    
    POP DX
    POP CX
    POP AX
    RET
FADE_IN_SIMPLE ENDP


; ========================================
; GENERAR MAPA TIPO PRINCIPAL (Bosque)
; ========================================
GENERAR_MAPA_TIPO_PRINCIPAL PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH SI
    
    ; Llenar con pasto
    MOV SI, 0
    MOV CX, 800
LLENAR_PASTO_PRINCIPAL:
    MOV MAPA_DATOS[SI], 0
    INC SI
    LOOP LLENAR_PASTO_PRINCIPAL
    
    ; Obstáculos balanceados
    MOV CONTADOR_OBSTACULOS, 7
    CALL COLOCAR_OBSTACULOS_PIEDRA
    
    MOV CONTADOR_OBSTACULOS, 5
    CALL COLOCAR_OBSTACULOS_AGUA
    
    MOV CONTADOR_OBSTACULOS, 30
    CALL COLOCAR_HIERBA
    
    ; Crear portales visuales (columnas 0 y 39, filas 8-12)
    CALL MARCAR_PORTALES_MAPA
    
    POP SI
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_TIPO_PRINCIPAL ENDP


; ========================================
; GENERAR MAPA TIPO MONTAÑOSO (Rocoso)
; ========================================
GENERAR_MAPA_TIPO_MONTANOSO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH SI
    
    ; Llenar con pasto
    MOV SI, 0
    MOV CX, 800
LLENAR_PASTO_MONTANOSO:
    MOV MAPA_DATOS[SI], 0
    INC SI
    LOOP LLENAR_PASTO_MONTANOSO
    
    ; MUCHAS rocas (montañoso)
    MOV CONTADOR_OBSTACULOS, 25
    CALL COLOCAR_OBSTACULOS_PIEDRA
    
    ; Poca agua
    MOV CONTADOR_OBSTACULOS, 2
    CALL COLOCAR_OBSTACULOS_AGUA
    
    ; Menos hierba
    MOV CONTADOR_OBSTACULOS, 15
    CALL COLOCAR_HIERBA
    
    ; Crear portal visual (solo izquierda para regresar al principal)
    CALL MARCAR_PORTAL_IZQUIERDO
    
    POP SI
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_TIPO_MONTANOSO ENDP


; ========================================
; GENERAR MAPA TIPO DESÉRTICO (Arena)
; ========================================
GENERAR_MAPA_TIPO_DESERTICO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH SI
    
    ; Llenar con pasto (arena)
    MOV SI, 0
    MOV CX, 800
LLENAR_ARENA_DESERTICO:
    MOV MAPA_DATOS[SI], 0
    INC SI
    LOOP LLENAR_ARENA_DESERTICO
    
    ; Pocas rocas
    MOV CONTADOR_OBSTACULOS, 4
    CALL COLOCAR_OBSTACULOS_PIEDRA
    
    ; Casi nada de agua (desierto)
    MOV CONTADOR_OBSTACULOS, 1
    CALL COLOCAR_OBSTACULOS_AGUA
    
    ; Poca vegetación
    MOV CONTADOR_OBSTACULOS, 8
    CALL COLOCAR_HIERBA
    
    ; Crear portal visual (solo derecha para regresar al principal)
    CALL MARCAR_PORTAL_DERECHO
    
    POP SI
    POP CX
    POP BX
    POP AX
    RET
GENERAR_MAPA_TIPO_DESERTICO ENDP


; ========================================
; MARCAR PORTALES EN EL MAPA (ambos lados)
; ========================================
MARCAR_PORTALES_MAPA PROC
    CALL MARCAR_PORTAL_IZQUIERDO
    CALL MARCAR_PORTAL_DERECHO
    RET
MARCAR_PORTALES_MAPA ENDP


; ========================================
; MARCAR PORTAL IZQUIERDO (filas 8-12, col 0)
; ========================================
MARCAR_PORTAL_IZQUIERDO PROC
    PUSH SI
    PUSH CX
    
    ; Marcar la columna 0 como portal (filas 8-12)
    ; Fila 10 (centro) = tile de portal tipo 5
    MOV SI, 400         ; Fila 10 * 40 + 0
    MOV MAPA_DATOS[SI], 5       ; Portal brillante
    
    ; Filas 9 y 11 también portal
    MOV SI, 360         ; Fila 9 * 40
    MOV MAPA_DATOS[SI], 5
    
    MOV SI, 440         ; Fila 11 * 40
    MOV MAPA_DATOS[SI], 5
    
    ; Filas 8 y 12 pasto para transición
    MOV SI, 320         ; Fila 8 * 40
    MOV MAPA_DATOS[SI], 0
    
    MOV SI, 480         ; Fila 12 * 40
    MOV MAPA_DATOS[SI], 0
    
    ; Marco de árboles arriba y abajo
    MOV SI, 280         ; Fila 7 * 40
    MOV MAPA_DATOS[SI], 3
    
    MOV SI, 520         ; Fila 13 * 40
    MOV MAPA_DATOS[SI], 3
    
    POP CX
    POP SI
    RET
MARCAR_PORTAL_IZQUIERDO ENDP


; ========================================
; MARCAR PORTAL DERECHO (filas 8-12, col 39)
; ========================================
MARCAR_PORTAL_DERECHO PROC
    PUSH SI
    PUSH CX
    
    ; Marcar la columna 39 como portal (filas 8-12)
    ; Fila 10 (centro) = tile de portal tipo 5
    MOV SI, 439         ; Fila 10 * 40 + 39
    MOV MAPA_DATOS[SI], 5       ; Portal brillante
    
    ; Filas 9 y 11 también portal
    MOV SI, 399         ; Fila 9 * 40 + 39
    MOV MAPA_DATOS[SI], 5
    
    MOV SI, 479         ; Fila 11 * 40 + 39
    MOV MAPA_DATOS[SI], 5
    
    ; Filas 8 y 12 pasto para transición
    MOV SI, 359         ; Fila 8 * 40 + 39
    MOV MAPA_DATOS[SI], 0
    
    MOV SI, 519         ; Fila 12 * 40 + 39
    MOV MAPA_DATOS[SI], 0
    
    ; Marco de árboles arriba y abajo
    MOV SI, 319         ; Fila 7 * 40 + 39
    MOV MAPA_DATOS[SI], 3
    
    MOV SI, 559         ; Fila 13 * 40 + 39
    MOV MAPA_DATOS[SI], 3
    
    POP CX
    POP SI
    RET
MARCAR_PORTAL_DERECHO ENDP


; ========================================
; COPIAR MAPA_DATOS A BUFFER PRINCIPAL
; ========================================
COPIAR_MAPA_DATOS_A_PRINCIPAL PROC
    PUSH SI
    PUSH DI
    PUSH CX
    PUSH ES
    PUSH DS
    POP ES
    
    LEA SI, MAPA_DATOS
    LEA DI, MAPA_PRINCIPAL_DATOS
    MOV CX, 800
    REP MOVSB
    
    POP ES
    POP CX
    POP DI
    POP SI
    RET
COPIAR_MAPA_DATOS_A_PRINCIPAL ENDP

; ========================================
; COPIAR BUFFER PRINCIPAL A MAPA_DATOS
; ========================================
COPIAR_PRINCIPAL_A_MAPA_DATOS PROC
    PUSH SI
    PUSH DI
    PUSH CX
    PUSH ES
    PUSH DS
    POP ES
    
    LEA SI, MAPA_PRINCIPAL_DATOS
    LEA DI, MAPA_DATOS
    MOV CX, 800
    REP MOVSB
    
    POP ES
    POP CX
    POP DI
    POP SI
    RET
COPIAR_PRINCIPAL_A_MAPA_DATOS ENDP

; ========================================
; COPIAR MAPA_DATOS A BUFFER MONTAÑOSO
; ========================================
COPIAR_MAPA_DATOS_A_MONTANOSO PROC
    PUSH SI
    PUSH DI
    PUSH CX
    PUSH ES
    PUSH DS
    POP ES
    
    LEA SI, MAPA_DATOS
    LEA DI, MAPA_MONTANOSO_DATOS
    MOV CX, 800
    REP MOVSB
    
    POP ES
    POP CX
    POP DI
    POP SI
    RET
COPIAR_MAPA_DATOS_A_MONTANOSO ENDP

; ========================================
; COPIAR BUFFER MONTAÑOSO A MAPA_DATOS
; ========================================
COPIAR_MONTANOSO_A_MAPA_DATOS PROC
    PUSH SI
    PUSH DI
    PUSH CX
    PUSH ES
    PUSH DS
    POP ES
    
    LEA SI, MAPA_MONTANOSO_DATOS
    LEA DI, MAPA_DATOS
    MOV CX, 800
    REP MOVSB
    
    POP ES
    POP CX
    POP DI
    POP SI
    RET
COPIAR_MONTANOSO_A_MAPA_DATOS ENDP

; ========================================
; COPIAR MAPA_DATOS A BUFFER DESÉRTICO
; ========================================
COPIAR_MAPA_DATOS_A_DESERTICO PROC
    PUSH SI
    PUSH DI
    PUSH CX
    PUSH ES
    PUSH DS
    POP ES
    
    LEA SI, MAPA_DATOS
    LEA DI, MAPA_DESERTICO_DATOS
    MOV CX, 800
    REP MOVSB
    
    POP ES
    POP CX
    POP DI
    POP SI
    RET
COPIAR_MAPA_DATOS_A_DESERTICO ENDP

; ========================================
; COPIAR BUFFER DESÉRTICO A MAPA_DATOS
; ========================================
COPIAR_DESERTICO_A_MAPA_DATOS PROC
    PUSH SI
    PUSH DI
    PUSH CX
    PUSH ES
    PUSH DS
    POP ES
    
    LEA SI, MAPA_DESERTICO_DATOS
    LEA DI, MAPA_DATOS
    MOV CX, 800
    REP MOVSB
    
    POP ES
    POP CX
    POP DI
    POP SI
    RET
COPIAR_DESERTICO_A_MAPA_DATOS ENDP


; ========================================
; ACTUALIZAR COLOR DEL SUELO SEGÚN MAPA
; ========================================
ACTUALIZAR_COLOR_SUELO PROC
    PUSH AX
    
    MOV AL, MAPA_ACTUAL
    
    CMP AL, MAPA_PRINCIPAL
    JE COLOR_BOSQUE
    
    CMP AL, MAPA_MONTANOSO
    JE COLOR_MONTANA
    
    CMP AL, MAPA_DESERTICO
    JE COLOR_DESIERTO
    
    JMP FIN_COLOR_SUELO
    
COLOR_BOSQUE:
    MOV AL, 2               ; Verde pastoso brillante (pasto fresco)
    MOV COLOR_SUELO_ACTUAL, AL
    JMP FIN_COLOR_SUELO
    
COLOR_MONTANA:
    MOV AL, 8               ; Gris lodoso/rocoso (tierra húmeda con rocas)
    MOV COLOR_SUELO_ACTUAL, AL
    JMP FIN_COLOR_SUELO
    
COLOR_DESIERTO:
    MOV AL, 6               ; Marrón arenoso (arena/tierra seca)
    MOV COLOR_SUELO_ACTUAL, AL
    
FIN_COLOR_SUELO:
    POP AX
    RET
ACTUALIZAR_COLOR_SUELO ENDP


; ========================================
; NÚMERO ALEATORIO 0-99
; Retorna en AL
; ========================================
RANDOM_100 PROC
    PUSH BX
    PUSH DX
    
    ; Obtener tiempo del sistema
    MOV AH, 2CH
    INT 21H
    
    ; DL = centésimas, DH = segundos
    MOV AL, DL
    ADD AL, DH
    
    ; Módulo 100
    MOV BL, 100
    DIV BL
    MOV AL, AH      ; Resto en AH
    
    POP DX
    POP BX
    RET
RANDOM_100 ENDP
