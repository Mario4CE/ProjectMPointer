# Explicacion de los archivos usados en el Server

* Empezamos con los archivos.h (Clases) y despues con los archivos.cpp que contienen la logica

## interfaz.h
- Este contiene la logica respecto a los componentes de la interfaz
- Tiene un metododo para actualizar el un label laamado lblRespuesta, que es el label que muestra que sucede con los comandos que resive o informacion del mismo server.
- Tiene un metodo para cerrar la ventana por medio de un comando

## ErrorLogger.h
  - Tiene los metodos de ErrorLogger.cpp, para una mayor facilidad de llamado.
  - Esta clase es la que se invoca cuando uno quiere recolpilar los errores que genera el codigo, o almenos los que estan explicitos en el codigo.

## InfoLogger.h
- Clase que se encarga de recopilar o mas bien llamar a las funciones que recopilan informacion o mensaajes.
- Al tenerla separada de la logica es mas facil su llamdo desde otro archivo.cpp, ademas de mas ordenado.

## ActualizarRespuesta.h
- clase que maneja al label de respuesta, mas que nada para que no se acceda direwctamente a la logica de la interfaz
- Tambien maneja el metodo para el cierre de la ventana

## MemoryLogger.h
- Clase que guarda lo que sucede con las peticiones del cliente.
- Hace uso de dos logger para manejar las peticiones
## MemoryManager.h
- Calse encargada de la administracion de memoria y las ordenes que resive.
- Es la clase mas importante ya que es la que resive, valida y hace las peticiones que resive del cliente

## Server.h
- Informacion para inicializar el server y conectarse con el cliente
- Tiene una direccion y un puerto definido


## ErrorLogger.cpp
- Logica necesaria para el funcionamiento de recopilacion de los errores.
- Se guarda en una sub carpeta de logs, llamada error

## InfoLogger.cpp
- Logica para recopilar datos, info o alguna informacion no asociada conb las ordenes que resive del cliente.
-  Se guarda en una subcarpeta llamada logs, sub carpeta llamada info

## MemoryLogger.cpp
- Basiamente tiene la logica para guardar la informacion de las ordenes que resive del cliente
  
## MemoryManager
- Posee lalogica que sabe que hacer con los comandos que resive
- Hace algunas validaciones para asegurarse qeu la informacion que se guarda y envia sea la correcta y evitar errores
- Esta posee los metodos de validacion y ejecucion de los comandos, cualquier cosa respecto a los camandos resividos pasa por aqui

## Server.cpp
- Logica encargada de inicializar al servidor y manejar la coneccion con el cliente

## Main.cpp
-Metodo principal que inicializa el programa
