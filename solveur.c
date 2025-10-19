#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "solveur.h"
#include "labyrinthe.h"



// Une file qui nous permet d'utiliser le Breadth first Search
struct Queue{
    struct Cell **array; // tableau de pointeurs vers des cellules
    int front;           // index du premier élément
    int size;            // nombre d'éléments dans la file
    int capacity;        // taille max du tableau (s'agrandit si besoin)
};

// Initialiser une file vide
struct Queue queue_init(){
    struct Queue queue;
    queue.front = 0;
    queue.size = 0;
    queue.capacity = 4; // taille de départ (peut doubler si nécessaire)
    queue.array = malloc(queue.capacity * sizeof(struct Cell*));
    return queue;
}

// Doubler la capacité de la file si elle est pleine
void agrandir_queue(struct Queue *queue) {
    int new_capacity = queue->capacity * 2;
    struct Cell** new_array = malloc(new_capacity * sizeof(struct Cell*));

    // On recopie les éléments dans le bon ordre
    for (int i = 0; i < queue->size; i++) {
        new_array[i] = queue->array[(queue->front + i) % queue->capacity];
    }

    free(queue->array);   // on libère l'ancien tableau
    queue->array = new_array;
    queue->capacity = new_capacity;
    queue->front = 0;    
}

// Ajouter un élément à la fin de la file
void queue_push(struct Queue *queue, struct Cell *cell) {
    if (queue->size == queue->capacity) {
        agrandir_queue(queue); // si la file est pleine, on l'agrandit
    }
    int index = (queue->front + queue->size) % queue->capacity; // donne le position où insérer
    queue->array[index] = cell;
    queue->size++;
}

// cette fonction sert à enlever les l'élément en début de file
struct Cell* queue_pop(struct Queue *queue) {
    if (queue->size == 0) {
        return NULL; // file vide
    }
    struct Cell* cell = queue->array[queue->front]; // récupérer la cellule en tête
    queue->front = (queue->front + 1) % queue->capacity; // avancer le front (cercle)
    queue->size--;
    return cell;
}


int queue_is_empty(struct Queue *queue) {
    return queue->size == 0;
}

// Libérer la mémoire utilisée par la file
void queue_free(struct Queue *queue) {
    free(queue->array);
}

// === Algorithme de résolution BFS ===
void resolution(struct Cell** grid, int width, int height) {
    // printf("Checkpoint -1");
    struct Queue queue = queue_init();

    struct Cell* start = &grid[0][0];                // départ en haut à gauche
    struct Cell* end = &grid[height - 1][width - 1]; // arrivée en bas à droite

    // Réinitialiser les infos de toutes les cellules
    for(int i = 0; i < height; i++){
        for(int j = 0; j< width; j++){
            grid[i][j].visite = 0;
            grid[i][j].parent = NULL;
            grid[i][j].chemin =0;
        }
    }

    // On marque la case de départ comme visitée
    start->visite = 1;
    queue_push(&queue, start);

    // printf("Checkpoint 0");

    // Parcours BFS
    while (!queue_is_empty(&queue)) {
        struct Cell* cur = queue_pop(&queue);

        if (cur == end) break; // si on a atteint la sortie, on arrête

        // explorer les voisins
        for (enum Direction d = NORTH; d <= WEST; d++) {
            struct Cell* nb = cur->adjacent_cells[d];
            if (nb != NULL && nb->visite == 0) {
                nb->visite = 1;
                nb->parent = cur;       // permet de savoir d'où on vient
                queue_push(&queue, nb); // on ajoute le voisin à explorer
            }
        }
    }


    struct Cell *cell = end;
    while(cell != NULL){
        cell->chemin = 1;
        cell = cell -> parent;
    }


    queue_free(&queue);
}
