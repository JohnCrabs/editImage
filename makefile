CFLAGS = -Wall -pedantic -Werror
LFLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_ttf
OBJS   = imgCalc.o epipole.o ortho.o LSM_solReduction.o graphicSystem.o inputSystem.o imgReduction.o filterTools.o mergeBands.o imgTools.o imgEdit.o main.o
FOLDER = src
PROG   = imgEditor
CC    = g++ -std=c++11 -std=gnu++14

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
%.o: $(FOLDER)/%.cpp $(FOLDER)/%.h
	$(CC) $(CFLAGS) -c $<

# linking the program.
$(PROG): $(OBJS)
	$(CC) $(OBJS) -o $(PROG) $(LFLAGS)

# cleaning everything that can be automatically recreated with "make".
clean:
	rm $(PROG) *.o
