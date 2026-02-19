#!/usr/bin/env bash
set -e

C_RST="\033[0m"
C_RED="\033[0;31m"
C_GRN="\033[0;32m"
C_BLU="\033[0;34m"
C_PUR="\033[0;35m"
C_YLW="\033[0;33m"

CONFIG_FILE="tests/configs/simple.conf"
VALGRIND_LOG="valgrind.log"
PORT=9090

echo -e "${C_PUR}=== Starting Valgrind Memory Test ===${C_RST}"

echo -e "${C_BLU}[*] Starting webserv with Valgrind...${C_RST}"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="$VALGRIND_LOG" ./webserv "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!

echo -e "${C_BLU}[*] Waiting for server on port $PORT...${C_RST}"
SERVER_READY=false
for i in {1..50}; do
  if curl -s "http://localhost:$PORT/" > /dev/null; then
    SERVER_READY=true
    break
  fi
  sleep 0.1
done

if [ "$SERVER_READY" != true ]; then
  echo -e "${C_RED}[❌] Server failed to start or respond!${C_RST}"
  kill -9 $SERVER_PID 2>/dev/null || true
  cat "$VALGRIND_LOG"
  exit 1
fi
echo -e "${C_GRN}[✅] Server is up and running!${C_RST}"

echo -e "${C_BLU}[*] Sending SIGINT to gracefully stop server...${C_RST}"
kill -INT $SERVER_PID
wait $SERVER_PID 2>/dev/null || true
echo -e "${C_GRN}[✅] Server stopped.${C_RST}"

echo -e "${C_PUR}=== Valgrind Report ===${C_RST}"
ERRORS=$(grep "ERROR SUMMARY:" "$VALGRIND_LOG" | awk '{print $4}')

if [ "$ERRORS" == "0" ]; then
  echo -e "${C_GRN}[✅] No memory leaks or errors detected!${C_RST}"
  grep "ERROR SUMMARY:" "$VALGRIND_LOG" | awk "{ printf \"${C_GRN}%s${C_RST}\n\", \$0 }"
  exit 0
else
  echo -e "${C_RED}[❌] Memory issues found! ($ERRORS errors)${C_RST}"
  
  awk '/LEAK SUMMARY:/{flag=1} /ERROR SUMMARY:/{flag=1} flag {
      if ($0 ~ /definitely lost:/ && $4 != "0") { printf "\033[0;31m%s\033[0m\n", $0 }
      else if ($0 ~ /ERROR SUMMARY:/ && $4 != "0") { printf "\033[0;31m%s\033[0m\n", $0 }
      else { printf "\033[0;33m%s\033[0m\n", $0 }
  }' "$VALGRIND_LOG"
  
  echo -e "\n${C_RED}Check $VALGRIND_LOG for full details.${C_RST}"
  exit 1
fi

#!/usr/bin/env bash
# set -e

# CONFIG_FILE="tests/configs/simple.conf"
# VALGRIND_LOG="valgrind.log"

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file="$VALGRIND_LOG" ./webserv "$CONFIG_FILE" &

# SERVER_PID=$!
# sleep 5

# if ! curl -s "http://localhost:9090/" > /dev/null; then
#   echo "❌ Server did not respond as expected"
#   kill $SERVER_PID
#   exit 1
# fi

# kill $SERVER_PID
# wait $SERVER_PID 2>/dev/null || true

# if grep -q "definitely lost: 0 bytes" "$VALGRIND_LOG" && grep -q "ERROR SUMMARY: 0 errors" "$VALGRIND_LOG"; then
#   echo "✅ No memory leaks detected"
#   exit 0
# else
#   echo "❌ Memory leaks detected"
#   cat "$VALGRIND_LOG"
#   exit 1
# fi