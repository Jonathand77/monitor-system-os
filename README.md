# monitor-system-os

## Monitor del Sistema con Interfaz Gráfica (GTK)

Este proyecto es un monitor de sistema multiplataforma con interfaz gráfica desarrollada en C usando GTK+3. Permite visualizar en tiempo real el uso de CPU, memoria RAM, disco y red.

## Características

- Visualización gráfica del uso de CPU, RAM, disco y red.
- Actualización automática de los datos cada 3 segundos.
- Código modular y portable para Linux y Windows (MSYS2 MinGW 64-bit).
- Interfaz sencilla y clara.

## Estructura del proyecto

```
monitor-system-os/
├── Makefile
└── src/
    ├── main.c
    ├── monitor.c
    ├── monitor.h
    └── gui.c
```

## Requisitos

### Linux

- `gcc`
- `make`
- `pkg-config`
- `libgtk-3-dev`

Instala las dependencias en Ubuntu/Debian con:

```sh
sudo apt-get update
sudo apt-get install build-essential pkg-config libgtk-3-dev make
```

### Windows (MSYS2 MinGW 64-bit)

1. Descarga e instala [MSYS2](https://www.msys2.org/).
2. Abre la terminal **MSYS2 MinGW 64-bit**.
3. Instala las dependencias:

```sh
pacman -Syu
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-toolchain mingw-w64-x86_64-pkg-config make
```

## Compilación

Desde la raíz del proyecto, ejecuta:

```sh
make
```

Esto generará el ejecutable `monitor-sistema-grafico`.

## Ejecución

En la misma terminal donde compilaste, ejecuta:

```sh
./monitor-sistema-grafico
```

En Windows, asegúrate de estar en la terminal **MSYS2 MinGW 64-bit** para que las librerías de GTK estén disponibles.

## Notas

- En Windows, la información de red solo mostrará un mensaje de "No implementado".
- En Windows, la información de CPU solo mostrará 0% (no implementado).
- El código está preparado para ser extendido y mejorado fácilmente.
- Si tienes problemas con las rutas, recuerda que en MSYS2 las rutas de Windows se escriben como `/c/Usuarios/TuUsuario/...`.

## Créditos

Desarrollado por [Tu Nombre o Equipo].

---

¡Contribuciones y sugerencias son bienvenidas!