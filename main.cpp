// main.cpp
// Teste da biblioteca de logging com múltiplas threads

#include "libtslog.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

using namespace std;

// Função que será executada por cada thread
void funcao_thread(int id_thread, int numero_logs) {
    LOG_INFO("Thread " + to_string(id_thread) + " iniciou");
    
    for (int i = 1; i <= numero_logs; i++) {
        this_thread::sleep_for(chrono::milliseconds(200));
        
        LOG_INFO("Thread " + to_string(id_thread) + " - mensagem " + to_string(i));
        
        if (i % 2 == 0) {
            LOG_ERROR("Thread " + to_string(id_thread) + " - erro simulado " + to_string(i));
        }
        
        if (i % 3 == 0) {
            LOG_WARN("Thread " + to_string(id_thread) + " - aviso " + to_string(i));
        }
    }
    
    LOG_INFO("Thread " + to_string(id_thread) + " terminou");
}

int main() {
    cout << "=== ETAPA 1: TESTE DE LOGGING THREAD-SAFE ===" << endl;
    cout << "Criando múltiplas threads que geram logs simultaneamente..." << endl;
    cout << endl;
    
    // Testando logs
    LOG_INFO("Iniciando teste da biblioteca de logging");
    LOG_WARN("Este é um teste de aviso");
    LOG_ERROR("Este é um teste de erro");
    
    cout << endl;
    cout << "=== TESTE COM 3 THREADS ===" << endl;
    
    // Testando com threads
    vector<thread> threads;
    int numero_threads = 3;
    int logs_por_thread = 5;
    
    for (int i = 1; i <= numero_threads; i++) {
        threads.push_back(thread(funcao_thread, i, logs_por_thread));
    }
    
    this_thread::sleep_for(chrono::milliseconds(500));
    LOG_WARN("Thread principal ainda rodando...");
    
    for (auto& t : threads) {
        t.join();
    }
    
    cout << endl;
    cout << "=== TESTE DE ESTRESSE COM MAIS THREADS ===" << endl;
    
    vector<thread> mais_threads;
    for (int i = 1; i <= 5; i++) {
        mais_threads.push_back(thread(funcao_thread, i + 10, 3));
    }
    
    for (auto& t : mais_threads) {
        t.join();
    }
    
    cout << endl;
    cout << "=== TESTE CONCLUÍDO ===" << endl;
    LOG_INFO("Todos os logs foram gerados sem problemas de concorrência");
    LOG_INFO("Verifique que nenhuma linha está misturada ou cortada");
    
    return 0;
}