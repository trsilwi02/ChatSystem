#!/bin/bash
# Test script for Chat System

echo "Chat System - Testing Guide"
echo "==========================="
echo ""
echo "Prerequisites:"
echo "  - Server built and ready"
echo "  - Client built and ready"
echo ""

# Function to check if port is in use
check_port() {
    if lsof -i :8888 >/dev/null 2>&1; then
        return 0  # Port in use
    else
        return 1  # Port free
    fi
}

# Get script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"

echo "Step 1: Starting Server..."
if check_port; then
    echo "ERROR: Port 8888 is already in use!"
    echo "Please stop any running servers first."
    exit 1
fi

cd "$BUILD_DIR"

# Determine build type
if [ -d "Release" ]; then
    SERVER_BIN="./Release/ChatServer"
    CLIENT_BIN="./Release/ChatClient"
elif [ -d "Debug" ]; then
    SERVER_BIN="./Debug/ChatServer"
    CLIENT_BIN="./Debug/ChatClient"
else
    SERVER_BIN="./ChatServer"
    CLIENT_BIN="./ChatClient"
fi

# Start server in background
if [ ! -f "$SERVER_BIN" ]; then
    echo "ERROR: Server executable not found at $SERVER_BIN"
    echo "Please build the project first with: ./build.sh"
    exit 1
fi

$SERVER_BIN &
SERVER_PID=$!
echo "Server started (PID: $SERVER_PID)"
echo ""

# Wait for server to start
echo "Waiting for server to initialize..."
sleep 2

echo "Step 2: Testing with client connections..."
echo ""
echo "Test Scenarios:"
echo "  1. Start client and register a new user"
echo "  2. Login with the registered credentials"
echo "  3. Start another client instance"
echo "  4. Register and login with different credentials"
echo "  5. Send messages between clients"
echo "  6. Check online users list"
echo "  7. Logout and verify offline status"
echo ""

echo "Launching clients... (open manually or script will close)"
echo "Press Ctrl+C to stop the test and clean up server"

# Wait for user input
read -p "Press Enter to stop server and cleanup..."

# Kill server
echo ""
echo "Stopping server..."
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "Test complete!"
