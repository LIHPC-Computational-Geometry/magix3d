import pyMagix3D as Mgx3D
import pytest

def test_perturbation(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    def pert(x,y,z):
        if(z>=0.4):
            z = 0.4
        return [x,y,z]

    ctx.getTopoManager().newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, True, .5, 10, 10)
    ctx.getGeomManager().addToGroup (["Surf0000"], 2, "bord")
    ctx.getGroupManager().addCartesianPerturbation("bord", pert)
    ctx.getMeshManager().newAllBlocksMesh()
    out, err = capfd.readouterr()
    assert("Des hexaédres semblent inversés dans le maillage structuré" not in err)

def test_perturbation_surf(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    def pert(x,y,z):
        if(z>=0.4):
            z = 0.4
        return [x,y,z]

    ctx.getTopoManager().newSphereWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.ENTIER, True, .5, 10, 10)
    ctx.getGeomManager().addToGroup (["Surf0000"], 2, "bord")
    ctx.getGroupManager().addCartesianPerturbation("bord", pert)
    ctx.getTopoManager().addToGroup (["Fa0014"], 2, "aaa")
    # we mesh one of the diagonal faces to force the premesh generation
    # of its edges
    ctx.getMeshManager().newFacesMesh ( ["Fa0014"] )
    ctx.undo()
    # we mesh the surface on which the perturbation is applied to
    ctx.getMeshManager().newFacesMesh ( ["Fa0010", "Fa0016", "Fa0022", "Fa0028", "Fa0034", "Fa0040"] )
    # we mesh again the diagonal face : if edge Ar0058's premesh is used
    # we have bad quality elements
    ctx.getMeshManager().newFacesMesh ( ["Fa0014"] )
    out, err = capfd.readouterr()
    assert("Des polygones semblent retournés" not in out)