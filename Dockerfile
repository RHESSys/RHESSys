# syntax=docker/dockerfile:1
FROM ubuntu:18.04
CMD ["/bin/bash"]
WORKDIR /RHESSys/
ADD rhessys_git/rhessys rhessys
ADD RHESSysIOinR RHESSysIOinR
RUN apt-get upgrade
RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" apt-get install --fix-missing -y build-essential clang pkg-config libbsd-dev libglib2.0 libglib2.0-dev libnetcdf-dev flex bison python3 vim r-base
RUN Rscript -e "install.packages(c('chron','dplyr','tidyr','readr','purrr','data.table','lubridate','sensitivity','hydroGOF','parallel','randtoolbox','rlang','tools','yaml'))"
