import pyMagix3D as Mgx3D
import pytest

# Met en évidence que les propriétés géométriques d'une entité géométrique sont perdues
#  lors d'opérations de base comme la translation, l'hométhétie, la rotation.
def test_nsbox_scylinder_error():
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    ctx.clearSession() # Clean the session after the previous test
    # Création d'un cylindre
    gm.newCylinder(Mgx3D.Point(0, 0, 0), .2, Mgx3D.Vector(1, 0, 0), Mgx3D.Portion.QUART)
    # Rotation d'une géométrie
    gm.rotate(["Vol0000"], Mgx3D.RotX(270))
    with pytest.raises(RuntimeError) as excinfo:
        # Création d'un bloc topologique sur une géométrie => PB: ce n'est plus un cylindre après le rotate
        tm.newTopoOGridOnGeometry("Vol0000", 0.5)
    expected = "CommandNewTopoOGridOnGeometry impossible, entité Vol0000 n'est pas d'un type supporté pour le moment"
    assert expected in str(excinfo.value)

def test_curve_on_surf_proj_0():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création d'une boite avec une topologie
    gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    with pytest.raises(RuntimeError) as excinfo:
        # Création d'une courbe par projection sur une surface
        gm.newCurveByCurveProjectionOnSurface("Crb0007", "Surf0002")
    expected = "OCC a échoué, création de la courbe composite entre  [ 1, 0, 0]  et  [ 1, 0, 0]"
    assert expected in str(excinfo.value)

# Met en évidence un bug lors la création d'une courbe composite 
# après projection d'une courbe sur une surface
def test_curve_on_surf_proj_1():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du cylindre Vol0000
    gm.newCylinder (Mgx3D.Point(-.135, 0, 0), .13, Mgx3D.Vector(.25, 0, 0), 3.600000e+02)
    # Création du cylindre Vol0001
    gm.newCylinder (Mgx3D.Point(0, 0, 0), 0.055, Mgx3D.Vector(0, .2, 0), 3.600000e+02)
    # Section suivant Surf0003 des entités Vol0000
    gm.section (["Vol0000"], "Surf0003")
    # Création du sommet Pt0006
    gm.newVertex (Mgx3D.Point(5.29150262212918e-3, 1.15325625946708e-1, -0.006), "TOTO")
    # Création du sommet Pt0007
    gm.newVertex (Mgx3D.Point(5.29150262212918e-2, 1.15325625946708e-1, -0.06), "TOTO")
    # Création du segment Crb0009
    gm.newSegment("Pt0006", "Pt0007", "TOTO")

    with pytest.raises(RuntimeError) as excinfo:
        # Création d'une courbe par projection sur une surface
        # OCC a échoué, création de la courbe composite entre  [ 3.63790805271381e-2, 1.15325625946708e-1, -0.04125]  et  [ 3.63790805271381e-2, 1.15325625946708e-1, -0.04125] 
        gm.newCurveByCurveProjectionOnSurface("Crb0009", "Surf0007", "TITI")
    assert "OCC a échoué, création de la courbe composite" in str(excinfo.value)

# Met en évidence un bug lors la création d'une courbe composite
# après projection d'une courbe sur une surface
def test_curve_on_surf_proj_2():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0.0615, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(-0.0015, 0.06, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(-0.0015, 0.0615, 0))
    # Création de l'arc de cercle Crb0000
    gm.newArcCircle("Pt0002", "Pt0001", "Pt0000", True)
    # Révolution de Crb0000
    gm.makeRevol (["Crb0000"], Mgx3D.RotX(90), False)
    # Création du sommet Pt0007
    gm.newVertex (Mgx3D.Point(-0.0015, 4.24264068711929e-2, 4.24264068711928e-2))
    # Création du sommet Pt0008
    gm.newVertex (Mgx3D.Point(0, 4.34870670429727e-2, 4.34870670429727e-2))
    # Création du segment Crb0005
    gm.newSegment("Pt0008", "Pt0007")

    with pytest.raises(RuntimeError) as excinfo:
        # Création d'une courbe par projection sur une surface
        # Erreur OCC
        gm.newCurveByCurveProjectionOnSurface("Crb0005", "Surf0000")
    assert "Erreur OCC" in str(excinfo.value)

# le maillage obtenu dans le fichier pb_perturbation.mli semble anormal
def test_perturbation(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()
    def pert(x, y, z):
        return [x, y, z+x/10.0 + y/5.0]

    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10)
    # Collage entre géométries avec topologies
    ctx.getGeomManager ( ).glue (["Vol0000","Vol0001"])
    # Modifie le groupe ZMAX
    gm.addToGroup (["Surf0005", "Surf0011"], 2, "ZMAX")
    # Modifie le groupe S5
    gm.addToGroup (["Surf0005"], 2, "S5")
    # Modifie le groupe S11
    gm.addToGroup (["Surf0011"], 2, "S11")
    ctx.getGroupManager().addCartesianPerturbation("ZMAX", pert)
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0005"] )
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0011"] )
    # Sauvegarde du maillage (mli)
    mm.writeMli("pb_perturbation.mli2")

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_circle_revol_180(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 2, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, 1, 0))
    # Création de l'arc de cercle Crb0000
    gm.newCircle("Pt0000", "Pt0001", "Pt0002")
    with pytest.raises(RuntimeError) as excinfo:
        # Révolution du cercle
        gm.makeRevol (["Crb0000"], Mgx3D.RotX(180), False)
    expected = "Une configuration imprévue a été rencontrée lors de la révolution d'une courbe composée: attention une courbe n'a pas deux sommets"
    assert expected in str(excinfo.value)

def test_glue_command_return():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    b1 = gm.newBox(Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    assert b1.getVolume() == "Vol0000"
    b2 = gm.newBox(Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1))
    assert b2.getVolume() == "Vol0001"
    collage = gm.glue ([b1.getVolume(),b2.getVolume()])
    # la commande de glue ne renseigne pas le résultat (CommandResult)
    # on devrait avoir la ligne suivante ; 
    # assert collage.getVolumes() == ["Vol0000", "Vol00001"]
    # Or on a :
    assert collage.getVolumes() == []

def test_ogrids_not_equal():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # création d'un cylindre avec topo + maillage
    tm.newCylinderWithTopo (Mgx3D.Point(5, 4.5, 0), 1, Mgx3D.Vector(0, 3, 0), 360, True, .5, 10, 10, 10)
    mm.newAllBlocksMesh()
    nb_regions_1 = mm.getNbRegions()
    ctx.clearSession()

    # création d'un cylindre sans topo + topo + maillage
    gm.newCylinder (Mgx3D.Point(5, 4.5, 0), 1, Mgx3D.Vector(0, 3, 0), 3.600000e+02)
    tm.newTopoOGridOnGeometry ("Vol0000",.5)
    mm.newAllBlocksMesh()
    nb_regions_2 = mm.getNbRegions()

    # le nombre de regions devrait être identique... et non
    assert nb_regions_1 > nb_regions_2

# Issue #214
def test_undo_clear_group():
    ctx = Mgx3D.getContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "BOX")
    # Vide le groupe BOX
    ctx.getGroupManager().clearGroup (3, "BOX")
    # Annulation de : Vide le groupe BOX
    ctx.undo()
    # Vol0000 ne devrait pas être dans Hors_Groupe_3D mais seulement dans BOX
    # assert "Vol0000" not in ctx.getGroupManager().getGeomEntities(["Hors_Groupe_3D"])

# Issue #215
def test_undo_add_to_group():
    ctx = Mgx3D.getContext()
    ctx.clearSession() # Clean the session after the previous test
    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Modifie le groupe BOX
    ctx.getGeomManager().addToGroup (["Vol0000"], 3, "BOX")
    # Annulation de : Modifie le groupe BOX
    ctx.undo()
    #Vol0000 est bien dans Hors_Groupe_3D qui est destroyed (en rouge dans l'ihm)