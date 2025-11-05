; MANEJO DE MAPAS - MAP.INC


.DATA
    ; Mapa (ahora 40x20 = 800 bytes para caber con HUD de 25px)
    MAPA_ANCHO DW ?
    MAPA_ALTO DW ?
    MAPA_DATOS DB 800 DUP(?)
    MAPA_CARGADO DB 0   
    NOMBRE_ARCHIVO DB 'mapa.txt', 0
    CARACTER_TEMP DB ?
    
    ; Viewport
    VIEWPORT_X DW 0
    VIEWPORT_Y DW 0
    
    ; Generación aleatoria
    SEMILLA DW 0
    CONTADOR_OBSTACULOS DW 0

.CODE

; CARGAR MAPA 

CARGAR_MAPA PROC
    ; Verificar si el mapa ya fue generado
    CMP MAPA_CARGADO, 1
    JE MAPA_YA_EXISTE
    
    ; Verificar si ya se inicializaron los mapas múltiples
    CMP MAPA_PRINCIPAL_GENERADO, 1
    JE MAPAS_MULTI_YA_LISTOS
    
    ; Establecer dimensiones para modo EGA 640x350
    MOV MAPA_ANCHO, 40
    MOV MAPA_ALTO, 20
    
    ; Inicializar semilla aleatoria con el reloj del sistema
    MOV AH, 2CH
    INT 21H
    MOV AL, DL      ; Centésimas de segundo
    MOV AH, DH      ; Segundos
    ADD AX, CX      ; Agregar minutos y horas
    MOV SEMILLA, AX
    
    ; Generar mapa aleatorio
    CALL GENERAR_MAPA_ALEATORIO
    
MAPAS_MULTI_YA_LISTOS:
    ; Marcar como cargado
    MOV BYTE PTR MAPA_CARGADO, 1
    
MAPA_YA_EXISTE:
    RET
CARGAR_MAPA ENDP


; LEER MAPA DESDE ARCHIVO

LEER_MAPA_DESDE_ARCHIVO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; Abrir archivo
    MOV AH, 3DH
    MOV AL, 0           ; Modo lectura
    LEA DX, NOMBRE_ARCHIVO
    INT 21H
    JC ERROR_ABRIR
    
    MOV BX, AX          ; Handle del archivo
    
    ; Saltar primera línea
SALTAR_PRIMERA_LINEA:
    MOV AH, 3FH
    MOV CX, 1
    LEA DX, CARACTER_TEMP
    INT 21H
    JC ERROR_LEER
    
    CMP CARACTER_TEMP, 10   ; Salto de línea
    JNE SALTAR_PRIMERA_LINEA
    
    ; Leer mapa línea por línea
    MOV SI, 0           
    
LEER_LINEA:
    CMP SI, 300         
    JAE FIN_LECTURA
    
LEER_CARACTER:
    MOV AH, 3FH
    MOV CX, 1
    LEA DX, CARACTER_TEMP
    INT 21H
    JC ERROR_LEER
    
    ; Verificar fin de archivo
    CMP AX, 0
    JE FIN_LECTURA
    
    ; Si es espacio o salto de línea, ignorar
    CMP CARACTER_TEMP, ' '
    JE LEER_CARACTER
    CMP CARACTER_TEMP, 10
    JE LEER_CARACTER
    CMP CARACTER_TEMP, 13
    JE LEER_CARACTER
    
    ; Si es 'R', terminamos el mapa
    CMP CARACTER_TEMP, 'R'
    JE FIN_LECTURA
    
    ; Convertir ASCII a número (0-3)
    MOV AL, CARACTER_TEMP
    SUB AL, '0'
    
    ; Guardar en mapa
    MOV MAPA_DATOS[SI], AL
    INC SI
    JMP LEER_CARACTER
    
FIN_LECTURA:
    ; Cerrar archivo
    MOV AH, 3EH
    INT 21H
    
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
    
ERROR_ABRIR:
ERROR_LEER:
    ; Si hay error, llenar con tierra
    MOV SI, 0
    MOV CX, 300
LLENAR_TIERRA_ERROR:
    MOV MAPA_DATOS[SI], 0
    INC SI
    LOOP LLENAR_TIERRA_ERROR
    
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
LEER_MAPA_DESDE_ARCHIVO ENDP


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
    
    ; 2. NO crear bordes 
    ; CALL CREAR_BORDES 
    
    ; 3. Colocar menos obstáculos de piedra
    MOV CONTADOR_OBSTACULOS, 7      
    CALL COLOCAR_OBSTACULOS_PIEDRA
    
    ; 4. Colocar menos obstáculos de agua
    MOV CONTADOR_OBSTACULOS, 4     
    CALL COLOCAR_OBSTACULOS_AGUA
    
    ; 5. Colocar hierba decorativa
    MOV CONTADOR_OBSTACULOS, 35     
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
    MOV CX, 40          
BORDE_SUP:
    MOV MAPA_DATOS[SI], 1
    INC SI
    LOOP BORDE_SUP
    
    ; Borde inferior
    MOV SI, 800        
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


; VERIFICAR SI POSICIÓN ESTÁ EN ZONA DE PORTAL
; Entrada: SI = índice en MAPA_DATOS (0-799)
; Salida: AL = 1 si está en zona de portal, 0 si no
VERIFICAR_ZONA_PORTAL PROC
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Convertir índice a columna y fila
    MOV AX, SI
    MOV BX, 40
    XOR DX, DX
    DIV BX
    ; AX = fila, DX = columna
    
    ; Verificar si está en filas 7-13
    CMP AX, 7
    JB NO_ES_ZONA_PORTAL
    CMP AX, 13
    JA NO_ES_ZONA_PORTAL
    
    ; Verificar portal izquierdo
    CMP DX, 4
    JB ES_ZONA_PORTAL
    
    ; Verificar portal derecho
    CMP DX, 36
    JAE ES_ZONA_PORTAL
    
NO_ES_ZONA_PORTAL:
    MOV AL, 0
    JMP FIN_VERIFICAR_ZONA_PORTAL
    
ES_ZONA_PORTAL:
    MOV AL, 1
    
FIN_VERIFICAR_ZONA_PORTAL:
    POP DX
    POP CX
    POP BX
    RET
VERIFICAR_ZONA_PORTAL ENDP


; COLOCAR OBSTÁCULOS DE PIEDRA

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
    
    ; Generar posición aleatoria
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 38          ; Ancho útil
    DIV BX
    ADD DX, 1           
    PUSH DX             
    
    CALL NUMERO_ALEATORIO
    MOV DX, 0
    MOV BX, 19          ; Alto útil (21-2)
    DIV BX
    ADD DX, 1           
    MOV BX, DX          
    
    ; Calcular posición en el mapa
    MOV AX, BX
    MOV CX, 40          ; 40 columnas
    MUL CX
    POP DX
    ADD AX, DX
    MOV SI, AX
    
    ; Verificar que no esté muy cerca del centro
    CMP SI, 400
    JB POSICION_OK_PIEDRA_1
    CMP SI, 450
    JA POSICION_OK_PIEDRA_1
    JMP SIGUIENTE_GRUPO_PIEDRA      ; Muy cerca del jugador
    
POSICION_OK_PIEDRA_1:
    ; Verificar que no esté en zona de portal
    CALL VERIFICAR_ZONA_PORTAL
    CMP AL, 1
    JE SIGUIENTE_GRUPO_PIEDRA       ; Está en zona de portal, buscar otra posición
    
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
    ADD SI, 40          ; Siguiente fila
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


; COLOCAR OBSTÁCULOS DE AGUA

COLOCAR_OBSTACULOS_AGUA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
SIGUIENTE_GRUPO_AGUA:
    CMP CONTADOR_OBSTACULOS, 0
    JNE CONTINUAR_AGUA
    JMP FIN_AGUA
CONTINUAR_AGUA:
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
    MOV BX, 19          ; Alto útil (21-2)
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
    JB POSICION_OK_AGUA_1
    CMP SI, 450
    JA POSICION_OK_AGUA_1
    JMP SIGUIENTE_GRUPO_AGUA
    
POSICION_OK_AGUA_1:
    ; Verificar que no esté en zona de portal
    CALL VERIFICAR_ZONA_PORTAL
    CMP AL, 1
    JE SIGUIENTE_GRUPO_AGUA
    
POSICION_OK_AGUA:
    ; Colocar grupo pequeño de agua
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
    
    JMP SHORT VOLVER_GRUPO_AGUA
    
VOLVER_GRUPO_AGUA:
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
    MOV BX, 19          ; Alto útil (21-2)
    DIV BX
    ADD DX, 1
    MOV BX, DX
    
    MOV AX, BX
    MOV CX, 40          ; 40 columnas
    MUL CX
    POP DX
    ADD AX, DX
    MOV SI, AX
    
    ; Verificar que no esté en zona de portal
    CALL VERIFICAR_ZONA_PORTAL
    CMP AL, 1
    JE SIGUIENTE_HIERBA
    
    ; Colocar solo si es tierra
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