#include <stdio.h> // Biblioteca para entrada e saída de dados
#include <stdlib.h>
#include <string.h>

#define MAX 200000
#define OLYMPIC_YEARS 1896 //definições de constantes

typedef struct { //Definição de como definir a estrutura Atleta
    int id;
    char name[100];
    char sex;
    int birth_year;
    int age;
} Athlete; 

