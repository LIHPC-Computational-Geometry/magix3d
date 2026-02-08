import pyMagix3D as Mgx3D
import math

# Création d'un arc d'ellipse à partir de 3 points
# centre, départ et arrivée.
# Issue235 : l'arc d'ellipse ne passe pas par le point d'arrivée
def test_issue235():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, .5, 1))
    # Création de l'arc d'ellipse Crb0000
    gm.newArcEllipse("Pt0000", "Pt0001", "Pt0002", False)

    # On vérifie que l'arc d'ellipse passe par Pt0002
    # Création du sommet Pt0003 sur la courbe Crb0000
    ctx.getGeomManager().newVertex ("Crb0000", 1.000000e+00)
    coord_are_equals(gm, "Pt0002", "Pt0003")

    # Annulation de : Création de l'arc d'ellipse Crb0000
    ctx.undo()
    # Création de l'arc d'ellipse Crb0000
    gm.newArcEllipse("Pt0000", "Pt0001", "Pt0002", True)

    # On vérifie que l'arc d'ellipse passe par Pt0002
    # Création du sommet Pt0003 sur la courbe Crb0000
    ctx.getGeomManager().newVertex ("Crb0000", 1.000000e+00)
    coord_are_equals(gm, "Pt0002", "Pt0003")

def coord_are_equals(gm, pt1, pt2):
    c1 = gm.getCoord(pt1)
    c2 = gm.getCoord(pt2)
    assert math.isclose(c1.getX(), c2.getX(), abs_tol=1e-15)
    assert math.isclose(c1.getY(), c2.getY(), abs_tol=1e-15)
    assert math.isclose(c1.getZ(), c2.getZ(), abs_tol=1e-15)

