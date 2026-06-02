#!/usr/bin/env python3
""""
Uses the Wilcoxon rank sum test to test for the mpemba effect in the data
"""

import numpy as np
import sys
from scipy.stats import mannwhitneyu
from sklearn.decomposition import PCA

# Import configuration and analysis modules
from config.paths import *
from analysis.data_loader import load_overview_file
from analysis.nucleation_analysis import get_linear_fit_parameters
from plotting.plotting_utils import *


def main():
    """Main function to generate statistical analysis plot."""

    # Define overview file
    overview_file = Path("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/file_overview.csv")

    if not overview_file.exists():
        print(f"Error: Overview file not found at {overview_file}")
        print("Please create a file_overview.csv with measurement files and temperatures")
        sys.exit(1)

    # Load data
    try:
        data = load_overview_file(overview_file)
    except Exception as e:
        print(f"Error loading overview file: {e}")
        sys.exit(1)

    # Extract unique measurement IDs
    measurement_ids = range(int(np.max(data[:, 2])) + 1)

    # Compute fit parameters for each measurement
    fit_parameters = []
    for meas_id in measurement_ids:
        subset = data[data[:, 2] == meas_id]
        temperatures = subset[:, 0]
        nucleation_rates = subset[:, 1]
        measurement_temp = subset[0, 3]

        try:
            delta_g, ln_j0, _ = get_linear_fit_parameters(temperatures, nucleation_rates)
            fit_parameters.append((delta_g, ln_j0, meas_id, measurement_temp))
        except Exception as e:
            print(f"Warning: Could not fit measurement {meas_id}: {e}")

    fit_parameters = np.array(fit_parameters)

    # Use PCA to project the parameter space onto a one dimensional subspace
    pca = PCA(n_components=1)
    reduced_fit_parameters = pca.fit_transform(fit_parameters)

    # Use the Wilcoxon test
    warm = np.unique(data[data[:, 3] == 70 + 273.15, 2]).astype(np.int_)
    cold = np.unique(data[data[:, 3] == 30 + 273.15, 2]).astype(np.int_)
    pvalue = mannwhitneyu(
        reduced_fit_parameters[warm],
        reduced_fit_parameters[cold],
        alternative="greater",
        method="auto"
    ).pvalue[0]

    # Print formatted results
    print(f"\n{'Mpemba Effect Analysis':^40}")
    print(f"{'-'*40}")
    print(f"Test: Mann-Whitney U Test")
    print(f"P-value: {pvalue:.2%}")
    print(f"Warm samples (n={len(warm)})")
    print(f"Cold samples (n={len(cold)})")
    print(f"Result: {'Mpemba effect detected ✓' if pvalue < 0.05 else 'No significant effect'}")


if __name__ == "__main__":
    main()
