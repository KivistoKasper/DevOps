# DevOps

COMP.SE.140-2025-2026-1 Continuous Development and Deployment - DevOps

This is a course work repository.

## Running in development

## Service 1

npm install

npm run dev

## Service 2

To run the service 2 in separetly:

Compile:
g++ main.cpp -o main

And run:
./main

The service 2 uses port 9191.

## With Docker

### To compose up, use:

docker-compose -f docker-compose.dev.yml up

### And to shut down, use:

docker-compose -f docker-compose.dev.yml down

### Check for running containers:

docker ps -a

### Prune all

docker-compose -f docker-compose.dev.yml down --remove-orphans
docker system prune -af --volumes
