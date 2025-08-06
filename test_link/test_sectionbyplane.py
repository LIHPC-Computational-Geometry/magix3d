import pyMagix3D as Mgx3D

# Tests de la fonction sectionByPlane du GeomManager
# pour laquelle un certain nombres de bug a été trouvé
# (cf. Issue#208)

def test_3_boxes():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création de la boite Vol0001
    gm.newBox(Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1, 1))
    # Création de la boite Vol0002
    gm.newBox(Mgx3D.Point(1.5, 0, 0), Mgx3D.Point(3, 1, 1))
    # Collage entre Vol0000 Vol0001 Vol0002
    gm.glue(["Vol0000","Vol0001","Vol0002"])
    gm.sectionByPlane(["Vol0000"], Mgx3D.Plane(Mgx3D.Point(0, 0, .2), Mgx3D.Vector(0, 0, 1)), "")

    # Ce 2e sectionByPlane fait disparaitre 2 pts : Pt0024 et Pt0025$
    # Du coup, Surf0022 ne comporte que Pt0026 et Pt0027. Il lui manque 2 pts.
    gm.sectionByPlane(["Vol0002"], Mgx3D.Plane(Mgx3D.Point(0, 0, .2), Mgx3D.Vector(0, 0, 1)), "")
    surf22_vertices = gm.getInfos("Surf0022", 2).vertices()
    assert len(surf22_vertices) == 4
    assert "Pt0026" in surf22_vertices
    assert "Pt0027" in surf22_vertices

    # Juste pour tester undo/redo
    assert gm.getNbVolumes() == 5
    ctx.undo()
    assert gm.getNbVolumes() == 4
    ctx.undo()
    assert gm.getNbVolumes() == 3
    ctx.redo()
    assert gm.getNbVolumes() == 4
    ctx.redo()
    assert gm.getNbVolumes() == 5

def test_3_boxes_two_sections():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création de la boite Vol0001
    gm.newBox(Mgx3D.Point(1, 0, 0), Mgx3D.Point(1.5, 1, 1))
    # Création de la boite Vol0002
    gm.newBox(Mgx3D.Point(1.5, 0, 0), Mgx3D.Point(3, 1, 1))
    # Collage entre Vol0000 Vol0001 Vol0002
    gm.glue(["Vol0000","Vol0001","Vol0002"])
    gm.sectionByPlane(["Vol0000", "Vol0002"], Mgx3D.Plane(Mgx3D.Point(0, 0, .2), Mgx3D.Vector(0, 0, 1)), "")

    # Vol0001 se trouve découpé à tort => 6 volumes au lieu de 5
    assert gm.getNbVolumes() == 5

    # Juste pour tester undo/redo
    ctx.undo()
    assert gm.getNbVolumes() == 3
    ctx.redo()
    assert gm.getNbVolumes() == 5

def test_nested_spheres():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la sphère creuse Vol0000
    gm.newHollowSphere (Mgx3D.Point(0, 0, 0), 1, 2, Mgx3D.Portion.ENTIER)
    # Création du volume Vol0001
    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER)
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])
    # Section par un plan de Vol0000 suivant [ [ 0, 0, -6.64992e-1] , [ 0, 0, 1] ]
    gm.sectionByPlane (["Vol0000"], Mgx3D.Plane(Mgx3D.Point(0, 0, -6.64992e-1), Mgx3D.Vector(0, 0, 1)), "")

    # Vol0001 se trouve découpé à tort => 4 volumes au lieu de 3
    assert gm.getNbVolumes() == 3
    assert gm.getNbSurfaces() == 5

    # Annulation de : Section par un plan de Vol0000 suivant [ [ 0, 0, -6.64992e-1] , [ 0, 0, 1] ]
    ctx.undo()
    # Section par un plan entre géométries avec topologies
    gm.sectionByPlane (["Vol0000","Vol0001"], Mgx3D.Plane(Mgx3D.Point(0, 0, -6.64992e-1), Mgx3D.Vector(0, 0, 1)), "")

    # Des volumes sont créés en double => 6 volumes au lieu de 4
    assert gm.getNbVolumes() == 4
    assert gm.getNbSurfaces() == 6

def test_nested_surfaces():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création d'une boite avec une topologie
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'une boite avec une topologie
    gm.newBox(Mgx3D.Point(-1, -1, -1), Mgx3D.Point(1, 2, 2))
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])
    # Destruction de  Vol0002 Vol0000
    gm.destroy(["Vol0002", "Vol0000"], False)
    assert gm.getNbSurfaces() == 12

    # Section par un plan de Surf0012 suivant  [  [ 1, .5, 0] ,  [ 0, 1, 0] ] 
    gm.sectionByPlane (["Surf0012"], Mgx3D.Plane(Mgx3D.Point(1, .5, 0), Mgx3D.Vector(0, 1, 0)), "")
    assert gm.getNbSurfaces() == 13
    ctx.undo()
    assert gm.getNbSurfaces() == 12
    gm.sectionByPlane (["Surf0012", "Surf0001"], Mgx3D.Plane(Mgx3D.Point(1, .5, 0), Mgx3D.Vector(0, 1, 0)), "")
    assert gm.getNbSurfaces() == 14
