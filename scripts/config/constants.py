"""Physical constants and plotting configuration."""

# Physical Constants
K_B = 1.380649e-23  # Boltzmann constant in J/K

# Color scheme
COLOR_BLUE = "#5ca7c2"
COLOR_ORANGE = "#ed8b00"
PLOT_COLORS = {
    'blue': COLOR_BLUE,
    'orange': COLOR_ORANGE,
    'warm': COLOR_ORANGE,  # Alias for warm temperature data
    'cold': COLOR_BLUE,     # Alias for cold temperature data
}

# Marker styles
MARKER_STYLES = ['o', 'v', '^', '<', '>', '1', 's', '+', 'x', 'd', 'h', 'p', '*', 'X', 'P', '8']

# Figure settings
DEFAULT_FIGSIZE = (10, 6)
DEFAULT_FIGSIZE_SMALL = (5, 4)
DEFAULT_FIGSIZE_MEDIUM = (5, 4.25)

# Font and LaTeX configuration
FONT_CONFIG_STANDARD = {
    "font.family": "serif",
    "text.usetex": True,
    "pgf.rcfonts": False,
    "font.size": 25,
    "text.latex.preamble": r"\usepackage{amsmath} \usepackage{amssymb} \usepackage{siunitx}",
}

FONT_CONFIG_SMALL = {
    "font.family": "serif",
    "text.usetex": True,
    "pgf.rcfonts": False,
    "font.size": 15,
    "text.latex.preamble": r"\usepackage{amsmath} \usepackage{amssymb} \usepackage{siunitx}",
}

FONT_CONFIG_MEDIUM = {
    "font.family": "serif",
    "text.usetex": True,
    "pgf.rcfonts": False,
    "font.size": 20,
    "text.latex.preamble": r"\usepackage{amsmath} \usepackage{amssymb} \usepackage{siunitx}",
}

# Plotting parameters
SYMBOL_SIZE = 60
SYMBOL_SIZE_SMALL = 4

