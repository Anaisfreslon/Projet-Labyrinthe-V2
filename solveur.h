#ifndef SOLVEUR_H
#define SOLVEUR_H

struct Cell {
    struct Cell *adjacent_cells[4];
    int x, y;
    int visite;
    int chemin;
    struct Cell *parent;
};

void resolution(struct Cell** grid, int width, int height);
void afficher_labyrinthe(struct Cell** grid, int width, int height);



#endif