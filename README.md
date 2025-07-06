# monitor-system-os

![GitHub repo size](https://img.shields.io/github/repo-size/jonathand77/monitor-system-os)
![GitHub contributors](https://img.shields.io/github/contributors/jonathand77/monitor-system-os)
![GitHub last commit](https://img.shields.io/github/last-commit/jonathand77/monitor-system-os)
![License](https://img.shields.io/badge/license-BSD%202--Clause-blue)
![Languages](https://img.shields.io/github/languages/count/jonathand77/monitor-system-os)

## 👥 Integrantes

| 👨‍💻 Nombre | 📧 Correo | 🐙 Usuario GitHub |
|---|---|---|
| **Jonathan David Fernandez Vargas** | jonathand.fernandez@udea.edu.co | [jonathand77](https://github.com/jonathand77) |
| **Valeria Alvarez Fernandez** | valeria.alvarezf@udea.edu.co | [vaf88](https://github.com/vaf88) |
| **Santiago Arenas Gomez** | santiago.arenas1@udea.edu.co | []() |

---

## 🖥️ Monitor del Sistema con Interfaz Gráfica (GTK)

Este proyecto es un **monitor de sistema multiplataforma** con interfaz gráfica desarrollada en C usando **GTK+3**. Permite visualizar en tiempo real el uso de **CPU**, **memoria RAM**, **disco**, **red** y la **lista de procesos activos** (PID y nombre), tanto en **Linux** como en **Windows** (MSYS2 MinGW 64-bit).

---

## ✨ Características

- 📊 Visualización gráfica del uso de **CPU**, **RAM**, **disco** y **red**
- 📝 Muestra la **lista de procesos** (PID y nombre) en la ventana
- 🔄 Actualización automática de todos los datos cada 2 segundos
- 🧩 Código modular y portable para **Linux** y **Windows**
- 💡 Interfaz sencilla y clara

---

## 📁 Estructura del proyecto

```
monitor-system-os/
├── Makefile
└── src/
    ├── main.c
    ├── monitor.c
    ├── monitor.h
    └── gui.c
```

---

## ⚙️ Requisitos

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

---

## 🛠️ Compilación

Desde la raíz del proyecto, ejecuta:

```sh
make
```

Esto generará el ejecutable `monitor-sistema-grafico`.

---

## 🚀 Ejecución

En la misma terminal donde compilaste, ejecuta:

```sh
./monitor-sistema-grafico
```

En Windows, asegúrate de estar en la terminal **MSYS2 MinGW 64-bit** para que las librerías de GTK estén disponibles.

---

## 📝 Notas

- En **Windows** y **Linux** se muestra la lista de procesos en la ventana (no solo en consola).
- El uso de **CPU** y **red** está implementado para ambos sistemas operativos.
- Si tienes problemas con las rutas, recuerda que en MSYS2 las rutas de Windows se escriben como `/c/Usuarios/TuUsuario/...`.
- El código está preparado para ser extendido y mejorado fácilmente.

---

## 👨‍💻 Créditos

Desarrollado por [Tu Nombre o Equipo].

---

¡Contribuciones y sugerencias son bienvenidas! 🚀