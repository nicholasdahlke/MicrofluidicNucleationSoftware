from time import process_time_ns
from unittest import result

from matplotlib.patches import Ellipse
import matplotlib.pyplot as plt
import numpy as np
import toml
import os
from scipy.optimize import curve_fit
from scipy.stats import mannwhitneyu, hmean
import itertools
from sklearn.decomposition import PCA

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

num_warm = 6
num_cold = 10
temps = [70]*num_warm + [30]*num_cold
k_b = 1.380649e-23
used_temps = None
param = []

def compute_confidence_ellipse(t, n_dot):
    t_hm = hmean(t)
    sigma_squared = np.var(1/t)
    rho_squared = (1/t_hm**2)/(1/t_hm**2 + sigma_squared)
    n = t.size
    lambda_1 = (1/n)*(rho_squared/(1-rho_squared))*(t_hm**2 + 1)
    lambda_2 = 1/n
    angle = np.arctan(1/(k_b*t_hm))
    return lambda_1, lambda_2, angle

ellipses = []
if __name__ == "__main__":
    colors = itertools.cycle(("tab:blue", "tab:orange", "tab:green", "tab:red", "tab:purple", "tab:brown", "tab:pink", "tab:gray", "tab:olive", "tab:cyan"))
    for file, temp in zip(inputs, temps):
        with open(file) as f:
            data = toml.loads(f.read())
        file = file.replace("-case.cf", "-case-bin-results.csv", 1)
        if not os.path.isfile(file):
            raise FileNotFoundError("The provided file '{}' does not exist.".format(file))

        result_nucleation = np.loadtxt(file, delimiter=";")
        result_nucleation[:,0] += 273.15
        ellipses.append(compute_confidence_ellipse(result_nucleation[:,0], result_nucleation[:,1]))
        color = next(colors)
        marker = "o"
        if temp > 50:
            marker = "x"
        #fig, ax = plt.subplots()
        #ax.scatter(result_nucleation[:, 0], result_nucleation[:, 1], color=color, marker=marker)
        fit_x = np.linspace(result_nucleation[:, 0].min(), result_nucleation[:, 0].max(), 50)

        lin_fit_func = lambda x, p0, p1: p0  - (p1/x)
        linear_fit = curve_fit(lin_fit_func, result_nucleation[:,0], np.log(result_nucleation[:,1]))[0]
        #nonlinear_fit_func = lambda x, p0, p1: p0 * np.exp(-p1/x)
        #nonlinear_fit = curve_fit(nonlinear_fit_func, result_nucleation[:,0], result_nucleation[:,1], p0=(np.exp(linear_fit[0]), linear_fit[1]), maxfev=10000000)[0]
        #param.append((linear_fit[0], linear_fit[1], nonlinear_fit[0], nonlinear_fit[1]))
        param.append((linear_fit[0], linear_fit[1]))
        if used_temps is None:
            used_temps = result_nucleation[:,0]
        else:
            used_temps = np.concatenate((used_temps, result_nucleation[:,0]))
        #ax.plot(fit_x, np.exp(linear_fit[0])*np.exp(-linear_fit[1]/fit_x), label="Linear")
        #plt.plot(fit_x, nonlinear_fit[0]*np.exp(-nonlinear_fit[1]/fit_x), label="Nonlinear")
        #ax.legend()

    param = np.array(param)

    #param[:, 2] = np.log(param[:,2])

    fit_x = np.linspace(param[:, 0].min(), param[:, 0].max(), len(param))
    fit_lin = curve_fit(lambda x, p0, p1: p0*x + p1, param[:,0], param[:,1])[0]

    param_cov = np.cov(np.transpose(param[:, 0:2]))
    eigenvalues, eigenvectors = np.linalg.eig(param_cov)
    print("Eigenvalues", eigenvalues)
    print("Eigenvectors", eigenvectors)
    print(param_cov)

    t_hm = hmean(used_temps)
    fig, ax = plt.subplots()
    ax.set_title("Constable plot")

    for point, ellipse in zip(param, ellipses):
        el = Ellipse(xy=(point[0], point[1]*k_b), height=np.log(ellipse[0]), width=ellipse[1], angle=np.rad2deg(ellipse[2]), linewidth=2, fill=False)
        ax.add_patch(el)

    ax.scatter(param[:num_warm, 0], param[:num_warm, 1]*k_b, c="tab:orange", label="Warm", marker="x")
    ax.scatter(param[num_warm:, 0], param[num_warm:, 1]*k_b, c="tab:blue", label="cold", marker="x")
    #plt.scatter(param[:num_warm, 2], param[:num_warm, 3]*k_b, c="tab:orange", label="Warm", marker="o")
    #plt.scatter(param[num_warm:, 2], param[num_warm:, 3]*k_b, c="tab:blue", label="cold", marker="o")
    ax.plot(fit_x, (fit_lin[0]*fit_x + fit_lin[1])*k_b, label="Linear fit")
    print("Gradient", fit_lin[0]*k_b)
    print("Grad the", 1/(k_b*t_hm))
    ax.set_xlabel(r"$\Delta G$")
    ax.set_ylabel(r"$\ln \dot{n}_0$")

    ax.legend()
    plt.show()

    # Mann-Whitney on raw p0 parameter
    #print("=== Mann-Whitney on raw p0 ===")
    #print("less      " + str(mannwhitneyu(param[:num_warm, 0], param[num_warm:, 0], alternative="less",       method="auto")))
    #print("greater   " + str(mannwhitneyu(param[:num_warm, 0], param[num_warm:, 0], alternative="greater",    method="auto")))
    #print("two-sided " + str(mannwhitneyu(param[:num_warm, 0], param[num_warm:, 0], alternative="two-sided",  method="auto")))

    # PCA reduction to 1D
    pca_linear = PCA(n_components=1)
    reduced_linear = pca_linear.fit_transform(param[:, 0:2])   # shape (n_samples, 1)


    print(f"Linear PC1 direction (p0, p1): {pca_linear.components_[0]}")

    # Mann-Whitney on PCA scores
    print("\n=== Mann-Whitney on PCA scores with linear fit ===")
    #print("less      " + str(mannwhitneyu(reduced_linear[:num_warm], reduced_linear[num_warm:], alternative="less",       method="auto")))
    print("greater   " + str(mannwhitneyu(reduced_linear[:num_warm], reduced_linear[num_warm:], alternative="greater",    method="auto")))
    print("two-sided " + str(mannwhitneyu(reduced_linear[:num_warm], reduced_linear[num_warm:], alternative="two-sided",  method="auto")))


