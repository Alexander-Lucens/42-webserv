#!/usr/bin/env bash
set -e

C_RST="\033[0m"
C_RED="\033[0;31m"
C_GRN="\033[0;32m"
C_BLU="\033[0;34m"
C_PUR="\033[0;35m"

SERVER_URL="http://localhost:9090/"
EXPECTED_STATUS=200
EXPECTED_BODY="Welcome to webserv"
TMP_BODY=$(mktemp)

echo -e "${C_PUR}=== Starting Endpoint Test ===${C_RST}"
echo -e "${C_BLU}[*] Testing GET ${SERVER_URL}...${C_RST}"

HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" "$SERVER_URL" || echo "FAIL")

if [ "$HTTP_STATUS" == "FAIL" ] || [ "$HTTP_STATUS" == "000" ]; then
  echo -e "${C_RED}[❌] Connection failed! Is the server running on $SERVER_URL?${C_RST}"
  rm -f "$TMP_BODY"
  exit 1
fi

echo -e "${C_BLU}[*] Received HTTP Status: $HTTP_STATUS${C_RST}"

if [ "$HTTP_STATUS" -ne "$EXPECTED_STATUS" ]; then
  echo -e "${C_RED}[❌] Expected HTTP status $EXPECTED_STATUS but got $HTTP_STATUS${C_RST}"
  rm -f "$TMP_BODY"
  exit 1
fi

if ! grep -q "$EXPECTED_BODY" "$TMP_BODY"; then
  echo -e "${C_RED}[❌] Expected body to contain '$EXPECTED_BODY' but it didn't.${C_RST}"
  echo -e "${C_RED}[!] Received body snippet (first 5 lines):${C_RST}"
  head -n 5 "$TMP_BODY"
  rm -f "$TMP_BODY"
  exit 1
fi

rm -f "$TMP_BODY"
echo -e "${C_GRN}[✅] Response test passed! Status and Body match.${C_RST}"