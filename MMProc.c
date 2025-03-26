#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_PROCESOS 32

// Función para medir tiempo en microsegundos
double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

// Crear matriz aleatoria (en memoria dinámica común)
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

// Multiplicación por proceso (cada proceso maneja un chunk de filas)
void multiplicar_chunk(int id_proceso, int n, int procesos, int** A, int** B, int* C_shm) {
    int filas_por_proceso = n / procesos;
    int inicio = id_proceso * filas_por_proceso;
    int fin = (id_proceso == procesos - 1) ? n : (inicio + filas_por_proceso);

    for (int i = inicio; i < fin; i++) {
        for (int j = 0; j < n; j++) {
            C_shm[i * n + j] = 0;
            for (int k = 0; k < n; k++) {
                C_shm[i * n + j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <dimension_matriz> <num_procesos>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int num_procesos = atoi(argv[2]);

    if (n <= 0 || num_procesos <= 0 || num_procesos > MAX_PROCESOS) {
        fprintf(stderr, "Error: La dimensión debe ser positiva y los procesos entre 1 y %d\n", MAX_PROCESOS);
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    int** A = crear_matriz(n);
    int** B = crear_matriz(n);

    // Crear memoria compartida para la matriz resultado
    int shm_id = shmget(IPC_PRIVATE, n * n * sizeof(int), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Error creando memoria compartida");
        return EXIT_FAILURE;
    }

    int* C_shm = (int*)shmat(shm_id, NULL, 0);
    if (C_shm == (void*)-1) {
        perror("Error al adjuntar memoria compartida");
        return EXIT_FAILURE;
    }

    double t0 = get_time();

    pid_t pids[MAX_PROCESOS];

    for (int i = 0; i < num_procesos; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("Error al crear proceso");
            return EXIT_FAILURE;
        } else if (pids[i] == 0) {
            // Proceso hijo realiza su chunk
            multiplicar_chunk(i, n, num_procesos, A, B, C_shm);
            shmdt(C_shm);  // Desvincular memoria compartida
            exit(EXIT_SUCCESS);
        }
    }

    // Proceso padre espera a todos los hijos
    for (int i = 0; i < num_procesos; i++) {
        waitpid(pids[i], NULL, 0);
    }

    double t1 = get_time();

    // Imprimir solo el tiempo (sin texto adicional)
    printf("%.6f\n", t1 - t0);

    // Liberar memoria local
    liberar_matriz(A, n);
    liberar_matriz(B, n);

    // Liberar memoria compartida
    shmdt(C_shm);
    shmctl(shm_id, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}
