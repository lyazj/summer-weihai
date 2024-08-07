import uproot
import numpy as np

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
print('Found fields:', events.fields)
[y, x] = np.meshgrid(range(50), range(50))
for i in range(len(events)):
    Edep = decode_edep(events, i)
    xy0 = np.argmax(Edep)
    x0, y0 = xy0 // 50, xy0 % 50
    showernum = 3.5 * 50 / 50.5
    distance = np.hypot(x - x0, y - y0)
    weight = distance <= showernum
    sumE = np.sum(Edep * weight)
    meanX = np.sum(x * Edep * weight) / sumE
    meanY = np.sum(y * Edep * weight) / sumE
    print('%4d: %.3e\t%.3e\t%.3e\t%.3e' %(i, x0, y0, meanX, meanY))