import sys
import pyMagix3D as Mgx3D

# Issue43 : fusion d'une arete topo + undo => faces topos incorrectes
# le problème est le même pour toutes les fusion topo : block, face, arete, sommet
def test_issue43():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    tm.newBoxWithTopo(Mgx3D.Point(0,0,0), Mgx3D.Point(1,1,1), 10, 10, 10)
    tm.newBoxWithTopo(Mgx3D.Point(2,0,0), Mgx3D.Point(3,1,1), 10, 10, 10)

    # Fusion sommet
    assert('Som0008' in tm.getInfos('Fa0006', 2).vertices())
    assert('Som0004' not in tm.getInfos('Fa0006', 2).vertices())
    tm.fuse2Vertices ("Som0004","Som0008")
    assert('Som0008' not in tm.getInfos('Fa0006', 2).vertices())
    assert('Som0004' in tm.getInfos('Fa0006', 2).vertices())
    ctx.undo()
    assert('Som0008' in tm.getInfos('Fa0006', 2).vertices())
    assert('Som0004' not in tm.getInfos('Fa0006', 2).vertices())

    # Fusion arete
    assert('Som0006' in tm.getInfos('Fa0005', 2).vertices())
    assert('Som0010' not in tm.getInfos('Fa0005', 2).vertices())
    tm.fuse2Edges("Ar0013","Ar0005")
    assert('Som0006' not in tm.getInfos('Fa0005', 2).vertices())
    assert('Som0010' in tm.getInfos('Fa0005', 2).vertices())
    ctx.undo()
    assert('Som0006' in tm.getInfos('Fa0005', 2).vertices())
    assert('Som0010' not in tm.getInfos('Fa0005', 2).vertices())
