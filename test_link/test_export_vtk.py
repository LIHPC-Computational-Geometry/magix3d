import os
import pyMagix3D as Mgx3D


def test_export_box(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    vtkfilename = "box.vtk"
    ctx.getGeomManager().newBox (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1))
    ctx.getTopoManager().newUnstructuredTopoOnGeometry("Vol0000")
    ctx.getMeshManager().newAllFacesMesh()
    ctx.getGeomManager().exportVTK(vtkfilename)

    assert os.path.exists(vtkfilename)
    assert os.path.getsize(vtkfilename) > 0
    out, err = capfd.readouterr()
    assert len(err) == 0

def test_export_cylinder(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    vtkfilename = "cylinder.vtk"
    ctx.getGeomManager().newCylinder(Mgx3D.Point(0.3, 0.5, 0.5), 0.4, Mgx3D.Vector(0.4, 0, 0), 360)
    ctx.getTopoManager().newUnstructuredTopoOnGeometry("Vol0000")
    ctx.getMeshManager().newAllFacesMesh()
    ctx.getGeomManager().exportVTK(vtkfilename)

    assert os.path.exists(vtkfilename)
    assert os.path.getsize(vtkfilename) > 0
    out, err = capfd.readouterr()
    assert len(err) == 0

def test_export_imported_step(capfd):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test

    test_folder = os.path.dirname(__file__)
    step_file_path = os.path.join(test_folder, 'B0.step')
    # Import STEP
    # Changement d'unité de longueur
    ctx.setLengthUnit(Mgx3D.Unit.meter)
    # Import STEP
    ctx.getGeomManager().importSTEP(step_file_path)

    vtkfilename = "B0.vtk"
    ctx.getTopoManager().newUnstructuredTopoOnGeometry("Vol0000")
    ctx.getMeshManager().newAllFacesMesh()
    ctx.getGeomManager().exportVTK(vtkfilename)

    assert os.path.exists(vtkfilename)
    assert os.path.getsize(vtkfilename) > 0
    out, err = capfd.readouterr()
    assert len(err) == 0
