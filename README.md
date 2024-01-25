# WordMutations Project

## Introduction
The WordMutations project is part of the Algorithms and Data Structures course, designed to explore efficient algorithms through a practical application. This C program identifies the least costly path for transforming one word into another by altering individual characters, based on specific transformation rules and a dictionary.

## Project Description
The core challenge of WordMutations is to navigate a dictionary of words, transforming a source word into a target word with minimal cost. This transformation follows specific rules, such as the number of characters that can be changed in each step and the overall cost associated with each transformation.

## Installation
1. **Clone the Repository:**
   - Use `git clone` or download the project's zip file to your local machine.

2. **Compile the Program:**
   - Navigate to the project directory.
   - Run the `make` command to compile the project using the provided Makefile.

   ```
   cd path/to/WordMutations
   make
   ```

## Usage
After compilation, you can run the program as follows:

```
./wrdmttns <dictionary_file>.dict <pairs_file>.pals
```

- `<dictionary_file>.dict` should be replaced with the path to your dictionary file.
- `<pairs_file>.pals` is the file containing word pairs and transformation rules.

## Project Structure
- `main.c`: The main program file.
- `dijkstra.c` and `dijkstra.h`: Implementation of the Dijkstra's algorithm for path finding.
- `graph.c` and `graph.h`: Graph creation and manipulation functions.
- `heap.c` and `heap.h`: Heap data structure for priority queue implementation.
- `array.c` and `array.h`: Utility functions for array handling.
- `file.c` and `file.h`: File handling functions.

## Features
- Efficient word transformation pathfinding.
- Implementation of Dijkstra's algorithm for optimal path calculation.
- Dynamic handling of dictionaries and word pairs.
- Cost calculation based on the number of character changes.

## Credits
Developed as an assignment for the Algorithms and Data Structures course.
