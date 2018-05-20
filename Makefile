OUTPUT_FILE=stegobmp

all:
	gcc -o $(OUTPUT_FILE) src/*.c src/*.h -lm

clean:
	rm -f $(OUTPUT_FILE) *.o