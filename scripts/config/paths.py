"""Path configuration for data and output files."""

from pathlib import Path

_output_dir = None


def set_output_dir(output_dir: str | Path) -> None:
    """Set the output directory for plots and results."""
    global _output_dir
    _output_dir = Path(output_dir)


def get_output_dir(create: bool = True) -> Path:
    """Get the output directory for plots and results."""
    if _output_dir is None:
        raise TypeError("Output directory not set.")
    if create:
        _output_dir.mkdir(parents=True, exist_ok=True)
    elif not _output_dir.exists():
        raise FileNotFoundError(f"Output directory does not exist: {_output_dir}")
    return _output_dir


def get_result_file(config_file: str | Path) -> Path:
    """
    Convert a .cf config file path to the corresponding -case-bin-results.csv file.

    Args:
        config_file: Path to the .cf configuration file

    Returns:
        Path to the corresponding results CSV file
    """
    config_file = Path(config_file)
    result_file = config_file.parent / config_file.name.replace(
        "-case.cf", "-case-bin-results.csv", 1
    )
    return result_file


def get_droplets_file(config_file: str | Path) -> Path:
    """
    Convert a .cf config file path to the corresponding -droplets.csv file.

    Args:
        config_file: Path to the .cf configuration file

    Returns:
        Path to the corresponding droplets CSV file
    """
    config_file = Path(config_file)
    droplets_file = config_file.parent / config_file.name.replace(
        "-case.cf", "-droplets.csv", 1
    )
    return droplets_file
