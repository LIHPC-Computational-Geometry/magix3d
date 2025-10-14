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

    assert len(gm.getInfos("Surf0000", 2).vertices()) == 4

    gm.makeExtrude (["Surf0000"], Mgx3D.Vector(0, 0, 5), False)
    assert gm.getNbVolumes() == 1

    out, err = capfd.readouterr()
    assert len(err) == 0

# Issue#221
def test_makeRevol2D(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    assert len(gm.getInfos("Vol0000", 3).surfaces()) == 6

    # keep=False => Vol0000 n'avait plus que 5 faces (bug corrigé)
    gm.makeRevol (["Surf0004"], Mgx3D.RotX(180), False)
    assert len(gm.getInfos("Vol0000", 3).surfaces()) == 6
    assert len(gm.getInfos("Vol0001", 3).surfaces()) == 5

    # Annulation de : Révolution de Surf0004
    ctx.undo()
    assert len(gm.getInfos("Vol0000", 3).surfaces()) == 6

    # keep=True => la box a 6 faces
    gm.makeRevol (["Surf0004"], Mgx3D.RotX(180), True)
    assert len(gm.getInfos("Vol0000", 3).surfaces()) == 6
    assert len(gm.getInfos("Vol0001", 3).surfaces()) == 5

    # Annulation de : Révolution de Surf0004
    ctx.undo()
    assert len(gm.getInfos("Vol0000", 3).surfaces()) == 6

    # cas 2*PI : méthode spécifique qui donne un cone (1 surface de moins)
    gm.makeRevol (["Surf0004"], Mgx3D.RotX(360), False)
    assert len(gm.getInfos("Vol0000", 3).surfaces()) == 6
    assert len(gm.getInfos("Vol0001", 3).surfaces()) == 3

    # Annulation de : Révolution de Surf0004
    ctx.undo()
    assert gm.getNbVolumes() == 1

    # En conservant l'entité initile => la box a 6 faces
    out, err = capfd.readouterr()
    assert len(err) == 0

# Même pb que Issue#221 en 1D (sur des courbes)
def test_makeRevol1D(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(0, 5, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(0, 10, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0001
    gm.newSegment("Pt0002", "Pt0001")

    assert len(gm.getInfos("Crb0000", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0001", 1).vertices()) == 2

    # keep=False => Crb0000 n'avait plus que 1 point (bug corrigé)
    # Révolution de Crb0001
    gm.makeRevol (["Crb0001"], Mgx3D.RotX(180), False)
    assert len(gm.getInfos("Crb0000", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0002", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0003", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0004", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0005", 1).vertices()) == 2

    # Annulation de : Révolution de Surf0004
    ctx.undo()
    assert len(gm.getInfos("Crb0000", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0001", 1).vertices()) == 2

    # keep=True => Crb0000 a 2 points
    # Révolution de Crb0001
    gm.makeRevol (["Crb0001"], Mgx3D.RotX(180), True)
    assert len(gm.getInfos("Crb0000", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0002", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0003", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0004", 1).vertices()) == 2
    assert len(gm.getInfos("Crb0005", 1).vertices()) == 2

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
    assert sorted(grm.getGeomSurfaces("Hors_Groupe_2D", 2)) == ['Surf0005', 'Surf0006']
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

def test_trou_oblong(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.micron)
    # Création du sommet Pt0000
    gm.newVertex (Mgx3D.Point(0, 0, 0))
    # Création du sommet Pt0001
    gm.newVertex (Mgx3D.Point(-30, 0, 0))
    # Création du sommet Pt0002
    gm.newVertex (Mgx3D.Point(-30, 415, 0))
    # Création du sommet Pt0003
    gm.newVertex (Mgx3D.Point(0, 415, 0))
    # Création du sommet Pt0004
    gm.newVertex (Mgx3D.Point(-15, 415, 0))
    # Création du segment Crb0000
    gm.newSegment("Pt0003", "Pt0000")
    # Création du segment Crb0001
    gm.newSegment("Pt0000", "Pt0001")
    # Création du segment Crb0002
    gm.newSegment("Pt0001", "Pt0002")
    # Création de l'arc de cercle Crb0003
    gm.newArcCircle("Pt0004", "Pt0003", "Pt0002", False)
    # Création de la surface Surf0000
    gm.newPlanarSurface (["Crb0000","Crb0001","Crb0002","Crb0003"], "")
    # Modifie le groupe TROU
    gm.addToGroup (["Surf0000"], 2, "TROU")
    # Révolution de Surf0000
    gm.makeRevol (["Surf0000"], Mgx3D.RotX(180), False)
    # Extrusion de Surf0004
    gm.makeExtrude (["Surf0004"], Mgx3D.Vector(0, 0, -700), False)
    # Modifie le groupe TROU
    gm.addToGroup (["Vol0001"], 3, "TROU")
    # Copie de  Vol0000 Vol0001
    gm.copy (["Vol0000","Vol0001"], False,"B")
    # Rotation de Vol0003 Vol0002 suivant  [  [ 0, 0, 0] ,  [ 1, 0, 0] , 180] 
    gm.rotate (["Vol0003","Vol0002"], Mgx3D.RotX(180))
    # Translation de Vol0003 Vol0002 suivant  [ 0, 0, -700] 
    gm.translate (["Vol0003","Vol0002"], Mgx3D.Vector(0, 0, -700))
    # Collage entre Vol0000 Vol0001 Vol0002 Vol0003
    gm.glue (["Vol0000","Vol0001","Vol0002","Vol0003"])
    # Fusion Booléenne de  Vol0000 Vol0001 Vol0002 Vol0003
    gm.fuse (["Vol0000","Vol0001","Vol0002","Vol0003"])

    assert gm.getNbVertices() == 13
    assert gm.getNbCurves() == 22
    assert gm.getNbSurfaces() == 12
    assert gm.getNbVolumes() == 1

    out, err = capfd.readouterr()
    assert len(err) == 0
