FROM ubuntu:22.04
LABEL maintainer="Jay"

WORKDIR /bcc-playground
RUN apt update && apt install -y zip bison netcat \
    build-essential cmake flex git libedit-dev \
    libllvm14 llvm-14-dev libclang-14-dev python3 \
    zlib1g-dev libelf-dev libfl-dev python3-setuptools \
    liblzma-dev libdebuginfod-dev arping netperf iperf \


RUN git clone https://github.com/iovisor/bcc.git /bcc-playground/bcc
WORKDIR /bcc-playground/bcc/build
RUN cmake .. && make && make install && cmake -DPYTHON_CMD=python3 ..
WORKDIR /bcc-playground/bcc/build/src/python
RUN make && make install
WORKDIR /bcc-playground/

ENTRYPOINT ["/bin/bash"]