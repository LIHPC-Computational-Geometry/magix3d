from docutils import nodes
from docutils.parsers.rst import Directive
from docutils.parsers.rst import directives

familles = {
                "géométrie": ["Opérations géométriques", "Geometry.png" ],
                "topologie": ["Opérations topologiques", "Topology.png" ],
                "maillage": ["Opérations sur les maillages", "Box.png" ],
                "repère": ["Opérations repères", "Repere.png"]
            }

sous_familles = {
                    "points": ['Opérations géométriques associées aux points', "Geometry.png" ],
                    "courbes": ['Opérations géométriques associées aux courbes', "Geometry.png" ],
                    "surface": ['Opérations géométriques associées aux surfaces', "Geometry.png" ],
                    "volumes": ['Opérations géométriques associées aux volumes', "Create_volume.png" ],
                    "sommets": ['Opérations topologiques associées aux sommets', "Create_vertex.png" ],
                    "faces": ['Opérations topologiques associées aux faces', "Create_face.png" ],
                    "arêtes": ['Opérations topologiques associées aux arêtes', "Create_edge.png" ],
                    "blocs": ['Opérations topologiques associées aux blocs', "Create_block.png" ],
                    "nuages": ['Opérations associées aux nuages de surface', "Geometry.png" ],
                    "surfacesmaillage": ['Opérations associées aux surfaces de maillage', "Geometry.png" ],
                    "volumesmaillage": ['Opérations associées aux volumes de maillage', "Geometry.png" ],
                    "repère": ["Opérations associées aux repères", "Repere.png"]
                }

operations = {
                "boite": ["Création de boite", "Create_box.png"],
                "operationsbooléennes": ["Opérations booléennes", "Boolean.png"],
                "discrétisation": ["Paramétrage de la discrétisation des arêtes ", "Edges_meshing_properties.png"],
                "découpageface": ["Découpage d'une face selon une arête", "Topo_split_face.png"],
                "découpagefaces": ["Découpage de faces [2D] selon une arête", "Topo_split_faces.png"],
                "geomtogroupe": ["Ajout/suppression/affectation d'entités géométriques à un groupe", "Geom_add_to_group.png"],
                "topotogroupe": ["Ajout/suppression/affectation d'entités topologiques à un groupe", "Topo_add_to_group.png"],
                "blocparrévolution": ["Création de blocs par révolution de la topologie 2D", "Create_block_by_edge_revolution.png"],
                "cylindre": ["Création de cylindre", "Cylinder.png"],
                "cone": ["Création de cone", "image78.png"],
                "sphère": ["Création de sphère", "image73.png"],
                "destructiongeom": ["Destruction d'entités géométriques", "Box_destruction.png"],
                "maillageblocs": ["Paramétrage du maillage des blocs", "Blocks_meshing_properties.png"],
                "découpageblocogrid": ["Découpage de bloc avec o-grid ", "Topo_split_block_with_ogrid.png"],
                "repère": ["Création d'un repère à partir de points", "Repere.png"],
                "coordonnéessommets": ["Modification des coordonnées de sommets topologiques", "Topo_project_vertices.png"],
                "extraitblocs": ["Extrait des blocs", "Topo_extract_blocks.png"],
                "associations": ["Associations blocs topologiques -> volume géométrique","Topo_entities_association.png"],
                "blocsassociations": ["Associations blocs topologiques -> volume géométrique","Blocks_association.png"],
                "decoupagearetes": ["Découpage d'arêtes topologiques ", "Edge_cutting.png"],
                "sensdiscrétisation": ["Sens de discrétisation d'arêtes topologiques", "Topo_edge_direction.png"],
                "prolongation":["Prolongation de découpage de face à partir d'un sommet situé sur une arête", "Topo_extend_split_face.png"],
                "homothétie": ["Homothétie d'entités topologiques", "Topo_homothety.png"],
                "fusion": ["Fusion de faces topologiques", "Topo_glue_2_faces.png"],
                "union":["Opérations booléennes", "Union.png"],
                "bloc":["Création de bloc par association à un volume", "Create_block.png"],
                "découpagebloc":["Découpage de blocs selon une arête", "Topo_split_block.png"],
                "modificationassociation":["Modification des associations topologiques vers entités géométriques", "Topo_entities_association.png"],
                "destructiontopo": ["Destruction d'entités topologiques", "image89.png"],
                "rotationgeom": ["Rotation d'entités géométriques", "image54.png"],
                "translation": ["Translation d'entités géométriques", "image53.png"]
             }

def familleOperation(argument):
    """Conversion function for the "famille" option."""
    return directives.choice(argument, list(familles))

def sousFamilleOperation(argument):
    """Conversion function for the "sousfamille" option."""
    return directives.choice(argument, list(sous_familles))

def operation(argument):
    """Conversion function for the "operation" option."""
    return directives.choice(argument, list(operations))

class TableOperation(Directive):
   required_arguments = 0
   optional_arguments = 0
   final_argument_whitespace = True
   option_spec = {
    'famille': familleOperation,
    'sousfamille': sousFamilleOperation,
    'operation': operation,
   }
   has_content = True
   add_index = True

   def run(self):
        # gives you access to the options of the directive
        options = self.options
        famille = familles[options["famille"]][0]
        img_famille = familles[options["famille"]][1]
        sous_famille = sous_familles[options["sousfamille"]][0]
        img_sous_famille = sous_familles[options["sousfamille"]][1]
        operation = operations[options["operation"]][0]
        img_operation = operations[options["operation"]][1]
        table_data = [  [nodes.paragraph(text='Groupe'), 
                            nodes.paragraph(text='Icône'), 
                            nodes.paragraph(text='Nom associé')], 
                        [nodes.paragraph(text='Famille d’opérations'),
                            image(img_famille),
                            nodes.paragraph(text=famille)],
                        [nodes.paragraph(text='Sous-famille d’opérations'), 
                            image(img_sous_famille),
                            nodes.paragraph(text=sous_famille)],
                        [nodes.paragraph(text='Opération'), 
                            image(img_operation),
                            nodes.paragraph(text=operation)]]
        col_widths = [30, 10, 60]
        header_rows = 1
        stub_columns = 0
        table = build_table_from_list(table_data, col_widths, header_rows, stub_columns)
        return [table]

def image(path):
    image = nodes.image()
    image['uri'] = '/images/' + path
    return image

def build_table_from_list(table_data, col_widths, header_rows, stub_columns):
    table = nodes.table()
    tgroup = nodes.tgroup(cols=len(col_widths))
    table += tgroup
    for col_width in col_widths:
        colspec = nodes.colspec(colwidth=col_width)
        if stub_columns:
            colspec.attributes['stub'] = 1
            stub_columns -= 1
        tgroup += colspec
    rows = []
    for row in table_data:
        row_node = nodes.row()
        for cell in row:
            entry = nodes.entry()
            entry += cell
            row_node += entry
        rows.append(row_node)
    if header_rows:
        thead = nodes.thead()
        thead.extend(rows[:header_rows])
        tgroup += thead
    tbody = nodes.tbody()
    tbody.extend(rows[header_rows:])
    tgroup += tbody
    return table

def toParagraph(s):
    return nodes.paragraph(text=s)

def checkValues(argument):
    return [i.split(',') for i in str(argument).split('\n')]

class TableParametresOperation(Directive):
   required_arguments = 0
   optional_arguments = 0
   final_argument_whitespace = True
   option_spec = {'valeurs': checkValues}
   has_content = False
   add_index = True

   def run(self):
        # gives you access to the options of the directive
        invals = self.options["valeurs"]
        values = [[toParagraph(j) for j in i] for i in invals]
        col_widths = [15, 30]
        header_rows = 0
        stub_columns = 0
        table = build_table_from_list(values, col_widths, header_rows, stub_columns)
        return [table]

def setup(app):
    app.add_directive('taboperation', TableOperation)
    app.add_directive('taboperationparams', TableParametresOperation)
    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }