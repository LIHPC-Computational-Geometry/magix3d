import pyMagix3D as Mgx3D
import pytest

def test_freeboundedtopo():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    gm.newVertex(Mgx3D.Point(-1.8, -.7, 1.2))
    gm.newVertex(Mgx3D.Point(-.7, .45, -.1))
    tm.newFreeBoundedTopoInGroup ("aaa", 3, ["Pt0000","Pt0001"])
    c = tm.getCoord("Som0007")
    assert c.getX() == -0.7
    assert c.getY() ==  0.45
    assert c.getZ() ==  1.2
