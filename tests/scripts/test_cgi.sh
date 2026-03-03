#!/usr/bin/env bash
# set -e

# Colors
C_RST="\033[0m"
C_RED="\033[0;31m"
C_GRN="\033[0;32m"
C_BLU="\033[0;34m"
C_PUR="\033[0;35m"
C_YEL="\033[0;33m"

CONFIG_FILE="tests/configs/simple.conf"
./webserv "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1

cleanup() {
    kill -TERM $SERVER_PID 2>/dev/null || true
}
trap cleanup EXIT

SERVER_URL="http://localhost:8080"
PASSED=0
FAILED=0

pass() { echo -e "${C_GRN}[✅] Passed: $1${C_RST}"; ((PASSED++)); }
fail() { echo -e "${C_RED}[❌] Failed: $1${C_RST}"; ((FAILED++)); }

# ============================================
# CGI TESTS
# ============================================
echo -e "${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  CGI TESTS${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"

# Test 1: Rust GET with name argument
echo -e "${C_BLU}[CGI Test 1] GET /cgi-bin/rust_program?name=Alice (Rust GET)${C_RST}"
TMP_BODY=$(mktemp)
HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" "$SERVER_URL/cgi-bin/rust_program?name=Alice")
if [ "$HTTP_STATUS" == "200" ] && grep -qi "Hello Alice" "$TMP_BODY"; then
    pass "Status: $HTTP_STATUS, got 'Hello Alice' in body"
else
    fail "Expected 200 with 'Hello Alice', got $HTTP_STATUS. Body: '$(cat "$TMP_BODY")'"
fi
rm -f "$TMP_BODY"

# Test 2: Rust POST with name argument
echo -e "${C_BLU}[CGI Test 2] POST /cgi-bin/rust_program?name=Alice (Rust POST)${C_RST}"
TMP_BODY=$(mktemp)
HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" \
    -X POST \
    -H "Content-Type: application/x-www-form-urlencoded" \
    -d "data=something" \
    "$SERVER_URL/cgi-bin/rust_program?name=Alice")
if [ "$HTTP_STATUS" == "200" ] && grep -qi "Hello Alice" "$TMP_BODY"; then
    pass "Status: $HTTP_STATUS, got 'Hello Alice' in body"
else
    fail "Expected 200 with 'Hello Alice', got $HTTP_STATUS. Body: '$(cat "$TMP_BODY")'"
fi
rm -f "$TMP_BODY"

# Test 3: Python GET with name argument
echo -e "${C_BLU}[CGI Test 3] GET /cgi-bin/script.py?name=Alice (Python GET)${C_RST}"
TMP_BODY=$(mktemp)
HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" "$SERVER_URL/cgi-bin/script.py?name=Alice")
if [ "$HTTP_STATUS" == "200" ] && grep -qi "Hello Alice" "$TMP_BODY"; then
    pass "Status: $HTTP_STATUS, got 'Hello Alice' in body"
else
    fail "Expected 200 with 'Hello Alice', got $HTTP_STATUS. Body: '$(cat "$TMP_BODY")'"
fi
rm -f "$TMP_BODY"

# Test 4: Python POST with name argument
echo -e "${C_BLU}[CGI Test 4] POST /cgi-bin/script.py?name=Alice (Python POST)${C_RST}"
TMP_BODY=$(mktemp)
HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" \
    -X POST \
    -H "Content-Type: application/x-www-form-urlencoded" \
    -d "data=something" \
    "$SERVER_URL/cgi-bin/script.py?name=Alice")
if [ "$HTTP_STATUS" == "200" ] && grep -qi "Hello Alice" "$TMP_BODY"; then
    pass "Status: $HTTP_STATUS, got 'Hello Alice' in body"
else
    fail "Expected 200 with 'Hello Alice', got $HTTP_STATUS. Body: '$(cat "$TMP_BODY")'"
fi
rm -f "$TMP_BODY"

# Test 5: Non-existent CGI script (graceful failure)
echo -e "${C_BLU}[CGI Test 5] GET /cgi-bin/nonexistent.py (file not found)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/cgi-bin/nonexistent.py")
if [ "$HTTP_STATUS" == "404" ] || [ "$HTTP_STATUS" == "500" ]; then
    pass "Status: $HTTP_STATUS, non-existent script handled gracefully"
else
    fail "Expected 404 or 500, got $HTTP_STATUS"
fi

# ============================================
# SUMMARY
# ============================================
echo -e ""
echo -e "${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  SUMMARY${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"
TOTAL=$((PASSED + FAILED))
echo -e "${C_YEL}Total:  $TOTAL${C_RST}"
echo -e "${C_GRN}Passed: $PASSED${C_RST}"
echo -e "${C_RED}Failed: $FAILED${C_RST}"

if [ "$FAILED" -eq 0 ]; then
    echo -e "${C_GRN}All CGI tests passed! 🎉${C_RST}"
else
    echo -e "${C_RED}$FAILED test(s) failed.${C_RST}"
    exit 1
fi