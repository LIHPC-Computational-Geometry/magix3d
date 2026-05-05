import pyMagix3D as Mgx3D

def test_issue72(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    #  Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    #  Découpage en O-grid des blocs structurés Bl0000
    tm.splitBlocksWithOgridV2 (["Bl0000"], [], .5, 10)
    # Fusion de sommets Som0014 avec Som006
    tm.snapVertices("Som0014", "Som0006", True)
    assert(tm.getNbBlocks() == 7)
    assert(tm.getNbFaces() == 24)

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_issue55(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    tm.splitBlock("Bl0000", "Ar0011",.5)
    tm.splitFace("Fa0014","Ar0023",.3,True)
    tm.splitFace("Fa0016","Ar0025",.5,True)
    tm.splitEdge("Ar0033",.3)
    tm.splitEdge("Ar0035",.5)
    tm.splitBlocksWithOgridV2(["Bl0001","Bl0002"],[],.5,10)
    assert(tm.getNbBlocks() == 12)
    assert(tm.getNbFaces() == 47)

    out, err = capfd.readouterr()
    assert len(err) == 0

# 4 blocs placés en forme de T. 
# Pointe formée sur l'arête centrale en haut du T
def test_issue223(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    tm.newFreeTopoInGroup("aaa", 3)
    tm.makeBlocksByExtrude(["Fa0001"], Mgx3D.Vector(1,0,0))
    tm.splitBlocksWithOgridV2(["Bl0001"], ["Fa0010", "Fa0009", "Fa0006", "Fa0008"], .5, 10)
    tm.splitBlocksWithOgridV2(["Bl0000"], ["Fa0003", "Fa0005", "Fa0002", "Fa0000"], .5, 10)
    tm.splitBlocksWithOgridV2(["Bl0006", "Bl0005", "Bl0002", "Bl0003"], [], .5, 10)
    assert(tm.getNbBlocks() == 24)
    assert(tm.getNbFaces() == 83)

    out, err = capfd.readouterr()
    assert len(err) == 0

