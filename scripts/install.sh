#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
INSTALL_PREFIX="${1:-${ROOT_DIR}/dist/newstep-mct}"
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"
cmake --build "${BUILD_DIR}" --config Release -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)"
cmake --install "${BUILD_DIR}"
echo "Installed to ${INSTALL_PREFIX}"
echo "Run: ${INSTALL_PREFIX}/bin/newstep-mct"
