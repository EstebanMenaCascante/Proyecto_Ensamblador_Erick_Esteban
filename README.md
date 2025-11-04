# 🎮 Juego de Exploración y Recolección de Recursos

## Proyecto de Arquitectura de Computadoras
**Desarrollado por:** Erick y Esteban  
**Lenguaje:** Ensamblador x86 (MASM/TASM)  
**Modo Gráfico:** EGA 640x350 (16 colores)

---

## 📋 Descripción del Juego

Juego de exploración en 2D donde el jugador debe recolectar recursos distribuidos aleatoriamente en un mapa. El objetivo es recolectar todos los recursos (minerales, madera y frutas) para ganar el juego y obtener la mayor puntuación posible.

### Características Principales:
- ✅ Pantalla de inicio con instrucciones
- ✅ Modo gráfico EGA 640x350 pixels
- ✅ Mapa generado aleatoriamente con obstáculos
- ✅ Sistema de colisiones
- ✅ Recolección de 3 tipos de recursos (45 totales)
- ✅ Sistema de puntuación
- ✅ HUD en tiempo real en esquina superior derecha
- ✅ Inventario detallado (tecla E)
- ✅ Efectos de sonido (PC Speaker)
- ✅ Efectos visuales al recolectar
- ✅ Pantalla de victoria animada con estadísticas

---

## 🎯 Objetivo del Juego

Recolecta **todos los recursos** disponibles en el mapa:
- 🔘 **15 Piedras (Minerales)** (Gris) - 10 puntos c/u
- 🟫 **15 Maderas** (Marrón) - 15 puntos c/u
- 🔴 **15 Frutas** (Rojo) - 20 puntos c/u

**Total de recursos:** 45  
**Puntuación máxima:** 675 puntos (150 + 225 + 300)

---

## 🎮 Controles

| Tecla | Acción |
|-------|--------|
| **W** | Mover arriba |
| **S** | Mover abajo |
| **A** | Mover izquierda |
| **D** | Mover derecha |
| **E** | Abrir/Cerrar inventario detallado |
| **ESC** | Salir del juego |

---

## 🖥️ Requisitos del Sistema

### Hardware:
- Procesador compatible con x86 (8086/80286 o superior)
- Tarjeta gráfica con soporte EGA o superior
- 640 KB de RAM
- PC Speaker para sonidos

### Software:
- DOSBox (recomendado para sistemas modernos)
- TASM (Turbo Assembler) o MASM (Microsoft Macro Assembler)
- TLINK (Turbo Linker)

---

## 🔧 Compilación e Instalación

### Paso 1: Compilar el proyecto

```batch
tasm /zi GAME.asm
tlink /v GAME.obj
```

### Paso 2: Ejecutar el juego

```batch
GAME.exe
```

### En DOSBox (Windows/Linux/Mac):

1. Montar la carpeta del proyecto:
```
mount c c:\ruta\al\proyecto
c:
```

2. Compilar y ejecutar:
```
tasm GAME.asm
tlink GAME.obj
GAME.exe
```

---

## 📁 Estructura del Proyecto

```
Proyecto_Ensamblador/
│
├── GAME.asm          # Programa principal
├── Graph.inc         # Funciones gráficas
├── map.inc           # Generación de mapas
├── player.inc        # Control del jugador
├── resour.inc        # Sistema de recursos
├── input.inc         # Manejo de teclado
├── menu.inc          # Pantalla de inicio
├── hud.inc           # Interfaz de usuario (HUD)
├── sound.inc         # Efectos de sonido
├── mapa.txt          # Archivo de mapa (legacy)
└── README.md         # Este archivo
```

---

## 🎨 Elementos del Juego

### Terrenos:
- 🟫 **Tierra** (marrón) - Terreno transitable
- ⬛ **Piedra** (negro) - Obstáculo sólido
- 🟦 **Agua** (azul) - Obstáculo líquido
- 🟩 **Hierba** (verde) - Decoración transitable

### Recursos:
- 🔘 **Mineral** (gris claro) - 10 puntos
- 🟫 **Madera** (marrón oscuro) - 15 puntos
- 🔴 **Fruta** (rojo) - 20 puntos

### Jugador:
- 🟥 **Avatar** (rojo brillante) - 16x16 pixels

---

## 🎵 Efectos de Sonido

El juego utiliza el **PC Speaker** para reproducir sonidos:
- 🔊 **Beep corto** - Al recolectar recursos
- 🎶 **Melodía** - Al completar el juego
- ⚠️ **Beep grave** - Colisión con obstáculos (si implementado)

---

## 🏆 Sistema de Puntuación

| Recurso | Cantidad | Puntos Unitarios | Total |
|---------|----------|------------------|-------|
| Piedra  | 15       | 10 pts          | 150 pts |
| Madera  | 15       | 15 pts          | 225 pts |
| Fruta   | 15       | 20 pts          | 300 pts |
| **TOTAL** | **45** | -               | **675 pts** |

**Estrategia:** Prioriza recolectar frutas para maximizar tu puntuación.

---

## 🐛 Solución de Problemas

### Problema: "No se encuentra el archivo GRAPH.INC"
**Solución:** Asegúrate de que todos los archivos .INC estén en el mismo directorio que GAME.asm

### Problema: "Error de modo gráfico"
**Solución:** Verifica que tu sistema o DOSBox soporte modo EGA

### Problema: "No se escucha sonido"
**Solución:** En DOSBox, habilita el PC Speaker en el archivo de configuración:
```ini
[speaker]
pcspeaker=true
```

### Problema: "Pantalla negra al iniciar"
**Solución:** Presiona cualquier tecla en la pantalla de inicio

---

## 📝 Características Técnicas

### Modo Gráfico:
- **Resolución:** 640x350 pixels
- **Colores:** 16 colores (paleta EGA)
- **Modo INT 10H:** AX = 0010h

### Memoria de Video:
- **Segmento:** 0A000h
- **Escritura directa** en memoria de video para mayor velocidad

### Generación de Mapa:
- Dimensiones: 40x21 tiles (16x16 pixels cada uno)
- Generación **aleatoria** de obstáculos
- Algoritmo de semilla basado en reloj del sistema

### Sistema de Colisiones:
- Detección por tiles
- Verificación antes del movimiento
- Prevención de superposición

---

## 👥 Créditos

**Desarrolladores:**
- Erick - Programación principal, sistema de gráficos y mapas
- Esteban - Sistema de recursos, HUD y sonidos

**Profesor:** [Nombre del profesor]  
**Curso:** Arquitectura de Computadoras  
**Fecha:** Noviembre 2025

---

## 📄 Licencia

Este proyecto es de uso educativo para el curso de Arquitectura de Computadoras.

---

## 🚀 Futuras Mejoras (Opcional)

- [ ] Múltiples niveles con dificultad creciente
- [ ] Enemigos con IA básica
- [ ] Sistema de vidas
- [ ] Guardado de puntuación máxima
- [ ] Música de fondo
- [ ] Animaciones del jugador
- [ ] Power-ups temporales
- [ ] Mini-mapa
---

**¡Disfruta el juego y obtén la puntuación más alta!** 🎮🏆
