import sys
import pyMagix3D as Mgx3D

def test_topo_box():
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()
    mm = ctx.getMeshManager()
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert tm.getNbBlocks()==1
    assert tm.getNbFaces()==6

    #This last command is mandatory to clean the session for the next test
    ctx.clearSession()
