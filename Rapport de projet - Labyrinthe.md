# Rapport de projet - Labyrinthe

## Partie 0 - Liens importants

- https://github.com/ParzivalPy/projet-labyrinthe

- https://github.com/Anaisfreslon/Projet-Labyrinthe-V2

## Partie 1 - Générateur

#### Choix de l'algorithme

Pour faire ce projet, suite à une proposition de l'énoncé, j'ai utilisé l'algorithme **Randomized Depth-First Search** qui fonctionne selon le principe suivant.

À chaque case, l'algo va choisir une des cases libres aux points cardinaux (libre veut dire : non-visitée et dans le tableau). S'il n'y a pas de cases libres, l'algorithme va remonter case par case jusqu'à en trouver une de libre.

#### Explication de la méthode et du code utilisés

Afin de pouvoir utiliser une boucle for qui tourne pendant _width _ height\* , j'ai utilisé une fonction par **récurrence**. Mon but était que la fonction s'appelle tant qu'une case libre n'a pas été trouvée.

```c
actual_cell->visited = true;
int free_cells = 4;

bool available_cells[4];
for (int i = 0; i < 4; i++) {
    available_cells[i] = true;
}
```

Tout d'abord, j'**initialise** les variables et tableaux dont j'aurais besoin :

- Dire que la cellule à été visitée.

- Créer un tableau qui gardera en mémoire les cellules adjacentes étant disponibles ou non avec des booléens.

```c
for (int i = 0; i < 4; i++)
    {
        if ((i == NORTH && actual_cell->y == 0) || (i == SOUTH && actual_cell->y == grid->height - 1))
        {
            free_cells--;
            // printf("%d : Case NORTH ou SOUTH vide\n", i);
            available_cells[i] = false;
        }
        else if ((i == WEST && actual_cell->x == 0) || (i == EAST && actual_cell->x == grid->width - 1))
        {
            free_cells--;
            // printf("%d : Case EAST ou WEST vide\n", i);
            available_cells[i] = false;
        }
        else if (i == NORTH)
        {
            if (grid->cells[actual_cell->y - 1][actual_cell->x].visited == true)
            {
                free_cells--;
                // printf("%d : Case NORTH visitée\n", i);
                available_cells[i] = false;
            }
        }
        else if (i == SOUTH)
        {
            if (grid->cells[actual_cell->y + 1][actual_cell->x].visited == true)
            {
                free_cells--;
                // printf("%d : Case SOUTH visitée\n", i);
                available_cells[i] = false;
            }
        }
        else if (i == WEST)
        {
            if (grid->cells[actual_cell->y][actual_cell->x - 1].visited == true)
            {
                free_cells--;
                // printf("%d : Case WEST visitée\n", i);
                available_cells[i] = false;
            }
        }
        else if (i == EAST)
        {
            if (grid->cells[actual_cell->y][actual_cell->x + 1].visited == true)
            {
                free_cells--;
                // printf("%d : Case EAST visitée\n", i);
                available_cells[i] = false;
            }
        }
    }
```

Ensuite je **vérifie** quelles sont les cellules adjacentes libres : d'abord en vérifiant qu'elle est bien dans le tableau (c'est-à-dire vérifier que lorsqu'une case est au bord du tableau, je précise que le côté bordé n'est pas libre) et ensuite en vérifiant qu'elle n'est pas déjà visitée grâce à l'attribut _visited_ de _struct Cell_.

```c
if (free_cells == 0)
{
    stack_pop(stack);
    if (stack->size == 0) {
        printf("Stack vide, plus de cases à explorer.\n");
        print_grid(grid);
        exit(EXIT_SUCCESS);
    }
    return select_case(stack->array[stack->size-1], grid, stack);
}
```

Ici, il s'agit de la partie qui m'as donnée le plus de problèmes. En effet, quand elle n'est pas là, tout va bien jusqu'à qu'une cellule n'est plus de cellules adjacentes, mais quand cette condition est là, c'est la **segmentation fault**.

Elle permet de s'auto-rappeler lorsqu'aucune des cases adjacentes n'est displonible afin de s'exécuter sur la cellule précédente de la pile, elle fait pour cela appel à _stack_pop_, qui est une fonction permettant de supprimer la dernière valeur de la pile qui représente le chemin direct vers la cellule actuelle.

#### Présentation des autres parties du code

```c
struct Stack stack_init();
void stack_destroy(struct Stack* stack);
void stack_push(struct Stack* stack, struct Cell* data);
struct Cell *stack_pop(struct Stack* stack);
```

Ces quatre fonction permettent de gérer la pile en l'initialisant, rajoutant un élément, supprimant un élément et supprimer la pile.

```c
struct Cell;
enum Direction;
struct Grid;
```

Il s'agit la des structures utilisées dans le code :

- Cell qui représente une cellule et contient les quatre cellules adjacentes, si elle a été visitée, sa position x et y.

- Direction permet de remplacer [0, 1, 2, 3] par [NORTH, SOUTH, EAST, WEST] pour une plus grande clarté du texte

- Grid représente la grille, elle est composé d'un tableau des cellules qui la compose, de sa hauteur, de sa largeur.

```c
struct Grid *create_grid(int width, int height);
void print_grid(struct Grid *grid);
void free_grid(struct Grid *grid);
void fill_cells(struct Grid *grid);
```

Et voici les fonctions qui s'occupent de créer la grille et les cellules, d'afficher la grille et de la supprimer.

## Partie 2 - Solveur

Dans la partie de la résolution du labyrinthe, nous avons décidé d’utiliser l’algorithme Breadth First Search (BFS). Cet algorithme utilise la recherche en largeur, et nous permet d’explorer un graphe une grille pour trouver le chemin le plus court entre l’entrée et la sortie du labyrinthe.

Voici l’idée principale de l’algo BFS: on part du point de départ et on explore par tour toutes les cases voisines de cette première case, puis on fait la même chose avec les cases suivantes. Quand l’algorithme trouve la case de sortie, ça y est, le chemin les plus court a été retrouvé.

Pour réaliser la partie du solveur, j’ai utilisé une file: une structure qui nous permet d’ajouter les nouveaux éléments à la fin et de retirer l’élément au début lorsque la file est pleine.

# Représenter le labyrinthe

Je commence par définir les cellule de notre labyrinthe grâce à une structure “Cell”:

```c
struct Cell {
    struct Cell *adjacent_cells[4];
    int x, y;
    int visite;
    int chemin;
    struct Cell *parent;
};
```

Cette structure va donc nous permettre de repérer les coordonnées de la cellule dans la grille et permettra de vérifier si la cellule a déjà été visité. A la fin du code, la structure nous servira à reconstruire le chemin trouvé.

# Gestion de la File

Pour gérer l’algorithme BFS, nous avons crée une structure de file dynamique, qui pourra s’agrandir quand elle est pleine. Elle nous permet également de stocker les cellules à explorer dans le bon ordre.

```c
struct Queue {
    struct Cell **array;
    int front;
    int size;
    int capacity;
};
```

Pour agrandir la file, nous utilisons cette fonction, que nous avons vu plusieurs fois :

```c
void agrandir_queue(struct Queue *queue) {
    int new_capacity = queue->capacity * 2;
    struct Cell** new_array = malloc(new_capacity * sizeof(struct Cell*));

    for (int i = 0; i < queue->size; i++) {
        new_array[i] = queue->array[(queue->front + i) % queue->capacity];
    }

    free(queue->array);
    queue->array = new_array;
    queue->capacity = new_capacity;
    queue->front = 0;
}
```

# La résolution BFS

Voici la fonction principale qui permet d’effectuer le parcours BFS :

```c
void resolution(struct Cell** grid, int width, int height) {
    struct Queue queue = queue_init();
    struct Cell* start = &grid[0][0];
    struct Cell* end = &grid[height - 1][width - 1];

    start->visite = 1;
    queue_push(&queue, start);

    while (!queue_is_empty(&queue)) {
        struct Cell* cur = queue_pop(&queue);

        if (cur == end) break; // on a trouvé la sortie

        // exploration des voisins
        for (int d = 0; d < 4; d++) {
            struct Cell* nb = cur->adjacent_cells[d];
            if (nb && nb->visite == 0) {
                nb->visite = 1;
                nb->parent = cur;
                queue_push(&queue, nb);
            }
        }
    }

    // reconstruction du chemin
    struct Cell *cell = end;
    while (cell != NULL) {
        cell->chemin = 1;
        cell = cell->parent;
    }

    queue_free(&queue);
}
```

# Les éléments utilisés

Le labyrinthe peut être considéré comme une grille de cellules et chaque cellule a des pointeurs vers les voisins qui sont tout autour (nord, sud, est et ouest dans notre code).

- La file permet d’explorer les cellules à explorer dans le bon ordre. On ajoute à la fin les nouvelles cellules découvertes grâce à la fonction **push ,** et c’est grâce à la fonction **pop** qu’on vient d’explorer.
- Les indicateurs de suivi :
  **visite** : indique si la cellule a déjà été visitée
  **parents** permet de se souvenir les cellule où on est déjà passé
  **chemin** sert à marquer les cases appartenant à la solution

Grâce à ces éléments, nous sommes sûrs de ne pas repasser par les mêmes cases ou de se perdre.

# Etapes du BFS dans les détails

### I- Initialisation

On commence par tout remettre à zéro :

- Toutes les cellules sont marquées comme **non visitées**.
- La **file** est vide, puis on y met la cellule de départ.
- On marque cette cellule comme **visitée**.

### II- Boucle d’exploration

Tant que la file n’est pas vide :

1. On retire la première cellule de la file (celle à explorer).
2. On regarde tous ses **voisins** (Nord, Sud, Est, Ouest).
3. Pour chaque voisin accessible et non encore visité :
   - On le marque comme visité.
   - On enregistre son **parent** (pour reconstruire le chemin plus tard).
   - On l’ajoute à la file pour qu’il soit exploré ensuite.
4. Si on arrive sur la cellule de fin, on peut arrêter la recherche.

### III- Reconstruction du chemin

Quand la sortie est atteinte, on part de la cellule d’arrivée et on remonte en suivant les pointeurs parents jusqu’au départ.

Chaque cellule traversée est marquée comme faisant partie du **chemin solution**.

### IV- Affichage du résultat

Une fois le chemin trouvé, le programme affiche le labyrinthe dans le terminal, en mettant :

- `E` pour le départ
- `S` pour la sortie
- pour les cellules du chemin
- `.` pour les autres cases
