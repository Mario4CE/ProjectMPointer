# Explicacion de los archivos usados en el Server

** Empezamos con los archivos.h (Clases) **

## interfaz.h
- Este contiene la logica respecto a los componentes de la interfaz
- Tiene un metododo para actualizar el un label laamado lblRespuesta, que es el label que muestra que sucede con los comandos que resive o informacion del mismo server.

## ErrorLogger.h
  - Tiene los metodos de ErrorLogger.cpp, para una mayor facilidad de llamado.
  - Esta clase es la que se invoca cuando uno quiere recolpilar los errores que genera el codigo, o almenos los que estan explicitos en el codigo.

## InfoLogger.h
- Clase que se encarga de recopilar o mas bien llamar a las funciones que recopilan informacion o mensaajes.
- Al tenerla separada de la logica es mas facil su llamdo desde otro archivo.cpp, ademas de mas ordenado.

## ActualizarRespuesta.h
- clase que maneja al label de respuesta, mas que nada para que no se acceda direwctamente a la logica de la interfaz

## MemoryLogger.h
- Clase que guarda lo que sucede con las peticiones del cliente.

## MemoryManager.h
- Calse encargada de la administracion de memoria.

##Server.h
- Informacion para inicializar el server y conectarse con el cliente

## ordenes.h
- Clase que maneja los comandos que puede resivir del cliente. Para asi saber que debede hacer.

** Seguimos con los archivos.cpp **




    
