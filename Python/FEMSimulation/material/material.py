import logging


class Material:
    conductivity: float
    density: float
    specific_heat: float

    def debug_log(self, prefix: str = ""):
        logging.debug(f"{prefix}Material:")
        logging.debug(f"{prefix}\tconductivity: {self.conductivity}")
        logging.debug(f"{prefix}\tdensity: {self.density}")
        logging.debug(f"{prefix}\tspecific_heat: {self.specific_heat}")
