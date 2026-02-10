#include <stdio.h>
#include <stdlib.h>
#include <string.h>


   /*Define constantes para:
   - tamanho máximo de cada linha do arquivo CSV
   - número máximo de atletas que podem ser armazenados*/

#define MAX_LINE 4096
#define MAX_ATHLETES 200000

/* Protótipos das funções auxiliares */
void trim(char *str);
int jaContado(int id, int *lista, int total);
void extrairCampos(char *linha, char *sex, char *noc, int *athlete_id, float *weight);


/*
   Função responsável por remover espaços em branco e
   quebras de linha do início e do fim de uma string.
   É utilizada para normalizar dados lidos do arquivo
   e entradas do usuário.
*/
void trim(char *str) {

    int inicio = 0;
    int fim = strlen(str) - 1;

    /* Localiza o início e o fim reais da string */
    while (str[inicio] == ' ' || str[inicio] == '\n' || str[inicio] == '\r')
        inicio++;

    while (fim >= inicio &&
          (str[fim] == ' ' || str[fim] == '\n' || str[fim] == '\r'))
        fim--;

    /* Reorganiza a string removendo os caracteres indesejados */
    int j = 0;
    for (int i = inicio; i <= fim; i++)
        str[j++] = str[i];

    str[j] = '\0';
}


/*
   Verifica se um atleta já foi contabilizado.
   Percorre a lista de IDs já armazenados e retorna:
   - 1 se o ID já existir
   - 0 caso contrário
*/
int jaContado(int id, int *lista, int total) {

    for (int i = 0; i < total; i++) {
        if (lista[i] == id)
            return 1;
    }

    return 0;
}


/*
   Extrai campos específicos de uma linha do arquivo CSV:
   - sexo
   - país (NOC)
   - athlete_id
   - peso

   O controle de aspas é necessário para tratar corretamente
   campos que podem conter vírgulas dentro de aspas.
*/
void extrairCampos(char *linha, char *sex, char *noc, int *athlete_id, float *weight) {

    int coluna = 1;
    int dentroAspas = 0;
    char campo[512];
    int j = 0;

    /* Inicializa os valores de saída */
    sex[0] = '\0';
    noc[0] = '\0';
    *athlete_id = -1;
    *weight = -1;

    /* Percorre toda a linha separando os campos */
    for (int i = 0; linha[i] != '\0'; i++) {

        if (linha[i] == '"') {
            dentroAspas = !dentroAspas;
            continue;
        }

        if ((linha[i] == ',' && !dentroAspas) || linha[i] == '\n') {

            campo[j] = '\0';

            /* Seleciona apenas as colunas relevantes */
            if (coluna == 2)
                strcpy(sex, campo);
            else if (coluna == 6)
                *weight = atof(campo);
            else if (coluna == 7)
                strcpy(noc, campo);
            else if (coluna == 8)
                *athlete_id = atoi(campo);

            coluna++;
            j = 0;
        } 
        else {
            if (j < 511)
                campo[j++] = linha[i];
        }
    }
}


int main() {

    /* Abertura do arquivo CSV */
    FILE *file = fopen("bios.csv", "r");
 

    /* Leitura do país informado pelo usuário */
    char pais[50];
    printf("Digite o nome do pais (em ingles, ex: Japan, Brazil, France): ");
    fflush(stdout);
    fgets(pais, 50, stdin);
    trim(pais);

    /* Estruturas de controle e contadores */
    char line[MAX_LINE];
    int atletas[MAX_ATHLETES];
    int totalAtletas = 0;
    int masculino = 0;
    int feminino = 0;

    /* Desconsidera o cabeçalho do arquivo */
    fgets(line, MAX_LINE, file);

    /* Processa cada linha do arquivo */
    while (fgets(line, MAX_LINE, file)) {

        char sex[20], noc[50];
        int athlete_id;
        float weight;

        extrairCampos(line, sex, noc, &athlete_id, &weight);
        trim(sex);
        trim(noc);

        /* Aplica os critérios de filtragem dos atletas */
        if (strcmp(noc, pais) == 0 &&
            athlete_id != -1 &&
            weight > 100.0 &&
            !jaContado(athlete_id, atletas, totalAtletas)) {

            atletas[totalAtletas++] = athlete_id;

            if (strcmp(sex, "Male") == 0)
                masculino++;
            else if (strcmp(sex, "Female") == 0)
                feminino++;
        }
    }

    fclose(file);

    /* Exibição dos resultados finais */
    printf("\n%s (peso > 100kg):\n", pais);
    printf("Total de atletas: %d\n", masculino + feminino);
    printf("Atletas masculinos: %d\n", masculino);
    printf("Atletas femininos: %d\n", feminino);

    return 0;
}