#!/bin/bash

# Default directory
DEFAULT_TEST_DIR="testing/Size07to15"
DEFAULT_DICT="testing/portugues.dict"

# Use the first argument as the test directory, or default
INPUT_DIR="${1:-$DEFAULT_TEST_DIR}"
MODE="$2"

# Smart directory resolution: check if it exists, if not check inside testing/
if [ -d "$INPUT_DIR" ]; then
    TEST_DIR="$INPUT_DIR"
elif [ -d "testing/$INPUT_DIR" ]; then
    TEST_DIR="testing/$INPUT_DIR"
else
    echo "Error: Directory '$INPUT_DIR' or 'testing/$INPUT_DIR' not found."
    exit 1
fi

# Remove trailing slash from TEST_DIR if present for clean filename
TEST_DIR=${TEST_DIR%/}

# Use the directory name for the results file
FOLDER_NAME=$(basename "$TEST_DIR")
if [ -n "$MODE" ]; then
    RESULTS_FILE="${FOLDER_NAME}_mode${MODE}.txt"
else
    RESULTS_FILE="${FOLDER_NAME}.txt"
fi

echo "Test Name/Time(s)/Memory(KB)/Method" > "$RESULTS_FILE"

echo "Starting benchmark on files in $TEST_DIR..."
echo "Results will be saved to $RESULTS_FILE"
echo ""

# Enable nullglob to handle cases where no .pals files exist
shopt -s nullglob
pals_files=("$TEST_DIR"/*.pals)
shopt -u nullglob

if [ ${#pals_files[@]} -eq 0 ]; then
    echo "No .pals files found in $TEST_DIR"
    exit 0
fi

for pals_file in "${pals_files[@]}"; do
    base_name=$(basename "$pals_file" .pals)
    
    # Determine dictionary file (local to test dir or default)
    if [ -f "$TEST_DIR/$base_name.dict" ]; then
        dict_file="$TEST_DIR/$base_name.dict"
    elif [ -f "$DEFAULT_DICT" ]; then
        dict_file="$DEFAULT_DICT"
    else
        dict_file=""
    fi
    
    if [ -n "$dict_file" ]; then
        # Run with /usr/bin/time to capture time (%e) and max resident memory (%M)
        # 2>&1 redirects stderr to stdout so we can capture it
        if [ -n "$MODE" ]; then
            output=$(/usr/bin/time -f "%e %M" ./wrdmttns "$dict_file" "$pals_file" "$MODE" 2>&1)
        else
            output=$(/usr/bin/time -f "%e %M" ./wrdmttns "$dict_file" "$pals_file" 2>&1)
        fi
        
        # Determine method label
        if [ -n "$MODE" ] && [ "$MODE" != "3" ]; then
            case "$MODE" in
                0) method="DIJKSTRA" ;;
                1) method="A*" ;;
                2) method="LAZY" ;;
                *) method="UNKNOWN" ;;
            esac
        else
            # Extract mode information from output (grep for Mode 3 selections)
            # Method pattern: [Mode 3] Size NN: ... → METHOD
            method_line=$(echo "$output" | grep "Mode 3" | head -1)
            
            if [ -n "$method_line" ]; then
                # Extract the method (DIJKSTRA, A*, or LAZY) from the line
                # Format: [Mode 3] Size  NN: DDDDD words, QQ queries → METHOD (reason)
                method=$(echo "$method_line" | sed -n 's/.*→ \([A-Z*]\+\) .*/\1/p' | head -1)
                if [ -z "$method" ]; then
                    method="MIXED"  # Multiple word sizes with different methods
                fi
            else
                method="UNKNOWN"
            fi
        fi
        
        # The output of 'time' is typically the last line
        stats=$(echo "$output" | tail -n 1)
        
        # Parse time and memory
        read time_val mem_val <<< "$stats"
        
        echo "$base_name/$time_val/$mem_val/$method" >> "$RESULTS_FILE"
        printf "%-25s Time=%7.2fs Memory=%7dKB Method=%s\n" "Processed $base_name" "$time_val" "$mem_val" "$method"
    else
        echo "Skipping $base_name: No dictionary found."
    fi
done

echo "Benchmark complete."
