/*
** EPITECH PROJECT, 2024
** game
** File description:
** game
*/
#include "include/my.h"

extern int sign;

void handler_sig(int sig, siginfo_t *si, void *unused)
{
    if (sig == SIGUSR1)
        sign = 0;
    else if (sig == SIGUSR2)
        sign = 1;
}

char (*create_map(void))[10]
{
    static char map[9][10] = {
        {"LCAOROACL"},
        {" T     F "},
        {"PPPPPPPPP"},
        {"         "},
        {"         "},
        {"         "},
        {"PPPPPPPPP"},
        {" F     T "},
        {"LCAOROACL"}
    };
    return map;
}

void print_map(char (*map)[10])
{
    printf("\n------------------");
    printf("-------------------\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("| %c ", map[i][j]);
            if (j == 8)
                printf("|");
        }
        printf("\n------------------");
        printf("-------------------\n");
    }
}

void attack(int me, int who_play, pids_t pids)
{
    int who_not_play = pids.first_pid == who_play ? pids.second_pid : pids.first_pid;
    char *pos;
    size_t len = 0;
    int result[9];
    int *bin;

    if (me == who_play) {
        printf("Witch piece do you want to move (x x) : ");
        getline(&pos, &len, stdin);
        bin = char_to_bin(pos[0]);
        send(bin, who_not_play);
    } else {
        bin = (int *)malloc(sizeof(int) * 4);
        for (int i = 0; i < 4; i++) {
            pause();
            bin[i] = sign;
            kill(who_play, SIGUSR2);
            printf("%d ", bin[i]);
        }
        printf("\n");
        //kill(who_play, SIGUSR2);
    }
}

int run(pids_t pids)
{
    int win = 0;
    char (*map)[10] = (char (*)[10])create_map();
    int me = getpid();
    int who_play = pids.first_pid;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler_sig;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    print_map(map);
    while (win == 0) {
        attack(me, who_play, pids);
        who_play = pids.first_pid == who_play ? pids.second_pid : pids.first_pid;
    }
    return win;
}
