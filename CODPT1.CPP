/**
 * Trabalho Prático - Análise Empírica de Complexidade de Algoritmos
 * Disciplina: Estruturas de Dados Básicos I
 * * OBJETIVO: Avaliar empiricamente o tempo de execução de algoritmos e comparar
 * com funções assintóticas conhecidas.
 * * REQUISITOS TÉCNICOS ATENDIDOS:
 * - Modularidade: Uso de std::function para injetar algoritmos.
 * - Plotagem: Exportação automática para CSV para geração de gráficos.
 * - Medição: Utilização da biblioteca <chrono>.
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <functional> 
#include <fstream>    

using namespace std;
using namespace std::chrono;

// Estrutura para armazenar os resultados (n, tempo médio)
struct ResultadoTeste {
    int n;
    double tempoMedioMs;
};

// =========================================================================
// 1. NÚCLEO DE ALGORITMOS (BUSCA E ORDENAÇÃO)
// =========================================================================

int buscaSequencial(const vector<int>& arr, int alvo) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == alvo) return i;
    }
    return -1;
}

int buscaBinaria(const vector<int>& arr, int alvo) {
    int inicio = 0, fim = arr.size() - 1;
    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        if (arr[meio] == alvo) return meio;
        if (arr[meio] < alvo) inicio = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

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

void quickSort(vector<int>& arr, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particiona(arr, baixo, alto);
        quickSort(arr, baixo, pi - 1);
        quickSort(arr, pi + 1, alto);
    }
}

void merge(vector<int>& arr, int esq, int meio, int dir) {
    int n1 = meio - esq + 1, n2 = dir - meio;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[esq + i];
    for (int j = 0; j < n2; j++) R[j] = arr[meio + 1 + j];
    
    int i = 0, j = 0, k = esq;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr, int esq, int dir) {
    if (esq < dir) {
        int meio = esq + (dir - esq) / 2;
        mergeSort(arr, esq, meio);
        mergeSort(arr, meio + 1, dir);
        merge(arr, esq, meio, dir); // Chamada correta da função de junção
    }
}

// =========================================================================
// 2. ENGINE DE TESTES (MODULARIDADE)
// =========================================================================

vector<int> gerarVetorAleatorio(int n) {
    vector<int> arr(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, n * 10);
    for (int i = 0; i < n; i++) arr[i] = dis(gen);
    return arr;
}

// Permite chamadas de outras funções externas ao projeto original
double executarTesteOrdenacao(function<void(vector<int>&, int, int)> algoritmo, vector<int>& arr) {
    auto inicio = high_resolution_clock::now();
    algoritmo(arr, 0, arr.size() - 1);
    auto fim = high_resolution_clock::now();
    return duration<double, milli>(fim - inicio).count();
}

double executarTesteBusca(function<int(const vector<int>&, int)> algoritmo, const vector<int>& arr, int alvo) {
    auto inicio = high_resolution_clock::now();
    algoritmo(arr, alvo);
    auto fim = high_resolution_clock::now();
    return duration<double, milli>(fim - inicio).count();
}

// =========================================================================
// 3. EXPORTAÇÃO E ANÁLISE ASSINTÓTICA
// =========================================================================

void exportarParaCSV(const vector<int>& tamanhosN, const vector<ResultadoTeste>& resSeq, 
                     const vector<ResultadoTeste>& resBin, const vector<ResultadoTeste>& resQuick, 
                     const vector<ResultadoTeste>& resMerge) {
    
    // Gera o arquivo na mesma pasta do executável
    ofstream arquivo("resultados_plotagem.csv");
    
    if (arquivo.is_open()) {
        // MUDANÇA AQUI: Usando ponto e vírgula (;) para o Excel brasileiro separar as colunas perfeitamente
        arquivo << "Tamanho(N);BuscaSeq(ms);BuscaBin(ms);QuickSort(ms);MergeSort(ms)\n";
        for (size_t i = 0; i < tamanhosN.size(); i++) {
            arquivo << tamanhosN[i] << ";" 
                    << resSeq[i].tempoMedioMs << ";"
                    << resBin[i].tempoMedioMs << ";" 
                    << resQuick[i].tempoMedioMs << ";"
                    << resMerge[i].tempoMedioMs << "\n";
        }
        arquivo.close();
        cout << "\n[!] Arquivo 'resultados_plotagem.csv' gerado com sucesso!\n";
    } else {
        cerr << "\n[ERRO] Nao foi possivel criar o arquivo CSV. Verifique as permissoes da pasta.\n";
    }
}

void analisarMelhorAjuste(const vector<ResultadoTeste>& resultados, const string& nomeAlgoritmo) {
    if (resultados.empty()) return;
    double mediaC_N = 0, mediaC_NLogN = 0, mediaC_LogN = 0;
    
    for (const auto& res : resultados) {
        double n = res.n, t = res.tempoMedioMs;
        mediaC_N += t / n;
        mediaC_NLogN += t / (n * log2(n));
        mediaC_LogN += t / log2(n);
    }
    mediaC_N /= resultados.size(); mediaC_NLogN /= resultados.size(); mediaC_LogN /= resultados.size();

    double var_N = 0, var_NLogN = 0, var_LogN = 0;
    for (const auto& res : resultados) {
        double n = res.n, t = res.tempoMedioMs;
        var_N += pow((t / n) - mediaC_N, 2);
        var_NLogN += pow((t / (n * log2(n))) - mediaC_NLogN, 2);
        var_LogN += pow((t / log2(n)) - mediaC_LogN, 2);
    }

    double cv_N = (sqrt(var_N / resultados.size())) / mediaC_N;
    double cv_NLogN = (sqrt(var_NLogN / resultados.size())) / mediaC_NLogN;
    double cv_LogN = (sqrt(var_LogN / resultados.size())) / mediaC_LogN;

    cout << "-> Melhor ajuste empirico (" << nomeAlgoritmo << "): ";
    if (cv_N < cv_NLogN && cv_N < cv_LogN) cout << "O(n)\n";
    else if (cv_NLogN < cv_N && cv_NLogN < cv_LogN) cout << "O(n log n)\n";
    else cout << "O(log n)\n";
}

// =========================================================================
// 4. FUNÇÃO PRINCIPAL
// =========================================================================

int main() {
    try {
        // Valores reduzidos para testes rápidos. 
        // Quando quiser testar de verdade, pode voltar para {1000, 2000, 5000,}
        vector<int> tamanhosN = {1000, 2000, 5000};
        int repeticoes = 5; 

        vector<ResultadoTeste> resBuscaSeq, resBuscaBin, resQuick, resMerge;

        cout << "===============================================" << endl;
        cout << "INICIANDO COLETA DE DADOS PARA O RELATORIO..." << endl;
        cout << "===============================================" << endl;

        for (int n : tamanhosN) {
            cout << "[#] Testando N = " << n << "..." << endl;
            double tSeq = 0, tBin = 0, tQuick = 0, tMerge = 0;

            for (int r = 0; r < repeticoes; r++) {
                vector<int> arrBase = gerarVetorAleatorio(n);
                int alvo = -1; 
                vector<int> arrQuick = arrBase;
                vector<int> arrMerge = arrBase;

                tQuick += executarTesteOrdenacao(quickSort, arrQuick);
                tMerge += executarTesteOrdenacao(mergeSort, arrMerge);
                tSeq += executarTesteBusca(buscaSequencial, arrBase, alvo);
                tBin += executarTesteBusca(buscaBinaria, arrQuick, alvo); 
            }

            resBuscaSeq.push_back({n, tSeq / repeticoes});
            resBuscaBin.push_back({n, tBin / repeticoes});
            resQuick.push_back({n, tQuick / repeticoes});
            resMerge.push_back({n, tMerge / repeticoes});
        }

        cout << "\n[!] Analisando complexidade assintotica..." << endl;
        analisarMelhorAjuste(resBuscaSeq, "Busca Sequencial");
        analisarMelhorAjuste(resBuscaBin, "Busca Binaria");
        analisarMelhorAjuste(resQuick, "QuickSort");
        analisarMelhorAjuste(resMerge, "MergeSort");

        cout << "\n[!] Gerando arquivo resultados_plotagem.csv..." << endl;
        exportarParaCSV(tamanhosN, resBuscaSeq, resBuscaBin, resQuick, resMerge);

        cout << "\n===============================================" << endl;
        cout << "SUCESSO! O arquivo CSV foi criado na mesma pasta do executavel." << endl;
        cout << "Pressione ENTER para finalizar." << endl;
        cout << "===============================================" << endl;

    } catch (const exception& e) {
        cerr << "\n!!! ERRO DETECTADO: " << e.what() << " !!!" << endl;
    }

    cin.ignore();
    cin.get(); 
    return 0;
}
