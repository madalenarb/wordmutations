#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

prompt_path() {
    local label="$1"
    local default_path="$2"
    local value
    read -r -p "$label [$default_path]: " value
    if [ -z "$value" ]; then
        echo "$default_path"
    else
        echo "$value"
    fi
}

run_program() {
    local dict_file
    local pals_file
    local mode

    dict_file=$(prompt_path "Dictionary file" "testing/portugues.dict")
    pals_file=$(prompt_path "Pairs file" "testing/Size07to15/Size_07_01.pals")
    read -r -p "Optimization mode (0-3, blank for default): " mode

    if [ -n "$mode" ]; then
        ./wrdmttns "$dict_file" "$pals_file" "$mode"
    else
        ./wrdmttns "$dict_file" "$pals_file"
    fi
}

run_benchmark() {
    local test_dir
    local mode

    test_dir=$(prompt_path "Test directory" "testing/Size07to15")
    read -r -p "Optimization mode (0-3, blank for default): " mode

    if [ -n "$mode" ]; then
        ./benchmark.sh "$test_dir" "$mode"
    else
        ./benchmark.sh "$test_dir"
    fi
}

compare_modes() {
    local test_dir
    test_dir=$(prompt_path "Test directory" "testing/Size07to15")
    chmod +x compare_modes.sh
    ./compare_modes.sh "$test_dir"
    python3 compare_results.py
}

compare_0_3() {
    local test_dir
    test_dir=$(prompt_path "Test directory" "testing/Size07to15")
    ./benchmark.sh "$test_dir" 0
    ./benchmark.sh "$test_dir" 3
    local folder_name
    folder_name="$(basename "${test_dir%/}")"
    python3 compare_benchmarks.py "${folder_name}_mode0.txt" "${folder_name}_mode3.txt"
}

while true; do
    echo ""
    echo "WordMutations Menu"
    echo "1) Run program"
    echo "2) Run benchmark"
    echo "3) Compare all modes"
    echo "4) Compare mode 0 vs 3"
    echo "5) Exit"
    read -r -p "Select an option: " choice

    case "$choice" in
        1) run_program ;;
        2) run_benchmark ;;
        3) compare_modes ;;
        4) compare_0_3 ;;
        5) exit 0 ;;
        *) echo "Invalid option." ;;
    esac

done
