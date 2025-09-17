FROM node

WORKDIR /usr/src/storage

# Copy package.json and package-lock.json first (better for caching layers)
COPY package*.json ./

# install dependencies, omitting the development dependencies
RUN npm install

COPY . .

USER node

CMD ["npm", "run", "dev"]