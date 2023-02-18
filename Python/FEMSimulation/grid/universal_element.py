import integral


class UniversalElement:
    class KsiEta:
        """ Universal 2D element: ksi, eta in range <-1, 1> """
        nodes_functions = integral.Element.NodesFunctions.KsiEta

        @staticmethod
        def interpolate(arguments: [float, float, float, float, ],
                        nodes_functions_values: [[float, float, float, float, ],  # N1(1), N2(1)...
                                                 [float, float, float, float, ],  # N1(2), N2(2)...
                                                 [float, float, float, float, ],  # N1(3), N2(3)...
                                                 [float, float, float, float, ], ],  # N1(4), N2(4)...
                        ) -> [float, float, float, float, ]:
            """ Interpolate:
                N1                        * x1           + N2                        * x2 + ...
                nodes_functions_values[0] * arguments[0] + nodes_functions_values[1] * arguments[2] + ...

                :param arguments: size N - see above
                :param nodes_functions_values: size integration_points x N
                :return interpolated values - size N
            """
            return [sum([node_function_value * argument
                         for argument, node_function_value in zip(arguments, node_function_values)
                         ]) for node_function_values in nodes_functions_values]

        def __init__(self, integral_points: integral.IntegralPoints):
            nf = self.nodes_functions
            self.nodes_functions_values = nf.calculate_values(integral_points)
            self.nodes_functions_derivatives_over_ksi_values = nf.calculate_derivatives_over_ksi_values(integral_points)
            self.nodes_functions_derivatives_over_eta_values = nf.calculate_derivatives_over_eta_values(integral_points)
            self.nodes_functions_derivatives = [(dksi, deta) for dksi, deta in
                                                zip(self.nodes_functions_derivatives_over_ksi_values,
                                                    self.nodes_functions_derivatives_over_eta_values)]

        def interpolate_using_derivatives_over_ksi(self, arguments: [float, ]) -> [float, ]:
            return UniversalElement.KsiEta.interpolate(arguments, self.nodes_functions_derivatives_over_ksi_values)

        def interpolate_using_derivatives_over_eta(self, arguments: [float, ]) -> [float, ]:
            return UniversalElement.KsiEta.interpolate(arguments, self.nodes_functions_derivatives_over_eta_values)
