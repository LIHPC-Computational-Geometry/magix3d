import pyMagix3D as Mgx3D


ctx = Mgx3D.getStdContext()
gm = ctx.getGeomManager()
tm = ctx.getTopoManager()
mm = ctx.getMeshManager()

# Création de la boite Vol0000
tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 5, 8, 10, "A")
# Création de la boite Vol0001
tm.newBoxWithTopo (Mgx3D.Point(10, 0, 0), Mgx3D.Point(1, 1, 1), 20, 8, 10, "B")
# Collage entre Vol0000 Vol0001
gm.glue (["Vol0000","Vol0001"])
# Création du maillage pour tous les blocs
mm.newAllBlocksMesh()