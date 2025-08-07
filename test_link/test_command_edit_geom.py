import pyMagix3D as Mgx3D

def test_command_join_entities(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(2, 0, 0), Mgx3D.Point(3, 1, 1), 10, 10, 10)
    assert gm.getNbSurfaces()==18
    assert gm.getNbCurves()==36

    # Fusion booléenne entre géométries avec topologies
    gm.fuse (["Vol0000","Vol0001", "Vol0002"])
    assert gm.getNbSurfaces()==14
    assert gm.getNbCurves()==28

    # Fusion de surfaces avec maj de la topologie
    gm.joinSurfaces (["Surf0005","Surf0011"])
    assert gm.getNbSurfaces()==13
    assert gm.getNbCurves()==27
    # Annulation de : Fusion de surfaces avec maj de la topologie
    ctx.undo()
    assert gm.getNbSurfaces()==14
    assert gm.getNbCurves()==28
    # Rejeu de : Fusion de surfaces avec maj de la topologie
    ctx.redo()
    assert gm.getNbSurfaces()==13
    assert gm.getNbCurves()==27

    # Fusion de surfaces avec maj de la topologie
    gm.joinSurfaces (["Surf0009","Surf0003"])
    assert gm.getNbSurfaces()==12
    assert gm.getNbCurves()==26
    # Annulation de : Fusion de surfaces avec maj de la topologie
    ctx.undo()
    assert gm.getNbSurfaces()==13
    assert gm.getNbCurves()==27
    # Rejeu de : Fusion de surfaces avec maj de la topologie
    ctx.redo()
    assert gm.getNbSurfaces()==12
    assert gm.getNbCurves()==26

    # Fusion de courbes avec maj de la topologie
    gm.joinCurves (["Crb0011","Crb0023"])
    assert gm.getNbCurves()==25
    # Annulation de : Fusion de courbes avec maj de la topologie
    ctx.undo()
    assert gm.getNbCurves()==26
    # Rejeu de : Fusion de courbes avec maj de la topologie
    ctx.redo()
    assert gm.getNbCurves()==25

def test_command_scaling(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assertPoint(gm, "Pt0001", 0, 0, 0)
    assertPoint(gm, "Pt0006", 1, 1, 1)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(.2, .2, .2), Mgx3D.Point(.8, .8, .8), 10, 10, 10)
    assertPoint(gm, "Pt0014", 0.8, 0.8, 0.8)

    # Homothétie d'une géométrie avec sa topologie
    gm.scale (["Vol0000","Vol0001"], 2.000000e+00)
    assertPoint(gm, "Pt0006", 2, 2, 2)
    assertPoint(gm, "Pt0014", 1.6, 1.6, 1.6)
    # Annulation de : Homothétie d'une géométrie avec sa topologie
    ctx.undo()
    assertPoint(gm, "Pt0006", 1, 1, 1)
    assertPoint(gm, "Pt0014", 0.8, 0.8, 0.8)
    # Rejeu de : Homothétie d'une géométrie avec sa topologie
    ctx.redo()
    assertPoint(gm, "Pt0006", 2, 2, 2)
    assertPoint(gm, "Pt0014", 1.6, 1.6, 1.6)

    # Homothétie de tout
    gm.scaleAll(5.000000e-01, Mgx3D.Point(2, 2, 2))
    assertPoint(gm, "Pt0001", 1, 1, 1)
    # Annulation de : Homothétie de tout
    ctx.undo()
    assertPoint(gm, "Pt0001", 0, 0, 0)
    # Rejeu de : Homothétie de tout
    ctx.redo()
    assertPoint(gm, "Pt0001", 1, 1, 1)

def test_command_scaling_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'un bloc topologique structuré sur une géométrie (Vol0000)
    tm.newStructuredTopoOnGeometry ("Vol0000")
    assertPoint(tm, "Som0006", 1, 1, 1)

    # Homothétie d'une géométrie avec sa topologie
    gm.scale (["Vol0000"], 3.000000e+00)
    assertPoint(tm, "Som0006", 3, 3, 3)

def test_command_remove(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert gm.getNbSurfaces()==6
    assert gm.getNbCurves()==12

    # Suppression d'entités géométriques et suppression des liens topologiques
    gm.destroy(["Crb0000"], True)
    assert gm.getNbSurfaces()==4
    assert gm.getNbCurves()==11
    # Annulation de : Suppression d'entités géométriques et suppression des liens topologiques
    ctx.undo()
    assert gm.getNbSurfaces()==6
    assert gm.getNbCurves()==12
    # Rejeu de : Suppression d'entités géométriques et suppression des liens topologiques
    ctx.redo()
    assert gm.getNbSurfaces()==4
    assert gm.getNbCurves()==11

    # Suppression d'entités géométriques et suppression des liens topologiques
    gm.destroy(["Surf0001"], True)
    assert gm.getNbSurfaces()==3
    assert gm.getNbCurves()==10
    # Annulation de : Suppression d'entités géométriques et suppression des liens topologiques
    ctx.undo()
    assert gm.getNbSurfaces()==4
    assert gm.getNbCurves()==11
    # Rejeu de : Suppression d'entités géométriques et suppression des liens topologiques
    ctx.redo()
    assert gm.getNbSurfaces()==3
    assert gm.getNbCurves()==10

def test_command_mirroring(capfd):
    #Ce test prend aussi en charge l'appel à CommandGeomCopy
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
    # Collage entre géométries avec topologies
    gm.glue (["Vol0000","Vol0001"])
    assert gm.getNbSurfaces()==11
    assert gm.getNbCurves()==20

    # Symétrie d'une copie d'une géométrie
    gm.copyAndMirror (["Vol0001"], Mgx3D.Plane(Mgx3D.Point(0, 0, 0), Mgx3D.Vector(0, 0, 1)), False,"")

    assert gm.getNbSurfaces()==17
    assert gm.getNbCurves()==32
    # Annulation de : Symétrie d'une copie d'une géométrie
    ctx.undo()
    assert gm.getNbSurfaces()==11
    assert gm.getNbCurves()==20
    # Rejeu de : Symétrie d'une copie d'une géométrie
    ctx.redo()
    assert gm.getNbSurfaces()==17
    assert gm.getNbCurves()==32

def test_command_common(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(.2, .2, .2), Mgx3D.Point(.8, .8, .8), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(.2, .8, .2), Mgx3D.Point(.8, 1.4, .8), 10, 10, 10)
    # Collage entre géométries avec topologies
    gm.glue (["Vol0001","Vol0002"])
    assert gm.getNbSurfaces()==17
    assert gm.getNbCurves()==32

    # Intersection booléenne entre géométries avec topologies
    gm.common (["Vol0000","Vol0001"])
    assert gm.getNbSurfaces()==11
    assert gm.getNbCurves()==20
    # Annulation de : Intersection booléenne entre géométries avec topologies
    ctx.undo()
    assert gm.getNbSurfaces()==17
    assert gm.getNbCurves()==32
    # Rejeu de : Intersection booléenne entre géométries avec topologies
    ctx.redo()
    assert gm.getNbSurfaces()==11
    assert gm.getNbCurves()==20

def test_command_common2d(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(.2, .2, .2), Mgx3D.Point(.8, .8, .8), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(.2, .8, .2), Mgx3D.Point(.8, 1.4, .8), 10, 10, 10)
    # Collage entre géométries avec topologies
    gm.glue (["Vol0001","Vol0002"])
    assert gm.getNbSurfaces()==17
    assert gm.getNbCurves()==32

    # Intersection booléenne entre 2 géométries 1D ou 2D avec topologies
    gm.common2D("Surf0003", "Surf0016", "")
    assert gm.getNbSurfaces()==15
    assert gm.getNbCurves()==25
    # Annulation de : Intersection booléenne entre 2 géométries 1D ou 2D, sans destruction
    ctx.undo()
    assert gm.getNbSurfaces()==17
    assert gm.getNbCurves()==32
    # Rejeu de : Intersection booléenne entre 2 géométries 1D ou 2D, sans destruction
    ctx.redo()
    assert gm.getNbSurfaces()==15
    assert gm.getNbCurves()==25

def assertPoint(m, point, x, y, z):
    p = m.getCoord(point)
    assert p.getX() == x
    assert p.getY() == y
    assert p.getZ() == z