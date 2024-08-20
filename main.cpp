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
        printf("my_pid: %d\nWaiting for ennemy connection...\n", pids.first_pid);
        pause();
        pids.second_pid = sign;
        kill(pids.second_pid, SIGUSR1);
        printf("Ennemy connected !\n");
    }
    run(pids);
    return 0;
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
