import math
import pyMagix3D as Mgx3D

def test_projection_cube(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    gm = ctx.getGeomManager ()
    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(2, .5, .7), "aaa")
    # Affectation d'une projection vers Surf0005 pour les entités topologiques Som0000
    tm.setGeomAssociation (["Som0000"], "Surf0005", True)
    vert = tm.getVertexAt(Mgx3D.Point(1, 0.5, 1))
    assert vert == 'Som0000'

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_projection_cube_inner(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    gm = ctx.getGeomManager ()
    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(2, 2, 1))
    # Création de la boite Vol0001
    gm.newBox (Mgx3D.Point(.5, .5, 0), Mgx3D.Point(1.5, 1.5, 1))
    # Différences entre géométries avec plusieurs entités à couper
    gm.cut (["Vol0000"], ["Vol0001"])
    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(.7, .8, .6),"aaa")
    # Affectation d'une projection vers Surf0012 pour les entités topologiques Som0000
    tm.setGeomAssociation (["Som0000"], "Surf0012", True)
    vert = tm.getVertexAt(Mgx3D.Point(.5, 0.8, 1))
    assert vert == 'Som0000'

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_projection_disc_inner(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    gm = ctx.getGeomManager ()
    # Création du cylindre creux Vol0000
    gm.newHollowCylinder (Mgx3D.Point(0, 0, 0), .5, 1, Mgx3D.Vector(1, 0, 0), 3.600000e+02)
    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(.1, .2, .3),"aaa")
    # Affectation d'une projection vers Surf0002 pour les entités topologiques Som0000
    tm.setGeomAssociation (["Som0000"], "Surf0002", True)
    y = 0.5*math.cos(math.atan(.3/.2))
    z = 0.5*math.sin(math.atan(.3/.2))
    vert = tm.getVertexAt(Mgx3D.Point(0, y, z))
    assert vert == 'Som0000'

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_projection_arc_circle(capfd):
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
