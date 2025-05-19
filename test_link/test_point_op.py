import pyMagix3D as Mgx3D

def test_point_op():
    p1 = Mgx3D.Point(10, 10, 10)
    p2 = p1 / 2
    assert p2.getX() == 5
    assert p2.getY() == 5
    assert p2.getZ() == 5
