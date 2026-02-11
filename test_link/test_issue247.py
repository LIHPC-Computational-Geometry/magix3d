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