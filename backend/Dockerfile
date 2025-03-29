FROM silkeh/clang:20 AS builder

ENV CPM_SOURCE_CACHE=/.cpm_cache

VOLUME [ "${CPM_SOURCE_CACHE}" ]

RUN apt-get update && apt-get install -y ninja-build && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN cmake -B build -S .       \
  -G Ninja                    \
  -D CMAKE_BUILD_TYPE=Release \
  -D ENABLE_TESTING=OFF       \
  -D BUILD_DOCS=OFF &&        \
  cmake --build build --config Release --target io_blair_server

FROM gcr.io/distroless/cc-debian12

ENV SERVER_PORT=443
EXPOSE ${SERVER_PORT}

WORKDIR /app

COPY --from=builder /app/build/bin/io_blair_server .

ENTRYPOINT [ "./io_blair_server" ]
