FROM gcc:latest
WORKDIR /app
RUN apt-get update && apt-get install -y libzmq3-dev
COPY receiver.c .
RUN gcc -o receiver receiver.c -lzmq
CMD ["./receiver"]
