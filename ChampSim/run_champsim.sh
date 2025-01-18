#!/bin/bash

if [ "$#" -lt 1 ]; then
    echo "Illegal number of parameters"
    echo "Usage: ./run_champsim.sh [TRACE] [OPTION]"
    exit 1
fi

TRACE_DIR=$PWD/traces
TRACE=${1}
OPTION=${2}
BINARY=champsim
N_WARM=0
N_SIM=2

# Sanity check
if [ -z $TRACE_DIR ] || [ ! -d "$TRACE_DIR" ] ; then
    echo "[ERROR] Cannot find a trace directory: $TRACE_DIR"
    exit 1
fi

if [ ! -f "bin/$BINARY" ] ; then
    echo "[ERROR] Cannot find a ChampSim binary: bin/$BINARY"
    exit 1
fi

re='^[0-9]+$'
if ! [[ $N_WARM =~ $re ]] || [ -z $N_WARM ] ; then
    echo "[ERROR]: Number of warmup instructions is NOT a number" >&2;
    exit 1
fi

re='^[0-9]+$'
if ! [[ $N_SIM =~ $re ]] || [ -z $N_SIM ] ; then
    echo "[ERROR]: Number of simulation instructions is NOT a number" >&2;
    exit 1
fi

if [ ! -f "$PWD/$TRACE" ] ; then
    echo "[ERROR] Cannot find a trace file: $PWD/$TRACE"
    exit 1
fi

(./bin/${BINARY} -warmup_instructions ${N_WARM}000000 -simulation_instructions ${N_SIM}000000 ${OPTION} -traces ${PWD}/${TRACE})
