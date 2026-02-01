CC = gcc
CFLAGS = -Wall -std=c99 -O3
TEST_DIR ?= testing/Size07to15
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
	
# Support for "make run_benchmark Big01"
ifeq (run_benchmark,$(firstword $(MAKECMDGOALS)))
  BENCHMARK_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(BENCHMARK_ARGS):;@:)
endif

run_benchmark: wrdmttns
	./benchmark.sh $(if $(BENCHMARK_ARGS),$(BENCHMARK_ARGS),$(TEST_DIR))

# Compare all optimization modes. Usage: make benchmark_modes [TEST_DIR=testing/Size07to15]
benchmark_modes:
	chmod +x compare_modes.sh
	./compare_modes.sh $(TEST_DIR)
	python3 compare_results.py

# Detailed comparison of Mode 0 vs Mode 3 with optimizations shown. Usage: make benchmark_detailed
benchmark_detailed:
	python3 benchmark_detailed.py

# Compare benchmark results. Usage: make compare OLD=old.txt NEW=new.txt
compare:
	python3 compare_benchmarks.py $(OLD) $(NEW)

# Run benchmarks for Mode 0 and Mode 3 and compare them
compare_0_3: wrdmttns
	./benchmark.sh $(TEST_DIR) 0
	./benchmark.sh $(TEST_DIR) 3
	python3 compare_benchmarks.py $(notdir $(patsubst %/,%,$(TEST_DIR)))_mode0.txt $(notdir $(patsubst %/,%,$(TEST_DIR)))_mode3.txt

# Interactive menu
menu: wrdmttns
	chmod +x menu.sh
	./menu.sh

clean:
	rm -rf $(OBJDIR) wrdmttns *~