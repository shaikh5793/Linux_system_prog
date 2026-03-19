#!/bin/bash

# Start Connected UDP Server
# Simple script to start the connected UDP server on port 8006

PORT=8006
SERVER_BINARY="./connected_server"
PIDFILE="/tmp/connected_server_${PORT}.pid"

echo "Starting Connected UDP Server..."
echo "Port: $PORT"
echo "Features: Standard UDP server compatible with connected clients"
echo "Server tracks client statistics and provides echo service"
echo

# Check if server binary exists
if [ ! -f "$SERVER_BINARY" ]; then
    echo "Error: $SERVER_BINARY not found. Run 'make' first."
    exit 1
fi

# Check if server is already running
if [ -f "$PIDFILE" ]; then
    PID=$(cat "$PIDFILE")
    if kill -0 "$PID" 2>/dev/null; then
        echo "Server already running with PID $PID"
        echo "Use 'pkill -f connected_server' to stop it"
        exit 1
    else
        rm -f "$PIDFILE"
    fi
fi

# Kill any existing server on this port
pkill -f "connected_server $PORT" 2>/dev/null
sleep 1

# Start the server
echo "Starting server on port $PORT..."
$SERVER_BINARY $PORT &
SERVER_PID=$!

# Save PID
echo $SERVER_PID > "$PIDFILE"

echo "Server started with PID $SERVER_PID"
echo "Server listening on port $PORT"
echo "Accepting both connected and regular UDP clients"
echo "Press Ctrl+C to stop"

# Wait for server and clean up on exit
trap "echo 'Stopping server...'; kill $SERVER_PID 2>/dev/null; rm -f '$PIDFILE'; exit 0" INT TERM

wait $SERVER_PID
rm -f "$PIDFILE"