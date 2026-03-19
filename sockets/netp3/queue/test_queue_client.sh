#!/bin/bash

# Test UDP Queue Client Operations
# Tests various queue operations with the server

SERVER_IP="127.0.0.1"
PORT=8003
CLIENT_BINARY="./queue_client"

echo "UDP Queue Client Test"
echo "Server: $SERVER_IP:$PORT"
echo

# Check if client binary exists
if [ ! -f "$CLIENT_BINARY" ]; then
    echo "Error: $CLIENT_BINARY not found. Run 'make' first."
    exit 1
fi

# Function to send command and show result
send_command() {
    local operation="$1"
    local message="$2"
    local desc="$3"
    
    echo "[$desc]"
    if [ -n "$message" ]; then
        echo "Command: $CLIENT_BINARY $SERVER_IP $PORT $operation \"$message\""
        $CLIENT_BINARY $SERVER_IP $PORT $operation "$message"
    else
        echo "Command: $CLIENT_BINARY $SERVER_IP $PORT $operation"
        $CLIENT_BINARY $SERVER_IP $PORT $operation
    fi
    echo
    sleep 1
}

# Test sequence
echo "=== Queue Test Sequence ==="
echo

send_command "status" "" "Check initial queue status"

send_command "send" "Hello World" "Send first message"

send_command "send" "UDP Queue Test" "Send second message"

send_command "send" "Third message" "Send third message"

send_command "status" "" "Check queue status after sending"

send_command "get" "" "Get first message"

send_command "get" "" "Get second message"

send_command "status" "" "Check queue status after getting messages"

send_command "get" "" "Get third message"

send_command "get" "" "Try to get from empty queue"

send_command "status" "" "Check final queue status"

echo "=== Test Complete ==="
