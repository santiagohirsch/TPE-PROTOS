Este repositorio estÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ destinado a la realizaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n del Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico Especial de la materia Protocolos de la ComunicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de la facultad de ingenierÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a ITBA (Instituto TecnolÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³gico de Buenos Aires)

### TODO
El informe
       DEBE contener informaciÃÂÃÂ³n sobre las pruebas de stress.  Algunas
       preguntas interesantes a responder son:

       *  ÃÂÃÂ¿Cual es la mÃÂÃÂ¡xima cantidad de conexiones simultÃÂÃÂ¡neas que
          soporta?

       *  ÃÂÃÂ¿CÃÂÃÂ³mo se degrada el throughput?

La consigna fue la siguiente:

"""
                    Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico Especial 2023/2
                               RevisiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n 0

Resumen

   Este documento describe el Trabajo Especial de la materia Protocolos
   de ComunicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n para la cursada del segundo cuatrimestre del aÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±o
   2023.

   En su ejecuciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n los alumnos DEBEN demostrar habilidad para la
   programaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de aplicaciones cliente/servidor con sockets, la
   comprensiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de estÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ndares de la industria, y la capacidad de diseÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±ar
   protocolos de aplicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n.

TerminologÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a

   Las palabras clave "DEBE", "NO DEBE", "OBLIGATORIO", "DEBERÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ", "NO
   DEBERÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ", "DEBERÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂA", "NO DEBERÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂA", "RECOMENDADO", "PUEDE" y "OPCIONAL"
   en este documento serÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡n interpretadas como se describe en el RFC 2119
   [RFC2119].

Tabla de Contenidos

   1.  Requerimientos Funcionales  . . . . . . . . . . . . . . . . .   1
   2.  Requerimientos No Funcionales . . . . . . . . . . . . . . . .   3
   3.  EvaluaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n  . . . . . . . . . . . . . . . . . . . . . . . . .   4
   4.  Referencias . . . . . . . . . . . . . . . . . . . . . . . . .   6
     4.1.  Normative References  . . . . . . . . . . . . . . . . . .   6
     4.2.  URIs  . . . . . . . . . . . . . . . . . . . . . . . . . .   7

1.  Requerimientos Funcionales

   El objetivo del trabajo es implementar un servidor para el protocolo
   POP3 (Post Office Protocol versiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n 3)[RFC1939] que pueda ser usado
   por Mail User Agents tales como Mozilla Thunderbird, Microsoft
   Outlook y Evolution para la recepciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de correos electrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³nicos.  No
   serÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ necesario soportar STARTTLS, TLS, SSL.

   El servidor DEBE

   1.  atender a mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂºltiples clientes en forma concurrente y simultÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡nea
       (al menos 500).

   2.  soportar autenticaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n usuario / contraseÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±a y pipelining
       [RFC2449].



                                Enunciado                       [Pag. 1]

                         Trabajo Especial 2023/2            October 2023


   3.  soportar conexiones IPv4 e IPV6.

   4.  reportar los fallos a los clientes usando toda la potencia del
       protocolo.

   5.  implementar mecanismos que permitan recolectar mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ©tricas que
       ayuden a monitorear la operaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n del sistema.

       A.  cantidad de conexiones histÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³ricas

       B.  cantidad de conexiones concurrentes

       C.  cantidad de bytes transferidos

       D.  cualquier otra mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ©trica que considere oportuno para el
           entendimiento del funcionamiento dinÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡mico del sistema

       Las mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ©tricas PUEDEN ser volÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡tiles (si se reinicia el servidor las
       estadÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­sticas pueden perderse).

   6.  implementar mecanismos que permitan a los usuarios cambiar la
       configuraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n del servidor en tiempo de ejecuciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n sin reiniciar
       el servidor.  Las diferentes implementaciones PUEDEN decidir
       disponibilizar otros cambios de ejecuciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n en tiempo de ejecuciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n
       de otras configuraciones (memoria utilizada en I/O, timeouts,
       etc).

   7.  implementar un registro de acceso que permitan a un administrador
       entender los accesos de cada uno de los usuarios.  Pensar en el
       caso de que llega una queja externa y el administrador debe saber
       quien fue el que se conectÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³ a cierto sitio web y cuando.

   8.  El servidor PUEDE implementar mecanismos que permita transformar
       los correos electrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³nicos utilizando aplicaciones externas.  Estas
       aplicaciones externas recibirÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡n por entrada estÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ndar el mensaje
       de correo y dejarÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ en su salida estÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ndar el mensaje procesado.
       La utilizaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de aplicaciones externas permite externalizar el
       procesamiento de los mensajes en aplicaciones que nada saben del
       protocolo que transporta los mensajes (en este caso POP3).  El
       programa externo por excelencia a correr es cat(1).  Existen
       herramientas anti-spam (basado en el contenido del mensaje) que
       utilizan esta interfaz como ser Spamassasin [1].  TambiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ©n existen
       herramientas en la lucha del malware como RenAttach [2].








                                Enunciado                       [Pag. 2]

                         Trabajo Especial 2023/2            October 2023


2.  Requerimientos No Funcionales

   Adicionalmente, la implementaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n DEBE

   1.  Estar escritos en el lenguaje de programaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n C, especÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­ficamente
       con la variante C11 (ISO/IEC 9899:2011).

   2.  Tener en cuenta todos los aspectos que hagan a la buena
       performance, escalabilidad y disponibilidad del servidor.  Se
       espera que se maneje de forma eficiente los flujos de informaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n
       (por ejemplo no cargar en memoria mensajes muy grandes, ser
       eficaz y eficiente en el intÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ©rprete de mensajes).  El informe
       DEBE contener informaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n sobre las pruebas de stress.  Algunas
       preguntas interesantes a responder son:

       *  ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¿Cual es la mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡xima cantidad de conexiones simultÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡neas que
          soporta?

       *  ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¿CÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³mo se degrada el throughput?

   3.  Seguir los lineamientos de IEEE Std 1003.1-2008, 2016 Edition /
       Base definitions / 12.  Utility Conventions [3] a menos que se
       especifique lo contrario: Esto se refiere a cÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³mo manejar
       argumentos de lÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­nea de comandos, parÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡metros, etc

   4.  DeberÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ documentar detalladamente el protocolo de monitoreo y
       configuraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n e implementar una aplicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n cliente.

   5.  Tanto la aplicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n servidor, como la aplicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n cliente de
       configuraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n/monitoreo DEBERÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂN manejar los argumentos de lÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­nea
       de comandos de cierta forma uniforme (por ejemplo -c <puerto>
       podrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a especificar el puerto utilizado para el protocolo de
       configuraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n/monitoreo).  Los detalles de quÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ© parÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡metros se
       deben manejar serÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ publicado en otro documento.

   6.  Si bien las programas son pequeÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±os podrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ utilizar librerÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­as o
       archivos (fragmento de cÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³digo) desarrollados por terceros siempre
       que se cumplan los siguientes requisitos:

       A.  La librerÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a o fragmento NO DEBE resolver las cuestiones de
           fondo del Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico.

       B.  La librerÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a o fragmento DEBE tener una licencia aprobada por
           la Open Source Initiative [4].

       C.  El uso de la librerÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a o fragmento DEBE ser aprobada por la
           CÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡tedra.




                                Enunciado                       [Pag. 3]

                         Trabajo Especial 2023/2            October 2023


       Para lograr la aprobaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n un alumno del grupo DEBE publicar una
       secuencia en el foro de discusiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n del trabajo prÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico.  La
       secuencia DEBE describir todos aquellos datos que permitan
       identificar a la librerÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a (por ejemplo la versiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n); su licencia
       de esta forma justificando porquÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ© es vÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡lido su uso; y el
       propÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³sito de su inclusiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n.  En caso de que sea un fragmento de
       cÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³digo debe adjuntarse.  EstÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ permitido utilizar cÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³digo publicado
       por los docentes durante la cursada actual, siempre que se
       atribuya correctamente.

   7.  A veces existirÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡n ambigÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂedades en las especificaciones o
       mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂºltiples formas en como se puede resolver o implementar un
       problema particular.  Por ser una materia de ingenierÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a se espera
       que los alumnos tomen decisiones de diseÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±o razonables en estos
       casos.  Los alumnos pueden basar sus decisiones en lo que conoce
       de ante mano de la tarea y en los objetivos enumerados en este
       documento o demÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡s enunciados.  Los docentes pueden darle consejos
       sobre las ventajas y desventajas de cada decisiones, pero los
       alumnos son los que en ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂºltima instancia las toman.

3.  EvaluaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n

   La realizaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n del Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico es una actividad grupal.  La
   calificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n es de carÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡cter grupal; pero si hay evidencias de que un
   alumno de un grupo no participÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³ en la elaboraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n, o ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ©ste no puede
   defender o demostrar su participaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n, entonces el alumno no podrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡
   aprobar el Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico.  Se espera transparencia en el
   desarrollo del trabajo (entregar el repositorio git).

   Cada grupo DEBE entregar todo el material necesario para poder
   reproducir el Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico.  Como mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­nimo DEBE contener:

   a.  Un informe en formato PDF [RFC3778] o text/plain (con
       codificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n UTF-8) que contenga las siguientes secciones
       (respetando el orden):

       1.   ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂndice

       2.   DescripciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n detallada de los protocolos y aplicaciones
            desarrolladas.  Debe especificar que se implementÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³.

       3.   Problemas encontrados durante el diseÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±o y la implementaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n.

       4.   Limitaciones de la aplicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n.

       5.   Posibles extensiones.

       6.   Conclusiones.



                                Enunciado                       [Pag. 4]

                         Trabajo Especial 2023/2            October 2023


       7.   Ejemplos de prueba.

       8.   GuÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a de instalaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n detallada y precisa.  No es necesario
            desarrollar un programa instalador.

       9.   Instrucciones para la configuraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n.

       10.  Ejemplos de configuraciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n y monitoreo.

       11.  Documento de diseÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ±o del proyecto (que ayuden a entender la
            arquitectura de la aplicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n).

   b.  CÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³digos fuente y archivos de construcciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n

   c.  Un archivo README en la raÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­z que describa al menos:

       A.  la ubicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de todos los materiales previamente enumerados

       B.  el procedimiento necesario para generar una versiÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n
           ejecutable de las aplicaciones

       C.  la ubicaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de los diferentes artefactos generados

       D.  cÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³mo se debe ejecutar las diferentes artefactos generados (y
           sus opciones)

   La entrega se realizarÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ por Campus ITBA en la asignaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n creada para
   ello con una fecha de entrega.  Se DEBE entregar un tarball que sea
   el producto de clonar el repositorio GIT (por lo tanto el repositorio
   GIT DEBE contener todos los materiales de entrega), y su historia.

   Una vez realizada la entrega los grupos DEBERÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂN mostrar el correcto
   funcionamiento del sistema con casos de prueba provisto por los
   equipos y provistos ese dÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a por la CÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡tedra.

   Para aprobar el Trabajo PrÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ctico se DEBE cumplir TODAS las siguientes
   condiciones:

   o  El material entregado DEBE estar completo (por ejemplo no se puede
      corregir si falta el informe o alguna clase)

   o  Se utilizan ÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂºnicamente las librerÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­a permitidas para los usos
      definidos.

   o  DEBE ser correcta las cuestiones relacionadas a la lectura/
      escrituras parciales.





                                Enunciado                       [Pag. 5]

                         Trabajo Especial 2023/2            October 2023


   o  DEBE ser correcta las cuestiones de entradas/salida no bloqueante.
      Por ejemplo las lecturas, escrituras y el establecimiento de
      nuevas conexiones DEBEN ser mediante suscripciones y no
      bloquearse.

   o  Sumar el 40% de los puntos de la calificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n total posible.  Por
      ejemplo, si la calificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡xima es 10 puntos, entonces se deben
      sumar 4 puntos.

   Se aceptarÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡n entregas tardÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­as entre 0 horas (inclusive) y 24 horas
   (exclusivo) luego de la fecha lÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ­mite de entrega, pero la calificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n
   mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡xima serÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡ el 40% de la calificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡xima original.

   La calificaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n mÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡xima posible serÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ¡

   1.  CUATRO puntos para una implementaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n bloqueante sin mecanismo de
       transformaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de mensajes.

   2.  SEIS puntos para una implementaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n bloqueante con mecanismo de
       transformaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de mensajes.

   3.  OCHO puntos para una implementaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n no bloqueante correcta sin
       mecanismo de transformaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de mensajes.

   4.  DIEZ puntos para una implementaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n no bloqueante con mecanismo
       de transformaciÃÂÃÂÃÂÃÂÃÂÃÂÃÂÃÂ³n de mensajes.

4.  Referencias

4.1.  Normative References

   [RFC1939]  Myers, J. and M. Rose, "Post Office Protocol - Version 3",
              STD 53, RFC 1939, May 1996.

   [RFC2119]  Bradner, S., "Key words for use in RFCs to Indicate
              Requirement Levels", BCP 14, RFC 2119, March 1997.

   [RFC2449]  Gellens, R., Newman, C., and L. Lundblade, "POP3 Extension
              Mechanism", RFC 2449, November 1998.

   [RFC3778]  Taft, E., Pravetz, J., Zilles, S., and L. Masinter, "The
              application/pdf Media Type", RFC 3778,
              DOI 10.17487/RFC3778, May 2004,
              <http://www.rfc-editor.org/info/rfc3778>.







                                Enunciado                       [Pag. 6]

                         Trabajo Especial 2023/2            October 2023


4.2.  URIs

   [1] https://spamassassin.apache.org/

   [2] http://www.pc-tools.net/unix/renattach/

   [3] https://pubs.opengroup.org/onlinepubs/9699919799/nframe.html

   [4] https://opensource.org/licenses










































                                Enunciado                       [Pag. 7]
"""
