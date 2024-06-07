/**
 * @file   g_tareas.ino
 * @brief  Definición e implementación de tareas de gestión de decisiones.
 */

// Creando un manejadores de las tareas.
//
TaskHandle_t retenedor_camara_task_handle;
TaskHandle_t gestion_cintas_task_handle;
TaskHandle_t promedio_tiempos_llenado_handle;

/******************************************************************************/
/*!
 * @brief  Tarea que se encarga analizar la lectura del QR. De esta manera se
 *         consigue retener la caja correctamente dependiendo de su tamaño y se
 *         genera/envía la orden para el robot industrial.
 * @param  void * parameter -> puntero a la estructura 'estado_cinta'
 * @return void
 */
void retenedor_camara_task(void * parameter)
{
    gestion_cintas_t * gestion_cintas = (gestion_cintas_t *) parameter;
    JsonDocument contenido_BBDD_id_caja, doc;
    char * id_caja;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;)
    {
        if (gestion_cintas->lectura_QR != "esperando_lectura")
        {
            id_caja = gestion_cintas->lectura_QR;
            gestion_cintas->lectura_QR = "esperando_lectura";

            // Función sin implementar, leer aviso de abajo.
            consulta_BBDD_y_generar_orden(id_caja, &contenido_BBDD_id_caja);

            /*-----------------------------------------------------+      
             |                 AVISO IMPORTANTE                    |
             +-----------------------------------------------------+
             |   Una vez se ha adquirido el id_caja, el siguiente  |
             |  paso sería consultar la base de datos mediante     |
             |  esta ESP32.                                        |
             |                                                     |
             |   Con la consulta se conseguirían dos objetivos:    |
             |    - Qué tipo de caja se va a retener.              |
             |    - El contenido de la caja (para generar la orden |
             |      del industrial en formato JSON).               |
             |                                                     |
             |   Con algo más de tiempo podríamos conseguir esta   |
             |  implementación, pero en ninguna asignatura se nos  |
             |  ha proporcionado las herramientas necesarias para  |
             |  realizar consultas a una base de datos mediante    |
             |  librerías o extensiones para otros lenguajes como  |
             |  el c++.                                            |
             |                                                     |
             |   Se va a suponer que el siguiente documento JSON   |
             |  contiene toda la información necesaria para ajus-  |
             |  tarse a una orden para el robot industrial, tal y  |
             |  como ha sido definida en la documentación.         |
             |                                                     |
             |   Documento JSON: "contenido_BBDD_id_caja"          |
             +-----------------------------------------------------*/

            if (contenido_BBDD_id_caja["tipo_caja"] == "S")
            {
                // Se guarda el tipo de caja que se va a llenar.
                gestion_cintas->caja_actual_en_llenado == CAJA_S;

                doc["accion"] = "retener_s";

                // Serializar el JSON en un String.
                String msg_json;
                serializeJson(doc, msg_json);

                // Enviar mensaje por el topic indicado.
                enviarMensajePorTopic(ACCION_RETENEDOR_TOPIC, msg_json);
            }
            else // ((contenido_BBDD_id_caja["tipo_caja"] == "M") ||
                 //  (contenido_BBDD_id_caja["tipo_caja"] == "L"))
            {
                if (contenido_BBDD_id_caja["tipo_caja"] == "M")
                {
                    // Se guarda el tipo de caja que se va a llenar.
                    gestion_cintas->caja_actual_en_llenado == CAJA_M;
                }
                else // (contenido_BBDD_id_caja["tipo_caja"] == "L")
                {
                    // Se guarda el tipo de caja que se va a llenar.
                    gestion_cintas->caja_actual_en_llenado == CAJA_L;
                }

                doc["accion"] = "retener_m_l";

                // Serializar el JSON en un String.
                String msg_json;
                serializeJson(doc, msg_json);

                // Enviar mensaje por el topic indicado.
                enviarMensajePorTopic(ACCION_RETENEDOR_TOPIC, msg_json);
            }

            /* ENVIAR ORDEN AL ROBOT INDUSTRIAL */

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(contenido_BBDD_id_caja, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(ORDEN_INDUSTRIAL_TOPIC, msg_json);

            ///////////////////////////////////////////////////////////////////
            /// Una vez ha sido enviada la orden, se toma el tiempo actual
            /// como el momento del inicio del llenado de la caja.
            ///
            gestion_cintas->tiempo_inicio_llenado = millis();
            ///////////////////////////////////////////////////////////////////

        }
        else // (gestion_cintas->lectura_QR == "esperando_lectura")
        {
            // Si se indica que se está esperando la lectura es porque todavía
            // no ha llegado una nueva lectura a través del topic LECTOR_QR_TOPIC.
        }

        // Espera absoluta de 1000 ms.
        vTaskDelayUntil(&xLastWakeTime, (1000 / portTICK_PERIOD_MS));
    }

    traceln("retenedor_camara_task() - Finalizando tarea...");
    vTaskDelete(NULL);

}   /* retenedor_camara_task() */

/******************************************************************************/
/*!
 * @brief  Tarea que se encarga de la gestión de las cintas y del retenedor.
 * @param  void * parameter -> puntero a la estructura 'estado_cinta'
 * @return void
 */
void gestion_cintas_task(void * parameter)
{
    gestion_cintas_t * gestion_cintas = (gestion_cintas_t *) parameter;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    // Crear un documento JSON.
    JsonDocument doc;

    unsigned long now, last = 0.0;
    bool cinta_02_activa = false;

    // Es le tiempo que está en marcha la cinta 2
    // para llevarse una caja a la siguiente fase.
    unsigned long cinta_02_intervalo = 5000;
    
    for (;;)
    {
        now = millis();

        if (((now - last) > cinta_02_intervalo) && (cinta_02_activa = true))
        {
            doc["accion"] = "detener";

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(ACCION_CINTA_02_TOPIC, msg_json);
        }

        if ((gestion_cintas->sensor_inicio_cinta == NO_HAY_CAJA) &&
            (gestion_cintas->sensor_final_cinta  == NO_HAY_CAJA) &&
            (gestion_cintas->caja_en_la_cinta_1  == 0))
        {
            // Detener la cinta 1 si no hay ninguna caja en la cinta.

            doc["accion"] = "detener";

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(ACCION_CINTA_01_TOPIC, msg_json);
        }
        else if ((gestion_cintas->sensor_inicio_cinta == HAY_CAJA) &&
                 (gestion_cintas->sensor_final_cinta  == NO_HAY_CAJA))
        {
            // Poner en marcha la cinta 1.

            portENTER_CRITICAL(&(gestion_cintas->taskMux));
            gestion_cintas->caja_en_la_cinta_1 += 1;
            portEXIT_CRITICAL(&(gestion_cintas->taskMux));
            
            doc["accion"] = "en marcha";

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(ACCION_CINTA_01_TOPIC, msg_json);
        }
        else if ((gestion_cintas->sensor_inicio_cinta == NO_HAY_CAJA) &&
                 (gestion_cintas->sensor_final_cinta  == HAY_CAJA))
        {
            // Detener la cinta 1 en cualquier circumstancia.

            doc["accion"] = "detener";

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por el topic indicado.
            enviarMensajePorTopic(ACCION_CINTA_01_TOPIC, msg_json);
        }
        else
        {
            ;
        }

        if (gestion_cintas->nueva_caja_llena == true)
        {
            gestion_cintas->nueva_caja_llena == false;

            ///////////////////////////////////////////////////////////////////
            /// Una nueva caja ha sido llenada, se obtiene el tiempo actual,
            /// posteriormente se realiza el cálculo del tiempo de llenado
            /// (tiempo_final - tiempo_inicial) y se envía a la cola de tiempos
            /// de llenado de la caja correspondiente.
            ///
            unsigned long tiempo_llenado, tiempo_fin_llenado;

            tiempo_fin_llenado = millis();
            tiempo_llenado = tiempo_fin_llenado - (gestion_cintas->tiempo_inicio_llenado);

            if (gestion_cintas->caja_actual_en_llenado == CAJA_S)
            {
                if ((gestion_cintas->buffer_tiempos_de_llenado_caja_s).put_item(tiempo_llenado) == buffer_lleno)
                {
                    infoln("gestion_cintas_task() - buffer_tiempos_de_llenado_caja_s está lleno");
                }
            }
            else if (gestion_cintas->caja_actual_en_llenado == CAJA_M)
            {
                if ((gestion_cintas->buffer_tiempos_de_llenado_caja_m).put_item(tiempo_llenado) == buffer_lleno)
                {
                    infoln("gestion_cintas_task() - buffer_tiempos_de_llenado_caja_m está lleno");
                }
            }
            else // (gestion_cintas->caja_actual_en_llenado == CAJA_L)
            {
                if ((gestion_cintas->buffer_tiempos_de_llenado_caja_l).put_item(tiempo_llenado) == buffer_lleno)
                {
                    infoln("gestion_cintas_task() - buffer_tiempos_de_llenado_caja_l está lleno");
                }
            }

            gestion_cintas->caja_actual_en_llenado = NINGUNA;
            ///////////////////////////////////////////////////////////////////

            // Dejar de retenr la caja con el retenedor.
            doc["accion"] = "soltar";

            // Serializar el JSON en un String.
            String msg_json;
            serializeJson(doc, msg_json);

            // Enviar mensaje por los topics indicados.
            enviarMensajePorTopic(ACCION_RETENEDOR_TOPIC, msg_json);

            // Poner en marcha la cinta 1 y la cinta 2.
            doc["accion"] = "en marcha";

            // Serializar el JSON en un String.
            msg_json = "";
            serializeJson(doc, msg_json);

            // Enviar mensaje por los topics indicados.
            enviarMensajePorTopic(ACCION_CINTA_01_TOPIC, msg_json);
            enviarMensajePorTopic(ACCION_CINTA_02_TOPIC, msg_json);
            cinta_02_activa = true;

            portENTER_CRITICAL(&(gestion_cintas->taskMux));
            gestion_cintas->caja_en_la_cinta_1 -= 1;
            portEXIT_CRITICAL(&(gestion_cintas->taskMux));
        }

        // Espera absoluta de 250 ms.
        vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
    }

    traceln("gestion_cintas_task() - Finalizando tarea...");
    vTaskDelete(NULL);

}   /* gestion_cintas_task() */

/******************************************************************************/
/*!
 * @brief  Esta tarea lee los valores de los siguientes buffers:
 *           - 'buffer_tiempos_de_llenado_caja_s'
 *           - 'buffer_tiempos_de_llenado_caja_m'
 *           - 'buffer_tiempos_de_llenado_caja_l'
 *
 *         Cuando alguno de ellos está lleno, calcula el promedio de
 *         todos los valores del buffer y publica el resultado vía MQTT.
 * @param  void * parameter -> puntero a la estructura 'estado_cinta'
 * @return void
 */
void promedio_tiempos_llenado(void * parameter)
{
    gestion_cintas_t * gestion_cintas = (gestion_cintas_t *) parameter;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    unsigned long aux, promedio;
    // Crear un documento JSON.
    JsonDocument doc;
    doc["promedio_caja_s"]["unidad"] = "milsegundos (tipo unsigned long)";

    for (;;)
    {
        if ((gestion_cintas->buffer_tiempos_de_llenado_caja_s).isFull() == true)
        {
            for (int i = 0; i < ((gestion_cintas->buffer_tiempos_de_llenado_caja_s).size()); i++)
            {
                (gestion_cintas->buffer_tiempos_de_llenado_caja_s).get_item(&aux);
                promedio += aux;
            }

            promedio = promedio / (unsigned long)((gestion_cintas->buffer_tiempos_de_llenado_caja_s).size());
            doc["promedio_caja_s"]["valor"]  = promedio;
        }
        else if ((gestion_cintas->buffer_tiempos_de_llenado_caja_m).isFull() == true)
        {
            for (int i = 0; i < ((gestion_cintas->buffer_tiempos_de_llenado_caja_m).size()); i++)
            {
                (gestion_cintas->buffer_tiempos_de_llenado_caja_m).get_item(&aux);
                promedio += aux;
            }

            promedio = promedio / (unsigned long)((gestion_cintas->buffer_tiempos_de_llenado_caja_m).size());
            doc["promedio_caja_m"]["valor"]  = promedio;
        }
        else if ((gestion_cintas->buffer_tiempos_de_llenado_caja_l).isFull() == true)
        {
            for (int i = 0; i < ((gestion_cintas->buffer_tiempos_de_llenado_caja_l).size()); i++)
            {
                (gestion_cintas->buffer_tiempos_de_llenado_caja_l).get_item(&aux);
                promedio += aux;
            }

            promedio = promedio / (unsigned long)((gestion_cintas->buffer_tiempos_de_llenado_caja_l).size());
            doc["promedio_caja_l"]["valor"]  = promedio;
        }
        else
        {
            ;
        }

        // Serializar el JSON en un String.
        String msg_json;
        serializeJson(doc, msg_json);

        // Enviar mensaje por los topics indicados.
        enviarMensajePorTopic(PROMEDIO_TIEMPOS_TOPIC, msg_json);

        promedio = 0.0;

        // Espera absoluta de 250 ms.
        vTaskDelayUntil(&xLastWakeTime, (250 / portTICK_PERIOD_MS));
    }

    traceln("promedio_tiempos_llenado() - Finalizando tarea...");
    vTaskDelete(NULL);

}   /* promedio_tiempos_llenado() */

/*** end of file ***/
