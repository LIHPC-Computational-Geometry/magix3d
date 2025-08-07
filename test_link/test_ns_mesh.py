import pyMagix3D as Mgx3D

def test_ns_mesh_1(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création du cylindre Vol0000
    gm.newCylinder (Mgx3D.Point(-.135, 0, 0), .145, Mgx3D.Vector(.25, 0, 0), 3.600000e+02)
    # Création du cylindre Vol0001
    gm.newCylinder (Mgx3D.Point(-.135, 0, 0), .13, Mgx3D.Vector(.25, 0, 0), 3.600000e+02)
    # Différences entre géométries avec plusieurs entités à couper
    ctx.getGeomManager ( ).cut (["Vol0000"], ["Vol0001"])
    # Création du segment Crb0006
    gm.newSegment("Pt0002", "Pt0000")
    # Création du sommet Pt0004 sur la courbe Crb0006
    gm.newVertex ("Crb0006", 5.000000e-01)
    # Création du sommet Pt0005
    gm.newVertex (Mgx3D.Point(.1225, 0, .1375))
    # Création de l'arc de cercle Crb0007
    gm.newArcCircle("Pt0004", "Pt0002", "Pt0005", True)
    # Création de l'arc de cercle Crb0008
    gm.newArcCircle("Pt0004", "Pt0005", "Pt0000", True)
    # Création de la surface Surf0008
    ctx.getGeomManager ( ).newPlanarSurface (["Crb0006","Crb0007","Crb0008"], "A")
    # Révolution de Surf0008
    gm.makeRevol (["Surf0008"], Mgx3D.RotX(360), False)
    # Modifie le groupe A
    gm.removeFromGroup (["Vol0003"], 3, "A")
    # Copie de  Vol0003
    ctx.getGeomManager ( ).copy (["Vol0003"], False,"")
    # Rotation de Vol0004 suivant  [  [ 0, 0, 0] ,  [ 0, 0, 1] , 180] 
    ctx.getGeomManager ( ).rotate (["Vol0004"], Mgx3D.RotZ(180))
    # Translation de Vol0004 suivant  [ -0.02, 0, 0] 
    ctx.getGeomManager ( ).translate (["Vol0004"], Mgx3D.Vector(-0.02, 0, 0))
    # Fusion Booléenne de  Vol0003 Vol0004 Vol0002
    ctx.getGeomManager ( ).fuse (["Vol0003","Vol0004","Vol0002"])
    # Rotation de Vol0005 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 45] 
    ctx.getGeomManager ( ).rotate (["Vol0005"], Mgx3D.RotX(45))
    # Création d'un bloc topologique non structuré sur une géométrie (Vol0005)
    tm.newUnstructuredTopoOnGeometry ("Vol0005")
    # Changement de discrétisation pour les arêtes Ar0000 Ar0007 Ar0001 Ar0003 Ar0005 ... 
    emp = Mgx3D.EdgeMeshingPropertySpecificSize(3.000000e-03)
    tm.setMeshingProperty (emp, ["Ar0000","Ar0007","Ar0001","Ar0003","Ar0005","Ar0010","Ar0006","Ar0004","Ar0011","Ar0002","Ar0009","Ar0008"])
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0002"] )
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0001"] )

    assert gm.getNbVolumes() == 1
    assert gm.getNbSurfaces() == 6
    assert mm.getNbFaces() == 5828
    out, err = capfd.readouterr()
    assert len(err) == 0

def test_ns_mesh_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), "B")
    gm.newCylinder(Mgx3D.Point(0.4, 0, 0.4), .1, Mgx3D.Vector(0, 1, 0), 3.600000e+02, "C")
    gm.glue(["Vol0000", "Vol0001"])
    tm.newUnstructuredTopoOnGeometry("Vol0002")
    tm.newUnstructuredTopoOnGeometry("Vol0001")
    mm.newAllBlocksMesh()

    assert gm.getNbVolumes() == 2
    assert mm.getNbFaces() < 1900
    assert mm.getNbFaces() > 1850
    out, err = capfd.readouterr()
    assert len(err) == 0

def test_ns_mesh_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    tm.newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(5, 0, 0), 90, False, 1, 10, 10, 10)
    emp = Mgx3D.EdgeMeshingPropertyUniform(30)
    ctx.getTopoManager().setMeshingProperty (emp, ["Ar0003","Ar0008","Ar0001"])
    ctx.getMeshManager().newAllBlocksMesh()

    assert mm.getNbFaces() < 1980
    assert mm.getNbFaces() > 1940
    out, err = capfd.readouterr()
    assert len(err) == 0

def test_block_on_box_union(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    gm.newBox(Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1))
    gm.translate(["Vol0001"], Mgx3D.Vector(0, 0.5, 0.5))
    gm.fuse(["Vol0000", "Vol0001"])
    tm.newUnstructuredTopoOnGeometry("Vol0002")
    mm.newAllBlocksMesh()

    assert gm.getNbVolumes() == 1
    out, err = capfd.readouterr()
    assert len(err) == 0
