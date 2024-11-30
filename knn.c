#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DATA 150
#define MAX_LINE 100
#define MAX_SPECIES 20

typedef struct {
    double sepal_length, sepal_width, petal_length, petal_width;
    char species[MAX_SPECIES];
} Iris;

typedef struct {
    double distance;
    char species[MAX_SPECIES];
} Distance;

// Função para calcular a distância euclidiana
double euclidean_distance(Iris a, Iris b) {
    return sqrt(pow(a.sepal_length - b.sepal_length, 2) +
                pow(a.sepal_width - b.sepal_width, 2) +
                pow(a.petal_length - b.petal_length, 2) +
                pow(a.petal_width - b.petal_width, 2));
}

// Função para ordenar as distâncias (QuickSort)
void quicksort(Distance *distances, int left, int right) {
    if (left < right) {
        int i = left, j = right;
        Distance pivot = distances[(left + right) / 2];
        while (i <= j) {
            while (distances[i].distance < pivot.distance) i++;
            while (distances[j].distance > pivot.distance) j--;
            if (i <= j) {
                Distance temp = distances[i];
                distances[i] = distances[j];
                distances[j] = temp;
                i++;
                j--;
            }
        }
        quicksort(distances, left, j);
        quicksort(distances, i, right);
    }
}

// Função para determinar a classe com base nos K vizinhos
void classify(int k, Distance *distances, int training_size, char *result) {
    int counts[MAX_DATA] = {0};
    char classes[MAX_DATA][MAX_SPECIES];
    int num_classes = 0;

    // Contar os votos
    for (int i = 0; i < k; i++) {
        int found = 0;
        for (int j = 0; j < num_classes; j++) {
            if (strcmp(distances[i].species, classes[j]) == 0) {
                counts[j]++;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(classes[num_classes], distances[i].species);
            counts[num_classes]++;
            num_classes++;
        }
    }

    // Encontrar a classe com mais votos
    int max_index = 0;
    for (int i = 1; i < num_classes; i++) {
        if (counts[i] > counts[max_index]) {
            max_index = i;
        }
    }
    strcpy(result, classes[max_index]);
}

int main() {
    char training_file[MAX_LINE], test_file[MAX_LINE];
    int k;

    // Solicitar o arquivo de treinamento
    printf("Digite o nome do arquivo de treinamento: ");
    scanf("%s", training_file);

    // Solicitar o valor de K
    printf("Digite o valor de K: ");
    scanf("%d", &k);

    // Solicitar o arquivo de teste
    printf("Digite o nome do arquivo de teste: ");
    scanf("%s", test_file);

    // Abrir arquivo de treinamento
    FILE *training_fp = fopen(training_file, "r");
    if (!training_fp) {
        printf("Erro ao abrir o arquivo de treinamento.\n");
        return 1;
    }

    // Ler dados de treinamento
    Iris training_data[MAX_DATA];
    int training_size = 0;
    while (fscanf(training_fp, "%lf,%lf,%lf,%lf,%s",
                  &training_data[training_size].sepal_length,
                  &training_data[training_size].sepal_width,
                  &training_data[training_size].petal_length,
                  &training_data[training_size].petal_width,
                  training_data[training_size].species) == 5) {
        training_size++;
    }
    fclose(training_fp);

    if (k > training_size) {
        printf("Erro: K não pode ser maior que o número de registros de treinamento (%d).\n", training_size);
        return 1;
    }

    // Abrir arquivo de teste
    FILE *test_fp = fopen(test_file, "r");
    if (!test_fp) {
        printf("Erro ao abrir o arquivo de teste.\n");
        return 1;
    }

    // Ler dados de teste e classificar
    Iris test_data;
    while (fscanf(test_fp, "%lf,%lf,%lf,%lf",
                  &test_data.sepal_length,
                  &test_data.sepal_width,
                  &test_data.petal_length,
                  &test_data.petal_width) == 4) {
        // Calcular distâncias
        Distance distances[MAX_DATA];
        for (int i = 0; i < training_size; i++) {
            distances[i].distance = euclidean_distance(test_data, training_data[i]);
            strcpy(distances[i].species, training_data[i].species);
        }

        // Ordenar distâncias
        quicksort(distances, 0, training_size - 1);

        // Classificar o registro de teste
        char predicted_species[MAX_SPECIES];
        classify(k, distances, training_size, predicted_species);

        // Imprimir resultado
        printf("Classificado como: %s\n", predicted_species);
    }
    fclose(test_fp);

    return 0;
}
