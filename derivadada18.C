#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ATLETAS 350000 // Aumentado para suportar os 300k do bios.csv

typedef struct
{
    int atleta_id;
    char nome[100];
    char genero;
    int data_nascimento;
    int ano_olimpiada;
    int menor_idade;
} Atletas;

// ordenação crescente (menor pro maior)
int comparar_idades(const void *a, const void *b)
{
    Atletas *atletaA = (Atletas *)a;
    Atletas *atletaB = (Atletas *)b;
    return (atletaA->menor_idade - atletaB->menor_idade);
}

int dividir_csv(char *linha, char campos[][512], int max_campos) //divide o arquivo para em subcampos para 
//facilitar a pesquisa e analise de dados
{
    int indice_linha = 0;
    int posicao_campo = 0;
    int indice_campo = 0;
    int dentro_aspas = 0;

    while (linha[indice_linha] && indice_campo < max_campos)
    {
        if (linha[indice_linha] == '"')
        {
            dentro_aspas = !dentro_aspas;
        }
        else if (linha[indice_linha] == ',' && !dentro_aspas)
        {
            campos[indice_campo][posicao_campo] = '\0';
            indice_campo++;
            posicao_campo = 0;
        }
        else if (linha[indice_linha] != '\n' && linha[indice_linha] != '\r')
        {
            campos[indice_campo][posicao_campo++] = linha[indice_linha];
        }
        indice_linha++;
    }
    campos[indice_campo][posicao_campo] = '\0';
    return indice_campo + 1;
}//Mostra a localização dentro da pesquisa do arquivo, ou seja, o número do campo dentro da linha do arquivo csv

int extrair_ano(char *texto)
{
    int length = strlen(texto);
    for (int i = 0; i <= length - 4; i++)
    {
        if (isdigit(texto[i]) && isdigit(texto[i + 1]) && isdigit(texto[i + 2]) && isdigit(texto[i + 3]))
        {
            return atoi(&texto[i]);
        }
    }
    return 0;
}//extrai o ano de strings para numeros inteiros

int main()
{
    Atletas *atletas = (Atletas *)calloc(MAX_ATLETAS, sizeof(Atletas));
    if (atletas == NULL)
    {
        printf("Erro: Memoria insuficiente!\n");
        return 1; //Verifica se tem memoria suficiente
    }

    // Começa o  menor_idade com um valor alto para a comparação
    for (int i = 0; i < MAX_ATLETAS; i++)
    {
        atletas[i].menor_idade = 999;
    }

    FILE *fbios = fopen("bios.csv", "r");
    FILE *fresults = fopen("results.csv", "r");// abre os arquivos

    char linhas[2048];
    char campos[30][512];

    if (!fbios || !fresults)
    {
        printf("Erro ao abrir arquivos. Verifique se bios.csv e results.csv estao na pasta.\n");
        return 1;
    }//verifica se abriu os arquivosde forma correta

    printf("Lendo base de atletas...\n");
    fgets(linhas, sizeof(linhas), fbios);
    while (fgets(linhas, sizeof(linhas), fbios))
    {
        dividir_csv(linhas, campos, 30);
        int id = atoi(campos[7]);
        if (id > 0 && id < MAX_ATLETAS)
        {
            atletas[id].atleta_id = id;
            strncpy(atletas[id].nome, campos[3], 99); // Usando 'Used name' que é mais limpo
            atletas[id].genero = campos[1][0];        // 'M' ou 'F' (Masculino/Feminino)
            atletas[id].data_nascimento = extrair_ano(campos[4]);
        }
    }

    printf("Lendo resultados e calculando idades...\n");//calcula a idade
    fgets(linhas, sizeof(linhas), fresults);
    while (fgets(linhas, sizeof(linhas), fresults))
    {
        dividir_csv(linhas, campos, 30);
        int id = atoi(campos[5]);

        if (id > 0 && id < MAX_ATLETAS && atletas[id].data_nascimento > 0)
        {
            int ano_olimp = extrair_ano(campos[0]);
            int idade = ano_olimp - atletas[id].data_nascimento;

            // Filtro  para idades impossíveis (ex: dados errados)
            if (idade > 5 && idade < atletas[id].menor_idade)
            {
                atletas[id].menor_idade = idade;
                atletas[id].ano_olimpiada = ano_olimp;
            }
        }
    }

    Atletas *homens = (Atletas *)malloc(MAX_ATLETAS * sizeof(Atletas));
    Atletas *mulheres = (Atletas *)malloc(MAX_ATLETAS * sizeof(Atletas)); 
    int h_count = 0, m_count = 0;

    for (int i = 0; i < MAX_ATLETAS; i++)
    {
        if (atletas[i].menor_idade > 0 && atletas[i].menor_idade != 999)
        {
            if (atletas[i].genero == 'M')
            {
                homens[h_count++] = atletas[i];
            }
            else if (atletas[i].genero == 'F')
            {
                mulheres[m_count++] = atletas[i];
            }//separa os atletas por genero
        }
    }

    qsort(homens, h_count, sizeof(Atletas), comparar_idades);
    qsort(mulheres, m_count, sizeof(Atletas), comparar_idades); //compara e ordena por idade

    printf("\n TOP 10 ATLETAS MASCULINOS MAIS JOVENS \n");
    for (int i = 0; i < 10 && i < h_count; i++)
        printf("%d. %-30s | %d anos (Olimpiada de %d)\n", i + 1, homens[i].nome, homens[i].menor_idade, homens[i].ano_olimpiada);

    printf("\n TOP 10 ATLETAS FEMININAS MAIS JOVENS \n");
    for (int i = 0; i < 10 && i < m_count; i++)
        printf("%d. %-30s | %d anos (Olimpiada de %d)\n", i + 1, mulheres[i].nome, mulheres[i].menor_idade, mulheres[i].ano_olimpiada);

    fclose(fbios);
    fclose(fresults);
    free(atletas);
    free(homens);
    free(mulheres);

    return 0;
}