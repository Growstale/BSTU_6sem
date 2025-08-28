set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_VERSION 1)

# Путь к компилятору MinGW
set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)

# Указываем, что это Windows
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

# Указываем пути для поиска библиотек и заголовочных файлов для Windows
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
