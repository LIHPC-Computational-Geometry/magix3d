import pyMagix3D as Mgx3D

def test_align_vertices(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage de l'arête Ar0000
    tm.splitEdge("Ar0000", .5)
    assertPoint(tm, 0, 0, 0.5)
    # Translation de la topologie seule Som0008
    tm.translate (["Som0008"], Mgx3D.Vector(0, .2, 0), False)
    assertPoint(tm, 0, 0.2, 0.5)
    # Aligne des sommets topologiques sur une droite définie par 2 points
    tm.alignVertices(Mgx3D.Point(0, 0, 1), Mgx3D.Point(0, 0, 0), ["Som0008"])
    assertPoint(tm, 0, 0, 0.5)

    out, err = capfd.readouterr()
    assert len(err) == 0

def assertPoint(tm, x, y, z):
    p = tm.getCoord("Som0008")
    assert p.getX() == x
    assert p.getY() == y
    assert p.getZ() == z
