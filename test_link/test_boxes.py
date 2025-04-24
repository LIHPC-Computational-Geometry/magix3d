import pyMagix3D as Mgx3D

def test_one_hollow_box(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "B")
    # Création du cylindre Vol0001
    gm.newCylinder (Mgx3D.Point(.5, .5, 0), .2, Mgx3D.Vector(0, 0, 1), 3.600000e+02, "C")
    # Différences entre géométries avec plusieurs entités à couper
    gm.cut (["Vol0000"], ["Vol0001"])
    # Création d'un bloc topologique structuré sans projection (Vol0002)
    tm.newFreeTopoOnGeometry ("Vol0002")
    # Découpage en O-grid des blocs structurés Bl0000
    tm.splitBlocksWithOgridV2 (["Bl0000"], ["Fa0004","Fa0005"], .5, 5)
    # Destruction des entités topologiques Bl0005
    tm.destroy (["Bl0005"], True)
    # Affectation d'une projection vers Surf0006 pour les entités topologiques Fa0021 Fa0019 Fa0020 Fa0018
    tm.setGeomAssociation (["Fa0021","Fa0019","Fa0020","Fa0018"], "Surf0006", True)
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_two_hollow_box(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "A")
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10, "B")
    # Collage entre géométries avec topologies
    gm.glue (["Vol0000","Vol0001"])
    # Création du cylindre Vol0002
    gm.newCylinder (Mgx3D.Point(.5, .5, 0), .2, Mgx3D.Vector(0, 0, 1), 3.600000e+02, "C")
    # Différences entre géométries avec plusieurs entités à couper
    gm.cut (["Vol0000"], ["Vol0002"])
    # Création d'un bloc topologique non structuré sur une géométrie (Vol0003)
    tm.newUnstructuredTopoOnGeometry ("Vol0003")
    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo (Mgx3D.Point(1.5, .5, 0), .3, Mgx3D.Vector(0, 0, 1), 360, False, 1, 10, 10, 10, "I")
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_split_blocks_with_ogrid(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(2, 0, 0), Mgx3D.Point(3, 1, 1), 10, 10, 10)
    # Collage entre géométries avec topologies
    gm.glue (["Vol0000","Vol0001","Vol0002"])
    # Découpage en O-grid des blocs structurés Bl0000
    tm.splitBlocksWithOgridV2 (["Bl0000"], ["Fa0001","Fa0005","Fa0004"], .5, 10)
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()
    assert mm.getNbRegions() == 10000

    out, err = capfd.readouterr()
    assert len(err) == 0
