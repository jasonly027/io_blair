FROM silkeh/clang:20

ENV SERVER_PORT=443

EXPOSE ${SERVER_PORT}

COPY . /app

WORKDIR /app

RUN cmake -B build -S . \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_TESTING=OFF \
  -D BUILD_DOCS=OFF 

RUN cmake --build build --config Release --target io_blair_server

WORKDIR /app/build/bin

CMD ./io_blair_server ${SERVER_PORT}
