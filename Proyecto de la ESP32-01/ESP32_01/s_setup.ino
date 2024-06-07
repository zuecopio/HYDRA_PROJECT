/**
 * @file   s_setup.ino
 * @brief  Definición e implementación de la funcione de inicialización.
 */

/******************************************************************************/
/*!
 * @brief  Función de inicialización de los sensores de presencia, la cámara,
 *         el pulsador de emergencia, el retenedor, y los motores de las cintas.
 *         En esta función además se crean las tareas de (g_tareas.ino).
 *           - sensor_inicio_cinta_task()
 *           - sensor_final_cinta_task()
 *           - lectorQR_task()
 * @param  void
 * @return void
 */
void on_setup(void)
{
    // SENSOR DE PRESENCIA AL INICIO DE LA CINTA
    //
    // Establecer trigPin en modo de salida.
    pinMode(trigPin_inicio_cinta, OUTPUT);
    // Establecer echoPin en modo de entrada.
    pinMode(echoPin_inicio_cinta, INPUT);

    // SENSOR DE PRESENCIA AL FINAL DE LA CINTA
    //
    // Establecer trigPin en modo de salida.
    pinMode(trigPin_final_cinta, OUTPUT);
    // Establecer echoPin en modo de entrada.
    pinMode(echoPin_final_cinta, INPUT);

    // CÁMARA PARA LECTURA DE QR
    //
    // Llamar a la función de inicialización. 
    camera_init();

    // PULSADOR DE EMERGENCIA
    //
    // Establecer EMERGENCY_BUTTON en modo input_pullup.
    pinMode(emergency_button.PIN, INPUT_PULLUP);
    // Crear el manejador de la interrupción.
    attachInterrupt(emergency_button.PIN, isr, FALLING);

    // RETENEDOR DE CAJAS EN LA CINTA
    //
    // Llamar a la función de inicialización.
    retenedor_init(RETENEDOR_PIN);

    // MOTOR CINTA 1
    //
    // Establecer motor_cinta_1 en modo de salida.
    pinMode(motor_cinta_1, OUTPUT);

    // MOTOR CINTA 2
    //
    // Establecer motor_cinta_2 en modo de salida.
    pinMode(motor_cinta_2, OUTPUT);

    /* Crear "sensor_inicio_cinta_task" usando la función xTaskCreatePinnedToCore() */
    xTaskCreatePinnedToCore(
        sensor_inicio_cinta_task,          /* Función de la tarea.        */
        "sensor_inicio_cinta_task",        /* Nombre de la tarea.         */
        10000,                             /* Tamaño de pila de la tarea. */
        (void *)&estado_cinta,             /* Parámetro de la tarea.      */
        1,                                 /* Prioridad de la tarea.      */
        &sensor_inicio_cinta_task_handle,  /* Manejador de la tarea.      */
        0);                                /* Anclar tarea al núcleo 0.   */

    /* Crear "sensor_final_cinta_task" usando la función xTaskCreatePinnedToCore() */
    xTaskCreatePinnedToCore(
        sensor_final_cinta_task,           /* Función de la tarea.        */
        "sensor_final_cinta_task",         /* Nombre de la tarea.         */
        10000,                             /* Tamaño de pila de la tarea. */
        (void *)&estado_cinta,             /* Parámetro de la tarea.      */
        1,                                 /* Prioridad de la tarea.      */
        &sensor_final_cinta_task_handle,   /* Manejador  de la tarea.     */
        0);                                /* Anclar tarea al núcleo 0.   */

    /* Crear "lectorQR_task" usando la función xTaskCreatePinnedToCore() */
    xTaskCreatePinnedToCore(
        lectorQR_task,                     /* Función de la tarea.        */
        "lectorQR_task",                   /* Nombre de la tarea.         */
        10000,                             /* Tamaño de pila de la tarea. */
        (void *)&estado_cinta,             /* Parámetro de la tarea.      */
        1,                                 /* Prioridad de la tarea.      */
        &lectorQR_task_handle,             /* Manejador  de la tarea.     */
        0);                                /* Anclar tarea al núcleo 0.   */

}   /* on_setup() */

/*** end of file ***/
