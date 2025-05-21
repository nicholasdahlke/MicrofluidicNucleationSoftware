import matplotlib.pyplot as plt
import numpy as np
import toml
import os
from scipy.optimize import curve_fit
import itertools
import matplotlib as mpl
from scipy.stats import linregress
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
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/07.03.25/11- 700Ul Oel- 20Ul Wasser- 30 Warm- -23 Auf -24 In 10Min Kalt- 23-3 Raumtemp- 16-37 Messung Fertig- 47-81955 Framerate-03072025162548-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/07.03.25/12-2- 700Ul Oel- 20Ul Wasser- 30 Warm- -23-5 Auf -24-5 In 10Min Kalt- 23-8 Raumtemp- 17-54 Messung Fertig- 47-81955 Framerate-03072025174949-0000-case.cf")
inputs.append("/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/07.03.25/13- 700Ul Oel- 10Ul Wasser- 30 Warm- -23-5 Auf -24-5 In 10Min Kalt- 22-7 Raumtemp- 19-00 Messung Fertig- 47-81955 Framerate-03072025185057-0000-case.cf")

num_warm = 6
num_cold = 10
temps = [70]*num_warm + [30]*num_cold


plt.rcParams.update({
    "font.family": "serif",
    "text.usetex": True,
    "pgf.rcfonts": False,
    "font.size": 11,
    "text.latex.preamble": r"\usepackage{amsmath} \usepackage{amssymb} \usepackage{siunitx}",
})
figsize = (3.08102, 2.3)
color_blue = "tab:blue"
color_orange = "#ed8b00"


param = []
fig, ax = plt.subplots(layout="constrained")
fig.set_size_inches(figsize)

error_n_dot = []
error_delta_g = []
if __name__ == "__main__":
    markers = itertools.cycle((',', '+', '.', 'o', '*'))
    for file, temp in zip(inputs, temps):
        with open(file) as f:
            data = toml.loads(f.read())
        file = file.replace("-case.cf", "-case-bin-results.csv", 1)
        if not os.path.isfile(file):
            raise FileNotFoundError("The provided file '{}' does not exist.".format(file))

        result_nucleation = np.loadtxt(file, delimiter=";")
        result_nucleation[:,0] += 273.15

        color = color_blue
        marker = next(markers)
        if temp > 50:
            color = color_orange
        try:
            fit = curve_fit(lambda x, p0, p1: p0  - (p1/x), result_nucleation[:,0], np.log(result_nucleation[:,1]))[0]
            param.append((fit[0], fit[1]))
            result_linregress = linregress(1/result_nucleation[:,0], np.log(result_nucleation[:,1]))
            error_n_dot.append(result_linregress.intercept_stderr)
            error_delta_g.append(result_linregress.stderr)


        except:
            print("Could not fit data")

    param = np.array(param)
    error_n_dot = np.array(error_n_dot)
    error_delta_g = np.array(error_delta_g)
    fit_x = np.linspace(param[:, 0].min(), param[:, 0].max(), len(param))
    fit_lin = curve_fit(lambda x, p0, p1: p0*x + p1, param[:,0], param[:,1])[0]
    ax.plot(fit_x, fit_lin[0]*fit_x + fit_lin[1], linewidth=1, color="black", label="Fit", zorder=1)
    ax.scatter(param[:num_warm, 0], param[:num_warm, 1], c=color_orange,label=r"$T_h = 70^\circ\text{C}$", zorder=2, s=15)
    ax.scatter(param[num_warm:, 0], param[num_warm:, 1], c=color_blue, label=r"$T_h = 30^\circ\text{C}$", zorder=2, s=15)
    #ax.errorbar(param[:num_warm, 0], param[:num_warm, 1], xerr=error_n_dot[:num_warm]/2, yerr=error_delta_g[:num_warm]/2, fmt='', linestyle='', marker='o', capsize=5, markersize=4,c=color_orange,label=r"$T_h = 70^\circ\text{C}$", zorder=2)
    #ax.errorbar(param[num_warm:, 0], param[num_warm:, 1], xerr=error_n_dot[num_warm:]/2, yerr=error_delta_g[num_warm:]/2, fmt='', linestyle='', marker='o', capsize=5, markersize=4,c=color_blue,label=r"$T_h = 30^\circ\text{C}$", zorder=2)

    #ax.grid()
    ax.set_ylabel(r"$\Delta G^*_r k_B^{-1}$ [K]")
    ax.set_ylim((-1.4e5,-0.2e5))
    ax.set_xlim((-530, -50))
    ax.ticklabel_format(style='sci', axis='y', scilimits=(0,0))
    ax.set_xlabel(r"$\ln\dot{n}_0\ [\text{m}^3 \text{s}^{-1}]$")
    #ax.legend(loc="lower right")
    ax.legend(bbox_to_anchor=(0., 1.2, 1., 0.102), loc="lower left", mode="expand", ncol=2, borderaxespad=0.)
    plt.savefig("/home/nicholas/plot_scatter_new.pdf")
    plt.figure(figsize=(11.6, 8.26))
    plt.rcParams["font.size"] = 11
    #plt.ylim((-210, 210))
    plt.title("Residuen")
    plt.scatter(param[:num_warm, 0], (param[:num_warm, 1] -(fit_lin[0]*param[:num_warm,0] + fit_lin[1]))/(fit_lin[0]*param[:num_warm,0] + fit_lin[1]), c=color_orange)
    plt.scatter(param[num_warm:, 0], (param[num_warm:, 1] -(fit_lin[0]*param[num_warm:,0] + fit_lin[1]))/(fit_lin[0]*param[num_warm:,0] + fit_lin[1]), c=color_blue)
    print(np.max(np.abs((param[:num_warm, 1] -(fit_lin[0]*param[:num_warm,0] + fit_lin[1]))/(fit_lin[0]*param[:num_warm,0] + fit_lin[1]))))
    print(np.max(np.abs((param[num_warm:, 1] -(fit_lin[0]*param[num_warm:,0] + fit_lin[1]))/(fit_lin[0]*param[num_warm:,0] + fit_lin[1]))))
    plt.axhline(y=0, color="grey", linestyle="dashed")
    plt.ylabel(r"$\Delta(\Delta G^*_r k_B^{-1})$ [K]")
    plt.xlabel(r"$\ln\dot{n}_0\ [\text{m}^3 \text{s}^{-1}]$")
    #plt.yscale("log")

    plt.show()
