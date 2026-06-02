#!/usr/bin/env python3
"""
Plot nucleation temperature dependence
"""

import numpy as np

from analysis import get_linear_fit_parameters
# Import configuration and analysis modules
from config.paths import *
from config.constants import DEFAULT_FIGSIZE, COLOR_BLUE, K_B
from plotting.plotting_utils import *
import matplotlib.pyplot as plt


def main():
    """Main function to plot the temperature dependence for a single measurement"""
    # Setup
    setup_plot_style(FONT_CONFIG_STANDARD)

    # Set output directory
    set_output_dir("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/analysis_ouput")

    # Read the droplet file
    case_file = "/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/5-1- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -22 - -24 In 9Min- 22-6 Raumtemp- 17-37 Messung Fertig- 47-81955 Framerate-03052025172613-0000_mirrored-case-bin-results.csv"
    bin_results_file = get_result_file(case_file)
    bin_data = np.loadtxt(bin_results_file, delimiter=";", skiprows=1)

    # Plot the result
    fig, ax = create_figure(DEFAULT_FIGSIZE)
    ax.scatter(bin_data[:, 0], bin_data[:, 1], color=COLOR_BLUE, marker="o", s=40)

    # Fit the data and plot the resulting curve
    delta_g, ln_n0, _ = get_linear_fit_parameters(bin_data[:, 0]+273.15, bin_data[:, 1])

    fit_t = np.linspace(bin_data[:, 0].min(), bin_data[:, 0].max(), 100)
    fit_n_dot = np.exp(ln_n0) * np.exp(-delta_g / (K_B * (fit_t + 273.15)))
    ax.plot(
        fit_t,
        fit_n_dot,
        color="black",
        linestyle="--",
    )

    # Configure the plot
    ax.set_xlabel(r"$T_c$  [°C]")
    ax.set_ylabel(r"$\dot{n}\ [\text{m}^{-3} \text{s}^{-1}]$")
    ax.locator_params(axis='y')

    # Save the result
    output_path = save_figure(fig, "nucleation_temp_ramp.pdf")
    print(f"Temperature dependence plot saved to: {output_path}")

    # Display plot
    plt.show()


if __name__ == "__main__":
    main()
