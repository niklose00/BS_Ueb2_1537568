FROM gcc:latest

WORKDIR /app

# Installiere ZeroMQ-Entwicklungspakete
RUN apt-get update && apt-get install -y libzmq3-dev

# Kopiere die Quell- und Header-Dateien in das Docker-Image
COPY sender.c .
COPY ./0_Bib/csv_writer.h ./csv_writer.h

# Kompiliere das Programm
RUN gcc -o sender sender.c -lzmq -lm

# Setze das Standardkommando
CMD ["./sender"]
