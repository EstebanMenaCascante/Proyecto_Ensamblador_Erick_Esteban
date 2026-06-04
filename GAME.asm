.MODEL SMALL
.STACK 100H


INCLUDE Graph.INC
INCLUDE map.INC
INCLUDE mapas.INC
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
    ; Variable PARA GAME OVER
    REINTENTAR DB 0
    MENSAJE_REINTENTAR DB 'Desea volver a intentar? (Y/N): ', '$'
    
    ; ASCII Art para victoria
    VICTORY_LINE1 DB ' __     _______ _____ _______ ____  _____  _____          _ ', '$'
    VICTORY_LINE2 DB ' \ \   / /_   _/ ____|__   __/ __ \|  __ \|_   _|   /\   | |', '$'
    VICTORY_LINE3 DB '  \ \ / /  | || |       | | | |  | | |__) | | |    /  \  | |', '$'
    VICTORY_LINE4 DB '   \ V /   | || |       | | | |  | |  _  /  | |   / /\ \ | |', '$'
    VICTORY_LINE5 DB '    \_/   |___| \____|  |_| |_|  |_|_| \_\__|_|/_/_/  \_\|_|', '$'
    
    STATS_LINE1 DB 'Piedras recolectadas: ', '$'
    STATS_LINE2 DB 'Maderas recolectadas: ', '$'
    STATS_LINE3 DB 'Frutas recolectadas: ', '$'
    STATS_LINE4 DB 'Total de recursos: ', '$'
    FELICITACION DB '*** MISION COMPLETADA CON EXITO ***', '$'
    PRESIONA_CONTINUAR DB 'Presiona cualquier tecla para continuar...', '$'
    
    ; Mensajes de GAME OVER
    GAMEOVER_LINE1 DB '  _____          __  __ ______    ______      ________ _____  ', '$'
    GAMEOVER_LINE2 DB ' / ____|   /\   |  \/  |  ____|  / __ \ \    / /  ____|  __ \ ', '$'
    GAMEOVER_LINE3 DB '| |  __   /  \  | \  / | |__    | |  | \ \  / /| |__  | |__) |', '$'
    GAMEOVER_LINE4 DB '| | |_ | / /\ \ | |\/| |  __|   | |  | |\ \/ / |  __| |  _  / ', '$'
    GAMEOVER_LINE5 DB '| |__| |/ ____ \| |  | | |____  | |__| | \  /  | |____| | \ \ ', '$'
    GAMEOVER_LINE6 DB ' \_____/_/    \_\_|  |_|______|  \____/   \/   |______|_|  \_\', '$'
    MENSAJE_AHOGADO DB '*** TE AHOGASTE EN EL AGUA ***', '$'
    MENSAJE_GAME_OVER DB 'Presiona cualquier tecla para salir...', '$'
    
.CODE
MAIN PROC
    MOV AX, @DATA
    MOV DS, AX
    
    ; Mostrar pantalla de inicio primero
    CALL MOSTRAR_MENU_INICIO
    
    ; Iniciar modo gráfico
    CALL INICIAR_MODO_GRAFICO
    
    ; Dibujar HUD primero 
    CALL DIBUJAR_HUD_COMPLETO
    
    ; Inicializar sistema de múltiples mapas
    CALL INICIALIZAR_MAPAS
    
    ; Cargar y dibujar mapa y recursos
    CALL CARGAR_MAPA
    CALL INICIALIZAR_JUGADOR
    CALL CARGAR_RECURSOS
    
    ; Dibujar frame inicial
    CALL DIBUJAR_FRAME_COMPLETO
    
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
    
    CALL LEER_TECLADO
    
    ; Verificar recolección 
    CALL VERIFICAR_RECOLECCION
    
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
    ; Pantalla de victoria mejorada 
    CALL PANTALLA_VICTORIA
    
    ; Volver a modo texto antes de salir
    MOV AX, 0003H
    INT 10H
    
    
    MOV AH, 4CH
    INT 21H
    
    RET
BUCLE_PRINCIPAL ENDP


; ACTUALIZAR JUEGO

ACTUALIZAR_JUEGO PROC
    CALL ACTUALIZAR_VIEWPORT
    CALL VERIFICAR_RECOLECCION
    CALL DIBUJAR_FRAME_COMPLETO
    RET
ACTUALIZAR_JUEGO ENDP


; PANTALLA DE VICTORIA CON ANIMACION

PANTALLA_VICTORIA PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ;Parpadeo de colores 
    MOV CX, 3
PARPADEO_COLORES:
    PUSH CX
    
    ;Azul brillante
    MOV AX, 0003H
    INT 10H
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 9FH         ; Fondo azul 
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ; Delay
    MOV CX, 0
    MOV DX, 8000
    MOV AH, 86H
    INT 15H
    
    ;Verde brillante
    MOV AX, 0003H
    INT 10H
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 0AFH        ; Fondo verde brillante
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ; Delay
    MOV CX, 0
    MOV DX, 8000
    MOV AH, 86H
    INT 15H
    
    POP CX
    LOOP PARPADEO_COLORES
    
    ;Verde con texto amarillo 
    MOV AX, 0003H
    INT 10H
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 2EH         ; Fondo verde, texto amarillo
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ;ASCII ART "VICTORIA!"
    
    ; Línea 1
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 3
    MOV DL, 10
    INT 10H
    MOV AH, 09H
    LEA DX, VICTORY_LINE1
    INT 21H
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    ; Línea 2
    MOV AH, 02H
    MOV DH, 4
    MOV DL, 10
    INT 10H
    MOV AH, 09H
    LEA DX, VICTORY_LINE2
    INT 21H
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    ; Línea 3
    MOV AH, 02H
    MOV DH, 5
    MOV DL, 10
    INT 10H
    MOV AH, 09H
    LEA DX, VICTORY_LINE3
    INT 21H
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    ; Línea 4
    MOV AH, 02H
    MOV DH, 6
    MOV DL, 10
    INT 10H
    MOV AH, 09H
    LEA DX, VICTORY_LINE4
    INT 21H
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    ; Línea 5
    MOV AH, 02H
    MOV DH, 7
    MOV DL, 10
    INT 10H
    MOV AH, 09H
    LEA DX, VICTORY_LINE5
    INT 21H
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    ;MENSAJE DE FELICITACION
    MOV AH, 02H
    MOV DH, 10
    MOV DL, 22
    INT 10H
    MOV AH, 09H
    LEA DX, FELICITACION
    INT 21H
    
    ; Reproducir música de victoria
    CALL SONIDO_VICTORIA
    
    ;ESTADISTICAS
    
    ; Piedras
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 13
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, STATS_LINE1
    INT 21H
    MOV AL, INVENTARIO[0]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    MOV DL, '1'
    INT 21H
    MOV DL, '5'
    INT 21H
    
    ; Maderas
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 14
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, STATS_LINE2
    INT 21H
    MOV AL, INVENTARIO[1]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    MOV DL, '1'
    INT 21H
    MOV DL, '5'
    INT 21H
    
    ; Frutas
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 15
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, STATS_LINE3
    INT 21H
    MOV AL, INVENTARIO[2]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    MOV DL, '1'
    INT 21H
    MOV DL, '5'
    INT 21H
    
    ; Total
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 17
    MOV DL, 20
    INT 10H
    MOV AH, 09H
    LEA DX, STATS_LINE4
    INT 21H
    MOV AL, INVENTARIO[0]
    ADD AL, INVENTARIO[1]
    ADD AL, INVENTARIO[2]
    CALL MOSTRAR_NUMERO_DOS_DIGITOS
    MOV DL, '/'
    MOV AH, 02H
    INT 21H
    MOV DL, '4'
    INT 21H
    MOV DL, '5'
    INT 21H
    
    ; === PUNTUACION ===
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 19
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, MENSAJE_PUNTOS
    INT 21H
    MOV AX, PUNTUACION_HUD
    CALL MOSTRAR_PUNTUACION_FINAL
    MOV DL, ' '
    MOV AH, 02H
    INT 21H
    MOV DL, 'p'
    INT 21H
    MOV DL, 't'
    INT 21H
    MOV DL, 's'
    INT 21H
    
    ; === MENSAJE FINAL ===
    MOV AH, 02H
    MOV DH, 22
    MOV DL, 19
    INT 10H
    MOV AH, 09H
    LEA DX, PRESIONA_CONTINUAR
    INT 21H
    
    ; Delay antes de esperar tecla
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    ; Limpiar buffer de teclado
LIMPIAR_BUFFER_VICTORIA:
    MOV AH, 01H
    INT 16H
    JZ BUFFER_LIMPIO_VICTORIA
    MOV AH, 00H
    INT 16H
    JMP LIMPIAR_BUFFER_VICTORIA
    
BUFFER_LIMPIO_VICTORIA:
    ; Esperar tecla
    MOV AH, 00H
    INT 16H
    
    POP DX
    POP CX
    POP BX
    POP AX
    RET
PANTALLA_VICTORIA ENDP



; AL = numero

MOSTRAR_NUMERO_DOS_DIGITOS PROC
    PUSH AX
    PUSH BX
    PUSH DX
    
    MOV BL, 10
    MOV AH, 0
    DIV BL          ; AL = decenas, AH = unidades
    
    ; Mostrar decenas
    PUSH AX
    ADD AL, '0'
    MOV DL, AL
    MOV AH, 02H
    INT 21H
    
    ; Mostrar unidades
    POP AX
    MOV AL, AH
    ADD AL, '0'
    MOV DL, AL
    MOV AH, 02H
    INT 21H
    
    POP DX
    POP BX
    POP AX
    RET
MOSTRAR_NUMERO_DOS_DIGITOS ENDP



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


;MUERTE POR AGUA

PANTALLA_GAME_OVER PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX
    
    ; Cambiar a modo texto
    MOV AX, 0003H
    INT 10H
    
    ; Parpadeo de color rojo
    MOV CX, 3
PARPADEO_ROJO:
    PUSH CX
    
    ;Rojo oscuro
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 40H         ; Fondo rojo oscuro
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ; Delay
    MOV CX, 0
    MOV DX, 8000
    MOV AH, 86H
    INT 15H
    
    ;Rojo brillante
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 4FH         ; Fondo rojo brillante
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ; Delay
    MOV CX, 0
    MOV DX, 8000
    MOV AH, 86H
    INT 15H
    
    POP CX
    LOOP PARPADEO_ROJO
    
    ;Fondo final rojo oscuro
    MOV AH, 06H
    MOV AL, 0
    MOV BH, 4EH         ; Fondo rojo, texto amarillo
    MOV CX, 0
    MOV DX, 184FH
    INT 10H
    
    ; ASCII Art "GAME OVER"
    MOV AH, 02H
    MOV BH, 0
    MOV DH, 5
    MOV DL, 8
    INT 10H
    MOV AH, 09H
    LEA DX, GAMEOVER_LINE1
    INT 21H
    
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 6
    MOV DL, 8
    INT 10H
    MOV AH, 09H
    LEA DX, GAMEOVER_LINE2
    INT 21H
    
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 7
    MOV DL, 8
    INT 10H
    MOV AH, 09H
    LEA DX, GAMEOVER_LINE3
    INT 21H
    
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 8
    MOV DL, 8
    INT 10H
    MOV AH, 09H
    LEA DX, GAMEOVER_LINE4
    INT 21H
    
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 9
    MOV DL, 8
    INT 10H
    MOV AH, 09H
    LEA DX, GAMEOVER_LINE5
    INT 21H
    
    MOV CX, 0
    MOV DX, 3000
    MOV AH, 86H
    INT 15H
    
    MOV AH, 02H
    MOV DH, 10
    MOV DL, 8
    INT 10H
    MOV AH, 09H
    LEA DX, GAMEOVER_LINE6
    INT 21H
    
    ; Pausa antes del mensaje
    MOV CX, 0
    MOV DX, 5000
    MOV AH, 86H
    INT 15H
    
    ; Mensaje
    MOV AH, 02H
    MOV DH, 13
    MOV DL, 25
    INT 10H
    MOV AH, 09H
    LEA DX, MENSAJE_AHOGADO
    INT 21H
    
    ;Reproducir música de derrota
    CALL MUSICA_GAME_OVER
    
    ;Preguntar si desea reintentar
    MOV AH, 02H
    MOV DH, 20
    MOV DL, 18
    INT 10H
    MOV AH, 09H
    LEA DX, MENSAJE_REINTENTAR
    INT 21H

ESPERAR_RESPUESTA_GO:
    ; Leer tecla y decidir
    MOV AH, 00H
    INT 16H        ; AL = tecla
    
   
    CMP AL, 'y'
    JE REINICIAR_DESDE_GO
    CMP AL, 'Y'
    JE REINICIAR_DESDE_GO
    
    CMP AL, 'n'
    JE SALIR_DESDE_GO
    CMP AL, 'N'
    JE SALIR_DESDE_GO
    
    ; Si no es Y ni N, esperar otra tecla válida
    JMP ESPERAR_RESPUESTA_GO

REINICIAR_DESDE_GO:
    ; Restaurar registros
    POP DX
    POP CX
    POP BX
    POP AX
    ; Reiniciar el juego directamente (no retornar)
    JMP REINICIAR_JUEGO

SALIR_DESDE_GO:
    ; Restaurar registros y salir
    POP DX
    POP CX
    POP BX
    POP AX
    ; Volver a modo texto
    MOV AX, 0003H
    INT 10H
    ; Salir del programa
    MOV AH, 4CH
    INT 21H
    
PANTALLA_GAME_OVER ENDP

; REINICIAR JUEGO
REINICIAR_JUEGO PROC
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX

    ; Reiniciar variables SIN recargar mapa ni recursos
    CALL INICIAR_MODO_GRAFICO
    CALL INICIALIZAR_JUGADOR      ; Resetear posición del jugador
    CALL REINICIAR_INVENTARIO      ; Limpiar inventario
    CALL RESTAURAR_RECURSOS_BACKUP ; Restaurar recursos a estado inicial
    CALL DIBUJAR_FRAME_COMPLETO
    CALL DIBUJAR_HUD_COMPLETO

    POP DX
    POP CX
    POP BX
    POP AX
    ; Saltar al bucle principal
    JMP BUCLE_JUEGO
REINICIAR_JUEGO ENDP

END MAIN