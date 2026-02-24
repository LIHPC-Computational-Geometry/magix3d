import pyMagix3D as Mgx3D
import pytest
import math
import LimaScripting as lima
def l2_norme(n0, n1):
    return math.sqrt( pow(n1.x()-n0.x(),2) + pow(n1.y()-n0.y(),2) + pow(n1.z()-n0.z(),2) )

def test_law_exponential_withoutAssoc(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.0001
    s1_ar0004 = 0.18

    # Création d'un bloc unitaire mis dans le groupe
    ctx.getTopoManager().newFreeTopoInGroup ("test", 3)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, True, True, s1_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])

    # Changement de discrétisation pour les arêtes Ar0004
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, False, True, s1_ar0004)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0004"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_exponential.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar00000
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) < eps )

    # test of first mesh edge size of topo edge Ar00004
    n2  = mesh_lima.noeud(2)
    n52 = mesh_lima.noeud(52)
    assert( abs(l2_norme(n2, n52) - s1_ar0004) < eps )


def test_law_exponential(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.02
    s1_ar0004 = 0.04

    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, True, True, s1_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])

    # Changement de discrétisation pour les arêtes Ar0004
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, False, True, s1_ar0004)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0004"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_exponential.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar00000
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) < eps )

    # test of first mesh edge size of topo edge Ar00004
    n5  = mesh_lima.noeud(5)
    n52 = mesh_lima.noeud(52)
    assert( abs(l2_norme(n5, n52) - s1_ar0004) < eps )


def test_law_exponential_onCurve(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0001 = 0.00007
    s1_ar0003 = 0.005

    # Création du cylindre Vol0000
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, -1), 1, Mgx3D.Vector(0, 0, 2), 3.600000e+02)
    # Création de la boite Vol0001
    ctx.getGeomManager().newBox (Mgx3D.Point(1, 0, -1), Mgx3D.Point(-1, -2, 1))
    # Fusion Booléenne de  Vol0001 Vol0000
    ctx.getGeomManager ( ).fuse (["Vol0001","Vol0000"])
    # Création d'un bloc topologique structuré sans projection (Vol0002)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0002")
    # Affectation d'une projection vers Pt0000 pour les entités topologiques Som0007
    ctx.getTopoManager ( ).setGeomAssociation (["Som0007"], "Pt0000", True)
    # Affectation d'une projection vers Pt0004 pour les entités topologiques Som0006
    ctx.getTopoManager ( ).setGeomAssociation (["Som0006"], "Pt0004", True)
    # Affectation d'une projection vers Crb0015 pour les entités topologiques Ar0003
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0003"], "Crb0015", True)
    # Affectation d'une projection vers Pt0001 pour les entités topologiques Som0003
    ctx.getTopoManager ( ).setGeomAssociation (["Som0003"], "Pt0001", True)
    # Affectation d'une projection vers Pt0005 pour les entités topologiques Som0002
    ctx.getTopoManager ( ).setGeomAssociation (["Som0002"], "Pt0005", True)
    # Affectation d'une projection vers Crb0017 pour les entités topologiques Ar0001
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0001"], "Crb0017", True)

    # Changement de discrétisation pour les arêtes Ar0001
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, True, True, s1_ar0001)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0001"])
    # Changement de discrétisation pour les arêtes Ar0003
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, False, True, s1_ar0003)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0003"])
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_exponential_curve.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar00001
    n2  = mesh_lima.noeud(2)
    n17 = mesh_lima.noeud(17)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n2, n17) - s1_ar0001) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n2, n17) - 0.0000594519721850694) < eps )

    # test of first mesh edge size of topo edge Ar00003
    n7  = mesh_lima.noeud(7)
    n43 = mesh_lima.noeud(43)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n7, n43) - s1_ar0003) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n7, n43) - 0.00473345420702485) < eps )


def test_meshing_law_geometric_onPortionOfCurve(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.3
    s1_ar0003 = 0.06

    # Création du cylindre Vol0000
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(0, 0, 1), 3.600000e+02)
    # Création d'un bloc topologique structuré sans projection (Vol0000)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0000")
    # Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0000
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0000"], "Crb0002", True)
    # Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0003
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0003"], "Crb0000", True)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, True, True, s1_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    # Changement de discrétisation pour les arêtes Ar0003
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, False, True, s1_ar0003)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0003"])
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_exponential_curve2.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar00000
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n0, n8) - 0.331676774653951) < eps )

    # test of first mesh edge size of topo edge Ar00003
    n7  = mesh_lima.noeud(7)
    n43 = mesh_lima.noeud(43)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n7, n43) - s1_ar0003) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n7, n43) - 0.0666309120859813) < eps )


def test_meshing_law_geometric_onSurface(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0023 = 0.0042
    s1_ar0022 = 2.3

    # Création du cylindre Vol0000
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(0, 0, 4), 3.600000e+02)
    # Création du cylindre Vol0001
    ctx.getGeomManager().newCylinder (Mgx3D.Point(1.5, 0, 0), 1, Mgx3D.Vector(0, 0, 4), 3.600000e+02)
    # Fusion Booléenne de  Vol0001 Vol0000
    ctx.getGeomManager ( ).fuse (["Vol0001","Vol0000"])
    # Fusion de surfaces Surf0007 Surf0010 Surf0012
    ctx.getGeomManager ( ).joinSurfaces (["Surf0007","Surf0010","Surf0012"])
    # Fusion de surfaces Surf0008 Surf0011 Surf0014
    ctx.getGeomManager ( ).joinSurfaces (["Surf0008","Surf0011","Surf0014"])
    # Fusion de surfaces Surf0006 Surf0013
    ctx.getGeomManager ( ).joinSurfaces (["Surf0006","Surf0013"])
    # Fusion de surfaces Surf0017 Surf0009
    ctx.getGeomManager ( ).joinSurfaces (["Surf0017","Surf0009"])
    # Fusion de courbes Crb0006 Crb0016
    ctx.getGeomManager ( ).joinCurves (["Crb0006","Crb0016"])
    # Fusion de courbes Crb0018 Crb0009 Crb0008
    ctx.getGeomManager ( ).joinCurves (["Crb0018","Crb0009","Crb0008"])
    # Fusion de courbes Crb0020 Crb0007
    ctx.getGeomManager ( ).joinCurves (["Crb0020","Crb0007"])
    # Création d'un bloc topologique structuré sans projection (Vol0002)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0002")
    # Découpage suivant Ar0011 des blocs Bl0000
    ctx.getTopoManager().splitBlocks (["Bl0000"],"Ar0011", .5)
    # Affectation d'une projection vers Surf0018 pour les entités topologiques Ar0023
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0023"], "Surf0018", True)
    # Affectation d'une projection vers Surf0018 pour les entités topologiques Ar0022
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0022"], "Surf0018", True)

    # Changement de discrétisation pour les arêtes Ar0023
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, True, True, s1_ar0023)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0023"])
    # Changement de discrétisation pour les arêtes Ar0022
    emp = Mgx3D.EdgeMeshingPropertyExponential(10, 1.1, False, True, s1_ar0022)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0022"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_exponential_surface.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar0023
    n9   = mesh_lima.noeud(9)
    n143 = mesh_lima.noeud(143)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n9, n143) - s1_ar0023) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n9, n143) - 3.94206709724702e-3) < eps )

    # test of first mesh edge size of topo edge Ar0022
    n8   = mesh_lima.noeud(8)
    n142 = mesh_lima.noeud(142)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n8, n142) - s1_ar0022) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n8, n142) - 2.12853778510894) < eps )