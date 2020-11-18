FROM ubuntu:20.04
ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update -y --fix-missing \
 && apt-get install -y --fix-missing \
 apt-utils \
 && apt-get install -y --fix-missing \
 sudo \
 vim \
 less \
 autoconf \
 binutils-dev \
 build-essential \
 curl \
 ccache \
 cmake \
 ninja-build \
 libtool \
 language-pack-zh-hans \
 subversion \
 git \
 python \
 net-tools \
 linux-tools-generic \
 gdb \
 gcc-9 \
 g++-9 \
 clang-9 \
 clang-format-9 \
 clang-tidy-9 \
 libclang-9-dev \
 lldb-9 \
 lld-9 \
 libc++-9-dev \
 libmysqlclient-dev \
 mysql-client \
 libcurl4-openssl-dev \
 libreadline6-dev \
 libssl-dev \
 libasan6 \
 tzdata \
 openssh-server




ENV LC_CTYPE=zh_CN.UTF-8 
ENV TZ=Asia/Shanghai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN dpkg-reconfigure -f noninteractive tzdata
ENV NINJA_STATUS="[%p/%f/%t %e] "
#RUN echo "/corefile/core-%e-%p-%t" > /proc/sys/kernel/core_pattern

RUN useradd --create-home --no-log-init --shell /bin/bash ubuntu
RUN adduser ubuntu sudo
RUN adduser ubuntu root
RUN passwd -d ubuntu
RUN echo "ubuntu   ALL=(ALL)     ALL" >>/etc/sudoers
RUN mkdir -p /data/mmorpg
RUN chown ubuntu /data/mmorpg
USER ubuntu


WORKDIR /home/ubuntu




