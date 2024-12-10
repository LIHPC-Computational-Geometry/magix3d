import pyMagix3D as Mgx3D

def test_ogrid():
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()

    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage de l'arête Ar0005
    ctx.getTopoManager().splitEdge ("Ar0005", .5)
    # Découpage de l'arête Ar0001
    ctx.getTopoManager().splitEdge ("Ar0001", .5)
    # Découpage en O-grid des blocs structurés Bl0000
    ctx.getTopoManager().splitBlocksWithOgridV2 (["Bl0000"], ["Fa0000", "Fa0001"], .5, 10)
    assert(tm.getNbBlocks() == 5)
