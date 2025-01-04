FROM gcc:latest
WORKDIR /app
RUN apt-get update && apt-get install -y libzmq3-dev
COPY sender.c .
RUN gcc -o sender sender.c -lzmq -lm
CMD ["./sender"]
