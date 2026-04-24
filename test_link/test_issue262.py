import pyMagix3D as Mgx3D
import math

# Issue262 : Le bloc créé oar boite englobante d'un volume peut avoir de mauvaises dimensions
# la création d'un bloc sur une sphère crée un bloc plat

def assertPoint(tm, name, x, y, z):
    p = tm.getCoord(name)
    assert math.isclose(p.getX(), x, rel_tol=1e-7)
    assert math.isclose(p.getY(), y, rel_tol=1e-7)
    assert math.isclose(p.getZ(), z, rel_tol=1e-7)

def test_issue262():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    # Création du volume Vol0000
    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER)
    # Création d'un bloc topologique structuré sans projection (Vol0000)
    tm.newFreeTopoOnGeometry ("Vol0000")
    # Annulation de : Création d'un bloc topologique structuré sans projection (Vol0000)
    ctx.undo()
    # Création d'un bloc unitaire mis dans le groupe AAA
    ctx.getTopoManager().newFreeBoundedTopoInGroup ("AAA", 3, ["Vol0000"])

    assertPoint(tm, "Som0000", -1, -1, -1)
    assertPoint(tm, "Som0007", 1, 1, 1)