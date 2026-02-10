#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ATLETAS 150000

typedef struct
{
    int atleta_id;
    char nome[100];
    char genero;
    int data_nascimento;
    int ano_olimpiada;
    int maior_idade;
} Atletas;

// Função para comparação de dois atletas para identificar quem é mais velho, e assim continuando até o final da pilha
int comparar_idades(const void *a, const void *b) {
    Atletas *atletaA = (Atletas *)a;
    Atletas *atletaB = (Atletas *)b;
    return (atletaB->maior_idade - atletaA->maior_idade);
}

//Serve para separar em campos sem as vírgulas e aspas, separando em campos
int dividir_csv(char *linha, char campos[][512], int max_campos) {
    int indice_linha = 0; // posição na linha original
    int posicao_campo = 0; // posição dentro do campo atual
    int indice_campo = 0; // índice do campo (coluna)
    int booleano = 0; //Verdadeiro ou falso

    //Lógica para identificar aspas e vírgulas, assim separando os campos a cada virgula separadora e aspas separadora.
    while (linha[indice_linha] && indice_campo < max_campos) {
        if (linha[indice_linha] == '"') {
            booleano = !booleano; //Alternar entre verdadeiro e falso
        } else if (linha[indice_linha] == ',' && !booleano) {
            campos[indice_campo][posicao_campo] = '\0'; //Encerrar no último campo
            indice_campo++;
            posicao_campo = 0;
        } else {
            campos[indice_campo][posicao_campo++] = linha[indice_linha];
        }
        indice_linha++;
    }
    campos[indice_campo][posicao_campo] = '\0';
    return indice_campo + 1;
}

//Transformar o ano em 4 digitos numerais, sem as informações em String
int extrair_ano(char *texto) {
    int length = strlen(texto);

    for (int i = 0; i <= length - 4; i++) {
        if (isdigit(texto[i]) && isdigit(texto[i+1]) && isdigit(texto[i+2]) && isdigit(texto[i+3])) {
            return atoi(&texto[i]); //Converter um texto em um número inteiro
        }
    }
    return 0;
}

int main(){
    //Utilizando ponteiro para evitar buffers muito longos de memória e para gravar informaçôes em uma lista grande
    Atletas *atletas = (Atletas *)calloc(MAX_ATLETAS, sizeof(Atletas));
    if(atletas == NULL){
        printf("Erro: Memoria insuficiente!\n");
        return 1;
    }
    
    //Selecionar as planilhas que serão usadas no código
    FILE *fbios = fopen("bios.csv", "r");
    FILE *fresults = fopen("results.csv", "r");
    
    char linhas[2048]; //Linha da planilha
    char campos[30][512]; //Campos e colunas da planilha

    //Mostrar o erro caso não ache as planilhas
    if (!fbios || !fresults) {
        printf("Erro ao abrir arquivos.\n");
        return 1;
    }
    
    fgets(linhas, sizeof(linhas), fbios); //Pular a linha de informações da planilha
    //Carregar todas as informações da planilha bios
    while(fgets(linhas, sizeof(linhas), fbios)){
        dividir_csv(linhas, campos, 30); //Conta quantas colunas tem naquela linha
        
        //Endereçamento de dados da planilha para o ponteiro Atletas
        int id = atoi(campos[7]);
        if(id > 0 && id < MAX_ATLETAS){
            atletas[id].atleta_id = id;
            strcpy(atletas[id].nome, campos[2]);
            atletas[id].genero = campos[1][0];
            atletas[id].data_nascimento = extrair_ano(campos[4]);
            atletas[id].maior_idade = 0;
        }
    }

    fgets(linhas,sizeof(linhas), fresults); //Pular a linha de informações da planilha
    //Carregar todas as informações da planilha results e calcula as idades
    while(fgets(linhas,sizeof(linhas), fresults)){
        dividir_csv(linhas, campos, 30); //Conta quantas colunas tem naquela linha
        int id = atoi(campos[5]);

        //Endereçamento de dados da planilha para o ponteiro Atletas
        if(id > 0 && id < MAX_ATLETAS && atletas[id].data_nascimento > 0){
            atletas[id].atleta_id = id;
            atletas[id].ano_olimpiada = extrair_ano(campos[0]);

            int idade = atletas[id].ano_olimpiada - atletas[id].data_nascimento;
            //Armazenamento para quem quem tem a maior idade
            if(idade > atletas[id].maior_idade){
                atletas[id].maior_idade = idade;
            }
        }
    }

    //Separação do ponteir para ordenar os gêneros
    Atletas *homens = (Atletas *)malloc(MAX_ATLETAS * sizeof(Atletas));
    Atletas *mulheres = (Atletas *)malloc(MAX_ATLETAS * sizeof(Atletas));

    int h_count = 0, m_count = 0;

    //Função para identificar os mais velhos pelo genero e ordenar de forma decrescente por idade
    for(int i = 0; i < MAX_ATLETAS; i++){
        if(atletas[i].maior_idade > 0){
            if(atletas[i].genero == 'M'){
                homens[h_count++] = atletas[i];
            } 
            else if (atletas[i].genero == 'F') {
                mulheres[m_count++] = atletas[i];
            }
        }
    }

    //Ordenação com novos gêneros selecionados
    qsort(homens, h_count, sizeof(Atletas), comparar_idades);
    qsort(mulheres, m_count, sizeof(Atletas), comparar_idades);

    // Arquivos de saída para o gnuplot, criação de arquivos .dat
    FILE *f_homens = fopen("top10_homens.dat", "w");
    FILE *f_mulheres = fopen("top10_mulheres.dat", "w");
    FILE *grafico_atletas = fopen("grafico_atletas.gp", "w");

    if (!f_homens || !f_mulheres) {
        printf("Erro ao criar arquivos de saída para o gnuplot.\n");
        return 1;
    }

    if (!grafico_atletas) {
        printf("Erro ao criar script gnuplot.\n");
        return 1;
    }

    for (int i = 0; i < 10 && i < h_count; i++) {
        fprintf(f_homens, "%d %d\n", i+1, homens[i].maior_idade);
    }

    for (int i = 0; i < 10 && i < m_count; i++) {
        fprintf(f_mulheres, "%d %d\n", i+1, mulheres[i].maior_idade);
    }

    //Cria automaticamente os códigos em formato gnuplot para o arquivo "grafico_atletas.gp"
    fprintf(grafico_atletas,
        "set terminal pngcairo size 1200,600 enhanced font 'Arial,12'\n"
        "set output 'atletas_mais_velhos.png'\n"
        "\n"
        "set title 'Os 10 Atletas Mais Velhos da História Olímpica'\n"
        "set xlabel 'Ranking'\n"
        "set ylabel 'Idade (anos)'\n"
        "\n"
        "set grid\n"
        "set key outside\n"
        "set style data histograms\n"
        "set style fill solid 0.7 border -1\n"
        "set boxwidth 0.6\n"
        "\n"
        "plot "
        "'top10_homens.dat' using 2:xtic(1) title 'Homens', "
        "'top10_mulheres.dat' using 2:xtic(1) title 'Mulheres'\n");

    //Limpeza e fecha os arquivos do .dat, informações e do gráfico
    fclose(f_homens);
    fclose(f_mulheres);
    fclose(grafico_atletas);

    FILE *grafico_atleta_run = popen("gnuplot", "w"); //Criação do arquivo gnuplot
    fprintf(grafico_atleta_run, "load 'grafico_atletas.gp'\n"); //Ler o arquivo gnuplot
    fflush(grafico_atleta_run); //Forçar o envio imediato e evitar ficar preso no buffer antes do programa fechar
    pclose(grafico_atleta_run); //Fecha o arquivo gnuplot

    //Exibição dos atletas mais velhos separados por gêneros
    printf("\n--- 10 atletas masculinos mais velhos de todos os tempos ---\n");
    for(int i = 0; i < 10 && i < h_count; i++)
        printf("%d. %-30s | %d anos\n", i+1, homens[i].nome, homens[i].maior_idade);

    printf("\n--- 10 atletas femininas mais velhas de todos os tempos ---\n");
    for(int i = 0; i < 10 && i < m_count; i++)
        printf("%d. %-30s | %d anos\n", i+1, mulheres[i].nome, mulheres[i].maior_idade);

    //Limpeza e fecha os arquivos
    fclose(fbios);
    fclose(fresults);
    free(atletas); 
    free(homens); 
    free(mulheres);

    return 0;
}