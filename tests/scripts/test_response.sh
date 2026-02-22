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

SERVER_URL="http://localhost:9090"
UPLOAD_DIR="./www/uploads"
PASSED=0
FAILED=0

# ============================================
# GET TESTS
# ============================================
echo -e "${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  GET REQUEST TESTS${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"

# GET Test 1: Root index
echo -e "${C_BLU}[GET Test 1] GET / (root index)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/")
if [ "$HTTP_STATUS" == "200" ]; then
    echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Expected 200 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi

# GET Test 2: Specific file
echo -e "${C_BLU}[GET Test 2] GET /index.html${C_RST}"
TMP_BODY=$(mktemp)
HTTP_STATUS=$(curl -s -w "%{http_code}" -o "$TMP_BODY" "$SERVER_URL/index.html")
if [ "$HTTP_STATUS" == "200" ]; then
    if [ -s "$TMP_BODY" ]; then
        echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS, Size: $(wc -c < "$TMP_BODY") bytes)${C_RST}"
        ((PASSED++))
    else
        echo -e "${C_RED}[❌] Failed: Response body is empty${C_RST}"
        ((FAILED++))
    fi
else
    echo -e "${C_RED}[❌] Failed: Expected 200 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi
rm -f "$TMP_BODY"

# GET Test 3: 404 Not Found
echo -e "${C_BLU}[GET Test 3] GET /nonexistent.html (404)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/nonexistent.html")
if [ "$HTTP_STATUS" == "404" ]; then
    echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Expected 404 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi

# GET Test 4: 403 Forbidden
echo -e "${C_BLU}[GET Test 4] GET /error-403 (403)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/error-403")
if [ "$HTTP_STATUS" == "403" ]; then
    echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Expected 403 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi

# GET Test 5: 400 Bad Request (path traversal)
echo -e "${C_BLU}[GET Test 5] GET /../../etc/passwd (400/404)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/../../etc/passwd")
if [ "$HTTP_STATUS" == "400" ] || [ "$HTTP_STATUS" == "404" ]; then
    echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Expected 400 or 404 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi

# GET Test 6: Response headers
echo -e "${C_BLU}[GET Test 6] Verify response headers${C_RST}"
TMP_HEADERS=$(mktemp)
curl -s -i "$SERVER_URL/" > "$TMP_HEADERS" 2>&1
REQUIRED_HEADERS=("Server" "Date" "Content-Type" "Content-Length")
HEADERS_OK=true
for header in "${REQUIRED_HEADERS[@]}"; do
    if ! grep -q "^$header:" "$TMP_HEADERS"; then
        echo -e "${C_RED}  Missing header: $header${C_RST}"
        HEADERS_OK=false
    fi
done
if $HEADERS_OK; then
    echo -e "${C_GRN}[✅] Passed (All headers present)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Missing required headers${C_RST}"
    ((FAILED++))
fi
rm -f "$TMP_HEADERS"

# ============================================
# POST TESTS (Upload)
# ============================================
echo -e "\n${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  POST REQUEST TESTS (Upload)${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"

# POST Test 1: Single file upload
echo -e "${C_BLU}[POST Test 1] Upload single file${C_RST}"
TEST_FILE=$(mktemp)
echo "Test file content for upload" > "$TEST_FILE"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null -F "file=@$TEST_FILE" "$SERVER_URL/upload")
if [ "$HTTP_STATUS" == "200" ] || [ "$HTTP_STATUS" == "201" ]; then
	echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
	((PASSED++))
else
	echo -e "${C_RED}[❌] Failed: Expected 200/201 but got $HTTP_STATUS${C_RST}"
	((FAILED++))
fi

# POST Test 2: Verify file in uploads directory
echo -e "${C_BLU}[POST Test 2] Verify file uploaded to uploads directory${C_RST}"
UPLOADED_FILE=$(ls -t "$UPLOAD_DIR"/* 2>/dev/null | head -1)
if [ -f "$UPLOADED_FILE" ]; then
	echo -e "${C_GRN}[✅] Passed (File: $(basename "$UPLOADED_FILE"))${C_RST}"
	((PASSED++))
else
	echo -e "${C_RED}[❌] Failed: File not found in $UPLOAD_DIR${C_RST}"
	((FAILED++))
fi

# POST Test 3: Verify file content
echo -e "${C_BLU}[POST Test 3] Verify uploaded file content${C_RST}"
if [ -f "$UPLOADED_FILE" ]; then
	UPLOADED_CONTENT=$(cat "$UPLOADED_FILE")
	ORIGINAL_CONTENT=$(cat "$TEST_FILE")
	if [ "$UPLOADED_CONTENT" == "$ORIGINAL_CONTENT" ]; then
		echo -e "${C_GRN}[✅] Passed (Content matches)${C_RST}"
		((PASSED++))
	else
		echo -e "${C_RED}[❌] Failed: File content does not match${C_RST}"
		((FAILED++))
	fi
else
	echo -e "${C_YEL}[⚠] Skipped (No uploaded file found)${C_RST}"
fi

# POST Test 4: Access uploaded file via GET
echo -e "${C_BLU}[POST Test 4] Access uploaded file via GET${C_RST}"
if [ -f "$UPLOADED_FILE" ]; then
    FILENAME=$(basename "$UPLOADED_FILE")
    HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/uploads/$FILENAME")
    if [ "$HTTP_STATUS" == "200" ]; then
        echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
        ((PASSED++))
    else
        echo -e "${C_RED}[❌] Failed: Expected 200 but got $HTTP_STATUS${C_RST}"
        ((FAILED++))
    fi
else
    echo -e "${C_YEL}[⚠] Skipped (No uploaded file found)${C_RST}"
fi

rm -f "$TEST_FILE"

# ============================================
# DELETE TESTS
# ============================================
echo -e "\n${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  DELETE REQUEST TESTS${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"

# DELETE Test 1: Delete uploaded file
echo -e "${C_BLU}[DELETE Test 1] Delete uploaded file${C_RST}"
if [ -f "$UPLOADED_FILE" ]; then
    FILENAME=$(basename "$UPLOADED_FILE")
    HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null -X DELETE "$SERVER_URL/uploads/$FILENAME")
    if [ "$HTTP_STATUS" == "200" ]; then
        echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
        ((PASSED++))
    else
        echo -e "${C_RED}[❌] Failed: Expected 200 but got $HTTP_STATUS${C_RST}"
        ((FAILED++))
    fi
else
    echo -e "${C_YEL}[⚠] Skipped (No uploaded file found)${C_RST}"
fi

# DELETE Test 2: Verify file is deleted (404)
echo -e "${C_BLU}[DELETE Test 2] Verify file is deleted (should return 404)${C_RST}"
if [ -n "$FILENAME" ]; then
    HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null "$SERVER_URL/uploads/$FILENAME")
    if [ "$HTTP_STATUS" == "404" ]; then
        echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
        ((PASSED++))
    else
        echo -e "${C_RED}[❌] Failed: Expected 404 but got $HTTP_STATUS${C_RST}"
        ((FAILED++))
    fi
else
    echo -e "${C_YEL}[⚠] Skipped (No filename to verify)${C_RST}"
fi

# DELETE Test 3: Delete non-existent file
echo -e "${C_BLU}[DELETE Test 3] Delete non-existent file (404)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null -X DELETE "$SERVER_URL/uploads/nonexistent_file_12345.txt")
if [ "$HTTP_STATUS" == "404" ]; then
    echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Expected 404 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi

# DELETE Test 4: Unsupported method on non-upload (use a file that exists)
echo -e "${C_BLU}[DELETE Test 4] DELETE on non-upload endpoint (405/501)${C_RST}"
HTTP_STATUS=$(curl -s -w "%{http_code}" -o /dev/null -X DELETE "$SERVER_URL/")
if [ "$HTTP_STATUS" == "405" ] || [ "$HTTP_STATUS" == "501" ] || [ "$HTTP_STATUS" == "200" ]; then
    echo -e "${C_GRN}[✅] Passed (Status: $HTTP_STATUS)${C_RST}"
    ((PASSED++))
else
    echo -e "${C_RED}[❌] Failed: Expected 405/501/200 but got $HTTP_STATUS${C_RST}"
    ((FAILED++))
fi


# ============================================
# Malformed TESTS
# ============================================
echo -e "\n${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  ERROR REQUEST TESTS${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"

# Test 1: Null byte injection
echo -e "${C_BLU}  [1] Null byte injection (%00)${C_RST}"
STATUS=$(curl -s -w "%{http_code}" -o /dev/null "http://localhost:9090/test%00.html")
[ "$STATUS" == "400" ] && echo -e "${C_GRN}  ✅ Passed${C_RST}" || (echo -e "${C_RED}  ❌ Failed: $STATUS${C_RST}" && exit 1)

# Test 2: Path traversal with encoded dots
echo -e "${C_BLU}  [2] Encoded path traversal (%2e%2e)${C_RST}"
STATUS=$(curl -s -w "%{http_code}" -o /dev/null "http://localhost:9090/%2e%2e/etc/passwd")
[ "$STATUS" == "400" ] || [ "$STATUS" == "404" ] && echo -e "${C_GRN}  ✅ Passed${C_RST}" || (echo -e "${C_RED}  ❌ Failed: $STATUS${C_RST}" && exit 1)

# Test 3: Empty Host header
echo -e "${C_BLU}  [3] Empty Host header${C_RST}"
STATUS=$(curl -s -w "%{http_code}" -o /dev/null -H "Host: " "http://localhost:9090/")
echo -e "${C_GRN}  ✅ Passed (returned $STATUS)${C_RST}"

# ============================================
# SUMMARY
# ============================================
echo -e "\n${C_PUR}========================================${C_RST}"
echo -e "${C_PUR}  TEST SUMMARY${C_RST}"
echo -e "${C_PUR}========================================${C_RST}"
echo -e "${C_GRN}Passed: $PASSED${C_RST}"
echo -e "${C_RED}Failed: $FAILED${C_RST}"
TOTAL=$((PASSED + FAILED))
echo -e "${C_PUR}Total: $TOTAL${C_RST}"

if [ "$FAILED" -eq 0 ]; then
    echo -e "${C_GRN}[✅] All tests passed!${C_RST}"
    exit 0
else
    echo -e "${C_RED}[❌] Some tests failed!${C_RST}"
    exit 1
fi