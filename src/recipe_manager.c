/*
Name - Aaditya Dua 
Batch - 62
SAP ID - 590024800
This program stores recipes and their ingredients.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Ingredient stores name, quantity, and its unit
typedef struct {
    char* name;
    double qty;
    char unit[20];
} Ingredient;

// Recipe stores name, servings, and a dynamic list of ingredients
typedef struct {
    int id;
    char* name;
    int servings;
    Ingredient* list;  
    int number;         
    int allocate;       
} Recipe;


Recipe* all_recipes = NULL; 
int recipe_num = 0;          
int recipe_lim = 0;          

const char* file_recipes = "recipes.txt";         // recipe info file
const char* file_ingredients = "ingredients.txt"; // ingredient info file


void get_str(char* prompt, char *buf) {
    printf("%s ", prompt);
    scanf(" %99[^\n]", buf);
}

int get_int(char* prompt) {
    int val;
    printf("%s ", prompt);
    scanf("%d", &val);
    return val;
}

double get_double(char* prompt) {
    double val;
    printf("%s ", prompt);
    scanf("%lf", &val);
    return val;
}


// Increase recipe array size when needed
void check_db_size() {
    if (recipe_num >= recipe_lim) {
        int new_lim = (recipe_lim == 0) ? 10 : recipe_lim * 2;

        Recipe* tmp = realloc(all_recipes, sizeof(Recipe) * new_lim);
        if (!tmp) {
            printf("Memory error\n");
            exit(1);
        }

        all_recipes = tmp;
        recipe_lim = new_lim;
    }
}

// Generate next recipe ID
int get_next_id() {
    int max = 0;
    for (int i = 0; i < recipe_num; i++) {
        if (all_recipes[i].id > max) 
            max = all_recipes[i].id;
    }
    return max + 1;
}

// Find recipe by ID
Recipe* find_recipe(int id) {
    for (int i = 0; i < recipe_num; i++) {
        if (all_recipes[i].id == id) 
            return &all_recipes[i];
    }
    return NULL;
}


// Add an ingredient to a recipe
void add_ingredient(Recipe* r, char* n, double q, char* u) {
    if (r->number >= r->allocate) {
        int nc = (r->allocate == 0) ? 4 : r->allocate * 2;

        Ingredient *tmp = realloc(r->list, sizeof(Ingredient) * nc);
        if (!tmp) return;

        r->list = tmp;
        r->allocate = nc;
    }

    Ingredient* ing = &r->list[r->number];
    ing->name = strdup(n);
    ing->qty = q;
    strncpy(ing->unit, u, 19);

    r->number++;
}

// Create a new recipe with user input
void create_recipe() {
    char name[100];
    get_str("Recipe Name:", name);
    if (strcmp(name, "0") == 0 || strlen(name) == 0) return;

    int serv = get_int("Servings:");

    check_db_size(); // ensure space for new recipe

    Recipe* r = &all_recipes[recipe_num];
    r->id = get_next_id();
    r->name = strdup(name);
    r->servings = serv;
    r->list = NULL;
    r->number = 0;
    r->allocate = 0;

    recipe_num++;
    printf("Recipe ID: %d\n", r->id);

    // Add ingredients
    while(1) {
        char iname[100];
        get_str("Ingredient (enter 'stop' to finish):", iname);
        if (strcmp(iname, "stop") == 0) break;

        double q = get_double("Qty:");
        char unit[20];
        get_str("Unit:", unit);

        add_ingredient(r, iname, q, unit);
    }
}

// Delete a recipe (free memory and shift array)
void delete_recipe() {
    int id = get_int("ID to delete:");
    int idx = -1;

    // Search recipe
    for (int i = 0; i < recipe_num; i++) {
        if (all_recipes[i].id == id) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Not found.\n");
        return;
    }

    Recipe* r = &all_recipes[idx];

    // Free recipe name and ingredient names
    free(r->name);
    for(int i = 0; i < r->number; i++)
        free(r->list[i].name);
    free(r->list);

    // Shift remaining recipes
    for (int i = idx; i < recipe_num - 1; i++) {
        all_recipes[i] = all_recipes[i+1];
    }

    recipe_num--;
    printf("Deleted.\n");
}

// List all recipes
void list_recipes() {
    printf("\n\t\t\tLIST OF RECIPES\t\t\t\n");
    for (int i = 0; i < recipe_num; i++) {
        printf("[%d] %s (serves %d)\n",
               all_recipes[i].id, all_recipes[i].name, all_recipes[i].servings);
    }
    printf("------------------\n");
}

// View single recipe
void view_recipe() {
    int id = get_int("Enter ID:");
    Recipe* r = find_recipe(id);

    if (!r) {
        printf("No recipes found.\n");
        return;
    }

    printf("\n%s (Servings: %d)\n", r->name, r->servings);
    for (int i = 0; i < r->number; i++) {
        printf(" - %.2f %s %s\n",
               r->list[i].qty, r->list[i].unit, r->list[i].name);
    }
}


// Save recipes and ingredients to files
void save_data() {
    FILE* fp1 = fopen(file_recipes, "w");
    FILE* fp2 = fopen(file_ingredients, "w");

    if (!fp1 || !fp2) {
        printf("Files could not be saved \n");
        return;
    }

    // Save recipes
    for (int i = 0; i < recipe_num; i++) {
        Recipe* r = &all_recipes[i];
        fprintf(fp1, "%d|%s|%d\n", r->id, r->name, r->servings);

        // Save ingredients belonging to this recipe
        for (int j = 0; j < r->number; j++) {
            fprintf(fp2, "%d|%s|%.2f|%s\n",
                    r->id, r->list[j].name, r->list[j].qty, r->list[j].unit);
        }
    }

    fclose(fp1);
    fclose(fp2);
    printf("Saved.\n");
}

// Load recipes and ingredients from files at startup
void load_data() {
    FILE* fp = fopen(file_recipes, "r");
    if (!fp) return;

    int id, serv;
    char name[100];

    // Load recipe info
    while (fscanf(fp, "%d|%99[^|]|%d\n", &id, name, &serv) == 3) {
        check_db_size();
        Recipe* r = &all_recipes[recipe_num];
        r->id = id;
        r->name = strdup(name);
        r->servings = serv;
        r->list = NULL;
        r->number = 0;
        r->allocate = 0;
        recipe_num++;
    }
    fclose(fp);

    // Load ingredients
    fp = fopen(file_ingredients, "r");
    if (!fp) return;

    int rid;
    char n[100], u[20];
    double q;

    while (fscanf(fp, "%d|%99[^|]|%lf|%19s\n", &rid, n, &q, u) == 4) {
        Recipe* r = find_recipe(rid);
        if (r) add_ingredient(r, n, q, u);
    }

    fclose(fp);
}


int main() {
    load_data();  // Load data at program start

    while (1) {
        printf("\n1. Add Recipe\n2. List\n3. View\n4. Save\n5. Delete\n0. Exit\n");
        int c = get_int("Choice:");

        if (c == 1) create_recipe();
        else if (c == 2) list_recipes();
        else if (c == 3) view_recipe();
        else if (c == 4) save_data();
        else if (c == 5) delete_recipe();
        else if (c == 0) {
            save_data(); // auto save on exit
            break;
        }
    }

    return 0;
}
