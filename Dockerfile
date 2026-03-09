FROM ubuntu:22.04

# Install build dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    g++-mingw-w64-x86-64 \
    wget \
    unzip \
    && rm -rf /var/lib/apt/lists/*

# Download SFML 2.6.2 for GCC MinGW (64-bit)
WORKDIR /opt
RUN wget https://www.sfml-dev.org/files/SFML-2.6.2-windows-gcc-13.1.0-mingw-64-bit.zip -O sfml.zip && \
    unzip sfml.zip && \
    rm sfml.zip && \
    mv SFML-2.6.2 /opt/sfml

# Set up working directory for the game source
WORKDIR /src
COPY . /src

# Compile the game
# -I /opt/sfml/include : Add SFML headers
# -L /opt/sfml/lib : Add SFML libraries
# We link statically to SFML and libstdc++
RUN x86_64-w64-mingw32-g++ -c *.cpp -I/opt/sfml/include -O2 -DSFML_STATIC && \
    x86_64-w64-mingw32-g++ *.o -o RPG_Game.exe -L/opt/sfml/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -static-libgcc -static-libstdc++ -lwinmm -lgdi32 -lopengl32 -lfreetype -ljpeg -lsfml-main

# Prepare the release folder
RUN mkdir -p /release && \
    cp RPG_Game.exe /release/ && \
    cp default.ttf *.png /release/ 2>/dev/null || true && \
    cp /opt/sfml/bin/*.dll /release/ && \
    cp /usr/lib/gcc/x86_64-w64-mingw32/10-posix/libstdc++-6.dll /release/ && \
    cp /usr/lib/gcc/x86_64-w64-mingw32/10-posix/libgcc_s_seh-1.dll /release/ && \
    cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll /release/ && \
    # The game seems to load "pixel.ttf", let's copy everything except cpp/h files just in case
    find . -maxdepth 1 -type f -not -name "*.cpp" -not -name "*.h" -not -name "*.o" -exec cp {} /release/ \;

# The container will just output the game files to stdout as a tar when run, 
# or we can use a bind mount to copy the files out.
CMD tar -cf - -C /release .
