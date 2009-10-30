########################################
#
# RHESSys master makefile
#
# This file will build, install, and clean rhessys and all associated tools
#
########################################

DIRS = \
	rhessys \
	g2w \
	cf

default:
	@dir_list='$(DIRS)'; \
	current_dir=`pwd`; \
	for dir in $$dir_list; do \
		cd $$current_dir/$$dir; \
		$(MAKE) -$(MAKEFLAGS); \
	done

install:
	@dir_list='$(DIRS)'; \
	current_dir=`pwd`; \
	for dir in $$dir_list; do \
		cd $$current_dir/$$dir; \
		$(MAKE) -$(MAKEFLAGS) install; \
	done

clean:
	@dir_list='$(DIRS)'; \
	current_dir=`pwd`; \
	for dir in $$dir_list; do \
		cd $$current_dir/$$dir; \
		$(MAKE) -$(MAKEFLAGS) clean; \
	done

clobber:
	@dir_list='$(DIRS)'; \
	current_dir=`pwd`; \
	for dir in $$dir_list; do \
		cd $$current_dir/$$dir; \
		$(MAKE) -$(MAKEFLAGS) clobber; \
	done
