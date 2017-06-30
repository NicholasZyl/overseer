FROM madduci/docker-ubuntu-cpp:gcc-6

RUN apt-get update \
    && apt-get install -y \
    libssl-dev git supervisor rsyslog \
    && echo '[cron]' >> /etc/supervisor/conf.d/cron.conf \
    && echo 'command=crontab' >> /etc/supervisor/conf.d/cron.conf \
    && echo '[syslog]' >> /etc/supervisor/conf.d/cron.conf \
    && echo 'command=rsyslog' >> /etc/supervisor/conf.d/cron.conf \
    && apt-get clean autoclean \
    && apt-get autoremove -y \
    && git clone https://github.com/jbeder/yaml-cpp.git /project/lib/yaml-cpp \
    && ( \
        cd /project/lib/yaml-cpp \
        && mkdir build \
        && cd build \
        && cmake -DBUILD_SHARED_LIBS=OFF -DYAML_CPP_BUILD_TOOLS=OFF .. \
        && make \
        && cp libyaml-cpp.a /usr/local/lib \
        && cd ../ \
        && rm -rf build \
        && mkdir /usr/local/include/yaml-cpp \
        && cp -R include/yaml-cpp/* /usr/local/include/yaml-cpp \
        && cd ../../ \
        && rm -rf lib/ \
    )

ADD . /project
WORKDIR /project

ENTRYPOINT supervisord -n