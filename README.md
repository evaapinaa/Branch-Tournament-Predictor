
# Tournament Branch Predictor

Este repositorio contiene una implementación de un **Tournament Branch Predictor** como parte de un simulador de CPU fuera de orden (Out-of-Order CPU). Este predictor combina predicciones globales y locales para mejorar la precisión en la predicción de bifurcaciones, seleccionando la mejor opción a través de un selector de torneo.

## Introducción
Un **Branch Predictor** es un componente fundamental de los procesadores modernos. Su objetivo es predecir la dirección que tomará una bifurcación (por ejemplo, un salto condicional en código), reduciendo los ciclos perdidos debido a bifurcaciones mal predichas.

El **Tournament Branch Predictor** utiliza dos métodos de predicción:

- **Predicción global**: Basada en el historial global de las bifurcaciones tomadas.
- **Predicción local**: Basada en el historial específico de una bifurcación particular (identificada por su dirección).

Un tercer componente, el **selector de torneo**, decide cuál de las dos predicciones usar en base a su historial de rendimiento.

## Estructura del código
El predictor se implementa en tres funciones principales:

### 1. Inicialización: `initialize_branch_predictor`
Esta función inicializa las estructuras necesarias:
- **`contador_global`**: Array de contadores de 2 bits para predicción global.
- **`historia_global`**: Variable que almacena los 64 bits más recientes del historial global.
- **`contador_local`**: Array de contadores de 3 bits para predicción local.
- **`historia_local`**: Array de historiales locales, cada uno con 10 bits.
- **`tournament`**: Selector de torneo (contadores de 2 bits para decidir entre global o local).

### 2. Predicción: `predict_branch`
Esta función calcula la predicción basada en los siguientes pasos:
- Genera índices para acceder a las tablas usando valores hash (PC e historial).
- Determina si las predicciones global y local predicen "**TAKEN**" o "**NOT_TAKEN**".
- Usa el selector de torneo para elegir entre la predicción global o local.

### 3. Actualización: `last_branch_result`
Después de ejecutar una bifurcación, esta función actualiza:
- Los contadores global y local según si la predicción fue correcta.
- El historial global y el historial local correspondiente.
- El selector de torneo, favoreciendo la predicción (global o local) que fue correcta.

## Cómo funciona

1. **Inicialización**:
   - Todos los contadores globales se inicializan a 2 (valor medio de 2 bits).
   - Los contadores locales se inicializan a 4 (valor medio de 3 bits) y los historiales locales a 1.
   - El selector de torneo se inicializa equilibrado (valor 2).

2. **Predicción**:
   - Se calcula un hash de la dirección del programa (PC) y el historial correspondiente.
   - Se obtienen predicciones "global" y "local" usando los contadores.
   - El selector de torneo elige la predicción que usará la CPU.

3. **Actualización**:
   - Los contadores y los historiales se actualizan según el resultado real de la bifurcación.
   - El selector ajusta sus contadores para favorecer la predicción (global o local) que fue correcta.

## Representación visual

### Selector de torneo:
- **Bits del selector**:
  - 0-1: Favorecen la predicción local.
  - 2-3: Favorecen la predicción global.

| Valor del selector | Preferencia       |
|--------------------|-------------------|
| 0-1                | Predicción local |
| 2-3                | Predicción global|

### Ejemplo de flujo:
1. Predicción:
   - Global: TAKEN (contador >= 2).
   - Local: NOT_TAKEN (contador < 4).
   - Selector: Prefiere global (valor >= 2).

2. Resultado real: TAKEN.
   - Incrementa el contador global.
   - Ajusta el selector para favorecer global.

## Compilación e integración
Asegúrate de incluir este archivo como parte de tu simulador de CPU. Si usas un Makefile, agrégalo al proceso de compilación.

Ejemplo:
```bash
make run_champsim
```
