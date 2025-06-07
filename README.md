**INSTALLATION**

*0: Install the Qt Framework*\n
MAC: brew install qt
LINUX: sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
WINDOWS: Download the installer

*1: Make a Build Directory*\n
mkdir build
cd build

*2: Generate the Build Files*\n
LINUX/MAC: cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt
WINDOWS: cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.9.0/mingw_64"

*3: Build the solution*\n
cmake --build .

*4: Run the executable*\n
./untitled
