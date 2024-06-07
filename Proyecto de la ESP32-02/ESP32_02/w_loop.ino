/**
 * @file   w_loop.ino
 * @brief  Definición e implementación de las funciones de bucle.
 */

/******************************************************************************/
/*!
 * @brief  Función de bucle vacío, no es necesario realizar alguna acción más
 *         aparte de las que realizan las tareas y la función 'mqttCallback'.
 * @param  void
 * @return void
 */
void on_loop(void)
{
    for(;;)
    {
        delay(1000);
    }
    
}   /* on_loop() */

/*** end of file ***/
