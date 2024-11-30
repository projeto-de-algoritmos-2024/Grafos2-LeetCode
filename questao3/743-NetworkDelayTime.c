#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Definindo um número máximo de nós (pode ser ajustado conforme necessário)
#define MAX_NODES 101

// Estrutura de uma aresta que conecta dois nós (node) com um peso (weight)
typedef struct {
    int node, weight;
} Edge;

// Estrutura de uma lista de adjacências para armazenar as arestas
typedef struct {
    Edge *edges;
    int size, capacity;
} AdjList;

// Função para inicializar a lista de adjacências para um nó
void initList(AdjList *list) {
    list->size = 0;  // Inicializamos o tamanho da lista como 0
    list->capacity = 10;  // Inicializamos a capacidade da lista com 10
    list->edges = malloc(list->capacity * sizeof(Edge));  // Alocando memória para armazenar as arestas
}

// Função para adicionar uma aresta à lista de adjacências de um nó
void addEdge(AdjList *list, int node, int weight) {
    if (list->size == list->capacity) {  // Se a lista estiver cheia, duplicamos sua capacidade
        list->capacity *= 2;
        list->edges = realloc(list->edges, list->capacity * sizeof(Edge));
    }
    list->edges[list->size++] = (Edge){node, weight};  // Adicionamos a aresta à lista
}

// Função para liberar a memória alocada para a lista de adjacências
void freeList(AdjList *list) {
    free(list->edges);  // Liberamos a memória ocupada pelas arestas
}

// Função que implementa o algoritmo de Dijkstra
int dijkstra(AdjList *graph, int n, int k) {
    int dist[MAX_NODES];  // Array para armazenar as distâncias mínimas dos nós a partir do nó inicial
    int visited[MAX_NODES] = {0};  // Array de controle para verificar se o nó já foi visitado

    for (int i = 1; i <= n; i++) {
        dist[i] = INT_MAX;  // Inicializamos todas as distâncias com o valor máximo possível
    }
    dist[k] = 0;  // A distância do nó inicial para ele mesmo é 0

    // Loop principal do algoritmo de Dijkstra
    for (int i = 1; i <= n; i++) {
        int u = -1;
        // Encontramos o nó com a menor distância não visitado
        for (int j = 1; j <= n; j++) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (dist[u] == INT_MAX) break;  // Se a distância for infinita, não há mais nós acessíveis
        visited[u] = 1;  // Marcamos o nó como visitado

        // Atualizamos as distâncias dos nós vizinhos do nó atual
        for (int j = 0; j < graph[u].size; j++) {
            int v = graph[u].edges[j].node;  // O nó vizinho
            int weight = graph[u].edges[j].weight;  // O peso da aresta
            if (dist[u] + weight < dist[v]) {  // Se encontramos uma distância menor para o nó vizinho
                dist[v] = dist[u] + weight;  // Atualizamos a distância
            }
        }
    }

    // Encontramos a maior distância entre os nós alcançáveis
    int maxTime = 0;
    for (int i = 1; i <= n; i++) {
        if (dist[i] == INT_MAX) return -1;  // Se algum nó não foi alcançado, retornamos -1
        if (dist[i] > maxTime) maxTime = dist[i];  // Mantemos a maior distância
    }
    return maxTime;  // Retornamos a maior distância, que é o tempo máximo necessário para que o sinal alcance todos os nós
}

// Função principal que resolve o problema de "Network Delay Time"
int networkDelayTime(int **times, int timesSize, int *timesColSize, int n, int k) {
    AdjList graph[MAX_NODES];  // Grafo representado por listas de adjacências
    for (int i = 1; i <= n; i++) {
        initList(&graph[i]);  // Inicializamos a lista de adjacências de cada nó
    }

    // Preenchemos as listas de adjacências com as arestas do grafo
    for (int i = 0; i < timesSize; i++) {
        int u = times[i][0];  // Nó de origem
        int v = times[i][1];  // Nó de destino
        int w = times[i][2];  // Peso da aresta
        addEdge(&graph[u], v, w);  // Adicionamos a aresta ao grafo
    }

    int result = dijkstra(graph, n, k);  // Aplicamos o algoritmo de Dijkstra para encontrar o tempo de atraso

    // Liberamos a memória alocada para as listas de adjacências
    for (int i = 1; i <= n; i++) {
        freeList(&graph[i]);
    }

    return result;  // Retornamos o tempo necessário para que o sinal alcance todos os nós, ou -1 se não for possível
}

