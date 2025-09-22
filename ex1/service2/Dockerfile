# Stage 1: Build stage
FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get install -y build-essential

WORKDIR /usr/src/service2

COPY . .

RUN g++ main.cpp -o main -static

# Stage 2: Runtime stage
FROM scratch

COPY --from=build /usr/src/service2/main /main

CMD ["/main"]   