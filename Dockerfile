FROM ubuntu:latest
LABEL maintainer="max.laager@gmail.com"
COPY ./Release/DarwinServer /usr/local/bin/DarwinServer
RUN groupadd --gid 1000 Darwin
RUN useradd --uid 1000 --gid 1000 -m Darwin
RUN chmod a+rx /usr/local/bin/DarwinServer

USER Darwin
CMD /usr/local/bin/DarwinServer