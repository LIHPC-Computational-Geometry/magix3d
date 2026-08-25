import pyMagix3D as Mgx3D

ctx = Mgx3D.getStdContext()
ctx.clearSession() # Clean the session after the previous test
tm = ctx.getTopoManager()
mm = ctx.getMeshManager()

# Création d'une boite avec une topologie
tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "Cube")
# Découpage de l'arête Ar0000
tm.splitEdge("Ar0000", .5)
# Translation de la topologie seule Som0008
tm.translate (["Som0008"], Mgx3D.Vector(0, .2, 0), False)
# Aligne des sommets topologiques sur une droite définie par 2 points
tm.alignVertices(Mgx3D.Point(0, 0, 1), Mgx3D.Point(0, 0, 0), ["Som0008"])

# Modifie le groupe Points
ctx.getGeomManager().addToGroup (["Pt0000", "Pt0001", "Pt0002", "Pt0003", "Pt0004", "Pt0005", "Pt0006", "Pt0007"], 0, "Points")

# Modifie le groupe Paires
ctx.getTopoManager().addToGroup (["Fa0000", "Fa0002", "Fa0004"], 2, "Paires")
# Modifie le groupe ImPaires
ctx.getTopoManager().addToGroup (["Fa0001", "Fa0003", "Fa0005"], 2, "ImPaires")

mm.newAllBlocksMesh()