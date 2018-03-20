TARGET=TinyJulia
SOURCES=parser.cpp lexer.cpp ast.cpp main.cpp
OBJECTS=${SOURCES:.cpp=.o}
.PHONY: clean run

$(TARGET): $(OBJECTS)
	g++ -std=c++11 -g -o $@ $^

parser.cpp: parser.y
	bison --defines=tokens.h -o $@ $^

lexer.cpp: lexer.l
	flex -o $@ $^

%.o:%.cpp
	g++ -std=c++11 -c -g -o $@ $^

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)
	rm -f parser.cpp lexer.cpp tokens.h
	rm -f run.asm run.o run

run: $(TARGET)
	./$(TARGET) try.tjl > run.asm
	nasm -felf run.asm
	gcc -m32 -o run run.o
	./run