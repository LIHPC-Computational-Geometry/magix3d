import pyMagix3D as Mgx3D

ctx = Mgx3D.getStdContext()
ctx.clearSession() # Clean the session after the previous test
gm = ctx.getGeomManager()
tm = ctx.getTopoManager()
mm = ctx.getMeshManager()

# Création d'une boite avec une topologie
tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
# Création d'une boite avec une topologie
tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
# Création d'une boite avec une topologie
tm.newBoxWithTopo (Mgx3D.Point(2, 0, 0), Mgx3D.Point(3, 1, 1), 10, 10, 10)
# Collage entre géométries avec topologies
gm.glue (["Vol0000","Vol0001","Vol0002"])
# Découpage en O-grid des blocs structurés Bl0000
tm.splitBlocksWithOgridV2 (["Bl0000"], ["Fa0001","Fa0005","Fa0004"], .5, 10)
# Création du maillage pour tous les blocs
mm.newAllBlocksMesh()
