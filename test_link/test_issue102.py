import pyMagix3D as Mgx3D

def test_issue102():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager ()
    tm.newSphereWithTopo(Mgx3D.Point(0,0,0), 1, Mgx3D.Portion.HUITIEME, False, 1, 20, 20, "A")
    tm.newHollowSphereWithTopo(Mgx3D.Point(0,0,0), 1, 1.5, Mgx3D.Portion.HUITIEME, True, 10, 10, "B")
    assert gm.getNbSurfaces() == 9
    assert gm.getNbVolumes() == 2
    ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
    # Surf0008 est détruite, Surf0000 est conservée
    assert gm.getNbSurfaces() == 8
    assert gm.getNbVolumes() == 2
    # Topo::Face0000 linked with Topo::CoFace0009, Topo::CoFace0015, Topo::CoFace0021 after glue()
    # but reversed link (from Topo:CoFace to Topo::Face) was not updated => clearSession triggers an error
    ctx.clearSession()
