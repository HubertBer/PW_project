CC := g++

# Target 1
wn: wyprzedazNew.cpp
	$(CC) -O2 -o wn wyprzedazNew.cpp

# Target 2
wpf: wyprzedazParallelFinal.cpp
	$(CC) -O2 -o wpf -pthread wyprzedazParallelFinal.cpp

# Target 3
womp: wyprzedazOMP.cpp
	$(CC) -O2 -o womp -fopenmp wyprzedazOMP.cpp

# Target to build all programs
all: wn wpf womp

# Clean target
clean:
	rm -f wn wpf womp
