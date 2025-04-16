import pyMagix3D as Mgx3D

def test_topo_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()

    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert tm.getNbBlocks()==1
    assert tm.getNbFaces()==6

def test_fuse_geom_topo():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager ()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo(Mgx3D.Point(1, 0, 0), Mgx3D.Point(2.5, 1, 1), 15, 10, 10)

    # Fusion booléenne entre géométries avec topologies
    assert gm.getNbVolumes() == 2
    assert gm.getNbSurfaces() == 12
    gm.fuse(["Vol0000","Vol0001"])
    assert gm.getNbVolumes() == 1
    assert gm.getNbSurfaces() == 10

    # Collage entre blocs Bl0000 et Bl0001
    assert tm.getNbBlocks() == 2
    assert tm.getNbFaces() == 11
    tm.fuse2Blocks("Bl0000","Bl0001")
    assert tm.getNbBlocks() == 1
    assert tm.getNbFaces() == 10
