"""Data loading utilities for nucleation analysis."""
from typing import Any

import numpy as np
from pathlib import Path


def load_nucleation_results(result_file: str | Path) -> np.ndarray:
    """
    Load nucleation rate data from a CSV file.

    Args:
        result_file: Path to the -case-bin-results.csv file

    Returns:
        numpy array with shape (n_measurements, 2) where:
        - Column 0: Temperature in Celsius
        - Column 1: Nucleation rate [m^-3 s^-1]
    """
    result_file = Path(result_file)
    if not result_file.exists():
        raise FileNotFoundError(f"Result file not found: {result_file}")

    data = np.loadtxt(result_file, delimiter=";")

    # Convert temperature from Celsius to Kelvin
    data[:, 0] += 273.15

    return data


def load_droplets_data(droplets_file: str | Path) -> np.ndarray:
    """
    Load droplet data from a CSV file.

    Args:
        droplets_file: Path to the -droplets.csv file

    Returns:
        numpy array with droplet measurements
    """
    droplets_file = Path(droplets_file)
    if not droplets_file.exists():
        raise FileNotFoundError(f"Droplets file not found: {droplets_file}")

    data = np.loadtxt(droplets_file, delimiter=";", skiprows=1)

    # Convert temperature from Celsius to Kelvin
    data[:, 0] += 273.15

    return data


def load_overview_file(overview_file: str | Path) -> np.ndarray:
    """
    Load nucleation measurement data from multiple CSV files via an overview file.

    This function reads a CSV overview file containing paths to individual measurement
    files and temperature values, then combines all data into a single array.

    Args:
        overview_file: Path to overview TOML file with measurement_file and temperature entries

    Returns:
        - np.ndarray: Combined nucleation data with shape (n_total, n_cols + 2) where:
            - Columns 0 to n_cols-2: Original measurement columns (including temperature in K, nucleation_rate, etc.)
            - Column n_cols-1: Measurement ID
            - Column n_cols: Group temperature in K
    """
    overview_file = Path(overview_file)
    if not overview_file.exists():
        raise FileNotFoundError(f"Overview file not found: {overview_file}")

    overview = np.genfromtxt(
        overview_file,
        delimiter=';',
        dtype=None,
        names=True,
        encoding="utf-8"
    )

    nucleation_files = []
    measurement_id = 0

    for nucleation in overview:
        # Get the results file (convert from -case.cf to -case-bin-results.csv)
        measurement_file = nucleation['measurement_file']
        if isinstance(measurement_file, np.bytes_):
            measurement_file = measurement_file.decode('utf-8')

        result_file = measurement_file.replace("-case.cf", "-case-bin-results.csv", 1)

        # Load nucleation data
        nucleation_data = np.loadtxt(result_file, delimiter=';')

        # Add ID and temperature columns
        id_arr = np.full((nucleation_data.shape[0], 1), measurement_id)
        temp_high_arr = np.full((nucleation_data.shape[0], 1), nucleation["temperature"])

        out = np.hstack((nucleation_data, id_arr, temp_high_arr))
        nucleation_files.append(out)
        measurement_id += 1

    # Stack all data and convert temperatures to Kelvin
    combined_data = np.vstack(nucleation_files)
    combined_data[:, [0, 3]] += 273.15

    return combined_data

