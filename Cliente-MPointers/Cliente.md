# Explicacion de los archivos usados en el cliente

* Empezamos con los archivos.h (Clases) y despues con los archivos.cpp que contienen la logica

## Cliente.h
- Este contiene la logica respecto a los componentes de la interfaz
- Tiene un metododo para actualizar el un label laamado lblRespuesta, que es el label que muestra que sucede con los comandos que resive o informacion del mismo server. Esto mediante el boton que envia el comando al server

## ErrorLogger.h
  - Tiene los metodos de ErrorLogger.cpp, para una mayor facilidad de llamado.
  - Esta clase es la que se invoca cuando uno quiere recolpilar los errores que genera el codigo, o almenos los que estan explicitos en el codigo.

## InfoLogger.h
- Clase que se encarga de recopilar o mas bien llamar a las funciones que recopilan informacion o mensaajes.
- Al tenerla separada de la logica es mas facil su llamdo desde otro archivo.cpp, ademas de mas ordenado.

## MemoryLogger.h
- Clase que guarda lo que sucede con las peticiones del cliente.

  

## ErrorLogger.cpp
- Logica necesaria para el funcionamiento de recopilacion de los errores.
- Se guarda en una sub carpeta de logs, llamada error

## InfoLogger.cpp
- Logica para recopilar datos, info o alguna informacion no asociada conb las ordenes que resive del cliente.
-  Se guarda en una subcarpeta llamada logs, sub carpeta llamada info

## MemoryLogger.cpp
- Basiamente tiene la logica para guardar la informacion de las ordenes que resive del cliente

##BtnEnvio.cpp
-Encargado de enviar los comandos al server.

## main.cpp
-Metodo principal que inicializa el programa



    
