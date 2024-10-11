import pyMagix3D as Mgx3D

def test_circle_by_center():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    p0 = gm.newVertex (Mgx3D.Point(0, 0, 0))
    p1 = gm.newVertex (Mgx3D.Point(1, 0, 0))
    p2 = gm.newVertex (Mgx3D.Point(2, 0, 0))
    gm.newArcCircle(p0, p1, p2, True)

    assert gm.getNbCurves()==1

def test_circle_by_3points():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    p0 = gm.newVertex (Mgx3D.Point(0, 0, 0))
    p1 = gm.newVertex (Mgx3D.Point(1, 0, 0))
    p2 = gm.newVertex (Mgx3D.Point(1, 1, 0))
    gm.newArcCircle(p0, p1, p2)

    p3 = gm.newVertex (Mgx3D.Point(0, 0, 1))
    p4 = gm.newVertex (Mgx3D.Point(1, 0, 1))
    p5 = gm.newVertex (Mgx3D.Point(1, 1, 1))
    gm.newArcCircle(p3, p4, p5,"aaa")

    assert gm.getNbCurves()==2