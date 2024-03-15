FROM ubuntu:23.10
LABEL maintainer="max.laager@gmail.com"
RUN apt update && apt install -y libstdc++6-amd64-cross libc6-amd64-cross && \
    apt-get clean &&  rm -rf /var/lib/apt/lists/*
RUN groupadd --gid 1001 Darwin
RUN useradd   --uid 1001 --gid 1001 -m Darwin
EXPOSE 45323

COPY ./Release/DarwinServer /usr/local/bin/DarwinServer
RUN chmod a+rx /usr/local/bin/DarwinServer
COPY ./Release/world_db.json /etc/world_db.json
RUN chmod a+rw /etc/world_db.json
USER Darwin
CMD /usr/local/bin/DarwinServer --world_db=/etc/world_db.json