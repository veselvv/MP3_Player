#!/bin/bash

set -e

echo "🔍 Проверка зависимостей..."

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Функция проверки через компиляцию
check_lib() {
    local lib_name=$1
    local pkg_name=$2
    local cflags=$3
    local libs=$4
    local test_code=$5

    echo -n "   Проверка $lib_name... "
    if echo "$test_code" | gcc -x c - -o /dev/null $(pkg-config --cflags --libs $pkg_name) 2>/dev/null; then
        echo -e "${GREEN}OK${NC}"
        return 0
    else
        echo -e "${RED}НЕ НАЙДЕН${NC}"
        echo "      $lib_name не найден. Установите:"
        case "$lib_name" in
            GTK4) echo "        sudo apt install libgtk-4-dev" ;;
            SDL2) echo "        sudo apt install libsdl2-dev" ;;
            SDL2_mixer) echo "        sudo apt install libsdl2-mixer-dev" ;;
            mpg123) echo "        sudo apt install libmpg123-dev" ;;
            *) echo "        Установите $lib_name вручную." ;;
        esac
        return 1
    fi
}

# Проверяем компилятор и make
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}❌ gcc не найден${NC}"
    exit 1
fi
echo -e "${GREEN}✅ gcc найден${NC}"

if ! command -v make &> /dev/null; then
    echo -e "${RED}❌ make не найден${NC}"
    exit 1
fi
echo -e "${GREEN}✅ make найден${NC}"

if ! command -v pkg-config &> /dev/null; then
    echo -e "${RED}❌ pkg-config не найден${NC}"
    exit 1
fi
echo -e "${GREEN}✅ pkg-config найден${NC}"

# Проверка GTK4
echo "Проверка GTK4:"
check_lib "GTK4" "gtk4" "" "" '#include <gtk/gtk.h>
int main() { gtk_init(); return 0; }'

# Проверка SDL2
echo "Проверка SDL2:"
check_lib "SDL2" "sdl2" "" "" '#include <SDL2/SDL.h>
int main() { SDL_Init(0); return 0; }'

# Проверка SDL2_mixer
echo "Проверка SDL2_mixer:"
check_lib "SDL2_mixer" "SDL2_mixer" "" "" '#include <SDL2/SDL_mixer.h>
int main() { Mix_Init(0); return 0; }'


# Проверка mpg123 (без pkg-config, так как .pc файл часто отсутствует)
echo "Проверка mpg123:"
if echo '#include <mpg123.h>
int main() { mpg123_init(); return 0; }' | gcc -x c - -o /dev/null -lmpg123 2>/dev/null; then
    echo -e "${GREEN}OK${NC}"
else
    echo -e "${RED}НЕ НАЙДЕН${NC}"
    echo "      mpg123 не найден. Установите:"
    echo "        sudo apt install libmpg123-dev   # Ubuntu/Debian"
    echo "        sudo dnf install mpg123-devel    # Fedora"
    echo "        sudo pacman -S mpg123            # Arch"
    exit 1
fi

echo -e "${GREEN}✅ Все зависимости установлены.${NC}"