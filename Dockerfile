FROM ubuntu:23.10 AS BUILD
RUN rm /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' >/etc/apt/apt.conf.d/keep-cache
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt update && apt install --yes libstdc++6-amd64-cross libc6-amd64-cross  \
    libxmu-dev libxi-dev libgl-dev libglx-dev libgles-dev  build-essential libx11-xcb-dev libegl1-mesa-dev libopengl-dev \
     libxkbcommon-dev libwayland-dev libxrandr-dev mesa-vulkan-drivers libglu1-mesa-dev libdrm-dev libegl-dev libglm-dev  \
    curl zip unzip tar cmake  pkg-config  python3-jinja2  gcc g++ git autoconf libtool meson wget libpulse-dev 
WORKDIR /src/
RUN git clone  --depth=1 --branch=master  https://github.com/microsoft/vcpkg.git
COPY vcpkg.json .
WORKDIR /src/vcpkg
RUN mkdir /src/vcpkg/downloads
WORKDIR /src/vcpkg/downloads
# de la merde pour le souci avec liblzma
RUN wget "https://github.com/bminor/xz/archive/refs/tags/v5.4.4.tar.gz"
RUN cp v5.4.4.tar.gz  /src/vcpkg/downloads/tukaani-project-xz-v5.4.4.tar.gz
RUN cp v5.4.4.tar.gz  /src/vcpkg/downloads/tukaani-project-xz-v5-c2846112.4.4.tar.gz
WORKDIR /src/vcpkg
RUN ./bootstrap-vcpkg.sh -disableMetrics && ./vcpkg integrate install && ./vcpkg install
WORKDIR /src
COPY . .
WORKDIR /src/build
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/src/vcpkg/scripts/buildsystems/vcpkg.cmake ..  && \
    cmake --build . --config Release -j 10 --target DarwinServer --target DarwinClient  && \
    strip ./Server/DarwinServer && \
    strip ./Client/DarwinClient
RUN mkdir /output && cp ./Server/DarwinServer /output && cp ./Client/DarwinClient /output

FROM ubuntu:23.10
RUN rm /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' >/etc/apt/apt.conf.d/keep-cache
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt update && apt install --yes libstdc++6-amd64-cross libc6-amd64-cross
RUN groupadd --gid 1001 Darwin
RUN useradd --uid 1001 --gid 1001 --create-home Darwin
USER Darwin
EXPOSE 45323
COPY --from=BUILD /output/DarwinServer /usr/local/bin/DarwinServer
COPY --from=BUILD /output/DarwinClient /usr/local/bin/DarwinClient
COPY Server/world_db.json /etc/world_db.json
CMD /usr/local/bin/DarwinServer --world_db=/etc/world_db.json
