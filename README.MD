# POP3 SERVER
Integrantes

| Alumno               | Legajo | Mail                  |
|----------------------|--------|-----------------------|
| Bruno Enzo Baumgart  | 62161  | bbaumgart@itba.edu.ar |
| Santiago Jose Hirsch | 62169  | shirsch@itba.edu.ar   |   
| Mauro Joaquin Vella  | 62134  | mvella@itba.edu.ar    |

# Documentacion

Puede hallarse la documentacion en el directorio "documents" en la raiz del proyecto. En la misma se encuentra el diagrama de flujo y el informe sobre el proyecto.

#Requerimientos previos
       - Make
       - GCC

### Compilacion
```make all```

Una vez compilado genera dos ejecutables `./pop3d` y `./pop3d-user` que corresponden al servidor y a la aplicación del usuario respectivamente.

###Ejecucion

###Servidor POP3
Se debe correr:

```./pop3d -d <mail> -p <port> -a <admin>:<password>  -u <user>:<username> ``` 
NOTE: Se pueden agregar multiples usuarios colocando distintas -u para cada usuario
NOTE 2: De no ponerse -p <port> el servidor correra automaticamente en el 1110

### Aplicacion de usuario

```./pop3d-user -a <admin>:<adminpassword> <COMMAND> [args]```

NOTE: los argumentos pueden ser opcionales.

Para ver que comandos utilizar se puede correr el comando con el flag `-h` para que de despliegue la lista de comandos.
