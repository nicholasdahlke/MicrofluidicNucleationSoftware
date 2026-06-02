#!/usr/bin/env python3
"""
Plot statistical convergence for a single measurement
"""

import numpy as np

# Import configuration and analysis modules
from config.paths import *
from config.constants import DEFAULT_FIGSIZE, COLOR_BLUE
from plotting.plotting_utils import *
import matplotlib.pyplot as plt


def main():
    """Main function to generate the statistical convergence plot for a single measurement"""
    # Setup
    setup_plot_style(FONT_CONFIG_STANDARD)

    # Set output directory
    set_output_dir("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/analysis_ouput")

    # Read the droplet file
    case_file = "/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000-case.cf"
    droplets_file = get_droplets_file(case_file)
    droplet_data = np.loadtxt(droplets_file, delimiter=";", skiprows=1)

    # Calculate the statistical convergence curve
    p_result = []
    unfrozen = 0
    total = 0
    for row in droplet_data:
        total += 1
        if row[1] != 1.0:
            unfrozen += 1
        p_result.append(unfrozen / total)

    # Plot the result
    fig, ax = create_figure(DEFAULT_FIGSIZE)

    ax.plot(p_result, color=COLOR_BLUE, linestyle="-", marker="o", markersize=5)

    ax.set_ylabel(r"$p$")
    ax.set_xlabel("Droplet")

    # Save the result
    output_path = save_figure(fig, "nucleation_stat_convergence.pdf")
    print(f"Statistical convergence plotsaved to: {output_path}")

    # Display plot
    plt.show()


if __name__ == "__main__":
    main()
