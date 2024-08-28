/*
** EPITECH PROJECT, 2024
** main
** File description:
** main
*/
#include "include/my.h"

int sign = 0;

int help(void)
{
    printf("USAGE\n\t./shogi [first_player_pid]\n");
    printf("DESCRIPTION\n\tfirst_player_pid: only for the 2nd player.");
    printf(" He use for connect the seconde player to the first\n");
    return 0;
}

void handler(int sig, siginfo_t *si, void *unused)
{
    sign = si->si_pid;
}

int start(int pid)
{
    struct sigaction sa;
    pids_t pids;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigaction(SIGUSR1, &sa, NULL);
    if (pid != 0) {
        pids.second_pid = getpid();
        printf("my_pid: %d\n", pids.second_pid);
        pids.first_pid = pid;
        kill(pids.first_pid, SIGUSR1);
        pause();
        printf("Succesfully connected !\n");
    } else {
        pids.first_pid = getpid();
        printf("my_pid: %d\nWaiting for ennemy connection...\n",
            pids.first_pid);
        pause();
        pids.second_pid = sign;
        kill(pids.second_pid, SIGUSR1);
        printf("Ennemy connected !\n");
    }
    run(pids);
    return 0;
}

Piece *init_player(int color, int direction)
{
    Piece *player = (Piece *)malloc(sizeof(Piece) * 20);

    int moves_arr[20][37] = {
        {0, direction, -50},                                             // Pion
        {0, direction, -50}, {0, direction, -50}, {0, direction, -50},
        {0, direction, -50}, {0, direction, -50}, {0, direction, -50},
        {0, direction, -50}, {0, direction, -50},
        {0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 8, 0, 9, -50},        // Lancer
        {2, 1, 2, -1, -50},                                              // Cavalier
        {-1, -1, 0, -1, 1, -1, -1, 1, 1, 1, -50},                        // Archer
        {-1, -1, 0, -1, 1, -1, -1, 0, 1, 0, 0, -1, -50},                 // Onagre
        {-1, -1, -1, 1, -1, 0, 1, -1, 1, 0, 1, 1, 0, -1, 0, 1, -50},     // Roi
        {0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 8, 0, 9, -50},        // Lancer
        {2, 1, 2, -1, -50},                                              // Cavalier
        {-1, -1, 0, -1, 1, -1, -1, 1, 1, 1, -50},                        // Archer
        {-1, -1, 0, -1, 1, -1, -1, 0, 1, 0, 0, -1, -50},                 // Onagre
        {-1, -1, -1, 1, 1, -1, 1, 1, -50},                               // Fou
        {0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 7, 0, 8, 0, 9, 0, -50}  // Tour
    };
    int piece_types[20] = {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P', 'L', 'C', 'A', 'O', 'R', 'L', 'C', 'A', 'O', 'F', 'T'};
    int positions[20][2] = {
        {1, direction == 1 ? 3 : 7}, {2, direction == 1 ? 3 : 7}, {3, direction == 1 ? 3 : 7},
        {4, direction == 1 ? 3 : 7}, {5, direction == 1 ? 3 : 7}, {6, direction == 1 ? 3 : 7},
        {7, direction == 1 ? 3 : 7}, {8, direction == 1 ? 3 : 7}, {9, direction == 1 ? 3 : 7},
        {1, direction == 1 ? 1 : 9}, {2, direction == 1 ? 1 : 9}, {3, direction == 1 ? 1 : 9},
        {4, direction == 1 ? 1 : 9}, {5, direction == 1 ? 1 : 9}, {9, direction == 1 ? 1 : 9},
        {8, direction == 1 ? 1 : 9}, {7, direction == 1 ? 1 : 9}, {6, direction == 1 ? 1 : 9},
        {direction == 1 ? 8 : 2, direction == 1 ? 2 : 8}, {direction == 1 ? 2 : 8, direction == 1 ? 2 : 8}
    };
    int move_lengths[20] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 19, 4, 11, 13, 17, 19, 4, 11, 13, 9, 37};

    for (int i = 0; i < 20; i++) {
        player[i].setup(piece_types[i], color, positions[i][0], positions[i][1]);
        int *moves = (int *)malloc(move_lengths[i] * sizeof(int));
        for (int j = 0; j < move_lengths[i]; j++) {
            moves[j] = moves_arr[i][j] * (i == 13 ? 1 : direction);  // Only negate direction for certain pieces
        }
        player[i].set_move(moves);
    }
    return player;
}

int main(int ac, char **av)
{
    if ((ac == 2 && strcmp(av[1], "-h") == 0) || ac > 2)
        return help();
    if (ac == 1)
        start(0);
    else if (my_isnum(av[1]) == 1)
        start(my_getnbr(av[1]));
    else
        return help();
    return 0;
}
