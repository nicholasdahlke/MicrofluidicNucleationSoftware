"""Common plotting utilities and styling."""

import matplotlib.pyplot as plt
import matplotlib as mpl
import itertools
from pathlib import Path
from config import (
    PLOT_COLORS,
    MARKER_STYLES,
    FONT_CONFIG_STANDARD,
    FONT_CONFIG_SMALL,
    FONT_CONFIG_MEDIUM,
)
from config import get_output_dir


def setup_plot_style(
    font_config: dict | None = None,
) -> None:
    """
    Apply consistent plotting style to matplotlib.

    Args:
        font_config: Font configuration dictionary. If None, uses FONT_CONFIG.
                    Options: FONT_CONFIG_STANDARD, FONT_CONFIG_SMALL, FONT_CONFIG_MEDIUM
    """
    if font_config is None:
        font_config = FONT_CONFIG_STANDARD

    plt.rcParams.update(font_config)


def create_figure(
    figsize: tuple[float, float] | None = (10, 6),
    layout: str = "constrained",
) -> tuple:
    """
    Create a figure with consistent styling.

    Args:
        figsize: Figure size as (width, height)
        layout: Layout manager ('constrained' or 'tight')

    Returns:
        Tuple of (fig, ax) matplotlib objects
    """
    fig, ax = plt.subplots(layout=layout)
    if figsize:
        fig.set_size_inches(figsize)
    return fig, ax


def save_figure(
    fig: plt.Figure,
    filename: str | Path,
    output_dir: Path | None = None,
) -> Path:
    """
    Save a figure with consistent settings.

    Args:
        fig: Matplotlib figure object
        filename: Output filename (can include extension or not)
        output_dir: Output directory (uses default if None)
        dpi: Dots per inch

    Returns:
        Path object of the saved file
    """
    if output_dir is None:
        output_dir = get_output_dir()

    filename = Path(filename)
    if not filename.suffix:
        filename = filename.with_suffix('.pdf')

    output_path = output_dir / filename.name
    fig.savefig(output_path, bbox_inches='tight')

    return output_path


def get_marker_cycle(n_cycles: int | None = None) -> itertools.cycle:
    """
    Get a cycle of marker styles for plotting multiple datasets.

    Args:
        n_cycles: Number of cycles (None for infinite)

    Returns:
        itertools.cycle object yielding marker styles
    """
    if n_cycles is not None:
        markers = MARKER_STYLES * (n_cycles // len(MARKER_STYLES) + 1)
        return iter(markers[:n_cycles])
    else:
        return itertools.cycle(MARKER_STYLES)


def get_color(
    label: str | None = None,
    temperature: float | None = None,
) -> str:
    """
    Get a color from the color scheme.

    Can select by label name or by temperature value.

    Args:
        label: Color name ('blue', 'orange', 'warm', 'cold')
        temperature: Temperature in Celsius. Returns 'warm' (70°C+) or 'cold' color

    Returns:
        Hex color string

    Raises:
        ValueError: If neither label nor temperature provided, or label not found
    """
    if label is not None:
        if label in PLOT_COLORS:
            return PLOT_COLORS[label]
        else:
            raise ValueError(f"Unknown color label: {label}. Options: {list(PLOT_COLORS.keys())}")

    if temperature is not None:
        if temperature >= 50:  # Warm
            return PLOT_COLORS['warm']
        else:  # Cold
            return PLOT_COLORS['cold']

    raise ValueError("Either 'label' or 'temperature' must be provided")


def add_legend_symbols(
    ax: plt.Axes,
    warm_label: str = "Warm",
    cold_label: str = "Cold",
    symbol_size: int = 60,
) -> None:
    """
    Add consistent legend symbols to a plot.

    Args:
        ax: Matplotlib axes object
        warm_label: Label for warm temperature condition
        cold_label: Label for cold temperature condition
        symbol_size: Size of legend markers
    """
    ax.scatter([], [], color=get_color('warm'), label=warm_label, s=symbol_size)
    ax.scatter([], [], color=get_color('cold'), label=cold_label, s=symbol_size)

