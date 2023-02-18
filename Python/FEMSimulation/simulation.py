import logging

from grid.grid import Grid
from material.material import Material


class Simulation:
    simulation_time: float
    simulation_time_step: float

    ambient_temperature: float

    convection: float

    initial_material_temperature: float
    material: Material

    grid: Grid

    def load_data_from_file(self, path: str):
        logging.info(f'Loading data from file "{path}"')

        with open(path) as data:
            lines_from_file: [str, ] = data.readlines()

        nodes_amount: int = 0
        elements_amount: int = 0

        nodes: [Grid.Element2D.Node] = []
        elements_indices: [[int, ], ] = []

        self.material = Material()
        line_idx = 0
        while line_idx < len(lines_from_file):
            line = lines_from_file[line_idx].strip()
            if line.startswith("SimulationTime"):
                self.simulation_time = float(line.split()[1])
            elif line.startswith("SimulationStepTime"):
                self.simulation_time_step = float(line.split()[1])
            elif line.startswith("Conductivity"):
                self.material.conductivity = float(line.split()[1])
            elif line.startswith("Alfa"):
                self.convection = float(line.split()[1])
            elif line.startswith("Tot"):
                self.ambient_temperature = float(line.split()[1])
            elif line.startswith("InitialTemp"):
                self.initial_material_temperature = float(line.split()[1])
            elif line.startswith("Density"):
                self.material.density = float(line.split()[1])
            elif line.startswith("SpecificHeat"):
                self.material.specific_heat = float(line.split()[1])
            elif line.startswith("Nodes number"):
                nodes_amount = int(line.split()[2])
            elif line.startswith("Elements number"):
                elements_amount = int(line.split()[2])
            elif line.startswith("*"):
                line_idx += 1
                if line.startswith("*Node"):
                    for line_idx in range(line_idx, line_idx + nodes_amount):
                        line = lines_from_file[line_idx]
                        id_, x, y = line.split(",")
                        nodes.append(Grid.Element2D.Node(int(id_) - 1, float(x), float(y)))
                elif line.startswith("*Element"):
                    for line_idx in range(line_idx, line_idx + elements_amount):
                        line = lines_from_file[line_idx]
                        element_number, n1, n2, n3, n4 = line.split(",")
                        elements_indices.append([int(i) - 1 for i in (n1, n2, n3, n4)])
                elif line.startswith("*BC"):
                    line = lines_from_file[line_idx]
                    for bc_elem_id in line.split(","):
                        nodes[int(bc_elem_id) - 1].bc = True
                continue
            line_idx += 1

        self.grid = Grid(nodes, elements_indices, material=self.material)

        if logging.root.isEnabledFor(logging.DEBUG):
            self.debug_log()

    def debug_log(self):
        logging.debug("Simulation:")
        logging.debug(f"\tsimulation_time: {self.simulation_time}")
        logging.debug(f"\tsimulation_time_step: {self.simulation_time_step}")
        logging.debug(f"\tambient_temperature: {self.ambient_temperature}")
        logging.debug(f"\tconvection: {self.convection}")
        logging.debug(f"\tinitial_material_temperature: {self.initial_material_temperature}")
        self.material.debug_log("\t")
        self.grid.debug_log("\t")

    def start(self):
        pass
