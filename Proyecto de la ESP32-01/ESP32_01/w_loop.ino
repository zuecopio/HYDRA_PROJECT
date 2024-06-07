/**
 * @file   w_loop.ino
 * @brief  Definición e implementación de la función de bucle.
 */

/******************************************************************************/
/*!
 * @brief  Cuando el pulsador de emergencia se presiona, esta
 *         función publica vía MQTT un aviso de emergencia.
 * @param  void
 * @return void
 */
void on_loop(void)
{
    if (emergency_button.pressed)
    {
        // Crear un documento JSON.
        JsonDocument doc;
        doc["pulsador"] = "presionado";

        // Serializar el JSON en un String.
        String msg_json;
        serializeJson(doc, msg_json);

        // Enviar mensaje por el topic indicado.
        enviarMensajePorTopic(PULSADOR_EMERGENCIA_TOPIC, msg_json);
        
        infoln("¡El botón de emergencia se ha pulsado!");
        emergency_button.pressed = false;
    }

}   /* on_loop() */

/*** end of file ***/
