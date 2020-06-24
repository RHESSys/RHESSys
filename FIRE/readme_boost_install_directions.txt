====================================================
Boost install/setup instructions for use with WMFire
====================================================

Instructions to build the Boost.Build tool, and build the WMFire shared library needed to run RHESSys-Fire

Will Burke 9/25/2017
Updated 6/19/2020

1)	Download C++ Boost library: http://www.boost.org/
	Directions were done using version 1.65.1 (1_65_1), replace file/folder names as necessary
	download .zip or similarly compressed file (note .7z is the Windows version).
	
2)	Extract file, and move resulting folder to "/usr/local/bin/" (standard location for unix libraries), 
	or alternatively, wherever you want.
	
3)	Open your .bash_profile (typically in your home directory, located on Mac in /Users/<user name>/ ), 
	or alternatively your .profile, and add the line:
	export BOOST_ROOT="/usr/local/bin/boost_1_65_1".
	
	Replacing the filepath and version number as needed. When finished, remember to source:
	source ~/.bash_profile
	
4)	In a terminal, navigate to your Boost library folder (running "cd $BOOST_ROOT" should take you there)
	Run bootstrap.sh
		* This is to build the Boost.Build tool, previously called "bjam" now "b2". You could also build
		the Boost libraries (by running "./b2"), but those are not actually needed to build the wmfire library
		and run RHESSys-Fire
	
5)	Make the Boost libraries and build tool acessible, add the following lines to your .bash_profile:
	export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/bin/boost_1_65_1
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/bin/boost_1_65_1
	export PATH=$PATH:$BOOST_ROOT (or move the "b2" binary to somewhere in your path)
	
6)	Navigate to the folder containing the WMFire model, e.g. /my_rhessys_version/RHESSys/FIRE
	Note: this should be your version of the model, where you have admin read/write privileges.
	Run 'b2'
	Run 'b2 clean'
	Run 'b2 release'
		* Preiously bjam was used in place of b2
	
	This should create the WMFire shared library, 'libwmfire.dylib' (or 'libwmfire.so') in the folder 
	build/darwin-X/release/threading_multi/ (or gcc-X instead of darwin)
	
7)	Make the libwmfire library available when compiling/running RHESSys.
	Copy libwmfire.dylib (or .so) to <your RHESSys version>/lib
		* If still in the /RHESSys/util/FIRE folder you can run:
		cp build/darwin-4.2.1/release/threading-multi/libwmfire.dylib ../lib/
		* Note: Previously the lib location was located at <your RHESSys version>/rhessys/lib

	Also copy libwmfire.dylib to the directory you run RHESSys from (often a scripts folder)
	OR add it to you PATH in your .bash_profile, eg:
	export PATH=$PATH:<your rhessys location>/rhessys/rhessys_git/rhessys/lib/"

8)	Compile RHESSys with the additional argument: make wmfire='T'

9)	If changes are made to the fire model, repeat steps 6-8
