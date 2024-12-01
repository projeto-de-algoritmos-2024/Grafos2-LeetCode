#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Define os limites do problema
#define MAX_NODES 12          // Máximo de nós no grafo
#define MAX_QUEUE_SIZE (1 << MAX_NODES) // Tamanho máximo da fila baseado nos estados

// Estrutura que representa um estado no algoritmo Dijkstra
typedef struct {
    int cost;   // Custo acumulado até o estado
    int node;   // Nó atual
    int state;  // Estado dos nós visitados, representado por um número binário
} State;

// Estrutura para a implementação de uma min-heap (fila de prioridade mínima)
typedef struct {
    State* data;    // Array de elementos na heap
    int size;       // Número atual de elementos
    int capacity;   // Capacidade máxima da heap
} MinHeap;

// Função para criar uma heap com capacidade específica
MinHeap* createHeap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->data = (State*)malloc(sizeof(State) * capacity);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Função para trocar dois elementos na heap
void swap(State* a, State* b) {
    State temp = *a;
    *a = *b;
    *b = temp;
}

// Função para reorganizar a heap ao inserir um novo elemento (sobe o menor elemento)
void heapifyUp(MinHeap* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2; // Índice do pai
        if (heap->data[parent].cost > heap->data[idx].cost) {
            swap(&heap->data[parent], &heap->data[idx]);
            idx = parent;
        } else {
            break;
        }
    }
}

// Função para reorganizar a heap ao remover o menor elemento (desce o menor elemento)
void heapifyDown(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;  // Filho esquerdo
    int right = 2 * idx + 2; // Filho direito

    // Verifica se o filho esquerdo é menor
    if (left < heap->size && heap->data[left].cost < heap->data[smallest].cost)
        smallest = left;

    // Verifica se o filho direito é menor
    if (right < heap->size && heap->data[right].cost < heap->data[smallest].cost)
        smallest = right;

    // Se encontramos um menor, trocamos e repetimos
    if (smallest != idx) {
        swap(&heap->data[smallest], &heap->data[idx]);
        heapifyDown(heap, smallest);
    }
}

// Insere um novo estado na heap
void push(MinHeap* heap, State s) {
    heap->data[heap->size++] = s; // Adiciona ao final
    heapifyUp(heap, heap->size - 1); // Reorganiza para manter a propriedade da heap
}

// Remove e retorna o estado com menor custo na heap
State pop(MinHeap* heap) {
    State top = heap->data[0];       // Menor elemento está no topo
    heap->data[0] = heap->data[--heap->size]; // Substitui pelo último elemento
    heapifyDown(heap, 0);            // Reorganiza a heap
    return top;
}

// Verifica se a heap está vazia
int isHeapEmpty(MinHeap* heap) {
    return heap->size == 0;
}

// Função principal para resolver o problema
int shortestPathLength(int** graph, int graphSize, int* graphColSize) {
    int finalState = (1 << graphSize) - 1; // Estado final onde todos os nós foram visitados (ex: 111...1)
    MinHeap* heap = createHeap(MAX_QUEUE_SIZE); // Cria a heap para armazenar estados
    int visited[MAX_NODES][1 << MAX_NODES];     // Marca os estados visitados
    memset(visited, 0, sizeof(visited));        // Inicializa como não visitado

    // Inicializa a heap com todos os nós como ponto de partida
    for (int i = 0; i < graphSize; i++) {
        State s = { .cost = 0, .node = i, .state = 1 << i }; // Estado inicial para cada nó
        push(heap, s);
        visited[i][1 << i] = 1; // Marca como visitado
    }

    // Algoritmo de Dijkstra
    while (!isHeapEmpty(heap)) {
        State current = pop(heap); // Remove o estado com menor custo

        // Se todos os nós foram visitados, retorna o custo acumulado
        if (current.state == finalState) {
            free(heap->data);
            free(heap);
            return current.cost;
        }

        // Explora os vizinhos do nó atual
        for (int i = 0; i < graphColSize[current.node]; i++) {
            int neighbor = graph[current.node][i]; // Vizinho atual
            int nextState = current.state | (1 << neighbor); // Atualiza o estado com o nó vizinho

            // Se este estado ainda não foi visitado
            if (!visited[neighbor][nextState]) {
                visited[neighbor][nextState] = 1; // Marca como visitado
                State next = { .cost = current.cost + 1, .node = neighbor, .state = nextState };
                push(heap, next); // Adiciona à heap
            }
        }
    }

    free(heap->data);
    free(heap);
    return -1; // Não deve acontecer
}

// Função auxiliar para criar uma matriz de adjacência
int** createGraph(int* edges[], int graphSize, int* graphColSize) {
    int** graph = (int**)malloc(graphSize * sizeof(int*));
    for (int i = 0; i < graphSize; i++) {
        graph[i] = edges[i];
    }
    return graph;
}