import pyMagix3D as Mgx3D

def test_box_delaunay_mesh():
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), False)
    # Changement de discrétisation pour les arêtes Ar0011 Ar0005 Ar0009 Ar0001
    emp = Mgx3D.EdgeMeshingPropertyUniform(1)
    tm.setMeshingProperty (emp,["Ar0011", "Ar0005", "Ar0009", "Ar0001"])
    # Changement de discrétisation pour les faces Fa0005
    tm.setMeshingProperty (Mgx3D.FaceMeshingPropertyDelaunayGMSH(0, 1e22),["Fa0005"])
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0005"] )
    assert(mm.getNbNodes() == 5)
    assert(mm.getNbFaces() == 4)

def test_cylinder_delaunay_mesh():
    ctx = Mgx3D.getStdContext()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360, False, 1, 10, 10, 10)
    # Changement de discrétisation pour les faces Fa0000
    tm.setMeshingProperty (Mgx3D.FaceMeshingPropertyDelaunayGMSH(0, 1e22),["Fa0000", "Fa0001", "Fa0002"])
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0000", "Fa0001", "Fa0002"] )
    assert(mm.getNbFaces() == 248)