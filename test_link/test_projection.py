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

def test_projection_glued_curves(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    # Création du cylindre creux Vol0000
    ctx.getGeomManager().newHollowCylinder (Mgx3D.Point(0, 0, 0), .5, 1, Mgx3D.Vector(10, 0, 0), 9.000000e+01, "A")
    # Copie de  Vol0000
    ctx.getGeomManager ( ).copy (["Vol0000"], True,"B")
    # Rotation de Vol0000 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 90] 
    ctx.getGeomManager ( ).rotate (["Vol0000"], Mgx3D.RotX(90))
    # Fusion Booléenne de  Vol0000 Vol0001
    ctx.getGeomManager ( ).fuse (["Vol0000","Vol0001"])
    # Fusion de surfaces Surf0000 Surf0006
    ctx.getGeomManager ( ).joinSurfaces (["Surf0000","Surf0006"])
    # Fusion de surfaces Surf0001 Surf0007
    ctx.getGeomManager ( ).joinSurfaces (["Surf0001","Surf0007"])
    # Fusion de courbes Crb0000 Crb0012
    ctx.getGeomManager ( ).joinCurves (["Crb0000","Crb0012"])
    # Création d'un bloc topologique structuré sans projection (Vol0002)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0002")
    # Affectation d'une projection vers Crb0024 pour les entités topologiques Som0001 Som0005 Ar0009
    ctx.getTopoManager ( ).setGeomAssociation (["Som0001","Som0005","Ar0009"], "Crb0024", True)
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()
    # Annulation de : Création du maillage pour tous les blocs
    ctx.undo()

    # Copie de  Crb0024
    ctx.getGeomManager ( ).copy (["Crb0024"], False,"")
    # Translation de Crb0025 suivant  [ .5, 0, 0] 
    ctx.getGeomManager ( ).translate (["Crb0025"], Mgx3D.Vector(.5, 0, 0))
    # Affectation d'une projection vers Crb0025 pour les entités topologiques Som0001 Som0005 Ar0009
    ctx.getTopoManager ( ).setGeomAssociation (["Som0001","Som0005","Ar0009"], "Crb0025", True)
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    out, err = capfd.readouterr()
    assert len(err) == 0
