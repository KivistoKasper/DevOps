# smaller base image for prod
FROM node:22-alpine

# Install build tools needed for native modules
RUN apk add --no-cache \
    python3 \
    make \
    g++ \
    && ln -sf python3 /usr/bin/python
    
# Set working directory
WORKDIR /usr/src/service1

# Copy only the package files first for layer caching
COPY package*.json ./

# Install only production dependencies
RUN npm ci --omit=dev

# Copy the rest of the application
COPY . .

# Ensure the app runs as a non-root user
USER node

# Start the app
CMD ["npm", "start"]
