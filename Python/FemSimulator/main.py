import copy
import logging
from typing import assert_never

import grid.grid
import integral
import simulation
import vtk_file_format
from grid.universal_element import UniversalElement
from matrix import Matrix

# logging.basicConfig(level=logging.DEBUG)
logging.basicConfig(level=logging.INFO)


def calculate_h_c_matrices(element: grid.grid.Grid.Element2D, conductivity: float,
                           integral_points: integral.IntegralPoints) -> (Matrix, Matrix):
    universal_element = UniversalElement.KsiEta(integral_points)

    jacobians = element.calculate_transform_jacobians_to_universal_element(universal_element)
    jacobians_dets = [jacobian.get_determinant() for jacobian in jacobians]

    # [4 x point][4 * N]Matrix{[2][1]}
    dns: [[Matrix, ]] = [
        [
            jacobian
            * Matrix(row=dnf).transpose()
            * (1 / jacobian_det)
            for dnf in zip(nf_d_ksi, nf_d_eta)
        ] for jacobian, jacobian_det, nf_d_ksi, nf_d_eta in
        zip(jacobians, jacobians_dets, universal_element.nodes_functions_derivatives_over_ksi_values,
            universal_element.nodes_functions_derivatives_over_eta_values)]

    # [4 x point][4 x N]
    dns_dxs = Matrix([[n[0][0] for n in point] for point in dns])
    dns_dys = Matrix([[n[1][0] for n in point] for point in dns])

    """ row x column """
    matrix_c = [(Matrix(matrix=[point_dns]))
    .transpose()  # column of N
    .multiply_first_vector_by_row(
        point_dns  # vector of N
    ) for point_dns in universal_element.nodes_functions_values]

    # [4 x point]Matrix{[4][4]} for every point: dNs/ds * (dNs/ds)^T
    dns_dxs_x_dns_dxs_ts = [Matrix(matrix=[dns_dx]).transpose().multiply_first_vector_by_row(dns_dx)
                            for dns_dx in dns_dxs]
    dns_dys_x_dns_dys_ts = [Matrix(matrix=[dns_dy]).transpose().multiply_first_vector_by_row(dns_dy)
                            for dns_dy in dns_dys]

    # k * 1/det_J * dNs/ds * (dNs/ds)^T
    hs = []
    for det_j, point_part1, point_part2 in zip(jacobians_dets, dns_dxs_x_dns_dxs_ts, dns_dys_x_dns_dys_ts):
        hs.append(Matrix(
            matrix=[[(elem1 + elem2) * det_j for elem1, elem2 in zip(point_part1_row, point_part2_row)
                     ] for point_part1_row, point_part2_row in zip(point_part1, point_part2)]))

    ip_coefficients = integral_points.coefficients
    wages = [w1 * w2 for w1 in ip_coefficients for w2 in ip_coefficients]

    h = Matrix.sum([matrix * wage for matrix, wage in zip(hs, wages)]) * conductivity
    c = Matrix.sum([matrix * wage * j_det for matrix, wage, j_det in zip(matrix_c, wages, jacobians_dets)]) \
        * (element.material.density * element.material.specific_heat)

    return h, c


# noinspection GrazieInspection
class GlobalMatrix(Matrix):
    def aggregate(self, matrix: Matrix, nodes: [grid.grid.Grid.Element2D.Node, ]):
        """
        :param matrix: h matrix N x N
        :param nodes: N nodes that created matrix
        """
        for row, node in zip(matrix, nodes):
            node_id_x = node.id_
            for col_idx, elem in enumerate(row):
                node_id_y = nodes[col_idx].id_
                self.matrix[node_id_x][node_id_y] += elem

    def aggregate1d(self, matrix: Matrix, nodes: [grid.grid.Grid.Element2D.Node, ]):
        """
        :param matrix: p matrix N x 1
        :param nodes: N nodes that created matrix
        """
        for row, node in zip(matrix, nodes):
            self.matrix[node.id_][0] += row[0]


def get_position_adjuster(edge_characteristic: dict):
    if "x" in edge_characteristic:
        return lambda position: (edge_characteristic["x"], position)
    if "y" in edge_characteristic:
        return lambda position: (position, edge_characteristic["y"])
    assert_never(edge_characteristic)


def calculate_hbc_p_matrices(element: [grid.grid.Grid.Element2D, ],
                             convection: float,
                             ambient_temp: float,
                             integral_points: integral.IntegralPoints) -> (Matrix, Matrix):
    if logging.root.isEnabledFor(logging.DEBUG):
        logging.debug(f"Calculating Hbc and P:")
        logging.debug(f"\tintegral_points: {integral_points}")
        logging.debug(f"\tconvection: {convection}")
        logging.debug(f"\tambient_temp: {ambient_temp}")
        element.debug_log(prefix="\t")

    hbc_parts = []
    p_parts = []

    nodes_functions = integral.Element.NodesFunctions.KsiEta.get()

    for edge in element.edges:
        if logging.root.isEnabledFor(logging.DEBUG):
            logging.debug(f"\tEdge:")
            edge.debug_log("\t\t")
        if not edge.bc:
            continue

        det_j = edge.calculate_length() / 2
        hbc_edge_parts = []
        p_edge_parts = []

        logging.debug(f"\t\tdet_j: {det_j}:")

        adjust_position: callable = get_position_adjuster(edge.characteristic_coordinates)

        for integral_point_argument, integral_point_coefficient in integral_points:
            ksi, eta = adjust_position(integral_point_argument)
            logging.debug(f"\t\tksi: {ksi}, eta: {eta}")

            nodes_values = Matrix(row=[node_func(ksi, eta) for node_func in nodes_functions])
            nodes_values_column = nodes_values.transpose()

            hbc_part = nodes_values_column * nodes_values

            p_edge_part = nodes_values_column * ambient_temp

            point_adjust = convection * integral_point_coefficient
            hbc_edge_parts.append(hbc_part * point_adjust)
            p_edge_parts.append(p_edge_part * point_adjust)

        p_parts.append(Matrix.sum(p_edge_parts) * det_j)
        hbc_parts.append(Matrix.sum(hbc_edge_parts) * det_j)

    return Matrix.sum(hbc_parts), Matrix.sum(p_parts)


def calculate_temperatures(h: Matrix, p: Matrix) -> Matrix:
    """[H] * {t} + {p} = 0
    [H] * {t} = -{p}
    [H]^-1 * [H] * {t} = [H]^-1 * {-p}
    {t} = [H]^-1 * {-p}"""
    return h.inverse() * p


# simulation_file = "Test1_4_4"
simulation_file = "Test2_4_4_MixGrid"
# simulation_file = "Test3_31_31_kwadrat"

sim = simulation.Simulation()
sim.load_data_from_file(r"Siatki MES-20221007/"f"{simulation_file}.txt")

integration_points = integral.gauss_legendre_integral_points[2]
logging.debug(f"Integral points: {integration_points}")

# sim.simulation_time_step /= 10
# sim.simulation_time *= 5

simulation_iterations: int = int(sim.simulation_time / sim.simulation_time_step)
logging.info("Starting simulation...")

temperatures = Matrix(dimension=simulation_iterations + 1)  # +1 to keep initial values

temperatures_at_nodes = Matrix(dimension=len(sim.grid.nodes), default_value=sim.initial_material_temperature)
temperatures[0] = temperatures_at_nodes.get_vector(column_idx=0)

h_c_template = GlobalMatrix(dimension=len(sim.grid.nodes), default_value=0)
p_template = GlobalMatrix(dimension=(1, len(sim.grid.nodes)), default_value=0)


for iteration in range(simulation_iterations):
    logging.info(f"Iteration: {iteration + 1} / {simulation_iterations}")

    h = copy.deepcopy(h_c_template)
    c = copy.deepcopy(h_c_template)
    p = copy.deepcopy(p_template)

    for element in sim.grid.elements:
        element.hbc, element.p = calculate_hbc_p_matrices(
            element=element,
            convection=sim.convection,
            ambient_temp=sim.ambient_temperature,
            integral_points=integration_points)

        element.h, element.c = calculate_h_c_matrices(
            element=element,
            conductivity=sim.material.conductivity,
            integral_points=integration_points
        )

        # matrix_h_with_hbc = element.h + element.hbc
        matrix_h_with_hbc = element.h
        if element.hbc:
            matrix_h_with_hbc += element.hbc

        h.aggregate(matrix_h_with_hbc, element.nodes)
        p.aggregate1d(element.p, element.nodes)
        c.aggregate(element.c, element.nodes)

    c_div_d_tau: Matrix = c / sim.simulation_time_step
    substitutive_h = h + c_div_d_tau

    # substitutive_p = p.to_matrix() + c_div_d_tau * temperatures_at_nodes
    substitutive_p = c_div_d_tau.multiply_by_vector(temperatures[iteration])
    substitutive_p += p

    temperatures_at_nodes: Matrix = calculate_temperatures(substitutive_h, substitutive_p)
    temperatures_at_nodes_row = temperatures_at_nodes.get_vector(column_idx=0)

    # ensure_temp_greater_than_100(temperatures_at_nodes_row)

    temperatures[iteration + 1] = temperatures_at_nodes_row[:]

    logging.info(f"Substitutive p: {substitutive_p.transpose()[0]}")
    logging.info(f"Temperatures: {temperatures_at_nodes_row}")
    logging.info(f"Temperatures (min, max): {min(temperatures_at_nodes_row)}, {max(temperatures_at_nodes_row)}")

    vtk_file_format.save_as_legacy_vtk(f"{simulation_file}_iteration_{iteration}",
                                       header=f"{simulation_file}_iteration_{iteration}",
                                       grid=sim.grid,
                                       temperatures=temperatures_at_nodes_row)

temperatures.print(precision=5, with_idx=True, with_min_max=True)
x = 1
