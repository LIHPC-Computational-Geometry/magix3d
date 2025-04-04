import pyMagix3D as Mgx3D

def test_nsbox_scylinder_error(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    ctx.clearSession() # Clean the session after the previous test
    # Création d'un cylindre
    gm.newCylinder(Mgx3D.Point(0, 0, 0), .2, Mgx3D.Vector(1, 0, 0), Mgx3D.Portion.QUART)
    # Création d'une boite
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    # Rotation d'une géométrie
    gm.rotate(["Vol0000"], Mgx3D.RotX(270))
    # Collage entre géométries
    gm.glue(["Vol0001", "Vol0000"])
    try:
        # Création d'un bloc topologique sur une géométrie => PB: ce n'est plus un cylindre après le glue
        tm.newTopoOGridOnGeometry("Vol0000", 0.5)
        assert False
    except RuntimeError:
        out, err = capfd.readouterr()
        expected = "CommandNewTopoOGridOnGeometry impossible, entité Vol0000 n'est pas d'un type supporté pour le moment"
        assert expected in err

# Met en évidence un bug lors d'une révolution d'une courbe
def test_curve_revol():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 2, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, 1, 0))
    # Création du cercle Crb0000
    gm.newCircle("Pt0000", "Pt0001", "Pt0002")
    # Création de la surface Surf0000
    gm.newPlanarSurface (["Crb0000"], "")

    try:
        # Révolution de Surf0000
        # Une configuration imprévue a été rencontrée lors de la révolution d'une courbe composée: attention une courbe n'a pas deux sommets
        gm.makeRevol (["Surf0000"], Mgx3D.RotX(90), False)
    except RuntimeError as e:
        assert "Une configuration imprévue a été rencontrée lors de la révolution d'une courbe composée" in str(e)

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

    try:
        # Création d'une courbe par projection sur une surface
        # OCC a échoué, création de la courbe composite entre  [ 3.63790805271381e-2, 1.15325625946708e-1, -0.04125]  et  [ 3.63790805271381e-2, 1.15325625946708e-1, -0.04125] 
        gm.newCurveByCurveProjectionOnSurface("Crb0009", "Surf0007", "TITI")
    except RuntimeError as e: 
        assert "OCC a échoué, création de la courbe composite" in str(e)

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

    try:
        # Création d'une courbe par projection sur une surface
        # Erreur OCC
        gm.newCurveByCurveProjectionOnSurface("Crb0005", "Surf0000")
    except RuntimeError as e: 
        assert "Erreur OCC" in str(e)

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
