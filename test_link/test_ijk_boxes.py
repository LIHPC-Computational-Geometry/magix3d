import pyMagix3D as Mgx3D

def test_ij_boxes(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    tm.setDefaultNbMeshingEdges(10)
    tm.newIJBoxesWithTopo(5, 2, False)
    emp = Mgx3D.EdgeMeshingPropertyUniform(20)
    tm.setParallelMeshingProperty(emp, "Ar0000")
    mm.newAllBlocksMesh()
    assert mm.getNbRegions() == 20000

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_ijk_boxes(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    mm = ctx.getMeshManager()

    tm.setDefaultNbMeshingEdges(10)
    tm.newIJKBoxesWithTopo(2, 2, 2, False)
    mm.newAllBlocksMesh()
    assert mm.getNbRegions() == 8000

    out, err = capfd.readouterr()
    assert len(err) == 0