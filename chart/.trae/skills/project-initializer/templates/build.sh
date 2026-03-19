#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "=== Building Project ==="

cd "$PROJECT_ROOT"

echo "1. Building C++ core library..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
make -j$(nproc)

echo "2. Copying native library..."
mkdir -p ../lib
cp src/core/libproject_core.* ../lib/
cp src/bridge/libproject_bridge.* ../lib/

cd "$PROJECT_ROOT"

echo "3. Building Java wrapper..."
cd src/java
mvn clean install -DskipTests

echo "4. Building application..."
cd ../app
mvn clean package -DskipTests

echo "=== Build Complete ==="
echo "Native libraries: $PROJECT_ROOT/lib/"
echo "Application JAR: $PROJECT_ROOT/src/app/target/*.jar"
