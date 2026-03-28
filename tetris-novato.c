#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TAM_FILA 5

typedef struct
{
    int id;
    char nome[2];
} Peca;

typedef struct
{
    Peca itens[TAM_FILA];
    int frente;
    int tras;
    int quantidade;
} FilaCircular;

int proximoId = 1;

Peca gerarPeca()
{
    Peca p;
    const char tipos[7] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    
    p.id = proximoId++;
    p.nome[0] = tipos[rand() % 7];
    p.nome[1] = '\0';

    return p;
}

void inicializarFila(FilaCircular *fila)
{
    fila->frente = 0;
    fila->tras = 0;
    fila->quantidade = 0;
}

int filaCheia(FilaCircular *fila)
{
    return fila->quantidade == TAM_FILA;
}

int filaVazia(FilaCircular *fila)
{
    return fila->quantidade == 0;
}

int enqueue(FilaCircular *fila, Peca peca)
{
    if (filaCheia(fila))
    {
        return 0;
    }

    fila->itens[fila->tras] = peca;
    fila->tras = (fila->tras + 1) % TAM_FILA;
    fila->quantidade++;

    return 1;
}

int dequeue(FilaCircular *fila, Peca *pecaRemovida)
{
    if (filaVazia(fila))
    {
        return 0;
    }

    *pecaRemovida = fila->itens[fila->frente];
    fila->frente = (fila->frente + 1) % TAM_FILA;
    fila->quantidade--;

    return 1;
}

void exibirFila(FilaCircular *fila)
{
    if (filaVazia(fila))
    {
        printf("\nFila vazia.\n");
        return;
    }

    printf("\n=== FILA ATUAL DE PECAS ===\n");

    for (int i = 0; i < fila->quantidade; i++)
    {
        int indice = (fila->frente + i) % TAM_FILA;
        printf("%d. Peca ID: %d | Tipo: %s\n", i + 1, fila->itens[indice].id, fila->itens[indice].nome);
    }

    printf("===========================\n");
}

void preencherFilaInicial(FilaCircular *fila)
{
    for (int i = 0; i < TAM_FILA; i++)
    {
        enqueue(fila, gerarPeca());
    }
}

void jogarPeca(FilaCircular *fila)
{
    Peca removida;

    if (dequeue(fila, &removida))
    {
        printf("\nPeca jogada com sucesso!\n");
        printf("Peca removida -> ID: %d | Tipo: %s\n", removida.id, removida.nome);

        Peca novaPeca = gerarPeca();
        enqueue(fila, novaPeca);

        printf("Nova peca inserida automaticamente -> ID: %d | Tipo: %s\n", novaPeca.id, novaPeca.nome);
    }
    else
    {
        printf("\nNao foi possivel jogar a peca. A fila esta vazia.\n");
    }
}

void inserirNovaPeca(FilaCircular *fila)
{
    if (filaCheia(fila))
    {
        printf("\nA fila ja esta cheia com 5 pecas.\n");
        printf("Para inserir manualmente, jogue uma peca antes.\n");
        return;
    }

    Peca novaPeca = gerarPeca();
    enqueue(fila, novaPeca);

    printf("\nNova peca inserida com sucesso!\n");
    printf("Peca inserida -> ID: %d | Tipo: %s\n", novaPeca.id, novaPeca.nome);
}

int main()
{
    FilaCircular fila;
    int opcao;

    srand(time(NULL));

    inicializarFila(&fila);
    preencherFilaInicial(&fila);

    do
    {
        printf("\n===== TETRIS STACK - NIVEL NOVATO =====\n");
        printf("1 - Visualizar fila\n");
        printf("2 - Jogar peca da frente\n");
        printf("3 - Inserir nova peca\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
            case 1:
                exibirFila(&fila);
                break;

            case 2:
                jogarPeca(&fila);
                exibirFila(&fila);
                break;

            case 3:
                inserirNovaPeca(&fila);
                exibirFila(&fila);
                break;

            case 0:
                printf("\nEncerrando o jogo...\n");
                break;

            default:
                printf("\nOpcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    return 0;
}