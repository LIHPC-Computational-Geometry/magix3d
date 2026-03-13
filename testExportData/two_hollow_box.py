ctx = Mgx3D.getStdContext()
ctx.clearSession() # Clean the session after the previous test
gm = ctx.getGeomManager()
tm = ctx.getTopoManager()
mm = ctx.getMeshManager()

# Création de la boite Vol0000
gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "A")
# Création d'une boite avec une topologie
tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10, "B")
# Collage entre géométries avec topologies
gm.glue (["Vol0000","Vol0001"])
# Création du cylindre Vol0002
gm.newCylinder (Mgx3D.Point(.5, .5, 0), .2, Mgx3D.Vector(0, 0, 1), 3.600000e+02, "C")
# Différences entre géométries avec plusieurs entités à couper
gm.cut (["Vol0000"], ["Vol0002"])
# Création d'un bloc topologique non structuré sur une géométrie (Vol0003)
tm.newUnstructuredTopoOnGeometry ("Vol0003")
# Création d'un cylindre avec une topologie
tm.newCylinderWithTopo (Mgx3D.Point(1.5, .5, 0), .3, Mgx3D.Vector(0, 0, 1), 360, False, 1, 10, 10, 10, "I")
# Création du maillage pour tous les blocs
mm.newAllBlocksMesh()