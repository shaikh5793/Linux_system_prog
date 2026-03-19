#!/bin/bash

# Start UDP Event Server
# Simple script to start the event server on port 8002

PORT=8002
SERVER_BINARY="./event_server"
PIDFILE="/tmp/event_server_${PORT}.pid"

echo "Starting UDP Event Server..."
echo "Port: $PORT"
echo "Commands: register, unregister"
echo "Server generates periodic test events"
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
        echo "Use 'pkill -f event_server' to stop it"
        exit 1
    else
        rm -f "$PIDFILE"
    fi
fi

# Kill any existing server on this port
pkill -f "event_server $PORT" 2>/dev/null
sleep 1

# Start the server
echo "Starting server on port $PORT..."
$SERVER_BINARY $PORT &
SERVER_PID=$!

# Save PID
echo $SERVER_PID > "$PIDFILE"

echo "Server started with PID $SERVER_PID"
echo "Server listening on port $PORT"
echo "Generating test events every 5 seconds"
echo "Press Ctrl+C to stop"

# Wait for server and clean up on exit
trap "echo 'Stopping server...'; kill $SERVER_PID 2>/dev/null; rm -f '$PIDFILE'; exit 0" INT TERM

wait $SERVER_PID
rm -f "$PIDFILE"