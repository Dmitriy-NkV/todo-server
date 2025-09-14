FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    software-properties-common \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update && apt-get install -y \
    g++-13 \
    cmake \
    make \
    git \
    libboost-all-dev \
    libpq-dev \
    postgresql-server-dev-all \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100

WORKDIR /app

COPY . .

RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_CXX_STANDARD=23 .. && \
    make -j$(nproc)

CMD ["./build/server"]