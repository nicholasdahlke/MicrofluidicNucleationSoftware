#!/usr/bin/env python3
"""
Statistical analysis of nucleation data with confidence ellipses.

Analyzes nucleation measurements with classical nucleation theory fitting
and plots confidence regions in parameter space.
"""

import numpy as np
import sys

# Import configuration and analysis modules
from config.paths import *
from config.constants import K_B, DEFAULT_FIGSIZE, COLOR_ORANGE, COLOR_BLUE
from analysis.data_loader import load_overview_file
from analysis.nucleation_analysis import get_linear_fit_parameters
from analysis.nucleation_analysis import get_confidence_ellipse
from plotting.plotting_utils import *
import matplotlib.pyplot as plt


def main():
    """Main function to generate statistical analysis plot."""
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
            delta_g, ln_j0, _ = get_linear_fit_parameters(temperatures, nucleation_rates)
            fit_parameters.append((delta_g, ln_j0, meas_id, measurement_temp))
        except Exception as e:
            print(f"Warning: Could not fit measurement {meas_id}: {e}")

    fit_parameters = np.array(fit_parameters)

    # Compute confidence ellipses for each measurement
    confidence_ellipses = []
    for meas_id in measurement_ids:
        subset = data[data[:, 2] == meas_id]
        temperatures = subset[:, 0]
        nucleation_rates = subset[:, 1]

        try:
            ellipse = get_confidence_ellipse(temperatures, nucleation_rates)
            confidence_ellipses.append(ellipse)
        except Exception as e:
            print(f"Warning: Could not compute ellipse for measurement {meas_id}: {e}")

    # Create plots
    fig1, ax1 = create_figure(figsize=DEFAULT_FIGSIZE)
    fig2, ax2 = create_figure(figsize=DEFAULT_FIGSIZE)

    # Plot 1: Nucleation rate vs temperature
    ax1.scatter(data[:, 0] - 273.15, data[:, 1])
    ax1.set_xlabel("$T_c$ [°C]")
    ax1.set_ylabel(r"$\dot{n}$ [m$^{-3}$ s$^{-1}$]")
    ax1.set_title("Nucleation rates")
    ax1.set_yscale("log")

    # Plot 2: Parameter space with confidence ellipses
    ax2.scatter(fit_parameters[:, 0], fit_parameters[:, 1])
    ax2.set_xlabel(r"$\Delta G$ [J]")
    ax2.set_ylabel(r"$\ln(\dot{n}_0)$ [m$^{-3}$ s$^{-1}$]")
    ax2.set_title("Fit parameter space with confidence regions")

    # Plot confidence ellipses, coloring by temperature
    for ellipse, fit_param in zip(confidence_ellipses, fit_parameters):
        # Color by measurement temperature
        color = COLOR_ORANGE if fit_param[3] > 323.15 else COLOR_BLUE  # 50°C threshold in K
        ax2.plot(ellipse[:, 0], ellipse[:, 1], color=color, alpha=0.6, linewidth=1.5)

    # Save figure
    output_path1 = save_figure(fig1, "nucleation_stat_analysis1.pdf")
    output_path2 = save_figure(fig2, "nucleation_stat_analysis2.pdf")

    print(f"Statistical analysis plots saved to: {output_path1, output_path2}")

    # Print summary statistics
    print(f"\n{'='*60}")
    print("Statistical Analysis Summary")
    print(f"{'='*60}")
    print(f"Total measurements analyzed: {len(fit_parameters)}")
    print(f"\nFit Parameters (ΔG in kJ/mol, ln(J₀) in m⁻³s⁻¹):")
    print(f"{'ID':<5} {'ΔG [J]':<15} {'ln(ṅ₀)':<15} {'T_meas (°C)':<15}")
    print("-" * 60)

    for fit_param in fit_parameters:
        print(f"{int(fit_param[2]):<5} {fit_param[0]:<15.2e} {fit_param[1]:<15.2e} {fit_param[3]-273.15:<15.1f}")

    # Display plots
    plt.show()


if __name__ == "__main__":
    main()
