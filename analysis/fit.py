import uproot
import numpy as np
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

from reco import *
reconstructed_energy = energy
true_energy = np.array(events['Energy']).mean(axis=-1)

# 进行线性刻度
def calibration_function(true_energy, a, b):
    return np.hypot(a / np.sqrt(true_energy), b) * true_energy

# 拟合刻度参数
params, _ = curve_fit(calibration_function, true_energy, np.abs(reconstructed_energy - true_energy))
a, b = params

# 应用刻度
calibrated_energy = calibration_function(true_energy, a, b)

# 绘制刻度前后的能量对比
plt.figure()
plt.plot(true_energy, reconstructed_energy, 'o', label='Reconstructed Energy')
plt.plot(true_energy, calibrated_energy, 'x', label='Calibrated Energy')
plt.plot(true_energy, true_energy, '-', label='True Energy')
plt.xlabel('True Energy (GeV)')
plt.ylabel('Energy (GeV)')
plt.legend()
plt.title('Energy Calibration')
plt.tight_layout()
plt.grid()
plt.show()
