import pyMagix3D as Mgx3D

def test_vertex_geom_topo():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    gm.newVertex (Mgx3D.Point(0, 0, 0))
    tm.newTopoOnGeometry ("Pt0000")
    p1 = gm.getCoord("Pt0000")
    p2 = tm.getCoord("Som0000")
    assert p1.getX() == p2.getX()
    assert p1.getY() == p2.getY()
    assert p1.getZ() == p2.getZ()
