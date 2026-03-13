import pyMagix3D as Mgx3D
import pytest
import math
import LimaScripting as lima

def l2_norme(n0, n1):
    return math.sqrt( pow(n1.x()-n0.x(),2) + pow(n1.y()-n0.y(),2) + pow(n1.z()-n0.z(),2) )


def test_law_biexponential_Linear(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.3
    s2_ar0000 = 0.2
    s1_ar0011 = 0.3
    s2_ar0011 = 0.2

    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(10, s1_ar0000, s2_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    # Changement de discrétisation pour les arêtes Ar0011
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(10, s1_ar0011, s2_ar0011, False)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0011"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_biexponential.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test first mesh edge size of topo edge Ar0000
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) < eps )

    # test last mesh edge size of topo edge Ar0000
    n1  = mesh_lima.noeud(1)
    n16 = mesh_lima.noeud(16)
    assert( abs(l2_norme(n1, n16) - s2_ar0000) < eps )

    # test first mesh edge size of topo edge Ar0011
    n6 = mesh_lima.noeud(6)
    n115 = mesh_lima.noeud(115)
    assert( abs(l2_norme(n6, n115) - s1_ar0011) < eps )

    # test last mesh edge size of topo edge Ar0011
    n2  = mesh_lima.noeud(2)
    n107 = mesh_lima.noeud(107)
    assert( abs(l2_norme(n2, n107) - s2_ar0011) < eps )

# Target sizes are not respected here. It is due to how
# the projected topological edge length is computed in
# the code. This also affects the other laws.
def test_law_biexponential_onCurve_1(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.000001
    s2_ar0000 = 0.05

    # Création du cylindre Vol0000
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(0, 0, 1), 3.600000e+02)
    # Création d'un bloc topologique structuré sans projection (Vol0000)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0000")
    # Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0000
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0000"], "Crb0002", True)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(50, s1_ar0000, s2_ar0000)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0000")
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_biexponential.mli2"
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
    assert( abs(l2_norme(n0, n8) - 1.11072074733155e-6) < eps )

    # test of last mesh edge size of topo edge Ar00000
    n1  = mesh_lima.noeud(1)
    n56 = mesh_lima.noeud(56)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n1, n56) - s2_ar0000) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n1, n56) - 5.55289000397246e-2) < eps )

    # we check now that the parallel edges has computed the
    # positions to respect the target sizes.
    # test of first mesh edge size of topo edge Ar00003
    n6   = mesh_lima.noeud(6)
    n155 = mesh_lima.noeud(155)
    # target meshing edge size (here, the size is respected
    # because the topological edge is straight)
    assert( abs(l2_norme(n6, n155) - s1_ar0000) < eps )

    # test of last mesh edge size of topo edge Ar00003
    n7   = mesh_lima.noeud(7)
    n203 = mesh_lima.noeud(203)
    # target meshing edge size (here, the size is respected
    # because the topological edge is straight)
    assert( abs(l2_norme(n7, n203) - s2_ar0000) < eps )


# Bi-exponential law with target first and last mesh edge size for a topological
# edge associated to a geometric curve with the two topological
# vertices matching the two geometric points bordering the curve.
# Target sizes may not be respected because computed on curved
# topological edge, while the final mesh is linear.
def test_law_biexponential_onCurve_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0001 = 0.00007
    s2_ar0001 = 0.003
    s1_ar0003 = 0.2
    s2_ar0003 = 0.4

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
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(30, s1_ar0001, s2_ar0001)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0001")
    # Changement de discrétisation pour les arêtes Ar0003
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(30, s1_ar0003, s2_ar0003)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0003"])
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_biexponential_3.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar0001
    n2  = mesh_lima.noeud(2)
    n37 = mesh_lima.noeud(37)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n2, n37) - s1_ar0001) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n2, n37) - 6.93887715010575e-5) < eps )

    # test of last mesh edge size of topo edge Ar0001
    n3  = mesh_lima.noeud(3)
    n65 = mesh_lima.noeud(65)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n3, n65) - s2_ar0001) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n3, n65) - 2.97380451228687e-3) < eps )


    # test of first mesh edge size of topo edge Ar0003
    n6  = mesh_lima.noeud(6)
    n95 = mesh_lima.noeud(95)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n6, n95) - s1_ar0003) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n6, n95) - 1.99666833293657e-1) < eps )

    # test of last mesh edge size of topo edge Ar0003
    n7   = mesh_lima.noeud(7)
    n123 = mesh_lima.noeud(123)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n7, n123) - s2_ar0003) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n7, n123) - 3.97338661590123e-1) < eps )


# Bi-exponential law with target first and last mesh edge size for a topological
# edge associated to a geometric surface.
def test_law_biexponential_onSurface(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0023 = 0.0042
    s1_ar0022 = 0.8
    s2_ar0022 = 0.5

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
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(20, s1_ar0023, s1_ar0023)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0023")
    # Changement de discrétisation pour les arêtes Ar0022
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(20, s1_ar0022, s2_ar0022)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0022"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_geometric_3.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar0023
    n9   = mesh_lima.noeud(9)
    n193 = mesh_lima.noeud(193)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n9, n193) - s1_ar0023) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n9, n193) - 4.14588643711679e-3) < eps )

    # test of last mesh edge size of topo edge Ar0023
    n10  = mesh_lima.noeud(10)
    n211 = mesh_lima.noeud(211)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n10, n211) - s1_ar0023) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n10, n211) - 4.15171460239938e-3) < eps )


    # test of first mesh edge size of topo edge Ar0022
    n11  = mesh_lima.noeud(11)
    n174 = mesh_lima.noeud(174)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n11, n174) - s1_ar0022) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n11, n174) - 7.89083037828939e-1) < eps )

    # test of last mesh edge size of topo edge Ar0022
    n8   = mesh_lima.noeud(8)
    n192 = mesh_lima.noeud(192)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n8, n192) - s2_ar0022) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n8, n192) - 4.9378232958398e-1) < eps )