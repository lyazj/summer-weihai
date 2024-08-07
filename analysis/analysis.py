import uproot

def decode_position(events):
    Layer = events['Pos'] // (events['NCellX'] * events['NCellY'])
    XCell = events['Pos'] % (events['NCellX'] * events['NCellY']) // events['NCellY']
    YCell = events['Pos'] % events['NCellY']
    return Layer, XCell, YCell

INPUT = 'tree.root'
events = uproot.open(INPUT + ':tree').arrays()
print('Found fields:', events.fields)
Layer, XCell, YCell = decode_position(events)
print('Layer:', Layer)
print('XCell:', XCell)
print('YCell:', YCell)
Edep = events['Edep']
