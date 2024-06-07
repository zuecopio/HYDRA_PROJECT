/**
 * @file   s_setup.ino
 * @brief  Definición e implementación de las funciones de inicialización.
 */

/******************************************************************************/
/*!
 * @brief  En esta función además se crean las tareas de (g_tareas.ino).
 *           - retenedor_camara_task()
 *           - gestion_cintas_task()
 *           - promedio_tiempos_llenado()
 * @param  void
 * @return void
 */
void on_setup(void)
{
    /* Crear "retenedor_camara_task" usando la función xTaskCreatePinnedToCore() */
    xTaskCreatePinnedToCore(
        retenedor_camara_task,            /* Función de la tarea.        */
        "retenedor_camara_task",          /* Nombre de la tarea.         */
        10000,                            /* Tamaño de pila de la tarea. */
        (void *)&gestion_cintas,          /* Parámetro de la tarea.      */
        1,                                /* Prioridad de la tarea.      */
        &retenedor_camara_task_handle,    /* Manejador de la tarea.      */
        0);                               /* Anclar tarea al núcleo 0.   */
    
    /* Crear "gestion_cintas_task" usando la función xTaskCreatePinnedToCore() */
    xTaskCreatePinnedToCore(
        gestion_cintas_task,              /* Función de la tarea.        */
        "gestion_cintas_task",            /* Nombre de la tarea.         */
        10000,                            /* Tamaño de pila de la tarea. */
        (void *)&gestion_cintas,          /* Parámetro de la tarea.      */
        1,                                /* Prioridad de la tarea.      */
        &gestion_cintas_task_handle,      /* Manejador de la tarea.      */
        0);                               /* Anclar tarea al núcleo 0.   */
    
    /* Crear "promedio_tiempos_llenado" usando la función xTaskCreatePinnedToCore() */
    xTaskCreatePinnedToCore(
        promedio_tiempos_llenado,         /* Función de la tarea.        */
        "promedio_tiempos_llenado",       /* Nombre de la tarea.         */
        10000,                            /* Tamaño de pila de la tarea. */
        (void *)&gestion_cintas,          /* Parámetro de la tarea.      */
        1,                                /* Prioridad de la tarea.      */
        &promedio_tiempos_llenado_handle, /* Manejador de la tarea.      */
        0);                               /* Anclar tarea al núcleo 0.   */

}   /* on_setup() */

/*** end of file ***/
