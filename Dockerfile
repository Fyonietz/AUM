FROM ubuntu:22.04

RUN apt-get update && apt-get install -y libstdc++6 libgcc-s1

WORKDIR /app
COPY Production-Test/ .

EXPOSE 9001

CMD ["./Phoenix"]

