#!/usr/bin/env bash
set -e

# Цвета
C_RST="\033[0m"
C_RED="\033[0;31m"
C_GRN="\033[0;32m"
C_BLU="\033[0;34m"
C_PUR="\033[0;35m"

ENDPOINT=${1:-"/"}
EXPECTED_STATUS=${2:-200}
LOCAL_FILE=${3:-"./www/base_page/index.html"}

CONFIG_FILE="tests/configs/simple.conf"
./webserv "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!

sleep 1

SERVER_URL="http://localhost:9090${ENDPOINT}"
TMP_BODY=$(mktemp)

echo -e "${C_PUR}=== Testing Endpoint: $ENDPOINT ===${C_RST}"
echo -e "${C_BLU}[*] Expecting Status: $EXPECTED_STATUS | Comparing with: $LOCAL_FILE${C_RST}"

if [ ! -f "$LOCAL_FILE" ]; then
  echo -e "${C_RED}[❌] Error: Local file '$LOCAL_FILE' not found! Check the path.${C_RST}"
  rm -f "$TMP_BODY"
  kill -TERM $SERVER_PID
  exit 1
fi

HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" "$SERVER_URL") || true

if [[ ! "$HTTP_STATUS" =~ ^[0-9]+$ ]] || [ "$HTTP_STATUS" == "000" ]; then
  echo -e "${C_RED}[❌] Connection failed! Is the server running on $SERVER_URL?${C_RST}"
  rm -f "$TMP_BODY"
  kill -TERM $SERVER_PID
  exit 1
fi

if [ "$HTTP_STATUS" -ne "$EXPECTED_STATUS" ]; then
  echo -e "${C_RED}[❌] Failed: Expected $EXPECTED_STATUS but got $HTTP_STATUS${C_RST}"
  rm -f "$TMP_BODY"
  kill -TERM $SERVER_PID
  exit 1
fi

if ! diff -q "$LOCAL_FILE" "$TMP_BODY" > /dev/null; then
  echo -e "${C_RED}[❌] Failed: Response body does not match $LOCAL_FILE exactly!${C_RST}"
  echo -e "${C_RED}[!] Diff output:${C_RST}"
  diff "$LOCAL_FILE" "$TMP_BODY" | head -n 10
  rm -f "$TMP_BODY"
  kill -TERM $SERVER_PID
  exit 1
fi

rm -f "$TMP_BODY"
kill -TERM $SERVER_PID
echo -e "${C_GRN}[✅] Test passed! The server returned the exact file.${C_RST}"