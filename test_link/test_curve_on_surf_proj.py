import pyMagix3D as Mgx3D

# Met en évidence un bug lors la création d'une courbe composite 
# après projection d'une courbe sur une surface
def test_curve_on_surf_proj():
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
