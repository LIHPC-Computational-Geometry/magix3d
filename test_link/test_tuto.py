import os
import pyMagix3D as Mgx3D

if 'TUTO_SCRIPTS_DIR' in os.environ:
    tuto_scripts_dir = os.getenv('TUTO_SCRIPTS_DIR')
else:
    tuto_scripts_dir = "../Docs/resources"

deux_boites_mli_file_name = "deuxBoites.mli2"
deux_boites_rp_mli_file_name = "deuxBoitesRP.mli2"

def test_tuto_2boitesConformes():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    exec(open(tuto_scripts_dir + "/2boitesConformes.py").read())
    gm = ctx.getGeomManager()
    assert gm.getNbVolumes()==2
    mm = ctx.getMeshManager()
    assert mm.getNbVolumes()==2
    assert os.path.exists(deux_boites_mli_file_name)
    assert os.path.getsize(deux_boites_mli_file_name) > 0
    assert mm.getNbRegions()==2760 #Nb cells
    
def test_tuto_2boitesAvecRecouvrement():
    ctx = Mgx3D.getStdContext()
    ctx.clearSession() # Clean the session after the previous test
    exec(open(tuto_scripts_dir + "/2boitesAvecRecouvrement.py").read())
    gm = ctx.getGeomManager()
    assert gm.getNbVolumes()==2
    mm = ctx.getMeshManager()
    assert mm.getNbVolumes()==2
    assert os.path.exists(deux_boites_rp_mli_file_name)
    assert os.path.getsize(deux_boites_rp_mli_file_name) > 0
    assert mm.getNbRegions()==1900 #Nb cells
