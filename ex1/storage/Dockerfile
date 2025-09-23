FROM node:22-alpine

# Install build tools needed for native modules
RUN apk add --no-cache \
    python3 \
    make \
    g++ \
    && ln -sf python3 /usr/bin/python

WORKDIR /usr/src/storage

# Copy package.json and package-lock.json first (better for caching layers)
COPY package*.json ./

# Install only production dependencies
RUN npm ci --omit=dev

COPY . .

# Need root access to create file to docker volume
# TODO: Find better way
USER root

CMD ["sh", "-c", "chown -R node:node /data && npm run dev"]
