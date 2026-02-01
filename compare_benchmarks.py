#!/usr/bin/env python3
import sys
import os

def load_results(filepath):
    results = {}
    try:
        with open(filepath, 'r') as f:
            for line in f:
                if line.startswith("Test Name"): continue
                parts = line.strip().split('/')
                if len(parts) >= 3:
                    name = parts[0].strip()
                    try:
                        time = float(parts[1])
                        mem = int(parts[2])
                        results[name] = {'time': time, 'mem': mem}
                    except ValueError:
                        continue
    except Exception as e:
        print(f"Error reading {filepath}: {e}")
        sys.exit(1)
    return results

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 compare_benchmarks.py <old_file> <new_file>")
        sys.exit(1)

    old_file = sys.argv[1]
    new_file = sys.argv[2]

    print(f"Comparing:\n OLD: {old_file}\n NEW: {new_file}\n")

    old_data = load_results(old_file)
    new_data = load_results(new_file)

    print(f"{'Test Name':<25} | {'Old(s)':<8} | {'New(s)':<8} | {'Speedup':<8} | {'Old(KB)':<8} | {'New(KB)':<8} | {'Diff(KB)':<8}")
    print("-" * 105)

    total_old = 0
    total_new = 0
    
    all_tests = sorted(set(old_data.keys()) | set(new_data.keys()))

    for test in all_tests:
        old = old_data.get(test)
        new = new_data.get(test)
        
        t_old_str = f"{old['time']:.4f}" if old else "N/A"
        t_new_str = f"{new['time']:.4f}" if new else "N/A"
        
        speedup_str = "N/A"
        if old and new:
            if new['time'] > 0:
                speedup = old['time'] / new['time']
                speedup_str = f"{speedup:.2f}x"
            elif old['time'] == 0:
                speedup_str = "1.00x"
            
            total_old += old['time']
            total_new += new['time']

        m_old_str = str(old['mem']) if old else "N/A"
        m_new_str = str(new['mem']) if new else "N/A"
        
        m_diff_str = "N/A"
        if old and new:
            diff = new['mem'] - old['mem']
            m_diff_str = f"{diff:+d}"

        print(f"{test:<25} | {t_old_str:<8} | {t_new_str:<8} | {speedup_str:<8} | {m_old_str:<8} | {m_new_str:<8} | {m_diff_str:<8}")

    print("-" * 105)
    if total_new > 0:
        global_speedup = total_old / total_new
        print(f"{'TOTAL':<25} | {total_old:<8.4f} | {total_new:<8.4f} | {global_speedup:<8.2f}x |")

if __name__ == "__main__":
    main()
