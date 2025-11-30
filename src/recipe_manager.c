/*
Name - Aaditya Dua 
Batch - 62
SAP ID - 590024800
This program is made to store and show recipes 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char* name;
    double qty;
    char unit[20];
} Ingredient;

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

const char* file_recipes = "recipes.txt";
const char* file_ingredients = "ingredients.txt";


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

int get_next_id() {
    int max = 0;
    for (int i = 0; i < recipe_num; i++) {
        if (all_recipes[i].id > max) max = all_recipes[i].id;
    }
    return max + 1;
}

Recipe* find_recipe(int id) {
    for (int i = 0; i < recipe_num; i++) {
        if (all_recipes[i].id == id) return &all_recipes[i];
    }
    return NULL;
}


void add_ingredient(Recipe* r, char* n, double q, char* u) {
    if ((*r).number >= (*r).allocate) {
        int nc = ((*r).allocate == 0) ? 4 : (*r).allocate * 2;
        Ingredient *tmp = realloc((*r).list, sizeof(Ingredient) * nc);
        if (!tmp) return;
        (*r).list = tmp;
        (*r).allocate = nc;
    }
    
    Ingredient* ing = &(*r).list[(*r).number];
    
    (*ing).name = strdup(n);
    (*ing).qty = q;
    strncpy((*ing).unit, u, 19);
    
    (*r).number++;
}


void create_recipe() {
    char name[100];
    get_str("Recipe Name:", name);
    if (strcmp(name, "0") == 0 || strlen(name) == 0) return;
    
    int serv = get_int("Servings:");
    
    check_db_size();
    
    Recipe* r = &all_recipes[recipe_num];
    
    (*r).id = get_next_id();
    (*r).name = strdup(name);
    (*r).servings = serv;
    (*r).list = NULL;
    (*r).number = 0;
    (*r).allocate = 0;
    
    recipe_num++;
    printf("Recipe ID: %d\n", (*r).id);
    
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


void delete_recipe() {
    int id = get_int("ID to delete:");
    int idx = -1;
    
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
    
    free((*r).name);
    
    for(int i=0; i < (*r).number; i++) {
        free((*r).list[i].name);
    }
    free((*r).list);
    
    for (int i = idx; i < recipe_num - 1; i++) {
        all_recipes[i] = all_recipes[i+1];
    }
    
    recipe_num--;
    printf("Deleted.\n");
}


void list_recipes() {
    printf("\n\t\t\tLIST\t\t\t\n");
    for (int i = 0; i < recipe_num; i++) {
        printf("[%d] %s (serves %d)\n", all_recipes[i].id, all_recipes[i].name, all_recipes[i].servings);
    }
    printf("------------------\n");
}


void view_recipe() {
    int id = get_int("Enter ID:");
    Recipe* r = find_recipe(id);
    if (!r) {
        printf("No recipes found.\n");
        return;
    }
    
    printf("\n%s (No. Of Servings %d)\n", (*r).name, (*r).servings);
    for (int i = 0; i < (*r).number; i++) {
        printf(" - %.2f %s %s\n", (*r).list[i].qty, (*r).list[i].unit, (*r).list[i].name);
    }
}


void save_data() {
    FILE* fp1 = fopen(file_recipes, "w");
    FILE* fp2 = fopen(file_ingredients, "w");
    
    if (!fp1 || !fp2) {
        printf("Files could not be saved \n");
        return;
    }
    
    for (int i = 0; i < recipe_num; i++) {
        Recipe* r = &all_recipes[i];
        fprintf(fp1, "%d|%s|%d\n", (*r).id, (*r).name, (*r).servings);
        
        for (int j = 0; j < (*r).number; j++) {
            fprintf(fp2, "%d|%s|%.2f|%s\n", 
                    (*r).id, 
                    (*r).list[j].name, 
                    (*r).list[j].qty, 
                    (*r).list[j].unit);
        }
    }
    
    fclose(fp1);
    fclose(fp2);
    printf("Saved.\n");
}


void load_data() {
    FILE* fp = fopen(file_recipes, "r");
    if (!fp) return;
    
    int id, serv;
    char name[100];
    
    while(fscanf(fp, "%d|%99[^|]|%d\n", &id, name, &serv) == 3) {
        check_db_size();
        Recipe* r = &all_recipes[recipe_num];
        (*r).id = id;
        (*r).name = strdup(name);
        (*r).servings = serv;
        (*r).list = NULL;
        (*r).number = 0;
        (*r).allocate = 0;
        recipe_num++;
    }
    fclose(fp);
    
    fp = fopen(file_ingredients, "r");
    if (!fp) return;
    
    int rid;
    char n[100], u[20];
    double q;
    while(fscanf(fp, "%d|%99[^|]|%lf|%19s\n", &rid, n, &q, u) == 4) {
        Recipe* r = find_recipe(rid);
        if (r) add_ingredient(r, n, q, u);
    }
    fclose(fp);
}


int main() {
    load_data();
    
    while(1) {
        printf("\n1. Add Recipe\n2. List\n3. View\n4. Save\n5. Delete\n0. Exit\n");
        int c = get_int("Choice:");
        
        if (c == 1) create_recipe();
        else if (c == 2) list_recipes();
        else if (c == 3) view_recipe();
        else if (c == 4) save_data();
        else if (c == 5) delete_recipe();
        else if (c == 0) {
            save_data();
            break;
        }
    }
    
    return 0;
}
