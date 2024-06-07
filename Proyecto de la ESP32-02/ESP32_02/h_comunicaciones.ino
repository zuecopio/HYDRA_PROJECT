/**
 * @file   h_comunicaciones.ino
 * @brief  Definición e implementación de las funciones de comunicación.
 */

/******************************************************************************/
/*!
 * @brief  Funcion de suscripción los a topics.
 * @param  void
 * @return void
 */
void suscribirseATopics(void)
{
    mqtt_subscribe(PULSADOR_EMERGENCIA_TOPIC);
    mqtt_subscribe(SENSOR_INICIO_CINTA_TOPIC);
    mqtt_subscribe(SENSOR_FINAL_CINTA_TOPIC);
    mqtt_subscribe(LECTOR_QR_TOPIC);
    mqtt_subscribe(INDUSTRIAL_AVISO_TOPIC);
    
}   /* suscribirseATopics() */

/******************************************************************************/
/*!
 * @brief  Función que gestiona los mensajes
 *         recibidos por los diferentes topics.
 * @param  topic            (char *)
 * @param  incomingMessage  (String)
 * @return void
 */
void alRecibirMensajePorTopic(char * topic, String incomingMessage)
{
    // Crear un documento JSON.
    JsonDocument doc;
    // Analiza la entrada JSON.
    DeserializationError err = deserializeJson(doc, incomingMessage);
    // ¿El análisis fue exitoso?
    if (err)
    {
        warn(F("deserializeJson() returned ")); warnln(err.f_str());
        return;
    }

    // Si se recibe un mensaje del topic...
    if (strcmp(topic, PULSADOR_EMERGENCIA_TOPIC) == 0)
    {
        if (doc["pulsador"] == "presionado")
        {
            /*-----------------------------------------------------+      
             |                 AVISO IMPORTANTE                    |
             +-----------------------------------------------------+
             |   En una implementación real, la sensorización y la |
             |  gestión del pulsador de emergencia se reservaría a |
             |  una sola ESP32. No es buena idea depender de vias  |
             |  de comunicación indirectas (MQTT) para elementos   |
             |  criticos de la estación, ya que si falla la        |
             |  conexión a internet, la parada de emergencia no se |
             |  realizaría a tiempo.                               |
             |                                                     |
             |   Dicho lo anterior, en esta solució se reaiza de   |
             |  la manera menos indicada para así seguir con el    |
             |  modelo de dispositivo sensor/actuador y dispositi- |
             |  vo controlador.                                    |
             |                                                     |
             |   Al accionarse el pulsador de emergencia, desenca- |
             |  denaría un corte del suministro eléctrico de las   |
             |  cintas 1 y 2 y de los robots de la estación.       |
             |                                                     |
             |   La parada de emergencia en la simulación de       |
             |  RoboDK ya está implementada, para reflejar la      |
             |  emergencia también en el mundo real se detendrán   |
             |  los motores de las cintas 1 y 2 (se apagarán los   |
             |  LEDs conectados a los pines 37 y 38 de la          |
             |  ESP32-01).                                         |
             +-----------------------------------------------------*/

            // Crear otro documento JSON.
            JsonDocument doc_02;
            doc["accion"] = "detener";

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc_02, msg_json);

            // Enviar mensaje por los topics indicados.
            enviarMensajePorTopic(ACCION_CINTA_01_TOPIC, msg_json);
            enviarMensajePorTopic(ACCION_CINTA_02_TOPIC, msg_json);
        }
    }
    else if (strcmp(topic, SENSOR_INICIO_CINTA_TOPIC) == 0)
    {
        if (doc["presencia"] == "hay caja")
        {
            gestion_cintas.sensor_inicio_cinta = HAY_CAJA;
        }
        else // (doc["presencia"] == "no hay caja")
        {
            gestion_cintas.sensor_inicio_cinta = NO_HAY_CAJA;
        }        
    }
    else if (strcmp(topic, SENSOR_FINAL_CINTA_TOPIC) == 0)
    {
        if (doc["presencia"] == "hay caja")
        {
            gestion_cintas.sensor_final_cinta = HAY_CAJA;
        }
        else // (doc["presencia"] == "no hay caja")
        {
            gestion_cintas.sensor_final_cinta = NO_HAY_CAJA;
        }        
    }
    else if (strcmp(topic, LECTOR_QR_TOPIC) == 0)
    {
        // Con la strcpy() se consigue la conversión de 'doc["id_caja"]', de
        // tipo (const char *), a 'gestion_cintas.lectura_QR', de tipo (char *).
        //
        strcpy(gestion_cintas.lectura_QR, doc["id_caja"]);
    }
    else if (strcmp(topic, INDUSTRIAL_AVISO_TOPIC) == 0)
    {
        if (doc["caja_llena"] == "nueva caja llena")
        {
            gestion_cintas.nueva_caja_llena = true;
        }
    }
    else
    {
        ;
    }
    
}   /* alRecibirMensajePorTopic() */

/******************************************************************************/
/*!
 * @brief  Función para enviar un mensaje por un topic.
 * @param  topic            (const char *)
 * @param  outgoingMessage  (String)
 * @return void
 */
void enviarMensajePorTopic(const char * topic, String outgoingMessage)
{
    mqtt_publish(topic, outgoingMessage.c_str());

}   /* enviarMensajePorTopic() */

/*** end of file ***/
