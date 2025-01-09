#!/bin/bash

# Definicje kolorów
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Funkcje pomocnicze
log_info() { echo -e "${GREEN}ℹ️ $1${NC}"; }
log_warn() { echo -e "${YELLOW}⚠️ $1${NC}"; }
log_error() { echo -e "${RED}❌ $1${NC}"; }

# Przejście do katalogu głównego projektu
cd "$(dirname "$0")/.." || exit 1

# Katalog build
BUILD_DIR="build"

# Czyszczenie katalogu build
if [ -d "$BUILD_DIR" ]; then
    log_info "Czyszczenie katalogu build..."
    rm -rf "$BUILD_DIR"
    log_info "Katalog build wyczyszczony"
else
    log_warn "Katalog build nie istnieje"
fi

log_info "Czyszczenie zakończone pomyślnie!" 