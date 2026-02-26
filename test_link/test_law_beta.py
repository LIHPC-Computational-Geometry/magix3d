import pyMagix3D as Mgx3D
import pytest
import math
import LimaScripting as lima

def l2_norme(n0, n1):
    return math.sqrt( pow(n1.x()-n0.x(),2) + pow(n1.y()-n0.y(),2) + pow(n1.z()-n0.z(),2) )


def test_law_beta(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.005

    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyBeta(10, 1.1, True, True, s1_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_beta.mli2"
    mm.writeMli(filename)
    mesh_lima = lima.Maillage()
    mesh_lima.lire(filename)

    # we choose a tolerance for tests on sizes
    eps = 10e-9

    # test of first mesh edge size of topo edge Ar00000
    n0 = mesh_lima.noeud(0)
    n8 = mesh_lima.noeud(8)
    assert( abs(l2_norme(n0, n8) - s1_ar0000) < eps )



# Beta law with target first mesh edge size seems to work only if this
# size is smaller than what would be the uniform mesh edge size.
def test_law_beta_onCurve_1(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0000 = 0.00008

    # Création du cylindre Vol0000
    ctx.getGeomManager().newCylinder (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(0, 0, 1), 3.600000e+02)
    # Création d'un bloc topologique structuré sans projection (Vol0000)
    ctx.getTopoManager().newFreeTopoOnGeometry ("Vol0000")
    # Affectation d'une projection vers Crb0000 pour les entités topologiques Ar0000
    ctx.getTopoManager ( ).setGeomAssociation (["Ar0000"], "Crb0002", True)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyBeta(10, 1.1, True, True, s1_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_beta_2.mli2"
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
    assert( abs(l2_norme(n0, n8) - 0.000123750926732273) < eps )



# Beta law with target first mesh edge size for a topological
# edge associated to a geometric curve with the two topological
# vertices matching the two geometric points bordering the curve.
def test_law_beta_onCurve_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    mm = ctx.getMeshManager()

    # chosen first mesh edge size
    s1_ar0001 = 0.00007

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
    emp = Mgx3D.EdgeMeshingPropertyGeometric(10, 1.1, True, True, s1_ar0001)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0001"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()

    # Sauvegarde du maillage (mli)
    filename = "meshing_law_beta_3.mli2"
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



# This test case shows that the "Inverser le sens" option doesn't work when the
# beta law is used with a target first mesh edge size. To show this, we use the
# same test as "test_law_beta", which is successful, and the only
# difference is we turn false the direction option.
def test_law_beta_fail(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    # chosen first mesh edge size
    s1_ar0000 = 0.005

    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    # Changement de discrétisation pour les arêtes Ar0000
    emp = Mgx3D.EdgeMeshingPropertyBeta(10, 1.1, False, True, s1_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    with pytest.raises(RuntimeError) as excinfo:
        # Création du maillage pour tous les blocs
        ctx.getMeshManager().newAllBlocksMesh()
    expected = "EdgeMeshingPropertyBeta, on ne peut pas trouver de beta"
    assert expected in str(excinfo.value)