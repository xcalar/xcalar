#
# From: https://github.com/solita/docker-systemd
# MIT License
#
FROM ubuntu:18.04
ENV container docker

RUN apt-get update && \
    apt-get install -y \
    dbus systemd && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Don't start any optional services except for the few we need.
RUN find /etc/systemd/system \
    /lib/systemd/system \
    -path '*.wants/*' \
    -not -name '*journald*' \
    -not -name '*systemd-tmpfiles*' \
    -not -name '*systemd-user-sessions*' \
    -exec rm -fv \{} \; \
    && systemctl set-default multi-user.target


COPY setup /sbin/

STOPSIGNAL SIGRTMIN+3

# Workaround for docker/docker#27202, technique based on comments from :docker/docker#9212
#CMD ["/bin/bash", "-c", "exec /sbin/init --log-target=journal 3>&1"]
CMD ["/lib/systemd/systemd","--system","--log-target=journal"]

