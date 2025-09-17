FROM node:20

WORKDIR /usr/src/storage

# Copy package.json and package-lock.json first (better for caching layers)
COPY package*.json ./

# install dependencies, omitting the development dependencies
RUN npm install

COPY . .

# Need root access to create file to docker volume
# TODO: Find better way
USER root

CMD ["sh", "-c", "chown -R node:node /data && npm run dev"]
