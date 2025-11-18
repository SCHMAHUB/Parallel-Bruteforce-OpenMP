# Bruteforce Paralelizado con OpenMP
![Version](https://img.shields.io/badge/version-1.0-blue.svg)
![Language](https://img.shields.io/badge/language-C-green.svg)
![OpenMP](https://img.shields.io/badge/parallel-OpenMP-orange.svg)

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

## Directivas OpenMP Utilizadas

### #pragma omp critical
- Solo UN hilo puede ejecutar el bloque a la vez
- Los demás hilos esperan su turno (mutex implícito)
- Garantiza actualización atómica de password_found y found_password

```c
//thread-safe with critical clausule
        if (strcmp(current, target) == 0) {
            #pragma omp critical
            {
                if (!password_found) {
                    password_found = 1;
                    strcpy(found_password, current);
                }
            }
        }
```

### #pragma omp parallel y #pragma omp for schedule(dynamic) nowait
- `#pragma omp for`: Distribuye las iteraciones del bucle entre los hilos
- `schedule(dynamic)`: Asignación dinámica de trabajo
- `nowait`: Los hilos NO esperan en una barrera al terminar

```c
 //parallelize the first tree searching level
        #pragma omp parallel
        {
            char current[MAX_PASSWORD_LENGTH + 1];

            //each thread start with a different character using schedule clausule with nowait
            #pragma omp for schedule(dynamic) nowait
            for (int i = 0; i < CHARSET_SIZE; i++) {
                if (!password_found) {
                    current[0] = CHARSET[i];
                    bruteforce_recursive(target, current, 1, len);
                }
            }
        }
```

### Estrategia de Paralelización

El código paralela SOLO el primer nivel del árbol de búsqueda:
```
      ROOT
      / | \
    a   b   c   ...  (Nivel 1 - PARALELO)
   /|\  /|\  /|\
  a b c a b c a b c  (Nivel 2+ - SECUENCIAL por cada hilo)
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

