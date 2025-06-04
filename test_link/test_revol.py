import pyMagix3D as Mgx3D


def test_curve_revol_180(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 2, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, 1, 0))
    # Création de l'arc de cercle Crb0000
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0002")
    # Révolution de l'arc de cercle
    gm.makeRevol (["Crb0000"], Mgx3D.RotX(180), False)
    assert gm.getNbSurfaces() == 1
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    tm.newStructuredTopoOnGeometry ("Surf0000")
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0000"] )
    assert mm.getNbFaces() == 100

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_curve_revol_360(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 1, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 2, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(1, 1, 0))
    # Création de l'arc de cercle Crb0000
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0002")
    # Révolution de l'arc de cercle
    gm.makeRevol (["Crb0000"], Mgx3D.RotX(360), False)
    tm.newStructuredTopoOnGeometry ("Surf0000")
    assert gm.getNbSurfaces() == 1
    # Création d'une face topologique structurée sur une géométrie (Surf0000)
    tm.newStructuredTopoOnGeometry ("Surf0000")
    # Création du maillage pour des faces
    mm.newFacesMesh ( ["Fa0000"] )
    # Attention, maillage moche !
    assert mm.getNbFaces() == 100

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_surf_revol(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
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
