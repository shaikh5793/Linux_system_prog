#!/bin/bash

# Connected UDP Test Script
# Automated tests for connected UDP functionality

SERVER_IP="127.0.0.1"
PORT=8006
CLIENT_BINARY="./connected_client"
SERVER_BINARY="./connected_server"
TEST_DURATION=5

echo "Connected UDP Automated Test"
echo "============================"

# Check if binaries exist
if [ ! -f "$CLIENT_BINARY" ]; then
    echo "Error: $CLIENT_BINARY not found. Run 'make' first."
    exit 1
fi

if [ ! -f "$SERVER_BINARY" ]; then
    echo "Error: $SERVER_BINARY not found. Run 'make' first."
    exit 1
fi

# Function to run test and check result
run_test() {
    local desc="$1"
    local expected_pattern="$2"
    
    echo -n "Test: $desc ... "
    
    # Create temporary file for output
    local temp_file=$(mktemp)
    
    # Run client with timeout and capture output
    timeout $TEST_DURATION $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
    local exit_code=$?
    
    # Read the output
    local output=$(cat "$temp_file")
    rm -f "$temp_file"
    
    # Check if client ran successfully (timeout should cause exit code 124)
    if [ $exit_code -eq 124 ] || [ $exit_code -eq 0 ]; then
        if [ -n "$expected_pattern" ] && [[ "$output" == *"$expected_pattern"* ]]; then
            echo "PASS"
            return 0
        elif [ -z "$expected_pattern" ]; then
            echo "PASS"
            return 0
        else
            echo "FAIL (expected: $expected_pattern)"
            echo "Output: $output"
            return 1
        fi
    else
        echo "FAIL (connection error)"
        echo "Output: $output"
        return 1
    fi
}

# Start server for testing
echo "Starting test server..."
$SERVER_BINARY $PORT >/dev/null 2>&1 &
SERVER_PID=$!
sleep 2

# Check if server started
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "Error: Failed to start server"
    exit 1
fi

echo "Server started with PID $SERVER_PID"
echo

# Run tests
TESTS_PASSED=0
TESTS_TOTAL=0

# Test 1: Client connection
((TESTS_TOTAL++))
echo -n "Test: Client connection to server ... "
temp_file=$(mktemp)
timeout 3 $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
output=$(cat "$temp_file")
rm -f "$temp_file"

if [[ "$output" == *"Connected to server"* ]] || [[ "$output" == *"Connected UDP Client"* ]]; then
    echo "PASS"
    ((TESTS_PASSED++))
else
    echo "FAIL"
    echo "Output: $output"
fi

# Test 2: Socket connection setup
((TESTS_TOTAL++))
echo -n "Test: Connected UDP socket setup ... "
temp_file=$(mktemp)
timeout 3 $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
output=$(cat "$temp_file")
rm -f "$temp_file"

if [[ "$output" == *"connect()"* ]] || [[ "$output" == *"Connected"* ]] || [ $? -eq 124 ]; then
    echo "PASS"
    ((TESTS_PASSED++))
else
    echo "FAIL"
    echo "Output: $output"
fi

# Test 3: Interactive menu availability
((TESTS_TOTAL++))
echo -n "Test: Interactive menu system ... "
temp_file=$(mktemp)
timeout 2 $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
output=$(cat "$temp_file")
rm -f "$temp_file"

if [[ "$output" == *"1."* ]] && [[ "$output" == *"2."* ]]; then
    echo "PASS"
    ((TESTS_PASSED++))
else
    echo "FAIL (no menu found)"
    echo "Output: $output"
fi

# Stop the server
echo
echo "Stopping test server..."
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo
echo "Test Results:"
echo "============="
echo "Tests passed: $TESTS_PASSED/$TESTS_TOTAL"

if [ $TESTS_PASSED -eq $TESTS_TOTAL ]; then
    echo "All tests PASSED! ✓"
    exit 0
else
    echo "Some tests FAILED! ✗"
    exit 1
fi