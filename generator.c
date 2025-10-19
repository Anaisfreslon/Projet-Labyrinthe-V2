#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "labyrinthe.h"
#include "solveur.h"



// -------------------------------------
//             STACK
// -------------------------------------

struct Stack
{
    struct Cell** array;
    int size;
    int capacity;
};

struct Stack stack_init() 
{
    struct Cell** array = malloc(2 * sizeof(struct Cell*));
    if (array == NULL) 
    {
        fprintf(stderr, "Error allocating memory");
        exit(EXIT_FAILURE);
    }

    return (struct Stack) 
    {
        .array = array,
        .size = 0,
        .capacity = 2
    };
}

void stack_destroy(struct Stack* stack) 
{
    free(stack->array);
    stack->array = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

void stack_push(struct Stack* stack, struct Cell* data) 
{
    if (stack->size >= stack->capacity) 
    {
        struct Cell** new_array = realloc(stack->array, 2 * stack->capacity * sizeof(struct Cell*));
        if (new_array == NULL) 
        {
            fprintf(stderr, "Error allocating memory");
            free(stack->array);
            exit(EXIT_FAILURE);
        }
        stack->array = new_array;
        stack->capacity *= 2;
    }

    stack->array[stack->size++] = data;
}

struct Cell *stack_pop(struct Stack* stack) 
{
    if (stack->size == 0) 
    {
        fprintf(stderr, "No items in stack");
        exit(EXIT_FAILURE);
    }

    stack->size--;
    return stack->array[stack->size];
}

// -------------------------------------
//             GRID
// -------------------------------------


struct Grid *create_grid(int width, int height) 
{
    struct Grid *grid = malloc(sizeof(struct Grid));
    grid->width = width;
    grid->height = height;
    grid->cells = malloc(height * sizeof(struct Cell *));
    if (grid->cells == NULL) 
    {
        printf("Err: Row Attribution malloc failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < height; i++) 
    {
        grid->cells[i] = malloc(width * sizeof(struct Cell));
        if (grid->cells[i] == NULL) 
        {
            printf("Err: Cases (columns) Attribution malloc failed");
            exit(EXIT_FAILURE);
        }
    }
    return grid;
}

void print_grid_with_path(struct Grid *grid) {
    int width = grid->width;
    int height = grid->height;

    // ligne du haut
    for (int x = 0; x < width; x++) {
        printf("+---");
    }
    printf("+\n");

    for (int y = 0; y < height; y++) {
        char line[4 * width + 1];
        char interline[4 * width + 1];
        line[0] = '|';
        interline[0] = '+';

        for (int x = 0; x < width; x++) {
            struct Cell *cell = &grid->cells[y][x];

            // symbole à afficher
            char symbol = ' ';
            if (x == 0 && y == 0) symbol = 'S';                   // départ
            else if (x == width - 1 && y == height - 1) symbol = 'E'; // arrivée
            else if (cell->chemin) symbol = '*';                  // chemin BFS

            // ligne horizontale
            line[4 * x + 1] = ' ';
            line[4 * x + 2] = symbol;
            line[4 * x + 3] = ' ';
            line[4 * x + 4] = (cell->adjacent_cells[EAST] != NULL) ? ' ' : '|';

            // interligne
            if (y == height - 1) {
                interline[4 * x + 1] = '-';
                interline[4 * x + 2] = '-';
                interline[4 * x + 3] = '-';
                interline[4 * x + 4] = '+';
            } else {
                interline[4 * x + 1] = (cell->adjacent_cells[SOUTH] != NULL) ? ' ' : '-';
                interline[4 * x + 2] = (cell->adjacent_cells[SOUTH] != NULL) ? ' ' : '-';
                interline[4 * x + 3] = (cell->adjacent_cells[SOUTH] != NULL) ? ' ' : '-';
                interline[4 * x + 4] = '+';
            }
        }

        line[4 * width] = '\0';
        interline[4 * width] = '\0';
        printf("%s\n%s\n", line, interline);
    }
}



void free_grid(struct Grid *grid) 
{
    for (int i = 0; i < grid->height; i++) 
    {
        free(grid->cells[i]);
    }
    free(grid->cells);
    free(grid);
}

void fill_cells(struct Grid *grid) 
{
    for (int y = 0; y < grid->height; y++) 
    {
        for (int x = 0; x < grid->width; x++) 
        {
            struct Cell *cell = &grid->cells[y][x];
            cell->visited = false;
            cell->x = x;
            cell->y = y;
            // initialisations utiles pour le solveur :
            cell->visite = 0;
            cell->chemin = 0;
            cell->parent = NULL;
            // voisins initialement NULL (pas de passage tant que générateur ne relie pas)
            for (int d = 0; d < 4; d++)
                cell->adjacent_cells[d] = NULL;
        }
    }
}

// -------------------------------------
//             LOGIC
// -------------------------------------

struct Cell *select_case(struct Cell *actual_cell, struct Grid *grid, struct Stack *stack) 
{
    actual_cell->visited = true;
    int free_cells = 4;

    bool available_cells[4];
    for (int i = 0; i < 4; i++) {
        available_cells[i] = true;
    }

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
    printf("Coordonées : (%d, %d)\n", actual_cell->y, actual_cell->x);
    printf("Free_cells : %d\n", free_cells);
    // printf("CHECKPOINT DE TES MORTS n1\n\n");
    // pas de segmentation fault

    if (free_cells == 0)
    {
        stack_pop(stack);
        if (stack->size == 0) {
            printf("Stack vide, plus de cases à explorer.\n");
            print_grid_with_path(grid);
            exit(EXIT_SUCCESS);
        }
        return select_case(stack->array[stack->size-1], grid, stack);
    }
    else {
        // for (int i = 0; i < 4; i++)
        // {
        //     printf("%d : %d\n", i, available_cells[i]);
        // }

        int random_choice = rand() % free_cells;
        // printf("\nNombre aléatoire: %d\n", random_choice);
        for (int i = 0; i < 4; i++)
        {
            if (available_cells[i] == true)
            {
                if (random_choice == 0)
                {
                    if (i == NORTH)
                    {
                        actual_cell->adjacent_cells[NORTH] = &grid->cells[actual_cell->y - 1][actual_cell->x];
                        grid->cells[actual_cell->y - 1][actual_cell->x].adjacent_cells[SOUTH] = actual_cell;
                        return &grid->cells[actual_cell->y - 1][actual_cell->x];
                    }
                    if (i == SOUTH)
                    {
                        actual_cell->adjacent_cells[SOUTH] = &grid->cells[actual_cell->y + 1][actual_cell->x];
                        grid->cells[actual_cell->y + 1][actual_cell->x].adjacent_cells[NORTH] = actual_cell;
                        return &grid->cells[actual_cell->y + 1][actual_cell->x];
                    }
                    if (i == WEST)
                    {
                        actual_cell->adjacent_cells[WEST] = &grid->cells[actual_cell->y][actual_cell->x - 1];
                        grid->cells[actual_cell->y][actual_cell->x - 1].adjacent_cells[EAST] = actual_cell;
                        return &grid->cells[actual_cell->y][actual_cell->x - 1];
                    }
                    if (i == EAST)
                    {
                        actual_cell->adjacent_cells[EAST] = &grid->cells[actual_cell->y][actual_cell->x + 1];
                        grid->cells[actual_cell->y][actual_cell->x + 1].adjacent_cells[WEST] = actual_cell;
                        return &grid->cells[actual_cell->y][actual_cell->x + 1];
                    }
                }
                if (available_cells[i] == true)
                {
                    random_choice--;
                }
            }
        }
    }
    printf("ATTENTION: A atteint la fin de la fonction select_case\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <width> <height>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    struct Grid *grid = create_grid(width, height);
    fill_cells(grid);

    struct Stack stack = stack_init();
    struct Cell *actual_case = &grid->cells[0][0];

    // Génération du labyrinthe
    for (int i = 0; i < width * height; i++) {
        stack_push(&stack, actual_case);
        actual_case = select_case(actual_case, grid, &stack);
    }

    // Résolution BFS
    resolution(grid->cells, grid->width, grid->height);

    // Affichage avec le chemin
    print_grid_with_path(grid);

    free_grid(grid);
    return 0;
}
