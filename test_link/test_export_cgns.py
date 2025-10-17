import os
import pyMagix3D as Mgx3D

def test_export_box(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    cgnsfilename = "demiCylinder.cgns"
    ctx.getTopoManager().newCylinderWithTopo (Mgx3D.Point(0, 0, 0), 1, Mgx3D.Vector(10, 0, 0), 360, True, .5, 10, 10, 10)
    ctx.getTopoManager().splitAllBlocks ("Ar0013",.5)
    ctx.getTopoManager().splitAllBlocks ("Ar0004",.5)
    ctx.getTopoManager().splitAllBlocks ("Ar0070",.5)
    ctx.getTopoManager().splitAllBlocks ("Ar0069",.5)
    ctx.getTopoManager().splitAllBlocks ("Ar0176",.5)
    ctx.getTopoManager().splitAllBlocks ("Ar0175",.5)
    ctx.getTopoManager ( ).destroy (["Bl0075","Bl0076","Bl0077","Bl0078","Bl0079","Bl0080","Bl0081","Bl0082","Bl0091","Bl0092","Bl0093","Bl0094","Bl0095","Bl0096","Bl0097","Bl0098","Bl0099","Bl0100","Bl0101","Bl0102","Bl0115","Bl0116","Bl0117","Bl0118","Bl0119","Bl0120","Bl0121","Bl0122","Bl0131","Bl0132","Bl0133","Bl0134","Bl0135","Bl0136","Bl0137","Bl0138","Bl0139","Bl0140","Bl0141","Bl0142","Bl0083","Bl0084","Bl0085","Bl0086","Bl0123","Bl0124","Bl0125","Bl0126"], True)
    ctx.getTopoManager().addToGroup (["Fa0393", "Fa0392", "Fa0285", "Fa0284", "Fa0395", "Fa0394", "Fa0287", "Fa0286", "Fa0279", "Fa0280", "Fa0387", "Fa0388", "Fa0278", "Fa0277", "Fa0386", "Fa0385"], 2, "Paroi")
    ctx.getTopoManager().addToGroup (["Fa0295", "Fa0294", "Fa0403", "Fa0402", "Fa0353", "Fa0352", "Fa0461", "Fa0460", "Fa0345", "Fa0344", "Fa0453", "Fa0452", "Fa0329", "Fa0330", "Fa0437", "Fa0438"], 2, "Farfield")
    ctx.getTopoManager().addToGroup (["Fa0096", "Fa0097", "Fa0136", "Fa0137", "Fa0129", "Fa0130", "Fa0125", "Fa0124", "Fa0094", "Fa0095", "Fa0134", "Fa0135", "Fa0128", "Fa0127", "Fa0122", "Fa0123"], 2, "Sortie")
    ctx.getTopoManager().addToGroup (["Fa0360", "Fa0369", "Fa0359", "Fa0368", "Fa0252", "Fa0261", "Fa0251", "Fa0260", "Fa0327", "Fa0322", "Fa0326", "Fa0321", "Fa0435", "Fa0430", "Fa0434", "Fa0429"], 2, "Symetrie")
    emp = Mgx3D.EdgeMeshingPropertyUniform(10)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0363")
    emp = Mgx3D.EdgeMeshingPropertyUniform(10)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0364")
    emp = Mgx3D.EdgeMeshingPropertyUniform(10)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0269")
    emp = Mgx3D.EdgeMeshingPropertyUniform(10)
    ctx.getTopoManager().setParallelMeshingProperty (emp,"Ar0270")
    ctx.getMeshManager().newAllBlocksMesh()
    ctx.getMeshManager().exportBlocksForCGNS(cgnsfilename)

    assert os.path.exists(cgnsfilename)
    assert os.path.getsize(cgnsfilename) > 0
    out, err = capfd.readouterr()
    assert len(err) == 0