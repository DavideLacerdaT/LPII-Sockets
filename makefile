# Makefile
CXX = g++
CXXFLAGS = -std=c++11 -pthread -Wall -Wextra

# Alvo principal
main: main.cpp libtslog.h
	$(CXX) $(CXXFLAGS) main.cpp -o main

# Executar o programa
run: main
	@echo "🚀 Executando programa..."
	./main

# Limpar arquivos compilados
clean:
	rm -f main
	@echo "🧹 Arquivos limpos"

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make       - Compila o programa"
	@echo "  make run   - Compila e executa"
	@echo "  make clean - Remove arquivos compilados"
	@echo "  make help  - Mostra esta ajuda"

.PHONY: run clean help