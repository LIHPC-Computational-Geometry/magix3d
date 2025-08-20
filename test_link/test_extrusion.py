import pyMagix3D as Mgx3D

# Tests sur les commandes d'extrusion : direction et révolution
# pour lesquelles un certain nombres de bug a été trouvé

# Issue#224
def test_makeExtrude(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du cylindre Vol0000
    gm.newVerticesCurvesAndPlanarSurface([Mgx3D.Point(0, 0, 0),Mgx3D.Point(5, 0, 0),Mgx3D.Point(5, 5, 0),Mgx3D.Point(0, 5, 0)], "")
    # Extrusion de Surf0000
    gm.makeExtrude (["Surf0000"], Mgx3D.Vector(0, 0, 5), False)

    # Annulation de : Extrusion de Surf0000
    ctx.undo()

    surf0_vertices = gm.getInfos("Surf0000", 2).vertices()
    assert len(surf0_vertices) == 4

    out, err = capfd.readouterr()
    assert len(err) == 0

# Issue#221
def test_makeRevol(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))

    vol0_surfaces = gm.getInfos("Vol0000", 3).surfaces()
    assert len(vol0_surfaces) == 6

    # Révolution de Surf0004
    gm.makeRevol (["Surf0004"], Mgx3D.RotX(180), False)

    vol0_surfaces = gm.getInfos("Vol0000", 3).surfaces()
    assert len(vol0_surfaces) == 6

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_makeExtrude_groups():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    grm = ctx.getGroupManager()

    # Création du cylindre Vol0000
    gm.newVerticesCurvesAndPlanarSurface([Mgx3D.Point(0, 0, 0),Mgx3D.Point(5, 0, 0),Mgx3D.Point(5, 5, 0),Mgx3D.Point(0, 5, 0)], "PLAN")
    # Extrusion de Surf0000
    gm.makeExtrude (["Surf0000"], Mgx3D.Vector(0, 0, 5), False)

    assert gm.getInfos("Vol0000",3).groupsName() == ['PLAN']
    assert sorted(grm.getGeomSurfaces("PLAN", 2)) == ['Surf0001', 'Surf0002', 'Surf0003', 'Surf0004']
    assert sorted(grm.getGeomSurfaces("Hors_Groupe_2D", 2)) == ['Surf0008', 'Surf0006']
    assert sorted(grm.getGeomCurves("PLAN", 1)) == ['Crb0004', 'Crb0005', 'Crb0006', 'Crb0007']
    assert sorted(grm.getGeomCurves("Hors_Groupe_1D", 1)) == ['Crb0008', 'Crb0009', 'Crb0010', 'Crb0011', 'Crb0012', 'Crb0013', 'Crb0014', 'Crb0015']
    assert grm.getGeomVertices("PLAN", 0) == []

def test_makeRevol_groups():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    grm = ctx.getGroupManager()

    # Création du cylindre Vol0000
    gm.newVerticesCurvesAndPlanarSurface([Mgx3D.Point(0, 0, 0),Mgx3D.Point(5, 0, 0),Mgx3D.Point(5, 5, 0),Mgx3D.Point(0, 5, 0)], "PLAN")
    # Révolution de Surf0000
    gm.makeRevol (["Surf0000"], Mgx3D.RotX(120), False)

    assert gm.getInfos("Vol0000",3).groupsName() == ['PLAN']
    assert sorted(grm.getGeomSurfaces("PLAN", 2)) == ['Surf0001', 'Surf0002', 'Surf0003']
    assert sorted(grm.getGeomSurfaces("Hors_Groupe_2D", 2)) == ['Surf0004', 'Surf0005']
    assert sorted(grm.getGeomCurves("PLAN", 1)) == ['Crb0004', 'Crb0005']
    assert sorted(grm.getGeomCurves("Hors_Groupe_1D", 1)) == ['Crb0006', 'Crb0007', 'Crb0008', 'Crb0009', 'Crb0010', 'Crb0011', 'Crb0012']
    assert grm.getGeomVertices("PLAN", 0) == []

def test_makeRevol_groups():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    grm = ctx.getGroupManager()

    # Création du cylindre Vol0000
    gm.newVerticesCurvesAndPlanarSurface([Mgx3D.Point(0, 0, 0),Mgx3D.Point(5, 0, 0),Mgx3D.Point(5, 5, 0),Mgx3D.Point(0, 5, 0)], "PLAN")
    # Révolution de Surf0000
    gm.makeRevol (["Surf0000"], Mgx3D.RotX(180), False)

    assert gm.getInfos("Vol0000",3).groupsName() == ['PLAN']
    assert sorted(grm.getGeomSurfaces("PLAN", 2)) == ['Surf0001', 'Surf0002', 'Surf0003']
    assert sorted(grm.getGeomSurfaces("PZ0_PLAN", 2)) == ['Surf0004', 'Surf0005']
    assert sorted(grm.getGeomCurves("PLAN", 1)) == ['Crb0004', 'Crb0005']
    assert sorted(grm.getGeomCurves("PZ0_PLAN", 1)) == ['Crb0006', 'Crb0007', 'Crb0008', 'Crb0009', 'Crb0010', 'Crb0011', 'Crb0012']
    assert grm.getGeomVertices("PLAN", 0) == []
