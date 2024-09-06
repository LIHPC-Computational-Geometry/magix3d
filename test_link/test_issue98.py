import pyMagix3D as Mgx3D

def test_issue98():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert len(tm.getVertices())==8
    tm.snapVertices("Som0006", "Som0004", True)
    assert len(tm.getVertices())==7
    ctx.undo()
    assert len(tm.getVertices())==8
    # Fusion de sommets Som0014 avec Som006
    tm.snapVertices("Som0006", "Som0004", True)
    assert len(tm.getVertices())==7
