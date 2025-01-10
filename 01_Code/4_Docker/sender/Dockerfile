FROM gcc:latest

WORKDIR /app

# Installiere ZeroMQ-Entwicklungspakete
RUN apt-get update && apt-get install -y libzmq3-dev

# Kopiere die Quell- und Header-Dateien in das Docker-Image
COPY ./01_Code/4_Docker/sender/sender.c .
COPY ./01_Code/0_Bib/csv_writer.h ./csv_writer.h
COPY ./01_Code/0_Bib/statistics.h ./statistics.h



# Kompiliere das Programm
RUN gcc -o sender sender.c -lzmq -lm

# Setze das Standardkommando
CMD ["./sender"]
