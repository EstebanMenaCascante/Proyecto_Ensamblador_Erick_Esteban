; PANTALLA DE INICIO Y MENU - MENU.INC

.DATA
    TITULO_LINEA1 DB '  _____ _    _ ______ _____ ____  ', '$'
    TITULO_LINEA2 DB ' / ____| |  | |  ____/ ____|  _ \ ', '$'
    TITULO_LINEA3 DB '| |  __| |  | | |__ | |  __| |_) |', '$'
    TITULO_LINEA4 DB '| | |_ | |  | |  __|| | |_ |  _ < ', '$'
    TITULO_LINEA5 DB '| |__| | |__| | |___| |__| | |_) |', '$'
    TITULO_LINEA6 DB ' \_____|\____/|______\_____|____/ ', '$'
    
    SUBTITULO DB 'JUEGO DE EXPLORACION Y RECURSOS', '$'
    OPCION_INICIO DB 'PRESIONA CUALQUIER TECLA PARA COMENZAR', '$'
    INSTRUCCION1 DB 'CONTROLES:', '$'
    INSTRUCCION2 DB 'W A S D - Movimiento', '$'
    INSTRUCCION3 DB 'E - Ver inventario', '$'
    INSTRUCCION4 DB 'ESC - Salir del juego', '$'
    INSTRUCCION5 DB 'OBJETIVO: Recolecta todos los recursos!', '$'
    ADVERTENCIA DB '!CUIDADO! NO TOQUES EL AGUA!!', '$'
    CREDITOS DB 'Desarrollado por: Erick y Esteban', '$'

.CODE

; MOSTRAR PANTALLA DE INICIO

MOSTRAR_MENU_INICIO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Limpiar pantalla
    MOV AX, 0003H
    INT 10H
    
    ; Cambiar color de fondo y texto
    MOV AH, 06H         ; Scroll up (limpiar con color)
    MOV AL, 00H         ; Limpiar toda la ventana
    MOV BH, 1FH         ; Fondo azul (1), texto blanco (F)
    MOV CX, 0000H       ; Esquina superior izquierda
    MOV DX, 184FH       ; Esquina inferior derecha (fila 24, col 79)
    INT 10H
    
    ; Posicionar cursor para título línea 1
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 3           ; Fila 3
    MOV DL, 22          ; Columna 22 (centrado)
    INT 10H
    
    MOV AH, 09H
    LEA DX, TITULO_LINEA1
    INT 21H
    
    ; Línea 2
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 4
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, TITULO_LINEA2
    INT 21H
    
    ; Línea 3
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 5
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, TITULO_LINEA3
    INT 21H
    
    ; Línea 4
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 6
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, TITULO_LINEA4
    INT 21H
    
    ; Línea 5
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 7
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, TITULO_LINEA5
    INT 21H
    
    ; Línea 6
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 8
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, TITULO_LINEA6
    INT 21H
    
    ; Subtítulo
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 10
    MOV DL, 24
    INT 10H
    MOV AH, 09H
    LEA DX, SUBTITULO
    INT 21H
    
    ; Instrucciones
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 13
    MOV DL, 32
    INT 10H
    MOV AH, 09H
    LEA DX, INSTRUCCION1
    INT 21H
    
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 14
    MOV DL, 27
    INT 10H
    MOV AH, 09H
    LEA DX, INSTRUCCION2
    INT 21H
    
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 15
    MOV DL, 29
    INT 10H
    MOV AH, 09H
    LEA DX, INSTRUCCION3
    INT 21H
    
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 16
    MOV DL, 28
    INT 10H
    MOV AH, 09H
    LEA DX, INSTRUCCION4
    INT 21H
    
    ; Objetivo
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 18
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, INSTRUCCION5
    INT 21H
    
    ; Advertencia en rojo (cambiar atributo)
    MOV AH, 09H
    MOV AL, 20H         ; Espacio
    MOV BH, 0
    MOV BL, 4EH         ; Fondo rojo, texto amarillo
    MOV CX, 31          ; Longitud del mensaje
    INT 10H
    
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 19
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, ADVERTENCIA
    INT 21H
    
    ; Opción de inicio (parpadeando)
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 20
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, OPCION_INICIO
    INT 21H
    
    ; Créditos
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 23
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, CREDITOS
    INT 21H
    
    ; Esperar tecla
    MOV AH, 00H
    INT 16H
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
MOSTRAR_MENU_INICIO ENDP

