#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <windows.h>

#include <iostream>

#include "Headers/Cell.h"
#include "Headers/DrawText.h"
#include "Headers/Field.h"
#include "Headers/GetCell.h"
#include "Headers/Global.h"

void showDifficultyMenu()
{
    int choice = 0;

    while (true)
    {
        std::cout << "Выберите уровень сложности:\n"
            "1 - Легкий (9x9, 10 мин)\n"
            "2 - Средний (16x16, 40 мин)\n"
            "3 - Сложный (30x16, 99 мин)\n"
            "Ваш выбор: ";
        std::cin >> choice;

        if (choice == 1)
        {
            settings = GameSettings(9, 9, 10);
            break;
        }
        else if (choice == 2)
        {
            settings = GameSettings(16, 16, 40);
            break;
        }
        else if (choice == 3)
        {
            settings = GameSettings(30, 16, 99);
            break;
        }
        else
        {
            std::cout << "Неверный выбор. Попробуйте снова.\n";
        }
    }
    ShowWindow(GetConsoleWindow(), SW_HIDE);
}


int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Установка текущей директории — рядом с .exe
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::filesystem::path exe_dir = std::filesystem::path(buffer).parent_path();
    std::filesystem::current_path(exe_dir);

    showDifficultyMenu();

    // Основное окно игры
    sf::RenderWindow window(
        sf::VideoMode(CELL_SIZE * settings.columns * SCREEN_RESIZE, SCREEN_RESIZE * (FONT_HEIGHT + CELL_SIZE * settings.rows)),
        "Minesweeper",
        sf::Style::Close
    );
    window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * settings.columns, FONT_HEIGHT + CELL_SIZE * settings.rows)));

    Field field;
    unsigned lag = 0;
    sf::Event event;
    auto previous_time = std::chrono::steady_clock::now();

    while (window.isOpen())
    {
        unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();
        lag += delta_time;
        previous_time += std::chrono::microseconds(delta_time);

        while (lag >= FRAME_DURATION)
        {
            unsigned char mouse_cell_x = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).x / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, settings.columns - 1);
            unsigned char mouse_cell_y = std::clamp(static_cast<int>(floor(sf::Mouse::getPosition(window).y / static_cast<float>(CELL_SIZE * SCREEN_RESIZE))), 0, settings.rows - 1);

            lag -= FRAME_DURATION;

            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter)
                    field.restart();
                else if (event.type == sf::Event::MouseButtonReleased)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                        field.open_cell(mouse_cell_x, mouse_cell_y);
                    else if (event.mouseButton.button == sf::Mouse::Right)
                        field.flag_cell(mouse_cell_x, mouse_cell_y);
                }
            }

            if (FRAME_DURATION > lag)
            {
                window.clear();
                field.draw(window);

                if (field.effect_over())
                {
                    if (field.get_game_over() == 1)
                        draw_text(1, (CELL_SIZE * settings.columns - 8 * FONT_WIDTH) / 2, (CELL_SIZE * settings.rows - FONT_HEIGHT) / 2, "VICTORY!", window);
                    else
                        draw_text(1, (CELL_SIZE * settings.columns - 4 * FONT_WIDTH) / 2, (CELL_SIZE * settings.rows - 2 * FONT_HEIGHT) / 2, "GAME\nOVER", window);
                }

                draw_text(0, 0, CELL_SIZE * settings.rows, "Mines:" + std::to_string(settings.mines - field.get_flags()), window);
                window.display();
            }
        }
    }

    return 0;
}
