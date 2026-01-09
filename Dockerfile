# ---------- Build stage ----------
FROM ubuntu:22.04 AS builder

RUN apt-get update && \
    apt-get install -y cmake g++ make && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything your CMake expects
COPY CMakeLists.txt .
COPY include ./include
COPY src ./src

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build

# ---------- Runtime stage ----------
FROM ubuntu:22.04

WORKDIR /app

COPY --from=builder /app/build/main .

EXPOSE 6379

CMD ["./main"]
