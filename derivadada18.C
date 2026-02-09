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

int participated[MAX] = {0}; // Considera apenas quem participou dos jogos olímpicos

int compare(const void *a, const void *b) {
    return ((Athlete*)a)->age - ((Athlete*)b)->age;
} //Ordena a idade dos atletas de forma crescente

int main() {
    FILE *results = fopen("./results/results.csv", "r");
    FILE *bios = fopen("./athletes/bios.csv", "r"); //Abre os arquivos results.csv e bios.csv para leitura

    if (results == NULL) {
        printf("Erro ao abrir results.csv\n");
        return 1;
    }

    if (bios == NULL) {
        printf("Erro ao abrir bios.csv\n");
        fclose(results);
        return 1;
    } // Verifica se os arquivos foram abertos corretamente

    char line[1024];//armazena cada linha dos arquivos

    fgets(line, sizeof(line), results); //ignora a primeira linha do results.csv (cabeçalho)
    while (fgets(line, sizeof(line), results)) {
        int id;
        sscanf(line, "%d", &id);

        if (id >= 0 && id < MAX)
            participated[id] = 1;
    } //Aqui saberemos quem participou dos jogos

    Athlete male[5000], female[5000];
    int m = 0, f = 0; //Classificação por genero

    fgets(line, sizeof(line), bios); //ignora a primeira linha do bios.csv (cabeçalho)

    while (fgets(line, sizeof(line), bios)) {
        Athlete a;
        char birth[20];

        sscanf(line, "%d,%99[^,],%c,%19[^,]",
               &a.id, a.name, &a.sex, birth); //Extrai os dados do bios e aramazena pra necessario

        if (a.id < 0 || a.id >= MAX) continue;

        if (!participated[a.id]) continue; //ignora quem nao participou dos jogos olímpicos
        if (strlen(birth) < 4) continue; //ignora quem nao tem o ano de nascimento completo

        sscanf(birth, "%d", &a.birth_year); //ano do nascimento do atleta

        a.age = OLYMPIC_YEARS - a.birth_year;//calcula a idade com base no ano que nasceu e nos jogos que participou 

        if (a.sex == 'M' && m < 5000) male[m++] = a; //separa em masculino
        else if (a.sex == 'F' && f < 5000) female[f++] = a;//separa em feminino
    }

    qsort(male, m, sizeof(Athlete), compare);
    qsort(female, f, sizeof(Athlete), compare); //ordena por idade

    printf("\nTop 10 atletas mais jovens (Masculino)\n");
    for (int i = 0; i < 10 && i < m; i++)
        printf("%d. %s - %d anos\n", i + 1, male[i].name, male[i].age); //mostra os 10 primeiros masculinos e protege se tiver menos de 10

    printf("\nTop 10 atletas mais jovens (Feminino)\n");
    for (int i = 0; i < 10 && i < f; i++)
        printf("%d. %s - %d anos\n", i + 1, female[i].name, female[i].age); //mostra os 10 primeiros femininose protege se tiver menos de 10

    fclose(results); //Fecha os arquivos
    fclose(bios);
    return 0;
}


