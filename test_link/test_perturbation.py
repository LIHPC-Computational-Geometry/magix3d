import pyMagix3D as Mgx3D
from math import *
import os

def test_perturbation1(capfd):
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

def test_perturbation2(capfd):
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

def test_perturbation3(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    def pert(x, y, z):
        return [x, y, z+x/10.0 + y/5.0]

    # Création d'une boite avec une topologie
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Modifie le groupe ZMAX
    ctx.getGeomManager().addToGroup (["Surf0005"], 2, "ZMAX")
    ctx.getGroupManager().addCartesianPerturbation("ZMAX", pert)
    # Création du maillage pour des faces
    ctx.getMeshManager().newFacesMesh ( ["Fa0005"] )
    # Sauvegarde du maillage (mli)
    mli_file_name = "perturbation3.mli2"
    ctx.getMeshManager().writeMli(mli_file_name)
    assert os.path.exists(mli_file_name)
    assert os.path.getsize(mli_file_name) > 0

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_perturbation4(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    def pert(x, y, z):
        return [x+0.1, y+0.1, z]

    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10, "A")
    ctx.getTopoManager().newBoxWithTopo (Mgx3D.Point(1, 0, 0), Mgx3D.Point(2, 1, 1), 10, 10, 10, "B")
    ctx.getGeomManager().glue(["Vol0000", "Vol0001"])
    ctx.getGroupManager().addCartesianPerturbation("B", pert)
    ctx.getTopoManager().setGeomAssociation(["Ar0011", "Ar0010", "Ar0008", "Ar0009", "Fa0003", "Fa0005", "Fa0004", "Fa0002"], "", True)
    ctx.getMeshManager().newBlocksMesh(["Bl0001"])
    ctx.getMeshManager().newBlocksMesh(["Bl0000"])

    out, err = capfd.readouterr()
    assert len(err) == 0

def test_perturbation5(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    def pert(r, t, p):
        return [ r*(1+cos(t*7)/20*cos(p*5)), t, p ]

    ctx.getTopoManager().newSphereWithTopo(Mgx3D.Point(0, 0, 0), 1, Mgx3D.Portion.QUART, True, .5, 10, 10)
    ctx.getGeomManager().addToGroup(["Surf0000"], 2, "EXT")
    ctx.getGroupManager().addPolarPerturbation("EXT", pert)
    ctx.getMeshManager().newFacesMesh(["Fa0010", "Fa0016", "Fa0022", "Fa0028"])
    mli_file_name = "perturbation4.mli2"
    ctx.getMeshManager().writeMli(mli_file_name)
    assert os.path.exists(mli_file_name)
    assert os.path.getsize(mli_file_name) > 0

    out, err = capfd.readouterr()
    assert len(err) == 0

