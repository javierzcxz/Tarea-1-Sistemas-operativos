CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lpthread

FUENTES = main.cpp leerplan.cpp grafo.cpp
CABECERAS = actividad.h grafo.h leerplan.h
OBJETOS = main.o leerplan.o grafo.o
EJECUTABLE = planificador

all: $(EJECUTABLE)

$(EJECUTABLE): $(FUENTES) $(CABECERAS)
	$(CXX) $(CXXFLAGS) -c $(FUENTES)
	$(CXX) $(CXXFLAGS) $(OBJETOS) -o $(EJECUTABLE) $(LDFLAGS)
	rm -f $(OBJETOS)

clean:
	rm -f $(OBJETOS) $(EJECUTABLE)

.PHONY: all clean