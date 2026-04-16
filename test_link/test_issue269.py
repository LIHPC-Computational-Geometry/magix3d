import pyMagix3D as Mgx3D
import pytest

# boucle infinie sur une EdgeMeshingPropertyInterpolate avec une face 
def test_issue269_1():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    # Création d'un bloc unitaire mis dans le groupe aaa
    tm.newFreeTopoInGroup ("aaa", 3)
    # Changement de discrétisation pour les arêtes Ar0006
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0000")
    tm.setMeshingProperty (emp, ["Ar0006"])
    # Changement de discrétisation pour les arêtes Ar0004
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0004")
    tm.setMeshingProperty (emp, ["Ar0004"])
    # Changement de discrétisation pour les arêtes Ar0005
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0001")
    tm.setMeshingProperty (emp, ["Ar0005"])
    # Changement de discrétisation pour les arêtes Ar0007
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0005")
    tm.setMeshingProperty (emp, ["Ar0007"])

    with pytest.raises(RuntimeError) as excinfo:
        # Création du maillage pour tous les blocs
        ctx.getMeshManager().newAllBlocksMesh()
    assert "Erreur d'interpolation : Ar0006 --> Ar0004 qui a déjà été parcourue (boucle dans l'interpolation)." in str(excinfo.value)

# boucle infinie sur une EdgeMeshingPropertyGlobalInterpolate avec une liste d'aretes 
def test_issue269_2():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newFreeTopoInGroup ("aaa", 3)
    emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(10, ["Ar0007"], ["Ar0004"])
    tm.setMeshingProperty (emp, ["Ar0006"])
    emp = Mgx3D.EdgeMeshingPropertyGlobalInterpolate(10, ["Ar0006"], ["Ar0005"])
    tm.setMeshingProperty (emp, ["Ar0004"])

    with pytest.raises(RuntimeError) as excinfo:
        ctx.getMeshManager().newAllBlocksMesh()
    assert "Erreur d'interpolation : Ar0006 --> Ar0004 qui a déjà été parcourue (boucle dans l'interpolation)." in str(excinfo.value)

# autre boucle infinie sur une EdgeMeshingPropertyInterpolate avec une face 
def test_issue269_3():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()

    tm.newFreeTopoInGroup ("aaa", 2)
    tm.splitFaces (["Fa0000"], "Ar0002", .3, .5, True)
    tm.splitFaces (["Fa0002"], "Ar0005", .5, .5, True)
    tm.splitFaces (["Fa0004"], "Ar0013", .8, .5, True)
    tm.splitFaces (["Fa0001"], "Ar0008", .8, .5, True)

    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0007")
    tm.setMeshingProperty (emp, ["Ar0022"])
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0008")
    tm.setMeshingProperty (emp, ["Ar0021"])
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0003")
    tm.setMeshingProperty (emp, ["Ar0019", "Ar0020"])
    emp = Mgx3D.EdgeMeshingPropertyInterpolate(10, "Fa0003")
    tm.setMeshingProperty (emp, ["Ar0014", "Ar0015"])

    with pytest.raises(RuntimeError) as excinfo:
        ctx.getMeshManager().newFacesMesh ( ["Fa0008"] )
    assert "Erreur d'interpolation : Ar0019 --> Ar0014 qui a déjà été parcourue (boucle dans l'interpolation)." in str(excinfo.value)
