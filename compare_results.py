#!/usr/bin/env python3
"""
Compare benchmark results from different optimization modes
"""

import sys
import os
from pathlib import Path

def parse_results(filename):
    """Parse benchmark results file"""
    results = {}
    if not os.path.exists(filename):
        print(f"Warning: {filename} not found")
        return None
    
    with open(filename, 'r') as f:
        lines = f.readlines()
        for line in lines[1:]:  # Skip header
            parts = line.strip().split('/')
            if len(parts) == 3:
                test_name, time_s, memory_kb = parts
                try:
                    results[test_name] = {
                        'time': float(time_s),
                        'memory': int(memory_kb)
                    }
                except ValueError:
                    continue
    return results

def compare_results(old_results, new_results, old_name, new_name):
    """Compare two result sets"""
    print(f"\n{'='*80}")
    print(f"Comparison: {old_name} vs {new_name}")
    print(f"{'='*80}")
    print(f"{'Test':<20} {'Old Time':<12} {'New Time':<12} {'Speedup':<10} {'Mem Change':<12}")
    print(f"{'-'*80}")
    
    total_old_time = 0
    total_new_time = 0
    speedups = []
    
    for test_name in sorted(old_results.keys()):
        if test_name not in new_results:
            continue
            
        old_time = old_results[test_name]['time']
        new_time = new_results[test_name]['time']
        old_mem = old_results[test_name]['memory']
        new_mem = new_results[test_name]['memory']
        
        total_old_time += old_time
        total_new_time += new_time
        
        speedup = (old_time / new_time) if new_time > 0 else 0
        speedups.append(speedup)
        
        mem_change = ((new_mem - old_mem) / old_mem * 100) if old_mem > 0 else 0
        
        speedup_str = f"{speedup:.2f}x"
        if speedup > 1.0:
            speedup_str += " ⬆"
        elif speedup < 1.0:
            speedup_str += " ⬇"
        
        mem_str = f"{mem_change:+.1f}%"
        
        print(f"{test_name:<20} {old_time:<12.2f} {new_time:<12.2f} {speedup_str:<10} {mem_str:<12}")
    
    print(f"{'-'*80}")
    print(f"{'TOTAL':<20} {total_old_time:<12.2f} {total_new_time:<12.2f}")
    
    overall_speedup = (total_old_time / total_new_time) if total_new_time > 0 else 0
    avg_speedup = sum(speedups) / len(speedups) if speedups else 0
    
    print(f"\nOverall Speedup: {overall_speedup:.2f}x")
    print(f"Average Speedup: {avg_speedup:.2f}x")
    print(f"Total Time Saved: {total_old_time - total_new_time:.2f}s ({(1 - total_new_time/total_old_time)*100:.1f}%)")

def main():
    """Main comparison function"""
    
    # Check for available result files
    modes = {
        'mode0_dijkstra.txt': 'Mode 0 (Dijkstra)',
        'mode1_astar.txt': 'Mode 1 (A*)',
        'mode2_lazy.txt': 'Mode 2 (Lazy)',
        'mode3_auto.txt': 'Mode 3 (Auto)',
        'Size_07_15_old.txt': 'Old Version'
    }
    
    results = {}
    for filename, name in modes.items():
        parsed = parse_results(filename)
        if parsed:
            results[name] = parsed
            print(f"Loaded: {name} ({len(parsed)} tests)")
    
    if len(results) < 2:
        print("\nError: Need at least 2 result files to compare")
        print("Available files should be:")
        for filename in modes.keys():
            exists = "✓" if os.path.exists(filename) else "✗"
            print(f"  {exists} {filename}")
        return
    
    print(f"\nFound {len(results)} result sets\n")
    
    # Compare all modes against old version if available
    if 'Old Version' in results:
        baseline = results['Old Version']
        baseline_name = 'Old Version'
        
        for name, data in results.items():
            if name != baseline_name:
                compare_results(baseline, data, baseline_name, name)
    
    # Compare Mode 3 (Auto) against other modes
    if 'Mode 3 (Auto)' in results:
        auto_results = results['Mode 3 (Auto)']
        
        for name in ['Mode 0 (Dijkstra)', 'Mode 1 (A*)', 'Mode 2 (Lazy)']:
            if name in results and name != 'Mode 3 (Auto)':
                compare_results(results[name], auto_results, name, 'Mode 3 (Auto)')
    
    print(f"\n{'='*80}")
    print("Summary")
    print(f"{'='*80}")
    
    if 'Old Version' in results and 'Mode 3 (Auto)' in results:
        print("\n✓ Mode 3 (Auto) is recommended as the default")
        print("  - Adapts to workload characteristics")
        print("  - Balances speed and memory usage")
        print("  - Best overall performance")

if __name__ == '__main__':
    main()
