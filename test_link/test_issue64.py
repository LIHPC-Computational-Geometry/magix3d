import pyMagix3D as Mgx3D

# Transfini par défaut à la place de directional
def test_issue64():
    ctx = Mgx3D.getStdContext()

    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert len(tm.getTransfiniteMeshLawBlocks()) == 1
    assert len(tm.getTransfiniteMeshLawFaces()) == 6
