#!/usr/bin/env bash
set -e

SVC_NAME=webserv

echo "🔄 Building and starting WebServer environment..."
rm -rf logs
docker compose up -d --build

echo "✅ Container is running!"

docker compose exec ${SVC_NAME} bash

echo "🛑 Stopping container..."
rm -rf logs
make fclean
docker compose down
