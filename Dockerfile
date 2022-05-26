#syntax=docker/dockerfile:q
FROM ubuntu:20.04
CMD ["/bin/bash"]
COPY ./rhessys /RHESSys
WORKDIR /RHESSys
RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" apt-get install --fix-missing -y build-essential clang pkg-config libbsd-dev libglib2.0 libglib2.0-dev libnetcdf-dev flex bison python3 libxml2-dev libfontconfig1-dev libmariadbclient-dev libssl-dev vim r-base git
RUN make all /RHESSys
RUN git clone https://github.com/RHESSys/RHESSysIOinR.git /RHESSysIOinR
RUN Rscript -e "install.packages(c('chron','dplyr','tidyr','readr','purrr','data.table','lubridate','sensitivity','hydroGOF','parallel','randtoolbox','rlang','tools','yaml','httr','gh','xml2','roxygen','devtools'), dependencies=TRUE)"
RUN Rscript -e "setwd('../RHESSysIOinR/')" -e "library(devtools)" -e "build()" -e "install()" -e "library(RHESSysIOinR)"
