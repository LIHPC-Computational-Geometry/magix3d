import pyMagix3D as Mgx3D
import math

def test_projection(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex(Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex(Mgx3D.Point(1, 0, 0))
    # Création du sommet Pt0002
    gm.newVertex(Mgx3D.Point(0, 1, 0))
    # Création de l'arc de cercle Crb0000
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0002", True)
    # Création du segment Crb0001
    gm.newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0002
    gm.newSegment("Pt0002", "Pt0000")
    # Création de la surface Surf0000
    gm.newPlanarSurface(["Crb0000","Crb0001","Crb0002"], "PLAN1")
    # Translation
    gm.translate(["Surf0000"], Mgx3D.Vector(0.1, 0.2, 0.1))
    # Rotation de Surf0000 suivant  [  [ 0, 0, 0] ,  [ 0, 1, 0] , 10] 
    gm.rotate(["Surf0000"], Mgx3D.RotY(10))
    # Création du sommet Pt0003
    gm.newVertex(Mgx3D.Point(-.5, .2, 0))
    # Création du sommet Pt0004 par projection sur Crb0000
    gm.newVertex("Pt0003", "Crb0000")

    c1 = gm.getCoord("Pt0002")
    c2 = gm.getCoord("Pt0004")
    assert math.isclose(c1.getX(), c2.getX(), abs_tol=1e-7)
    assert math.isclose(c1.getY(), c2.getY(), abs_tol=1e-7)
    assert math.isclose(c1.getZ(), c2.getZ(), abs_tol=1e-7)

    out, err = capfd.readouterr()
    assert len(err) == 0
