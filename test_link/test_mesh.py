import sys
import pyMagix3D as Mgx3D

def test_topo_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    mm = ctx.getMeshManager()
    #create a single box with one block
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # mesh blocks now
    mm.newAllBlocksMesh()
    assert mm.getNbNodes()==1331
    assert mm.getNbFaces()==600
    assert mm.getNbRegions()==1000
