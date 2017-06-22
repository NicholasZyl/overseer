FROM madduci/docker-ubuntu-cpp:gcc-6

RUN apt-get update \
    && apt-get install -y \
    libssl-dev supervisor

ENTRYPOINT supervisord