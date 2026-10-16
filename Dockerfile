FROM debian:trixie-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends gcc make libc6-dev \
 && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY . .
RUN make all example && make install PREFIX=/usr DESTDIR=/out

FROM debian:trixie-slim
COPY --from=build /out/ /
COPY --from=build /src/example/example /usr/local/bin/closedcaption-example
RUN ldconfig
ENTRYPOINT ["closedcaption-example"]
