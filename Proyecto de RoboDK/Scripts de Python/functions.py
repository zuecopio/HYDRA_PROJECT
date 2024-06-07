"""
En este script se definen las funciones necesarias para el cobot (assemble_box)
y para el robot industrial (fill_box).

Subprogramas para (fill_box): (copy_object) y (pick_dispositivo).
"""

# ---------------------------------------------------------------------------- #
# IMPORTACIONES NECESARIAS

from json.__init__ import loads as json_loads
import numpy as np

from robodk.robolink import ITEM_TYPE_FRAME, ITEM_TYPE_PROGRAM
from robodk.robolink import ITEM_TYPE_TARGET, ITEM_TYPE_OBJECT

from robodk.robolink import Robolink, RUNMODE_SIMULATE

from robodk.robomath import xyzrpw_2_pose

# Cualquier interacción con RoboDK debe realizarse a través de RDK:
RDK = Robolink()

# ---------------------------------------------------------------------------- #
# VINCULAR ROBOTS DE ROBODK -> ITEM_TYPE_ROBOT

colaborative_robot = RDK.Item('Omron TM5X-900')
industrial_robot   = RDK.Item('Denso VS-6577G-B')
linear_rail_robot  = RDK.Item('Brooks PreciseFlex Linear Rail 1500')

# ---------------------------------------------------------------------------- #
# VINCULAR CINTAS (AGV - Cajas) DE ROBODK -> ITEM_TYPE_ROBOT

agv_caja_s_y = RDK.Item('AGV - Caja S - Rotación en Y')
agv_caja_s_z = RDK.Item('AGV - Caja S - Rotación en Z')

agv_caja_m_y = RDK.Item('AGV - Caja M - Rotación en Y')
agv_caja_m_z = RDK.Item('AGV - Caja M - Rotación en Z')

agv_caja_l_y = RDK.Item('AGV - Caja L - Rotación en Y')
agv_caja_l_z = RDK.Item('AGV - Caja L - Rotación en Z')

# ---------------------------------------------------------------------------- #
# VINCULAR HERRAMIENTAS DE ROBODK -> ITEM_TYPE_TOOL

industrial_tool = RDK.Item('herramienta_industrial')

# ---------------------------------------------------------------------------- #

# Esto ejecutará el programa API en el simulador (offline programming).
RDK.setRunMode(RUNMODE_SIMULATE)

# Establecer el parámetro de redondeo (radio de fusión, curvatura, ...)
industrial_robot.setRounding(10)

# Establecer velocidad lineal en mm/s.
industrial_robot.setSpeed(200)
# Establecer la velocidad de la articulación en deg/s.
industrial_robot.setSpeedJoints(100.0)

# ---------------------------------------------------------------------------- #
# FUNCIONES DESARROLLADAS

def stop_all():
    """
    Esta función sirve para detener el cobot, el
    robot industrial y las cintas en la simulación. 
    """
    colaborative_robot.Stop()

    industrial_robot.Stop()

    agv_caja_s_y.Stop()
    agv_caja_s_z.Stop()

    agv_caja_m_y.Stop()
    agv_caja_m_z.Stop()

    agv_caja_l_y.Stop()
    agv_caja_l_z.Stop()

    ### end def stop_all() ###

def assemble_box(msg):
    """
    Esta función analiza el mensaje recibido (en concreto la información sobre
    el tipo de caja) y ejecuta el subprograma de Montaje de Caja + Transporte
    correspondiente.
    """

    # El primer paso es parsear msg:
    msg_dict = json_loads((msg.payload).decode('UTF-8'))

    prog = RDK.Item('Montaje de Caja ' + msg_dict["tipo_caja"] + ' + Transporte', ITEM_TYPE_PROGRAM)
    prog.RunCode()

    # Debido a la implementación en RoboDK, no se espera a que finalice el
    # programa de Transporte, mientras que con la instrucción 'WaitFinished',
    # se espera a que termine el programa de Montaje de Caja.
    prog.WaitFinished()

    ### end def assemble_box() ###

def copy_object(object_to_copy_name, new_object_name):
    """
    Con esta función se duplica un objeto existente en la estación
    'object_to_copy_name', puede ser una caja o un dispositivo de las
    estanterías. El objeto copiado se guarda en la misma posición que
    el original con el nombre 'new_object_name'.
    """

    if 'Caja' in object_to_copy_name:
        ref_frame = RDK.Item(object_to_copy_name + ' - AGV')

    elif ('tablet' in object_to_copy_name) or ('reloj' in object_to_copy_name):
        ref_frame = RDK.Item('Estantería 01')

    else:
        ref_frame = RDK.Item('Estantería 02')

    obj = RDK.Item(object_to_copy_name, ITEM_TYPE_OBJECT)
    RDK.Copy(obj)
    new_obj = RDK.Paste()
    new_obj.setPose(obj.Pose())
    new_obj.setName(new_object_name)
    new_obj.setParent(ref_frame)
    new_obj.setVisible(True, False) # Mostrar el objeto pero no el marco de referencia.

    ### end def copy_object() ###

def pick_dispositivo(id_dispositivo):
    """
    Esta función ejecuta el pick del dispositivo indicado 'id_dispositivo'.
    """
    dispositivo_aux = RDK.Item(id_dispositivo + '_aux')

    # Establecer herramienta de la herramienta industria:
    industrial_robot.setTool(industrial_tool)

    # Establecer sistema de referencia del eje lineal:
    linear_rail_robot.setPoseFrame(RDK.Item('Brooks PreciseFlex Linear Rail 1500 Base'))

    #########################################
    ##  FASE 1. APROXIMACIÓN PARA EL PICK  ##
    #########################################

    # ---[ ESTANTERÍA 1 ]--------------------------------------------------------------- #

    if 'tablet' in id_dispositivo:
        if '01' in id_dispositivo:
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger tablet_X_01'), True)

        elif '02' in id_dispositivo:
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger tablet_X_02'), True)

        else: # 'tablet'
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger tablet_X_funda'), True)

    elif 'reloj' in id_dispositivo:
        if 'A' in id_dispositivo:
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger reloj_A_0X'), True)

        else: # 'B'
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger reloj_B_0X'), True)

        industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))
        industrial_robot.MoveJ(RDK.Item('Aprox Estantería 1 Balda de Abajo'), True)

    # ---[ ESTANTERÍA 2 ]--------------------------------------------------------------- #

    elif 'telefono' in id_dispositivo:
        if (('A' in id_dispositivo) or ('B' in id_dispositivo) or
            ('C' in id_dispositivo) or ('D' in id_dispositivo)):
            if '01' in id_dispositivo:
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_AD_01'), True)

            elif '02' in id_dispositivo:
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_AD_02'), True)

            elif '03' in id_dispositivo:
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_AD_03'), True)

            else: # 'funda'
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_AD_funda'), True)

        else: # 'E', 'F', 'G' y 'H'
            if '01' in id_dispositivo:
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_EH_01'), True)

            elif '02' in id_dispositivo:
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_EH_02'), True)

            else: # 'funda'
                linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger telefono_EH_funda'), True)

        industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))
        industrial_robot.MoveL(RDK.Item('Aproximación superior Estantería 2'), True)

    elif 'pulsera' in id_dispositivo:
        if 'A' in id_dispositivo:
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger pulsera_A_0X'), True)

        else: # 'B'
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger pulsera_B_0X'), True)

        industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))
        industrial_robot.MoveJ(RDK.Item('Aprox Estantería 2 Balda de Abajo'), True)

    elif 'ereader' in id_dispositivo:
        if 'funda' in id_dispositivo:
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger ereader_X_funda'), True)

            industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))
            industrial_robot.MoveJ(RDK.Item('Aprox Estantería 2 Balda de Abajo'), True)

        else: # 'A' y 'B'
            linear_rail_robot.MoveJ(RDK.Item('Aproximación para coger ereader_X_0X'), True)

            industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))
            industrial_robot.MoveL(RDK.Item('Aproximación superior Estantería 2'), True)

    else:
        pass

    ####################################
    ##  FASE 2. PICK DEL DISPOSITIVO  ##
    ####################################

    # Establecer sistema de referencia del 'id_dispositivo':
    industrial_robot.setPoseFrame(RDK.Item(id_dispositivo, ITEM_TYPE_FRAME))

    if 'tablet' in id_dispositivo:
        # MoveL (Pre Pick Horizontal):
        industrial_robot.MoveL(RDK.Item('Pre Pick Horizontal', ITEM_TYPE_TARGET), True)
        # MoveL (Pick Horizontal):
        industrial_robot.MoveL(RDK.Item('Pick Horizontal', ITEM_TYPE_TARGET), True)

        # Establecer 'ventosa_industrial_01, 02 y 03' = 1:
        RDK.setParam('ventosa_industrial_01', 1)
        RDK.setParam('ventosa_industrial_02', 1)
        RDK.setParam('ventosa_industrial_03', 1)

    else:
        # MoveL (Pre Pick Vertical):
        industrial_robot.MoveL(RDK.Item('Pre Pick Vertical', ITEM_TYPE_TARGET), True)
        # MoveL (Pick Horizontal):
        industrial_robot.MoveL(RDK.Item('Pick Vertical', ITEM_TYPE_TARGET), True)

        # Establecer 'ventosa_industrial_01' = 1:
        RDK.setParam('ventosa_industrial_01', 1)

        if ('reloj' not in id_dispositivo) and ('pulsera' not in id_dispositivo):
            # Establecer 'ventosa_industrial_02' = 1:
            RDK.setParam('ventosa_industrial_02', 1)

    # Adjuntar el dispositivo a la herramienta industrial:
    # industrial_tool.AttachClosest('', 1.0, list) <- no hace el attach, no sé por qué :-(
    prog = RDK.Item('attach_item', ITEM_TYPE_PROGRAM) # solución al problema de arriba :P
    prog.RunCode()
    prog.WaitFinished()

    # Ocultar dispositivo auxiliar de la estantería:
    dispositivo_aux.setVisible(False, False)

    # Pausa de 1.0 seg.:
    industrial_robot.Pause(1000.0)

    if 'tablet' in id_dispositivo:
        # MoveL (Post Pick Horizontal 1):
        industrial_robot.MoveL(RDK.Item('Post Pick Horizontal 1', ITEM_TYPE_TARGET), True)
        # MoveL (Post Pick Horizontal 2):
        industrial_robot.MoveL(RDK.Item('Post Pick Horizontal 2', ITEM_TYPE_TARGET), True)
    else:
        # MoveL (Post Pick Vertical 1):
        industrial_robot.MoveL(RDK.Item('Post Pick Vertical 1', ITEM_TYPE_TARGET), True)
        # MoveL (Post Pick Vertical 2):
        industrial_robot.MoveL(RDK.Item('Post Pick Vertical 2', ITEM_TYPE_TARGET), True)

    #############################################
    ##  FASE 3. IR A LA POSICIÓN DE PRE-PLACE  ##
    #############################################

    # Establecer sistema de referencia del robot industrial:
    industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))

    # MoveL (Objetivo base del Denso):
    if 'reloj' in id_dispositivo:
        industrial_robot.MoveJ(RDK.Item('Aprox Estantería 1 Balda de Abajo'), True)

    if 'pulsera' in id_dispositivo:
        industrial_robot.MoveJ(RDK.Item('Aprox Estantería 2 Balda de Abajo'), True)

    if ('ereader' in id_dispositivo) and ('funda' in id_dispositivo):
        industrial_robot.MoveJ(RDK.Item('Aprox Estantería 2 Balda de Abajo'), True)

    if (('telefono' in id_dispositivo) or
        (('ereader' in id_dispositivo) and ('funda' not in id_dispositivo))):
        industrial_robot.MoveL(RDK.Item('Aproximación superior Estantería 2'), True)
        industrial_robot.MoveL(RDK.Item('Objetivo base del Denso'), True)
    else:
        industrial_robot.MoveJ(RDK.Item('Objetivo base del Denso'), True)

    # Mostrar dispositivo auxiliar de la estantería:
    dispositivo_aux.setVisible(True, False)

    # Establecer sistema de referencia del eje lineal:
    linear_rail_robot.setPoseFrame(RDK.Item('Brooks PreciseFlex Linear Rail 1500 Base'))

    # MoveJ (Objetivo base del Eje Lineal):
    linear_rail_robot.MoveJ(RDK.Item('Objetivo base del Eje Lineal'), True)

    ### end def pick_dispositivo() ###

def fill_box(msg):
    """
    Esta función analiza el mensaje recibido (tipo de caja, cantidad de
    dispositivos de la caja, los tipos de dispositivo y su posicion de place)
    y ejecuta el Pick & Place de cada dispositivo dentro de la caja.

    Subprogramas para (fill_box): (copy_object) y (pick_dispositivo).
    """

    while RDK.getParam("sensor_final_cinta") == "no hay caja":
        pass

    # Establecer 'estado_industrial' = "ocupado":
    RDK.setParam("estado_industrial", "ocupado")

    # Establecer 'caja_llena' = "caja en proceso":
    RDK.setParam("caja_llena", "caja en proceso")

    # El primer paso es parsear msg:
    msg_dict = json_loads((msg.payload).decode('UTF-8'))

    num_dispositivos = msg_dict["num_dispositivos"]
    tipo_caja = msg_dict["tipo_caja"]

    copy_object('Caja ' + tipo_caja, 'Caja ' + tipo_caja + ' - aux')
    item_caja = RDK.Item('Caja ' + tipo_caja + ' - aux', ITEM_TYPE_OBJECT)
    item_caja.setVisible(False, False)

    for i in range(1, num_dispositivos + 1):

        iterator = "item_" + str(i)

        # Conversión de str a vector de posiciones:
        pose_array = np.fromstring(msg_dict[iterator]["posicion_place"], sep=',')

        place_pose = xyzrpw_2_pose(pose_array)

        pre_place_array = pose_array
        pre_place_array[2] = 500.000
        pre_place_01 = xyzrpw_2_pose(pre_place_array)


        if msg_dict[iterator]["dispositivo"] == 'telefono_A_01':
            msg_dict[iterator]["dispositivo"] = "telefono_A_02"

        # ---[ FASE 1: Pick del dispositivo ]------------------------------------ #
        copy_object(msg_dict[iterator]["dispositivo"] + '_aux', 'item_' + str(i))

        pick_dispositivo(msg_dict[iterator]["dispositivo"])

        # ---[ FASE 2: Place del dispositivo ]----------------------------------- #
        industrial_robot.setPoseTool(industrial_tool)
        industrial_robot.setPoseFrame(RDK.Item('Caja ' + tipo_caja +' - AGV'))
        industrial_robot.MoveJ(pre_place_01, True)
        industrial_robot.MoveL(place_pose, True)

        industrial_tool.DetachAll(item_caja)

        # Establecer 'ventosa_industrial_01, 02 y 03' = 0:
        RDK.setParam('ventosa_industrial_01', 0)
        RDK.setParam('ventosa_industrial_02', 0)
        RDK.setParam('ventosa_industrial_03', 0)

        industrial_robot.MoveL(pre_place_01, True)

        industrial_robot.setPoseFrame(RDK.Item('Denso VS-6577G-B Base'))
        industrial_robot.MoveJ(RDK.Item('Objetivo base del Denso'), True)

    # Establecer 'estado_industrial' = "libre":
    RDK.setParam("estado_industrial", "libre")

    # Establecer 'caja_llena' = "nueva caja llena":
    RDK.setParam("caja_llena", "nueva caja llena")

    ### end def fill_box() ###

# end of file #
