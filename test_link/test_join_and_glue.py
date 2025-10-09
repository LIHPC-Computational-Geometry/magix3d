import pyMagix3D as Mgx3D

def test_join_surfaces():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2.1, 1, 1), 10, 10, 10)
    # Fusion booléenne entre géométries avec topologies
    ctx.getGeomManager().fuse (["Vol0000","Vol0001"])
    # Collage entre blocs Bl0000 et Bl0001
    tm.fuse2Blocks ("Bl0000","Bl0001")
    # Fusion de surfaces avec maj de la topologie
    gm.joinSurfaces (["Surf0003","Surf0009"])
    assert "Surf0003" not in gm.getSurfaces()
    assert "Surf0009" not in gm.getSurfaces()
    assert "Surf0012" in gm.getSurfaces()
    assert "Surf0012" == tm.getInfos("Fa0003", 2).geom_entity
    assert "Surf0012" == tm.getInfos("Fa0009", 2).geom_entity

def test_glue_with_joined_surfaces(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession()
    gm = ctx.getGeomManager()

    # Création du volume Vol0000
    gm.newSphere (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.DEMI, "A")
    # Création du volume Vol0001
    gm.newSphere (Mgx3D.Point(0, 0, 0), 2, Mgx3D.Portion.DEMI, "B")
    # Fusion de surfaces Surf0005 Surf0004
    gm.joinSurfaces (["Surf0005","Surf0004"])
    # Collage entre Vol0000 Vol0001
    gm.glue (["Vol0000","Vol0001"])

    assert gm.getNbSurfaces() == 6
    assert gm.getNbVolumes() == 2
    out, err = capfd.readouterr()
    assert len(err) == 0

def test_glue_cylinder():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    # Création d'un cylindre avec une topologie
    tm.newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360, True, .5, 10, 10, 10)
    # Création d'un cylindre creux avec une topologie
    tm.newHollowCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, 1.2, Mgx3D.Vector(10, 0, 0), 360, True, 10, 10, 10)

    assert gm.getNbVolumes() == 2
    assert gm.getNbSurfaces() == 7
    assert tm.getNbBlocks() == 9
    assert tm.getNbFaces() == 42
    ctx.getGeomManager ( ).glue (["Vol0000","Vol0001"])
    assert gm.getNbVolumes() == 2
    assert gm.getNbSurfaces() == 6
    assert tm.getNbBlocks() == 9
    assert tm.getNbFaces() == 38

def pre_glue(tm):
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(10, 1, 2), 10, 20, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(10, 0, 0), Mgx3D.Point(12, 1, 2), 10, 20, 10)
    # Découpage de la face Fa0001
    tm.splitFace ("Fa0001", "Ar0005", .5, True)
    # Suppression des projections pour les entités topologiques Fa0013 Fa0012 Ar0025 Ar0027 Ar0028 ... 
    tm.setGeomAssociation (["Fa0013","Fa0012","Ar0025","Ar0027","Ar0028","Ar0026","Ar0024","Som0017","Som0016"], "", True)
    # Modification de la position des sommets topologiques Som0017 Som0016 en coordonnées cartésiennes
    tm.setVertexLocation (["Som0017","Som0016"], True, 9.5, False, 0, False, 0)
    # Découpage suivant Ar0023 des blocs Bl0001
    tm.splitBlocks (["Bl0001"],"Ar0023", .5)
    # Fusion de sommets Som0010 Som0011 avec Som0008 Som0009
    tm.snapVertices ("Ar0014", "Ar0012", False)
    # Modification de la position des sommets topologiques Som0010 Som0011 en coordonnées cartésiennes
    tm.setVertexLocation (["Som0010","Som0011"], True, 9.5, False, 0, False, 0)
    # Modification de la position des sommets topologiques Som0020 Som0021 Som0018 Som0019 en coordonnées cartésiennes
    tm.setVertexLocation (["Som0020","Som0021","Som0018","Som0019"], True, 10, False, 0, False, 0)
    # Projection automatique pour les sommets topologiques Som0020 Som0021 Som0018 Som0019
    tm.projectVerticesOnNearestGeomEntities (["Som0020","Som0021","Som0018","Som0019"], ["Pt0010","Pt0011","Pt0009","Pt0008"], True)
    # Projection automatique pour les faces topologiques Fa0022
    tm.projectFacesOnSurfaces (["Fa0022"])
    # Changement de discrétisation pour Ar0036
    emp = Mgx3D.EdgeMeshingPropertyUniform(10)
    tm.setParallelMeshingProperty (emp,"Ar0036")

def post_glue(tm, mm):
    # Affectation d'une projection vers Vol0000 pour les entités topologiques Bl0002
    tm.setGeomAssociation (["Bl0002"], "Vol0000", False)
    # Découpage de l'arête Ar0003
    tm.splitEdge ("Ar0003", .5)
    # Découpage de l'arête Ar0001
    tm.splitEdge ("Ar0001", .5)
    # Changement de discrétisation pour Ar0044
    emp = Mgx3D.EdgeMeshingPropertyGeometric(10, 1.2, False)
    tm.setParallelMeshingProperty (emp,"Ar0044")
    # Création du maillage pour tous les blocs
    mm.newAllBlocksMesh()

def test_glue_topo1():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    pre_glue(tm)
    # on ne fait rien
    post_glue(tm, mm)
    assert tm.getNbFaces() == 17
    assert mm.getNbFaces() == 2300
    assert mm.getNbNodes() == 6017

def test_glue_topo2():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    pre_glue(tm)
    tm.fuse2Faces ("Fa0012","Fa0014")
    tm.fuse2Faces ("Fa0013","Fa0016")
    post_glue(tm, mm)
    assert tm.getNbFaces() == 15
    assert mm.getNbFaces() == 2100
    assert mm.getNbNodes() == 5786

def test_glue_topo3():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    pre_glue(tm)
    tm.glue2Topo ("Vol0000","Vol0001")
    post_glue(tm, mm)
    assert tm.getNbFaces() == 15
    assert mm.getNbFaces() == 2100
    assert mm.getNbNodes() == 5786

