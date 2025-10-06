#!/bin/bash

PORT=9091
RUN_TIME=10  # tempo total em segundos que o exemplo vai rodar

# Limpar e compilar
echo "Compilando programa..."
make clean
make

# Inicia o servidor em background
echo "Iniciando servidor na porta $PORT..."
./app server $PORT &
SERVER_PID=$!
sleep 1  # espera o servidor iniciar

# Função para simular cliente enviando mensagens
run_client() {
    CLIENT_ID=$1
    MSG=$2
    DELAY=$3

    sleep $DELAY  # espera um pouco antes de iniciar
    echo "Cliente $CLIENT_ID iniciando..."
    # Redireciona mensagens para stdin do cliente
    (echo "$MSG"; sleep 2; echo "/sair") | ./app client $CLIENT_ID 127.0.0.1 $PORT
}

# Mensagens para cada cliente
MESSAGES=("Oi do cliente 1" "Oi do cliente 2" "Oi do cliente 3" "Oi do cliente 4" "Oi do cliente 5")

# Inicia 5 clientes em background com delays diferentes
for i in ${!MESSAGES[@]}; do
    CLIENT_ID=$((i+1))
    MSG="${MESSAGES[$i]}"
    DELAY=$((i*2))  # cada cliente começa com delay diferente (0s, 2s, 4s, 6s, 8s)
    run_client $CLIENT_ID "$MSG" $DELAY &
done

echo "Todos os clientes iniciados. O script vai encerrar automaticamente em $RUN_TIME segundos."

# Espera o tempo definido
sleep $RUN_TIME

# Mata servidor e clientes
echo "Encerrando todos os processos..."
kill $SERVER_PID 2>/dev/null
kill $(jobs -p) 2>/dev/null

echo "Exemplo finalizado!"