#!/bin/bash

echo "🔍 Проверка зависимостей..."

# Проверка gcc
if ! command -v gcc &> /dev/null; then
    echo "❌ gcc не найден. Установите build-essential."
    exit 1
fi
echo "✅ gcc найден"

# Проверка make
if ! command -v make &> /dev/null; then
    echo "❌ make не найден. Установите make."
    exit 1
fi
echo "✅ make найден"

# Проверка pkg-config
if ! command -v pkg-config &> /dev/null; then
    echo "❌ pkg-config не найден. Установите pkg-config."
    exit 1
fi
echo "✅ pkg-config найден"

# Проверка каждой зависимости
check_dep() {
    local dep=$1
    local pkg_name=$2
    local header=$3
    local libfile=$4

    if pkg-config --exists "$pkg_name" 2>/dev/null; then
        echo "✅ $dep найден (pkg-config)"
        return 0
    fi

    # Проверяем заголовочный файл
    if [ -f "$header" ]; then
        echo "✅ $dep найден (заголовок $header)"
        return 0
    fi

    # Проверяем библиотеку
    if ldconfig -p | grep -q "$libfile" || find /usr -name "lib$libfile.so*" -print -quit 2>/dev/null | grep -q .; then
        echo "✅ $dep найден (библиотека lib$libfile.so)"
        return 0
    fi

    echo "❌ $dep не найден."
    echo "   Для установки:"
    case "$dep" in
        gtk4) echo "     Ubuntu: sudo apt install libgtk-4-dev" ;;
        sdl2) echo "     Ubuntu: sudo apt install libsdl2-dev" ;;
        SDL2_mixer) echo "     Ubuntu: sudo apt install libsdl2-mixer-dev" ;;
        mpg123) echo "     Ubuntu: sudo apt install libmpg123-dev" ;;
        *) echo "     Установите $dep вручную." ;;
    esac
    return 1
}

check_dep "gtk4" "gtk4" "/usr/include/gtk-4.0/gtk/gtk.h" "gtk-4"
check_dep "sdl2" "sdl2" "/usr/include/SDL2/SDL.h" "SDL2"
check_dep "SDL2_mixer" "SDL2_mixer" "/usr/include/SDL2/SDL_mixer.h" "SDL2_mixer"
check_dep "mpg123" "mpg123" "/usr/include/mpg123.h" "mpg123"

echo "✅ Все зависимости проверены."