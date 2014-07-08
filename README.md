FractalMap
=========

This is an open source program in early development.

The point of this program/game is to allow the user/player to explore a world that has infinite precision.
It is our goal to make a map that is made up of a linked block structure that becomes more and more detailed as the user "zooms into" the map.
The user will be able to zoom in practically infinitely.
The user will also be able to zoom out practically infinitely.
The amount that a user can zoom out will be limited by the data types used (signed long long int).
so on most modern machines that will be plus or minus 9.223372e+18 levels from the origin.
although, practically, your hard drive would run out of memory before you used even a small fraction of those levels.

Check out the images folder to see some examples of what is currently working.

###how to compile using Code::Blocks

+ make sure you have the proper SDL 2 development libraries set up on your machine. (SDL2 and SDL2_Image) Your compiler will have to know where to look for the SDL header files (SDL2/SDL.h, etc...) Follow [LazyFoo's SDL Tutorials](http://lazyfoo.net/tutorials/SDL/index.php) to set up SDL on your machine.
+ open the project in Code::Blocks (version 12.11 or later)
+ select "ReleaseWin" as you build target (for windows)
+ select "ReleaseLinux" as you build target (for linux)
+ compile the program (press F9)
+ *if you are having troubles compiling the program, please contact either Jensen or JP about your issue. We are here to help*

####Commit Conventions

added    +

removed   -

updated   ^

tweaked   ~

bug fix  %

###Collaborators



JensenR30 - JensenR30@Gmail.com

+ Fractal Block Data Architecture
+ Memory Management


JPisaBrony - jdpohl789@gmail.com

+ randomized graphics generation
+ Cross compilation with Linux
