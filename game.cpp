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

char **create_map(void)
{
    char** map = (char**)malloc(9 * sizeof(char*));
    const char* rows[] = {
        "LCAOROACL",
        " T     F ",
        "PPPPPPPPP",
        "         ",
        "         ",
        "         ",
        "PPPPPPPPP",
        " F     T ",
        "LCAOROACL"
    };

    for (int i = 0; i < 9; ++i) {
        map[i] = (char*)malloc(10 * sizeof(char));
        strcpy(map[i], rows[i]);
    }
    return map;
}

void print_map(char **map)
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

int attack(int me, int who_play, pids_t pids, Piece *ourPieces, Piece *ennemyPieces)
{
    int who_not_play = pids.first_pid == who_play ? pids.second_pid : pids.first_pid;
    char *pos;
    size_t len = 0;
    int *result = (int *)malloc(sizeof(int) * 8);
    int *bin;
    char p;
    int *pos_int = (int *)malloc(sizeof(int) * 2);
    
    for (int i = 0; i < 20; i++) {
        ourPieces[i].print();
        //ennemyPieces[i].print();
    }
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
    char **map = create_map();
    int me = getpid();
    int who_play = pids.first_pid;
    struct sigaction sa;
    Piece *ourPieces;
    Piece *ennemyPieces;

    if (me == who_play) {
        ourPieces = init_player(COLOR_CYAN, 1);
        ennemyPieces = init_player(COLOR_YELLOW, -1);
    } else {
        ourPieces = init_player(COLOR_YELLOW, -1);
        ennemyPieces = init_player(COLOR_CYAN, 1);
    }
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler_sig;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    print_map(map);
    initscr();
    start_color();
    noecho();
    curs_set(0);
    refresh();
    while (getch() != 27) {
        clear();
        attack(me, who_play, pids, ourPieces, ennemyPieces);
        refresh();
        who_play = pids.first_pid == who_play ? pids.second_pid : pids.first_pid;
    }
    endwin();
    return win;
}
