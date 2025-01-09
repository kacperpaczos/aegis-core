#!/bin/bash

# Definicje kolorów i stałych
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

BUILD_DIR="build"

# Funkcje pomocnicze
log_info() { echo -e "${GREEN}ℹ️ $1${NC}"; }
log_warn() { echo -e "${YELLOW}⚠️ $1${NC}"; }
log_error() { echo -e "${RED}❌ $1${NC}"; }

check_dependencies() {
    local deps=("g++" "meson" "ninja-build" "libssl-dev" "openssl" "libpoco-dev")
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null && ! dpkg -l | grep -q "^ii.*$dep"; then
            log_warn "Brak wymaganej zależności: $dep"
            sudo apt-get update
            sudo apt-get install -y "$dep"
            
            if [ "$dep" = "libpoco-dev" ]; then
                sudo apt-get install -y libpocofoundation-dev libpoconet-dev libpocojson-dev libpocoutil-dev
            fi
        fi
    done
}

setup_directories() {
    mkdir -p "${BUILD_DIR}"
}

build_project() {
    log_info "Konfiguracja projektu..."
    cd "${BUILD_DIR}" || exit 1
    meson setup .. || {
        log_error "Błąd konfiguracji Meson"
        cd ..
        exit 1
    }

    log_info "Kompilacja projektu..."
    ninja || {
        log_error "Błąd kompilacji"
        cd ..
        exit 1
    }
    cd ..
}

run_server() {
    log_info "🚀 Uruchamianie serwera..."
    log_info "Naciśnij Ctrl+C aby zakończyć"
    "${BUILD_DIR}/aegis-core"
}

# Główny skrypt
log_info "🚀 Inicjalizacja projektu aegis-core..."
check_dependencies
setup_directories
build_project
run_server