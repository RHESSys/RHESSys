# syntax=docker/dockerfile:1
FROM ubuntu:20.04
CMD ["/bin/bash"]
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

# install BOOST libraries for RHESSys FIRE
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
RUN mv boost_1_82_0.tar.gz /usr/local/bin
RUN tar -xzf /usr/local/bin/boost_1_82_0.tar.gz -C /usr/local/bin/

ENV BOOST_ROOT=/usr/local/bin/boost_1_82_0
ENV CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/bin/boost_1_82_0/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/bin/boost_1_82_0/
ENV PATH=$PATH:$BOOST_ROOT
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/RHESSys/lib/

RUN echo 'export BOOST_ROOT=/usr/local/bin/boost_1_82_0' >>~/.bashrc
RUN echo 'export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/bin/boost_1_82_0' >>~/.bashrc
RUN echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/bin/boost_1_82_0' >>~/.bashrc
RUN echo 'export PATH=$PATH:$BOOST_ROOT' >>~/.bashrc
RUN echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/RHESSys/lib/' >>~/.bashrc
RUN . ~/.bashrc

RUN cd $BOOST_ROOT
RUN echo $SHELL
#RUN chmod +x bootstrap.sh
CMD ["/bin/sh", "bootstrap.sh"]
RUN . bootstrap.sh
RUN . $BOOST_ROOT/b2

# set the working directory
# copy the build environment to the working dir
WORKDIR /RHESSys
COPY ./ .
RUN mkdir lib

# finish installing WMFire library
RUN cd FIRE
RUN b2
RUN b2 clean
RUN b2 release

RUN cp build/clang-darwin14/release/threading-multi/libwmfire.dylib ../lib/

RUN cd /RHESSys/rhessys
RUN make wmfire='T'

# install r-base and r-base-dev
RUN DEBIAN_FRONTEND="noninteractive" wget -qO- https://cloud.r-project.org/bin/linux/ubuntu/marutter_pubkey.asc | tee -a /etc/apt/trusted.gpg.d/cran_ubuntu_key.asc && \
	add-apt-repository "deb https://cloud.r-project.org/bin/linux/ubuntu $(lsb_release -cs)-cran40/" && \
	apt-get update && apt-get install --yes r-base r-base-dev

# install R packages
RUN Rscript -e "install.packages(c('chron','data.table','devtools','forcats','formattable','gh','ggpubr','httr','hydroGOF','lhs','lubridate','randtoolbox','readxl','rlang','roxygen2','rmarkdown','sensitivity','stringr','tibble','tidyverse','yaml','xml2'), dependencies=TRUE)"
RUN Rscript -e "library('devtools')" -e "install_github('RHESSys/RHESSysIOinR', ref='develop', build_vignettes=FALSE, dependencies=TRUE)"

# Special thanks to Ojas for finding Viruzzo and other excellent people over at the RPS Discord server who donated their time, patience and expertise to help us get this dockerfile fixed and cleaned up, in accordance with good IT practices.
