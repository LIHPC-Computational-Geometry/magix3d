import pyMagix3D as Mgx3D

def test_sectionbyplane():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    gm = ctx.getGeomManager ()
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    gm.newBox(Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1, 1))
    gm.newBox(Mgx3D.Point(1.5, 0, 0), Mgx3D.Point(3, 1, 1))
    gm.glue (["Vol0000","Vol0001","Vol0002"])
    gm.sectionByPlane (["Vol0000"], Mgx3D.Plane(Mgx3D.Point(0, 0, .2), Mgx3D.Vector(0, 0, 1)), "")
    gm.sectionByPlane (["Vol0002"], Mgx3D.Plane(Mgx3D.Point(0, 0, .2), Mgx3D.Vector(0, 0, 1)), "")
    nbpoints = gm.getNbVertices()
    nbcurves = gm.getNbCurves()
    assert nbpoints == 24
    assert nbcurves == 44
