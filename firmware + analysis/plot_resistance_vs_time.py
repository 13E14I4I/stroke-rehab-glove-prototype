"""
plot_resistance_vs_time.py

Reads all CSV files in the sample_data folder and generates
a resistance-vs-time plot for each trial.

Expected CSV format:
time_s,voltage_v,resistance_ohms
0.00,3.75,3011.76
1.00,3.17,1735.29
"""

from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt


def plot_resistance_vs_time(csv_path: Path, output_folder: Path) -> None:
    """
    Reads one CSV file and saves a resistance vs. time plot.
    """

    # Read CSV file
    df = pd.read_csv(csv_path)

    # Make sure required columns exist
    required_columns = {"time_s", "resistance_ohms"}
    missing_columns = required_columns - set(df.columns)

    if missing_columns:
        print(f"Skipping {csv_path.name}: missing columns {missing_columns}")
        return

    # Convert values to numeric in case anything was read as text
    df["time_s"] = pd.to_numeric(df["time_s"], errors="coerce")
    df["resistance_ohms"] = pd.to_numeric(df["resistance_ohms"], errors="coerce")

    # Drop rows with invalid/missing numeric data
    df = df.dropna(subset=["time_s", "resistance_ohms"])

    if df.empty:
        print(f"Skipping {csv_path.name}: no valid data found")
        return

    # Create plot
    plt.figure(figsize=(8, 5))
    plt.plot(df["time_s"], df["resistance_ohms"], marker="o")

    plt.title(f"Resistance vs. Time: {csv_path.stem}")
    plt.xlabel("Time (s)")
    plt.ylabel("Resistance (ohms)")
    plt.grid(True)
    plt.tight_layout()

    # Save plot
    output_path = output_folder / f"{csv_path.stem}_resistance_vs_time.png"
    plt.savefig(output_path, dpi=300)
    plt.close()

    print(f"Saved plot: {output_path}")


def main() -> None:
    # Folder containing trial CSV files
    data_folder = Path("sample_data")

    # Folder where plots will be saved
    output_folder = Path("results")
    output_folder.mkdir(exist_ok=True)

    # Find all CSV files in sample_data
    csv_files = sorted(data_folder.glob("*.csv"))

    if not csv_files:
        print("No CSV files found in sample_data folder.")
        return

    # Plot each file
    for csv_path in csv_files:
        plot_resistance_vs_time(csv_path, output_folder)


if __name__ == "__main__":
    main()