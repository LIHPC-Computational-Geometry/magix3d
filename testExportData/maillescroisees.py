print(tm.getInfos("Bl0054",3).coedges())
for coedge in tm.getInfos("Bl0054",3).coedges():
    print(coedge, " -> " ,tm.getInfos(coedge, 1).geom_entity)