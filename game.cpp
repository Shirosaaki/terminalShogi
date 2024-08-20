/*
** EPITECH PROJECT, 2024
** game
** File description:
** game
*/
#include "include/my.h"

char **create_map(void)
{
    static char map[9][18] = {
        {"L C A O R O A C L"},
        {"  T           F  "},
        {"P P P P P P P P P"},
        {"                 "},
        {"                 "},
        {"                 "},
        {"P P P P P P P P P"},
        {"  F           T  "},
        {"L C A O R O A C L"}
    };
    return (char **)map;
}

int run(pids_t pids)
{
    int win = 0;
    char (*map)[18] = (char (*)[18])create_map();

    for (int i = 0; i < 9; i++) {
        printf("%s\n", map[i]);
    }
    return win;
}