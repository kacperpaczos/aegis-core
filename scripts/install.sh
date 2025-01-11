#!/bin/bash

# Kolory do formatowania wyjścia
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Funkcje pomocnicze do logowania
log_info() { echo -e "${GREEN}[INFO] $1${NC}"; }
log_warn() { echo -e "${YELLOW}[WARN] $1${NC}"; }
log_error() { echo -e "${RED}[ERROR] $1${NC}"; }

# Lista wymaganych pakietów
PACKAGES=(
    "build-essential"
    "g++"
    "meson"
    "ninja-build"
    "pkg-config"
    "libpoco-dev"
)

# Aktualizacja repozytoriów
log_info "Aktualizacja repozytoriów..."
sudo apt-get update || {
    log_error "Nie udało się zaktualizować repozytoriów"
    exit 1
}

# Instalacja pakietów
log_info "Instalacja wymaganych pakietów..."
for package in "${PACKAGES[@]}"; do
    if dpkg -l | grep -q "^ii.*$package"; then
        log_info "Pakiet $package jest już zainstalowany"
    else
        log_info "Instalacja pakietu $package..."
        sudo apt-get install -y "$package" || {
            log_error "Nie udało się zainstalować pakietu $package"
            exit 1
        }
    fi
done

log_info "Instalacja zależności zakończona pomyślnie!"
log_info "Możesz teraz zbudować projekt używając ./scripts/start.sh"
