import math
import pyMagix3D as Mgx3D

def test_get_edge_length():
    unit_test (1.0, 5.0)
    unit_test (2.0, 3.0)
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    radius = 2.0
    length = 3.0

def unit_test(radius, length):
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    ctx.getTopoManager().newCylinderWithTopo (Mgx3D.Point(0, 0, 0), radius, Mgx3D.Vector(length, 0, 0), 360, True, .5, 10, 10, 10)
    # Ar0013 : arête de la face latérale
    # le bloc a pour longueur d'arete la diagonale du carre de cote radius
    assert math.isclose(ctx.getTopoManager().getEdgeLength("Ar0013"), square_diagonal(radius), abs_tol=1e-15)
    # la longueur projetee est un quart de circonference
    assert math.isclose(ctx.getTopoManager().getEdgeProjectedLength("Ar0013"), circumference_quarter(radius), abs_tol=1e-15)
    # Ar0023 : arête de du côté du cylindre
    # le bloc a pour longueur la longur du cylindre
    assert math.isclose(ctx.getTopoManager().getEdgeLength("Ar0023"), length, abs_tol=1e-15)
    # la longueur projetee est la longueur du bloc
    assert math.isclose(ctx.getTopoManager().getEdgeProjectedLength("Ar0023"), length, abs_tol=1e-15)
    # Ar0014 : arête de face du o-grid
    # le o-grid est à 0.5, donc la longueur d'arete est la moitie de la diagonale du carre de cote radius
    assert math.isclose(ctx.getTopoManager().getEdgeLength("Ar0014"), square_diagonal(radius) / 2.0, abs_tol=1e-15)
    # l'arete est associée à une face coplanaire avec le plan XY, donc la longueur projetée est égale à la longueur d'arete
    assert math.isclose(ctx.getTopoManager().getEdgeProjectedLength("Ar0014"), square_diagonal(radius) / 2.0, abs_tol=1e-15)
    # Ar0046 : arête de côté du o-grid
    # le bloc a pour longueur la longur du cylindre
    assert math.isclose(ctx.getTopoManager().getEdgeLength("Ar0046"), length, abs_tol=1e-15)
    # l'arête n'est associée à aucune entité géométrique, donc la longueur projetée est égale à la longueur d'arete
    assert math.isclose(ctx.getTopoManager().getEdgeProjectedLength("Ar0046"), length, abs_tol=1e-15)

def square_diagonal(radius):
    return math.sqrt(2.0 * radius ** 2)

def circumference_quarter(radius):
    return (math.pi * radius) / 2.0
