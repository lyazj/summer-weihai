import uproot
import numpy as np
from scipy.optimize import curve_fit
import matplotlib as mpl
import matplotlib.pyplot as plt
mpl.use('agg')

from reco import *
reconstructed_energy = energy
true_energy = np.array(events['Energy'])

a1, a0 = np.polyfit(true_energy, reconstructed_energy, 1)
calibrated_energy = a1 * true_energy + a0

plt.figure()
plt.plot(true_energy, reconstructed_energy, '.', label='Reconstructed Energy')
plt.plot(true_energy, calibrated_energy, '*', label='Calibrated Energy')
plt.title('Energy Calibration')
plt.xlabel('True Energy (MeV)')
plt.ylabel('Energy (MeV)')
plt.xscale('log')
plt.yscale('log')
plt.legend()
plt.tight_layout()
plt.grid()
plt.savefig('calibration.pdf')
plt.close()

# 定义能量分辨率函数
def resolution_function(E, a, b):
    return E * np.hypot(a / np.sqrt(E), b) 

E = unique_energy
sigma = energy_distparam[:,1] / a1

a = np.mean(sigma / np.sqrt(E))
b = 0
params, cov = curve_fit(resolution_function, E, sigma, p0=(a, b))
a, b = params
print(f'a={a:.3e} b={b:.3e}')
print(cov)

y = sigma / E
y1 = resolution_function(E, a, b) / E

plt.figure()
plt.plot(E, sigma / E * 100, '.', label='simulation')
E = np.logspace(np.log10(np.min(E)) - 0.1, np.log10(np.max(E)) + 0.1, 101)
plt.plot(E, resolution_function(E, a, b) / E * 100, '-', label=f'$\\sigma/E = \\text{{{a:.3e}}} / \\sqrt{{E}} \\oplus \\text{{{b:.3e}}}$')
plt.title('Energy Uncertainty')
plt.xlabel('Energy (MeV)')
plt.ylabel('Relative Uncertainty (%)')
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig('uncertainty.pdf')
