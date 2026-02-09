import pyMagix3D as Mgx3D

def test_issue47_crb():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))# Révolution de Crb0010
    # OCC n'a pas pu effectuer une révolution d'une surface
    gm.makeRevol (["Crb0010"], Mgx3D.RotX(90), False)

def test_issue47_pt():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # Création de la boite Vol0000
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))# Révolution de Crb0010
    # CRASH...
    gm.makeRevol (["Pt0002"], Mgx3D.RotX(90), False)