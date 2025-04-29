import pyMagix3D as Mgx3D

def test_topo_box(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert tm.getNbBlocks()==1
    assert tm.getNbFaces()==6

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_fuse_geom(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création du cylindre Vol0000
    gm.newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(1, 0, 0), 1.800000e+02)
    # Création du volume Vol0001
    gm.newSphere (Mgx3D.Point(1, 0, 0), 1, Mgx3D.Portion.QUART)
    # Rotation de Vol0001 suivant  [  [ 1, 0, 0] ,  [ 1, 0, 1] , 270] 
    gm.rotate (["Vol0001"], Mgx3D.Rotation(Mgx3D.Point(1, 0, 0), Mgx3D.Point(1, 0, 1), 270))
    # Fusion Booléenne de  Vol0001 Vol0000
    gm.fuse (["Vol0001","Vol0000"])
    assert gm.getNbVolumes() == 1
    # Création d'un bloc topologique non structuré sur une géométrie (Vol0002)
    tm.newUnstructuredTopoOnGeometry ("Vol0002")
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_fuse_geom_topo(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

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

    out, err = capfd.readouterr()
    assert len(err) == 0
