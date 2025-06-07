**INSTALLATION**

*0: Install the Qt Framework*
MAC: brew install qt
LINUX: sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
WINDOWS: Download the installer

*1: Make a Build Directory*
mkdir build
cd build

*2: Generate the Build Files*
LINUX/MAC: cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt
WINDOWS: cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.9.0/mingw_64"

*3: Build the solution*
cmake --build .

*4: Run the executable*
./untitled
