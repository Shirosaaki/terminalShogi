/*
** EPITECH PROJECT, 2024
** my
** File description:
** my
*/

#ifndef MY_H_
    #define MY_H_
    #include <cstdio>
    #include <cstring>
    #include <csignal>
    #include <cstdlib>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <fcntl.h>

typedef struct pids_s {
    int first_pid;
    int second_pid;
} pids_t;


int my_isnum(char const *str);
int my_getnbr(char const *str);

#endif /* !MY_H_ */
