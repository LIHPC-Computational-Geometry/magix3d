import pyMagix3D as Mgx3D
import pytest

def test_extend_split_face():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Découpage de la face Fa0003
    tm.splitFace ("Fa0003", "Ar0002", .3, True)
    assert tm.getNbFaces() == 7
    with pytest.raises(RuntimeError) as excinfo:
        # Découpage de la face Fa0001 par prolongation
        tm.extendSplitFace ("Fa0001", "Som0007")
    expected = "La face ne peut être découpée avec un sommet au sommet de la face (en prendre un sur un côté)"
    assert expected in str(excinfo.value)
    with pytest.raises(RuntimeError) as excinfo:
        # Découpage de la face Fa0002 par prolongation
        tm.extendSplitFace ("Fa0002", "Som0009")
    expected = "La face ne peut être découpée avec ce sommet, on ne trouve pas d'arête contenant ce sommet"
    assert expected in str(excinfo.value)
    # Découpage de la face Fa0001 par prolongation
    tm.extendSplitFace ("Fa0001", "Som0009")
    assert tm.getNbFaces() == 8

def test_extend_split_face_with_degenerated_face():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    tm = ctx.getTopoManager()
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des liens topologiques
    ctx.getGeomManager().destroy(["Vol0000"], True)
    # Découpage de la face Fa0000
    tm.splitFace ("Fa0000", "Ar0000", .5, True)
    assert tm.getNbFaces() == 7
    # Fusion de sommets Som0007 avec Som0006
    tm.snapVertices ("Som0007", "Som0006", True)
    # Découpage de la face Fa0003 par prolongation
    # On est en face de la dégénérescence
    tm.extendSplitFace ("Fa0003", "Som0009")
    assert tm.getNbFaces() == 8
    # Découpage de la face Fa0009
    # On n'est pas en face de la dégénérescence
    tm.splitFace ("Fa0009", "Ar0010", .5, True)
    assert tm.getNbFaces() == 8

def test_no_more_infinite_loop():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    gm = ctx.getGeomManager()
    tm = ctx.getTopoManager()
    # Création d'une boite avec une topologie
    tm.newBoxWithTopo (Mgx3D.Point(0, 0, 0), Mgx3D.Point(1, 1, 1), 10, 10, 10)
    # Suppression d'entités géométriques et suppression des liens topologiques
    gm.destroy(["Vol0000"], True)
    # Destruction des entités topologiques Bl0000
    tm.destroy (["Bl0000"], False)
    tm.splitFaces (["Fa0005", "Fa0001", "Fa0004", "Fa0000"], "Ar0005", .5, .5)
    assert tm.getNbFaces() == 10
