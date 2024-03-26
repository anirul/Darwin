FROM ubuntu:23.10 AS BASE
RUN rm /etc/apt/apt.conf.d/docker-clean; echo 'Binary::apt::APT::Keep-Downloaded-Packages "true";' >/etc/apt/apt.conf.d/keep-cache

FROM BASE AS BUILD
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt update && apt install --yes libstdc++6-amd64-cross libc6-amd64-cross  \
    libxmu-dev libxi-dev libgl-dev libglx-dev libgles-dev  build-essential libx11-xcb-dev libegl1-mesa-dev libopengl-dev \
     libxkbcommon-dev libwayland-dev libxrandr-dev mesa-vulkan-drivers libglu1-mesa-dev libdrm-dev libegl-dev libglm-dev  \
    curl zip unzip tar cmake  pkg-config  python3-jinja2  gcc g++ git  autoconf libtool meson
RUN git clone --depth=1 --branch=2024.02.14 --config=advice.detachedHead=false https://github.com/microsoft/vcpkg.git /var/tmp/vcpkg
RUN /var/tmp/vcpkg/bootstrap-vcpkg.sh -disableMetrics
RUN --mount=type=bind,target=/var/tmp/src \
    --mount=type=cache,target=/root/.cache/vcpkg \
    cmake -S /var/tmp/src -B /var/tmp/build -D CMAKE_TOOLCHAIN_FILE=/var/tmp/vcpkg/scripts/buildsystems/vcpkg.cmake
RUN --mount=type=bind,target=/var/tmp/src \
    cmake --build /var/tmp/build --config Release --parallel 10 --target DarwinServer

FROM BASE
LABEL maintainer="max.laager@gmail.com"
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt update && apt install --yes libstdc++6-amd64-cross libc6-amd64-cross
RUN groupadd --gid 1001 Darwin
RUN useradd --uid 1001 --gid 1001 --create-home Darwin
USER Darwin
EXPOSE 45323
COPY --from=BUILD /var/tmp/build/Server/DarwinServer /usr/local/bin/DarwinServer
COPY Server/world_db.json /etc/world_db.json
CMD /usr/local/bin/DarwinServer --world_db=/etc/world_db.json
