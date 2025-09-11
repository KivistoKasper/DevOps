# DevOps

COMP.SE.140-2025-2026-1 Continuous Development and Deployment - DevOps

This is a course work repository.

## Running in development

### To compose up, use:

docker-compose -f docker-compose.dev.yml up

### And to shut down, use:

docker-compose -f docker-compose.dev.yml down

### Check for running containers:

docker ps -a

### Prune all

docker-compose down --remove-orphans
docker system prune -af --volumes
