#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include <stdbool.h>

enum Direction {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

// Structure d'une cellule (commune au générateur et au solveur)
struct Cell {
    struct Cell *adjacent_cells[4];
    bool visited;
    int x;
    int y;
    int visite;
    int chemin;
    struct Cell *parent;
};

// Structure de la grille (utilisée par le générateur)
struct Grid {
    struct Cell **cells;
    int width;
    int height;
};

#endif
