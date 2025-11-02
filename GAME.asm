.MODEL SMALL
.STACK 100H

; Incluir archivos de funciones
INCLUDE Graph.INC
INCLUDE map.INC
INCLUDE player.INC
INCLUDE resour.INC
INCLUDE input.INC
INCLUDE menu.INC
INCLUDE hud.INC
INCLUDE sound.INC
INCLUDE effects.INC

.DATA
    MENSAJE_INICIO DB 'JUEGO DE EXPLORACION - PRESIONA TECLA', '$'
    MENSAJE_VICTORIA DB '!FELICIDADES! RECOLECTASTE TODOS LOS RECURSOS', '$'
    MENSAJE_FIN DB 'FIN DEL JUEGO', '$'
    MENSAJE_PUNTOS DB 'PUNTUACION FINAL: ', '$'
    
.CODE
MAIN PROC
    MOV AX, @DATA
    MOV DS, AX
    
    ; Mostrar pantalla de inicio
    CALL MOSTRAR_MENU_INICIO
    
    ; Iniciar modo gráfico
    CALL INICIAR_MODO_GRAFICO
    CALL CARGAR_MAPA
    CALL INICIALIZAR_JUGADOR
    CALL CARGAR_RECURSOS
    
    ; Dibujar frame inicial
    CALL DIBUJAR_FRAME_COMPLETO
    
    ; Dibujar HUD por primera vez
    CALL DIBUJAR_HUD_COMPLETO
    
    CALL BUCLE_PRINCIPAL
    
    ; Volver a modo texto
    MOV AX, 0003H
    INT 10H
    
   
    MOV AH, 09H
    LEA DX, MENSAJE_FIN
    INT 21H
    
    
    MOV AH, 4CH
    INT 21H
MAIN ENDP


; BUCLE PRINCIPAL DEL JUEGO

BUCLE_PRINCIPAL PROC
BUCLE_JUEGO:
    ; Leer teclado
    CALL LEER_TECLADO
    
    ; Verificar recolección
    CALL VERIFICAR_RECOLECCION
    
    ; Actualizar HUD
    CALL DIBUJAR_HUD_COMPLETO
    
    ; Control de velocidad
    MOV CX, 0
    MOV DX, 1000
    MOV AH, 86H
    INT 15H
    
    ; Verificar victoria
    CALL VERIFICAR_VICTORIA
    CMP AL, 1
    JE VICTORIA
    
    JMP BUCLE_JUEGO
    
VICTORIA:
    ; Reproducir sonido de victoria
    CALL SONIDO_VICTORIA
    
    ; Pantalla de victoria mejorada
    CALL PANTALLA_VICTORIA
    RET
BUCLE_PRINCIPAL ENDP


; ACTUALIZAR JUEGO

ACTUALIZAR_JUEGO PROC
    CALL ACTUALIZAR_VIEWPORT
    CALL VERIFICAR_RECOLECCION
    CALL DIBUJAR_FRAME_COMPLETO
    CALL DIBUJAR_HUD_COMPLETO
    RET
ACTUALIZAR_JUEGO ENDP


; PANTALLA DE VICTORIA

PANTALLA_VICTORIA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Volver a modo texto
    MOV AX, 0003H
    INT 10H
    
    ; Cambiar color de fondo a verde
    MOV AH, 06H
    MOV AL, 00H
    MOV BH, 2FH         ; Fondo verde, texto blanco
    MOV CX, 0000H
    MOV DX, 184FH
    INT 10H
    
    ; Título de victoria
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 8
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, MENSAJE_VICTORIA
    INT 21H
    
    ; Mostrar puntuación
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 12
    MOV DL, 26
    INT 10H
    MOV AH, 09H
    LEA DX, MENSAJE_PUNTOS
    INT 21H
    
    ; Convertir y mostrar número de puntos
    MOV AX, PUNTUACION_HUD
    CALL MOSTRAR_PUNTUACION_FINAL
    
    ; Mensaje de salida
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 20
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, MENSAJE_FIN
    INT 21H
    
    ; Esperar tecla
    MOV AH, 00H
    INT 16H
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
PANTALLA_VICTORIA ENDP


; MOSTRAR PUNTUACIÓN FINAL
; AX = puntuación

MOSTRAR_PUNTUACION_FINAL PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Convertir número a string y mostrar
    MOV CX, 0
    MOV BX, 10
    
DIVIDIR_LOOP:
    MOV DX, 0
    DIV BX
    PUSH DX
    INC CX
    CMP AX, 0
    JNE DIVIDIR_LOOP
    
MOSTRAR_LOOP:
    POP DX
    ADD DL, '0'
    MOV AH, 02H
    INT 21H
    LOOP MOSTRAR_LOOP
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
MOSTRAR_PUNTUACION_FINAL ENDP

END MAIN