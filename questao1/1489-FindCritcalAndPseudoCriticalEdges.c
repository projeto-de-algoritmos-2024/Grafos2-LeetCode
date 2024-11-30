#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_NODES 100  // Definição do número máximo de nós no grafo
#define MAX_EDGES 1000 // Definição do número máximo de arestas no grafo

// Estrutura que representa uma aresta no grafo
typedef struct {
    int u, v, weight, index;
} Edge;

// Arrays globais para o algoritmo de Union-Find (usado para detectar componentes conectados no grafo)
int parent[MAX_NODES];
int rank[MAX_NODES];

// Função para inicializar o conjunto disjunto (Union-Find)
void makeSet(int n) {
    for (int i = 0; i < n; i++) {
        parent[i] = i; // Cada nó é seu próprio pai inicialmente
        rank[i] = 0;    // O rank começa em 0
    }
}

// Função de busca (find) com compressão de caminho
int find(int x) {
    if (x != parent[x]) { // Se o nó não for o representante de seu conjunto
        parent[x] = find(parent[x]); // Caminha até a raiz, comprimindo o caminho
    }
    return parent[x]; // Retorna o representante do conjunto
}

// Função de união (union) para juntar dois conjuntos
void unionSets(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    if (rootX != rootY) { // Se eles estiverem em conjuntos diferentes
        if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX; // A raiz do conjunto de maior rank se torna o representante do outro conjunto
        } else if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        } else {
            parent[rootY] = rootX;
            rank[rootX]++; // Se ambos têm o mesmo rank, o rank aumenta
        }
    }
}

// Função para comparar arestas de forma crescente com base no peso (necessária para ordenar as arestas)
int compareEdges(const void* a, const void* b) {
    return ((Edge*)a)->weight - ((Edge*)b)->weight;
}

// Função para calcular o peso da MST (árvore geradora mínima)
// A função pode pular ou forçar a inclusão de uma aresta específica durante o cálculo
int calculateMST(int n, Edge* edges, int edgesSize, int skipEdge, int forceEdge) {
    makeSet(n);  // Inicializa o conjunto disjunto
    int weight = 0; // Variável para armazenar o peso total da MST
    int edgeCount = 0; // Contador de arestas na MST

    // Se for forçar a inclusão de uma aresta, faz isso primeiro
    if (forceEdge != -1) {
        unionSets(edges[forceEdge].u, edges[forceEdge].v); // União das extremidades da aresta
        weight += edges[forceEdge].weight; // Soma o peso da aresta forçada
        edgeCount++; // Conta a aresta forçada
    }

    // Itera sobre todas as arestas
    for (int i = 0; i < edgesSize; i++) {
        if (i == skipEdge) continue; // Pula a aresta a ser excluída
        if (find(edges[i].u) != find(edges[i].v)) { // Se as extremidades das arestas não estão conectadas
            unionSets(edges[i].u, edges[i].v); // Faz a união dos conjuntos das extremidades
            weight += edges[i].weight; // Soma o peso da aresta
            edgeCount++; // Conta a aresta
        }
    }

    // Se a quantidade de arestas na MST for igual a n-1, retornamos o peso da MST
    // Caso contrário, retornamos INT_MAX, significando que a MST não foi formada
    return (edgeCount == n - 1) ? weight : INT_MAX;
}

// Função principal para encontrar as arestas críticas e pseudo-críticas
int** findCriticalAndPseudoCriticalEdges(int n, int** edges, int edgesSize, int* edgesColSize, int* returnSize, int** returnColumnSizes) {
    Edge edgeList[MAX_EDGES]; // Array para armazenar as arestas, com limite de `MAX_EDGES`

    // Converter as arestas recebidas (de uma matriz de inteiros) para a estrutura `Edge`
    for (int i = 0; i < edgesSize; i++) {
        edgeList[i].u = edges[i][0];     // Nó de origem
        edgeList[i].v = edges[i][1];     // Nó de destino
        edgeList[i].weight = edges[i][2]; // Peso da aresta
        edgeList[i].index = i;            // Índice original da aresta
    }

    // Ordenar as arestas por peso
    qsort(edgeList, edgesSize, sizeof(Edge), compareEdges);

    // Calcular o peso da MST sem excluir ou forçar nenhuma aresta
    int baseWeight = calculateMST(n, edgeList, edgesSize, -1, -1);

    // Alocar memória para armazenar o resultado: duas listas de índices de arestas
    int** result = (int**)malloc(2 * sizeof(int*));
    result[0] = (int*)malloc(edgesSize * sizeof(int)); // Lista para arestas críticas
    result[1] = (int*)malloc(edgesSize * sizeof(int)); // Lista para arestas pseudo-críticas
    int criticalCount = 0, pseudoCriticalCount = 0; // Contadores para as listas

    // Verificar cada aresta
    for (int i = 0; i < edgesSize; i++) {
        // Verificar se a aresta é crítica (se a exclusão dela aumenta o peso da MST)
        if (calculateMST(n, edgeList, edgesSize, i, -1) > baseWeight) {
            result[0][criticalCount++] = edgeList[i].index; // Se for crítica, adiciona ao resultado
        } 
        // Verificar se a aresta é pseudo-crítica (se a inclusão dela não muda o peso da MST)
        else if (calculateMST(n, edgeList, edgesSize, -1, i) == baseWeight) {
            result[1][pseudoCriticalCount++] = edgeList[i].index; // Se for pseudo-crítica, adiciona ao resultado
        }
    }

    // Definir o tamanho do retorno
    *returnSize = 2;
    *returnColumnSizes = (int*)malloc(2 * sizeof(int));
    (*returnColumnSizes)[0] = criticalCount; // Número de arestas críticas
    (*returnColumnSizes)[1] = pseudoCriticalCount; // Número de arestas pseudo-críticas

    return result; // Retorna o resultado com as duas listas
}

/*int main() {
    // Número de nós no grafo
    int n = 5;

    // Representação das arestas do grafo
    // Cada linha representa: {nó origem, nó destino, peso da aresta}
    int edges[][3] = {
        {0, 1, 1}, // Aresta conectando nó 0 ao nó 1 com peso 1
        {1, 2, 1}, // Aresta conectando nó 1 ao nó 2 com peso 1
        {2, 3, 2}, // Aresta conectando nó 2 ao nó 3 com peso 2
        {0, 3, 2}, // Aresta conectando nó 0 ao nó 3 com peso 2
        {0, 4, 3}, // Aresta conectando nó 0 ao nó 4 com peso 3
        {3, 4, 3}, // Aresta conectando nó 3 ao nó 4 com peso 3
        {1, 4, 6}  // Aresta conectando nó 1 ao nó 4 com peso 6
    };

    // Quantidade total de arestas (número de linhas na matriz `edges`)
    int edgesSize = sizeof(edges) / sizeof(edges[0]);
    // Número de colunas em cada linha de `edges` (nó origem, destino e peso)
    int edgesColSize = 3;

    // Converter a matriz de arestas para um array de ponteiros
    // Isso é necessário porque a função `findCriticalAndPseudoCriticalEdges` espera um **ponteiro para ponteiro**
    int** edgesPtr = (int**)malloc(edgesSize * sizeof(int*));
    for (int i = 0; i < edgesSize; i++) {
        edgesPtr[i] = (int*)malloc(edgesColSize * sizeof(int));
        memcpy(edgesPtr[i], edges[i], edgesColSize * sizeof(int)); // Copia cada linha de `edges` para o novo array
    }

    // Variáveis para capturar o retorno da função
    int returnSize;          // Tamanho do array de retorno
    int* returnColumnSizes;  // Tamanhos das sublistas de retorno (arestas críticas e pseudo-críticas)

    // Chamar a função principal que identifica arestas críticas e pseudo-críticas
    int** result = findCriticalAndPseudoCriticalEdges(n, edgesPtr, edgesSize, &edgesColSize, &returnSize, &returnColumnSizes);

    // Exibir as arestas críticas
    printf("Critical Edges: ");
    for (int i = 0; i < returnColumnSizes[0]; i++) {
        printf("%d ", result[0][i]); // Índices das arestas críticas
    }
    printf("\n");

    // Exibir as arestas pseudo-críticas
    printf("Pseudo-Critical Edges: ");
    for (int i = 0; i < returnColumnSizes[1]; i++) {
        printf("%d ", result[1][i]); // Índices das arestas pseudo-críticas
    }
    printf("\n");

    // Limpar a memória alocada dinamicamente
    for (int i = 0; i < edgesSize; i++) {
        free(edgesPtr[i]); // Libera cada linha de `edgesPtr`
    }
    free(edgesPtr);              // Libera o array principal de ponteiros
    free(result[0]);             // Libera a lista de arestas críticas
    free(result[1]);             // Libera a lista de arestas pseudo-críticas
    free(result);                // Libera o array principal de retorno
    free(returnColumnSizes);     // Libera o array de tamanhos das sublistas

    return 0; // Indica que o programa terminou com sucesso
}*/ 
