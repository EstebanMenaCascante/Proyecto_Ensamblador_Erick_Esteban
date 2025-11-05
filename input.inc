; ENTRADA DE TECLADO - INPUT.INC

.CODE

LEER_TECLADO PROC
    MOV AH, 01H
    INT 16H
    JZ NO_TECLA

    MOV AH, 00H
    INT 16H

    CMP AL, 'w'
    JE ES_W
    CMP AL, 'W'
    JE ES_W

    CMP AL, 's'
    JE ES_S
    CMP AL, 'S'
    JE ES_S

    CMP AL, 'a'
    JE ES_A
    CMP AL, 'A'
    JE ES_A

    CMP AL, 'd'
    JE ES_D
    CMP AL, 'D'
    JE ES_D

    CMP AL, 'e'
    JE ES_E
    CMP AL, 'E'
    JE ES_E

    CMP AL, 27      ; ESC
    JE ES_ESC

    JMP NO_TECLA

ES_W: JMP MOVER_ARRIBA
ES_S: JMP MOVER_ABAJO
ES_A: JMP MOVER_IZQUIERDA
ES_D: JMP MOVER_DERECHA
ES_E: JMP ABRIR_INVENTARIO
ES_ESC: JMP SALIR_JUEGO

NO_TECLA: RET

ABRIR_INVENTARIO:
    CALL MOSTRAR_INVENTARIO_COMPLETO
    RET

; RUTINAS DE MOVIMIENTO CON ACTUALIZACIÓN PREVIA

MOVER_ARRIBA:
    MOV DIRECCION, 2
    ; Verificar límite superior (Y mínimo = 25 del HUD)
    MOV AX, JUGADOR_Y
    CMP AX, 27              ; 25 + 2 (mínimo para no salirse)
    JA CONTINUAR_ARRIBA     ; Si Y > 27, sí mover
    RET                     ; Si no, salir
CONTINUAR_ARRIBA:
    MOV AX, JUGADOR_X
    MOV JUGADOR_X_ANTERIOR, AX
    MOV AX, JUGADOR_Y
    MOV JUGADOR_Y_ANTERIOR, AX
    SUB JUGADOR_Y, 2
    CALL VERIFICAR_COLISION
    JNC MOVIMIENTO_OK_ARRIBA
    CMP AL, 2
    JNE RESTAURAR_Y_ARRIBA
    CALL PANTALLA_GAME_OVER
    RET
RESTAURAR_Y_ARRIBA:
    ADD JUGADOR_Y, 2
    RET
MOVIMIENTO_OK_ARRIBA:
    CALL REDIBUJAR_PARCIAL
    CALL VERIFICAR_CAMBIO_MAPA
    RET

MOVER_ABAJO:
    MOV DIRECCION, 3
    ; Verificar límite inferior (Y máximo = 25 + 20*16 - 16 = 329)
    MOV AX, JUGADOR_Y
    CMP AX, 327             ; 329 - 2 (máximo para no salirse)
    JB CONTINUAR_ABAJO      ; Si Y < 327, sí mover
    RET                     ; Si no, salir
CONTINUAR_ABAJO:
    MOV AX, JUGADOR_X
    MOV JUGADOR_X_ANTERIOR, AX
    MOV AX, JUGADOR_Y
    MOV JUGADOR_Y_ANTERIOR, AX
    ADD JUGADOR_Y, 2
    CALL VERIFICAR_COLISION
    JNC MOVIMIENTO_OK_ABAJO
    CMP AL, 2
    JNE RESTAURAR_Y_ABAJO
    CALL PANTALLA_GAME_OVER
    RET
RESTAURAR_Y_ABAJO:
    SUB JUGADOR_Y, 2
    RET
MOVIMIENTO_OK_ABAJO:
    CALL REDIBUJAR_PARCIAL
    CALL VERIFICAR_CAMBIO_MAPA
    RET

MOVER_IZQUIERDA:
    MOV DIRECCION, 1

    ; Verificar si está en zona de portal antes de bloquear
    MOV AX, JUGADOR_X
    CMP AX, 8               ; Muy cerca del borde izquierdo
    JBE VERIFICAR_PORTAL_IZQ
    ; Movimiento normal

    ; Verificar límite izquierdo (X mínimo = 0)
    MOV AX, JUGADOR_X
    CMP AX, 2               ; 0 + 2 (mínimo para no salirse)
    JA CONTINUAR_IZQ        ; Si X > 2, sí mover
    RET                     ; Si no, salir
CONTINUAR_IZQ:

    MOV JUGADOR_X_ANTERIOR, AX
    MOV AX, JUGADOR_Y
    MOV JUGADOR_Y_ANTERIOR, AX
    SUB JUGADOR_X, 2
    CALL VERIFICAR_COLISION
    JNC MOVIMIENTO_OK_IZQ
    CMP AL, 2
    JNE RESTAURAR_X_IZQ
    CALL PANTALLA_GAME_OVER
    RET
RESTAURAR_X_IZQ:
    ADD JUGADOR_X, 2
    RET
VERIFICAR_PORTAL_IZQ:
    ; Verificar si está en la zona Y del portal (fila 10 aprox)
    CALL ESTA_EN_ZONA_PORTAL
    CMP AL, 1
    JE PERMITIR_SALIDA_IZQ
    RET                     ; No está en portal, no mover
PERMITIR_SALIDA_IZQ:
    MOV JUGADOR_X_ANTERIOR, AX
    MOV AX, JUGADOR_Y
    MOV JUGADOR_Y_ANTERIOR, AX
    SUB JUGADOR_X, 2
    JMP MOVIMIENTO_OK_IZQ
MOVIMIENTO_OK_IZQ:
    CALL REDIBUJAR_PARCIAL
    CALL VERIFICAR_CAMBIO_MAPA
    RET

MOVER_DERECHA:
    MOV DIRECCION, 0
    ; Verificar si está en zona de portal antes de bloquear
    MOV AX, JUGADOR_X
    CMP AX, 616             ; Muy cerca del borde derecho (624-8)
    JAE VERIFICAR_PORTAL_DER
    ; Movimiento normal
    ; Verificar límite derecho (X máximo = 40*16 - 16 = 624)
    MOV AX, JUGADOR_X
    CMP AX, 622             ; 624 - 2 (máximo para no salirse)
    JB CONTINUAR_DER        ; Si X < 622, sí mover
    RET                     ; Si no, salir
CONTINUAR_DER:
    MOV JUGADOR_X_ANTERIOR, AX
    MOV AX, JUGADOR_Y
    MOV JUGADOR_Y_ANTERIOR, AX
    ADD JUGADOR_X, 2
    CALL VERIFICAR_COLISION
    JNC MOVIMIENTO_OK_DER
    CMP AL, 2
    JNE RESTAURAR_X_DER
    CALL PANTALLA_GAME_OVER
    RET
RESTAURAR_X_DER:
    SUB JUGADOR_X, 2
    RET
VERIFICAR_PORTAL_DER:
    ; Verificar si está en la zona Y del portal (fila 10 aprox)
    CALL ESTA_EN_ZONA_PORTAL
    CMP AL, 1
    JE PERMITIR_SALIDA_DER
    RET                     ; No está en portal, no mover
PERMITIR_SALIDA_DER:
    MOV JUGADOR_X_ANTERIOR, AX
    MOV AX, JUGADOR_Y
    MOV JUGADOR_Y_ANTERIOR, AX
    ADD JUGADOR_X, 2
    JMP MOVIMIENTO_OK_DER
MOVIMIENTO_OK_DER:
    CALL REDIBUJAR_PARCIAL
    CALL VERIFICAR_CAMBIO_MAPA
    RET

; ========================================
; VERIFICAR SI EL JUGADOR ESTÁ EN ZONA DE PORTAL
; Verifica si JUGADOR_Y está en el rango del portal (fila 8-12)
; Retorna: AL = 1 si está en zona de portal, 0 si no
; ========================================
ESTA_EN_ZONA_PORTAL PROC
    PUSH BX
    
    ; Portal en filas 8-12 (Y: 153-217 con offset de 25)
    ; Fila 8 * 16 + 25 = 153
    ; Fila 12 * 16 + 25 = 217
    MOV BX, JUGADOR_Y
    
    ; ¿Está entre Y=153 y Y=217?
    CMP BX, 153
    JB NO_ESTA_EN_PORTAL
    CMP BX, 217
    JA NO_ESTA_EN_PORTAL
    
    ; Sí está en la zona del portal
    MOV AL, 1
    JMP FIN_VERIFICAR_PORTAL
    
NO_ESTA_EN_PORTAL:
    MOV AL, 0
    
FIN_VERIFICAR_PORTAL:
    POP BX
    RET
ESTA_EN_ZONA_PORTAL ENDP

MUERTE_POR_AGUA:
    CALL PANTALLA_GAME_OVER
    RET

REDIBUJAR_PARCIAL PROC
    ; Borrar sprite anterior del jugador
    CALL BORRAR_SPRITE_JUGADOR

    ; Dibujar jugador en nueva posición
    CALL DIBUJAR_JUGADOR
    RET
REDIBUJAR_PARCIAL ENDP


SALIR_JUEGO:
    MOV AX, 4C00H
    INT 21H

LEER_TECLADO ENDP