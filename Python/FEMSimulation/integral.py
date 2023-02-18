from math import sqrt
from typing import NamedTuple


class IntegralPoints(NamedTuple):
    arguments: tuple[float]  # Arguments to function at which calculate function value.
    coefficients: tuple[float]  # Valueability of function results at arguments.

    def __iter__(self) -> (float, float):
        for argument, coefficient in zip(self.arguments, self.coefficients):
            yield argument, coefficient


# noinspection PyTypeChecker
gauss_legendre_integral_points = {
    2: IntegralPoints(
        arguments=(-1 / sqrt(3), 1 / sqrt(3)),
        coefficients=(1, 1)
    ),
    3: IntegralPoints(
        arguments=(-sqrt(3 / 5), 0, sqrt(3 / 5)),
        coefficients=(5 / 9, 8 / 9, 5 / 9)
    ),
    4: IntegralPoints(
        arguments=(-0.906180, -0.538469, 0, 0.906180, 0.538469),
        coefficients=(0.236927, 0.478629, 0.568889, 0.236927, 0.478629)
    )
}


class Element:
    class NodesFunctions:
        """ Node functions for -1 to 1 system. """

        class KsiEta:
            class N1:
                @staticmethod
                def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                    return (1 - ksi) * (1 - eta) / 4

                class Derivative:
                    class OverKsi:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return -(1 / 4) * (1 - eta)

                    class OverEta:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return -(1 / 4) * (1 - ksi)

            class N2:
                @staticmethod
                def __new__(cls, ksi: float, eta: float, *args, **kwargs):
                    return (1 + ksi) * (1 - eta) / 4

                class Derivative:
                    class OverKsi:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return (1 / 4) * (1 - eta)

                    class OverEta:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return -(1 / 4) * (1 + ksi)

            class N3:
                @staticmethod
                def __new__(cls, ksi: float, eta: float, *args, **kwargs):
                    return (1 + ksi) * (1 + eta) / 4

                class Derivative:
                    class OverKsi:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return (1 / 4) * (1 + eta)

                    class OverEta:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return (1 / 4) * (1 + ksi)

            class N4:
                @staticmethod
                def __new__(cls, ksi: float, eta: float, *args, **kwargs):
                    return (1 - ksi) * (1 + eta) / 4

                class Derivative:
                    class OverKsi:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return -(1 / 4) * (1 + eta)

                    class OverEta:
                        @staticmethod
                        def __new__(cls, ksi: float, eta: float, *args, **kwargs) -> float:
                            return (1 / 4) * (1 - ksi)

            @classmethod
            def get(cls):
                return [cls.N1, cls.N2, cls.N3, cls.N4]

            @classmethod
            def get_derivatives_over_ksi(cls):
                return [func.Derivative.OverKsi for func in cls.get()]

            @classmethod
            def get_derivatives_over_eta(cls):
                return [func.Derivative.OverEta for func in cls.get()]

            @classmethod
            def calculate_functions_values(cls, functions: [callable], integral_points: IntegralPoints):
                args = integral_points.arguments
                elements = [[func(ksi, eta) for func in functions] for eta in args for ksi in args]
                return elements

            @classmethod
            def calculate_functions_values_with_wages(cls, functions: [callable], integral_points: IntegralPoints):
                args = integral_points.arguments
                coefficients = integral_points.coefficients
                elements = [[func(ksi, eta) * w1 * w2  for func in functions] for eta, w1 in zip(args, coefficients)
                            for ksi, w2 in zip(args, coefficients)]
                return elements

            @classmethod
            def calculate_values(cls, integral_points: IntegralPoints):
                return cls.calculate_functions_values(cls.get(), integral_points)

            @classmethod
            def calculate_values_with_wages(cls, integral_points: IntegralPoints):
                return cls.calculate_functions_values_with_wages(cls.get(), integral_points)

            @classmethod
            def calculate_derivatives_over_ksi_values(cls, integral_points: IntegralPoints):
                return cls.calculate_functions_values(cls.get_derivatives_over_ksi(), integral_points)

            @classmethod
            def calculate_derivatives_over_eta_values(cls, integral_points: IntegralPoints):
                return cls.calculate_functions_values(cls.get_derivatives_over_eta(), integral_points)

