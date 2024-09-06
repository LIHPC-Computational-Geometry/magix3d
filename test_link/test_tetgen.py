import pyMagix3D as Mgx3D

def test_tetgen():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), False)
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()
