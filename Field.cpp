#include <random>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem> 

#include "Headers/Cell.h"
#include "Headers/Field.h"
#include "Headers/GetCell.h"
#include "Headers/Global.h"

Field::Field() :
	game_over(1),
	random_engine(random_device()),
	level_index(0)
{
	apply_level_settings();
	restart();
}

void Field::apply_level_settings()
{

	cells.clear();
	for (unsigned char a = 0; a < settings.rows; a++)
	{
		for (unsigned char b = 0; b < settings.columns; b++)
		{
			cells.push_back(Cell(b, a));
		}
	}
	game_over = 0;
	first_click = 0;
}

void Field::next_level()
{
	level_index = (level_index + 1) % levels.size();
	apply_level_settings();
	restart();
}

bool Field::effect_over()
{
	for (Cell& cell : cells)
	{
		if (0 < cell.get_effect_timer())
		{
			return 0;
		}
	}
	return 1;
}

char Field::get_game_over()
{
	return game_over;
}

unsigned short Field::get_flags()
{
	unsigned short total_flags = 0;
	for (Cell& cell : cells)
	{
		total_flags += cell.get_is_flagged();
	}
	return total_flags;
}

void Field::draw(sf::RenderWindow& i_window)
{
	sf::RectangleShape cell_shape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
	sf::Sprite icon_sprite;

	if (icons_texture.getSize().x == 0)
	{
		std::string texturePath = "Resourses/Images/Icons" + std::to_string(CELL_SIZE) + ".png";
		std::cout << "Попытка загрузить текстуру из: " << std::filesystem::absolute(texturePath) << std::endl;
		if (!icons_texture.loadFromFile(texturePath))
		{
			std::cerr << "Ошибка: не удалось загрузить текстуру!" << std::endl;
			if (!std::filesystem::exists(texturePath))
			{
				std::cerr << "Файл не существует по этому пути!" << std::endl;
			}
		}
	}

	icon_sprite.setTexture(icons_texture);

	for (unsigned char a = 0; a < settings.columns; a++)
	{
		for (unsigned char b = 0; b < settings.rows; b++)
		{
			cell_shape.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
			Cell* cell = get_cell(a, b, cells);

			if (cell->get_is_open())
			{
				unsigned char mines_around = cell->get_mines_around();
				cell_shape.setFillColor(sf::Color(146, 182, 255));
				i_window.draw(cell_shape);
				if (mines_around > 0)
				{
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(CELL_SIZE * mines_around, 0, CELL_SIZE, CELL_SIZE));
					i_window.draw(icon_sprite);
				}
			}
			else
			{
				cell_shape.setFillColor(sf::Color(0, 73, 255));
				if (game_over == 0)
				{
					switch (cell->get_mouse_state())
					{
					case 1:
						cell_shape.setFillColor(sf::Color(36, 109, 255));
						break;
					case 2:
						cell_shape.setFillColor(sf::Color(0, 36, 255));
						break;
					}
				}
				i_window.draw(cell_shape);
				if (cell->get_is_flagged())
				{
					icon_sprite.setPosition(static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b));
					icon_sprite.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));
					i_window.draw(icon_sprite);
				}
			}
			cell->set_mouse_state(0);
			if (game_over != 0 && cell->get_effect_timer() < EFFECT_DURATION)
			{
				unsigned char effect_size = static_cast<unsigned char>(2 * round(0.5f * CELL_SIZE * ((EFFECT_DURATION - cell->get_effect_timer()) / static_cast<float>(EFFECT_DURATION))));
				std::uniform_int_distribution<unsigned short> effect_duration_distribution(1, EFFECT_DURATION - 1);
				cell_shape.setPosition(floor(CELL_SIZE * (0.5f + a) - 0.5f * effect_size), floor(CELL_SIZE * (0.5f + b) - 0.5f * effect_size));
				cell_shape.setSize(sf::Vector2f(effect_size, effect_size));
				cell_shape.setFillColor(game_over == -1 ? sf::Color(255, 36, 0) : sf::Color(255, 255, 255));
				i_window.draw(cell_shape);
				cell_shape.setSize(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
				if (cell->update_effect_timer())
				{
					if (a > 0 && get_cell(a - 1, b, cells)->get_effect_timer() == EFFECT_DURATION)
						get_cell(a - 1, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					if (b > 0 && get_cell(a, b - 1, cells)->get_effect_timer() == EFFECT_DURATION)
						get_cell(a, b - 1, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					if (a + 1 < settings.columns && get_cell(a + 1, b, cells)->get_effect_timer() == EFFECT_DURATION)
						get_cell(a + 1, b, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
					if (b + 1 < settings.rows && get_cell(a, b + 1, cells)->get_effect_timer() == EFFECT_DURATION)
						get_cell(a, b + 1, cells)->set_effect_timer(static_cast<unsigned char>(effect_duration_distribution(random_engine)));
				}
			}
		}
	}
}

void Field::flag_cell(unsigned char i_x, unsigned char i_y)
{
	if (0 == game_over)
	{
		get_cell(i_x, i_y, cells)->flag();
	}
}

void Field::open_cell(unsigned char i_x, unsigned char i_y)
{
	if (first_click == 0)
	{
		first_click = 1;

		// 1. Собираем все возможные координаты, кроме первой кликнутой
		std::vector<std::pair<unsigned char, unsigned char>> positions;
		for (unsigned char x = 0; x < settings.columns; ++x)
		{
			for (unsigned char y = 0; y < settings.rows; ++y)
			{
				if (x != i_x || y != i_y)
				{
					positions.emplace_back(x, y);
				}
			}
		}

		// 2. Перемешиваем координаты
		std::shuffle(positions.begin(), positions.end(), random_engine);

		// 3. Устанавливаем мины
		for (unsigned short a = 0; a < settings.mines; ++a)
		{
			auto [mx, my] = positions[a];
			get_cell(mx, my, cells)->set_mine();
		}

		// 4. Пересчитываем количество мин вокруг каждой клетки
		for (Cell& cell : cells)
		{
			cell.count_mines_around(cells);
		}
	}

	if (game_over == 0 && get_cell(i_x, i_y, cells)->get_is_flagged() == 0)
	{
		if (get_cell(i_x, i_y, cells)->open(cells) == 1)
		{
			game_over = -1;
		}
		else
		{
			unsigned short total_closed_cells = 0;
			for (Cell& cell : cells)
			{
				total_closed_cells += 1 - cell.get_is_open();
			}

			if (settings.mines == total_closed_cells)
			{
				game_over = 1;
				get_cell(i_x, i_y, cells)->set_effect_timer(EFFECT_DURATION - 1);
			}
		}
	}
}

void Field::restart()
{
	if (0 != game_over)
	{
		game_over = 0;
		for (Cell& cell : cells)
		{
			cell.reset();
		}
	}
}

void Field::set_mouse_state(unsigned char i_mouse_state, unsigned char i_x, unsigned char i_y)
{
	get_cell(i_x, i_y, cells)->set_mouse_state(i_mouse_state);
}
