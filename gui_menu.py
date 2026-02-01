#!/usr/bin/env python3
import os
import shlex
import subprocess
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
IS_WINDOWS = os.name == "nt"

DEFAULT_DICT = os.path.join("testing", "portugues.dict")
DEFAULT_PALS = os.path.join("testing", "Size07to15", "Size_07_01.pals")
DEFAULT_TEST_DIR = os.path.join("testing", "Size07to15")


def to_output_paths(pals_path: str) -> str:
    if pals_path.endswith(".pals"):
        return pals_path[:-5] + ".paths"
    return pals_path + ".paths"


def _win_to_wsl_path(path: str) -> str:
    drive, tail = os.path.splitdrive(os.path.abspath(path))
    if not drive:
        return path.replace("\\", "/")
    drive_letter = drive.replace(":", "").lower()
    tail = tail.replace("\\", "/")
    return f"/mnt/{drive_letter}{tail}"


def _wsl_to_win_path(path: str) -> str:
    if not path.startswith("/mnt/"):
        return path
    parts = path.split("/")
    if len(parts) < 4:
        return path
    drive_letter = parts[2].upper()
    rest = "\\".join(parts[3:])
    return f"{drive_letter}:\\{rest}"


def _to_wsl_arg(path: str) -> str:
    if os.path.isabs(path):
        return _win_to_wsl_path(path)
    return _win_to_wsl_path(os.path.join(SCRIPT_DIR, path))


def run_cmd(cmd, cwd=SCRIPT_DIR):
    if IS_WINDOWS:
        wsl_cwd = _win_to_wsl_path(cwd)
        cmd_str = " ".join(shlex.quote(part) for part in cmd)
        full_cmd = f"cd {shlex.quote(wsl_cwd)} && {cmd_str}"
        result = subprocess.run(["wsl", "bash", "-lc", full_cmd], capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr
    result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    return result.returncode, result.stdout, result.stderr


class App(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("WordMutations GUI")
        self.geometry("900x650")

        self.dict_var = tk.StringVar(value=DEFAULT_DICT)
        self.pals_var = tk.StringVar(value=DEFAULT_PALS)
        self.mode_var = tk.StringVar(value="Default")
        self.test_dir_var = tk.StringVar(value=DEFAULT_TEST_DIR)

        self._build_ui()

    def _build_ui(self):
        padding = {"padx": 8, "pady": 6}

        main = ttk.Frame(self)
        main.pack(fill=tk.BOTH, expand=True)

        inputs = ttk.LabelFrame(main, text="Run Program")
        inputs.pack(fill=tk.X, **padding)

        ttk.Label(inputs, text="Dictionary file").grid(row=0, column=0, sticky=tk.W, **padding)
        ttk.Entry(inputs, textvariable=self.dict_var, width=70).grid(row=0, column=1, sticky=tk.W, **padding)
        ttk.Button(inputs, text="Browse", command=self.browse_dict).grid(row=0, column=2, **padding)

        ttk.Label(inputs, text="Pairs file").grid(row=1, column=0, sticky=tk.W, **padding)
        ttk.Entry(inputs, textvariable=self.pals_var, width=70).grid(row=1, column=1, sticky=tk.W, **padding)
        ttk.Button(inputs, text="Browse", command=self.browse_pals).grid(row=1, column=2, **padding)

        ttk.Label(inputs, text="Optimization mode").grid(row=2, column=0, sticky=tk.W, **padding)
        mode_combo = ttk.Combobox(inputs, textvariable=self.mode_var, state="readonly", width=20)
        mode_combo["values"] = ("Default", "0", "1", "2", "3")
        mode_combo.grid(row=2, column=1, sticky=tk.W, **padding)

        ttk.Button(inputs, text="Run Program", command=self.run_program).grid(row=2, column=2, **padding)

        bench = ttk.LabelFrame(main, text="Benchmarks")
        bench.pack(fill=tk.X, **padding)

        ttk.Label(bench, text="Test directory").grid(row=0, column=0, sticky=tk.W, **padding)
        ttk.Entry(bench, textvariable=self.test_dir_var, width=70).grid(row=0, column=1, sticky=tk.W, **padding)
        ttk.Button(bench, text="Browse", command=self.browse_test_dir).grid(row=0, column=2, **padding)

        ttk.Button(bench, text="Run Benchmark", command=self.run_benchmark).grid(row=1, column=0, **padding)
        ttk.Button(bench, text="Compare all modes", command=self.compare_modes).grid(row=1, column=1, **padding)
        ttk.Button(bench, text="Compare mode 0 vs 3", command=self.compare_0_3).grid(row=1, column=2, **padding)

        output = ttk.LabelFrame(main, text="Output")
        output.pack(fill=tk.BOTH, expand=True, **padding)

        self.output_text = tk.Text(output, wrap=tk.NONE)
        self.output_text.pack(fill=tk.BOTH, expand=True)

    def browse_dict(self):
        path = filedialog.askopenfilename(initialdir=SCRIPT_DIR, filetypes=[("Dictionary", "*.dict"), ("All", "*.*")])
        if path:
            self.dict_var.set(self._relative_or_abs(path))

    def browse_pals(self):
        path = filedialog.askopenfilename(initialdir=SCRIPT_DIR, filetypes=[("Pairs", "*.pals"), ("All", "*.*")])
        if path:
            self.pals_var.set(self._relative_or_abs(path))

    def browse_test_dir(self):
        path = filedialog.askdirectory(initialdir=SCRIPT_DIR)
        if path:
            self.test_dir_var.set(self._relative_or_abs(path))

    def _relative_or_abs(self, path):
        try:
            rel = os.path.relpath(path, SCRIPT_DIR)
            if not rel.startswith(".."):
                return rel
        except ValueError:
            pass
        return path

    def _append_output(self, text):
        self.output_text.insert(tk.END, text)
        self.output_text.see(tk.END)

    def run_program(self):
        dict_file = self.dict_var.get().strip()
        pals_file = self.pals_var.get().strip()
        mode = self.mode_var.get().strip()

        if not dict_file or not pals_file:
            messagebox.showerror("Missing input", "Dictionary and pairs files are required.")
            return

        run_dict = _to_wsl_arg(dict_file) if IS_WINDOWS else dict_file
        run_pals = _to_wsl_arg(pals_file) if IS_WINDOWS else pals_file

        cmd = ["./wrdmttns", run_dict, run_pals]
        if mode != "Default":
            cmd.append(mode)

        self.output_text.delete("1.0", tk.END)
        code, out, err = run_cmd(cmd)
        if out:
            self._append_output(out)
        if err:
            self._append_output(err)

        if code != 0:
            messagebox.showerror("Run failed", "wrdmttns exited with an error. See output.")
            return

        output_file = to_output_paths(pals_file)
        if os.path.exists(os.path.join(SCRIPT_DIR, output_file)):
            output_file = os.path.join(SCRIPT_DIR, output_file)
        elif IS_WINDOWS:
            wsl_output = to_output_paths(run_pals)
            output_file = _wsl_to_win_path(wsl_output)

        if os.path.exists(output_file):
            self._append_output("\n--- Output file ---\n")
            with open(output_file, "r", encoding="utf-8", errors="replace") as fh:
                self._append_output(fh.read())
        else:
            self._append_output("\nOutput file not found: " + output_file + "\n")

    def run_benchmark(self):
        test_dir = self.test_dir_var.get().strip() or DEFAULT_TEST_DIR
        run_test_dir = _to_wsl_arg(test_dir) if IS_WINDOWS else test_dir
        self.output_text.delete("1.0", tk.END)
        code, out, err = run_cmd(["./benchmark.sh", run_test_dir])
        self._append_output(out)
        self._append_output(err)
        if code != 0:
            messagebox.showerror("Benchmark failed", "benchmark.sh exited with an error. See output.")

    def compare_modes(self):
        test_dir = self.test_dir_var.get().strip() or DEFAULT_TEST_DIR
        run_test_dir = _to_wsl_arg(test_dir) if IS_WINDOWS else test_dir
        self.output_text.delete("1.0", tk.END)
        code1, out1, err1 = run_cmd(["./compare_modes.sh", run_test_dir])
        code2, out2, err2 = run_cmd(["python3", "compare_results.py"])
        self._append_output(out1 + err1 + out2 + err2)
        if code1 != 0 or code2 != 0:
            messagebox.showerror("Compare failed", "Compare modes exited with an error. See output.")

    def compare_0_3(self):
        test_dir = self.test_dir_var.get().strip() or DEFAULT_TEST_DIR
        run_test_dir = _to_wsl_arg(test_dir) if IS_WINDOWS else test_dir
        folder_name = os.path.basename(test_dir.rstrip("/"))
        self.output_text.delete("1.0", tk.END)
        code0, out0, err0 = run_cmd(["./benchmark.sh", run_test_dir, "0"])
        code3, out3, err3 = run_cmd(["./benchmark.sh", run_test_dir, "3"])
        codec, outc, errc = run_cmd(["python3", "compare_benchmarks.py", f"{folder_name}_mode0.txt", f"{folder_name}_mode3.txt"])
        self._append_output(out0 + err0 + out3 + err3 + outc + errc)
        if code0 != 0 or code3 != 0 or codec != 0:
            messagebox.showerror("Compare failed", "Compare 0 vs 3 exited with an error. See output.")


if __name__ == "__main__":
    app = App()
    app.mainloop()
