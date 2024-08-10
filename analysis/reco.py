import uproot
import numpy as np
from matplotlib import pyplot as plt
from scipy.stats import norm

__all__ = ['events', 'pos', 'energy', 'unique_energy', 'energy_distparam']

def decode_position(events):
    Layer = events['Pos'] // (events['NCellX'] * events['NCellY'])
    XCell = events['Pos'] % (events['NCellX'] * events['NCellY']) // events['NCellY']
    YCell = events['Pos'] % events['NCellY']
    return Layer, XCell, YCell

def decode_edep(events, i):
    Layer, XCell, YCell = decode_position(events[i])
    Edep = np.zeros((events[i]['NLayer'], events[i]['NCellX'], events[i]['NCellY']))
    for l, x, y, e in zip(Layer, XCell, YCell, events[i]['Edep']):
        Edep[l, x, y] = e
    return Edep.sum(axis=0)


INPUT = 'tree.root'
events = uproot.open(INPUT + ':tree').arrays()
pos = np.empty((len(events), 2))
energy = np.empty(len(events))
print('Found fields:', events.fields)

[y, x] = np.meshgrid(range(50), range(50))
for i in range(len(events)):
    Edep = decode_edep(events, i)
    xy0 = np.argmax(Edep)
    x0, y0 = xy0 // 50, xy0 % 50
    showernum = (3.5 + 0.5) * 50 / 50.5
    distance = np.hypot(x - x0, y - y0)
    weight = distance <= showernum
    sumE = np.sum(Edep * weight)
    meanX = np.sum(x * Edep * weight) / sumE
    meanY = np.sum(y * Edep * weight) / sumE
    pos[i] = [meanX, meanY]
    energy[i] = sumE

unique_energy = np.array(np.unique(events['Energy']))
energy_distparam = np.empty((len(unique_energy), 2))
for i in range(len(unique_energy)):
    e = np.array([energy[j] for j in range(len(energy)) 
                  if events['Energy'][j] == unique_energy[i]])
    energy_distparam[i] = norm.fit(e)

print(energy_distparam)

x = np.logspace(np.log10(np.min(energy)), np.log10(np.max(energy)), 10001)
y = np.zeros(x.shape)
for mu, sigma in energy_distparam:
    y += norm(mu, sigma).pdf(x)

plt.figure()
plt.plot(x, y, label='Reconstructed Energy')
plt.title('Energy Distribution')
plt.xlabel('True Energy (MeV)')
plt.ylabel('Density')
plt.xscale('log')
plt.yscale('linear')
plt.legend()
plt.tight_layout()
plt.grid()
plt.savefig('distribution.pdf')
plt.close()
