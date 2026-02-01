CC = gcc
CFLAGS = -Wall -std=c99 -O3
OBJDIR = obj

default: wrdmttns

$(OBJDIR):
	mkdir -p $(OBJDIR)

wrdmttns: $(OBJDIR)/main.o $(OBJDIR)/file.o $(OBJDIR)/array.o $(OBJDIR)/graph.o $(OBJDIR)/heap.o $(OBJDIR)/dijkstra.o
	$(CC) $(CFLAGS) -o wrdmttns $(OBJDIR)/main.o $(OBJDIR)/file.o $(OBJDIR)/array.o $(OBJDIR)/graph.o $(OBJDIR)/heap.o $(OBJDIR)/dijkstra.o

$(OBJDIR)/main.o: main.c file.h array.h graph.h heap.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c main.c -o $(OBJDIR)/main.o

$(OBJDIR)/file.o: file.c file.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c file.c -o $(OBJDIR)/file.o

$(OBJDIR)/array.o: array.c array.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c array.c -o $(OBJDIR)/array.o

$(OBJDIR)/graph.o: graph.c graph.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c graph.c -o $(OBJDIR)/graph.o

$(OBJDIR)/heap.o: heap.c heap.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c heap.c -o $(OBJDIR)/heap.o

$(OBJDIR)/dijkstra.o:  dijkstra.c dijkstra.h | $(OBJDIR)
	$(CC) $(CFLAGS) -c dijkstra.c -o $(OBJDIR)/dijkstra.o

clean:
	rm -rf $(OBJDIR) wrdmttns *~