#!/bin/bash

# Test Scripts Usage Guide
# Shows available test scripts and their usage

echo "UDP Communication Test Scripts"
echo "=============================="
echo

echo "QUEUE SERVER & CLIENT:"
echo "----------------------"
echo "1. start_queue_server.sh     - Start UDP queue server on port 8003"
echo "2. test_queue_client.sh      - Test queue operations (send/get/status)"
echo "3. queue_interactive.sh      - Interactive queue client shell"
echo "4. queue_test.sh             - Automated queue functionality tests"
echo

echo "EVENT SERVER & CLIENT:"
echo "----------------------"
echo "1. start_event_server.sh     - Start UDP event server on port 8002"
echo "2. test_event_client.sh      - Test single event client (20 seconds)"
echo "3. event_multi_client.sh     - Demo multiple clients receiving events"
echo "4. event_test.sh             - Automated event functionality tests"
echo

echo "USAGE EXAMPLES:"
echo "==============="
echo

echo "Queue Server Testing:"
echo "  Terminal 1: ./start_queue_server.sh"
echo "  Terminal 2: ./test_queue_client.sh"
echo "  Terminal 3: ./queue_interactive.sh"
echo "  Terminal 4: ./queue_test.sh"
echo

echo "Event Server Testing:"
echo "  Terminal 1: ./start_event_server.sh"
echo "  Terminal 2: ./test_event_client.sh"
echo "  Terminal 3: ./event_multi_client.sh"
echo "  Terminal 4: ./event_test.sh"
echo

echo "QUICK START:"
echo "============"
echo "1. Compile programs: make"
echo "2. Start server: ./start_queue_server.sh or ./start_event_server.sh"
echo "3. Run tests: ./queue_test.sh or ./event_test.sh"
echo "4. Try tests: ./test_queue_client.sh or ./test_event_client.sh"
echo

echo "NOTES:"
echo "======"
echo "- Queue server uses port 8003"
echo "- Event server uses port 8002"
echo "- All scripts check for binary existence before running"
echo "- Use Ctrl+C to stop servers and interactive clients"
echo "- Test scripts provide automated pass/fail results"
