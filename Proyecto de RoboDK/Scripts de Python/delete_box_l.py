"""
Este script borra el contenido del AGV - Caja L tras haber sido llenada. Con
esta acción se consigue un efecto visual de desaparición de las cajas que
llegan al final de la cinta 2. Además, cuando se borran todos los ítems del
interior, se libera memoria del programa al borrar ítems innecesarios.

Después de borrar el contenido del AGV - Caja L, este vuelve
a estar operativo y listo para el llenado de una nueva caja.
"""

# ---------------------------------------------------------------------------- #
# IMPORTACIONES NECESARIAS

from robodk.robolink import Robolink, ITEM_TYPE_OBJECT, InputError

# Cualquier interacción con RoboDK debe realizarse a través de RDK:
RDK = Robolink()

# ---------------------------------------------------------------------------- #
# VINCULAR OBJETOS Y BORRARLOS, si no existen, se captura la excepción.

try:
    box_to_be_deleted = RDK.Item('Caja L - aux', ITEM_TYPE_OBJECT)
    box_to_be_deleted.Delete()
except InputError:
    pass

# end of file #
