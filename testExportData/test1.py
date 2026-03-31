import pyMagix3D as Mgx3D

ctx = Mgx3D.getStdContext()
gm = ctx.getGeomManager()
tm = ctx.getTopoManager()
mm = ctx.getMeshManager()

# Création de la boite Vol0000
tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10 , 10, 10, "A")
# Section par un plan de Vol0000 suivant  [  [ 4.61847e-1, 0, 0] ,  [ 1, 0, 0] ] 
gm.sectionByPlane (["Vol0000"], Mgx3D.Plane(Mgx3D.Point(0.45, 0, 0), Mgx3D.Vector(1, 0, 0)), "B")
# Création du maillage pour tous les blocs
mm.newAllBlocksMesh()