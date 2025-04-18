import pyMagix3D as Mgx3D
import pytest

def test_getVertexAt():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()

    gm.newVertex(Mgx3D.Point(0, 0, 0))
    pt0 = gm.getVertexAt(Mgx3D.Point(0, 0, 0))
    assert pt0 == "Pt0000"
    pt1 = gm.getVertexAt(Mgx3D.Point(1e-14, 0, 0))
    assert pt1 == "Pt0000"
    with pytest.raises(RuntimeError) as excinfo:
        pt2 = gm.getVertexAt(Mgx3D.Point(1e-12, 0, 0))
    assert "getVertexAt impossible, on trouve 0 sommets" in str(excinfo.value)

def test_newVertex():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()

    gm.newVertex(Mgx3D.Point(1, 2, 3)/100)
    c = gm.getCoord("Pt0000")
    assert c.getX() == 0.01
    assert c.getY() == 0.02
    assert c.getZ() == 0.03

def test_translate_vertex():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()

    gm.newVertex(Mgx3D.Point(1, 0, 0))
    gm.translate(["Pt0000"], Mgx3D.Vector(0, 1, 0))
    c = gm.getCoord("Pt0000")
    assert c.getX() == 1
    assert c.getY() == 1
    assert c.getZ() == 0
