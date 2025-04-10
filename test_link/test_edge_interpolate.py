import pyMagix3D as Mgx3D
import pytest

def test_edgeInterpolate_destroy():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager ()
    mm = ctx.getMeshManager ()
    tm.newTopoVertex (Mgx3D.Point(0, 0, 0),"")
    tm.newTopoVertex (Mgx3D.Point(1, 0, 0),"")
    tm.newTopoVertex (Mgx3D.Point(1, 1, 0),"")
    tm.newTopoVertex (Mgx3D.Point(0, 1, 0),"")
    tm.newTopoVertex (Mgx3D.Point(2, 0, 0),"")
    tm.newTopoVertex (Mgx3D.Point(2, 1, 0),"")
    tm.newTopoEntity (["Som0000", "Som0001", "Som0002", "Som0003"], 2, "")
    tm.newTopoEntity (["Som0001", "Som0004", "Som0005", "Som0002"], 2, "")
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, ["Ar0005"])
    tm.setMeshingProperty (emp, ["Ar0001"])
    tm.destroy (["Fa0001"], True)
    # the meshing property of Ar0001 references Ar0005, which is destroyed
    # along with Fa0001. Thus, Ar0001 becomes unmeshable
    with pytest.raises(RuntimeError) as excinfo:
        ctx.getMeshManager().newAllFacesMesh()
    assert "Ar0001" in str(excinfo.value)
