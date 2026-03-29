#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5
#define TAM_PILHA 3

typedef struct
{
    int id;
    char nome;
} Peca;

typedef struct
{
    Peca itens[TAM_FILA];
    int frente;
    int tras;
    int quantidade;
} FilaCircular;

typedef struct
{
    Peca itens[TAM_PILHA];
    int topo;
} Pilha;

int proximoId = 0;

Peca gerarPeca()
{
    Peca p;
    const char tipos[7] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};

    p.id = proximoId++;
    p.nome = tipos[rand() % 7];

    return p;
}

/* ===== FILA ===== */

void inicializarFila(FilaCircular *fila)
{
    fila->frente = 0;
    fila->tras = 0;
    fila->quantidade = 0;
}

int filaVazia(FilaCircular *fila)
{
    return fila->quantidade == 0;
}

int filaCheia(FilaCircular *fila)
{
    return fila->quantidade == TAM_FILA;
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

int dequeue(FilaCircular *fila, Peca *peca)
{
    if (filaVazia(fila))
    {
        return 0;
    }

    *peca = fila->itens[fila->frente];
    fila->frente = (fila->frente + 1) % TAM_FILA;
    fila->quantidade--;

    return 1;
}

void preencherFilaInicial(FilaCircular *fila)
{
    for (int i = 0; i < TAM_FILA; i++)
    {
        enqueue(fila, gerarPeca());
    }
}

/* ===== PILHA ===== */

void inicializarPilha(Pilha *pilha)
{
    pilha->topo = -1;
}

int pilhaVazia(Pilha *pilha)
{
    return pilha->topo == -1;
}

int pilhaCheia(Pilha *pilha)
{
    return pilha->topo == TAM_PILHA - 1;
}

int push(Pilha *pilha, Peca peca)
{
    if (pilhaCheia(pilha))
    {
        return 0;
    }

    pilha->topo++;
    pilha->itens[pilha->topo] = peca;
    return 1;
}

int pop(Pilha *pilha, Peca *peca)
{
    if (pilhaVazia(pilha))
    {
        return 0;
    }

    *peca = pilha->itens[pilha->topo];
    pilha->topo--;
    return 1;
}

/* ===== EXIBICAO ===== */

void exibirFila(FilaCircular *fila)
{
    printf("Fila de pecas: ");

    if (filaVazia(fila))
    {
        printf("(vazia)");
    }
    else
    {
        for (int i = 0; i < fila->quantidade; i++)
        {
            int indice = (fila->frente + i) % TAM_FILA;
            printf("[%c %d] ", fila->itens[indice].nome, fila->itens[indice].id);
        }
    }

    printf("\n");
}

void exibirPilha(Pilha *pilha)
{
    printf("Pilha de reserva (Topo -> Base): ");

    if (pilhaVazia(pilha))
    {
        printf("(vazia)");
    }
    else
    {
        for (int i = pilha->topo; i >= 0; i--)
        {
            printf("[%c %d] ", pilha->itens[i].nome, pilha->itens[i].id);
        }
    }

    printf("\n");
}

void exibirEstado(FilaCircular *fila, Pilha *pilha)
{
    printf("\nEstado atual:\n");
    exibirFila(fila);
    exibirPilha(pilha);
}

/* ===== ACOES ===== */

void jogarPeca(FilaCircular *fila)
{
    Peca removida;
    Peca nova;

    if (dequeue(fila, &removida))
    {
        printf("\nPeca jogada: [%c %d]\n", removida.nome, removida.id);

        nova = gerarPeca();
        enqueue(fila, nova);

        printf("Nova peca inserida na fila: [%c %d]\n", nova.nome, nova.id);
    }
    else
    {
        printf("\nFila vazia.\n");
    }
}

void reservarPeca(FilaCircular *fila, Pilha *pilha)
{
    Peca removida;
    Peca nova;

    if (pilhaCheia(pilha))
    {
        printf("\nPilha de reserva cheia.\n");
        return;
    }

    if (dequeue(fila, &removida))
    {
        push(pilha, removida);
        printf("\nPeca reservada: [%c %d]\n", removida.nome, removida.id);

        nova = gerarPeca();
        enqueue(fila, nova);

        printf("Nova peca inserida na fila: [%c %d]\n", nova.nome, nova.id);
    }
    else
    {
        printf("\nFila vazia.\n");
    }
}

void usarPecaReservada(Pilha *pilha)
{
    Peca usada;

    if (pop(pilha, &usada))
    {
        printf("\nPeca reservada usada: [%c %d]\n", usada.nome, usada.id);
    }
    else
    {
        printf("\nPilha de reserva vazia.\n");
    }
}

/* ===== MAIN ===== */

int main()
{
    FilaCircular fila;
    Pilha pilha;
    int opcao;

    srand(time(NULL));

    inicializarFila(&fila);
    inicializarPilha(&pilha);
    preencherFilaInicial(&fila);

    do
    {
        exibirEstado(&fila, &pilha);

        printf("\n1 - Jogar peca\n");
        printf("2 - Reservar peca\n");
        printf("3 - Usar peca reservada\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
            case 1:
                jogarPeca(&fila);
                break;
            case 2:
                reservarPeca(&fila, &pilha);
                break;
            case 3:
                usarPecaReservada(&pilha);
                break;
            case 0:
                printf("\nEncerrando...\n");
                break;
            default:
                printf("\nOpcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}