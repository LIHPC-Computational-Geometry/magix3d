import pyMagix3D as Mgx3D
import pytest

def test_beta_shrinkage_law(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)

    with pytest.raises(RuntimeError) as excinfo:
        # Changement de discrétisation pour les arêtes Ar0011
        emp = Mgx3D.EdgeMeshingPropertyBeta(10,1.02)
        ctx.getTopoManager().setMeshingProperty (emp, ["Ar0011"])
    expected = "la valeur de beta doit être comprise entre 1 et 1.01"
    assert expected in str(excinfo.value)    

    with pytest.raises(RuntimeError) as excinfo:
        # Changement de discrétisation pour les arêtes Ar0011
        emp = Mgx3D.EdgeMeshingPropertyBeta(10,1.000005)
        ctx.getTopoManager().setMeshingProperty (emp, ["Ar0011"])
    expected = "la valeur de beta doit être comprise entre 1 et 1.01"
    assert expected in str(excinfo.value)    

    emp = Mgx3D.EdgeMeshingPropertyBeta(10,1.01)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0011"])

    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()