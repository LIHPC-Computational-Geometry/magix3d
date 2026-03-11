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
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(10,1,s1_ar0000,1,s2_ar0000)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0000"])
    # Changement de discrétisation pour les arêtes Ar0011
    emp = Mgx3D.EdgeMeshingPropertyBiexponential(10,1,s1_ar0011,1,s2_ar0011, False)
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