#ifndef SELECT_MENU_H
#define SELECT_MENU_H

#include <windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include "strconv.h"

namespace {
const int SELECT_MENU_DEFAULT = 7;
const int SELECT_MENU_ORANGE = 12;
}

static void select_menu_setcolor(int color)
{
    ::SetConsoleTextAttribute(::GetStdHandle(STD_ERROR_HANDLE), color);
}

static void select_menu_gotoxy(int x, int y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    ::SetConsoleCursorPosition(::GetStdHandle(STD_ERROR_HANDLE), c);
}

static int select_menu(std::vector<std::string> choices)
{
    ::unicode_ostream uout(std::cerr, ::GetConsoleOutputCP());
    uout << choices.size() << std::endl;
    if (choices.size() == 0) return -1;
    std::size_t choice = 0;
    ::system("cls");
    for (;;)
    {
        for (std::size_t i=0; i<choices.size(); i++)
        {
            ::select_menu_gotoxy(0, i+1);
            if (i == choice)
            {
                ::select_menu_setcolor(SELECT_MENU_ORANGE);
            }
            else
            {
                ::select_menu_setcolor(SELECT_MENU_DEFAULT);
            }
            uout << choices[i];
        }
        ::select_menu_gotoxy(0, 0);
        ::select_menu_setcolor(SELECT_MENU_DEFAULT);
        uout << u8"実行したいメニューを上下キーで選択してください. (ENTER/決定、ESC/キャンセル)";
        char key = ::_getch();
        if (key == 27) // esc key
        {
            system("cls");
            return -1;
        }
        if (key == 72 && choice > 0) // up arrow key
        {
            choice--;
        }
        if (key == 80 && choice < choices.size() -1) // down arrow key
        {
            choice++;
        }
        if (key == '\r') // enter key
        {
            break;
        }
    }
    ::system("cls");
    return choice + 1;
}

#endif // SELECT_MENU_H
