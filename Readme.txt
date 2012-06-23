Hi this is the Readme file for my Gsoc 2012 project "Object Based Image Analysis Tooles for Opticks"

It has a folder named OA.
1. Copy OA  "<SDK-directory>\application\PlugIns\src\"
2. COpy OBIA_plugin.sln to "<SDK-directory>\application\"
3. run OBIA_plugin in visual studio and build as per your choice.


For running the plugin in opticks :
Copy the dll files from Release folder of build "<sdk-directory>\Build\Binaries-Win32-release\PlugIns\" to the plugin folder of Opticks installation. 
Run opticks.
	

I have added folder named mymeanshift. This folder contains the meanshift executable that implements the meanshift algorithm for image segmentation.
You just have to give the location of "mymeanshift/release"  folder when you are asked to browse for meanshift folder. 
This is just a temporary folder and will be romoved once I integrate the segmentation code in tha source.
The mean shift code is made using the "EDISON" project.