import pyMagix3D as Mgx3D

def test_issue72():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    #  Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    #  Découpage en O-grid des blocs structurés Bl0000
    tm.splitBlocksWithOgridV2 (["Bl0000"], [], .5, 10)
    # Fusion de sommets Som0014 avec Som006
    tm.snapVertices("Som0014", "Som0006", True)