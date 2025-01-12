FROM gcc:latest

WORKDIR /app

# Installiere ZeroMQ-Entwicklungspakete
RUN apt-get update && apt-get install -y libzmq3-dev


# Kopiere Quell- und Header-Dateien
COPY ./01_Code/4_Docker/sender/sender.c ./sender.c
COPY ./01_Code/0_Bib/csv_writer.c ./csv_writer.c
COPY ./01_Code/0_Bib/statistics.c ./statistics.c
COPY ./01_Code/0_Bib/csv_writer.h ./csv_writer.h
COPY ./01_Code/0_Bib/statistics.h ./statistics.h

# Kompiliere das Programm
RUN gcc -g -o sender sender.c csv_writer.c statistics.c -lzmq -lm



# Setze das Standardkommando
CMD ["./sender"]

