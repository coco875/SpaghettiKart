#!/bin/bash
set -e

BUILD_TYPE="${1:-Release}"
BUILD_DIR="build"
IMAGE_NAME="spaghettikart-builder"

# Build Docker image if needed
if ! docker image inspect "${IMAGE_NAME}" &>/dev/null; then
    echo "Building Docker image..."
    docker build -t "${IMAGE_NAME}" .
fi

# Clean build directory if it contains incompatible cache
if [[ -f "${BUILD_DIR}/CMakeCache.txt" ]] && grep -q "/home/coco" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
    echo "Cleaning incompatible build cache..."
    rm -rf "${BUILD_DIR}"
fi

# Run build in Docker
echo "Building in Docker container..."
docker run --rm \
    -v "$(pwd):/project" \
    -e BUILD_TYPE="${BUILD_TYPE}" \
    "${IMAGE_NAME}" \
    bash -c "
        cmake -B ${BUILD_DIR} -G Ninja \
            -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
            -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
            -DVCPKG_TARGET_TRIPLET=x64-linux && \
        cmake --build ${BUILD_DIR} --parallel
    "

echo "Build complete: ${BUILD_DIR}/Spaghettify"
