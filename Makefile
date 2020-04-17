CC := g++
CFLAGS := -std=c++17 
FILES := zelduino.cpp
OUTPUT := zelduino

zelduino: 
	$(CC) $(FILES) $(CFLAGS) -o $(OUTPUT)

clean: 
	rm -rf $(OUTPUT)