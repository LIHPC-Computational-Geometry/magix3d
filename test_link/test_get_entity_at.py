import pyMagix3D as Mgx3D
import pytest

def test_getVertexAt():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
    pt0 = gm.getVertexAt(Mgx3D.Point(0, 0, 0))
    assert pt0 == "Pt0000"
    pt1 = gm.getVertexAt(Mgx3D.Point(1e-14, 0, 0))
    assert pt1 == "Pt0000"
    with pytest.raises(RuntimeError) as excinfo:
        pt2 = gm.getVertexAt(Mgx3D.Point(1e-12, 0, 0))
    assert "getVertexAt impossible, on trouve 0 sommets" in str(excinfo.value)
