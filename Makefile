CC = gcc
CFLAGS = -I/opt/homebrew/Cellar/sdl2/2.32.4/include
LDFLAGS = -L/opt/homebrew/Cellar/sdl2/2.32.4/lib -lSDL2
TARGET = Shapes3D
SRC = 3D.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)