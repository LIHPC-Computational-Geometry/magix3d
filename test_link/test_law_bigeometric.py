import pyMagix3D as Mgx3D
import pytest
import math
import LimaScripting as lima

def l2_norme(n0, n1):
    return math.sqrt( pow(n1.x()-n0.x(),2) + pow(n1.y()-n0.y(),2) + pow(n1.z()-n0.z(),2) )


# The bigeometric progression law with target mesh edge size on both sides
# seems to work only if the two sizes are bigger than what would be the
# uniform mesh edge size on the topological edge.
def test_law_bigeometric_Linear(capfd):
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
    emp = Mgx3D.EdgeMeshingPropertyBigeometric(10,1,s1_ar0000,1,s2_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    # Changement de discrétisation pour les arêtes Ar0011
    emp = Mgx3D.EdgeMeshingPropertyBigeometric(10,1,s1_ar0011,1,s2_ar0011, False)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0011"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_bigeometric.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar0000
    # here, we show that the requested size is not
    # obtained
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) > eps )
    assert( abs(l2_norme(n0, n8) - 0.35) < eps )

    # test of last mesh edge size of topo edge Ar0000
    # here, we show that the requested size is not
    # obtained
    n1  = mesh_lima.noeud(1)
    n16 = mesh_lima.noeud(16)
    assert( abs(l2_norme(n1, n16) - s2_ar0000) > eps )
    assert( abs(l2_norme(n1, n16) - 0.25) < eps )

    # test of first mesh edge size of topo edge Ar0011
    # here, we show that the requested size is not
    # obtained
    n6 = mesh_lima.noeud(6)
    n115 = mesh_lima.noeud(115)
    assert( abs(l2_norme(n6, n115) - s1_ar0011) > eps )
    assert( abs(l2_norme(n6, n115) - 0.35) < eps )

    # test of last mesh edge size of topo edge Ar0011
    # here, we show that the requested size is not
    # obtained
    n2  = mesh_lima.noeud(2)
    n107 = mesh_lima.noeud(107)
    assert( abs(l2_norme(n2, n107) - s2_ar0000) > eps )
    assert( abs(l2_norme(n2, n107) - 0.25) < eps )


# This test shows an example of "Découpage polaire" option, with geometric law.
# In 3D, it's difficult to get a suitable situation to use this option.
# We highlight that it doesn't make sense to be able to pick a target first
# mesh edge size, while applying this polar option. As the law applies on
# angles, it does not take this size into account at all.
def test_law_bigeometric_polar(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s_ar0000 = 0.3

    # Création du sommet Pt0000
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0002
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, -1, 0))
    # Création du cylindre Vol0000
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(0, 0, 1), 3.600000e+02)
    # Création du cylindre Vol0001
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1.4, Mgx3D.Vector(0, 0, 1), 3.600000e+02)
    # Différences entre géométries avec plusieurs entités à couper
    ctx.getGeomManager ( ).cut (["Vol0001"], ["Vol0000"])
    # Création de la boite Vol0003
    ctx.getGeomManager().newBox (Mgx3D.Point(2, 0, 0), Mgx3D.Point(-2, -2, 2))
    # Différences entre géométries avec plusieurs entités à couper
    ctx.getGeomManager ( ).cut (["Vol0002"], ["Vol0003"])
    # Création d'un bloc topologique structuré sans projection (Vol0004)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0004")
    # Affectation d'une projection vers Pt0003 pour les entités topologiques Som0005
    ctx.getTopoManager ( ).setGeomAssociation (["Som0005"], "Pt0003", True)
    # Affectation d'une projection vers Pt0004 pour les entités topologiques Som0001
    ctx.getTopoManager ( ).setGeomAssociation (["Som0001"], "Pt0004", True)
    # Affectation d'une projection vers Pt0005 pour les entités topologiques Som0007
    ctx.getTopoManager ( ).setGeomAssociation (["Som0007"], "Pt0005", True)
    # Affectation d'une projection vers Pt0006 pour les entités topologiques Som0003
    ctx.getTopoManager ( ).setGeomAssociation (["Som0003"], "Pt0006", True)
    # Affectation d'une projection vers Pt0015 pour les entités topologiques Som0004
    ctx.getTopoManager ( ).setGeomAssociation (["Som0004"], "Pt0015", True)
    # Affectation d'une projection vers Pt0016 pour les entités topologiques Som0000
    ctx.getTopoManager ( ).setGeomAssociation (["Som0000"], "Pt0016", True)
    # Affectation d'une projection vers Pt0017 pour les entités topologiques Som0006
    ctx.getTopoManager ( ).setGeomAssociation (["Som0006"], "Pt0017", True)
    # Affectation d'une projection vers Pt0018 pour les entités topologiques Som0002
    ctx.getTopoManager ( ).setGeomAssociation (["Som0002"], "Pt0018", True)
    # Affectation d'une projection vers Crb0022 pour les entités topologiques Ar0002
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0002"], "Crb0022", True)
    # Affectation d'une projection vers Crb0019 pour les entités topologiques Ar0003
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0003"], "Crb0019", True)
    # Affectation d'une projection vers Crb0023 pour les entités topologiques Ar0000
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0000"], "Crb0023", True)
    # Affectation d'une projection vers Crb0021 pour les entités topologiques Ar0001
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0001"], "Crb0021", True)
    # Affectation d'une projection vers Surf0015 pour les entités topologiques Fa0002
    ctx.getTopoManager ( ).setGeomAssociation (["Fa0002"], "Surf0015", True)
    # Affectation d'une projection vers Surf0017 pour les entités topologiques Fa0003
    ctx.getTopoManager ( ).setGeomAssociation (["Fa0003"], "Surf0017", True)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyBigeometric(10,1,s_ar0000,1,s_ar0000, Mgx3D.Point(.45, 0, 0))
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_bigeometric_polar.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar0000
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n0, n8) - s_ar0000) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n0, n8) - 7.72599466265082e-1) < eps )

    # test of last mesh edge size of topo edge Ar0000
    n1  = mesh_lima.noeud(1)
    n16 = mesh_lima.noeud(16)
    # target meshing edge size (not respected)
    assert( abs(l2_norme(n1, n16) - s_ar0000) > eps )
    # real meshing edge size
    assert( abs(l2_norme(n1, n16) - 3.13783707036233e-1) < eps )