import pyMagix3D as Mgx3D

def test_surf_revol(capfd):
    ctx = Mgx3D.getStdContext()
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, -.528, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, .35, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(.49, .19, 0))
    # Création du sommet Pt0003
    gm.newVertex (Mgx3D.Point(.5, .2, 0))
    # Création du sommet Pt0004
    gm.newVertex (Mgx3D.Point(0, .355, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0002", "Pt0003")
    # Création du segment Crb0001
    gm.newSegment("Pt0004", "Pt0001")
    # Création de l'arc de cercle Crb0002
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0002", True)
    # Création de l'arc de cercle Crb0003
    gm.newArcCircle("Pt0000", "Pt0003", "Pt0004", True)
    # Création de la surface Surf0000
    gm.newPlanarSurface (["Crb0000","Crb0001","Crb0002","Crb0003"], "")
    # Révolution de Surf0000
    gm.makeRevol (["Surf0000"], Mgx3D.RotX(90), False)

    assert gm.getNbSurfaces() == 6
    out, err = capfd.readouterr()
    assert len(err) == 0
