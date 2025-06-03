#include <vector>

#include "Headers/Cell.h"
#include "Headers/GetCell.h"
#include "Headers/Global.h"

Cell::Cell(unsigned char i_x, unsigned char i_y) : //Работает с ячейками и их состояниями. 
	mines_around(0),
	x(i_x),
	y(i_y)
{
	reset(); //Фокусируемся на одной ячейке
}

bool Cell::get_is_flagged()
{
	return is_flagged;
}

bool Cell::get_is_mine()
{
	return is_mine;
}

bool Cell::get_is_open()
{
	return is_open;
}

bool Cell::open(std::vector<Cell>& i_cells)
{
	// Нельзя открыть уже открытую или флагнутую ячейку
	if (is_open || is_flagged)
		return false;

	is_open = true;

	// Если клетка - мина, активируем эффект
	if (is_mine)
	{
		effect_timer--;
		return true;
	}

	// Если мин вокруг нет, рекурсивно открываем соседей
	if (mines_around == 0)
	{
		for (char dx = -1; dx <= 1; ++dx)
		{
			for (char dy = -1; dy <= 1; ++dy)
			{
				int nx = x + dx;
				int ny = y + dy;

				// Пропустить саму себя и выход за границы
				if ((dx == 0 && dy == 0) ||
					nx < 0 || ny < 0 || nx >= settings.columns || ny >= settings.rows)
					continue;

				Cell* neighbor = get_cell(nx, ny, i_cells);
				if (neighbor)
					neighbor->open(i_cells); // Рекурсивно
			}
		}
	}

	return 0; // Не мина
}

bool Cell::update_effect_timer() //Возвращает true, если таймер достиг 0
{
	if (0 < effect_timer)
	{
		effect_timer--;

		if (0 == effect_timer)
		{
			return 1;
		}
	}

	return 0;
}

unsigned char Cell::get_effect_timer()
{
	return effect_timer;
}

unsigned char Cell::get_mines_around()
{
	return mines_around;
}

unsigned char Cell::get_mouse_state()
{
	return mouse_state;
}

void Cell::count_mines_around(std::vector<Cell>& i_cells)
{
	// Сброс количества
	mines_around = 0;

	// Мины не нуждаются в подсчете соседей
	if (is_mine)
		return;

	for (char dx = -1; dx <= 1; ++dx)
	{
		for (char dy = -1; dy <= 1; ++dy)
		{
			int nx = x + dx;
			int ny = y + dy;

			// Пропустить саму себя и выход за границы
			if ((dx == 0 && dy == 0) ||
				nx < 0 || ny < 0 || nx >= settings.columns || ny >= settings.rows)
				continue;

			Cell* neighbor = get_cell(nx, ny, i_cells);
			if (neighbor && neighbor->get_is_mine())
				++mines_around;
		}
	}
}

void Cell::flag() //Установка и снятие флага
{
	if (0 == is_open)
	{
		is_flagged = !is_flagged;
	}
}

void Cell::reset()
{
	is_flagged = 0;
	is_mine = 0;
	is_open = 0;
	effect_timer = EFFECT_DURATION;
	mouse_state = 0;
}

void Cell::set_effect_timer(unsigned char i_effect_timer) //Усиановка эффекта
{
	effect_timer = i_effect_timer;
}

void Cell::set_mine() //Установка мины
{
	is_mine = 1;
}

void Cell::set_mouse_state(unsigned char i_mouse_state) //Установка состояния мыши
{
	mouse_state = i_mouse_state;
}