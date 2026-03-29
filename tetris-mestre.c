#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_FILA 5
#define TAM_PILHA 3
#define TAM_HISTORICO 50

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
} PilhaReserva;

typedef struct
{
    FilaCircular fila;
    PilhaReserva pilha;
} EstadoJogo;

typedef struct
{
    EstadoJogo itens[TAM_HISTORICO];
    int topo;
} PilhaHistorico;

int proximoId = 0;

/* =========================================================
   GERACAO DE PECAS
   ========================================================= */

Peca gerarPeca()
{
    Peca p;
    const char tipos[7] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};

    p.id = proximoId++;
    p.nome = tipos[rand() % 7];

    return p;
}

/* =========================================================
   FILA CIRCULAR
   ========================================================= */

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

Peca frenteFila(FilaCircular *fila)
{
    return fila->itens[fila->frente];
}

void substituirFrenteFila(FilaCircular *fila, Peca peca)
{
    if (!filaVazia(fila))
    {
        fila->itens[fila->frente] = peca;
    }
}

void preencherFilaInicial(FilaCircular *fila)
{
    for (int i = 0; i < TAM_FILA; i++)
    {
        enqueue(fila, gerarPeca());
    }
}

/* =========================================================
   PILHA DE RESERVA
   ========================================================= */

void inicializarPilha(PilhaReserva *pilha)
{
    pilha->topo = -1;
}

int pilhaVazia(PilhaReserva *pilha)
{
    return pilha->topo == -1;
}

int pilhaCheia(PilhaReserva *pilha)
{
    return pilha->topo == TAM_PILHA - 1;
}

int push(PilhaReserva *pilha, Peca peca)
{
    if (pilhaCheia(pilha))
    {
        return 0;
    }

    pilha->topo++;
    pilha->itens[pilha->topo] = peca;
    return 1;
}

int pop(PilhaReserva *pilha, Peca *peca)
{
    if (pilhaVazia(pilha))
    {
        return 0;
    }

    *peca = pilha->itens[pilha->topo];
    pilha->topo--;
    return 1;
}

Peca topoPilha(PilhaReserva *pilha)
{
    return pilha->itens[pilha->topo];
}

void substituirTopoPilha(PilhaReserva *pilha, Peca peca)
{
    if (!pilhaVazia(pilha))
    {
        pilha->itens[pilha->topo] = peca;
    }
}

/* =========================================================
   HISTORICO PARA DESFAZER
   ========================================================= */

void inicializarHistorico(PilhaHistorico *historico)
{
    historico->topo = -1;
}

int historicoVazio(PilhaHistorico *historico)
{
    return historico->topo == -1;
}

int pushHistorico(PilhaHistorico *historico, EstadoJogo estado)
{
    if (historico->topo == TAM_HISTORICO - 1)
    {
        return 0;
    }

    historico->topo++;
    historico->itens[historico->topo] = estado;
    return 1;
}

int popHistorico(PilhaHistorico *historico, EstadoJogo *estado)
{
    if (historicoVazio(historico))
    {
        return 0;
    }

    *estado = historico->itens[historico->topo];
    historico->topo--;
    return 1;
}

EstadoJogo capturarEstado(FilaCircular *fila, PilhaReserva *pilha)
{
    EstadoJogo estado;
    estado.fila = *fila;
    estado.pilha = *pilha;
    return estado;
}

void restaurarEstado(EstadoJogo *estado, FilaCircular *fila, PilhaReserva *pilha)
{
    *fila = estado->fila;
    *pilha = estado->pilha;
}

/* =========================================================
   EXIBICAO
   ========================================================= */

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

void exibirPilha(PilhaReserva *pilha)
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

void exibirEstado(FilaCircular *fila, PilhaReserva *pilha)
{
    printf("\nEstado atual:\n");
    exibirFila(fila);
    exibirPilha(pilha);
}

/* =========================================================
   ACOES DO JOGO
   ========================================================= */

void jogarPeca(FilaCircular *fila, PilhaReserva *pilha, PilhaHistorico *historico)
{
    Peca removida;
    Peca nova;

    pushHistorico(historico, capturarEstado(fila, pilha));

    if (dequeue(fila, &removida))
    {
        nova = gerarPeca();
        enqueue(fila, nova);

        printf("\nPeca jogada: [%c %d]\n", removida.nome, removida.id);
        printf("Nova peca inserida na fila: [%c %d]\n", nova.nome, nova.id);
    }
    else
    {
        printf("\nFila vazia.\n");
    }
}

void reservarPeca(FilaCircular *fila, PilhaReserva *pilha, PilhaHistorico *historico)
{
    Peca removida;
    Peca nova;

    if (pilhaCheia(pilha))
    {
        printf("\nPilha de reserva cheia.\n");
        return;
    }

    pushHistorico(historico, capturarEstado(fila, pilha));

    if (dequeue(fila, &removida))
    {
        push(pilha, removida);
        nova = gerarPeca();
        enqueue(fila, nova);

        printf("\nPeca reservada: [%c %d]\n", removida.nome, removida.id);
        printf("Nova peca inserida na fila: [%c %d]\n", nova.nome, nova.id);
    }
    else
    {
        printf("\nFila vazia.\n");
    }
}

void usarPecaReservada(FilaCircular *fila, PilhaReserva *pilha, PilhaHistorico *historico)
{
    Peca usada;

    if (pilhaVazia(pilha))
    {
        printf("\nPilha de reserva vazia.\n");
        return;
    }

    pushHistorico(historico, capturarEstado(fila, pilha));

    if (pop(pilha, &usada))
    {
        printf("\nPeca reservada usada: [%c %d]\n", usada.nome, usada.id);
    }
}

void trocarTopoComFrente(FilaCircular *fila, PilhaReserva *pilha, PilhaHistorico *historico)
{
    Peca pecaFila;
    Peca pecaPilha;

    if (filaVazia(fila))
    {
        printf("\nFila vazia. Nao ha o que trocar.\n");
        return;
    }

    if (pilhaVazia(pilha))
    {
        printf("\nPilha de reserva vazia. Nao ha o que trocar.\n");
        return;
    }

    pushHistorico(historico, capturarEstado(fila, pilha));

    pecaFila = frenteFila(fila);
    pecaPilha = topoPilha(pilha);

    substituirFrenteFila(fila, pecaPilha);
    substituirTopoPilha(pilha, pecaFila);

    printf("\nTroca realizada com sucesso.\n");
    printf("Frente da fila recebeu: [%c %d]\n", pecaPilha.nome, pecaPilha.id);
    printf("Topo da pilha recebeu: [%c %d]\n", pecaFila.nome, pecaFila.id);
}

void desfazerUltimaJogada(FilaCircular *fila, PilhaReserva *pilha, PilhaHistorico *historico)
{
    EstadoJogo estadoAnterior;

    if (popHistorico(historico, &estadoAnterior))
    {
        restaurarEstado(&estadoAnterior, fila, pilha);
        printf("\nUltima jogada desfeita com sucesso.\n");
    }
    else
    {
        printf("\nNao ha jogadas para desfazer.\n");
    }
}

void inverterFilaComPilha(FilaCircular *fila, PilhaReserva *pilha, PilhaHistorico *historico)
{
    if (pilhaVazia(pilha))
    {
        printf("\nPilha vazia. Nao ha o que inverter.\n");
        return;
    }

    pushHistorico(historico, capturarEstado(fila, pilha));

    int qtdTrocas = pilha->topo + 1;
    Peca tempFila[TAM_FILA];
    Peca tempPilha[TAM_PILHA];

    for (int i = 0; i < fila->quantidade; i++)
    {
        int indice = (fila->frente + i) % TAM_FILA;
        tempFila[i] = fila->itens[indice];
    }

    for (int i = 0; i <= pilha->topo; i++)
    {
        tempPilha[i] = pilha->itens[i];
    }

    for (int i = 0; i < qtdTrocas; i++)
    {
        int indiceFila = (fila->frente + i) % TAM_FILA;
        fila->itens[indiceFila] = tempPilha[pilha->topo - i];
        pilha->itens[pilha->topo - i] = tempFila[i];
    }

    printf("\nInversao entre fila e pilha realizada com sucesso.\n");
}

void exibirMenu()
{
    printf("\n1 - Jogar peca\n");
    printf("2 - Reservar peca\n");
    printf("3 - Usar peca reservada\n");
    printf("4 - Trocar topo da pilha com frente da fila\n");
    printf("5 - Desfazer ultima jogada\n");
    printf("6 - Inverter fila com pilha\n");
    printf("0 - Sair\n");
    printf("Escolha uma opcao: ");
}

/* =========================================================
   MAIN
   ========================================================= */

int main()
{
    FilaCircular fila;
    PilhaReserva pilha;
    PilhaHistorico historico;
    int opcao;

    srand(time(NULL));

    inicializarFila(&fila);
    inicializarPilha(&pilha);
    inicializarHistorico(&historico);
    preencherFilaInicial(&fila);

    do
    {
        exibirEstado(&fila, &pilha);
        exibirMenu();
        scanf("%d", &opcao);

        switch (opcao)
        {
            case 1:
                jogarPeca(&fila, &pilha, &historico);
                break;
            case 2:
                reservarPeca(&fila, &pilha, &historico);
                break;
            case 3:
                usarPecaReservada(&fila, &pilha, &historico);
                break;
            case 4:
                trocarTopoComFrente(&fila, &pilha, &historico);
                break;
            case 5:
                desfazerUltimaJogada(&fila, &pilha, &historico);
                break;
            case 6:
                inverterFilaComPilha(&fila, &pilha, &historico);
                break;
            case 0:
                printf("\nEncerrando o jogo...\n");
                break;
            default:
                printf("\nOpcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}