#!/bin/bash

set -e

N=${1:-100}
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================"
echo " Lab 3, Task 2: Client-Server"
echo "======================================"

echo ""
echo "[1/3] Building..."
cmake -B build -S . -DCMAKE_CXX_COMPILER=g++
cmake --build build -j$(nproc)

BIN_DIR="build/bin/g++"

echo ""
echo "[2/3] Running client_server with N=$N ..."
"$BIN_DIR/client_server" "$N"

echo ""
echo "[3/3] Running verification test..."
"$BIN_DIR/test_results"

echo ""
echo "======================================"
echo " Done!"
echo "======================================"
