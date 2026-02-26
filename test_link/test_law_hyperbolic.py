import pyMagix3D as Mgx3D
import pytest
import math
import LimaScripting as lima

def l2_norme(n0, n1):
    return math.sqrt( pow(n1.x()-n0.x(),2) + pow(n1.y()-n0.y(),2) + pow(n1.z()-n0.z(),2) )


def test_law_hyperbolic_Linear(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.06
    s2_ar0000 = 0.008

    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyHyperbolic(10,s1_ar0000,s2_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_hyperbolic.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar00000
    # here, we show that the requested size is not
    # obtained
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) > eps )
    assert( abs(l2_norme(n0, n8) - 0.0114028117028031) < eps )

    # test of last mesh edge size of topo edge Ar00000
    # here, we show that the requested size is not
    # obtained
    n1  = mesh_lima.noeud(1)
    n16 = mesh_lima.noeud(16)
    assert( abs(l2_norme(n1, n16) - s2_ar0000) > eps )
    assert( abs(l2_norme(n1, n16) - 0.0796198050777711) < eps )



# Hyperbolic law with target first mesh edge size for a topological
# edge associated to a geometric surface.
def test_law_hyperbolic_onSurface(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0023 = 0.01
    s2_ar0023 = 2.4
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
    emp = Mgx3D.EdgeMeshingPropertyHyperbolic(10,s1_ar0023,s2_ar0023)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0023"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_hyperbolic_surface.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar0023
    n9   = mesh_lima.noeud(9)
    n143 = mesh_lima.noeud(143)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n9, n143) - s2_ar0023) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n9, n143) - 1.48428681456749) < eps )

    # test of last mesh edge size of topo edge Ar0023
    n10   = mesh_lima.noeud(10)
    n151 = mesh_lima.noeud(151)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n10, n151) - s1_ar0023) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n10, n151) - 0.0119670075859925) < eps )