"""Nucleation theory calculations including confidence intervals."""

import numpy as np
from config import K_B


def get_linear_fit_parameters(
        temperature: np.ndarray,
        nucleation_rate: np.ndarray,
) -> tuple[float, float, np.ndarray]:
    """
    Fit temperature and nucleation rate data using linear regression.

    Uses classical nucleation theory: ln(J) = ln(J_0) - (ΔG / k_B) * (1/T)

    This function performs a linear fit on ln(nucleation_rate) vs 1/temperature
    and returns the Gibbs energy and pre-factor from the fit parameters.

    Args:
        temperature: Absolute temperatures in Kelvin
        nucleation_rate: Nucleation rates [m^-3 s^-1]

    Returns:
        tuple containing:
        - gibbs_energy: Activation Gibbs energy [J]
        - ln_j0: Natural logarithm of pre-exponential factor ln(J_0)
        - cov: Covariance matrix from polyfit
    """
    p, cov = np.polyfit(1 / temperature, np.log(nucleation_rate), 1, cov=True)
    # p[0] is -ΔG/k_b, p[1] is ln(J_0)
    gibbs_energy = -p[0] * K_B
    ln_j0 = p[1]

    return gibbs_energy, ln_j0, cov


def givens_rotation(angle: float) -> list:
    """
    Generate a 2D Givens rotation matrix for the given angle.

    Args:
        angle: Rotation angle in radians

    Returns:
        2x2 rotation matrix as nested list
    """
    return [
        [np.cos(angle), -np.sin(angle)],
        [np.sin(angle), np.cos(angle)]
    ]


def get_confidence_ellipse(
        temperature: np.ndarray,
        nucleation_rate: np.ndarray,
        confidence: float = 0.95,
) -> np.ndarray:
    """
    Compute a confidence ellipse for fit parameters in parameter space.

    Uses eigenvalue decomposition of the covariance matrix from linear regression fit,
    then transforms and rotates the ellipse to physical parameter space.

    Args:
        temperature: Absolute temperatures in Kelvin
        nucleation_rate: Nucleation rates [m^-3 s^-1]
        confidence: Confidence level (default 0.95 for 95%)

    Returns:
        np.ndarray: Ellipse points in (ΔG, ln(J_0)) parameter space with shape (100, 2)
    """
    # Get physical shifts and the covariance matrix
    x_shift, y_shift, raw_cov = get_linear_fit_parameters(temperature, nucleation_rate)

    # Compute eigenvalues/vectors on the covariance matrix
    raw_vals, raw_vecs = np.linalg.eigh(raw_cov)

    # Dynamically locate the dominant axis of the ellipse
    max_idx = np.argmax(raw_vals)
    min_idx = 1 - max_idx

    # Calculate the parametric tilt angle
    angle = np.arctan2(raw_vecs[1, max_idx], raw_vecs[0, max_idx])

    # Apply confidence scaling factor
    scale_factor = np.sqrt(-2 * np.log(1 - confidence))
    axes_ellipse = np.sqrt(np.abs(raw_vals)) * scale_factor

    # For better visibility, scale the minor axis
    scale_minor = 1
    axes_ellipse[min_idx] *= scale_minor

    # Generate the base ellipse points
    t = np.linspace(0, 2 * np.pi, 100)
    ellipse_points = np.array([
        axes_ellipse[max_idx] * np.cos(t),
        axes_ellipse[min_idx] * np.sin(t)
    ])

    # Rotate the ellipse while it is still in stable coordinate space
    ellipse_points = np.array(givens_rotation(angle)) @ ellipse_points
    ellipse_points = ellipse_points.T

    # Transform from slope space into physical Delta G space
    transform_matrix = np.array([[-K_B, 0], [0, 1]])
    ellipse_points = ellipse_points @ transform_matrix

    # Shift to the physical parameters center
    ellipse_points[:, 0] += x_shift
    ellipse_points[:, 1] += y_shift

    return ellipse_points
