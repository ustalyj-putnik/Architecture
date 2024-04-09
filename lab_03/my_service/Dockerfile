FROM ubuntu

ENV TZ=Europe/Moscow

RUN apt-get update && apt-get upgrade -y

RUN DEBIAN_FRONTEND=noninteractive apt-get install -y tzdata git build-essential autoconf libtool
RUN apt-get install -y python3 pip python3.10-venv iputils-ping gcc-12 g++ libpq-dev postgresql-client wrk libssl-dev zlib1g-dev
RUN apt-get install -y pkg-config cmake libboost-all-dev && apt-get clean

RUN apt-get install -y gnupg wget
RUN wget -qO- https://www.mongodb.org/static/pgp/server-7.0.asc | tee /etc/apt/trusted.gpg.d/server-7.0.asc
RUN echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/7.0 multiverse" | tee /etc/apt/sources.list.d/mongodb-org-7.0.list
RUN apt-get update && apt-get install -y mongodb-mongosh

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
RUN mkdir build && cd build && cmake .. && make && cd ..
#ENTRYPOINT [ "/app/build/lab_03" ]