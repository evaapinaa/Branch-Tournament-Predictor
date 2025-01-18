#include "ooo_cpu.h"
#define TAKEN true
#define NOT_TAKEN false
#define NUM_CONT 1024
#define NUM_LOCAL 1024
#define BITS_HIST_LOCAL 10
#define NUM_SELECTOR 1024

uint8_t contador_global[NUM_CONT];
uint8_t contador_local[NUM_LOCAL]; // Contador local
uint64_t historia_global;
uint64_t historia_local[NUM_LOCAL]; // Historia local, cada entrada de 10 bits
uint8_t tournament[NUM_SELECTOR]; // Selector

void O3_CPU::initialize_branch_predictor()
{
    // Inicializar contador global 2 bits
    for (int i = 0; i < NUM_CONT ; i++) {
        contador_global[i] = 2;
    }
    historia_global = 0; // Inicializar historia global
    
    // Inicializar contador local de 3 bits e historia local
    for (int i = 0; i < NUM_LOCAL ; i++) {
        contador_local[i] = 4; // valor medio del rango (son 3 bits -> 0 1 2 3 4 5 6 7)
        historia_local[i] = 1; // todo a 1 como indica el libro
    }
    
    // Inicializar el selector de torneo
    for (int i = 0; i < NUM_SELECTOR ; i++) {
        tournament[i] = 2; // equilibrado entre local y global (bits 0-1 para contador local y 2-3 para el global)
    }
}

uint8_t O3_CPU::predict_branch(uint64_t pc)
{
    // Indices para acceder a las tablas de los contadores y del selector
    uint64_t hashContGlobal = (pc ^ historia_global) % NUM_CONT;
    uint64_t hashContLocal =  pc % NUM_LOCAL;
    uint64_t hashTournament = pc % NUM_SELECTOR;

    // Tenemos que determinar las predicciones para poder elegir entre el contador global o local
    bool predecirGlobal = contador_global[hashContGlobal] >= 2;
    bool predecirLocal = contador_local[historia_local[hashContLocal]] >= 4;

    // Aquí el tournament predictor elige la mejor opción
    if (tournament[hashTournament] >= 2) {
        // Preferencia por la predicción global
        if (predecirGlobal) {
            // Si la predicción global es TAKEN
            return TAKEN;
        } else {
            // Si la prediccion global es NOT_TAKEN
            return NOT_TAKEN;
        }
    } else {
        // Preferencia por la predicción local
        if (predecirLocal) {
            // Si la predicción local es TAKEN
            return TAKEN;
        } else {
            // Si la predicción local es NOT_TAKEN
            return NOT_TAKEN;
        }
    }
}

void O3_CPU::last_branch_result(uint64_t pc, uint8_t taken)
{
    // Índices para acceder a las tablas de los contadores y del selector
    uint64_t hashContGlobal = (pc ^ historia_global) % NUM_CONT;
    uint64_t hashContLocal = pc % NUM_LOCAL;
    uint64_t hashTournament = pc % NUM_SELECTOR;

    // Taken contiene 0 o 1 en funcion de si se tomo o no
    // Actualizar contador_global
    if (taken != 0) {
        if (contador_global[hashContGlobal] < 3)
            contador_global[hashContGlobal]++;
    } else {
        if (contador_global[hashContGlobal] > 0)
            contador_global[hashContGlobal]--;
    }

    historia_global = historia_global << 1; // Desplazo un bit a la izq, inserta un 0
    if (taken) historia_global++; // sumo 1 si fue tomado

    // Actualizar contador_local
    if (taken != 0) {
        if (contador_local[historia_local[hashContLocal]] < 7) // el valor máximo con 3 bits es 7
            contador_local[historia_local[hashContLocal]]++;
    } else {
        if (contador_local[historia_local[hashContLocal]] > 0)
            contador_local[historia_local[hashContLocal]]--;
    }

    // Actualizar historia local con el resultado más reciente 
    historia_local[hashContLocal] = historia_local[hashContLocal] << 1; // Desplazo un bit a la izquierda
    historia_local[hashContLocal] = historia_local[hashContLocal] | taken; // Inserta un 1 si es taken, si no 0
    uint64_t mascara = (1 << BITS_HIST_LOCAL) - 1; // La mascara garantiza que se mantengan los 10 bits de entrada, y se descartan los bits más antiguos.
                                                   // si no, se produciría segmentation fault
    historia_local[hashContLocal] = historia_local[hashContLocal] & mascara;  // Se aplica la máscara con un AND 

    // Actualizar predictor tournament
    // Si el valor del contador global es mayor o igual a 2 es TAKEN. Si taken también es true, devuelve true
    bool selecGlobal = (contador_global[hashContGlobal] >= 2) == taken;
    // Si el valor del contador local es mayor o igual a 4 es TAKEN. Si taken también es true, devuelve true
    bool selecLocal = (contador_local[historia_local[hashContLocal]] >= 4) == taken;
    
    // Comprueba si la predicción global fue correcta y la local incorrecta
    // recordemos que el selector es un contador saturado de 2 bits, en el que le asginamos los bits 0-1 al contador local y 2-3 al global
    if (selecGlobal && !selecLocal) {
        if (tournament[hashTournament] < 3)
            tournament[hashTournament]++; // incrementamos para favorecer al global
    // Comprueba si la predicción local fue correcta y la global incorrecta
    } else if (!selecGlobal && selecLocal) {
        if (tournament[hashTournament] > 0)
            tournament[hashTournament]--; // decrementamos para favorecer al local
    }
    // Si ambos contadores aciertan o fallan, no se hace ningún ajsute

}


