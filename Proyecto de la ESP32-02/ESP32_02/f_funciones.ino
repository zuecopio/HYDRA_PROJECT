/**
 * @file   f_funciones.ino
 * @brief  Definición e implementación de las funciones
 *         de comunicación con la base de datos.
 */

/******************************************************************************/
/*!
 * @brief  Función de consulta de la Base de Datos (sin implementar).
 * @param  id_caja  Identificador de la caja para buscar en la base de datos.
 * @param  orden_id_caja  Puntero a un documento JSON donde se insertará toda
 *                        la información necesaria para la orden del industrial.
 * @return void
 */
void consulta_BBDD_y_generar_orden(char * id_caja, JsonDocument * orden_id_caja)
{
    // TODO: Código que permita conectarse a la BBDD y
    //       realizar las consultar pertinentes en ella.

    // TODO: A partir de las consultas rellenar el documento JSON
    //       'contenido_BBDD_id_caja' de forma adecuada para que se
    //       adapte al formato establecido en la documentación.
    //
    //       Ejemplo del formato:
    //
    //          {
    //            "tipo_caja": "S",
    //            "num_dispositivos": 2,
    //            "item_1": {
    //              "dispositivo": "tablet_A_01",
    //              "posicion_place": "120.0, 75.0, 40.0, -180.0, 0.0, 180.0"
    //            },
    //            "item_2": {
    //              "dispositivo": "tablet_D_02",
    //              "posicion_place": "120.0, 225.0, 40.0, -180.0, 0.0, 0.0"
    //            }
    //          }
}

/*** end of file ***/
