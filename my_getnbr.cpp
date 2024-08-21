/*
** EPITECH PROJECT, 2024
** getnb
** File description:
** getnbr
*/
#include "include/my.h"

int my_getnbr(char const *str)
{
    int nb = 0;
    int neg = 1;
    int i = 0;

    for (; str[i] == '+' || str[i] == '-'; i++)
        if (str[i] == '-')
            neg *= -1;
    for (; str[i] >= '0' && str[i] <= '9'; i++) {
        nb *= 10;
        nb += str[i] - '0';
    }
    return nb * neg;
}

int my_isnum(char const *str)
{
    int i = 0;

    for (; str[i] == '+' || str[i] == '-' ||
             (str[i] >= '0' && str[i] <= '9'); i++);
    return i == strlen(str) ? 1 : 0;
}

int *char_to_bin(char c)
{
    int *bin = (int *)malloc(sizeof(int) * 4);
    int b = c - '0';

    for (int i = 3; i >= 0; i--) {
        bin[i] = b % 2;
        b /= 2;
    }
    return bin;
}