FROM ubuntu

ENV TZ=Europe/Moscow

RUN apt-get update && apt-get upgrade -y
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata git build-essential autoconf libtool
RUN apt-get install -y python3 pip python3.10-venv iputils-ping gcc-12 g++ libpq-dev postgresql-client wrk libssl-dev zlib1g-dev
RUN apt-get install -y pkg-config cmake libboost-all-dev && apt-get clean

RUN git clone -b poco-1.12.4-release https://github.com/pocoproject/poco.git &&\
    cd poco &&\
    mkdir cmake-build &&\
    cd cmake-build &&\
    cmake .. &&\
    cmake --build . --config Release &&\
    cmake --build . --target install &&\
    cd && rm poco/* -rf


RUN ldconfig

COPY . /app
WORKDIR /app
RUN mkdir build && cd build && cmake .. && make