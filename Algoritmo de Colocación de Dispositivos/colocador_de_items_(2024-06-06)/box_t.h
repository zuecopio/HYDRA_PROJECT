/**
 * @file     box_t.h
 *
 * @brief    Implementación y definición de la clase box_t.
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

#ifndef BOX_T_H
#define BOX_T_H

#include <list>
#include <string>
#include "defines.h"
#include "logger.h"
#include "space_t.h"
#include "item_t.h"

using namespace std;

// static const char * BOX_TAG = __FILE__;
static const char * BOX_TAG = "box_t.h";

class box_t
{
    private:

        // ATRIBUTOS.
        boxType_t type;
        space_t size;
        list<space_t> spaceInUse;
        list<item_t> itemsToPlace;
        list<item_t> placedItems;
        string mqtt_order; // JSON format

    public:

        /******************************************************************************/
        /*!
         * @brief  El constructor de la clase box_t.
         * @param  type  Indica qué tipo de caja es.
         * @param  itemsToPlaceInOrder  Lista de elementos a colocar en la caja. 
         */
        box_t(boxType_t type, list<item_t> * itemsToPlaceInOrder)
        {
            traceln(BOX_TAG, "box_t()");
            this->type = type;

            switch (type)
            {
                case BOX_S:
                    this->size.set_space(0, 0, 0, 240, 300, 240);
                    this->spaceInUse.push_back(space_t(0, 0, 0, 240, 300, 0));
                    break;
                
                case BOX_M:
                    this->size.set_space(0, 0, 0, 320, 300, 240);
                    this->spaceInUse.push_back(space_t(0, 0, 0, 320, 300, 0));
                    break;

                case BOX_L:
                    this->size.set_space(0, 0, 0, 480, 300, 240);
                    this->spaceInUse.push_back(space_t(0, 0, 0, 480, 300, 0));
                    break;

                default:
                    break;

            }

            this->itemsToPlace = *itemsToPlaceInOrder;
            this->mqtt_order = "";

            traceln(BOX_TAG, "box_t() - END");

        }   /* box_t() */

        /******************************************************************************/
        /*!
         * @brief  El destructor de la clase box_t.
         * @param  void
         */
        ~box_t(void)
        {
            traceln(BOX_TAG, "~box_t()");
            traceln(BOX_TAG, "~box_t() - END");

        }   /* ~box_t() */

        /******************************************************************************/
        /*!
         * @brief  Determina si newPlaceSpace es una posición válida, es decir, no
         *         se sale de la caja y no se encuentra en volumen ya ocupado.
         * @param  newPlaceSpace  El nuevo espacio a verificar.
         * @return Verdadero o falso.
         */
        bool
        is_valid_space(space_t newSpace)
        {
            traceln(BOX_TAG, "is_valid_space()");
            bool answer = true;

            if ((newSpace.max_x() > this->size.max_x()) ||
                (newSpace.max_y() > this->size.max_y()) ||
                (newSpace.max_z() > this->size.max_z()))
            {
                answer = false;
            }

            for (list<space_t>::iterator it = spaceInUse.begin();
                ((it != spaceInUse.end())); ++it)
            {
                if (newSpace.is_subset_of(*it))
                {
                    answer = false;
                }
            }

            traceln(BOX_TAG, "is_valid_space() - END");

            return (answer);

        }   /* is_valid_space() */

        /******************************************************************************/
        /*!
         * @brief  Encuentra un nuevo punto donde se colocará el siguiente elemento.
         * @param  void
         * @return Nuevo punto que indica el siguiente lugar de origen.
         */
        point_t
        search_newOriginPoint(void)
        {
            traceln(BOX_TAG, "search_newOriginPoint()");
            point_t newPoint;
            int auxY;

            // 1) Initialize newPoint to its maximum possible values.
            newPoint.x = this->size.max_x();
            newPoint.y = this->size.max_y();
            newPoint.z = this->size.max_z();

            // 2) Calculate the intersection point betwen all spaceInUses.
            for (list<space_t>::iterator it = spaceInUse.begin();
                ((it != spaceInUse.end())); ++it)
            {
                // Calculate the x.
                if (it->max_x() < newPoint.x)
                {
                    newPoint.x = it->max_x();
                }    

                // Calculate the y.
                if (it->max_y() < newPoint.y)
                {
                    newPoint.y = it->max_y();
                } 

                // Calculate the z.
                if (it->max_z() < newPoint.z)
                {
                    newPoint.z = it->max_z();
                } 
            }

            // 3) If any coordinate is on the limit of the box, it is set to 0.
            if (newPoint.x == this->size.max_x())
            {
                newPoint.x = 0;
            }

            if (newPoint.y == this->size.max_y())
            {
                newPoint.y = 0;
            }

            // 4) Set the y coordinate always to 0, check if newPos is free, 
            //    if it is not, reset the y coordinate (auxY).
            auxY = newPoint.y;
            newPoint.y = 0;
            space_t aux(newPoint.x, newPoint.y, newPoint.z,
                        newPoint.x + 1, newPoint.y + 1, newPoint.z + 1);

            #if 0
            for (list<space_t>::iterator it = spaceInUse.begin();
                ((it != spaceInUse.end())); ++it)
            {
                if (aux.is_subset_of(* it))
                {
                    newPoint.y = auxY;
                }
            }
            #endif

            #if 1
            if (!(is_valid_space(aux)))
            {
                newPoint.y = auxY;
            }
            #endif

            // 5) Return new origin point.
            traceln(BOX_TAG, "search_newOriginPoint() - END"); 
            return (newPoint);
        
        }   /* search_newOriginPoint() */

        /******************************************************************************/
        /*!
         * @brief  Busca posibles uniones de volumen que puedan existir entre el aux
         *         e it, y viceversa. Se realizan operaciones para sumar el volumen
         *         de espacio ocupado.
         * @param  *it            Puntero a un iterador de la lista spaceInUse.
         * @param  *aux           El puntero a space_t que se desea 
         *                        agregar a la lista spaceInUse.
         * @param  *spaceIsAdded  Puntero a la variable bool.
         * @return void 
         */
        void
        search_possible_unions(space_t * aux, list<space_t>::iterator * it,
                               bool * needsToBeAdded)
        {
            traceln(BOX_TAG, "search_possible_unions()");

            //////////////////////////////////////
            ////   CUANDO AUX COINCIDE CON    ////
            ////   LA CARA X POSITIVA DE IT   ////
            //////////////////////////////////////

            if ((*it)->max_x() == aux->min_x())
            {
                // NO HAY DIFERENCIAS EN EL EJE Y O Z.
                //
                if (((*it)->max_y() == aux->max_y()) && ((*it)->max_z() == aux->max_z()))
                {
                    // Fusionar it con aux.
                    aux->set_space((*it)->min_x(), (*it)->min_y(), (*it)->min_z(),
                                   aux->max_x(), aux->max_y(), aux->max_z());

                    // Borrar el contenido de it y modificar el puntero it al siguiente iterador.
                    (*it) = spaceInUse.erase((*it));
                    (*needsToBeAdded) = true;
                }
                // DIFERENCIAS POSITIVAS EN EL EJE (Y O Z) O (Y Y Z). (IT ES SUPERIOR).
                //
                else if (((*it)->max_y() >= aux->max_y()) && ((*it)->max_z() >= aux->max_z()))
                {
                    // it no se elimina, pero se modifica el min de aux.
                    aux->set_space((*it)->min_x(), (*it)->min_y(), (*it)->min_z(),
                                   aux->max_x(), aux->max_y(), aux->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                // DIFERENCIAS NEGATIVAS EN EL EJE (Y O Z) O (Y Y Z). (IT ES INFERIOR).
                //
                else if (((*it)->max_y() <= aux->max_y()) && ((*it)->max_z() <= aux->max_z()))
                {
                    // Añadir aux sin modificar y modificar el max de it.
                    (*it)->set_space((*it)->min_x(), (*it)->min_y(), (*it)->min_z(),
                                      aux->max_x(), (*it)->max_y(), (*it)->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                else
                {
                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                }
            }

            //////////////////////////////////////
            ////   CUANDO AUX COINCIDE CON    ////
            ////   LA CARA NEGATIVA X DE IT.  ////
            //////////////////////////////////////

            else if ((*it)->min_x() == aux->max_x())
            {
                // NO HAY DIFERENCIAS EN EL EJE Y O Z.
                //
                if ((aux->max_y() == (*it)->max_y()) && (aux->max_z()) == (*it)->max_z())
                {
                    // Fusionar aux con it.
                    aux->set_space(aux->min_x(), aux->min_y(), aux->min_z(),
                                   (*it)->max_x(), aux->max_y(), aux->max_z());

                    // Borrar el contenido de it y modificar el puntero it al siguiente iterador.
                    (*it) = spaceInUse.erase((*it));
                    (*needsToBeAdded) = true;
                }
                // DIFERENCIAS POSITIVAS EN EL EJE (Y O Z) O (Y Y Z). (IT ES SUPERIOR).
                //
                else if (((*it)->max_y() >= aux->max_y()) && ((*it)->max_z() >= aux->max_z()))
                {
                    // it no se elimina, pero se modifica el max de aux.
                    aux->set_space(aux->min_x(), aux->min_y(), aux->min_z(),
                                   (*it)->max_x(), aux->max_y(), aux->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                // DIFERENCIAS NEGATIVAS EN EL EJE (Y O Z) O (Y Y Z). (IT ES INFERIOR).
                //
                else if (((*it)->max_y() <= aux->max_y()) && ((*it)->max_z() <= aux->max_z()))
                {
                    // Añadir aux sin modificar y modificar el min de it.
                    (*it)->set_space(aux->min_x(), aux->min_y(), aux->min_z(),
                                      (*it)->max_x(), (*it)->max_y(), (*it)->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                else
                {
                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                }
            }

            //////////////////////////////////////
            ////   CUANDO AUX COINCIDE CON    ////
            ////   LA CARA POSITIVA Y DE IT.  ////
            //////////////////////////////////////

            else if ((*it)->max_y() == aux->min_y())
            {
                /// NO HAY DIFERENCIAS ENTRE LOS EJES.
                if (((*it)->max_x() == aux->max_x()) && ((*it)->max_z() == aux->max_z()))
                {
                    // Fusionar it con aux.
                    aux->set_space((*it)->min_x(), (*it)->min_y(), (*it)->min_z(),
                                   aux->max_x(), aux->max_y(), aux->max_z());
                    // Borrar el contenido de it y modificar el puntero it al siguiente iterador.
                    (*it) = spaceInUse.erase((* it));
                    (*needsToBeAdded) = true;
                }
                /// DIFERENCIAS POSITIVAS EN EL EJE (X O Z) O (X Y Z). (IT ES SUPERIOR)
                else if (((*it)->max_x() >= aux->max_x()) && ((*it)->max_z() >= aux->max_z()))
                {
                    // it no se elimina, pero se modifica el min de aux.
                    aux->set_space((*it)->min_x(), (*it)->min_y(), (*it)->min_z(),
                                   aux->max_x(), aux->max_y(), aux->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                /// DIFERENCIAS NEGATIVAS EN EL EJE (X O Z) O (X Y Z). (IT ES INFERIOR)
                else if (((*it)->max_x() <= aux->max_x()) && ((*it)->max_z() <= aux->max_z()))
                {
                    // Añadir aux sin modificar y modificar el max de it.
                    (*it)->set_space((*it)->min_x(), (*it)->min_y(), (*it)->min_z(),
                                      (*it)->max_x(), aux->max_y(), (*it)->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                else
                {
                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                }
            }
            
            //////////////////////////////////////
            ////   CUANDO AUX COINCIDE CON    ////
            ////   LA CARA NEGATIVA Y DE IT.  ////
            //////////////////////////////////////

            else if ((*it)->min_y() == aux->max_y())
            {
                // NO HAY DIFERENCIAS ENTRE LOS EJES.
                //
                if (((*it)->max_x() == aux->max_x()) && ((*it)->max_z() == aux->max_z()))
                {
                    // Fusionar aux con it.
                    aux->set_space(aux->min_x(), aux->min_y(), aux->min_z(),
                                   aux->max_x(), (*it)->max_y(), aux->max_z());

                    // Borrar el contenido de it y modificar el puntero it al siguiente iterador.
                    (*it) = spaceInUse.erase((*it));
                    (*needsToBeAdded) = true;
                }
                // DIFERENCIAS POSITIVAS EN EL EJE (X O Z) O (X Y Z). (IT ES SUPERIOR).
                //
                else if (((*it)->max_x() >= aux->max_x()) && ((*it)->max_z() >= aux->max_z()))
                {
                    // it no se elimina, pero se modifica el max de aux.
                    aux->set_space(aux->min_x(), aux->min_y(), aux->min_z(),
                                   aux->max_x(), (*it)->max_y(), aux->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                // DIFERENCIAS NEGATIVAS EN EL EJE (X O Z) O (X Y Z). (IT ES INFERIOR).
                //
                else if (((*it)->max_x() <= aux->max_x()) && ((*it)->max_z() <= aux->max_z()))
                {
                    // Añadir aux sin modificar y modificar el min de it.
                    (*it)->set_space(aux->min_x(), aux->min_y(), aux->min_z(),
                                      (*it)->max_x(), (*it)->max_y(), (*it)->max_z());

                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                    (*needsToBeAdded) = true;
                }
                else
                {
                    // Modificar el puntero it al siguiente iterador.
                    ++(*it);
                }
            }

            /////////////////////////////////////
            ////   CUANDO AUX NO COINCIDE    ////
            ////  CON CUALQUIER CARA X O Y.  ////
            /////////////////////////////////////

            else
            {
                // Modificar el puntero it al siguiente iterador.
                ++(*it);
            }

            traceln(BOX_TAG, "search_possible_unions() - END");

        }   /* search_possible_unions() */       

        /******************************************************************************/
        /*!
         * @brief  Actualiza el espacio actual en uso de la caja.
         * @param  toAdd  Nuevo espacio en uso para ser añadido.
         * @return void 
         */
        void
        update_spaceInUse(space_t toAdd)
        { 
            traceln(BOX_TAG, "update_spaceInUse()");

            space_t aux(toAdd.min_x(), toAdd.min_y(), 0, toAdd.max_x(), toAdd.max_y(), toAdd.max_z());
            bool needsToBeAdded = false;
            string info;
            
            #if 0
            info = "spaceInUse (before): ";
            for (list<space_t>::iterator it = spaceInUse.begin();
                (it != spaceInUse.end()); ++it)
            {
                info += it->space_to_str() + " & ";
            }
            debugln(BOX_TAG, info.c_str());
            #endif            
            
            #if 1
            /////////////////////////////////////////////////////////
            // Mira las posibles uniones space_t que se pueden hacer.
            for (list<space_t>::iterator it2 = spaceInUse.begin(); (it2 != spaceInUse.end());
            /* Los incrementos de it2 se realizan en la función search_possible_unions(). */)
            {
                search_possible_unions(&aux, &it2, &needsToBeAdded);  
            }

            spaceInUse.push_front(aux); // Inserta aux al principio de la lista.
            #endif 

            #if 1
            info = "spaceInUse (before): ";
            for (list<space_t>::iterator it = spaceInUse.begin();
                (it != spaceInUse.end()); ++it)
            {
                info += it->space_to_str() + " & ";
            }
            debugln(BOX_TAG, info.c_str());
            #endif 

            ////////////////////////////////////////////////////////////////////
            // Si algún espacio de la lista se compone de otros espacios de la
            // misma lista, los subconjuntos se eliminan de la lista spaceInUse.
            for (list<space_t>::iterator it1 = spaceInUse.begin();
                 it1 != spaceInUse.end(); ++it1)
            {
                list<space_t>::iterator it2 = it1;
                ++it2;
                if (it2 == spaceInUse.end())
                {
                    it2 = spaceInUse.begin();
                }

                while (it2 != it1)
                {

                    if(it1->is_made_up_of(*it2))
                    {
                        it2 = spaceInUse.erase(it2);
                    }
                    else
                    {
                        ++it2;
                    }
                    
                    if (it2 == spaceInUse.end())
                    {
                        it2 = spaceInUse.begin();
                    }
                }
            }

            #if 1
            info = "spaceInUse (after):  ";
            for (list<space_t>::iterator it = spaceInUse.begin();
                (it != spaceInUse.end()); ++it)
            {
                info += it->space_to_str() + " & ";
            }
            info += "\n";
            debugln(BOX_TAG, info.c_str());
            #endif

            traceln(BOX_TAG, "update_spaceInUse() - END");

        }   /* update_spaceInUse() */

        /******************************************************************************/
        /*!
         * @brief  Coloca todos los elementos de itemsToPlace dentro de la caja y
         *         actualiza la lista placedItems. También establece targetPlace
         *         de todos los elementos.
         * @param  void
         * @return void
         */
        void
        place_items_in_box(void)
        {
            traceln(BOX_TAG, "place_items_in_box()");

            point_t newOriginPoint;
            space_t newPlaceSpace;
            int i = 0;
            bool exitFor = false;
            string info;

            // 1) Intenta colocar un elemento mientras itemsToPlace no está vacío.
            while (!(itemsToPlace.empty()))
            {  
                // 2) Busca un nuevo punto de origen.
                newOriginPoint = search_newOriginPoint();

                // 3) Obtiene el primer elemento de la lista itemsToPlace.
                exitFor = false;
                list<item_t>::iterator it;
                for (it = itemsToPlace.begin();
                    ((it != itemsToPlace.end()) && (exitFor == false)); ++it)
                {
                    newPlaceSpace = it->get_size() + newOriginPoint;

                    // 3.1) Intenta colocar el elemento; si no encaja, intenta
                    //      con el siguiente elemento de la lista itemsToPlace.  
                    if (is_valid_space(newPlaceSpace))
                    {
                        // 3.1.1) Insertar elemento en la lista placedItems.
                        it->set_posInBox(newPlaceSpace);
                        placedItems.push_back(*it);
                        info = "item_" + to_string(i) + ": ";
                        info += it->get_posInBox().space_to_str();
                        infoln(BOX_TAG, info.c_str());
                        i++;

                        // 3.1.2) Actualizar lista spaceInUse.
                        update_spaceInUse(it->get_posInBox());

                        // 3.1.3) Borrar el elemento de la lista itemsToPlace. 
                        itemsToPlace.erase(it);

                        // 3.1.4) Salir del bucle for. 
                        exitFor = true;
                    }
                }

                #if 1
                // 4) Si ningún it encaja, llenar el espacio con vacío.
                if (exitFor == false)
                {
                    point_t newEndPoint;

                    // 1) Inicializar newEndPoint a sus valores máximos
                    //    posibles pero z se establece en el valor z de
                    //    newOriginPoint.
                    newEndPoint.x = this->size.max_x();
                    newEndPoint.y = this->size.max_y();
                    newEndPoint.z = newOriginPoint.z;

                    for (list<space_t>::iterator it = spaceInUse.begin();
                         (it != spaceInUse.end()); ++it)
                    {
                        if ((it->min_x() > newOriginPoint.x) &&
                            (it->min_x() < newEndPoint.x))
                        {
                            newEndPoint.x = it->min_x();
                        }

                        if ((it->min_y() > newOriginPoint.y) &&
                            (it->min_y() < newEndPoint.y))
                        {
                            newEndPoint.y = it->min_y();
                        }

                        if ((it->max_z() > newOriginPoint.z) &&
                            (it->max_z() > newEndPoint.z))
                        {
                            newEndPoint.z = it->max_z();
                        }
                    }

                    // 2) Actualizar lista spaceInUse.
                    update_spaceInUse(space_t(newOriginPoint.x, newOriginPoint.y, newOriginPoint.z,
                                              newEndPoint.x, newEndPoint.y, newEndPoint.z));
                }
                #endif
            }

            traceln(BOX_TAG, "place_items_in_box() - END");

        }   /* place_items_in_box() */

        /******************************************************************************/
        /*!
         * @brief  Este método calcula las poses de TCP para todos los elementos
         *         para que puedan colocarse correctamente en el simulador RoboDK.
         * @param  void
         * @return void
         */
        void
        calculate_TCP_poses(void)
        {
            traceln(BOX_TAG, "calculate_TCP_poses()");

            uint16_t x, y, z;
            int16_t w; // r = -180, p = 0
            uint8_t y_decimal;

            for (list<item_t>::iterator it = placedItems.begin();
                (it != placedItems.end()); ++it)
            {
                x = (it->get_posInBox().min_x()) + (((it->get_posInBox().max_x()) - (it->get_posInBox().min_x())) / 2);
                y = (it->get_posInBox().min_y()) + (((it->get_posInBox().max_y()) - (it->get_posInBox().min_y())) / 2);
                z = (it->get_posInBox().max_z());

                itemType_t it_type = it->get_type();

                if (((size.max_y()) - y) <= (y - (size.min_y())))
                {
                    if ((it_type == RELOJ) || (it_type == PULSERA))
                    {
                        y = y - 72; // serían en verdad (-72.5)
                        w = -90;
                    }
                    else if ((it_type == TELEFONO) || (it_type == FUNDA_TELEFONO) ||
                             (it_type == EREADER)  || (it_type == FUNDA_EREADER))
                    {
                        y = y - 35;
                        w = -90;
                    }
                    else // ((it_type == TABLET) || (it_type == FUNDA_TABLET))
                    {
                        w = 0;
                    }
                }
                else
                {
                    if ((it_type == RELOJ) || (it_type == PULSERA))
                    {
                        y = y + 72; // serían en verdad (72.5)
                        w = 90;
                    }
                    else if ((it_type == TELEFONO) || (it_type == FUNDA_TELEFONO) ||
                             (it_type == EREADER)  || (it_type == FUNDA_EREADER))
                    {
                        y = y + 35;
                        w = 90;
                    }
                    else // ((it_type == TABLET) || (it_type == FUNDA_TABLET))
                    {
                        w = 180;
                    }
                }
                
                y_decimal = ((it->get_type() == RELOJ) || (it->get_type() == PULSERA)) ? (5) : (0);

                it->set_target_str(to_string(x) + ".0, " + to_string(y) + "." + to_string(y_decimal) + ", " + to_string(z) + ".0, -180.0, 0.0, " + to_string(w) + ".0");
            }

            traceln(BOX_TAG, "calculate_TCP_poses() - END");

        }   /* calculate_TCP_poses() */

        /******************************************************************************/
        /*!
         * @brief  Este método genera una orden en formato JSON para enviársela
         *         al robot industrial del simulador RoboDK (vía MQTT).
         * @param  void
         * @return void
         */
        void
        generate_mqtt_order(void)
        {
            traceln(BOX_TAG, "generate_mqtt_order()");

            calculate_TCP_poses();
            int total_items = placedItems.size(), i = 0;

            string box_type;
            if (type == BOX_S)
            {
                box_type = "S";
            }
            else if (type == BOX_M)
            {
                box_type = "M";
            }
            else // type == BOX_L
            {
                box_type = "L";
            }

            mqtt_order = "{\n";
            mqtt_order += "  \"tipo_caja\": \""      + box_type               + "\",\n";
            mqtt_order += "  \"num_dispositivos\": " + to_string(total_items) +   ",\n";          
            
            for (list<item_t>::iterator it = placedItems.begin();
                (it != placedItems.end()); ++it)
            {
                i++;
                /*
                    "item_01": {
                      "dispositivo": "tablet_A_02",
                      "posicion_place": "120.0, 75.0, 120.0, -180.0, 0.0, 180.0"
                    }
                */
                mqtt_order += "  \"item_" + to_string(i) + "\": {\n";
                mqtt_order += "    \"dispositivo\": \""    + (it->get_item_id())    + "\"," + "\n";
                mqtt_order += "    \"posicion_place\": \"" + (it->get_target_str()) + "\""  + "\n";
                mqtt_order += "  }";
                
                mqtt_order += (i != total_items) ? (",\n") : ("\n");
            }

            mqtt_order += "}";

            traceln(BOX_TAG, "generate_mqtt_order() - END");

        }   /* generate_mqtt_order() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el atributo mqtt_order de box_t.
         * @param  void
         * @return La orden en formato JSON.
         */
        string
        get_mqtt_order(void)
        {
            return mqtt_order;

        }   /* gst_mqtt_order() */
};

#endif /* BOX_T_H */

/*** end of file ***/