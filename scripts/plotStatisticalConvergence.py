import matplotlib.pyplot as plt
import numpy as np
import toml
import os
from scipy.optimize import curve_fit
import itertools
import matplotlib as mpl
inputs = []
inputs.append("/home/nicholas/testvideo/700Ul Oel; 10Ul Wasser; 30 Warm; -39 Kalt; 47,8196 Framerate-11222024174140-0000-case.cf")

num_warm = 1
num_cold = 0
temps = [70]*num_warm + [30]*num_cold


color_blue = "#5ca7c2"
color_orange = "#ed8b00"
param = []
figsize = (11.38, 8.67)
#plt.rcParams["font.size"] = 35
#fig, ax = plt.subplots(layout="constrained")
#fig.set_size_inches(figsize)


if __name__ == "__main__":
    markers = itertools.cycle((',', '+', '.', 'o', '*'))
    for file, temp in zip(inputs, temps):
        file = file.replace("-case.cf", "-droplets.csv", 1)
        if not os.path.isfile(file):
            raise FileNotFoundError("The provided file '{}' does not exist.".format(file))

        result_nucleation = np.loadtxt(file, delimiter=";", skiprows=1)
        result_nucleation[:,0] += 273.15

        color = color_blue
        marker = next(markers)
        if temp > 50:
            color = color_orange

        p_result = []
        unfrozen = 0
        total = 0
        for row in result_nucleation:
            print(row)
            total += 1
            if row[1] != 1.0:
                unfrozen += 1
            p_result.append(unfrozen/total)
        plt.plot(p_result, color=color)




    plt.show()
