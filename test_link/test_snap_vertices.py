import pyMagix3D as Mgx3D
import pytest

def test_snap_vertices_1():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    tm.splitFace ("Fa0000", "Ar0001", .5, True)
    tm.splitBlocksWithOgridV2 (["Bl0000"], ["Fa0007","Fa0006","Fa0005","Fa0004"], .5, 10)
    with pytest.raises(RuntimeError) as excinfo:
        tm.snapVertices ("Fa0022", "Fa0021", True)
    assert "il n'est pas prévu pour le moment le cas avec plusieurs arêtes communes entre les sommets fusionnés" in str(excinfo.value)

def test_snap_vertices_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newIJBoxesWithTopo(1, 2, False)
    tm.snapVertices ("Ar0001", "Ar0003", True)
    tm.snapVertices ("Ar0013", "Ar0015", True)

    out, err = capfd.readouterr()
    assert len(err) == 0