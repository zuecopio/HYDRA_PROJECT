"""
Este script atiende a las ordenes enviadas por MQTT dirigidas al robot industrial.
"""

# ---------------------------------------------------------------------------- #
# IMPORTACIONES NECESARIAS

import paho.mqtt.client as mqtt
from functions import fill_box

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

ORDEN_INDUSTRIAL_TOPIC  = "giirob/PR2/A04/escenario/estanterias/robot/orden"
ESTADO_INDUSTRIAL_TOPIC = "giirob/PR2/A04/escenario/estanterias/robot/estado"

INDUSTRIAL_AVISO_TOPIC  = "giirob/PR2/A04/escenario/estanterias/robot/caja_llena"

# ---------------------------------------------------------------------------- #
# FUNCIONES DESARROLLADAS

def on_message(mqttc, obj, msg):
    """
    Cuando llega un mensaje a través del topic ORDEN_INDUSTRIAL_TOPIC, se
    llama a la fill_box(), pasándole como parámetro msg sin parsear, ya que
    el mensaje se analizará dentro de esa función. 
    """

    if msg.topic == ORDEN_INDUSTRIAL_TOPIC:
        # Suponiendo que el mensaje es correcto ...
        try:
            fill_box(msg)
        except:
            pass

        msg_str = "{\n  \"estado\": \"libre\"\n}"
        mqttc.publish(ESTADO_INDUSTRIAL_TOPIC, msg_str)

        msg_str = "{\n  \"caja_llena\": \"nueva caja llena\"\n}"
        mqttc.publish(INDUSTRIAL_AVISO_TOPIC, msg_str)

    else:
        pass

    ### end def on_message() ###

# ---------------------------------------------------------------------------- #
# CREAR CLIENTE Y ESTABLECER CONEXIÓN MQTT (BUCLE)

industrial_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
industrial_client.on_message = on_message

industrial_client.username_pw_set(username = USERNAME, password = PASSWORD)
industrial_client.connect(BROKER, PORT, 60)

industrial_client.subscribe(ORDEN_INDUSTRIAL_TOPIC, 0)

industrial_client.loop_forever()

# end of file #
