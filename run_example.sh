#!/bin/bash

PORT=8080
RUN_TIME=30  # tempo total em segundos que o exemplo vai rodar
MESSAGES=("Oi do cliente 1" "Oi do cliente 2" "Oi do cliente 3")

# Limpar e compilar
echo "Compilando programa..."
make clean
make

# Inicia o servidor em background
echo "Iniciando servidor na porta $PORT..."
./app server $PORT &
SERVER_PID=$!
sleep 1  # espera o servidor iniciar

# Inicia os clientes em background
for i in ${!MESSAGES[@]}; do
    CLIENT_ID=$((i+1))
    MSG="${MESSAGES[$i]}"
    ./app client $CLIENT_ID "$MSG" 127.0.0.1 $PORT &
done

echo "Todos os clientes iniciados. O script vai encerrar automaticamente em $RUN_TIME segundos."

# Espera o tempo definido
sleep $RUN_TIME

# Mata servidor e clientes
echo "Encerrando todos os processos..."
kill $SERVER_PID 2>/dev/null
kill $(jobs -p) 2>/dev/null

echo "Exemplo finalizado!"