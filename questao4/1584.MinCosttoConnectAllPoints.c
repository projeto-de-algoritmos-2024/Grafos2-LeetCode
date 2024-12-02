#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// Função para calcular a distância de Manhattan entre dois pontos
int manhattanDistance(int* point1, int* point2) {
    return abs(point1[0] - point2[0]) + abs(point1[1] - point2[1]);
}

// Função principal para calcular o custo mínimo usando o Algoritmo de Prim
int minCostConnectPoints(int** points, int pointsSize, int* pointsColSize) {
    // Array para marcar pontos visitados
    bool* visited = (bool*)calloc(pointsSize, sizeof(bool));
    
    // Array para armazenar os custos mínimos para cada ponto
    int* minCost = (int*)malloc(pointsSize * sizeof(int));
    for (int i = 0; i < pointsSize; i++) {
        minCost[i] = INT_MAX; // Inicializa com valor máximo
    }

    int totalCost = 0;
    minCost[0] = 0; // Começa pelo primeiro ponto

    for (int i = 0; i < pointsSize; i++) {
        int currPoint = -1;
        int currMinCost = INT_MAX;

        // Encontra o próximo ponto com o menor custo
        for (int j = 0; j < pointsSize; j++) {
            if (!visited[j] && minCost[j] < currMinCost) {
                currMinCost = minCost[j];
                currPoint = j;
            }
        }

        // Marca o ponto atual como visitado
        visited[currPoint] = true;
        totalCost += currMinCost;

        // Atualiza os custos mínimos para os pontos não visitados
        for (int j = 0; j < pointsSize; j++) {
            if (!visited[j]) {
                int distance = manhattanDistance(points[currPoint], points[j]);
                if (distance < minCost[j]) {
                    minCost[j] = distance;
                }
            }
        }
    }

    free(visited);
    free(minCost);
    return totalCost;
}