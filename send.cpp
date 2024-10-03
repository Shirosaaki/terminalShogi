/*
** EPITECH PROJECT, 2024
** send
** File description:
** send
*/
#include "include/my.h"

extern int sign;

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
    free(bin);
}

int piece_ir(int x, int y, Piece *ourPieces)
{
    for (int i = 0; i < 20; i++) {
        if (ourPieces[i].x == x && ourPieces[i].y == y)
            return 1;
    }
    return 0;
}

void print_attack(char *pos, Piece *ourPieces)
{
    mvprintw(20, 0, "Witch piece do you want to move (x y) : ");
    getstr(pos);
    while (piece_ir(pos[0] - 48, pos[2] - 48, ourPieces) == 0) {
        printw("NO piece IR\n");
        printw("Witch piece do you want to move (x y) : ");
        getstr(pos);
    }
}

int no_piece_between(int x1, int y1, int x2, int y2, Piece *ourPieces, Piece *en)
{
    int x = x1;
    int y = y1;

    for (int i = 0; i < 20; i++) {
        if (ourPieces[i].x == x1 && ourPieces[i].y == y1) {
            if (ourPieces[i].piece == 'C')
                return 1;
        }
    }

    int dx = (x2 > x1) ? 1 : (x2 < x1) ? -1 : 0;
    int dy = (y2 > y1) ? 1 : (y2 < y1) ? -1 : 0;

    x += dx;
    y += dy;

    while (x != x2 || y != y2) {
        if (piece_ir(x, y, ourPieces) == 1 || piece_ir(x, y, en) == 1)
            return 0;
        x += dx;
        y += dy;
    }
    return 1;
}

int move_ir(int x1, int y1, int x2, int y2, Piece *ourPieces, Piece *en)
{
    for (int i = 0; i < 20; i++) {
        if (ourPieces[i].x == x1 && ourPieces[i].y == y1) {
            if (ourPieces[i].can_move(x2, y2) == 1
            && piece_ir(x2, y2, ourPieces) == 0) {
                if (no_piece_between(x1, y1, x2, y2, ourPieces, en) == 0)
                    return 0;
                ourPieces[i].x = x2;
                ourPieces[i].y = y2;
                if (piece_ir(x2, y2, en) == 1) {
                    for (int j = 0; j < 20; j++) {
                        if (en[j].x == x2 && en[j].y == y2) {
                            en[j].x = 20;
                            en[j].y = 0;
                            break;
                        }
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

void print_dep(char *pos, Piece *ourPieces, Piece *ennemyPieces)
{
    int p1 = pos[0] - 48;
    int p2 = pos[2] - 48;

    printw("Where do you want to move (x y) : ");
    getstr(pos);
    while (move_ir(p1, p2, pos[0] - 48, pos[2] - 48, ourPieces, ennemyPieces) == 0) {
        printw("NO move IR\n");
        printw("Where do you want to move (x y) : ");
        getstr(pos);
    }
}