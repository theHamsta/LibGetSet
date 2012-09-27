### Windows Build Instructions ###

Download and install Cmake
http://www.cmake.org/

Download or build GetSet libraries
https://sourceforge.net/projects/getset/

Start Cmake Gui and select the "example/console" directory.
You can use the same directory for both the source code and where to build the binaries.

Click configure and select your build system (Usually some version of VisualStudio)

If cmake complains that GetSet was not found, select the directory where GetSetConfig.cmake is located and click configure again.
If Configure is successful, click Generate and close Cmake

If you are using VisualStudio, go to the directory where you built the binaries and open the solution file.
Right-click the GetSetTest project and select "Set as start-up project" and press F5 to build and run.

If you encounter errors you are more than welcome to contact me with fixes! aaichert@gmail.com
