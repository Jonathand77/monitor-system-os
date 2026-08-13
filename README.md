# **📘 Monitor del Sistema con Interfaz Gráfica (GTK)**

---

## 🛠️ Stack tecnológico y Arquitectura

![C](https://img.shields.io/badge/C-17A2B8?logo=c&logoColor=white)
![GTK](https://img.shields.io/badge/GTK-3-73D216?logo=gtk&logoColor=white)
![Make](https://img.shields.io/badge/Build-Make-6C757D)
![Linux](https://img.shields.io/badge/Linux-Compatible-FCC624?logo=linux&logoColor=black)
![Windows](https://img.shields.io/badge/Windows-MSYS2%20MinGW%2064--bit-0078D6?logo=windows&logoColor=white)
![Architecture](https://img.shields.io/badge/Architecture-Layered%20(GUI%20%2F%20App%20%2F%20Monitor%20%2F%20Platform)-blue)
![License](https://img.shields.io/badge/license-BSD%202--Clause-blue)
![GitHub repo size](https://img.shields.io/github/repo-size/jonathand77/monitor-system-os)
![GitHub contributors](https://img.shields.io/github/contributors/jonathand77/monitor-system-os)
![GitHub last commit](https://img.shields.io/github/last-commit/jonathand77/monitor-system-os)
![Languages](https://img.shields.io/github/languages/count/jonathand77/monitor-system-os)

## 👥 Integrantes

| 👨‍💻 Nombre | 📧 Correo | 🐙 Usuario GitHub |
|---|---|---|
| **Jonathan David Fernandez Vargas** | jonathand.fernandez@udea.edu.co | [jonathand77](https://github.com/jonathand77) |
| **Valeria Alvarez Fernandez** | valeria.alvarezf@udea.edu.co | [vaf88](https://github.com/vaf88) |

**Monitor de sistema multiplataforma (Linux / Windows) con interfaz gráfica en GTK+3, que muestra CPU, RAM, disco, red y procesos en tiempo real.**

---

## 1. 🔍 Introducción

Este proyecto es un **monitor de sistema multiplataforma** con interfaz gráfica desarrollada en C usando **GTK+3**. Permite visualizar en tiempo real el uso de **CPU**, **memoria RAM**, **disco**, **red** y la **lista de procesos activos** (PID y nombre), tanto en **Linux** como en **Windows** (MSYS2 MinGW 64-bit).

La solución está organizada en capas con una única dirección de dependencia (`main → gui → app → monitor → platform`), de modo que la obtención de datos del sistema operativo está completamente aislada de la interfaz gráfica y de la lógica de negocio.

---

## 2. ✨ Características

- 📊 Visualización gráfica del uso de **CPU**, **RAM**, **disco** y **red**
- 📝 Muestra la **lista de procesos** (PID y nombre) en la ventana
- 🔄 Actualización automática de todos los datos cada 2 segundos
- 🧩 Código modular y portable para **Linux** y **Windows**, sin lógica de plataforma dispersa por el código
- 🧪 Tests unitarios de la lógica de cálculo, independientes de GTK y del sistema operativo
- 💡 Interfaz sencilla y clara

---

## 3. ⚙️ Requisitos Previos

### 3.1 Linux

- `gcc`
- `make`
- `pkg-config`
- `libgtk-3-dev`

Instala las dependencias en Ubuntu/Debian con:

```sh
sudo apt-get update
sudo apt-get install build-essential pkg-config libgtk-3-dev make
```

### 3.2 Windows (MSYS2 MinGW 64-bit)

1. Descarga e instala [MSYS2](https://www.msys2.org/).
2. Abre la terminal **MSYS2 MinGW 64-bit**.
3. Instala las dependencias:

```sh
pacman -Syu
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-toolchain mingw-w64-x86_64-pkg-config make
```

---

## 📦 Estructura del Proyecto

```text
monitor-system-os/
├── LICENSE
├── Makefile
├── .gitignore
├── tests/
│   └── test_monitor.c        # tests de la lógica pura (sin GTK)
└── src/
    ├── main.c                 # bootstrap: crea la ventana y arranca el loop de GTK
    ├── gui.h / gui.c           # construcción y actualización de los widgets
    ├── app.h / app.c           # orquesta el refresco y guarda el estado entre ciclos
    ├── monitor.h / monitor.c   # fachada de monitoreo (independiente de plataforma)
    ├── platform.h              # interfaz PlatformOps que cada SO implementa
    ├── platform_linux.c        # implementación para Linux (/proc)
    ├── platform_windows.c      # implementación para Windows (WinAPI)
    └── text_buffer.h           # helper para escribir listados largos sin desbordar el buffer
```

Añadir soporte para un nuevo sistema operativo solo requiere un nuevo `platform_<so>.c` que implemente `platform.h` y una línea en el `Makefile`; no hace falta tocar `main.c`, `gui.c`, `app.c` ni `monitor.c`.

---

## 4. 🖥️ Guía Paso a Paso para Levantar el Proyecto

### 4.1 Clonar el repositorio

```sh
git clone https://github.com/jonathand77/monitor-system-os.git
cd monitor-system-os
```

### 4.2 Compilar

Desde la raíz del proyecto (en Linux, o en la terminal **MSYS2 MinGW 64-bit** en Windows):

```sh
make
```

Esto compila `src/` completo y genera el ejecutable `monitor-sistema-grafico`. El `Makefile` detecta el sistema operativo automáticamente y enlaza `platform_linux.c` o `platform_windows.c` según corresponda (en Windows, además enlaza `iphlpapi` para las estadísticas de red).

### 4.3 Ejecutar

En la misma terminal donde compilaste:

```sh
./monitor-sistema-grafico
```

En Windows, asegúrate de estar en la terminal **MSYS2 MinGW 64-bit** para que las librerías de GTK estén disponibles.

### 4.4 Correr los tests

```sh
make test
```

Compila y ejecuta `tests/test_monitor.c` contra la lógica real de `monitor.c` + la implementación de plataforma correspondiente. No requiere GTK instalado, ya que la GUI no participa en la compilación de los tests.

---

## 5. 🏗️ Arquitectura y Buenas Prácticas

### 5.1 Responsabilidad de cada módulo

| Módulo | Responsabilidad |
|---|---|
| `main.c` | Bootstrap: inicializa GTK, crea la ventana y arranca el timer de refresco |
| `gui.c` / `gui.h` | Construye los widgets y los actualiza a partir de un snapshot de datos |
| `app.c` / `app.h` | Orquesta el ciclo de refresco y conserva el estado entre llamadas (últimas muestras de CPU, contadores de red) |
| `monitor.c` / `monitor.h` | Fachada de monitoreo: expone CPU/memoria/disco/red/procesos sin exponer cómo se obtienen |
| `platform.h` | Interfaz `PlatformOps` que cada sistema operativo implementa |
| `platform_linux.c` | Implementación Linux, leyendo `/proc` |
| `platform_windows.c` | Implementación Windows, usando la WinAPI (`GetSystemTimes`, `GlobalMemoryStatusEx`, `GetIfTable`, `Toolhelp32`) |

### 5.2 Decisiones de diseño

- **Separación de responsabilidades**: cada capa depende únicamente de la capa inmediatamente inferior (`main → gui → app → monitor → platform`); ninguna capa "salta" a una más profunda.
- **Sin lógica de plataforma dispersa**: en vez de `#ifdef _WIN32` repetido en cada función, cada sistema operativo implementa una única tabla de funciones (`PlatformOps`). Agregar una plataforma nueva no obliga a modificar código existente.
- **Sin estado global expuesto**: las últimas muestras de CPU y los contadores de red viven como variables `static` privadas de `app.c`, no como globales visibles desde cualquier archivo.
- **Buffers de tamaño acotado y seguros**: el listado de procesos usa un helper compartido (`text_buffer.h`) que nunca desborda el buffer y marca explícitamente cuando la lista se trunca, en vez de cortar en silencio.
- **Lógica pura testeable**: el cálculo de uso de CPU (`calculate_cpu_usage`) no depende de GTK ni de llamadas al sistema operativo, por lo que se puede probar de forma aislada (ver `tests/test_monitor.c`).

---

## 6. 📊 Datos monitoreados

| Indicador | Fuente en Linux | Fuente en Windows | Actualización |
|---|---|---|---|
| **CPU** | `/proc/stat` | `GetSystemTimes` | cada 2 s |
| **Memoria RAM** | `/proc/meminfo` | `GlobalMemoryStatusEx` | cada 2 s |
| **Disco** | `statvfs("/")` | `GetDiskFreeSpaceExA` | cada 2 s |
| **Red** | `/proc/net/dev` | `GetIfTable` (iphlpapi) | cada 2 s |
| **Procesos** (PID + nombre) | `/proc/<pid>/status` | `CreateToolhelp32Snapshot` | cada 2 s |

---

## 7. 📝 Notas

- En **Windows** y **Linux** se muestra la lista de procesos en la ventana (no solo en consola).
- El uso de **CPU** y **red** está implementado para ambos sistemas operativos con la misma fórmula de cálculo.
- Si tienes problemas con las rutas en Windows, recuerda que en MSYS2 las rutas se escriben como `/c/Usuarios/TuUsuario/...`.
- El código está preparado para extenderse fácilmente: nuevas plataformas solo requieren implementar `platform.h`.

---

## 8. 🤝 Contribuciones

¡Contribuciones y sugerencias son bienvenidas! 🚀

---
## **Fin de la guía y manual de usuario.**
---
