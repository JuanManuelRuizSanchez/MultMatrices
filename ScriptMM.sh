#!/bin/bash

# --- Configuración ---
dimensions=(256 512 1024 2048 4096)  # Tamaños de matrices a evaluar
iterations=10                         # Iteraciones por configuración
thread_counts=(2 4 8 16 32)            # Cantidades de hilos a evaluar
process_counts=(2 4 8 16 32)           # Cantidades de procesos a evaluar

# --- Ejecución secuencial ---
output_file="ScriptMMS.csv"
echo "Dimension,Iteration,Time" > "$output_file"

for dim in "${dimensions[@]}"; do
    for (( iter=1; iter<=iterations; iter++ )); do
        time_result=$(./MMSec "$dim")
        echo "$dim,$iter,$time_result" >> "$output_file"
    done
done

# --- Ejecución con hilos ---
for num_hilos in "${thread_counts[@]}"; do
    output_file="ScriptMMH_${num_hilos}hilos.csv"
    echo "Dimension,Iteration,Threads,Time" > "$output_file"

    for dim in "${dimensions[@]}"; do
        for (( iter=1; iter<=iterations; iter++ )); do
            time_result=$(./MMhilos "$dim" "$num_hilos")
            echo "$dim,$iter,$num_hilos,$time_result" >> "$output_file"
        done
    done
done

# --- Ejecución con procesos ---
for num_procesos in "${process_counts[@]}"; do
    output_file="ScriptMMP_${num_procesos}procesos.csv"
    echo "Dimension,Iteration,Processes,Time" > "$output_file"

    for dim in "${dimensions[@]}"; do
        for (( iter=1; iter<=iterations; iter++ )); do
            time_result=$(./MMProc "$dim" "$num_procesos")
            echo "$dim,$iter,$num_procesos,$time_result" >> "$output_file"
        done
    done
done

echo "✅ Todas las pruebas han finalizado. Los archivos CSV han sido generados."
