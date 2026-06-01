import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter
import os
from scipy.optimize import curve_fit
color_blue = "#5ca7c2"

plt.rcParams.update({
    "font.family": "serif",
    "text.usetex": True,
    "pgf.rcfonts": False,
    "font.size": 20,
    "text.latex.preamble": r"\usepackage{amsmath} \usepackage{amssymb} \usepackage{siunitx}",
})

def find_case_bin_results(folder_path):
    """
    Recursively searches for files ending in '-case-bin-results.csv'
    under the given folder and returns a list of their full paths.
    """
    result_files = []

    for root, dirs, files in os.walk(folder_path):
        for filename in files:
            if filename.endswith('-case-bin-results.csv'):
                full_path = os.path.join(root, filename)
                result_files.append(os.path.abspath(full_path))

    return result_files

def plot_bin_file(in_path, axes):
    data = np.loadtxt(in_path, delimiter=';')
    if np.isnan(data).any():
        return
    fit = curve_fit(lambda x, p0, p1: p0  - (p1/x), data[:,0], np.log(data[:,1]))[0]
    fit_x = np.linspace(min(data[:,0]), max(data[:,0]), 100)
    axes.plot(fit_x, np.exp(fit[0]) * np.exp(-fit[1]/fit_x), lw=1.5, c="black", zorder=0)
    axes.scatter(data[:,0], data[:,1], zorder=1, c=color_blue, s=60)

figsize = (6,4.5)
fig, ax = plt.subplots(layout="constrained")
fig.set_size_inches(figsize)
ax.yaxis.set_major_formatter(ScalarFormatter())
ax.yaxis.set_minor_formatter(ScalarFormatter())

#in_files = find_case_bin_results('/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp')
in_files = ["/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/5-1- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -22 - -24 In 9Min- 22-6 Raumtemp- 17-37 Messung Fertig- 47-81955 Framerate-03052025172613-0000_mirrored-case-bin-results.csv"]

for file in in_files:
    plot_bin_file(file, ax)

#ax.ticklabel_format(style='sci', axis='y', scilimits=(0,0))

ax.set_ylim((2.5e9, 1.2e10))
#ax.set_yscale("log")
ax.set_xlabel(r"$T_c$  [°C]")
ax.set_ylabel(r"$\dot{n}\ [\text{m}^{-3} \text{s}^{-1}]$")
ax.locator_params(axis='y')
plt.savefig("/home/nicholas/single_bin.pdf")

plt.show()