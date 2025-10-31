
; RECURSOS E INVENTARIO 

.DATA
    ; Recursos (X, Y, Tipo, Activo) - cada recurso usa 8 bytes
    NUM_RECURSOS EQU 15
    RECURSOS DW 15 DUP(0, 0, 0, 1)  ; X, Y, Tipo, Activo
    
    ; Inventario
    INVENTARIO DB 3 DUP(0)
    OBJETIVO DB 2, 2, 2

.CODE

; CARGAR RECURSOS (ahora aleatorio)

CARGAR_RECURSOS PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    MOV SI, 0               ; Índice en array de recursos
    MOV DI, 0               ; Contador de recursos colocados
    
    ; Colocar 5 minerales (tipo 1)
    MOV CX, 5
COLOCAR_MINERALES:
    PUSH CX
    MOV BX, 1               ; Tipo mineral
    CALL GENERAR_POSICION_RECURSO
    POP CX
    LOOP COLOCAR_MINERALES
    
    ; Colocar 5 maderas (tipo 2)
    MOV CX, 5
COLOCAR_MADERAS:
    PUSH CX
    MOV BX, 2               ; Tipo madera
    CALL GENERAR_POSICION_RECURSO
    POP CX
    LOOP COLOCAR_MADERAS
    
    ; Colocar 5 frutas (tipo 3)
    MOV CX, 5
COLOCAR_FRUTAS:
    PUSH CX
    MOV BX, 3               ; Tipo fruta
    CALL GENERAR_POSICION_RECURSO
    POP CX
    LOOP COLOCAR_FRUTAS
    
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
CARGAR_RECURSOS ENDP


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
    
    ; Generar Y aleatorio (16 a 320 para 336 pixels)
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV CX, 304             ; 320-16
    DIV CX
    ADD DX, 16
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
    ADD AX, 8
    XOR DX, DX
    DIV CX
    MOV DX, AX              ; DX = fila
    
    ; Calcular índice en el mapa
    MOV AX, DX
    MOV CX, 40              ; 40 columnas
    MUL CX
    ADD AX, BX
    
    ; Verificar límites
    CMP AX, 840
    JAE POSICION_INVALIDA
    
    MOV BX, AX
    
    ; Verificar que sea tierra (0) o hierba (3)
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