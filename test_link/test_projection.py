import math
import pyMagix3D as Mgx3D

def test_projection_cube():
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

def test_projection_cube_inner():
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

def test_projection_disc_inner():
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