#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_HILOS 32

typedef struct {
    int id;
    int inicio_fila;
    int fin_fila;
    int n;
    int** A;
    int** B;
    int** C;
} DatosHilo;

// Función para medir tiempo en microsegundos
double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

// Crear matriz aleatoria
int** crear_matriz(int n) {
    int** matriz = (int**)malloc(n * sizeof(int*));
    if (!matriz) {
        perror("Error al reservar memoria");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        matriz[i] = (int*)malloc(n * sizeof(int));
        if (!matriz[i]) {
            perror("Error al reservar memoria");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < n; j++) {
            matriz[i][j] = rand() % 100;
        }
    }
    return matriz;
}

// Liberar matriz
void liberar_matriz(int** matriz, int n) {
    for (int i = 0; i < n; i++) free(matriz[i]);
    free(matriz);
}

// Función ejecutada por cada hilo
void* kernel_multiplicacion_hilo(void* arg) {
    DatosHilo* datos = (DatosHilo*)arg;
    for (int i = datos->inicio_fila; i < datos->fin_fila; i++) {
        for (int j = 0; j < datos->n; j++) {
            datos->C[i][j] = 0;
            for (int k = 0; k < datos->n; k++) {
                datos->C[i][j] += datos->A[i][k] * datos->B[k][j];
            }
        }
    }
    pthread_exit(NULL);
}

// main recibe matriz y hilos como argumentos
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <dimension_matriz> <num_hilos>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int num_hilos = atoi(argv[2]);

    if (n <= 0 || num_hilos <= 0 || num_hilos > MAX_HILOS) {
        fprintf(stderr, "Error: La dimensión debe ser positiva y los hilos entre 1 y %d\n", MAX_HILOS);
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    int** A = crear_matriz(n);
    int** B = crear_matriz(n);
    int** C = crear_matriz(n);

    pthread_t hilos[num_hilos];
    DatosHilo datos[num_hilos];

    int filas_por_hilo = n / num_hilos;
    int resto = n % num_hilos;

    double t0 = get_time();

    for (int i = 0; i < num_hilos; i++) {
        datos[i].id = i;
        datos[i].inicio_fila = i * filas_por_hilo;
        datos[i].fin_fila = (i + 1) * filas_por_hilo;
        if (i == num_hilos - 1) {
            datos[i].fin_fila += resto;  // Último hilo toma las filas sobrantes
        }
        datos[i].n = n;
        datos[i].A = A;
        datos[i].B = B;
        datos[i].C = C;
        pthread_create(&hilos[i], NULL, kernel_multiplicacion_hilo, &datos[i]);
    }

    for (int i = 0; i < num_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }

    double t1 = get_time();

    // Imprime solo el tiempo (formato limpio)
    printf("%.6f\n", t1 - t0);

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}
