import matplotlib.pyplot as plt
import numpy as np
import toml
import os
from scipy.optimize import curve_fit
from scipy.stats import linregress
import itertools
import matplotlib as mpl
inputs = []
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/22.03.25/18- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -22-5 - -24-5 In 10Min Kalt- 23-6 Raumtemp- 17-02 Messung Fertig- 47-81955 Framerate-03222025165618-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/22.03.25/19- 700L Oel- 10Ul Wasser- 70 Warm- Rampe -22-5 - -24-5 In 10Min Kalt- 25-2 Raumtemp- 18-44 Messung Fertig- 47-81955 Framerate-03222025183855-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/22.03.25/20- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -22 - -24 In 10Min Kalt- 26-9 Raumtemp- 20-08  Messung Fertig- 47-81955 Framerate-03222025195804-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/21.03.25/15- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -22 Auf -24 In 10Min- 24-5 Raumtemp- 20-59 Messung Fertig- 47-81955 Framerate-03212025205040-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/21.03.25/16- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -22 Auf -24 In 10Min- 24-6 Raumtemp- 21-51 Messung Fertig- 47-81955 Framerate-03212025214019-0000-case.cf")
#zu kurz ->inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/23.03.25/21- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -22 - -24 In 10Min Kalt- 26-2 Raumptemp- 17-16 Messung Fertig- 47-81955 Framerate-03232025171337-0000-case.cf")
#zu kurz ->inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/23.03.25/22- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -21-5 - -23-5 In 10Min Kalt- 26-7 Raumptemp- 18-07 Messung Fertig- 47-81955 Framerate-03232025180509-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/23.03.25/23- 700Ul Oel- 10Ul Wasser- 70 Warm- Rampe -21-5 - -23-5 In 10Min Kalt-26-8 Raumptemp- 19-08 Messung Fertig- 47-81955 Framerate-03232025190102-0000-case.cf")


#inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/3- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -20 - -21 In 4 Min Danach Stable Auf -21- 22-8 Raumtemp-15-08 Messung Fertig- 47-81955 Framerate-03052025145632-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/6- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -22 - -24 In 10Min- 22-0 Raumtemp-18-36 Messung Fertig- 47-81955 Framerate-03052025182331-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/5-1- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -22 - -24 In 9Min- 22-6 Raumtemp- 17-37 Messung Fertig- 47-81955 Framerate-03052025172613-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/05.03.25/4- 700Ul Oel- 20Ul Wasser - 30 Warm- Ramp- -21 - -23 In 8Min- 23 Raumtemp- 16-06 Messung Fertig- 47-81955 Framerate-03052025155701-0000_mirrored-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/14.03.25/14- 700 Ul Oel- 10Ul Wasser- 30 Warm- Rampe -22 - -24 In 10Min Kalt- 20-7 Raumtemp- 16-13 Messung Fertig- 47-81955 Framerate-03142025160022-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/06.03.25- mit Öl Glasspritze/7- 700Ul Oel- 20Ul Wasser- 30 Warm- Ramp- -22 - -24 In 10Min - 23-7 Raumtemp- 16-06 Messung Fertig- 47-81955 Framerate-03062025155328-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/06.03.25- mit Öl Glasspritze/8- 700Ul Oel- 20Ul Wasser- 30 Warm- Ramp- -23 - -24 In 10Min - 23-3 Raumtemp- 16-59 Messung Fertig- 47-81955 Framerate-03062025164610-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/06.03.25- mit Öl Glasspritze/9- 700Ul Oel- 20Ul Wasser- 30 Warm- Ramp- -23 - -24 In 10Min - 23-4 Raumtemp- 18-18 Messung Fertig- 47-81955 Framerate-03062025180650-0000-case.cf")
#inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/06.03.25- mit Öl Glasspritze/10- 700Ul Oel- 20Ul Wasser- 30 Warm- Ramp- -22 - -23 In 10Min - 24-0 Raumtemp- 19-06 Messung Fertig- 47-81955 Framerate-03062025190030-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/07.03.25/11- 700Ul Oel- 20Ul Wasser- 30 Warm- -23 Auf -24 In 10Min Kalt- 23-3 Raumtemp- 16-37 Messung Fertig- 47-81955 Framerate-03072025162548-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/07.03.25/12-2- 700Ul Oel- 20Ul Wasser- 30 Warm- -23-5 Auf -24-5 In 10Min Kalt- 23-8 Raumtemp- 17-54 Messung Fertig- 47-81955 Framerate-03072025174949-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/07.03.25/13- 700Ul Oel- 10Ul Wasser- 30 Warm- -23-5 Auf -24-5 In 10Min Kalt- 22-7 Raumtemp- 19-00 Messung Fertig- 47-81955 Framerate-03072025185057-0000-case.cf")

temps = [70]*6 + [30]*10

plt.rcParams.update({
    "font.family": "serif",
    "text.usetex": True,
    "pgf.rcfonts": False,
    "font.size": 11,
    "text.latex.preamble": r"\usepackage{amsmath} \usepackage{amssymb} \usepackage{siunitx}",
})


figsize = (6.3, 3.5)
color_blue = "tab:blue"
color_orange = "#ed8b00"

data_array = []
fig, ax = plt.subplots(layout="constrained")
fig.set_size_inches(figsize)
residuals = []
if __name__ == "__main__":
    markers = itertools.cycle(("o", "v", "^", "<", ">", "1", "s", "+", "x", "d", "h", "p", "*", "X", "P", "8"))
    for file, temp in zip(inputs, temps):
        with open(file) as f:
            data = toml.loads(f.read())
        file = file.replace("-case.cf", "-case-bin-results.csv", 1)
        if not os.path.isfile(file):
            raise FileNotFoundError("The provided file '{}' does not exist.".format(file))

        result_nucleation = np.loadtxt(file, delimiter=";")
        result_nucleation[:,0] += 273.15
        data_array.append(result_nucleation.transpose())

        color = color_blue
        marker = next(markers)
        if temp > 50:
            color = color_orange
        ax.scatter(result_nucleation[:, 0] - 273.15, result_nucleation[:, 1], color=color, marker=marker, s=15)

        fit_x = np.linspace(result_nucleation[:, 0].min(), result_nucleation[:, 0].max(), 50)
        #try:
        fit = curve_fit(lambda x, p0, p1: p0  - (p1/x), result_nucleation[:,0], np.log(result_nucleation[:,1]))[0]
        result_linregress = linregress(1/result_nucleation[:,0], np.log(result_nucleation[:,1]))
        ax.plot(fit_x - 273.15, np.exp(fit[0]) * np.exp(-fit[1]/fit_x), color=color, lw=1)
        #ax.fill_between(fit_x, np.exp(fit[0]) * np.exp(-fit[1]/fit_x) + error_intercep, np.exp(fit[0]) * np.exp(-fit[1]/fit_x) - error_slope, alpha=.2, color=color)
        residuals.append(np.pow(np.sum(result_nucleation[:,1]-(np.exp(fit[0]) * np.exp(-fit[1]/result_nucleation[:, 0]))), 2))
        #except ValueError:
        #    print("Could not fit data")


    ax.plot([],[], "o", color=color_orange, label=r"$T_h = 70^\circ\text{C}$")
    ax.plot([],[], "o", color=color_blue, label=r"$T_h = 30^\circ\text{C}$")
    ax.plot([],[], "-", color="black", label=r"Fit")

    #ax.grid()
    ax.set_ylim((1e8, 1e11))
    #ax.set_xlim((249, 253.5))
    #ax.set_xticks([250, 251, 252])
    ax.set_yscale("log")
    ax.set_xlabel(r"$T_k$  [°C]")
    ax.set_ylabel(r"$\dot{n}\ [\text{m}^3 \text{s}^{-1}]$")
    #ax.legend()
    ax.legend(bbox_to_anchor=(0., 1.05, 1., 0.102), loc="lower left", mode="expand", ncol=3, borderaxespad=0.)
    plt.savefig("/home/nicholas/plot_nucleaten.pdf")

    plt.figure()
    plt.scatter(temps, residuals)
    plt.yscale("log")
    plt.show()
