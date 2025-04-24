import pyMagix3D as Mgx3D
import math

def test_translate(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création de la boite Vol0001
    gm.newBox (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1))
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])
    # Création d'un bloc topologique structuré sur une géométrie (Vol0000)
    tm.newStructuredTopoOnGeometry ("Vol0000")
    # Création d'un bloc topologique structuré sur une géométrie (Vol0001)
    tm.newStructuredTopoOnGeometry ("Vol0001")

    s1 = tm.getCoord("Som0001")
    assertPoints(gm, tm, 0, 0, 0)

    # Translation d'une géométrie avec sa topologie
    gm.translate (["Vol0000","Vol0001"], Mgx3D.Vector(1, 2, 3))
    assertPoints(gm, tm, 1, 2, 3)

    # Translation d'une topologie avec sa géométrie
    tm.translate (["Bl0000","Bl0001"], Mgx3D.Vector(1, 2, 3), True)
    assertPoints(gm, tm, 2, 4, 6)

    out, err = capfd.readouterr()
    assert len(err) == 0

def assertPoints(gm, tm, x, y, z):
    p1 = gm.getCoord("Pt0001")
    p2 = tm.getCoord("Som0001")
    assert p1.getX() == x
    assert p1.getY() == y
    assert p1.getZ() == z
    assert p2.getX() == x
    assert p2.getY() == y
    assert p2.getZ() == z
