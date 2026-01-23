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
    #include <unistd.h>
    #include <ncurses.h>

typedef struct pids_s {
    int first_pid;
    int second_pid;
} pids_t;


class Piece {
    int color;
    int *moves;

    public:
        int x;
        int y;
        char piece;
        void setup(char pi, int col, int x_t, int y_t) {
            piece = pi;
            color = col;
            x = x_t;
            y = y_t;
        };
        void set_move(int *posibility) {
            moves = posibility;
        };
        int can_move(int mx, int my) {
            for (int i = 0; moves[i] != -50 && moves[i] != 50; i += 2)
                if (mx == x + moves[i] && my == y + moves[i + 1])
                    return 1;
            return 0;
        };
        void print(void) {
            attron(COLOR_PAIR(color));
            printw("%c", piece);
            attroff(COLOR_PAIR(color));
        };
};

int my_isnum(char const *str);
int my_getnbr(char const *str);
int run(pids_t pids);
int *char_to_bin(char c);
void send(int *bin, int pid);
int bin_to_dec(int *bin);
int piece_ir(int x, int y, Piece *ourPieces);
void print_attack(char *pos, Piece *ourPieces);
void print_dep(char *pos, Piece *ourPieces, Piece *ennemyPieces);
Piece *init_player(int color, int direction);

#endif /* !MY_H_ */
