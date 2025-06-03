#include <vector>
#include "Headers/Cell.h"
#include "Headers/GetCell.h"
#include "Headers/Global.h"

//Преобразование координат ячейки в ее порядковый номер
Cell* get_cell(unsigned char i_x, unsigned char i_y, std::vector<Cell>& i_cells)
{
	return &i_cells[i_y * settings.columns + i_x];
}