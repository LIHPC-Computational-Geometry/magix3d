import sys
import pyMagix3D as Mgx3D

def test_issue98():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()
    mm = ctx.getMeshManager()
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert len(ctx.getTopoManager().getVertices())==8
    tm.snapVertices("Som0006", "Som0004", True)
    assert len(ctx.getTopoManager().getVertices())==7
    ctx.undo()
    assert len(ctx.getTopoManager().getVertices())==8
    # Fusion de sommets Som0014 avec Som006
    ctx.getTopoManager().snapVertices("Som0006", "Som0004", True)
    assert len(ctx.getTopoManager().getVertices())==7
