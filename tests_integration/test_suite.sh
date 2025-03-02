#!/bin/bash

# Start the server in the background
./build/main &  
SERVER_PID=$!

# Give the server time to start
sleep 2  

FAILED_TESTS=0  # Counter for failed tests

# Unified function to run both GET and POST tests
run_test() {
    METHOD=$1
    URL=$2
    DATA=$3
    EXPECTED_CODE=$4
    EXPECTED_BODY=$5
    DESCRIPTION=$6
    TIMEOUT=3  # Set timeout in seconds

    # Set curl options based on method (GET or POST)
    if [ "$METHOD" == "POST" ]; then
        RESPONSE=$(timeout "$TIMEOUT" curl -s -w "|%{http_code}" -X POST -d "$DATA" -H "Content-Type: application/json" "$URL")
    else
        RESPONSE=$(timeout "$TIMEOUT" curl -s -w "|%{http_code}" "$URL")
    fi

    BODY=$(echo "$RESPONSE" | cut -d '|' -f 1)
    CODE=$(echo "$RESPONSE" | cut -d '|' -f 2)

    # Check response status code
    if [ -z "$CODE" ]; then
        echo "[FAIL] $DESCRIPTION - Timed out after $TIMEOUT seconds"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    elif [ "$CODE" -ne "$EXPECTED_CODE" ]; then
        echo "[FAIL] $DESCRIPTION - Expected: $EXPECTED_CODE, Got: $CODE"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    elif [ "$METHOD" == "POST" ] && [ "$BODY" != "$EXPECTED_BODY" ]; then
        echo "[FAIL] $DESCRIPTION - Expected body: '$EXPECTED_BODY', Got: '$BODY'"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    else
        echo "[PASS] $DESCRIPTION"
    fi
}

# Define GET test cases
run_test "GET" "http://localhost:8080/" "" 200 "" "Homepage should return 200"
# run_test "GET" "http://localhost:8080/unknown" "" 404 "" "Unknown route should return 404"
# run_test "GET" "http://localhost:8080/api/data" "" 200 "" "API endpoint should return 200"
# run_test "GET" "http://localhost:8080/protected" "" 401 "" "Unauthorized access should return 401"

# # Define POST test cases
run_test "POST" "http://localhost:8080/echo" 'hello' 200 "hello" "Echo should return same message"
# run_test "POST" "http://localhost:8080/api/create" '{"name":"test"}' 201 "{"id":1,"name":"test"}" "Create endpoint should return 201 with created object"

# Stop the server
kill $SERVER_PID

# Show summary and exit with error if tests failed
if [ "$FAILED_TESTS" -gt 0 ]; then
    echo "Tests failed: $FAILED_TESTS"
    exit 1
else
    echo "All tests passed!"
    exit 0
fi
