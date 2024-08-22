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
    char piece;
    short color;
    int *moves;
    int x;
    int y;

    public:
        void setup(char pi, short col, int x_t, int y_t) {
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
        void print() {
            init_pair(1, color, COLOR_BLACK);
            attron(COLOR_PAIR(1));
            mvprintw(y, x, "%c", piece);
        }
};


class Pieces {
    public:
        int piece;
        int can_move;
        int x_move;
        int y_move;

        void get_piece(char pi) {
            switch (pi)
            {
            case 'P':
                piece = 1;
            case 'T':
                piece = 2;
            case 'F':
                piece = 3;
            case 'C':
                piece = 4;
            case 'O':
                piece = 5;
            case 'A':
                piece = 6;
            case 'L':
                piece = 7;
            default:
                piece = 8;
            }
        }
        /*void move(int x, int y, int mx, int my, char **map) {
            switch (piece)
            {
            case 1:
                move_p(x, y, mx, my, map);
            case 2:
                move_t(x, y, mx, my, map);
            case 3:
                move_f(x, y, mx, my, map);
            case 4:
                move_c(x, y, mx, my, map);
            case 5:
                move_o(x, y, mx, my, map);
            case 6:
                move_a(x, y, mx, my, map);
            case 7:
                move_l(x, y, mx, my, map);
            default:
                move_r(x, y, mx, my, map);
            }
        }

        void move_p(int x, int y, int mx, int my, char **map) {
            if (my == y + 1 && mx == x && ) {

            }
        }*/
};

int my_isnum(char const *str);
int my_getnbr(char const *str);
int run(pids_t pids);
int *char_to_bin(char c);
void send(int *bin, int pid);
int bin_to_dec(int *bin);
int piece_ir(int x, int y, char **map);
void print_attack(char **pos, char **map, size_t *len);
Piece *init_player(short color, int direction);

#endif /* !MY_H_ */
