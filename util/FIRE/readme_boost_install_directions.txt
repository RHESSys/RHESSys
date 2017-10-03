Boost Library Install/Setup instructions for use with WMFire

Will Burke 9/25/17

1)	Download C++ Boost library: http://www.boost.org/
	Directions were done using version 1.65.1 (1_65_1), replace file/folder names as necessary
	download .zip or similarly compressed file (note .7z cannot be extracted on Mac)
	
2)	Extract file, and move resulting folder to "/usr/local/bin/" (standard location for unix libraries), 
	or alternatively, wherever you want
	
4)	Open your .bash_profile (typically located on Mac in /Users/<user name>/ ) and add the line:
	export BOOST_ROOT="/usr/local/bin/boost_1_65_1"
	
	Replacing the filepath and version number is needed. When finished, remember to source:
	source ~/.bash_profile
	
5)	In a terminal, navigate to your Boost library folder (running "cd $BOOST_ROOT" should take you there)
	Run bootstrap.sh

6)	In the same folder, there should be a new executable called "b2",
	Finish building the library by running "./b2"
	This may take a bit and will create a bunch of new files and folders
	
7)	Add the following lines to your .bash_profile:
	export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/local/bin/boost_1_65_1
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/bin/boost_1_65_1
	
8)	Add directory to path
	export PATH=$PATH:$BOOST_ROOT
	or alternatively, move the "bjam" executable to somewhere in your path
	
9)	Navigate to the folder containing the WMFire model, eg. /work.salsa/RHESSys/rhessys_rrb/RHESSys/util/FIRE
	IMPORTANT: this should be your version of the model, where you have admin read/write privileges.
	Run 'bjam'
	Run 'bjam clean'
	Run 'bjam release'
	This should create the file libwmfire.dylib in the folder build/darwin-4.2.1/release/threading_multi/
	Keep in mind, the git version of the the fire model in RHESSys currently has a copy of libwmfire.dylib, 
	but this should be deleted/overwritten by your new version
	
10)	Copy the libwmfire.dylib file to <your RHESSys version>/rhessys/lib
	Again, overwrite any existing versions
	The following line run from your fire model folder should do this:
	cp build/darwin-4.2.1/release/threading-multi/libwmfire.dylib ../../rhessys/lib/

11)	Also copy libwmfire.dylib to the directory you run RHESSys from (often scripts)
	or alternatively (MAY NOT WORK), add it to you PATH in your .bash_profile, eg:
	export PATH=$PATH:<your rhessys location>/rhessys/rhessys_git/rhessys/lib/"

12)	Compile RHESSys with the additional argument: make wmfire='T'

13)	If changes are made to the fire model, repeat steps 9-12
	

