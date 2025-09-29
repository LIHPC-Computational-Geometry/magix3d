import pyMagix3D as Mgx3D
import pytest

def test_set_block_meshing_property(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    gm.newBox (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1))
    gm.newBox (Mgx3D.Point(2, 0, 0), Mgx3D.Point(3, 1, 1))
    gm.newBox (Mgx3D.Point(3, 0, 0), Mgx3D.Point(4, 1, 1))

    tm.newUnstructuredTopoOnGeometry("Vol0000")
    tm.newUnstructuredTopoOnGeometry("Vol0001")
    tm.newUnstructuredTopoOnGeometry("Vol0002")
    tm.newUnstructuredTopoOnGeometry("Vol0003")

    tm.setMeshingProperty (Mgx3D.BlockMeshingPropertyDelaunayTetgen(Mgx3D.BlockMeshingPropertyDelaunayTetgen.Min), ["Bl0000"])
    tm.setMeshingProperty (Mgx3D.BlockMeshingPropertyDelaunayTetgen(Mgx3D.BlockMeshingPropertyDelaunayTetgen.QC, 0.8, 0.1), ["Bl0001"])
    tm.setMeshingProperty (Mgx3D.BlockMeshingPropertyDelaunayTetgen(Mgx3D.BlockMeshingPropertyDelaunayTetgen.QCRadius, 0.5), ["Bl0002"])
    tm.setMeshingProperty (Mgx3D.BlockMeshingPropertyDelaunayTetgen(Mgx3D.BlockMeshingPropertyDelaunayTetgen.QCMaxVol, 0.001), ["Bl0003"])

    ctx.getMeshManager().newBlocksMesh(["Bl0000", "Bl0001", "Bl0002", "Bl0003"])

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_set_edge_meshing_property():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    emp = Mgx3D.EdgeMeshingPropertyUniform(12)
    tm.setMeshingProperty (emp, ["Ar0000"])

    with pytest.raises(RuntimeError) as excinfo:
        ctx.getMeshManager().newAllBlocksMesh()
    assert "2 arêtes n'ont pas la même discrétisation" in str(excinfo.value)

def test_set_face_meshing_property(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()

    gm.newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    tm.newUnstructuredTopoOnGeometry("Vol0000")
    tm.setMeshingProperty (Mgx3D.FaceMeshingPropertyDelaunayGMSH(0.03, 0.07), ["Fa0005"])
    ctx.getMeshManager().newAllBlocksMesh()

    out, err = capfd.readouterr()
    assert len(err) == 0
