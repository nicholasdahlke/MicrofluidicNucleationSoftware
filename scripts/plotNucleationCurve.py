#!/usr/bin/env python3
"""
Plot nucleation rate curves with fits for multiple measurements.

This script loads nucleation data from multiple experiments and plots them
with fitted curves based on classical nucleation theory.
"""

import numpy as np
import sys
from pathlib import Path

# Import configuration and analysis modules

from analysis import get_linear_fit_parameters
from config.constants import K_B, COLOR_ORANGE, COLOR_BLUE, DEFAULT_FIGSIZE, SYMBOL_SIZE
from config.paths import get_result_file, get_output_dir, set_output_dir
from analysis.data_loader import load_nucleation_results, load_overview_file
from plotting.plotting_utils import (
    setup_plot_style,
    create_figure,
    save_figure,
    get_color,
    get_marker_cycle,
    FONT_CONFIG_STANDARD,
)


def main():
    """Main function to generate nucleation curve plot."""

    # Setup
    setup_plot_style(FONT_CONFIG_STANDARD)

    # Define overview file
    overview_file = Path("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/file_overview.csv")

    # Set output directory
    set_output_dir("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/analysis_ouput")

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
            delta_g, ln_n0, _ = get_linear_fit_parameters(temperatures, nucleation_rates)
            fit_parameters.append((delta_g, ln_n0, meas_id, measurement_temp))
        except Exception as e:
            print(f"Warning: Could not fit measurement {meas_id}: {e}")

    fit_parameters = np.array(fit_parameters)

    # Create figure and markers for plotting
    fig, ax = create_figure(DEFAULT_FIGSIZE)
    markers = get_marker_cycle(len(data))

    for id in measurement_ids:
        subset = data[data[:, 2] == id]
        subset[:, [0, 3]] = subset[:, [0, 3]] - 273.15
        temperature = subset[0, 3]
        # Select color based on temperature
        color = get_color(temperature=temperature)
        marker = next(markers)

        # Plot data points
        ax.scatter(
            subset[:, 0],
            subset[:, 1],
            color=color,
            marker=marker,
            s=SYMBOL_SIZE,
        )

        # Plot fit curves
        fit_t = np.linspace(subset[:, 0].min(), subset[:, 0].max(), 100)
        fit_n_dot = np.exp(fit_parameters[id][1])*np.exp(-fit_parameters[id][0]/(K_B*(fit_t+273.15)))
        ax.plot(
            fit_t,
            fit_n_dot,
            color=color,
            linestyle="--",
        )

    # Configure plot appearance
    ax.scatter([], [], color=COLOR_ORANGE, label=r"$T_h = 70^\circ\text{C}$", s=SYMBOL_SIZE)
    ax.scatter([], [], color=COLOR_BLUE, label=r"$T_h = 30^\circ\text{C}$", s=SYMBOL_SIZE)

    ax.set_ylim((1e8, 1e11))
    ax.set_yscale("log")
    ax.set_xlabel(r"$T_c$  [°C]")
    ax.set_ylabel(r"$\dot{n}\ [\text{m}^{-3} \text{s}^{-1}]$")
    ax.legend()

    # Save figure
    output_path = save_figure(fig, "nucleation_curve.pdf")
    print(f"Plot saved to: {output_path}")

    # Display plots
    try:
        import matplotlib.pyplot as plt
        plt.show()
    except:
        pass


if __name__ == "__main__":
    main()

