/**
 * Trabalho Prático - Análise Empírica de Complexidade de Algoritmos
 * Disciplina: Estruturas de Dados Básicos I
 * * Este programa avalia empiricamente o tempo de execução de algoritmos
 * de busca (Sequencial e Binária) e ordenação (Quicksort e Mergesort).
 * Ele coleta tempos médios para diferentes tamanhos de entrada (n) e
 * ajusta os valores para identificar a melhor ordem de complexidade
 * assintótica: O(log n), O(n) ou O(n log n).
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// =========================================================================
// 1. ALGORITMOS DE BUSCA
// =========================================================================

/**
 * Busca Sequencial: Percorre o vetor linearmente.
 * Complexidade Teórica: O(n)
 */
int buscaSequencial(const vector<int>& arr, int alvo) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == alvo) return i;
    }
    return -1;
}

/**
 * Busca Binária: Divide o espaço de busca pela metade. Requer vetor ordenado.
 * Complexidade Teórica: O(log n)
 */
int buscaBinaria(const vector<int>& arr, int alvo) {
    int inicio = 0;
    int fim = arr.size() - 1;
    
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        if (arr[meio] == alvo) return meio;
        if (arr[meio] < alvo) inicio = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

// =========================================================================
// 2. ALGORITMOS DE ORDENAÇÃO
// =========================================================================

/**
 * Função auxiliar para o QuickSort (Particionamento de Lomuto)
 */
int particiona(vector<int>& arr, int baixo, int alto) {
    int pivo = arr[alto];
    int i = (baixo - 1);
    
    for (int j = baixo; j <= alto - 1; j++) {
        if (arr[j] < pivo) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[alto]);
    return (i + 1);
}

/**
 * QuickSort: Algoritmo de divisão e conquista.
 * Complexidade Teórica: O(n log n) no caso médio, O(n^2) no pior caso.
 */
void quickSort(vector<int>& arr, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particiona(arr, baixo, alto);
        quickSort(arr, baixo, pi - 1);
        quickSort(arr, pi + 1, alto);
    }
}

/**
 * Função auxiliar para o MergeSort (Intercalação)
 */
void merge(vector<int>& arr, int esq, int meio, int dir) {
    int n1 = meio - esq + 1;
    int n2 = dir - meio;
    
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[esq + i];
    for (int j = 0; j < n2; j++) R[j] = arr[meio + 1 + j];
    
    int i = 0, j = 0, k = esq;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

/**
 * MergeSort: Algoritmo de divisão e conquista estável.
 * Complexidade Teórica: O(n log n) garantido.
 */
void mergeSort(vector<int>& arr, int esq, int dir) {
    if (esq < dir) {
        int meio = esq + (dir - esq) / 2;
        mergeSort(arr, esq, meio);
        mergeSort(arr, meio + 1, dir);
        mergeSort(arr, esq, dir); // Intercala as metades
    }
}

// =========================================================================
// 3. INFRAESTRUTURA DE TESTES E COLETA EMPÍRICA
// =========================================================================

// Estrutura para armazenar os resultados (n, tempo)
struct ResultadoTeste {
    int n;
    double tempoMedioMs; // Tempo em milissegundos
};

// Gera um vetor com números aleatórios
vector<int> gerarVetorAleatorio(int n) {
    vector<int> arr(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, n * 10); // Valores de 1 a 10*n
    
    for (int i = 0; i < n; i++) {
        arr[i] = dis(gen);
    }
    return arr;
}

// =========================================================================
// 4. ANÁLISE DE COMPLEXIDADE (CURVE FITTING)
// =========================================================================

/**
 * Analisa os dados empíricos e indica qual função assintótica melhor se ajusta.
 * Método: Calcula a variância da constante C = Tempo / f(n).
 * A função que tiver C mais "constante" (menor coeficiente de variação) é a vencedora.
 */
void analisarMelhorAjuste(const vector<ResultadoTeste>& resultados, const string& nomeAlgoritmo) {
    if (resultados.empty()) return;

    double mediaC_N = 0, mediaC_NLogN = 0, mediaC_LogN = 0;
    
    // Calcula as médias das constantes
    for (const auto& res : resultados) {
        double n = res.n;
        double t = res.tempoMedioMs;
        
        mediaC_N += t / n;
        mediaC_NLogN += t / (n * log2(n));
        mediaC_LogN += t / log2(n);
    }
    mediaC_N /= resultados.size();
    mediaC_NLogN /= resultados.size();
    mediaC_LogN /= resultados.size();

    double var_N = 0, var_NLogN = 0, var_LogN = 0;
    
    // Calcula a variância
    for (const auto& res : resultados) {
        double n = res.n;
        double t = res.tempoMedioMs;
        
        var_N += pow((t / n) - mediaC_N, 2);
        var_NLogN += pow((t / (n * log2(n))) - mediaC_NLogN, 2);
        var_LogN += pow((t / log2(n)) - mediaC_LogN, 2);
    }

    // Coeficiente de variação (Desvio Padrão / Média)
    double cv_N = (sqrt(var_N / resultados.size())) / mediaC_N;
    double cv_NLogN = (sqrt(var_NLogN / resultados.size())) / mediaC_NLogN;
    double cv_LogN = (sqrt(var_LogN / resultados.size())) / mediaC_LogN;

    cout << "--- Analise de Ajuste Assintotico para " << nomeAlgoritmo << " ---" << endl;
    cout << "Variacao para O(log n):   " << cv_LogN << endl;
    cout << "Variacao para O(n):       " << cv_N << endl;
    cout << "Variacao para O(n log n): " << cv_NLogN << endl;

    cout << "-> Melhor ajuste empirico: ";
    if (cv_N < cv_NLogN && cv_N < cv_LogN) cout << "O(n)" << endl;
    else if (cv_NLogN < cv_N && cv_NLogN < cv_LogN) cout << "O(n log n)" << endl;
    else cout << "O(log n)" << endl;
    cout << "--------------------------------------------------------\n\n";
}

// =========================================================================
// 5. FUNÇÃO PRINCIPAL (ORQUESTRADOR)
// =========================================================================

int main() {
    // Tamanhos crescentes de N para os testes
    vector<int> tamanhosN = {10000, 20000, 40000, 80000, 160000};
    int repeticoes = 10; // Quantidade de vezes que rodamos para tirar a média (minimizar efeitos externos) [cite: 15]

    vector<ResultadoTeste> resBuscaSeq, resBuscaBin, resQuick, resMerge;

    cout << "Iniciando bateria de testes. Isso pode levar alguns segundos...\n\n";

    for (int n : tamanhosN) {
        double tempoTotalSeq = 0, tempoTotalBin = 0;
        double tempoTotalQuick = 0, tempoTotalMerge = 0;

        for (int r = 0; r < repeticoes; r++) {
            // Gera dados iniciais
            vector<int> arrBase = gerarVetorAleatorio(n);
            
            // Para as buscas, vamos procurar um elemento que NÃO está no vetor 
            // para forçar o pior caso e ter dados mais consistentes.
            int alvo = -1; 
            
            // ---------------------------------------------------------
            // Teste: Ordenação QuickSort
            // ---------------------------------------------------------
            vector<int> arrQuick = arrBase; // Cópia para não afetar os outros
            auto inicio = high_resolution_clock::now();
            quickSort(arrQuick, 0, arrQuick.size() - 1);
            auto fim = high_resolution_clock::now();
            tempoTotalQuick += duration<double, milli>(fim - inicio).count();

            // ---------------------------------------------------------
            // Teste: Ordenação MergeSort
            // ---------------------------------------------------------
            vector<int> arrMerge = arrBase;
            inicio = high_resolution_clock::now();
            mergeSort(arrMerge, 0, arrMerge.size() - 1);
            fim = high_resolution_clock::now();
            tempoTotalMerge += duration<double, milli>(fim - inicio).count();

            // ---------------------------------------------------------
            // Teste: Busca Sequencial (usando vetor desordenado)
            // ---------------------------------------------------------
            inicio = high_resolution_clock::now();
            buscaSequencial(arrBase, alvo);
            fim = high_resolution_clock::now();
            // Multiplicamos por 1000 para converter microsegundos se for muito rapido, 
            // mas o tipo double, milli ja mantem a precisao fracionaria em milissegundos.
            tempoTotalSeq += duration<double, milli>(fim - inicio).count();

            // ---------------------------------------------------------
            // Teste: Busca Binária (Obrigatoriamente num vetor ordenado)
            // ---------------------------------------------------------
            // Note que arrQuick já está ordenado pelo teste do QuickSort logo acima.
            inicio = high_resolution_clock::now();
            buscaBinaria(arrQuick, alvo);
            fim = high_resolution_clock::now();
            tempoTotalBin += duration<double, milli>(fim - inicio).count();
        }

        // Armazena as médias (Pares: n, tempo) [cite: 16]
        resBuscaSeq.push_back({n, tempoTotalSeq / repeticoes});
        resBuscaBin.push_back({n, tempoTotalBin / repeticoes});
        resQuick.push_back({n, tempoTotalQuick / repeticoes});
        resMerge.push_back({n, tempoTotalMerge / repeticoes});
    }

    // --- EXIBIÇÃO DE RESULTADOS E AJUSTE DE FUNÇÕES --- [cite: 17, 18, 19]

    cout << fixed << setprecision(5);
    cout << "=== RESULTADOS DA BUSCA (Tempos em ms) ===\n";
    cout << "Tamanho (N)\tSeq (O(n))\tBinaria (O(log n))\n";
    for (size_t i = 0; i < tamanhosN.size(); i++) {
        cout << tamanhosN[i] << "\t\t" << resBuscaSeq[i].tempoMedioMs << "\t\t" << resBuscaBin[i].tempoMedioMs << "\n";
    }
    cout << "\n";
    analisarMelhorAjuste(resBuscaSeq, "Busca Sequencial");
    analisarMelhorAjuste(resBuscaBin, "Busca Binaria");

    cout << "=== RESULTADOS DA ORDENACAO (Tempos em ms) ===\n";
    cout << "Tamanho (N)\tQuickSort\tMergeSort\n";
    for (size_t i = 0; i < tamanhosN.size(); i++) {
        cout << tamanhosN[i] << "\t\t" << resQuick[i].tempoMedioMs << "\t\t" << resMerge[i].tempoMedioMs << "\n";
    }
    cout << "\n";
    analisarMelhorAjuste(resQuick, "QuickSort");
    analisarMelhorAjuste(resMerge, "MergeSort");

    return 0;
}
