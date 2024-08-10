import uproot
import numpy as np
from matplotlib import pyplot as plt
from scipy.stats import norm

__all__ = ['events', 'pos', 'energy', 'energy_distparm']

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
    return Edep.sum(axis=0), Edep

INPUT = 'tree.root'
events = uproot.open(INPUT + ':tree').arrays()
pos = np.empty((len(events), 2))
energy = np.empty(len(events))
print('Found fields:', events.fields)

[y, x] = np.meshgrid(range(50), range(50))
Edep_total = np.zeros((50, 50))  # 用于存储所有事件的能量分布

for i in range(len(events)):
    Edep_sum, Edep = decode_edep(events, i)
    xy0 = np.argmax(Edep_sum)
    x0, y0 = xy0 // 50, xy0 % 50
    showernum = (3.5+0.5) * 50 / 50.5			##0.5代表半个cell
    distance = np.hypot(x - x0, y - y0)
    weight = distance <= showernum
    sumE = np.sum(Edep_sum * weight)
    meanX = np.sum(x * Edep_sum * weight) / sumE
    meanY = np.sum(y * Edep_sum * weight) / sumE
    pos[i] = [meanX, meanY]
    energy[i] = sumE

    Edep_total += Edep_sum  # 累加每个事件的能量分布

# 对能量进行拟合
unique_energy = np.unique(events['Energy'])
energy_distparm = np.empty((len(unique_energy), 2))
for i in range(len(unique_energy)):
    e = np.array([energy[j] for j in range(len(energy)) 
                  if events['Energy'][j] == unique_energy[i]])
    energy_distparm[i] = norm.fit(e)

print(energy_distparm)

x = np.logspace(np.log10(np.min(energy)), np.log10(np.max(energy)), 10001)
y = np.zeros(x.shape)
for mu, sigma in energy_distparm:
    y += norm(mu, sigma).pdf(x)
# 能量分布图
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

# 绘制能量分布的二维图
plt.figure(facecolor='white')  
plt.imshow(Edep_total, origin='lower', cmap='jet', extent=(0, 50, 0, 50))
plt.colorbar(label='Energy (MeV)')
plt.title('2D Energy Distribution')
plt.xlabel('X ')
plt.ylabel('Y ')
plt.tight_layout()
plt.savefig('energy_distribution_2D.pdf')
plt.close()
