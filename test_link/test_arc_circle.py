import pyMagix3D as Mgx3D

def test_circle_by_center():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    p0 = gm.newVertex (Mgx3D.Point(0, 0, 0))
    p1 = gm.newVertex (Mgx3D.Point(1, 0, 0))
    p2 = gm.newVertex (Mgx3D.Point(2, 0, 0))
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0002", True)

    assert gm.getNbCurves()==1

def test_circle_by_3points():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    p0 = gm.newVertex (Mgx3D.Point(0, 0, 0))
    p1 = gm.newVertex (Mgx3D.Point(1, 0, 0))
    p2 = gm.newVertex (Mgx3D.Point(1, 1, 0))
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0002")

    p3 = gm.newVertex (Mgx3D.Point(0, 0, 1))
    p4 = gm.newVertex (Mgx3D.Point(1, 0, 1))
    p5 = gm.newVertex (Mgx3D.Point(1, 1, 1))
    gm.newArcCircle("Pt0003", "Pt0004", "Pt0005","aaa")

    assert gm.getNbCurves()==2