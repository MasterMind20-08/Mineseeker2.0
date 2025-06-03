#pragma once
#include <vector>
#include <random>
#include <SFML/Graphics.hpp>

#include "Cell.h"
#include "Global.h"

class Field
{
public:
    Field();

    void draw(sf::RenderWindow& i_window);
    void flag_cell(unsigned char i_x, unsigned char i_y);
    void open_cell(unsigned char i_x, unsigned char i_y);
    void restart();
    void set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y);

    void next_level(); // добавлено: смена уровня через индекс
    bool effect_over();
    char get_game_over();
    unsigned short get_flags();

private:
    std::random_device random_device;
    std::default_random_engine random_engine;

    sf::Texture icons_texture;

    std::vector<GameSettings> levels; // добавлено: список уровней
    size_t level_index;               // добавлено: текущий индекс уровня

    std::vector<Cell> cells;

    char game_over;
    char first_click;

    void apply_level_settings(); // добавлено: установка размеров и мин
};
