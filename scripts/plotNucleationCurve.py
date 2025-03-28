import matplotlib.pyplot as plt
import numpy as np
import toml
import os
from scipy.optimize import curve_fit
import itertools

inputs = []
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/3- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -20 - -21 In 4 Min Danach Stable Auf -21- 22-8 Raumtemp-15-08 Messung Fertig- 47-81955 Framerate-03052025145632-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/6- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -22 - -24 In 10Min- 22-0 Raumtemp-18-36 Messung Fertig- 47-81955 Framerate-03052025182331-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/5-1- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -22 - -24 In 9Min- 22-6 Raumtemp- 17-37 Messung Fertig- 47-81955 Framerate-03052025172613-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/4- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -21 - -23 In 8Min- 23 Raumtemp- 16-06 Messung Fertig- 47-81955 Framerate-03052025155701-0000_mirrored-case.cf")


if __name__ == "__main__":
    marker = itertools.cycle(("tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple", "tab:brown", "tab:pink", "tab:gray", "tab:olive", "tab:cyan"))
    for file in inputs:
        with open(file) as f:
            data = toml.loads(f.read())
        file = file.replace("-case.cf", "-case-bin-results.csv", 1)
        if not os.path.isfile(file):
            raise FileNotFoundError("The provided file '{}' does not exist.".format(file))

        result_nucleation = np.loadtxt(file, delimiter=";")

        color = next(marker)
        plt.scatter(result_nucleation[:, 0], result_nucleation[:, 1], color=color)

        fit_x = np.linspace(result_nucleation[:, 0].min(), result_nucleation[:, 0].max(), 50)
        try:
            fit = curve_fit(lambda x, p0, p1: p0 * np.exp(p1*x), result_nucleation[:,0], result_nucleation[:,1], [1.1e-7, -1.5])[0]
            plt.plot(fit_x, fit[0]*np.exp(fit_x*fit[1]), color=color, label="Fit")
        except:
            print("Could not fit data")



    plt.yscale("log")
    plt.show()
