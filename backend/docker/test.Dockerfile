FROM silkeh/clang:20

RUN apt-get update && apt-get install -y ninja-build && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -B build -S .       \
  -G Ninja                    \
  -D CMAKE_BUILD_TYPE=Debug   \
  -D ENABLE_TESTING=ON        \
  -D BUILD_DOCS=OFF &&        \
  cmake --build build --config Debug --target io_blair_server_test

WORKDIR /app/build

CMD ctest --build-config Debug --output-on-failure
