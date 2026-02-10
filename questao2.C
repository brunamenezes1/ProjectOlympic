#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define tamanho maximo de uma linha do csv e numero maximo de atletas unicos para armazenar os ids
#define MAX_LINE 4096
#define MAX_ATHLETES 200000

//remove espaços em branco do inicio e do fim de uma string, verifica se um id ja foi contado e extrai os campos necessarios de uma linha do csv
void trim(char *str);
int jaContado(int id, int *lista, int total);
void extrairCampos(char *linha, char *sex, char *noc, int *athlete_id);

//indice do começo e indice do fim de cada campo na linha do csv, pula espaços em branco e caracteres de nova linha, copia os campos para as variaveis correspondentes e finaliza as strings com o caractere nulo
void trim(char *str) {
    int inicio = 0;
    int fim = strlen(str) - 1;

    while (str[inicio] == ' ' || str[inicio] == '\n' || str[inicio] == '\r')
        inicio++;

    while (fim >= inicio &&
          (str[fim] == ' ' || str[fim] == '\n' || str[fim] == '\r'))
        fim--;

    int j = 0;
    for (int i = inicio; i <= fim; i++)
        str[j++] = str[i];

    str[j] = '\0';
}

//percorre ids ja guardados para verifica se o id do atleta atual ja foi contado, se sim retorna 1, se nao retorna 0
int jaContado(int id, int *lista, int total) {
    for (int i = 0; i < total; i++) {
        if (lista[i] == id)
            return 1;
    }
    return 0;
}

void extrairCampos(char *linha, char *sex, char *noc, int *athlete_id) {
    /*
        Esta função faz a leitura de uma linha de um arquivo CSV e extrai
        apenas alguns campos específicos. Ela percorre a linha caractere
        por caractere, separando os campos pelas vírgulas, mas respeitando
        os casos em que o conteúdo está entre aspas, para não quebrar campos
        que possuem vírgulas internas.
    */

    int coluna = 1;
    int dentroAspas = 0;
    char campo[512];
    int j = 0;

    /*
        As variáveis de saída são inicializadas para evitar lixo de memória.
        Durante a leitura, quando o fim de um campo é detectado, a função
        verifica a coluna atual e, se for uma das colunas desejadas, copia
        o valor correspondente: sexo (coluna 2), NOC do país (coluna 7)
        e ID do atleta (coluna 8).
    */

    sex[0] = '\0';
    noc[0] = '\0';
    *athlete_id = -1;

    for (int i = 0; linha[i] != '\0'; i++) {

        if (linha[i] == '"') {
            dentroAspas = !dentroAspas;
            continue;
        }

        if ((linha[i] == ',' && !dentroAspas) || linha[i] == '\n') {

            campo[j] = '\0';

            if (coluna == 2)
                strcpy(sex, campo);
            else if (coluna == 7)
                strcpy(noc, campo);
            else if (coluna == 8)
                *athlete_id = atoi(campo);

            coluna++;
            j = 0;
        } else {
            if (j < 511)
                campo[j++] = linha[i];
        }
    }

    if (j > 0 && coluna == 8) {
        campo[j] = '\0';
        *athlete_id = atoi(campo);
    }
}

void gerarGraficoGenero(char *pais, int masculino, int feminino) {
    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot) {
        printf("Erro ao abrir o gnuplot.\n");
        return;
    }

    fprintf(gnuplot, "set terminal pngcairo size 800,600 enhanced font 'Arial,12'\n");
    fprintf(gnuplot, "set output 'grafico_genero_%s.png'\n", pais);
    fprintf(gnuplot, "set title 'Distribuição de Gênero - %s'\n", pais);

    fprintf(gnuplot, "set style data boxes\n");
    fprintf(gnuplot, "set style fill solid 0.85 border rgb 'black'\n");
    fprintf(gnuplot, "set boxwidth 0.6\n");
    fprintf(gnuplot, "set ylabel 'Número de Atletas'\n");
    fprintf(gnuplot, "set grid ytics\n");
    fprintf(gnuplot, "set xtics ('Masculino' 1, 'Feminino' 2)\n");

    fprintf(gnuplot,
            "plot '-' using 1:2 title 'Masculino' lc rgb '#4A90E2', "
            "'-' using 1:2 title 'Feminino' lc rgb '#E24A90'\n");

    fprintf(gnuplot, "1 %d\n", masculino);
    fprintf(gnuplot, "e\n");
    fprintf(gnuplot, "2 %d\n", feminino);
    fprintf(gnuplot, "e\n");

    pclose(gnuplot);
}

int main() {
     //abre o arquivo bios.csv para leitura, se nao conseguir abrir exibe uma mensagem de erro e encerra o programa
    FILE *file = fopen("bios.csv", "r");
    if (!file) {
        printf("Erro ao abrir bios.csv\n");
        return 1;
    }
    //solicita ao usuario o nome do pais para o qual deseja contar os atletas, armazena a resposta em uma string e remove espaços em branco do inicio e do fim da string
    char pais[50];
    printf("Digite o nome do pais (em ingles, ex: Japan, Brazil, France): ");
    fflush(stdout);
    fgets(pais, 50, stdin);
    trim(pais);
    //variaveis para armazenar a linha lida do arquivo, os ids dos atletas ja contados, o total de atletas unicos e as contagens de atletas masculinos e femininos
    char line[MAX_LINE];
    int atletas[MAX_ATHLETES];
    int totalAtletas = 0;

    int masculino = 0;
    int feminino = 0;

    
    fgets(line, MAX_LINE, file);

        /*
        Este laço lê o arquivo CSV linha por linha e extrai, para cada registro,
        os campos de sexo, país (NOC) e ID do atleta, preparando os dados para
        a verificação e contagem.
    */
    while (fgets(line, MAX_LINE, file)) {

        char sex[20], noc[50];
        int athlete_id;

        extrairCampos(line, sex, noc, &athlete_id);

        trim(sex);
        trim(noc);

        /*
            O código verifica se o atleta pertence ao país informado e se
            ainda não foi contabilizado. Caso seja um atleta novo, ele é
            registrado e contado de acordo com o sexo.
        */
        if (strcmp(noc, pais) == 0 && athlete_id != -1) {

            if (!jaContado(athlete_id, atletas, totalAtletas)) {
                atletas[totalAtletas++] = athlete_id;

                if (strcmp(sex, "Male") == 0)
                    masculino++;
                else if (strcmp(sex, "Female") == 0)
                    feminino++;
            }
        }
    }

    fclose(file);

    printf("\n%s:\n", pais);
    printf("Total de atletas: %d\n", masculino + feminino);
    printf("Atletas masculinos: %d\n", masculino);
    printf("Atletas femininos: %d\n", feminino);

    gerarGraficoGenero(pais, masculino, feminino);


    return 0;
}