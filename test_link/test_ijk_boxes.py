import pyMagix3D as Mgx3D

def test_ij_boxes(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    tm.setDefaultNbMeshingEdges(100)
    tm.newIJBoxesWithTopo(5, 2, False)
    emp = Mgx3D.EdgeMeshingPropertyUniform(10)
    tm.setParallelMeshingProperty(emp, "Ar0000")
    mm.newAllBlocksMesh()
    assert mm.getNbRegions() == 1000000

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_ijk_boxes(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    tm.setDefaultNbMeshingEdges(100)
    tm.newIJKBoxesWithTopo(5, 5, 5, False)
    mm.newAllBlocksMesh()
    assert mm.getNbRegions() == 125000

    out, err = capfd.readouterr()
    assert len(err) == 0