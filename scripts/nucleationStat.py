import matplotlib.pyplot as plt
import numpy as np

# Constants and setting
overview_file = "/home/nicholas/Mpempa Videos/Messungen Jufo 2025 Temp Ramp/file_overview.csv"
k_B = 1.38064852e-23


def read_overview_file(overview_file_: str):
    """
    Loads nucleation measurement data from multiple CSV files referenced in an overview file.

    Args:
        overview_file_ (str): Path to overview CSV with columns 'measurement_file' and 'temperature'

    Returns:
        np.ndarray: Combined nucleation data [data_cols..., measurement_id, temperature_K].
                   Columns 0 and 3 converted from Celsius to Kelvin.
    """
    overview = np.genfromtxt(overview_file_, delimiter=';', dtype=None, names=True, encoding="utf-8")
    nucleation_files = []
    id = 0
    for nucleation in overview:
        nucleation_data = np.loadtxt(nucleation['measurement_file'].replace("-case.cf", "-case-bin-results.csv", 1),
                                     delimiter=';')
        id_arr = np.full((nucleation_data.shape[0], 1), id)
        temp_high_arr = np.full((nucleation_data.shape[0], 1), nucleation["temperature"])
        out = np.hstack((nucleation_data, id_arr, temp_high_arr))
        nucleation_files.append(out)
        id += 1

    nucleation_files = np.vstack(nucleation_files)
    nucleation_files[:, [0, 3]] += 273.15
    return nucleation_files


def get_linear_fit_parameters(temperature_: np.ndarray, nucleation_rate_: np.ndarray):
    """
    Fits temperature and nucleation rate data using linear regression.

    Uses the classical nucleation theory: ln(J) = ln(J_0) - (ΔG / k_B) * (1/T)

    Args:
        temperature_ (np.ndarray): Absolute temperatures in Kelvin
        nucleation_rate_ (np.ndarray): Nucleation rates (1/nucleation_rate per temperature)

    Returns:
        tuple: (gibbs_energy [J], ln(J_0)) from linear fit parameters
    """
    p, cov = np.polyfit(1 / temperature_, np.log(nucleation_rate_), 1, cov=True)
    # p0 is -Delta_G/k_b, p1 is ln(n_dot 0)
    return -p[0] * k_B, p[1], cov


def givens_rotation(angle: float):
    """
    Generates a 2D Givens rotation matrix for the given angle.

    Args:
        angle (float): Rotation angle in radians

    Returns:
        list: 2x2 rotation matrix [[cos(θ), sin(θ)], [-sin(θ), cos(θ)]]
    """
    return [[np.cos(angle), -np.sin(angle)], [np.sin(angle), np.cos(angle)]]


def get_confidence_ellipse(temperature_: np.ndarray, nucleation_rate_: np.ndarray, confidence=0.95):
    # 1. Get physical shifts and the RAW, numerically stable covariance matrix
    x_shift, y_shift, raw_cov = get_linear_fit_parameters(temperature_, nucleation_rate_)

    # 2. Compute eigenvalues/vectors on the RAW matrix (avoids float precision loss)
    raw_vals, raw_vecs = np.linalg.eigh(raw_cov)

    # 3. Dynamically locate the dominant axis of the needle
    max_idx = np.argmax(raw_vals)
    min_idx = 1 - max_idx

    # 4. Calculate the true parametric tilt angle
    angle = np.arctan2(raw_vecs[1, max_idx], raw_vecs[0, max_idx])

    # 5. Apply your confidence scaling factor
    scale_factor = np.sqrt(-2 * np.log(1 - confidence))
    axes_ellipse = np.sqrt(np.abs(raw_vals)) * scale_factor

    # For better visibility, scale the minor axis by a factor
    scale_minor = 1
    axes_ellipse[min_idx] *= scale_minor

    # 6. Generate the base narrow ellipse points
    t = np.linspace(0, 2 * np.pi, 100)
    ellipse_points = np.array([
        axes_ellipse[max_idx] * np.cos(t),
        axes_ellipse[min_idx] * np.sin(t)
    ])

    # 7. Rotate the narrow ellipse while it is still in the stable coordinate space
    ellipse_points = givens_rotation(angle) @ ellipse_points
    ellipse_points = ellipse_points.T

    # 8. Transform the raw slope space into physical Delta G space
    # This stretches/squashes the ellipse into its final, physically accurate scale
    transform_matrix = np.array([[-k_B, 0], [0, 1]])
    ellipse_points = ellipse_points @ transform_matrix

    # 9. Shift to the physical parameters center
    ellipse_points[:, 0] += x_shift
    ellipse_points[:, 1] += y_shift

    return ellipse_points


if __name__ == "__main__":
    data = read_overview_file(overview_file)

    # Get fit parameters for all datasets
    fit_parameters = []
    for id in range(int(np.max(data[:, 2]))+1):
        delta_g, ln_ndot0,_ = get_linear_fit_parameters(data[data[:, 2] == id, 0], data[data[:, 2] == id, 1])
        temperature = data[data[:, 2] == id, 3][0]
        fit_parameters.append((delta_g, ln_ndot0, id, temperature))
    fit_parameters = np.array(fit_parameters)

    confidence_ellipses = []
    for id in range(int(np.max(data[:, 2]))+1):
        confidence_ellipses.append(get_confidence_ellipse(data[data[:, 2] == id, 0], data[data[:, 2] == id, 1]))

    # Set up plotting
    fig, axes = plt.subplots(1, 2)

    # Plot nucleation rate vs temperature
    axes[0].scatter(data[:, 0], data[:, 1])
    axes[0].set_xlabel("$T$")
    axes[0].set_ylabel(r"$\dot{n}$")
    axes[0].set_title("Nucleation rates")

    # Plot fit parameter space
    axes[1].scatter(fit_parameters[:, 0], fit_parameters[:, 1])
    axes[1].set_xlabel(r"$\Delta G$")
    axes[1].set_ylabel(r"$\ln(\dot{n}_0)$")
    axes[1].set_title("Fit parameter space")

    # Plot confidence ellipses in fit parameters space
    for ellipse, fit_param in zip(confidence_ellipses, fit_parameters):
        axes[1].plot(ellipse[:, 0], ellipse[:, 1], color="tab:orange" if fit_param[3] > 323.15 else "tab:blue")

    plt.show()
