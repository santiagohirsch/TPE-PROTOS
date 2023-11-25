# POP3 SERVER
Integrantes

| Alumno               | Legajo | Mail                  |
|----------------------|--------|-----------------------|
| Bruno Enzo Baumgart  | 62161  | bbaumgart@itba.edu.ar |
| Santiago Jose Hirsch | 62169  | shirsch@itba.edu.ar   |   
| Mauro Joaquin Vella  | 62134  | mvella@itba.edu.ar    |

# Documentacion

Puede hallarse la documentacion en la raiz del proyecto. En la misma se encuentra el diagrama de flujo y el informe sobre el proyecto.

# Requerimientos previos
       - Make
       - GCC

### Compilacion
```make all```

Una vez compilado genera dos ejecutables `./pop3d` y `./pop3d-user` que corresponden al servidor y a la aplicación del usuario respectivamente.

### Ejecucion

### Servidor POP3
Se debe correr:

```./pop3d -d <mail> -p <port-ipv4> -P <port-ipv6> -a <admin>:<password>  -u <user>:<username> ``` 

IMPORTANT:    El -d <mail> DEBE ir antes del admin (-a <admin>:<password>) y de los usuarios (-u <user>:<password>).
              El resto de los comandos no tiene orden.

NOTE: Se pueden agregar multiples usuarios colocando distintas -u para cada usuario
NOTE 3: De no especificarse <port-ipv4> con el flag -p el servidor correrá automáticamente en el puerto 1110
NOTE 3: De no especificarse <port-ipv6> con el flag -P el servidor correrá automáticamente en el puerto 9090

### Aplicacion de usuario

```./pop3d-user -a <admin>:<adminpassword> <COMMAND> [args]```

NOTE: los argumentos dependiendo el COMMAND pueden ser opcionales.

Para ver que comandos utilizar se puede correr el comando con el flag `-h` para que de despliegue la lista de comandos.
