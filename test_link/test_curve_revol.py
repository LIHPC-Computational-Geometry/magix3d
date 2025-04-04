import pyMagix3D as Mgx3D

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
