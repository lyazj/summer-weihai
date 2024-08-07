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
for i in range(len(events)):
    Edep = decode_edep(events, i)
