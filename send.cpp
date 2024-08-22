/*
** EPITECH PROJECT, 2024
** send
** File description:
** send
*/
#include "include/my.h"

extern int sign;

int get_sig(int i)
{
    if (i == 0)
        return SIGUSR1;
    else
        return SIGUSR2;
}

void send(int *bin, int pid)
{
    for (int i = 0; i < 4; i++) {
        kill(pid, get_sig(bin[i]));
        pause();
    }
    free(bin);
}

int piece_ir(int x, int y, char **map)
{
    if (map[x][y] == ' ')
        return 1;
    return 0;
}

void print_attack(char **pos, char **map, size_t *len)
{
    printf("Witch piece do you want to move (x x) : ");
    getline(pos, len, stdin);
    while (piece_ir((*pos)[0] - '0' - 1, (*pos)[2] - '0' - 1, map) != 0) {
        printf("NO piece IR\n");
        printf("Witch piece do you want to move (x x) : ");
        getline(pos, len, stdin);
    }
}