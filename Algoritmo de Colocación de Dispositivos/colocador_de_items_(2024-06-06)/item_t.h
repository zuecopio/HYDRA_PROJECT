/**
 * @file     item_t.h
 *
 * @brief    Implementación y definición de la clase item_t.
 * 
 * @version  0.6   (2024/06/06) Traducción de los comentarios al español
 * @version  0.5   (2024/05/07) Solucionado bug en la clase box_t "bucle infinito"
 * @version  0.4.1 (2024/05/06) Añadido cálculo de TCP pose de todos los items
 * @version  0.4   (2024/04/28) Añadida función (generate_mqtt_order)
 * @version  0.3.2 (2024/04/27) Añadido cálculo de TCP pose de PULSERA y RELOJ
 * @version  0.3.1 (2024/04/26) Añadido cálculo de TCP pose de TELEFONO y TABLET
 * @version  0.2   (2024/03/20) Añadido funciones de log
 * @version  0.1   (2024/03/15) Prototipo inicial funcional
 *
 * @author   Grupo PR2-A04' <mbelmar@etsinf.upv.es>
 * 
 * @date     Junio, 2024
 * @section  PR2-GIIROB
 */

#ifndef ITEM_T_H
#define ITEM_T_H

#include <string>
#include "defines.h"
#include "logger.h"
#include "space_t.h"

using namespace std;

// static const char * ITEM_TAG = __FILE__;
static const char * ITEM_TAG = "item_t.h";

class item_t
{
    private:

        // ATRIBUTOS.
        string item_id;
        itemType_t type;
        space_t size;
        space_t posInBox;
        string target_str; // posición + orientación

    public:
        
        /******************************************************************************/
        /*!
         * @brief  El constructor de la clase item_t.
         * @param  item_id  Indica qué tipo/modelo/variante de item es.
         */
        item_t(string item_id)
        {
            traceln(ITEM_TAG, "item_t()");
            this->item_id = item_id;

            if (item_id.find("pulsera") != string::npos) 
            {
                this->type = PULSERA;
                this->size.set_space(0, 0, 0, 80, 75, 30);
            }
            else if (item_id.find("reloj") != string::npos)
            {
                this->type = RELOJ;
                this->size.set_space(0, 0, 0, 80, 75, 60);
            }
            else if ((item_id.find("telefono") != string::npos) && (item_id.find("funda") != string::npos))
            {
                this->type = FUNDA_TELEFONO;
                this->size.set_space(0, 0, 0, 80, 150, 20);
            }
            else if (item_id.find("telefono") != string::npos)
            {
                this->type = TELEFONO;
                this->size.set_space(0, 0, 0, 80, 150, 60);
            }
            else if ((item_id.find("ereader") != string::npos) && (item_id.find("funda") != string::npos))
            {
                this->type = FUNDA_EREADER;
                this->size.set_space(0, 0, 0, 120, 150, 20);
            }
            else if (item_id.find("ereader") != string::npos)
            {
                this->type = EREADER;
                this->size.set_space(0, 0, 0, 120, 150, 40);
            }
            else if ((item_id.find("tablet") != string::npos) && (item_id.find("funda") != string::npos))
            {
                this->type = FUNDA_TABLET;
                this->size.set_space(0, 0, 0, 240, 150, 20);
            }
            else if (item_id.find("tablet") != string::npos)
            {
                this->type = TABLET;
                this->size.set_space(0, 0, 0, 240, 150, 40);
            }
            else
            {
                ;
            }

            this->posInBox = this->size;
            this->target_str = "";

            traceln(ITEM_TAG, "item_t() - END");
        
        }   /* item_t() */

        /******************************************************************************/
        /*!
         * @brief  El destructor de la clase item_t.
         * @param  void
         */
        ~item_t(void)
        {
            traceln(ITEM_TAG, "~item_t()");
            traceln(ITEM_TAG, "~item_t() - END");
            
        }   /* ~item_t() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el atributo de size item_t.
         * @param  void
         * @return size  El atributo privado que indica el tamaño del item.
         */
        space_t
        get_size(void)
        {
            traceln(ITEM_TAG, "get_size()");
            traceln(ITEM_TAG, "get_size() - END");
            return size;
        
        }   /* get_size() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el atributo posInBox de item_t.
         * @param  void
         * @return El valor de la posición del item en la caja.
         */
        space_t
        get_posInBox(void)
        {
            traceln(ITEM_TAG, "get_posInBox()");
            traceln(ITEM_TAG, "get_posInBox() - END");
            return posInBox;
        
        }   /* get_posInBox() */

        /******************************************************************************/
        /*!
         * @brief  Método para modificar el atributo privado posInBox de item_t.
         * @param  posInBox  La variable space_t que se guardará en posInBox.
         * @return void
         */
        void
        set_posInBox(space_t newPosInBox)
        {
            traceln(ITEM_TAG, "set_posInBox()");
            this->posInBox = newPosInBox;
            traceln(ITEM_TAG, "set_posInBox() - END");

        }   /* set_posInBox() */

        /******************************************************************************/
        /*!
         * @brief  Método para modificar el atributo privado target_str de item_t.
         * @param  target_str  La pose de TCP calculada.
         * @return void
         */
        void
        set_target_str(string target_str)
        {
            this->target_str = target_str;
        
        }   /* set_target_str() */ 

        /******************************************************************************/
        /*!
         * @brief  Devuelve el atributo type de item_t.
         * @param  void
         * @return El tipo de item.
         */
        itemType_t
        get_type(void)
        {
            return type;
        
        }   /* get_type() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el atributo target_str de item_t.
         * @param  void
         * @return El objetivo TCP.
         */
        string
        get_target_str(void)
        {
            return target_str;
        
        }   /* get_target_str() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el atributo item_id de item_t.
         * @param  void
         * @return El identificador del item.
         */
        string
        get_item_id(void)
        {
            return item_id;

        }   /* get_item_id() */
};

#endif /* ITEM_T_H */

/*** end of file ***/