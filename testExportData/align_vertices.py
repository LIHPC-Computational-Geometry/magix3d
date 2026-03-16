import pyMagix3D as Mgx3D

ctx = Mgx3D.getStdContext()
ctx.clearSession() # Clean the session after the previous test
tm = ctx.getTopoManager()

# Création d'une boite avec une topologie
tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
# Découpage de l'arête Ar0000
tm.splitEdge("Ar0000", .5)
# Translation de la topologie seule Som0008
tm.translate (["Som0008"], Mgx3D.Vector(0, .2, 0), False)
# Aligne des sommets topologiques sur une droite définie par 2 points
tm.alignVertices(Mgx3D.Point(0, 0, 1), Mgx3D.Point(0, 0, 0), ["Som0008"])
