import pyMagix3D as Mgx3D
from math import *
import os

def test_split_faces2D_1(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, 1, 0))
    # Création du sommet Pt0003
    gm.newVertex (Mgx3D.Point(1, 0, 0))
    # Création d'une surface avec vecteur de points
    gm.newVerticesCurvesAndPlanarSurface ([Mgx3D.Point(0, 1, 0),Mgx3D.Point(1, 1, 0),Mgx3D.Point(1, 0, 0),Mgx3D.Point(0, 0, 0)], "")
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    tm.newStructuredTopoOnGeometry ("Surf0000")
    assert tm.getNbFaces()==1

    # Découpage des faces 2D structurées Fa0000 suivant l'arête Ar0001 avec comme ratio 5.000000e-01
    tm.splitFaces (["Fa0000"], "Ar0001", .5, .5)
    assert tm.getNbFaces()==2
    assertVertex(tm, "Som0004", 1, 0.5, 0)

def test_split_faces2D_2(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des entités topologiques dépendantes
    ctx.getGeomManager().destroyWithTopo(["Vol0000"], False)

    # Découpage de 2 faces par l'arete opposée aux deux faces
    # Découpage des faces 2D structurées Fa0000 Fa0005 suivant l'arête Ar0003 avec comme ratio 3.000000e-01
    ctx.getTopoManager().splitFaces (["Fa0000","Fa0005"], "Ar0003", .3, .5)
    assert tm.getNbFaces() == 8
    ctx.undo()
    assert tm.getNbFaces() == 6
        
    # Découpage de 2 faces par l'arete adjacente aux deux faces
    # Découpage des faces 2D structurées Fa0005 Fa0001 suivant l'arête Ar0005 avec comme ratio 5.000000e-01
    ctx.getTopoManager().splitFaces (["Fa0005","Fa0001"], "Ar0005", .5, .5)
    assert tm.getNbFaces() == 8
    ctx.undo()
    assert tm.getNbFaces() == 6

    # Découpage des faces 2D structurées Fa0005 Fa0001 Fa0004 Fa0000 suivant l'arête Ar0001 avec comme ratio 3.000000e-01
    ctx.getTopoManager().splitFaces (["Fa0005","Fa0001","Fa0004","Fa0000"], "Ar0001", .3, .5)
    assert tm.getNbFaces() == 10

def test_split_faces2D_3(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des entités topologiques dépendantes
    ctx.getGeomManager().destroyWithTopo(["Vol0000"], False)
    # Découpage de toutes les faces 2D structurées
    ctx.getTopoManager().splitAllFaces ("Ar0005", .5, .5)
    assert tm.getNbFaces() == 10

def test_split_faces2D_4(capfd):
    # Test de extendSplitFaces avec une face dégénérée
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des liens topologiques
    ctx.getGeomManager().destroy(["Vol0000"], True)
    # Destruction des entités topologiques Bl0000
    ctx.getTopoManager ( ).destroy (["Bl0000"], False)
    # Découpage de la face Fa0000
    ctx.getTopoManager().splitFace ("Fa0000", "Ar0000", .5, True)
    assert tm.getNbFaces() == 7
    # On attaque depuis la face opposée à la dégénerescence
    # Découpage de la face Fa0003 par prolongation
    ctx.getTopoManager().extendSplitFace ("Fa0003", "Som0009")
    assert tm.getNbFaces() == 8

def test_split_faces2D_4(capfd):
    # Test de splitFaces avec o-grid
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création du sommet Pt0000
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    ctx.getGeomManager().newVertex (Mgx3D.Point(1, 0, 0))
    # Création du sommet Pt0002
    ctx.getGeomManager().newVertex (Mgx3D.Point(2, 0, 0))
    # Création du segment Crb0000
    ctx.getGeomManager().newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0001
    ctx.getGeomManager().newSegment("Pt0001", "Pt0002")
    # Création de l'arc de cercle Crb0002
    ctx.getGeomManager().newArcCircle("Pt0001", "Pt0002", "Pt0000", True)
    # Création de la surface Surf0000
    ctx.getGeomManager ( ).newPlanarSurface (["Crb0001","Crb0002","Crb0000"], "")
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0000")
    # Découpage de toutes les faces 2D structurées
    ctx.getTopoManager().splitAllFaces ("Ar0001", .2, .7)
    assert tm.getNbFaces() == 4
    assertVertex(tm, "Som0006", 1.5663118960624631, 0, 0)
    assertVertex(tm, "Som0007", 1.5663118960624631, 0.4114496766047312, 0)

def test_split_faces3D_1(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage de la face Fa0005
    ctx.getTopoManager().splitFace ("Fa0005", "Ar0005", .3, True)
    assert tm.getNbFaces() == 7

def test_split_faces3D_2(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage des faces Fa0005 Fa0000 suivant l'arête Ar0005 avec comme ratio 3.000000e-01
    ctx.getTopoManager().splitFaces (["Fa0005","Fa0000"], "Ar0005", .3, .5)
    assert tm.getNbFaces() == 8
    ctx.undo()
    assert tm.getNbFaces() == 6

    # cas où l'arete n'est pas adjacente à une des faces et qu'on ne peut pas itérer
    # Une seule face découpée
    # Découpage des faces Fa0001 Fa0000 suivant l'arête Ar0005 avec comme ratio 3.000000e-01
    ctx.getTopoManager().splitFaces (["Fa0001","Fa0000"], "Ar0005", .3, .5)
    assert tm.getNbFaces() == 7

def test_split_faces3D_3(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    ctx.getTopoManager().snapVertices ("Som0007", "Som0006", True)
    # découpage d'une face dégenérée
    ctx.getTopoManager().splitFace ("Fa0001", "Ar0007", .3, True)
    assert tm.getNbFaces() == 7

def assertVertex(tm, vname, x, y, z):
    p = tm.getCoord(vname)
    assert p.getX() == x
    assert p.getY() == y
    assert p.getZ() == z