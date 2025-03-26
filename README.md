# MultMatrices

Aquí tienes un **README.md** estructurado y claro para tu repositorio **MultMatrices**.  

---

# Multiplicación de Matrices en C  

Este repositorio contiene implementaciones en C del algoritmo de **multiplicación de matrices** en tres versiones:  
- **Secuencial** (MMSec.c)  
- **Paralelo con hilos (pthreads)** (MMhilos.c)  
- **Paralelo con procesos (fork y memoria compartida)** (MMProc.c)  

## Autor  
**Juan Manuel Ruiz Sánchez**  

## 📜 Uso  
Compila y ejecuta cada versión de la siguiente manera:  

### Versión secuencial  
```sh
gcc MMSec.c -o MMSec
./MMSec [Dimension]
```

### Versión con hilos  
```sh
gcc MMhilos.c -o MMhilos -pthread
./MMhilos [Dimension] [Número de hilos]
```

### Versión con procesos  
```sh
gcc MMProc.c -o MMProc
./MMProc [Dimension] [Número de procesos]
```

## Reportes de rendimiento  
Se incluyen reportes de análisis de rendimiento generados con **gprof** en `reporte_MM.txt`, así como el script `ScriptMM.sh` que automatiza las pruebas con distintas configuraciones de paralelización.  

