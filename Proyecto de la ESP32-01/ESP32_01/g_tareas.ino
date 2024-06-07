/**
 * @file   g_tareas.ino
 * @brief  Definición e implementación de las tareas de sensorización.
 */

// Creando los manejadores de las tareas.
//
TaskHandle_t sensor_inicio_cinta_task_handle; 
TaskHandle_t sensor_final_cinta_task_handle; 
TaskHandle_t lectorQR_task_handle; 

/******************************************************************************/
/*!
 * @brief  Tarea que se encarga de sensorizar el inicio de la cinta 1.
 * @param  void * parameter -> puntero a la estructura 'estado_cinta'
 * @return void
 */
void sensor_inicio_cinta_task(void * parameter)
{
    float distance;
    estado_cinta_t * estado_cinta = (estado_cinta_t *) parameter;
    estado_sensor_t estado_anterior = DESCONOCIDO;
    // Crear un documento JSON.
    JsonDocument doc;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    { 
        // Obtener la distancia desde el sensor...
        //
        distance = getUsDistance(inicio_cinta);

        // Si la distancia es inferior a 10 cm, es que
        // hay una caja colocada delante del sensor.
        //
        if (distance < 10.0)
        {
            doc["presencia"] = "hay caja";
            estado_cinta->sensor_inicio_cinta = HAY_CAJA;
        }
        else
        {
            doc["presencia"] = "no hay caja";
            estado_cinta->sensor_inicio_cinta = NO_HAY_CAJA;
        }      
    
        if (estado_cinta->sensor_inicio_cinta != estado_anterior)
        {
            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(SENSOR_INICIO_CINTA_TOPIC, msg_json);

            estado_anterior = estado_cinta->sensor_inicio_cinta;
        }
            
        // Espera absoluta de 250 ms.
        vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
    }

    traceln("sensor_inicio_cinta_task() - Finalizando tarea...");
    vTaskDelete(NULL);

}   /* sensor_inicio_cinta_task() */

/******************************************************************************/
/*!
 * @brief  Tarea que se encarga de sensorizar el final de la cinta 1.
 * @param  void * parameter -> puntero a la estructura 'estado_cinta'
 * @return void
 */
void sensor_final_cinta_task(void * parameter)
{
    float distance;
    estado_cinta_t * estado_cinta = (estado_cinta_t *) parameter;
    estado_sensor_t estado_anterior = DESCONOCIDO;
    // Crear un documento JSON.
    JsonDocument doc;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    { 
        // Obtener la distancia desde el sensor...
        //
        distance = getUsDistance(final_cinta);

        // Si la distancia es inferior a 10 cm, es que
        // hay una caja colocada delante del sensor.
        //
        if (distance < 5.0)
        {
            doc["presencia"] = "hay caja";
            estado_cinta->sensor_final_cinta = HAY_CAJA;
        }
        else
        {
            doc["presencia"] = "no hay caja";
            estado_cinta->sensor_final_cinta = NO_HAY_CAJA;
        }      
    
        if (estado_cinta->sensor_final_cinta != estado_anterior)
        {
            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(SENSOR_FINAL_CINTA_TOPIC, msg_json);

            estado_anterior = estado_cinta->sensor_final_cinta;
        }
            
        // Espera absoluta de 250 ms.
        vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
    }

    traceln("sensor_inicio_cinta_task() - Finalizando tarea...");
    vTaskDelete(NULL);

}   /* sensor_final_cinta_task() */

/******************************************************************************/
/*!
 * @brief  Tarea que se encarga realizar la lectura del QR
 *         cuando hay una caja disponible para ser escaneada.
 * @param  void * parameter -> puntero a la estructura 'estado_cinta'
 * @return void
 */
void lectorQR_task(void * parameter)
{
    estado_cinta_t * estado_cinta = (estado_cinta_t *) parameter;
    String id_caja = "Decodificación FALLIDA", id_caja_anterior = "Decodificación FALLIDA";
    // Crear un documento JSON.
    JsonDocument doc;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;)
    {         
        // Bucle para leer el código QR en tiempo real.
        while ((estado_cinta->sensor_final_cinta == HAY_CAJA))
        {
            id_caja = camera_get_QR();

            if ((id_caja != "Decodificación FALLIDA") && (id_caja != id_caja_anterior))
            {
                doc["id_caja"] = id_caja;
                // Serializar el JSON en un String.
                String msg_json;
                serializeJson(doc, msg_json);

                // Enviar mensaje por el topic indicado.
                enviarMensajePorTopic(LECTOR_QR_TOPIC, msg_json);
                id_caja_anterior = id_caja;
            }
            else
            {
                // No se ha escaneado el QR correctamente, se
                // vuelve mientras haya una caja para escanear.
            }
        }

        // Espera absoluta de 250 ms.
        vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
    }

    traceln("lectorQR_task() - Finalizando tarea...");
    vTaskDelete(NULL);

}   /* lectorQR_task() */

/*** end of file ***/
