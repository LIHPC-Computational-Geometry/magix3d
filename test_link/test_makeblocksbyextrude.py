import pyMagix3D as Mgx3D

def test_makeblocksbyextrude():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    tm.newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, True, .5, 10, 10)
    tm.makeBlocksByExtrude (["Fa0010", "Fa0040"],Mgx3D.Vector(-1, 0, 1))
    gg = ctx.getGroupManager()
    blocklist = gg.getTopoBlocks("Hors_Groupe_3D", 3)
    assert len(blocklist) == 9
