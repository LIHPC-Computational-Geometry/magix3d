import pyMagix3D as Mgx3D

ctx = Mgx3D.getStdContext()
ctx.clearSession() # Clean the session after the previous test
gm = ctx.getGeomManager()
tm = ctx.getTopoManager()
mm = ctx.getMeshManager()

# Création d'une boite avec une topologie
ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
# Création du maillage pour tous les blocs
ctx.getMeshManager().newAllBlocksMesh()