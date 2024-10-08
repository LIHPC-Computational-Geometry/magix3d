import pyMagix3D as Mgx3D

def test_circle_by_center():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    gm.newArcCircle(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 0, 0), True)

    assert gm.getNbCurves()==1

def test_circle_by_3points():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    gm.newArcCircle(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 0, 0), Mgx3D.Point(1, 1, 0))
    gm.newArcCircle(Mgx3D.Point(0, 0, 1), Mgx3D.Point(1, 0, 1), Mgx3D.Point(1, 1, 1),"aaa")

    assert gm.getNbCurves()==2