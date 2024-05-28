/** Arda Onur, S018752
 * This is the kitchen simulation code for OzuRest.
 * French chef and 3 students from gastronomy department are preparing delicious meals in here
 * You need to solve their problems.
**/
#include "meal.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define GLOVE_COUNT 3
#define POT_SIZE 3
#define APPRENTICE_COUNT 3
#define MEALS_TO_PREPARE 4
#define REQUIRED_INGREDIENTS 3

struct meal Menu[4] = {
    {"Menemen", {{"Tomato", 3}, {"Onion", 4}, {"Egg", 1}}, 10},
    {"Chicken Pasta", {{"Pasta", 2}, {"Chicken", 5}, {"Curry", 2}}, 8}, 
    {"Beef Steak", {{"Beef", 7}, {"Pepper", 3}, {"Garlic", 2}}, 13}, 
    {"Ali Nazik", {{"Eggplant", 4}, {"Lamb Meat", 4}, {"Yoghurt", 1}}, 10}
}; 

int meal_counter = 0;
int pot_count = 0;
int meal_ing_counter = 0;

//Creating 3 mutex for Glove pot and İngridient because these three things must be lock and other apprentice must not take these while other one while using it
pthread_mutex_t glove_mutex_Pthread[GLOVE_COUNT];
pthread_mutex_t pot_mutex_Pthread;
pthread_mutex_t ingridient_mutex_Pthread;
//Locking mechanism for gloves
void put_gloves(int apprentice_id) {
    printf("Apprentice %d is picking gloves \n", apprentice_id);
    pthread_mutex_lock(&glove_mutex_Pthread[apprentice_id]);
    pthread_mutex_lock(&glove_mutex_Pthread[(apprentice_id + 1) % GLOVE_COUNT]);
    printf("Apprentice %d has picked gloves\n", apprentice_id);
}
//Unlocking mechanism for gloves
void remove_gloves(int apprentice_id) {
    pthread_mutex_unlock(&glove_mutex_Pthread[apprentice_id]);
    pthread_mutex_unlock(&glove_mutex_Pthread[(apprentice_id + 1) % GLOVE_COUNT]);
    printf("Apprentice %d has removed gloves\n", apprentice_id);
}

void pick_ingredient(int apprentice_id, int* meal_index, int* ing_index) {
    put_gloves(apprentice_id);
    pthread_mutex_lock(&ingridient_mutex_Pthread);
    *meal_index = meal_counter;
    *ing_index = meal_ing_counter;
    printf("Apprentice %d has taken ingredient %s\n", apprentice_id, Menu[*meal_index].ingredients[*ing_index].name);
    meal_ing_counter = meal_ing_counter + 1;
    pthread_mutex_unlock(&ingridient_mutex_Pthread);
    remove_gloves(apprentice_id);
} 

void prepare_ingredient(int apprentice_id, int meal_index, int ing_index) {
    printf("Apprentice %d is preparing: %s \n", apprentice_id, Menu[meal_index].ingredients[ing_index].name);
    sleep(Menu[meal_index].ingredients[ing_index].time_to_process);
    printf("Apprentice %d is done preparing %s \n", apprentice_id, Menu[meal_index].ingredients[ing_index].name);
}

void put_ingredient(int id, int meal_index, int ing_index) {
    while(1) {
        if(pot_count < POT_SIZE) {
            printf("Apprentince %d is trying to put %s into pot\n", id, Menu[meal_index].ingredients[ing_index].name);
            pthread_mutex_lock(&pot_mutex_Pthread);
            printf("Apprentince %d has put %s into pot\n", id, Menu[meal_index].ingredients[ing_index].name);
            pot_count = pot_count + 1;
            pthread_mutex_unlock(&pot_mutex_Pthread);
            break;
        } else {
            break;
        }
    }
}

void help_chef(int apprentice_id) {
    int meal_index;
    int meal_ingredient_index;
    int i = 1;
    while(true) {
       // İf meal_ing_counter is not equal REQUIRED_INGREDIENTS and  meal_counter is not MEALS_TO_PREPARE apprentices must help chef 
      if(meal_ing_counter!= REQUIRED_INGREDIENTS && meal_counter != MEALS_TO_PREPARE) {
            pick_ingredient(apprentice_id, &meal_index, &meal_ingredient_index);
            prepare_ingredient(apprentice_id, meal_index, meal_ingredient_index);
            put_ingredient(apprentice_id, meal_index, meal_ingredient_index);
        } else {
            break;
        }
    }
}

void *apprentice(int *apprentice_id) {
    printf("Im apprentice %d\n", *apprentice_id);
    int i = 1;
    while(true) {
        if(meal_counter != MEALS_TO_PREPARE) {
            help_chef(*apprentice_id);
        }else{
          break;
        }
    }
    pthread_exit(NULL);
}

void *chef() {
  while(true) {  
        if(pot_count == POT_SIZE) {
            printf("Chef is preparing meal %s\n", Menu[meal_counter].name);
            sleep(Menu[meal_counter].time_to_prepare);
            printf("Chef prepared the meal %s\n", Menu[meal_counter].name);
            meal_ing_counter = 0;
            meal_counter = meal_counter + 1;
            pot_count = 0;
            sleep(3); 
            if(meal_counter == MEALS_TO_PREPARE){
                printf("ALL FOOD İS DONE, CHEF CAN GO HOME");
              break;
            }  
        }
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t apprentice_threads[APPRENTICE_COUNT];
    pthread_t chef_thread;
    int apprentice_ids[APPRENTICE_COUNT] = {0 ,1 ,2};
for (size_t i = 0; i < GLOVE_COUNT; i++) {
    pthread_mutex_init(&glove_mutex_Pthread[i], NULL);
}
for (size_t i = 0; i < APPRENTICE_COUNT; i++) {
    pthread_create(&apprentice_threads[i], NULL, (void *(*)(void *))apprentice, &apprentice_ids[i]);
}
pthread_create(&chef_thread, NULL, chef, NULL);
for (size_t i = 0; i < APPRENTICE_COUNT; i++) {
    pthread_join(apprentice_threads[i], NULL);
}
    pthread_join(chef_thread, NULL);
    return 0;
}
