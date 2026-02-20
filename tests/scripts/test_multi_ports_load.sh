#!/usr/bin/env bash
set -e

C_RST="\033[0m"
C_RED="\033[0;31m"
C_GRN="\033[0;32m"
C_BLU="\033[0;34m"
C_PUR="\033[0;35m"
C_YLW="\033[0;33m"

SERVER_URL_TEMPLATE="http://127.0.0.1:808"
CONCURRENCY=10
DURATION="3S"
MIN_SUCCESS_RATE=95

CONFIG_FILE="tests/configs/multi_port.conf"

./webserv "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!

TOTAL_AVAILABILITY=0
PORT_COUNT=0

echo -e "${C_PUR}===== Multi-Port Load Test Started =====${C_RST}"
echo -e "${C_YLW}Concurrency:${C_RST} $CONCURRENCY"
echo -e "${C_YLW}Duration:${C_RST} $DURATION"
echo

for i in {0..9}; do
  PORT="808$i"
  URL="${SERVER_URL_TEMPLATE}${i}/"

  echo -e "${C_BLU}Testing port ${PORT}...${C_RST}"

  RESULT=$(siege -c "$CONCURRENCY" -t "$DURATION" "$URL" --no-parser 2>&1)

  SIEGE_REPORT=$(echo "$RESULT" | awk '/\{/{flag=1; next} /\}/{flag=0} flag')
  AVAILABILITY=$(echo "$SIEGE_REPORT" | awk -F: '/"availability":/ {print $2}' | tr -d ' ,')

  if [[ -z "$AVAILABILITY" ]]; then
    echo -e "${C_RED}❌ Failed to parse availability for port ${PORT}${C_RST}"
    kill -TERM "$SERVER_PID"
    exit 1
  fi

  TOTAL_AVAILABILITY=$(awk "BEGIN {print $TOTAL_AVAILABILITY + $AVAILABILITY}")
  PORT_COUNT=$((PORT_COUNT + 1))

  IS_ZERO=$(awk "BEGIN {print $AVAILABILITY == 0 ? 1 : 0}")
  IS_LOW=$(awk "BEGIN {print $AVAILABILITY < $MIN_SUCCESS_RATE ? 1 : 0}")

  if [[ "$IS_ZERO" -eq 1 ]]; then
    echo -e "${C_RED}❌ Port ${PORT} failed availability: ${AVAILABILITY}%${C_RST}"
    kill -TERM "$SERVER_PID"
    exit 1
  fi

  if [[ "$IS_LOW" -eq 1 ]]; then
    echo -e "${C_RED}❌ Port ${PORT} below threshold (${AVAILABILITY}% < ${MIN_SUCCESS_RATE}%)${C_RST}"
    kill -TERM "$SERVER_PID"
    exit 1
  fi

  echo -e "${C_GRN}✅ Port ${PORT} passed — availability: ${AVAILABILITY}%${C_RST}"
  sleep 6
  echo
done

kill -TERM "$SERVER_PID"

AVERAGE=$(awk "BEGIN {print $TOTAL_AVAILABILITY / $PORT_COUNT}")

IS_FAIL=$(awk "BEGIN {print $AVERAGE < $MIN_SUCCESS_RATE ? 1 : 0}")

echo -e "${C_PUR}===== Summary =====${C_RST}"
echo -e "Average availability: ${C_GRN}${AVERAGE}%${C_RST}"

if [[ "$IS_FAIL" -eq 1 ]]; then
  echo -e "${C_RED}❌ Overall availability below ${MIN_SUCCESS_RATE}%${C_RST}"
  exit 1
fi

echo -e "${C_GRN}ALL PORTS PASSED UNDER LOAD${C_RST}"
