#!/bin/bash

# Test UDP Discovery Client
# Tests discovery client registration and communication

SERVER_IP="127.0.0.1"
PORT=8001
CLIENT_BINARY="./discovery_client"
TEST_DURATION=15

echo "UDP Discovery Client Test"
echo "Server: $SERVER_IP:$PORT"
echo "Test Duration: $TEST_DURATION seconds"
echo

# Check if client binary exists
if [ ! -f "$CLIENT_BINARY" ]; then
    echo "Error: $CLIENT_BINARY not found. Run 'make' first."
    exit 1
fi

# Function to start discovery client with timeout
start_discovery_client() {
    local client_name="$1"
    local duration="$2"
    
    echo "Starting $client_name for $duration seconds..."
    timeout $duration $CLIENT_BINARY $SERVER_IP $PORT &
    local client_pid=$!
    echo "$client_name started with PID $client_pid"
    return $client_pid
}

# Test sequence
echo "=== Discovery Client Test Sequence ==="
echo

# Check if server is running
echo "Checking server connection..."
timeout 2 $CLIENT_BINARY $SERVER_IP $PORT >/dev/null 2>&1 &
SERVER_CHECK_PID=$!
sleep 1

if ! kill -0 $SERVER_CHECK_PID 2>/dev/null; then
    echo "Error: Cannot connect to server at $SERVER_IP:$PORT"
    echo "Make sure the server is running: ./start_discovery_server.sh"
    exit 1
fi

# Kill the test client
kill $SERVER_CHECK_PID 2>/dev/null
wait $SERVER_CHECK_PID 2>/dev/null
echo "Server is running!"
echo

echo "Starting discovery client to register with server..."
echo "The client will register with the server and maintain connection"
echo "Press Ctrl+C to stop early, or wait $TEST_DURATION seconds"
echo

# Start the discovery client
$CLIENT_BINARY $SERVER_IP $PORT &
CLIENT_PID=$!

# Set up signal handler for clean shutdown
trap "echo 'Stopping discovery client...'; kill $CLIENT_PID 2>/dev/null; wait $CLIENT_PID 2>/dev/null; echo 'Test stopped by user'; exit 0" INT

# Wait for test duration
sleep $TEST_DURATION

# Stop the client
kill $CLIENT_PID 2>/dev/null
wait $CLIENT_PID 2>/dev/null

echo
echo "=== Test Complete ==="
echo "The client was registered with the server for $TEST_DURATION seconds"