DIRS = \
	grass2world \
	AverageTables_Unix

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
