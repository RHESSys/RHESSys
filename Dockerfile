# syntax=docker/dockerfile:1
FROM ubuntu:20.04
# ensures every time we use 20.04 LTS
# this is the latest known version that does not segfault

# install linux libs
RUN DEBIAN_FRONTEND="noninteractive" apt-get update && \
	apt-get install --yes \
		bison \
		build-essential \
		clang \
		cmake \
		dirmngr \
		flex \
		git \
		libbsd-dev \
		libfreetype6-dev \
		libfribidi-dev \
		libfontconfig1-dev \
		libgdal-dev \
		libglib2.0 \
		libglib2.0-dev \
		libharfbuzz-dev \
		libjpeg-dev \
		libmariadb-dev \
		libnetcdf-dev \
		libpq-dev \
		libpng-dev \
		libssl-dev \
		libtiff5-dev \
		libudunits2-dev \
		libxml2-dev \
		pkg-config \
		python3 \
		software-properties-common \
		vim \
		wget
		
# install r-base and r-base-dev
RUN DEBIAN_FRONTEND="noninteractive" wget -qO- https://cloud.r-project.org/bin/linux/ubuntu/marutter_pubkey.asc | tee -a /etc/apt/trusted.gpg.d/cran_ubuntu_key.asc && \
	add-apt-repository "deb https://cloud.r-project.org/bin/linux/ubuntu $(lsb_release -cs)-cran40/" && \
	apt-get update && apt-get install --yes r-base r-base-dev

# set the working directory
# copy the build environment to the working dir
# make rhessys and install it (install path set in makefile)
WORKDIR /RHESSys
COPY ./rhessys .
RUN make clean && make all

# install R packages
RUN Rscript -e "install.packages(c('chron','data.table','devtools','forcats','formattable','gh','ggpubr','httr','hydroGOF','lhs','lubridate','randtoolbox','readxl','rlang','roxygen2','rmarkdown','sensitivity','stringr','tibble','tidyverse','yaml','xml2'), dependencies=TRUE)"
RUN Rscript -e "library('devtools')" -e "install_github('RHESSys/RHESSysIOinR', ref='develop', build_vignettes=FALSE, dependencies=TRUE)"

# Special thanks to Ojas for finding Viruzzo and other excellent people over at the RPS Discord server who donated their time, patience and expertise to help us get this dockerfile fixed and cleaned up, in accordance with good IT practices.