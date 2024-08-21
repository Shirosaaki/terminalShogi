/*
** EPITECH PROJECT, 2024
** send
** File description:
** send
*/
#include "include/my.h"

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
}
