#
# For the copyright information for this file, please search up the
# directory tree for the first README file.
#

FROM ubuntu AS builder

RUN : \
  && export DEBIAN_FRONTEND=noninteractive \
  && apt-get -qy update \
  && apt-get -qy install \
       maven \
       openjdk-8-jdk \
       postgresql \
       sudo \
;

COPY build safrn.tar.gz /x/
RUN cd /x && ./build

FROM postgres:alpine

ENV POSTGRES_USER=dashboard
ENV POSTGRES_PASSWORD=dashboard

RUN : \
  && mkdir -p /run/nginx \
  && apk --no-cache add \
       bash \
       nginx \
       openjdk8-jre \
;

WORKDIR /dashboard

COPY --from=builder /dashboard.jar ./
COPY dashboard http.conf https.conf ./

ENTRYPOINT ["./dashboard"]
CMD []
