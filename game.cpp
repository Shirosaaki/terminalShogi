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

void print_map(Piece *ourPieces, Piece *ennemyPieces)
{
    printw("\n-------------------------------------\n");
    for (int i = 1; i < 10; i++) {
        printw("|");
        for (int j = 0; j < 10; j++) {
            bool printed = false;
            for (int k = 0; k < 20; k++) {
                if (ourPieces[k].x == j && ourPieces[k].y == i) {
                    ourPieces[k].print();
                    printed = true;
                    break;
                } else if (ennemyPieces[k].x == j && ennemyPieces[k].y == i) {
                    ennemyPieces[k].print();
                    printed = true;
                    break;
                }
            }
            if (!printed && j != 0)
                printw(" ");
            if (j != 9 && j != 0)
                printw(" | ");
            else
                printw(" ");
        }
        printw("|\n");
        printw("-------------------------------------\n");
    }
}

int attack(int me, int who_play, pids_t pids, Piece *ourPieces, Piece *ennemyPieces)
{
    int who_not_play = pids.first_pid == who_play ? pids.second_pid : pids.first_pid;
    char *pos;
    size_t len = 0;
    int *result = (int *)malloc(sizeof(int) * 8);
    int *bin;
    char p;
    int *pos_int = (int *)malloc(sizeof(int) * 2);

    print_map(ourPieces, ennemyPieces);
    refresh();
    free(result);
    return 1;
    /*if (me == who_play) {
        print_attack(&pos, map, &len);
        bin = char_to_bin(pos[0]);
        send(bin, who_not_play);
        bin = char_to_bin(pos[2]);
        send(bin, who_not_play);
    } else {
        for (int i = 0; i < 8; i++) {
            pause();
            result[i] = sign;
            printf("%d ", result[i]);
            kill(who_play, SIGUSR2);
            if (i == 3)
                kill(who_play, SIGUSR1);
        }
        printf("\n");
        pos_int[0] = bin_to_dec(result) - 1;
        pos_int[1] = bin_to_dec(result + 4) - 1;
        p = map[pos_int[0]][pos_int[1]];
        printf("Piece: %c\n", p);
    }*/
}


int run(pids_t pids)
{
    int win = 0;
    int me = getpid();
    int who_play = pids.first_pid;
    struct sigaction sa;
    Piece *ourPieces;
    Piece *ennemyPieces;

    if (me == who_play) {
        ourPieces = init_player(1, 1);
        ennemyPieces = init_player(2, -1);
    } else {
        ourPieces = init_player(2, -1);
        ennemyPieces = init_player(1, 1);
    }
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler_sig;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    initscr();
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    noecho();
    curs_set(0);
    refresh();
    do {
        clear();
        attack(me, who_play, pids, ourPieces, ennemyPieces);
        refresh();
        who_play = pids.first_pid == who_play ? pids.second_pid : pids.first_pid;
    } while (getch() != 27 && win == 0);
    endwin();
    return win;
}
