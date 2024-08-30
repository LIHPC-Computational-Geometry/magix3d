import pyMagix3D as Mgx3D

def test_brep():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()

    # 1. Export
    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    gm.newSphere (Mgx3D.Point(2, 0, 0), 1, Mgx3D.Portion.ENTIER)
    gm.exportBREP("test_brep_all.brep")
    gm.exportBREP(["Vol0001"], "test_brep_select.brep")

    # 2. Import All
    ctx.clearSession() # Clean the session after the previous test
    gm.importBREP("test_brep_all.brep")
    assert gm.getNbVolumes()==2
    assert gm.getNbSurfaces()==7
    assert gm.getNbCurves()==13
    assert gm.getNbVertices()==10

    # 3. Import Selection
    ctx.clearSession() # Clean the session after the previous test
    gm.importBREP("test_brep_select.brep")
    assert gm.getNbVolumes()==1
    assert gm.getNbSurfaces()==1
    assert gm.getNbCurves()==1
    assert gm.getNbVertices()==2
