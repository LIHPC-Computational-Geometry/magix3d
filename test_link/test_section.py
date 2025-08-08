import os
import pyMagix3D as Mgx3D

def test_section_box1(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    ctx.clearSession() # Clean the session after the previous test

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "A")
    # Création de la boite Vol0001
    gm.newBox (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), "B")
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])
    # Création d'un bloc topologique structuré sur une géométrie (Vol0000)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Vol0000")
    # Création d'un bloc topologique structuré sur une géométrie (Vol0001)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Vol0001")

    # Création du sommet Pt0016 sur la courbe Crb0001
    gm.newVertex ("Crb0001", 5.000000e-01, "C")
    # Création du sommet Pt0017 sur la courbe Crb0003
    gm.newVertex ("Crb0003", 5.000000e-01, "C")
    # Création du sommet Pt0018 sur la courbe Crb0007
    gm.newVertex ("Crb0007", 5.000000e-01, "C")
    # Création du sommet Pt0019 sur la courbe Crb0005
    gm.newVertex ("Crb0005", 5.000000e-01, "C")

    # Création du segment Crb0024
    gm.newSegment("Pt0016", "Pt0017", "C")
    # Création du segment Crb0025
    gm.newSegment("Pt0018", "Pt0017", "C")
    # Création du segment Crb0026
    gm.newSegment("Pt0019", "Pt0018", "C")
    # Création du segment Crb0027
    gm.newSegment("Pt0016", "Pt0019", "C")
    # Création de la surface Surf0012
    gm.newPlanarSurface (["Crb0024","Crb0025","Crb0026","Crb0027"], "C")

    # Découpage suivant Ar0008 des blocs Bl0000
    tm.splitBlocks (["Bl0000"],"Ar0008", .5)
    # Section par un plan entre géométries avec topologies
    gm.section (["Vol0000"], "Surf0012")
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()
    # Sauvegarde du maillage (mli)
    filename = "section.mli2"
    mm.writeMli(filename)

    assert os.path.exists(filename)
    assert os.path.getsize(filename) > 0
    assert gm.getNbVolumes() == 3
    assert gm.getNbSurfaces() == 17
    assert tm.getNbBlocks() == 3
    assert tm.getNbFaces() == 16
    assert mm.getNbFaces() == 1200

    out, err = capfd.readouterr()
    assert len(err) == 0


def test_section_box2(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    ctx.clearSession() # Clean the session after the previous test

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "A")
    # Création de la boite Vol0001
    gm.newBox (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), "B")
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])
    # Création de la boite Vol0002
    gm.newBox (Mgx3D.Point(0, .9, 0), Mgx3D.Point(2, 1.3, 1), "C")
    # Section entre géométries
    gm.section (["Vol0000","Vol0001"], "Surf0014")

    assert gm.getNbVolumes() == 5
    assert gm.getNbSurfaces() == 26

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_section_box_by_cylinder(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    ctx.clearSession() # Clean the session after the previous test

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(-.1, -.25, -.1), Mgx3D.Point(.1, .25, .1), "A")
    # Création du cylindre Vol0001
    gm.newCylinder (Mgx3D.Point(-.25, 0, 0), 0.05, Mgx3D.Vector(.5, 0, 0), 3.600000e+02, "B")
    # Section suivant Surf0006 des entités Vol0000
    gm.section (["Vol0000"], "Surf0006")

    assert gm.getNbVolumes() == 3
    assert gm.getNbSurfaces() == 12

    out, err = capfd.readouterr()
    assert len(err) == 0