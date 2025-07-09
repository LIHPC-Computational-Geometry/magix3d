import pyMagix3D as Mgx3D

# Vérification que le maillage Tetra fonctionne
def test_issue208():
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
