FROM gcc:12

# === Environment variables ===
ENV DEBIAN_FRONTEND=noninteractive
ENV AERONWRAPPER_INSTALL=/opt/aeronWrapper/install
ENV PATH=$AERONWRAPPER_INSTALL/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/aeronWrapper/source/build/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=$AERONWRAPPER_INSTALL/lib/pkgconfig:$PKG_CONFIG_PATH
ENV CMAKE_PREFIX_PATH=$AERONWRAPPER_INSTALL:$CMAKE_PREFIX_PATH
ENV JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
ENV PATH=$JAVA_HOME/bin:$PATH

# === Install dependencies ===
RUN apt-get update && apt-get install -y \
    tar git wget unzip build-essential \
    libssl-dev zlib1g-dev uuid-dev \
    nlohmann-json3-dev libjsoncpp-dev openjdk-17-jdk \
    && rm -rf /var/lib/apt/lists/*

# === Install CMake 4.1.1 ===
RUN wget https://github.com/Kitware/CMake/releases/download/v4.1.1/cmake-4.1.1-linux-x86_64.sh \
    -O /tmp/cmake.sh \
    && chmod +x /tmp/cmake.sh \
    && /tmp/cmake.sh --skip-license --prefix=/usr/local

RUN cmake --version

# === Build AeronWrapper (keep source & build files) ===
RUN git clone https://github.com/ehsanrashid/aeronWrapper.git /opt/aeronWrapper/source \
    && mkdir -p /opt/aeronWrapper/source/build \
    && cd /opt/aeronWrapper/source/build \
    && cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$AERONWRAPPER_INSTALL \
    && cmake --build . -j$(nproc) \
    && cmake --install .

# Now the aeronmd binary should be generated inside the wrapper build
ENV PATH=/opt/aeronWrapper/source/build/bin:$PATH
ENV LD_LIBRARY_PATH=/opt/aeronWrapper/source/build/lib:$LD_LIBRARY_PATH

# === Build Drogon ===
RUN git clone --recursive https://github.com/drogonframework/drogon.git /tmp/drogon \
    && mkdir -p /tmp/drogon/build \
    && cd /tmp/drogon/build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. \
    && make -j$(nproc) \
    && make install \
    && rm -rf /tmp/drogon

# === Set working directory and clone API_Gateway ===
WORKDIR /workspace
RUN git clone https://github.com/Huzaifa309/Api_gateway.git .

# === Default command: start Aeron  ===
CMD ["/bin/bash"] 


