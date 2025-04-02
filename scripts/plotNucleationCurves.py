import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
m = 249.9
c = -5648.3
color_blue = "#5ca7c2"
color_orange = "#ed8b00"
cmap = mpl.colors.LinearSegmentedColormap.from_list("", [color_blue, color_orange])
plt.rcParams["font.size"] = 35

figsize = (11.37, 8.66)
num_lines = 20
x = np.linspace(249, 253, 100)
fig, ax = plt.subplots(layout="constrained", figsize=figsize)
error = 0.01
for a in np.linspace(-400, -100, num_lines):
    b = (a * m) + c
    b_bottom = b * (1-error)
    b_top = b * (1+error)
    n_bottom = np.exp(a-(b_bottom/x))
    n_top = np.exp(a-(b_top/x))
    #ax.fill_between(x, n_bottom, n_top, color=cmap(n_bottom/b_bottom), alpha=0.5)
    ax.plot(x, np.exp(a-(b/x)), c=cmap((a+400)/500))

sm = plt.cm.ScalarMappable(cmap=cmap, norm=plt.Normalize(vmin=-400, vmax=-100))
sm.set_array([])  # Leere Array für die Colorbar
cbar = plt.colorbar(sm, ax=ax)
cbar.set_label(r"$\ln\dot{n}_0\ [\text{m}^3 \text{s}^{-1}]$")
ax.set_xticks([ 249, 251, 253])
ax.set_ylim((1e8, 1e11))
ax.set_yscale('log')
ax.set_ylabel(r"$\dot{n}\ [\text{m}^3 \text{s}^{-1}]$")
ax.set_xlabel(r"$T_k \ [\text{K}]$")
ax.grid()
plt.show()