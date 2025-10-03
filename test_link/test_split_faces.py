import pyMagix3D as Mgx3D
import math

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
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des entités topologiques dépendantes
    gm.destroyWithTopo(["Vol0000"], False)

    # Découpage de 2 faces par l'arete opposée aux deux faces
    # Découpage des faces 2D structurées Fa0000 Fa0005 suivant l'arête Ar0003 avec comme ratio 3.000000e-01
    tm.splitFaces (["Fa0000","Fa0005"], "Ar0003", .3, .5)
    assert tm.getNbFaces() == 8
    ctx.undo()
    assert tm.getNbFaces() == 6
        
    # Découpage de 2 faces par l'arete adjacente aux deux faces
    # Découpage des faces 2D structurées Fa0005 Fa0001 suivant l'arête Ar0005 avec comme ratio 5.000000e-01
    tm.splitFaces (["Fa0005","Fa0001"], "Ar0005", .5, .5)
    assert tm.getNbFaces() == 8
    ctx.undo()
    assert tm.getNbFaces() == 6

    # Découpage des faces 2D structurées Fa0005 Fa0001 Fa0004 Fa0000 suivant l'arête Ar0001 avec comme ratio 3.000000e-01
    tm.splitFaces (["Fa0005","Fa0001","Fa0004","Fa0000"], "Ar0001", .3, .5)
    assert tm.getNbFaces() == 10

def test_split_faces2D_3(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des entités topologiques dépendantes
    gm.destroyWithTopo(["Vol0000"], False)
    # Découpage de toutes les faces 2D structurées
    tm.splitAllFaces ("Ar0005", .5, .5)
    assert tm.getNbFaces() == 10

def test_split_faces2D_4(capfd):
    # Test de extendSplitFaces avec une face dégénérée
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des liens topologiques
    gm.destroy(["Vol0000"], True)
    # Destruction des entités topologiques Bl0000
    ctx.getTopoManager ( ).destroy (["Bl0000"], False)
    # Découpage de la face Fa0000
    tm.splitFaces (["Fa0000"], "Ar0000", .5, 0, True)
    assert tm.getNbFaces() == 7
    # On attaque depuis la face opposée à la dégénerescence
    # Découpage de la face Fa0003 par prolongation
    tm.extendSplitFace ("Fa0003", "Som0009")
    assert tm.getNbFaces() == 8

def test_split_faces2D_4(capfd):
    # Test de splitFaces avec o-grid
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(1, 0, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(2, 0, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0001
    gm.newSegment("Pt0001", "Pt0002")
    # Création de l'arc de cercle Crb0002
    gm.newArcCircle("Pt0001", "Pt0002", "Pt0000", True)
    # Création de la surface Surf0000
    ctx.getGeomManager ( ).newPlanarSurface (["Crb0001","Crb0002","Crb0000"], "")
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    tm.newStructuredTopoOnGeometry ("Surf0000")
    # Découpage de toutes les faces 2D structurées
    tm.splitAllFaces ("Ar0001", .2, .7)
    assert tm.getNbFaces() == 4
    assertVertex(tm, "Som0006", 1.5663118960624631, 0, 0)
    assertVertex(tm, "Som0007", 1.5663118960624631, 0.4114496766047312, 0)

def test_split_faces3D_1(capfd):
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage de la face Fa0005
    tm.splitFaces (["Fa0005"], "Ar0005", .3, 0, True)
    assert tm.getNbFaces() == 7

def test_split_faces3D_2(capfd):
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage des faces Fa0005 Fa0000 suivant l'arête Ar0005 avec comme ratio 3.000000e-01
    tm.splitFaces (["Fa0005","Fa0000"], "Ar0005", .3, .5)
    assert tm.getNbFaces() == 8
    ctx.undo()
    assert tm.getNbFaces() == 6

    # cas où l'arete n'est pas adjacente à une des faces et qu'on ne peut pas itérer
    # Une seule face découpée
    # Découpage des faces Fa0001 Fa0000 suivant l'arête Ar0005 avec comme ratio 3.000000e-01
    tm.splitFaces (["Fa0001","Fa0000"], "Ar0005", .3, .5)
    assert tm.getNbFaces() == 7

def test_split_faces3D_3(capfd):
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    tm.snapVertices ("Som0007", "Som0006", True)
    # découpage d'une face dégenérée
    tm.splitFaces (["Fa0001"], "Ar0007", .3, 0, True)
    assert tm.getNbFaces() == 7

def test_projection_3d(capfd):
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # Découpage des faces structurées Fa0001 suivant l'arête Ar0007 avec comme ratio 6.200000e-01
    tm.splitFaces (["Fa0001"], "Ar0007", .62, .5, False)
    assertVertex(tm, "Som0008", 1, 0.62, 0)
    # Annulation de : Découpage des faces structurées Fa0001 suivant l'arête Ar0007 avec comme ratio 6.200000e-01
    ctx.undo()
    # Découpage des faces structurées Fa0001 suivant l'arête Ar0007 avec comme ratio 6.200000e-01
    tm.splitFaces (["Fa0001"], "Ar0007", .62, .5, True)
    assertVertex(tm, "Som0008", 1, 0.6, 0)

def test_projection_2d(capfd):
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'un bloc unitaire mis dans le groupe aaa
    tm.newFreeTopoInGroup ("aaa", 2)
    # Changement de discrétisation pour Ar0003
    emp = Mgx3D.EdgeMeshingPropertyUniform(2)
    tm.setParallelMeshingProperty (emp,"Ar0003")
    # Découpage de la face Fa0000
    tm.splitFaces (["Fa0000"], "Ar0003", .7, 0, True)
    assertVertex(tm, "Som0005", 0, 0.5, 0)
    
    # Annulation de : Découpage des faces structurées Fa0000 suivant l'arête Ar0003 avec comme ratio 7.000000e-01
    ctx.undo()
    # Découpage de la face Fa0000
    tm.splitFaces (["Fa0000"], "Ar0003", .7, 0, False)
    assertCloseVertex(tm, "Som0005", 0, 0.3, 0)

    # Annulation de : Découpage des faces structurées Fa0000 suivant l'arête Ar0003 avec comme ratio 7.000000e-01
    ctx.undo()
    # Changement de discrétisation pour Ar0003
    emp = Mgx3D.EdgeMeshingPropertyGeometric(20, 1.1)
    tm.setParallelMeshingProperty (emp,"Ar0003")
    # Découpage de la face Fa0000
    tm.splitFaces (["Fa0000"], "Ar0003", .5, 0, True)
    assertCloseVertex(tm, "Som0005", 0, 0.2782614533742, 0)

    # Annulation de : Création du maillage pour toutes les faces
    ctx.undo()
    # Découpage de la face Fa0000
    tm.splitFaces (["Fa0000"], "Ar0003", .5, 0, False)
    assertVertex(tm, "Som0005", 0, 0.5, 0)

def test_projection_2d_2faces(capfd):
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'un bloc unitaire mis dans le groupe aaa
    tm.newFreeTopoInGroup ("aaa", 2)
    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(2, 0, 0),"bbb")
    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(2, 1, 0),"bbb")
    # Création d'une face topologique structurée
    tm.newTopoEntity (["Som0000", "Som0004", "Som0005", "Som0003"], 2, "bbb")
    # Changement de discrétisation pour les arêtes Ar0003
    emp = Mgx3D.EdgeMeshingPropertyGeometric(10, 1.1)
    tm.setMeshingProperty (emp, ["Ar0003"])
    # Découpage de la face Fa0000
    tm.splitFaces (["Fa0000"], "Ar0001", .5, 0, True)
    assertCloseVertex(tm, "Som0007", 1, 0.383066910297221, 0)
    # Découpage de la face Fa0001
    tm.splitFaces (["Fa0001"], "Ar0005", .5, 0, False)
    assertVertex(tm, "Som0007", 1, 0.5, 0)
    # Pourquoi déplace-t-on le sommet au lieu d'en recréer un ?
    # Si on undo, le sommet ne revient pas à sa place originelle -> c'est un bug
    # Bug affichage. Pb de rafraîchissement. En les désaffichant et réaffichant c'est bon

def assertVertex(tm, vname, x, y, z):
    p = tm.getCoord(vname)
    assert p.getX() == x
    assert p.getY() == y
    assert p.getZ() == z

def assertCloseVertex(tm, vname, x, y, z):
    p = tm.getCoord(vname)
    assert math.isclose(p.getX(), x, abs_tol=1e-15)
    assert math.isclose(p.getY(), y, abs_tol=1e-15)
    assert math.isclose(p.getZ(), z, abs_tol=1e-15)