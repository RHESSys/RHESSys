# syntax=docker/dockerfile:1
FROM ubuntu:20.04
CMD ["/bin/bash"]
COPY ./rhessys /RHESSys
WORKDIR /RHESSys
RUN apt-get update
RUN DEBIAN_FRONTEND="noninteractive" apt-get install --fix-missing -y build-essential clang pkg-config libbsd-dev libglib2.0 libglib2.0-dev libnetcdf-dev flex bison python3 libxml2-dev libfontconfig1-dev libmariadbclient-dev libharfbuzz-dev libfribidi-dev libfreetype6-dev libpng-dev libtiff5-dev libjpeg-dev libssl-dev vim r-base git
RUN make all /RHESSys
RUN Rscript -e "install.packages(c('chron','dplyr','tidyr','readr','purrr','forcats','stringr','tibble','formattable','ggpubr','readxl','data.table','lubridate','sensitivity','hydroGOF','parallel','randtoolbox','rlang','tools','yaml','httr','gh','xml2','roxygen','rmarkdown','devtools'), dependencies=TRUE)"
RUN Rscript -e "library('devtools')" -e "install_github('RHESSys/RHESSysIOinR', ref='develop', build_vignettes=FALSE, dependencies=TRUE)"
