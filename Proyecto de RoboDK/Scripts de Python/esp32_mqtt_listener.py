"""
Este script ajusta el valor de ciertas variables de RoboDK para que sean
acordes con la realidad, para ello se aprovechan los mensajes publicados
por la ESP32-01 y la ESP32-02 (sensores y actuadores).

Además cuando se envía una orden al robot industrial o al cobot, se publica
el estado del robot a "ocupado". Cuando los robots acaben las ordenes
publicaran automáticamente el estado del robot a "libre".
"""

# ---------------------------------------------------------------------------- #
# IMPORTACIONES NECESARIAS

from json.__init__ import loads as json_loads
import paho.mqtt.client as mqtt
from robodk.robolink import Robolink
from functions import stop_all

# Cualquier interacción con RoboDK debe realizarse a través de RDK:
RDK = Robolink()

# ---------------------------------------------------------------------------- #
# PARÁMETROS Y TOPICS PARA LA CONEXIÓN CON MQTT

BROKER   = 'broker.emqx.io'
PORT     =  1883
USERNAME = 'emqx'
PASSWORD = 'public'

# BROKER   = "mqtt.dsic.upv.es"
# PORT     =  1883
# USERNAME = "giirob"
# PASSWORD = "UPV2024"

# TOPICS PARA LOS SENSORES
SENSOR_INICIO_CINTA_TOPIC  = "giirob/PR2/A04/escenario/cinta/01/sensores/inicio_cinta"
SENSOR_FINAL_CINTA_TOPIC   = "giirob/PR2/A04/escenario/cinta/01/sensores/final_cinta"
PULSADOR_EMERGENCIA_TOPIC  = "giirob/PR2/A04/escenario/pulsador_emergencia/estado"

# TOPICS PARA LOS ACTUADORES
ACCION_RETENEDOR_TOPIC     = "giirob/PR2/A04/escenario/cinta/01/retenedor/accion"
ACCION_CINTA_01_TOPIC      = "giirob/PR2/A04/escenario/cinta/01/cinta/accion"
ACCION_CINTA_02_TOPIC      = "giirob/PR2/A04/escenario/cinta/02/cinta/accion"

# TOPICS PARA LOS ROBOTS
ORDEN_COBOT_TOPIC          = "giirob/PR2/A04/escenario/montaje_cajas/robot/orden"
ESTADO_COBOT_TOPIC         = "giirob/PR2/A04/escenario/montaje_cajas/robot/estado"

ORDEN_INDUSTRIAL_TOPIC     = "giirob/PR2/A04/escenario/estanterias/robot/orden"
ESTADO_INDUSTRIAL_TOPIC    = "giirob/PR2/A04/escenario/estanterias/robot/estado"
INDUSTRIAL_AVISO_TOPIC     = "giirob/PR2/A04/escenario/estanterias/robot/caja_llena"

# ---------------------------------------------------------------------------- #
def on_message(mqttc, obj, msg):
    """
    Función que analiza el topic recibido y realiza la acción oportuna.
    """

    # El primer paso es parsear msg:
    msg_dict = json_loads((msg.payload).decode('UTF-8'))

    if msg.topic == SENSOR_INICIO_CINTA_TOPIC:
        # Cuando la ESP32-01 notifica que hay una caja al inicio de la cinta,
        # se ajusta la variable 'sensor_inicio_cinta' de RoboDK para que sea
        # acorde con la realidad:

        if msg_dict["presencia"] == "hay caja":
            RDK.setParam("sensor_inicio_cinta", "hay caja")
        else:
            RDK.setParam("sensor_inicio_cinta", "no hay caja")

    elif msg.topic == SENSOR_FINAL_CINTA_TOPIC:
        # Cuando la ESP32-01 notifica que hay una caja al final de la cinta,
        # se ajusta la variable 'sensor_final_cinta' de RoboDK para que sea
        # acorde con la realidad:

        if msg_dict["presencia"] == "hay caja":
            RDK.setParam("sensor_final_cinta", "hay caja")
        else:
            RDK.setParam("sensor_final_cinta", "no hay caja")

    elif msg.topic == ACCION_RETENEDOR_TOPIC:
        # Cuando la ESP32-02 acciona o libera el retenedor, se ajusta la variable
        # 'accion_retenedor' de RoboDK para que sea acorde con la realidad:

        if msg_dict["accion"] == "retener":
            RDK.setParam("accion_retenedor", "retener")
        else:
            RDK.setParam("accion_retenedor", "soltar")

    elif msg.topic == ACCION_CINTA_01_TOPIC:
        # Cuando la ESP32-02 pone en marcha o detiene la cinta 1, se ajusta
        # la variable 'accion_cinta_01' de RoboDK para que sea acorde con
        # la realidad:

        if msg_dict["accion"] == "en marcha":
            RDK.setParam("accion_cinta_01", "en marcha")
        else:
            RDK.setParam("accion_cinta_01", "detener")

    elif msg.topic == ACCION_CINTA_02_TOPIC:
        # Cuando la ESP32-02 pone en marcha o detiene la cinta 2, se ajusta
        # la variable 'accion_cinta_02' de RoboDK para que sea acorde con
        # la realidad:

        if msg_dict["accion"] == "en marcha":
            RDK.setParam("accion_cinta_02", "en marcha")
        else:
            RDK.setParam("accion_cinta_02", "detener")

    elif msg.topic == ORDEN_COBOT_TOPIC:
        # Cuando el cobot recibe una orden, se publica
        # su nuevo estado, que pasa a ser "ocupado":

        msg_str = "{\n  \"estado\": \"ocupado\"\n}"
        mqttc.publish(ESTADO_COBOT_TOPIC, msg_str)

    elif msg.topic == ORDEN_INDUSTRIAL_TOPIC:
        # Cuando el robot industrial recibe una orden, se
        # publica su nuevo estado, que pasa a ser "ocupado":

        msg_str = "{\n  \"estado\": \"ocupado\"\n}"
        mqttc.publish(ESTADO_INDUSTRIAL_TOPIC, msg_str)

    elif msg.topic == INDUSTRIAL_AVISO_TOPIC:
        # Cuando el robot industrial llena una caja y lo notifica,
        # se ajusta la variable 'caja_llena' de RoboDK:

        if msg_dict["caja_llena"] == "nueva caja llena":
            RDK.setParam("caja_llena", "nueva caja llena")

    elif msg.topic == PULSADOR_EMERGENCIA_TOPIC:
        # Cuando el pulsador de emergencia sea presionado, se
        # detienen todos los elementos de la simulación de RoboDK:

        if msg_dict["pulsador"] == "presionado":
            stop_all()

    else:
        pass

    ### end def on_message() ###

# ---------------------------------------------------------------------------- #
# CREAR CLIENTE Y ESTABLECER CONEXIÓN MQTT (BUCLE)

esp32_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
esp32_client.on_message = on_message

esp32_client.username_pw_set(username = USERNAME, password = PASSWORD)
esp32_client.connect(BROKER, PORT, 60)

# Para que el cobot sepa si hay una
# caja ya montada al inicio de la cinta 1.
esp32_client.subscribe(SENSOR_INICIO_CINTA_TOPIC, 0)

# Para que el robot industrial sepa si hay una
# caja en la zona de place lista para ser llenada.
esp32_client.subscribe(SENSOR_FINAL_CINTA_TOPIC, 0)

# Si el pulsador de emergencia se
# activa se detiene el robot indistrial.
esp32_client.subscribe(PULSADOR_EMERGENCIA_TOPIC, 0)

# Otros topics:
esp32_client.subscribe(ACCION_RETENEDOR_TOPIC, 0)
esp32_client.subscribe(ACCION_CINTA_01_TOPIC, 0)
esp32_client.subscribe(ACCION_CINTA_02_TOPIC, 0)

esp32_client.subscribe(ORDEN_COBOT_TOPIC, 0)
esp32_client.subscribe(ORDEN_INDUSTRIAL_TOPIC, 0)
esp32_client.subscribe(INDUSTRIAL_AVISO_TOPIC, 0)

esp32_client.loop_forever()

# end of file #
