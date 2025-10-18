# Rapport de projet - Labyrinthe

## Partie 1 - Générateur

#### Choix de l'algorithme

Pour faire ce projet, suite à une proposition de l'énoncé, j'ai utilisé l'algorithme **Randomized Depth-First Search** qui fonctionne selon le principe suivant.

À chaque case, l'algo va choisir une des cases libres aux points cardinaux (libre veut dire : non-visitée et dans le tableau). S'il n'y a pas de cases libres, l'algorithme va remonter case par case jusqu'à en trouver une de libre.

#### Explication de la méthode et du code utilisés

Afin de pouvoir utiliser une boucle for qui tourne pendant *width * height* , j'ai utilisé une fonction par **récurrence**. Mon but était que la fonction s'appelle tant qu'une case libre n'a pas été trouvée.

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

Ensuite je **vérifie** quelles sont les cellules adjacentes libres : d'abord en vérifiant qu'elle est bien dans le tableau (c'est-à-dire vérifier que lorsqu'une case est au bord du tableau, je précise que le côté bordé n'est pas libre) et ensuite en vérifiant qu'elle n'est pas déjà visitée grâce à l'attribut *visited* de *struct Cell*.

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

Elle permet de s'auto-rappeler lorsqu'aucune des cases adjacentes n'est displonible afin de s'exécuter sur la cellule précédente de la pile, elle fait pour cela appel à *stack_pop*, qui est une fonction permettant de supprimer la dernière valeur de la pile qui représente le chemin direct vers la cellule actuelle. 

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
