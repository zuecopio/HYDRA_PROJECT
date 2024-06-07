/**
 * @file   h_comunicaciones.ino
 * @brief  Definición e implementación de las funciones de comunicación.
 */

/******************************************************************************/
/*!
 * @brief  Funcion de suscripción a los topics.
 * @param  void
 * @return void
 */
void suscribirseATopics(void)
{
    mqtt_subscribe(ACCION_RETENEDOR_TOPIC);
    mqtt_subscribe(ACCION_CINTA_01_TOPIC);
    mqtt_subscribe(ACCION_CINTA_02_TOPIC);
    
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
    if (strcmp(topic, ACCION_RETENEDOR_TOPIC) == 0)
    {
        String accion_retenedor = doc["accion"];
        
        if (accion_retenedor == "retener_s")
        {
            // Acionar retenedor para sujetar una caja tipo S:
            retenedor_set_accion(RETENER_S);
            // Esperar a que la neumática realice su labor:
            delay(1000);
        }
        else if (accion_retenedor == "retener_m_l")
        {
            // Acionar retenedor para sujetar una caja tipo M o L:
            retenedor_set_accion(RETENER_M_L);
            // Esperar a que la neumática realice su labor:
            delay(1000);
        }
        else // accion_retenedor == "soltar"
        {
            // Acionar retenedor para soltar cualquier una caja:
            retenedor_set_accion(SOLTAR);
            // Esperar a que la neumática realice su labor:
            delay(1000);
        }
    }
    else if (strcmp(topic, ACCION_CINTA_01_TOPIC) == 0)
    {
        String accion_cinta_01  = doc["accion"];

        if (accion_cinta_01 == "en marcha")
        {
            digitalWrite(motor_cinta_1, EN_MARCHA);
        }
        else // accion_cinta_01 == "detener"
        {
            digitalWrite(motor_cinta_1, DETENER);
        }
    }
    else if (strcmp(topic, ACCION_CINTA_02_TOPIC) == 0)
    {
        String accion_cinta_02  = doc["accion"];

        if (accion_cinta_02 == "en marcha")
        {
            digitalWrite(motor_cinta_2, EN_MARCHA);
        }
        else // accion_cinta_02 == "detener"
        {
            digitalWrite(motor_cinta_2, DETENER);
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
