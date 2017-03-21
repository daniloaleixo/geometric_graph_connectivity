CC = gcc
CFLAGS = -Wall 
LFLAGS = -lm
OUT = ep1
IN = ep1.c

all: $(OUT)

clean:
	rm -f $(OUT) ep1.o
	

$(OUT): ep1.o 
	$(CC) ep1.o $(CFLAGS) $(LFLAGS) -o $(OUT)

ep1_final.o: $(IN)
	$(CC) $(IN) $(CFLAGS) -c -o ep1.o

