#!/bin/bash

# Multi-Client UDP Event Test
# Tests multiple clients receiving events simultaneously

SERVER_IP="127.0.0.1"
PORT=8002
CLIENT_BINARY="./event_client"
NUM_CLIENTS=3
TEST_DURATION=15

echo "UDP Event Multi-Client Test"
echo "Server: $SERVER_IP:$PORT"
echo "Number of clients: $NUM_CLIENTS"
echo "Test Duration: $TEST_DURATION seconds"
echo

# Check if client binary exists
if [ ! -f "$CLIENT_BINARY" ]; then
    echo "Error: $CLIENT_BINARY not found. Run 'make' first."
    exit 1
fi

# Array to store client PIDs
declare -a CLIENT_PIDS=()

# Function to start event clients
start_clients() {
    echo "Starting $NUM_CLIENTS event clients..."
    
    for i in $(seq 1 $NUM_CLIENTS); do
        echo "Starting Event Client $i..."
        $CLIENT_BINARY $SERVER_IP $PORT &
        CLIENT_PID=$!
        CLIENT_PIDS+=($CLIENT_PID)
        echo "Client $i started with PID $CLIENT_PID"
        sleep 1
    done
    
    echo "All clients started!"
    echo
}

# Function to stop all clients
stop_clients() {
    echo "Stopping all event clients..."
    
    for pid in "${CLIENT_PIDS[@]}"; do
        if kill -0 $pid 2>/dev/null; then
            kill $pid 2>/dev/null
        fi
    done
    
    # Wait for all clients to stop
    for pid in "${CLIENT_PIDS[@]}"; do
        wait $pid 2>/dev/null
    done
    
    echo "All clients stopped"
}

# Check if server is running
echo "Checking server connection..."
timeout 2 $CLIENT_BINARY $SERVER_IP $PORT >/dev/null 2>&1 &
SERVER_CHECK_PID=$!
sleep 1

if ! kill -0 $SERVER_CHECK_PID 2>/dev/null; then
    echo "Error: Cannot connect to server at $SERVER_IP:$PORT"
    echo "Make sure the server is running: ./start_event_server.sh"
    exit 1
fi

# Kill the test client
kill $SERVER_CHECK_PID 2>/dev/null
wait $SERVER_CHECK_PID 2>/dev/null
echo "Server is running!"
echo

# Test sequence
echo "=== Multi-Client Event Test ==="
echo

# Set up signal handler for clean shutdown
trap "echo 'Stopping test...'; stop_clients; echo 'Test stopped by user'; exit 0" INT

# Start all clients
start_clients

echo "All clients are now registered and receiving events"
echo "Each client will receive the same events broadcast by the server"
echo "Press Ctrl+C to stop early, or wait $TEST_DURATION seconds"
echo

# Wait for test duration
sleep $TEST_DURATION

# Stop all clients
stop_clients

echo
echo "=== Test Complete ==="
echo "All $NUM_CLIENTS clients received events for $TEST_DURATION seconds"
echo "This demonstrates the event notification pattern where multiple"
echo "clients can receive the same events simultaneously"