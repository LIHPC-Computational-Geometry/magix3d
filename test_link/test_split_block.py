import pyMagix3D as Mgx3D
import pytest

def test_split_block_1():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    tm.splitBlock("Bl0000", "Ar0001", .5)
    tm.splitBlock("Bl0001", "Ar0012", .5)

    with pytest.raises(RuntimeError) as excinfo:
        tm.splitBlock("Bl0004", "Ar1016", True)
    assert "getCoEdge impossible" in str(excinfo.value)

    with pytest.raises(RuntimeError) as excinfo:
        tm.splitBlock("Bl0004", "Ar0018", True)
    assert "split ne retrouve pas la direction" in str(excinfo.value)

    tm.splitBlock("Bl0004", "Ar0016", .5)
    tm.splitBlock("Bl0005", "Ar0036", .5)

    ctx.getMeshManager().newAllBlocksMesh()

def test_split_block_2(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newIJBoxesWithTopo(2, 1, False)
    tm.splitBlocksWithOgridV2(["Bl0000","Bl0001"], ["Fa0005","Fa0011"], .5, 7)
    assert tm.getNbBlocks() == 10

    ctx.getMeshManager().newAllFacesMesh()
    assert ctx.getMeshManager().getNbFaces() == 3230

    out, err = capfd.readouterr()
    assert len(err) == 0