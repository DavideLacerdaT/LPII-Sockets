CXX = g++
CXXFLAGS = -std=c++17 -pthread -Wall -Wextra

# Listagem de arquivos objeto
OBJS = main.o server.o client.o chatroom.o

# Alvo principal
app: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o app

# Compilação dos objetos
main.o: main.cpp server.h client.h chatroom.h libtslog.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

server.o: server.cpp server.h chatroom.h libtslog.h
	$(CXX) $(CXXFLAGS) -c server.cpp -o server.o

client.o: client.cpp client.h libtslog.h
	$(CXX) $(CXXFLAGS) -c client.cpp -o client.o

chatroom.o: chatroom.cpp chatroom.h libtslog.h
	$(CXX) $(CXXFLAGS) -c chatroom.cpp -o chatroom.o

# Executar o programa normalmente
run: app
	@echo "🚀 Executando programa..."
	./app

# Executar script de exemplo
example: app
	@echo "🚀 Rodando exemplo automatizado..."
	./run_example.sh

# Limpar arquivos compilados
clean:
	rm -f app $(OBJS)
	@echo "🧹 Arquivos limpos"

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make       - Compila o programa"
	@echo "  make run   - Compila e executa normalmente"
	@echo "  make example - Compila e executa o script automatizado"
	@echo "  make clean - Remove arquivos compilados"
	@echo "  make help  - Mostra esta ajuda"

.PHONY: run example clean help