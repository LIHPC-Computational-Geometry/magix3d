import pyMagix3D as Mgx3D
import pytest

def test_revol_crb():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))# Révolution de Crb0010

    assert gm.getNbSurfaces() == 6
    # Message avant correction :
    # OCC n'a pas pu effectuer une révolution d'une surface
    gm.makeRevol (["Crb0010"], Mgx3D.RotX(90), False)
    assert gm.getNbSurfaces() == 7

def test_revol_pt():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))# Révolution de Crb0010

    # Message avant correction : CRASH...
    assert gm.getNbCurves() == 12
    gm.makeRevol (["Pt0002"], Mgx3D.RotX(90), False)
    assert gm.getNbCurves() == 13

def test_rotate_crb():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))

    with pytest.raises(RuntimeError) as excinfo:
        gm.rotate (["Crb0010"], Mgx3D.RotX(120))
    assert "Opération impossible car Crb0010 est reliée à Vol0000" in str(excinfo.value)

def test_rotate_pt():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))

    with pytest.raises(RuntimeError) as excinfo:
        gm.rotate (["Pt0002"], Mgx3D.RotX(120))
    assert "Opération impossible car Pt0002 est reliée à Vol0000" in str(excinfo.value)

# Issue#248: révolution de courbes qui crée un volume
def test_volume_not_created():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    gm.newVertex(Mgx3D.Point(0,0,0))
    gm.newVertex(Mgx3D.Point(0,2,0))
    gm.newVertex(Mgx3D.Point(-3,0,0))
    gm.newVertex(Mgx3D.Point(-2,0,0))
    gm.newVertex(Mgx3D.Point(2,0,0))
    gm.newVertex(Mgx3D.Point(3,0,0))
    gm.newSegment("Pt0002", "Pt0003")
    gm.newSegment("Pt0004", "Pt0005")
    gm.newArcCircle("Pt0000", "Pt0004", "Pt0001", True)
    gm.newArcCircle("Pt0000", "Pt0001", "Pt0003", True)
    gm.newArcCircle("Pt0000", "Pt0005", "Pt0002", True)
    gm.newPlanarSurface(["Crb0001", "Crb0004", "Crb0000", "Crb0003", "Crb0002"], "")

    assert gm.getNbVolumes() == 0
    gm.makeRevol(["Crb0002", "Crb0003"], Mgx3D.RotX(360), False)
    assert gm.getNbVolumes() == 0
