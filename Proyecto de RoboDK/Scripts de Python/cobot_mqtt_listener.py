"""
Este script atiende a las ordenes enviadas por MQTT dirigidas al cobot.
"""

# ---------------------------------------------------------------------------- #
# IMPORTACIONES NECESARIAS

import paho.mqtt.client as mqtt
from functions import assemble_box

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

ORDEN_COBOT_TOPIC       = "giirob/PR2/A04/escenario/montaje_cajas/robot/orden"
ESTADO_COBOT_TOPIC      = "giirob/PR2/A04/escenario/montaje_cajas/robot/estado"

# ---------------------------------------------------------------------------- #
# FUNCIONES DESARROLLADAS

def on_message(mqttc, obj, msg):
    """
    Cuando llega un mensaje a través del topic ORDEN_COBOT_TOPIC, se llama a la
    función assemble_box(), pasándole como parámetro msg sin parsear, ya que el
    mensaje se analizará dentro de esa función. 
    """

    if msg.topic == ORDEN_COBOT_TOPIC:
        # Suponiendo que el mensaje es correcto ...
        assemble_box(msg)

        msg_str = "{\n  \"estado\": \"libre\"\n}"
        mqttc.publish(ESTADO_COBOT_TOPIC, msg_str)

    else:
        pass

    ### end def on_message() ###

# ---------------------------------------------------------------------------- #
# CREAR CLIENTE Y ESTABLECER CONEXIÓN MQTT (BUCLE)

cobot_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
cobot_client.on_message = on_message

cobot_client.username_pw_set(username = USERNAME, password = PASSWORD)
cobot_client.connect(BROKER, PORT, 60)

cobot_client.subscribe(ORDEN_COBOT_TOPIC, 0)

cobot_client.loop_forever()

# end of file #
