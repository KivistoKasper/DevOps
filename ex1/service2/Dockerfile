# Stage 1: Build stage
FROM alpine:3.22 AS build

#RUN apt-get update && apt-get install -y build-essential
RUN apk add --no-cache g++ musl-dev

WORKDIR /usr/src/service2

COPY . .

RUN g++ main.cpp -o main -static

# Stage 2: Runtime stage
FROM scratch

COPY --from=build /usr/src/service2/main /main

CMD ["/main"]   