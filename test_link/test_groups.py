
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
