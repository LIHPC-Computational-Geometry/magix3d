import pyMagix3D as Mgx3D

def test_tetgen():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), False)
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()
    assert ctx.getMeshManager().getNbRegions() > 2300
    assert ctx.getMeshManager().getNbRegions() < 2400

def test_tetgen2():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'un cylindre avec une topologie
    ctx.getTopoManager().newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(5, 0, 0), 90, False, 1, 10, 10, 10)
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()
    assert ctx.getMeshManager().getNbRegions() > 800
    assert ctx.getMeshManager().getNbRegions() < 900
    # Annulation de : Création du maillage pour tous les blocs
    ctx.undo()

    # Changement de discrétisation pour les arêtes Ar0003
    emp = Mgx3D.EdgeMeshingPropertyUniform(30)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0003"])
    # Changement de discrétisation pour les arêtes Ar0008
    emp = Mgx3D.EdgeMeshingPropertyUniform(30)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0008"])
    # Changement de discrétisation pour les arêtes Ar0001
    emp = Mgx3D.EdgeMeshingPropertyUniform(30)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0001"])
    # Création du maillage pour tous les blocs
    ctx.getMeshManager().newAllBlocksMesh()
    assert ctx.getMeshManager().getNbRegions() > 2900
    assert ctx.getMeshManager().getNbRegions() < 3000
