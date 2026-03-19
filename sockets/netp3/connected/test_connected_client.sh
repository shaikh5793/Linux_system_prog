#!/bin/bash

# Test Connected UDP Client
# Tests connected UDP client functionality and interactive features

SERVER_IP="127.0.0.1"
PORT=8006
CLIENT_BINARY="./connected_client"
TEST_DURATION=30

echo "Connected UDP Client Test"
echo "Server: $SERVER_IP:$PORT"
echo "Test Duration: $TEST_DURATION seconds"
echo

# Check if client binary exists
if [ ! -f "$CLIENT_BINARY" ]; then
    echo "Error: $CLIENT_BINARY not found. Run 'make' first."
    exit 1
fi

# Check if server is running
echo "Checking server connection..."
timeout 2 $CLIENT_BINARY $SERVER_IP $PORT >/dev/null 2>&1 &
SERVER_CHECK_PID=$!
sleep 1

if ! kill -0 $SERVER_CHECK_PID 2>/dev/null; then
    echo "Error: Cannot connect to server at $SERVER_IP:$PORT"
    echo "Make sure the server is running: ./start_connected_server.sh"
    exit 1
fi

# Kill the test client
kill $SERVER_CHECK_PID 2>/dev/null
wait $SERVER_CHECK_PID 2>/dev/null
echo "Server is running!"
echo

# Test sequence
echo "=== Connected UDP Client Test Sequence ==="
echo

echo "Starting connected UDP client..."
echo "The client will connect to the server and run interactively"
echo "Features being tested:"
echo "  - Connected UDP socket setup"
echo "  - Simplified send()/recv() API"
echo "  - Interactive menu system"
echo "  - Error handling and reconnection"
echo
echo "Press Ctrl+C to stop early, or wait $TEST_DURATION seconds"
echo

# Start the connected client
$CLIENT_BINARY $SERVER_IP $PORT &
CLIENT_PID=$!

# Set up signal handler for clean shutdown
trap "echo 'Stopping connected client...'; kill $CLIENT_PID 2>/dev/null; wait $CLIENT_PID 2>/dev/null; echo 'Test stopped by user'; exit 0" INT

# Wait for test duration
sleep $TEST_DURATION

# Stop the client
kill $CLIENT_PID 2>/dev/null
wait $CLIENT_PID 2>/dev/null

echo
echo "=== Test Complete ==="
echo "The connected client was tested for $TEST_DURATION seconds"
echo "Connected UDP provides simplified API while maintaining UDP semantics"