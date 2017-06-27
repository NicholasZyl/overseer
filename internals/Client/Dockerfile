FROM madduci/docker-ubuntu-cpp:gcc-6

RUN apt-get update \
    && apt-get install -y \
    libssl-dev nginx supervisor \
    && echo '[cron]' >> /etc/supervisor/conf.d/cron.conf \
    && echo 'command=crontab' >> /etc/supervisor/conf.d/cron.conf \
    && apt-get clean autoclean \
    && apt-get autoremove -y

ADD . /project
WORKDIR /project

RUN mkdir -p /project/build \
    && cd /project/build \
    && cmake .. \
    && cmake --build . --target Client