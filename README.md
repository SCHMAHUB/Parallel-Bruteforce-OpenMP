# Bruteforce Paralelizado con OpenMP

Proyecto que demuestra el uso de OpenMP para paralelizar un algoritmo de fuerza bruta para descifrar contraseñas con rapidos intentos de combinaciones.

## Características

- Implementación en C puro con OpenMP
- Paralelización mediante directivas OpenMP
- Búsqueda recursiva optimizada
- Medición precisa del tiempo de ejecución
- Detección temprana de contraseña encontrada
- Configuración dinámica del número de hilos

## Directivas OpenMP Utilizadas

El código utiliza las siguientes directivas OpenMP:

1. **`#pragma omp parallel`**: Crea una región paralela donde múltiples hilos ejecutan el código
2. **`#pragma omp for schedule(dynamic) nowait`**: Distribuye iteraciones del bucle entre hilos de forma dinámica
3. **`#pragma omp critical`**: Sección crítica para acceso exclusivo a variables compartidas
4. **`omp_set_num_threads()`**: Configura el número de hilos a utilizar
5. **`omp_get_wtime()`**: Obtiene el tiempo de alta precisión para mediciones

## Compilación

### Requisitos
- GCC con soporte OpenMP

- En Debian/Ubuntu
```bash
sudo apt install gcc g++
```

### Compilar con bash
```bash
chmod +x build.sh
./build.sh
```

### Compilar manualmente
```bash
gcc -fopenmp -o bruteforce bruteforce.c
```

## Uso

### Sintaxis
```bash
./bruteforce [num_hilos] [contraseña_objetivo]
```

### Parámetros
- `num_hilos` (opcional): Número de hilos OpenMP a utilizar (por defecto: 4)
- `contraseña_objetivo` (opcional): Contraseña a encontrar (por defecto: "abc123")

### Ejemplos

**Ejemplo 1: Contraseña simple con 4 hilos**
```bash
./bruteforce 4 abc
```

**Ejemplo 2: Contraseña con 8 hilos**
```bash
./bruteforce 8 a1b2
```

**Ejemplo 3: Usar valores por defecto**
```bash
./bruteforce
```


## Conjunto de Caracteres

El programa utiliza el siguiente conjunto de caracteres:
```
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789
```
Total: 62 caracteres (26 minúsculas + 26 mayúsculas + 10 dígitos)

## Complejidad

El número de combinaciones a probar crece exponencialmente:

| Longitud | Combinaciones |
|----------|---------------|
| 1        | 62            |
| 2        | 3,844         |
| 3        | 238,328       |
| 4        | 14,776,336    |
| 5        | 916,132,832   |

## Salida del Programa

El programa muestra:
1. Configuración inicial (hilos, conjunto de caracteres, complejidad)
2. Progreso por longitud de contraseña
3. Contraseña encontrada
4. Tiempo transcurrido en segundos
5. Velocidad de búsqueda (intentos/segundo)

### Ejemplo de salida:
```
==============================================
  PARALLEL BRUTEFORCE WITH OpenMP
==============================================

Config.:
-> Char. group: abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789
-> Group size: 62 caracteres
-> Number of threads: 4
-> Target password length: 5

Total combinations to try out: 931151402

Starting parallelized search with OpenMP...
Target password (length 5)
Available threads: 4

Trying passwords with length 1...
Trying passwords with length 2...
Trying passwords with length 3...
Trying passwords with length 4...
Trying passwords with length 5...

==============================================
PASSWORD FOUND: abc12
Elapsed time: 0.078173 seconds
Aprox. Velocity: 11911432978.23 attempts/second
==============================================
```

## Análisis de Rendimiento

### Escalabilidad
El programa muestra mejoras de rendimiento al aumentar el número de hilos:
- Mayor número de hilos = búsqueda más rápida
- El rendimiento depende del número de núcleos disponibles
- Usar más hilos que núcleos puede degradar el rendimiento

### Estrategia de Paralelización
1. **División del trabajo**: El primer nivel del árbol de búsqueda se paraleliza distribuyendo los caracteres iniciales entre hilos
2. **Búsqueda recursiva**: Cada hilo explora su subárbol de forma recursiva
3. **Terminación temprana**: Cuando un hilo encuentra la contraseña, todos los demás se detienen mediante la variable compartida `password_found`
4. **Sección crítica**: La actualización de la contraseña encontrada está protegida para evitar condiciones de carrera

