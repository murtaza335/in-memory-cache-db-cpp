# ---------- Debug / Valgrind stage ----------
FROM ubuntu:22.04

# Install build tools and valgrind
RUN apt-get update && \
    apt-get install -y cmake g++ make valgrind && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy everything
COPY CMakeLists.txt . 
COPY include ./include
COPY src ./src

# Build debug version (for Valgrind)
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
    && cmake --build build

EXPOSE 6379

# Start bash so you can run Valgrind manually
CMD ["bash"]
