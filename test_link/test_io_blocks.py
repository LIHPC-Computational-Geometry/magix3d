import os
import pyMagix3D as Mgx3D

test_folder = os.path.dirname(__file__)

def test_import_no_assoc_250():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    mgxt_filename = os.path.join(test_folder, "data/half_sphere_250.mgxt")
    tm.importBlocks(mgxt_filename, False)

    assert tm.getNbBlocks()==6
    assert tm.getNbFaces()==26
    assert tm.getNbEdges()==39
    assert len(tm.getVertices()) == 20

def test_import_no_assoc_260():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    mgxt_filename = os.path.join(test_folder, "data/half_sphere_260.mgxt")
    tm.importBlocks(mgxt_filename, False)

    assert tm.getNbBlocks()==6
    assert tm.getNbFaces()==26
    assert tm.getNbEdges()==39
    assert len(tm.getVertices()) == 20

def test_export_no_assoc():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager ()
    tm = ctx.getTopoManager ()

    ctx.getTopoManager().newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.DEMI, True, .5, 10, 10)
    mgxt_filename = os.path.join(test_folder, "data/half_sphere_export.mgxt")
    tm.exportBlocks(mgxt_filename, False)
    assert os.path.exists(blk_filename)
    assert os.path.getsize(blk_filename) > 0

    ctx.clearSession()
    tm.importBlocks(mgxt_filename, False)
    assert tm.getNbBlocks()==6
    assert tm.getNbFaces()==26
    assert tm.getNbEdges()==39
    assert len(tm.getVertices()) == 20
