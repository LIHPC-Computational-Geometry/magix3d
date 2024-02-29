import sys
import pyMagix3D as Mgx3D

def test_geom_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    assert gm.getNbVolumes()==1
    assert gm.getNbSurfaces()==6
    assert gm.getNbCurves()==12
    assert gm.getNbVertices()==8


