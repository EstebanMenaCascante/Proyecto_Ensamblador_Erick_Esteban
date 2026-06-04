; HUD (HEADS-UP DISPLAY) - INTERFAZ DE USUARIO EN JUEGO

.DATA
    ; Sistema de puntuación
    PUNTUACION_HUD DW 0
    PUNTOS_MINERAL_HUD EQU 10
    PUNTOS_MADERA_HUD EQU 15
    PUNTOS_FRUTA_HUD EQU 20
    
    ; Textos para HUD
    TXT_MINERAL DB 'Piedra:', '$'
    TXT_MADERA DB 'Madera:', '$'
    TXT_FRUTA DB 'Fruta:', '$'
    TXT_TOTAL DB 'Total:', '$'
    TXT_PUNTOS DB 'Puntos:', '$'
    TXT_INVENTARIO_TITLE DB 'INVENTARIO DE RECURSOS', '$'
    TXT_PRESIONA_E DB 'Presiona E para ver inventario', '$'
    TXT_CERRAR_INV DB 'Presiona E para cerrar', '$'
    TXT_BARRA DB '/', '$'
    
    ; Textos cortos para HUD horizontal
    TXT_P DB 'P:', '$'
    TXT_M DB 'M:', '$'
    TXT_F DB 'F:', '$'
    TXT_T DB 'T:', '$'
    TXT_BARRA_15 DB '/15', '$'
    TXT_BARRA_45 DB '/45', '$'
    NUMERO_STR_HUD DB '00', '$'
    
    ; Estado del inventario
    INVENTARIO_ABIERTO DB 0

.CODE

; DIBUJAR HUD COMPLETO EN BARRA HORIZONTAL SUPERIOR

DIBUJAR_HUD_COMPLETO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; Dibujar barra horizontal en la parte superior
    ; Barra de 640x25 pixels
    
    ; Dibujar fondo de la barra
    MOV DX, 0           
    MOV BL, 0          
    
FILA_BARRA_HUD:
    CMP BL, 25
    JAE FIN_BARRA_HUD
    
    MOV CX, 0          
    PUSH BX
    MOV BX, 0           
    
COLUMNA_BARRA_HUD:
    CMP BX, 640
    JAE FIN_FILA_BARRA_HUD
    
    MOV AL, 0           
    CALL ESCRIBIR_PIXEL
    
    INC CX
    INC BX
    JMP COLUMNA_BARRA_HUD
    
FIN_FILA_BARRA_HUD:
    POP BX
    INC DX
    INC BL
    JMP FILA_BARRA_HUD
    
FIN_BARRA_HUD:
    
    ; Dibujar contadores de recursos horizontalmente
    CALL DIBUJAR_CONTADOR_RECURSOS
    
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_HUD_COMPLETO ENDP


; DIBUJAR CONTADOR DE RECURSOS EN EL HUD HORIZONTAL

DIBUJAR_CONTADOR_RECURSOS PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    
    ; Guardar modo de video y cambiar temporalmente a texto
    PUSH AX
    MOV AH, 0FH
    INT 10H             ; Obtener modo de video actual
    POP AX
    
    ; Posicionar cursor y escribir en modo texto sobre gráficos
    
    MOV CX, 10
    MOV DX, 8
    MOV AL, 7           ; Gris claro
    MOV BL, 10
    CALL DIBUJAR_CUADRADO_SIMPLE
    
    ; Texto "P:" en posición (3,0)
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 15          
    MOV CX, 2
    MOV DH, 0
    MOV DL, 3
    PUSH DS
    POP ES
    LEA BP, TXT_P
    INT 10H
    
    ; Número actual de piedras
    MOV SI, 0
    MOV AL, INVENTARIO[SI]
    MOV DL, 5
    CALL MOSTRAR_NUMERO_HUD
    
    ; Texto "/15"
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 14          
    MOV CX, 3
    MOV DH, 0
    MOV DL, 7
    PUSH DS
    POP ES
    LEA BP, TXT_BARRA_15
    INT 10H
    
    ;MADERA 
    MOV CX, 90
    MOV DX, 8
    MOV AL, 11          
    MOV BL, 10
    CALL DIBUJAR_CUADRADO_SIMPLE
    
    ; Texto "M:"
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 15
    MOV CX, 2
    MOV DH, 0
    MOV DL, 13
    PUSH DS
    POP ES
    LEA BP, TXT_M
    INT 10H
    
    ; Número de maderas
    MOV SI, 1
    MOV AL, INVENTARIO[SI]
    MOV DL, 15
    CALL MOSTRAR_NUMERO_HUD
    
    ; Texto "/15"
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 14
    MOV CX, 3
    MOV DH, 0
    MOV DL, 17
    PUSH DS
    POP ES
    LEA BP, TXT_BARRA_15
    INT 10H
    
    ;FRUTA
    MOV CX, 170
    MOV DX, 8
    MOV AL, 12        
    MOV BL, 10
    CALL DIBUJAR_CUADRADO_SIMPLE
    
    ; Texto "F:"
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 15
    MOV CX, 2
    MOV DH, 0
    MOV DL, 23
    PUSH DS
    POP ES
    LEA BP, TXT_F
    INT 10H
    
    ; Número de frutas
    MOV SI, 2
    MOV AL, INVENTARIO[SI]
    MOV DL, 25
    CALL MOSTRAR_NUMERO_HUD
    
    ; Texto "/15"
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 14
    MOV CX, 3
    MOV DH, 0
    MOV DL, 27
    PUSH DS
    POP ES
    LEA BP, TXT_BARRA_15
    INT 10H
    
    ;TOTAL
    MOV AL, INVENTARIO[0]
    ADD AL, INVENTARIO[1]
    ADD AL, INVENTARIO[2]
    
    
    MOV CX, 250
    MOV DX, 8
    PUSH AX
    MOV AL, 14          
    MOV BL, 10
    CALL DIBUJAR_CUADRADO_SIMPLE
    POP AX
    
    ; Texto "T:"
    MOV AH, 13H
    PUSH AX
    MOV AL, 1
    MOV BH, 0
    MOV BL, 15
    MOV CX, 2
    MOV DH, 0
    MOV DL, 33
    PUSH DS
    POP ES
    LEA BP, TXT_T
    INT 10H
    POP AX
    
    ; Número total
    MOV DL, 35
    CALL MOSTRAR_NUMERO_HUD
    
    ; Texto "/45"
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 14
    MOV CX, 3
    MOV DH, 0
    MOV DL, 37
    PUSH DS
    POP ES
    LEA BP, TXT_BARRA_45
    INT 10H
    
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_CONTADOR_RECURSOS ENDP


; MOSTRAR NÚMERO EN HUD (0-99)
; AL = número, DL = columna

MOSTRAR_NUMERO_HUD PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Convertir número a ASCII
    XOR AH, AH
    MOV BL, 10
    DIV BL              ; AL = decenas, AH = unidades
    
    ; Si es mayor o igual a 10, mostrar decena
    CMP AL, 0
    JE SOLO_UNIDAD_HUD
    
    ; Mostrar decena
    ADD AL, '0'
    MOV NUMERO_STR_HUD[0], AL
    MOV AL, AH
    ADD AL, '0'
    MOV NUMERO_STR_HUD[1], AL
    
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 15         
    MOV CX, 2
    MOV DH, 0
    ; DL ya tiene la columna
    PUSH DS
    POP ES
    LEA BP, NUMERO_STR_HUD
    INT 10H
    JMP FIN_MOSTRAR_NUM_HUD
    
SOLO_UNIDAD_HUD:
    ; Solo mostrar unidad
    MOV AL, AH
    ADD AL, '0'
    MOV NUMERO_STR_HUD[0], AL
    
    MOV AH, 13H
    MOV AL, 1
    MOV BH, 0
    MOV BL, 15
    MOV CX, 1
    MOV DH, 0
    ; DL ya tiene la columna
    PUSH DS
    POP ES
    LEA BP, NUMERO_STR_HUD
    INT 10H
    
FIN_MOSTRAR_NUM_HUD:
    POP DX
    POP CX
    POP BX
    POP AX
    RET
MOSTRAR_NUMERO_HUD ENDP


; DIBUJAR UN CUADRADO SIMPLE
; CX = X, DX = Y, AL = Color, BL = Tamaño

DIBUJAR_CUADRADO_SIMPLE PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    PUSH SI
    PUSH DI
    
    MOV AH, AL          ; Guardar color en AH
    XOR BH, BH          ; BX = tamaño
    MOV SI, 0           ; Contador fila
    MOV DI, DX          ; Y inicial guardado
    
FILA_CUAD:
    CMP SI, BX
    JAE FIN_CUAD
    
    PUSH CX             ; Guardar X inicial
    PUSH SI
    MOV SI, 0           ; Contador columna
    
COLUMNA_CUAD:
    CMP SI, BX
    JAE FIN_FILA_CUAD
    
    MOV AL, AH          ; Restaurar color
    MOV DX, DI          ; Y actual
    CALL ESCRIBIR_PIXEL
    
    INC CX
    INC SI
    JMP COLUMNA_CUAD
    
FIN_FILA_CUAD:
    POP SI
    POP CX              ; Restaurar X inicial
    INC DI
    INC SI
    JMP FILA_CUAD
    
FIN_CUAD:
    POP DI
    POP SI
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_CUADRADO_SIMPLE ENDP


; DIBUJAR NÚMERO SIMPLE (0-9)
; AL = número, CX = X, DX = Y

DIBUJAR_NUMERO_SIMPLE PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Cada número es un conjunto de pixels
    
    CMP AL, 0
    JE DIBUJAR_0
    CMP AL, 1
    JE DIBUJAR_1
    CMP AL, 2
    JE DIBUJAR_2
    CMP AL, 3
    JE DIBUJAR_3
    CMP AL, 4
    JE DIBUJAR_4
    CMP AL, 5
    JE DIBUJAR_5
    JMP FIN_NUMERO_S
    
DIBUJAR_0:
    MOV AL, 15
    MOV BL, 5
    CALL DIBUJAR_CUADRADO_SIMPLE
    JMP FIN_NUMERO_S
    
DIBUJAR_1:
    MOV AL, 15
    MOV BL, 3
    CALL DIBUJAR_CUADRADO_SIMPLE
    JMP FIN_NUMERO_S
    
DIBUJAR_2:
    MOV AL, 15
    MOV BL, 5
    CALL DIBUJAR_CUADRADO_SIMPLE
    JMP FIN_NUMERO_S
    
DIBUJAR_3:
    MOV AL, 15
    MOV BL, 5
    CALL DIBUJAR_CUADRADO_SIMPLE
    JMP FIN_NUMERO_S
    
DIBUJAR_4:
    MOV AL, 15
    MOV BL, 5
    CALL DIBUJAR_CUADRADO_SIMPLE
    JMP FIN_NUMERO_S
    
DIBUJAR_5:
    MOV AL, 15
    MOV BL, 5
    CALL DIBUJAR_CUADRADO_SIMPLE
    
FIN_NUMERO_S:
    POP DX
    POP CX
    POP BX
    POP AX
    RET
DIBUJAR_NUMERO_SIMPLE ENDP


; DIBUJAR PIXEL BLANCO

DIBUJAR_PIXEL_BLANCO PROC
    PUSH AX
    MOV AL, 15
    CALL ESCRIBIR_PIXEL
    POP AX
    RET
DIBUJAR_PIXEL_BLANCO ENDP




; AGREGAR PUNTOS AL RECOLECTAR RECURSO
; BL = tipo de recurso (1=mineral, 2=madera, 3=fruta)

AGREGAR_PUNTOS_HUD PROC
    PUSH AX
    
    CMP BL, 1
    JE PUNTOS_MIN_ADD
    CMP BL, 2
    JE PUNTOS_MAD_ADD
    CMP BL, 3
    JE PUNTOS_FRU_ADD
    JMP FIN_AGREGAR_PTS
    
PUNTOS_MIN_ADD:
    MOV AX, PUNTUACION_HUD
    ADD AX, PUNTOS_MINERAL_HUD
    MOV PUNTUACION_HUD, AX
    JMP FIN_AGREGAR_PTS
    
PUNTOS_MAD_ADD:
    MOV AX, PUNTUACION_HUD
    ADD AX, PUNTOS_MADERA_HUD
    MOV PUNTUACION_HUD, AX
    JMP FIN_AGREGAR_PTS
    
PUNTOS_FRU_ADD:
    MOV AX, PUNTUACION_HUD
    ADD AX, PUNTOS_FRUTA_HUD
    MOV PUNTUACION_HUD, AX
    
FIN_AGREGAR_PTS:
    POP AX
    RET
AGREGAR_PUNTOS_HUD ENDP


; MOSTRAR INVENTARIO COMPLETO (Pantalla con E)

MOSTRAR_INVENTARIO_COMPLETO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Cambiar a modo texto temporalmente
    MOV AX, 0003H
    INT 10H
    
    
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 1FH        
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ; Título "INVENTARIO DE RECURSOS"
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 3
    MOV DL, 28
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_INVENTARIO_TITLE
    INT 21H
    
    
    MOV AH, 02H
    MOV DH, 4
    MOV DL, 20
    INT 10H
    MOV CX, 40
LINEA_SEP:
    MOV AH, 02H
    MOV DL, '='
    INT 21H
    LOOP LINEA_SEP
    
    ;PIEDRA
    MOV AH, 02H
    MOV DH, 7
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_MINERAL
    INT 21H
    
    ; Cantidad
    MOV AL, INVENTARIO[0]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS_INV
    
    ; Barra
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    
    ; Total (15)
    MOV DL, '1'
    MOV AH, 02H
    INT 21H
    MOV DL, '5'
    MOV AH, 02H
    INT 21H
    
    ;MADERA
    MOV AH, 02H
    MOV DH, 10
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_MADERA
    INT 21H
    
    ; Cantidad
    MOV AL, INVENTARIO[1]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS_INV
    
    ; Barra
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    
    ; Total (15)
    MOV DL, '1'
    MOV AH, 02H
    INT 21H
    MOV DL, '5'
    MOV AH, 02H
    INT 21H
    
    ; === FRUTA ===
    MOV AH, 02H
    MOV DH, 13
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_FRUTA
    INT 21H
    
    ; Cantidad
    MOV AL, INVENTARIO[2]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS_INV
    
    ; Barra
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    
    ; Total (15)
    MOV DL, '1'
    MOV AH, 02H
    INT 21H
    MOV DL, '5'
    MOV AH, 02H
    INT 21H
    
    ;TOTAL RECOLECTADO
    MOV AH, 02H
    MOV DH, 16
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_TOTAL
    INT 21H
    
    ; Calcular total
    MOV AL, INVENTARIO[0]
    ADD AL, INVENTARIO[1]
    ADD AL, INVENTARIO[2]
    
    ; Mostrar total (0-45)
    CALL MOSTRAR_NUMERO_DOS_DIGITOS_INV
    
    ; Barra
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    
    ; Total objetivo (45)
    MOV DL, '4'
    MOV AH, 02H
    INT 21H
    MOV DL, '5'
    MOV AH, 02H
    INT 21H
    
    ;PUNTUACIÓN
    MOV AH, 02H
    MOV DH, 19
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_PUNTOS
    INT 21H
    
    ; Mostrar puntuación
    MOV AX, PUNTUACION_HUD
    CALL MOSTRAR_NUMERO_INVENTARIO
    
    ; Mensaje para cerrar
    MOV AH, 02H
    MOV DH, 22
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, TXT_CERRAR_INV
    INT 21H
    
    ; Esperar tecla E
ESPERAR_E:
    MOV AH, 00H
    INT 16H
    CMP AH, 12H         ; Scan code de E
    JNE ESPERAR_E
    
    ; Volver a modo gráfico
    CALL INICIAR_MODO_GRAFICO
    CALL DIBUJAR_FRAME_COMPLETO
    CALL DIBUJAR_HUD_COMPLETO
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
MOSTRAR_INVENTARIO_COMPLETO ENDP


; MOSTRAR NÚMERO EN INVENTARIO (convierte AX a string)

MOSTRAR_NUMERO_INVENTARIO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    MOV CX, 0
    MOV BX, 10
    
DIVIDIR_NUM:
    MOV DX, 0
    DIV BX
    PUSH DX
    INC CX
    CMP AX, 0
    JNE DIVIDIR_NUM
    
MOSTRAR_DIGITO:
    POP DX
    ADD DL, '0'
    MOV AH, 02H
    INT 21H
    LOOP MOSTRAR_DIGITO
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
MOSTRAR_NUMERO_INVENTARIO ENDP


; MOSTRAR NÚMERO DE DOS DÍGITOS EN INVENTARIO (0-99)
; AL = número a mostrar

MOSTRAR_NUMERO_DOS_DIGITOS_INV PROC
    PUSH AX
    PUSH BX
    PUSH DX
    
    XOR AH, AH          ; AX = número
    MOV BL, 10
    DIV BL              ; AH = unidades, AL = decenas
    
    MOV BL, AH          ; Guardar unidades en BL
    
    ; Mostrar decenas (si es 0, mostrar espacio o 0)
    CMP AL, 0
    JE SOLO_UNIDAD_INV
    
    ADD AL, '0'
    MOV DL, AL
    MOV AH, 02H
    INT 21H
    JMP MOSTRAR_UNIDAD_INV
    
SOLO_UNIDAD_INV:
    ; Si no hay decenas, mostrar espacio
    MOV DL, ' '
    MOV AH, 02H
    INT 21H
    
MOSTRAR_UNIDAD_INV:
    ; Mostrar unidades
    MOV AL, BL
    ADD AL, '0'
    MOV DL, AL
    MOV AH, 02H
    INT 21H
    
    POP DX
    POP BX
    POP AX
    RET
MOSTRAR_NUMERO_DOS_DIGITOS_INV ENDP
