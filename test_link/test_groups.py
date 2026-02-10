
import pyMagix3D as Mgx3D
import pytest

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
    assert 0 == len(block_info.groups())
    for group_name in group_names:
        # Lien implicite group -> block
        assert block in ctx.getGroupManager().getTopoBlocks(group_name, 3)
        # Lien implicite group -> faces
        assert set(block_info.cofaces()) == set(ctx.getGroupManager().getTopoFaces(group_name, 3))
        # Lien implicite group -> edges
        assert set(block_info.coedges()) == set(ctx.getGroupManager().getTopoEdges(group_name, 3))
        # Lien implicite group -> vertices
        assert set(block_info.vertices()) == set(ctx.getGroupManager().getTopoVertices(group_name, 3))

    vol = block_info.geom_entity
    vol_info = ctx.getGeomManager().getInfos(vol, 3)
    assert len(group_names) == len(vol_info.groups())
    for group_name in group_names:
        # Lien explicite volume -> group
        assert group_name in vol_info.groups()
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
    assert "aaa" in vol_info.groups()
    surf_info = ctx.getGeomManager().getInfos("Surf0000",2)
    assert "aaa" not in surf_info.groups()
    crb_info = ctx.getGeomManager().getInfos("Crb0000",1)
    assert "aaa" not in crb_info.groups()
    pt_info = ctx.getGeomManager().getInfos("Pt0000",0)
    assert "aaa" not in pt_info.groups()

def test_section_by_plane():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "box")
    vol_info = ctx.getGeomManager().getInfos("Vol0000",3)
    assert "box" in vol_info.groups()
    # Section par un plan entre géométries avec topologies
    ctx.getGeomManager().sectionByPlane (["Vol0000"], Mgx3D.Plane(Mgx3D.Point(0, 0, .5), Mgx3D.Vector(0, 0, 1)), "aaa")
    # Les entités de dimension inférieure à 2 sont affectées au groupe aaa de manière explicite
    vol_info = ctx.getGeomManager().getInfos("Vol0001",3)
    assert "box" in vol_info.groups()
    surf_info = ctx.getGeomManager().getInfos("Surf0010",2)
    assert "aaa" in surf_info.groups()
    crb_info = ctx.getGeomManager().getInfos("Crb0014",1)
    assert "aaa" in crb_info.groups()
    pt_info = ctx.getGeomManager().getInfos("Pt0008",0)
    assert "aaa" in pt_info.groups()
    # Destruction de  Pt0008
    ctx.getGeomManager().destroy(["Pt0008"], False)
    assert "DETRUIT" in ctx.getGroupManager().getInfos("aaa", 2)
    assert "DETRUIT" in ctx.getGroupManager().getInfos("box", 3)
    # Annulation de : Destruction de  Pt0008
    ctx.undo()
    # Les entités de dimension inférieure à 2 sont affectées au groupe aaa de manière explicite
    vol_info = ctx.getGeomManager().getInfos("Vol0001",3)
    assert "box" in vol_info.groups()
    surf_info = ctx.getGeomManager().getInfos("Surf0010",2)
    assert "aaa" in surf_info.groups()
    crb_info = ctx.getGeomManager().getInfos("Crb0014",1)
    assert "aaa" in crb_info.groups()
    pt_info = ctx.getGeomManager().getInfos("Pt0008",0)
    assert "aaa" in pt_info.groups()
    # Annulation de : Section par un plan entre géométries avec topologies
    ctx.undo()
    vol_info = ctx.getGeomManager().getInfos("Vol0000",3)
    assert "box" in vol_info.groups()
    assert "DETRUIT" in ctx.getGroupManager().getInfos("aaa", 1)
    assert "DETRUIT" in ctx.getGroupManager().getInfos("aaa", 2)

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
    assert "aaa" in surf_info.groups()
    crb_info = ctx.getGeomManager().getInfos("Crb0012",1)
    assert "aaa" in crb_info.groups()
    pt_info = ctx.getGeomManager().getInfos("Pt0008",0)
    assert "aaa" in pt_info.groups()

def test_new_planar_surface():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "box")
    # Création de la surface Surf0006
    ctx.getGeomManager().newPlanarSurface (["Crb0002","Crb0006","Crb0010","Crb0011"], "aaa")
    # Les entités de dimension inférieure à 2 sont affectées au groupe aaa de manière implicite
    # Cette commande crée 1 surface à partir de 4 courbes existantes
    surf_info = ctx.getGeomManager().getInfos("Surf0006",2)
    assert "aaa" in surf_info.groups()
    crb_info = ctx.getGeomManager().getInfos("Crb0002",1)
    assert "aaa" not in crb_info.groups()
    pt_info = ctx.getGeomManager().getInfos("Pt0002",0)
    assert "aaa" not in pt_info.groups()

# Issue #215
def test_undo_add_to_group():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Modifie le groupe BOX
    ctx.getGeomManager().addToGroup (["Vol0000"], 3, "BOX")
    # Annulation de : Modifie le groupe BOX
    ctx.undo()
    # Hors_Groupe_3D n'est plus détruit après fix de l'issue#215
    assert "DETRUIT" not in ctx.getGroupManager().getInfos("Hors_Groupe_3D", 3)

# Issue #214
def test_undo_clear_group():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "BOX")
    # Vide le groupe BOX
    ctx.getGroupManager().clearGroup (3, "BOX")
    # Annulation de : Vide le groupe BOX
    ctx.undo()
    # Vol0000 ne devrait pas être dans Hors_Groupe_3D mais seulement dans BOX
    assert "Vol0000" not in ctx.getGroupManager().getGeomVolumes("Hors_Groupe_3D", 3)
    assert "Vol0000" in ctx.getGroupManager().getGeomVolumes("BOX", 3)

# Issue #219
def test_changeGroupName():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "BOX")
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10, "BOX2")
    # Vol0000 est dans BOX. Vol0001 est dans BOX2.
    assert "Vol0000" in ctx.getGroupManager().getGeomVolumes("BOX", 3)
    assert "Vol0001" in ctx.getGroupManager().getGeomVolumes("BOX2", 3)
    ctx.getGroupManager().changeGroupName("BOX2", "NEW_BOX", 3)
    assert "Vol0000" in ctx.getGroupManager().getGeomVolumes("BOX", 3)
    assert "Vol0001" in ctx.getGroupManager().getGeomVolumes("NEW_BOX", 3)
    ctx.undo()
    assert "Vol0000" in ctx.getGroupManager().getGeomVolumes("BOX", 3)
    assert "Vol0001" in ctx.getGroupManager().getGeomVolumes("BOX2", 3)
    ctx.getGroupManager().clearGroup (3, "BOX")
    assert "Vol0000" in ctx.getGroupManager().getGeomVolumes("Hors_Groupe_3D", 3)
    with pytest.raises(RuntimeError) as excinfo:
        ctx.getGroupManager().changeGroupName("Hors_Groupe_3D", "TOTO", 3)
    assert "Il n'est pas possible de changer le nom du groupe par défaut" in str(excinfo.value)

def test_topo_group():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Modifie le groupe aaa
    ctx.getTopoManager().addToGroup (["Bl0000"], 3, "aaa")

    assert ctx.getGroupManager().getTopoBlocks("aaa", 3) == ["Bl0000"]
    assert ctx.getTopoManager().getInfos("Bl0000",3).groups() == ["aaa"]

# vérifie que 2 ajouts successifs ne produisent pas une erreur
def test_two_adds(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Modifie le groupe A
    ctx.getGeomManager().addToGroup (["Pt0000"], 0, "A")
    assert ctx.getGroupManager().getGeomVertices("A", 0) == ['Pt0000']

    # Modifie le groupe A (2e ajout)
    ctx.getGeomManager().addToGroup (["Pt0000"], 0, "A")
    assert ctx.getGroupManager().getGeomVertices("A", 0) == ['Pt0000']

    ctx.getGeomManager().addToGroup (["Pt0001"], 0, "Hors_Groupe_0D")
    out, err = capfd.readouterr()
    assert "Le groupe A possède déjà Pt0000" in out

# suppression de la vérification d'appartenance 
# au même groupe dans l'association topo --> geom
def test_geom_assoc1():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"")

    assert tm.getInfos("Som0000", 0).groups() == ['Hors_Groupe_0D']

    # Affectation d'une projection vers Pt0001 pour les entités topologiques Som0000
    # L'association ne peut se faire car le groupe Hors_Groupe_0D possède Pt0001 ainsi que Som0000
    # Il est recommandé de supprimer la topologie du groupe
    tm.setGeomAssociation (["Som0000"], "Pt0001", True)

    assert tm.getInfos("Som0000", 0).groups() == ['Hors_Groupe_0D']

def test_geom_assoc2():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    # Création de la boite Vol0000
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Modifie le groupe A
    ctx.getGeomManager().addToGroup (["Pt0001"], 0, "A")

    # Création d'un sommet géométrique par coordonnées
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"A")

    assert tm.getInfos("Som0000", 0).groups() == ['A']

    # Affectation d'une projection vers Pt0001 pour les entités topologiques Som0000
    # L'association ne peut se faire car le groupe Hors_Groupe_0D possède Pt0001 ainsi que Som0000
    # Il est recommandé de supprimer la topologie du groupe
    tm.setGeomAssociation (["Som0000"], "Pt0001", True)

    assert tm.getInfos("Som0000", 0).groups() == ['A']

# issue#245: undo on addToGroup raises an error
def test_topo_surface():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 0, 0))
    ctx.getGeomManager().newVertex (Mgx3D.Point(1, 0, 0))
    ctx.getGeomManager().newVertex (Mgx3D.Point(1, 1, 0))
    ctx.getGeomManager().newVertex (Mgx3D.Point(0, 1, 0))
    ctx.getGeomManager().newSegment("Pt0000", "Pt0001")
    ctx.getGeomManager().newSegment("Pt0001", "Pt0002")
    ctx.getGeomManager().newSegment("Pt0002", "Pt0003")
    ctx.getGeomManager().newSegment("Pt0003", "Pt0000")
    ctx.getGeomManager ( ).newPlanarSurface (["Crb0000","Crb0001","Crb0002","Crb0003"], "")
    ctx.getGeomManager().addToGroup (["Surf0000"], 2, "face_cgns")
    ctx.getGeomManager().addToGroup (["Crb0002"], 1, "haut_cgns")
    ctx.getGeomManager().addToGroup (["Crb0000"], 1, "bad_cgns")
    ctx.getGeomManager().addToGroup (["Crb0003"], 1, "gauche_cgns")
    ctx.getGeomManager().addToGroup (["Crb0001"], 1, "droite_cgns")
    ctx.getTopoManager().newStructuredTopoOnGeometry ("Surf0000")
    ctx.getTopoManager().addToGroup(["Fa0000"], 2, "aaa")
    assert ctx.getGroupManager().getTopoFaces("aaa", 2) == ["Fa0000"]
    ctx.undo()
    assert ctx.getGroupManager().getTopoFaces("aaa", 2) == []
