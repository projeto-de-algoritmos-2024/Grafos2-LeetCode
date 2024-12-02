#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Define os limites do problema
#define MAX_NODES 12          // Máximo de nós que o grafo pode ter
#define MAX_QUEUE_SIZE (1 << MAX_NODES) // Tamanho máximo da fila baseado nos estados possíveis (2^n)

// Estrutura que representa um estado no algoritmo de busca (usado em Dijkstra)
typedef struct {
    int cost;   // Custo acumulado até o estado atual
    int node;   // Nó atual
    int state;  // Estado representado pelos nós visitados, codificado como um número binário
} State;

// Estrutura para a implementação de uma fila de prioridade mínima (min-heap)
typedef struct {
    State* data;    // Array para armazenar os elementos da heap
    int size;       // Número atual de elementos na heap
    int capacity;   // Capacidade máxima da heap
} MinHeap;

// Função para criar uma heap com uma capacidade específica
MinHeap* createHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->data = (State*)malloc(sizeof(State) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Redimensiona a heap dinamicamente, dobrando sua capacidade
void resizeHeap(MinHeap* heap) {
    heap->capacity *= 2; // Dobre a capacidade
    heap->data = (State*)realloc(heap->data, sizeof(State) * heap->capacity);
    if (!heap->data) {
        fprintf(stderr, "Erro: Falha ao realocar memória para a heap!\n");
        exit(EXIT_FAILURE);
    }
}

// Função para trocar dois elementos na heap
void swap(State* a, State* b) {
    State temp = *a;
    *a = *b;
    *b = temp;
}

// Reorganiza a heap após a inserção de um novo elemento (heapify-up)
void heapifyUp(MinHeap* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2; // Índice do pai
        if (heap->data[parent].cost > heap->data[idx].cost) {
            swap(&heap->data[parent], &heap->data[idx]); // Troca com o pai, se necessário
            idx = parent; // Continua subindo
        } else {
            break; // Propriedade da heap está satisfeita
        }
    }
}

// Reorganiza a heap após a remoção do menor elemento (heapify-down)
void heapifyDown(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;  // Índice do filho esquerdo
    int right = 2 * idx + 2; // Índice do filho direito

    // Verifica se o filho esquerdo é menor que o atual
    if (left < heap->size && heap->data[left].cost < heap->data[smallest].cost)
        smallest = left;

    // Verifica se o filho direito é menor que o menor encontrado até agora
    if (right < heap->size && heap->data[right].cost < heap->data[smallest].cost)
        smallest = right;

    // Se o menor não é o nó atual, troca e continua descendo
    if (smallest != idx) {
        swap(&heap->data[smallest], &heap->data[idx]);
        heapifyDown(heap, smallest);
    }
}

// Insere um novo estado na heap
void push(MinHeap* heap, State s) {
    if (heap->size == heap->capacity) {
        resizeHeap(heap); // Redimensiona a heap se necessário
    }
    heap->data[heap->size++] = s; // Adiciona o novo elemento ao final
    heapifyUp(heap, heap->size - 1); // Ajusta a heap para manter sua propriedade
}

// Remove e retorna o estado com menor custo da heap
State pop(MinHeap* heap) {
    State top = heap->data[0]; // O menor elemento está no topo
    heap->data[0] = heap->data[--heap->size]; // Substitui o topo pelo último elemento
    heapifyDown(heap, 0); // Ajusta a heap para manter sua propriedade
    return top;
}

// Verifica se a heap está vazia
int isHeapEmpty(MinHeap* heap) {
    return heap->size == 0;
}

// Função principal para encontrar o menor caminho que visita todos os nós (problema do caixeiro viajante simplificado)
int shortestPathLength(int** graph, int graphSize, int* graphColSize) {
    // Define o estado final (todos os nós visitados), ex: para 3 nós, será 111 (binário)
    int finalState = (1 << graphSize) - 1;

    // Cria a heap para armazenar os estados a serem processados
    MinHeap* heap = createHeap(MAX_QUEUE_SIZE);

    // Array para rastrear os estados já visitados
    int visited[MAX_NODES][1 << MAX_NODES];
    memset(visited, 0, sizeof(visited)); // Inicializa como não visitados

    // Inicializa a heap com todos os nós como pontos de partida
    for (int i = 0; i < graphSize; i++) {
        State s = { .cost = 0, .node = i, .state = 1 << i }; // Estado inicial de cada nó
        push(heap, s); // Adiciona o estado à heap
        visited[i][1 << i] = 1; // Marca este estado como visitado
    }

    // Algoritmo principal (variação do Dijkstra)
    while (!isHeapEmpty(heap)) {
        // Remove o estado com menor custo
        State current = pop(heap);

        // Verifica se todos os nós foram visitados
        if (current.state == finalState) {
            free(heap->data);
            free(heap);
            return current.cost; // Retorna o custo acumulado
        }

        // Explora os vizinhos do nó atual
        for (int i = 0; i < graphColSize[current.node]; i++) {
            int neighbor = graph[current.node][i]; // Vizinho atual
            int nextState = current.state | (1 << neighbor); // Atualiza o estado com o nó vizinho

            // Se este estado ainda não foi visitado
            if (!visited[neighbor][nextState]) {
                visited[neighbor][nextState] = 1; // Marca como visitado
                State next = { .cost = current.cost + 1, .node = neighbor, .state = nextState };
                push(heap, next); // Adiciona o novo estado à heap
            }
        }
    }

    free(heap->data);
    free(heap);
    return -1; // Este caso não deve ocorrer
}

// Função auxiliar para criar um grafo a partir de uma matriz de adjacência
int** createGraph(int* edges[], int graphSize, int* graphColSize) {
    int** graph = (int**)malloc(graphSize * sizeof(int*));
    for (int i = 0; i < graphSize; i++) {
        graph[i] = edges[i]; // Cada linha da matriz corresponde a um nó e seus vizinhos
    }
    return graph;
}
