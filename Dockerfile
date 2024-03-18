FROM ubuntu:23.10 AS BUILD
RUN apt update && apt install -y libstdc++6-amd64-cross libc6-amd64-cross  \
    libxmu-dev libxi-dev libgl-dev libglx-dev libgles-dev  build-essential libx11-xcb-dev libegl1-mesa-dev libopengl-dev \
     libxkbcommon-dev libwayland-dev libxrandr-dev mesa-vulkan-drivers libglu1-mesa-dev libdrm-dev libegl-dev libglm-dev  \
    curl zip unzip tar cmake  pkg-config  python3-jinja2  gcc g++ git  autoconf libtool meson && \
    apt-get clean &&  rm -rf /var/lib/apt/lists/*
WORKDIR /src
RUN git clone https://github.com/microsoft/vcpkg.git
RUN pwd
COPY vcpkg.json .
RUN ls
WORKDIR /src/vcpkg
RUN git checkout tags/2024.02.14
RUN  ./bootstrap-vcpkg.sh && ./vcpkg integrate install
RUN  ./bootstrap-vcpkg.sh && ./vcpkg install
WORKDIR /src
# TODO better docker ignore
COPY . .
WORKDIR /src/build
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/src/vcpkg/scripts/buildsystems/vcpkg.cmake ..
RUN cmake --build . --config Release -j 10
RUN find . -name "DarwinServer"




FROM ubuntu:23.10
LABEL maintainer="max.laager@gmail.com"
RUN apt update && apt install -y libstdc++6-amd64-cross libc6-amd64-cross && \
    apt-get clean &&  rm -rf /var/lib/apt/lists/*
RUN groupadd --gid 1001 Darwin
RUN useradd   --uid 1001 --gid 1001 -m Darwin
EXPOSE 45323

COPY --from=BUILD /src/build/Server/DarwinServer /usr/local/bin/DarwinServer
RUN chmod a+rx /usr/local/bin/DarwinServer
COPY ./Server/world_db.json /etc/world_db.json
RUN chmod a+rw /etc/world_db.json
USER Darwin
CMD /usr/local/bin/DarwinServer --world_db=/etc/world_db.json