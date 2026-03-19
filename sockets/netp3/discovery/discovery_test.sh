#!/bin/bash

# Simple UDP Discovery Test Script
# Quick test to verify discovery functionality

SERVER_IP="127.0.0.1"
PORT=8001
CLIENT_BINARY="./discovery_client"
TEST_DURATION=5

echo "UDP Discovery Basic Test"
echo "======================="

# Check if client binary exists
if [ ! -f "$CLIENT_BINARY" ]; then
    echo "Error: $CLIENT_BINARY not found. Run 'make' first."
    exit 1
fi

# Function to run test and check result
run_test() {
    local desc="$1"
    local duration="$2"
    local expected_pattern="$3"
    
    echo -n "Test: $desc ... "
    
    # Create temporary file for output
    local temp_file=$(mktemp)
    
    # Run client with timeout and capture output
    timeout $duration $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
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

# Check server connection
echo "Checking server connection..."
timeout 2 $CLIENT_BINARY $SERVER_IP $PORT >/dev/null 2>&1 &
SERVER_CHECK_PID=$!
sleep 1

if ! kill -0 $SERVER_CHECK_PID 2>/dev/null; then
    echo "Error: Cannot connect to server at $SERVER_IP:$PORT"
    echo "Start server first: ./start_discovery_server.sh"
    exit 1
fi

# Kill the test client
kill $SERVER_CHECK_PID 2>/dev/null
wait $SERVER_CHECK_PID 2>/dev/null
echo "Server is running!"
echo

# Run tests
TESTS_PASSED=0
TESTS_TOTAL=0

# Test 1: Client connection and registration
((TESTS_TOTAL++))
echo -n "Test: Client connection and registration ... "
temp_file=$(mktemp)
timeout 3 $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
output=$(cat "$temp_file")
rm -f "$temp_file"

if [[ "$output" == *"Discovery client started"* ]] && [[ "$output" == *"Registration sent"* ]]; then
    echo "PASS"
    ((TESTS_PASSED++))
else
    echo "FAIL"
    echo "Output: $output"
fi

# Test 2: Communication with server
((TESTS_TOTAL++))
echo -n "Test: Communication with server ... "
temp_file=$(mktemp)
timeout $TEST_DURATION $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1
output=$(cat "$temp_file")
rm -f "$temp_file"

if [[ "$output" == *"Connected to server"* ]] || [[ "$output" == *"ACK"* ]]; then
    echo "PASS"
    ((TESTS_PASSED++))
else
    echo "FAIL (no server response)"
    echo "Output: $output"
fi

# Test 3: Clean shutdown
((TESTS_TOTAL++))
echo -n "Test: Clean shutdown ... "
temp_file=$(mktemp)
timeout 3 $CLIENT_BINARY $SERVER_IP $PORT > "$temp_file" 2>&1 &
client_pid=$!
sleep 1
kill -INT $client_pid 2>/dev/null
wait $client_pid 2>/dev/null
output=$(cat "$temp_file")
rm -f "$temp_file"

if [[ "$output" == *"Unregistered from server"* ]] || [[ "$output" == *"Shutting down"* ]]; then
    echo "PASS"
    ((TESTS_PASSED++))
else
    echo "FAIL"
    echo "Output: $output"
fi

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