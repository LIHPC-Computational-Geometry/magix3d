
import pyMagix3D as Mgx3D

def test_group_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    # Création d'une boite avec une topologie
    hors_group = "Hors_Groupe_3D"
    acier = "Acier"
    plastique = "Plastique"
    coque = "Coque"
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, acier)
    assert_all(ctx, "Bl0000", [acier])
    
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    assert_all(ctx, "Bl0001", [hors_group])
    ctx.getGeomManager().addToGroup(["Vol0001"], 3, plastique)
    assert_all(ctx, "Bl0001", [plastique])
    ctx.undo()
    assert_all(ctx, "Bl0001", [hors_group])
    ctx.redo()
    assert_all(ctx, "Bl0001", [plastique])

    ctx.getGeomManager().setGroup(["Vol0001"], 3, coque)
    assert_all(ctx, "Bl0001", [coque])
    ctx.undo()
    assert_all(ctx, "Bl0001", [plastique])
    ctx.redo()
    assert_all(ctx, "Bl0001", [coque])
    
    ctx.getGeomManager().removeFromGroup(["Vol0001"], 3, coque)
    assert_all(ctx, "Bl0001", [hors_group])
    ctx.undo()
    assert_all(ctx, "Bl0001", [coque])
    
    ctx.getGroupManager().clearGroup(3, coque)
    assert_all(ctx, "Bl0001", [hors_group])
    ctx.undo()
    #TODO Créer une issue
    #assert_all(ctx, "Bl0001", [coque])

    ctx.getGeomManager().addToGroup(["Vol0001"], 3, plastique) 
    #TODO Cf issue précédente
    #assert_all(ctx, "Bl0001", [plastique, coque])


def assert_all(ctx, block, group_names):
    block_info = ctx.getTopoManager().getInfos(block, 3)
    # Lien explicite block -> group
    assert 0 == len(block_info.groupsName())
    for group_name in group_names:
        # Lien implicite group -> block
        assert block in ctx.getGroupManager().getTopoBlocks(group_name, 3)
        # Lien implicite group -> faces
        assert set(block_info.cofaces()) == set(ctx.getGroupManager().getTopoFaces(group_name, 3))
        # Lien implicite group -> edges
        assert set(block_info.coedges()) == set(ctx.getGroupManager().getTopoEdges(group_name, 3))
        # Lien implicite group -> vertices
        assert set(block_info.vertices()) == set(ctx.getGroupManager().getTopoVertices(group_name, 3))

    vol = block_info.geomEntity()
    vol_info = ctx.getGeomManager().getInfos(vol, 3)
    assert len(group_names) == len(vol_info.groupsName())
    for group_name in group_names:
        # Lien explicite volume -> group
        assert group_name in vol_info.groupsName()
        # Lien explicite group -> volume
        assert vol in ctx.getGroupManager().getGeomVolumes(group_name, 3)
        # Lien implicite group -> surfaces
        assert set(vol_info.surfaces()) == set(ctx.getGroupManager().getGeomSurfaces(group_name, 3))
        # Lien implicite group -> curves
        assert set(vol_info.curves()) == set(ctx.getGroupManager().getGeomCurves(group_name, 3))
        # Lien implicite group -> vertices
        assert set(vol_info.vertices()) == set(ctx.getGroupManager().getGeomVertices(group_name, 3))

def test_new_box():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "aaa")
    vol_info = ctx.getGeomManager().getInfos("Vol0000",3)
    # Les entités de dimension inférieure à 3 sont affectées au groupe aaa de manière implicite
    assert "aaa" in vol_info.groupsName()
    surf_info = ctx.getGeomManager().getInfos("Surf0000",2)
    assert "aaa" not in surf_info.groupsName()
    crb_info = ctx.getGeomManager().getInfos("Crb0000",1)
    assert "aaa" not in crb_info.groupsName()
    pt_info = ctx.getGeomManager().getInfos("Pt0000",0)
    assert "aaa" not in pt_info.groupsName()

def test_section_by_plane():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "box")
    # Section par un plan entre géométries avec topologies
    ctx.getGeomManager ( ).sectionByPlane (["Vol0000"], Mgx3D.Plane(Mgx3D.Point(0, 0, .5), Mgx3D.Vector(0, 0, 1)), "aaa")
    # Les entités de dimension inférieure à 2 sont affectées au groupe aaa de manière explicite
    surf_info = ctx.getGeomManager().getInfos("Surf0010",2)
    assert "aaa" in surf_info.groupsName()
    crb_info = ctx.getGeomManager().getInfos("Crb0014",1)
    assert "aaa" in crb_info.groupsName()
    pt_info = ctx.getGeomManager().getInfos("Pt0008",0)
    assert "aaa" in pt_info.groupsName()

def test_new_vertices_curves_and_planar_surface():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "box")
    # Création d'une surface avec vecteur de points
    ctx.getGeomManager ( ).newVerticesCurvesAndPlanarSurface ([Mgx3D.Point(1, 1, 0),Mgx3D.Point(1, 1, 1),Mgx3D.Point(1, 0, 1),Mgx3D.Point(1, 0, 0)], "aaa")
    # Les entités de dimension inférieure à 2 sont affectées au groupe aaa de manière explicite
    # Cette commande crée 4 points + 4 courbes + 1 surface
    surf_info = ctx.getGeomManager().getInfos("Surf0006",2)
    assert "aaa" in surf_info.groupsName()
    crb_info = ctx.getGeomManager().getInfos("Crb0012",1)
    assert "aaa" in crb_info.groupsName()
    pt_info = ctx.getGeomManager().getInfos("Pt0008",0)
    assert "aaa" in pt_info.groupsName()

def test_new_planar_surface():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "box")
    # Création de la surface Surf0006
    ctx.getGeomManager ( ).newPlanarSurface (["Crb0002","Crb0006","Crb0010","Crb0011"], "aaa")
    # Les entités de dimension inférieure à 2 sont affectées au groupe aaa de manière implicite
    # Cette commande crée 1 surface à partir de 4 courbes existantes
    surf_info = ctx.getGeomManager().getInfos("Surf0006",2)
    assert "aaa" in surf_info.groupsName()
    crb_info = ctx.getGeomManager().getInfos("Crb0002",1)
    assert "aaa" not in crb_info.groupsName()
    pt_info = ctx.getGeomManager().getInfos("Pt0002",0)
    assert "aaa" not in pt_info.groupsName()
