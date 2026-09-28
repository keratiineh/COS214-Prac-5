FROM gcc:latest

WORKDIR /app

# Install build tools, valgrind, and gdb
RUN apt-get update && apt-get install -y --no-install-recommends \
    make \
    valgrind \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Copy source repository into container
COPY . .

# Compile application using the project Makefile
RUN make clean && make

# Run CampusGuard simulation
CMD ["./campus_guard"]
