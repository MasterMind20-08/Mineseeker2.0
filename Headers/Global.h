#pragma once

// Размер ячейки
constexpr unsigned char CELL_SIZE = 16;
// Длительность эффекта
constexpr unsigned char EFFECT_DURATION = 16;
// Размер шрифта
constexpr unsigned char FONT_HEIGHT = 16;
constexpr unsigned char FONT_WIDTH = 8;
// Масштаб окна
constexpr unsigned char SCREEN_RESIZE = 4;
// Длительность кадра
constexpr unsigned short FRAME_DURATION = 16667;

// Структура настроек игры (для уровней сложности)
struct GameSettings {
    unsigned char columns;
    unsigned char rows;
    unsigned short mines;

    GameSettings(unsigned char c, unsigned char r, unsigned short m)
        : columns(c), rows(r), mines(m) {
    }
};

// Настройки по умолчанию (будет заменена в main по выбору пользователя)
inline GameSettings settings(8, 8, 8);

// ✅ Добавляем реализацию функции прямо в заголовочный файл
inline void set_level(unsigned char c, unsigned char r, unsigned short m) {
    settings.columns = c;
    settings.rows = r;
    settings.mines = m;
};