#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

// Función para medir tiempo en microsegundos
double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

// Crear matriz y llenarla con valores aleatorios
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

// KERNEL: multiplicación de matrices
void kernel_multiplicacion(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <dimension_matriz>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        fprintf(stderr, "Error: La dimensión debe ser positiva\n");
        return EXIT_FAILURE;
    }

    srand(time(NULL));  // Semilla aleatoria

    int** A = crear_matriz(n);
    int** B = crear_matriz(n);
    int** C = crear_matriz(n);  // Resultado (no se imprime)

    // Medir tiempo solo del kernel
    double t0 = get_time();
    kernel_multiplicacion(A, B, C, n);
    double t1 = get_time();

    // Imprimir solo el número
    printf("%.6f\n", t1 - t0);

    liberar_matriz(A, n);
    liberar_matriz(B, n);
    liberar_matriz(C, n);

    return EXIT_SUCCESS;
}