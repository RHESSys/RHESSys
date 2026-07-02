# syntax=docker/dockerfile:1
FROM ubuntu:24.04

# make sure that it is non-interactive
ARG DEBIAN_FRONTEND=noninteractive
ARG TZ=America/Los_Angeles
ENV DEBIAN_FRONTEND=${DEBIAN_FRONTEND} \
    TZ=${TZ}

# install linux libs
RUN DEBIAN_FRONTEND="noninteractive" apt-get update && \
	apt-get install --yes \
		bison \
		build-essential \
		clang-20 \
		flex \
		lld-20 \
		libnetcdf-dev \
		libomp-20-dev \
		pkg-config \
		python3 \
		wget

# RUN ln -sf /usr/lib/llvm-20/bin/ld.lld /usr/local/bin/ld.lld

# set the working directory
# copy the build environment to the working dir
# make rhessys and install it (install path set in makefile)
WORKDIR /RHESSys
COPY ./rhessys .
RUN make clean && make all openmp='T' 
#CC=clang-20 CMD_OPTS='-fuse-ld=lld'

# Special thanks to Ojas for finding Viruzzo and other excellent people over at the RPS Discord server who donated their time, patience and expertise to help us get this dockerfile fixed and cleaned up, in accordance with good IT practices.
