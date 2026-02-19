#!/usr/bin/env bash
set -e

SERVER_URL="http://localhost:9090/"
CONCURRENCY=10
DURATION="30S"
MIN_SUCCESS_RATE=95

CONFIG_FILE="tests/configs/simple.conf"
./webserv "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!

sleep 1

RESULT=$(siege -c "$CONCURRENCY" -t "$DURATION" "$SERVER_URL" --no-parser 2>&1)

SIEGE_REPORT=$(echo "$RESULT" | awk '/\{/{flag=1; next} /\}/{flag=0} flag')

echo -e "\033[0;35m  ===== Siege Report =====  \033[0m"
echo "$SIEGE_REPORT" | awk -F':' '{ 
    gsub(/^[ \t]+/, "", $1); 
    gsub(/^[ \t]+/, " ", $2); 
    printf "\033[0;34m%s\033[0m:\033[0;32m%s\033[0m\n", $1, $2 
}'

TRANSACTIONS=$(echo "$SIEGE_REPORT" | awk -F: '/"transactions":/ {print $2}' | tr -d ' ,')
SUCCESS_RATE=$(echo "$SIEGE_REPORT" | awk -F: '/"availability":/ {print $2}' | tr -d ' ,')
FAILED_RATE=$(echo "$SIEGE_REPORT" | awk -F: '/"failed_transactions":/ {print $2}' | tr -d ' ,')


if [[ -z "$TRANSACTIONS" || "$TRANSACTIONS" -eq 0 ]]; then
  echo "❌ Transaction count is zero. Load test did not execute properly."
  kill -TERM $SERVER_PID
  exit 1
fi

IS_LOW_SUCCESS=$(awk "BEGIN {print ($SUCCESS_RATE < $MIN_SUCCESS_RATE) ? 1 : 0}")
if [[ "$IS_LOW_SUCCESS" -eq 1 ]]; then
  echo "❌ Success rate is below $MIN_SUCCESS_RATE%. Actual: $SUCCESS_RATE%"
  kill -TERM $SERVER_PID
  exit 1
fi

IS_HIGH_FAIL=$(awk "BEGIN {print (($FAILED_RATE / $TRANSACTIONS) * 100 >= 5) ? 1 : 0}")
if [[ "$IS_HIGH_FAIL" -eq 1 ]]; then
  echo "❌ Failed rate is too high. Total transactions: $TRANSACTIONS, Failed: $FAILED_RATE"
  kill -TERM $SERVER_PID
  exit 1
fi

kill -TERM $SERVER_PID
echo "✅ Load test passed with success rate: ${SUCCESS_RATE}%"