#!/bin/bash

# Start UDP Discovery Server
# Simple script to start the discovery server on port 8001

PORT=8001
SERVER_BINARY="./discovery_server"
PIDFILE="/tmp/discovery_server_${PORT}.pid"

echo "Starting UDP Discovery Server..."
echo "Port: $PORT"
echo "Commands: register, unregister"
echo "Server maintains client registry"
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
        echo "Use 'pkill -f discovery_server' to stop it"
        exit 1
    else
        rm -f "$PIDFILE"
    fi
fi

# Kill any existing server on this port
pkill -f "discovery_server $PORT" 2>/dev/null
sleep 1

# Start the server
echo "Starting server on port $PORT..."
$SERVER_BINARY $PORT &
SERVER_PID=$!

# Save PID
echo $SERVER_PID > "$PIDFILE"

echo "Server started with PID $SERVER_PID"
echo "Server listening on port $PORT"
echo "Accepting client registrations"
echo "Press Ctrl+C to stop"

# Wait for server and clean up on exit
trap "echo 'Stopping server...'; kill $SERVER_PID 2>/dev/null; rm -f '$PIDFILE'; exit 0" INT TERM

wait $SERVER_PID
rm -f "$PIDFILE"