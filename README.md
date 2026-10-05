# Tarea 1 — Planificador Dieciochero

---

## 1. Descripción del proyecto

El señor Loyola quiere celebrar las Fiestas Patrias durante toda la semana y tener todo bien organizado. Para eso se desarrolló un programa que lee un archivo con las actividades del día (por ejemplo "prender el carbón", "comprar la carne" o "asar la longaniza") y las va ejecutando en el orden correcto.

Cada actividad tiene un ID, un nombre, un tiempo en milisegundos y una lista de dependencias. Una dependencia es una actividad que debe terminar antes de que otra pueda comenzar. Por ejemplo, no se puede asar la longaniza si todavía no está prendido el carbón.

El programa recibe el archivo con el plan y un número `K`, que corresponde a la cantidad máxima de actividades que pueden estar corriendo al mismo tiempo. Cada actividad se ejecuta en un proceso distinto, creado con `fork()`. No se usan hilos.

---

## 2. Archivos del proyecto

```text
Tarea-1-Sistemas-operativos/
├── actividad.h
├── grafo.h
├── grafo.cpp
├── leerplan.h
├── leerplan.cpp
├── main.cpp
├── Makefile
├── plan.txt
└── README.md
```

| Archivo | Para qué sirve |
|:---|:---|
| `actividad.h` | Define cómo se guarda una actividad. |
| `leerplan.cpp` / `leerplan.h` | Lee el archivo del plan y lo guarda en memoria. |
| `grafo.cpp` / `grafo.h` | Valida el plan y decide qué actividades ya se pueden ejecutar. |
| `main.cpp` | Crea los procesos, controla el límite K, usa los pipes y maneja el Ctrl+C. |
| `Makefile` | Compila todo con un solo comando. |
| `plan.txt` | Plan de ejemplo para probar. |

---

## 3. Compilación y ejecución

Desde la carpeta del proyecto:

```bash
make
```

Esto compila todos los archivos con las opciones `-std=c++17 -Wall -Wextra -lpthread` y genera el ejecutable `planificador`. Al terminar, el mismo `make` borra los archivos objeto (`.o`), por lo que solo queda el ejecutable.

Para ejecutar:

```bash
./planificador <archivo.txt> <K>
```

| Parámetro | Descripción |
|:---|:---|
| `<archivo.txt>` | Archivo con las actividades. |
| `<K>` | Máximo de procesos al mismo tiempo. Debe ser mayor que 0. |

Ejemplo:

```bash
./planificador plan.txt 3
```

Esto ejecuta el plan de `plan.txt` con un máximo de 3 actividades al mismo tiempo.

Si el plan tiene un ciclo o una dependencia que no existe, el programa avisa del error y no ejecuta nada.

---

## 4. Formato del archivo

Cada línea del archivo es una actividad:

```text
ID : nombre : tiempo_ms : dependencia1, dependencia2
```

Ejemplo:

```text
1 : prender_carbon : 500 :
2 : comprar_carne : 1200 :
3 : comprar_pan : 300 :
4 : asar_longaniza : 800 : 1, 2
5 : armar_choripan : 250 : 3, 4
6 : servir_mesa : 100 : 5
```

- Si una actividad no depende de ninguna otra, el último campo se deja vacío.
- Si no se indica el tiempo, el programa asigna uno al azar entre 100 y 5000 ms.
- Las líneas vacías se ignoran.

---

## 5. Funciones implementadas

### actividad.h
- `Actividad`: struct que guarda el id, el nombre, el tiempo en ms y un vector con las dependencias.

### leerplan.cpp
- `limpiar`: elimina los espacios del inicio y del final de un texto. Es necesaria porque las líneas del archivo traen espacios, y el ID `"1 "` no es igual a `"1"`.
- `leerPlan`: lee el archivo línea por línea, separa los 4 campos usando `:` y guarda cada actividad en un `map` con su ID como clave. Si falta el tiempo, asigna uno al azar.

### grafo.cpp
- `dependenciasValidas`: revisa que todas las dependencias existan en el plan.
- `hayCiclo`: detecta si el plan tiene un ciclo (por ejemplo, la 1 depende de la 2 y la 2 depende de la 1). Con un ciclo ninguna de las dos podría empezar nunca.
- `revisarGrafo`: junta las dos revisiones anteriores y se llama una vez al inicio.
- `iniciarEstados`: deja todas las actividades en `PENDIENTE`.
- `prepararEspera`: calcula cuántas dependencias le faltan a cada actividad, registra quién depende de quién y deja en la lista `listas` las actividades que no dependen de nadie, que son las primeras que se pueden ejecutar.
- `avisarTerminada`: cuando una actividad termina, le resta 1 al contador de las que dependían de ella. Si a alguna le llega a 0, pasa a `listas`.
- `abortarDependientes`: si una actividad falla, marca como `ABORTADA` a todas las que dependían de ella, directa o indirectamente. El resto sigue normal.

### main.cpp
- `crearpipes`: crea un pipe con nombre (`mkfifo`) por cada actividad, llamado `actividad_<ID>`. Si había uno antiguo, lo borra antes.
- `abrirpipes`: abre el extremo de lectura de cada pipe en el proceso padre.
- `crearAct`: crea un proceso hijo con `fork()`. El hijo simula la actividad con `usleep` durante su tiempo y, al terminar, escribe su ID en el pipe de cada actividad que depende de ella. El padre guarda el PID para saber qué actividad corresponde a cada proceso.
- `esperaract`: espera con `waitpid` a que termine un hijo y revisa cómo terminó. Si terminó bien, la marca como `TERMINADA`, lee los mensajes de los pipes de sus dependientes y avisa que ya terminó. Si terminó con error, la marca como `ABORTADA` y aborta su rama.
- `controlc`: manejador de la señal `SIGINT`. Solo activa una variable (`ctrlc`) para avisar que se apretó Ctrl+C.
- `main`: revisa los argumentos, lee y valida el plan, crea los pipes y luego repite lo siguiente: si hay una actividad lista y hay menos de K procesos corriendo, lanza una; si no, espera a que termine algún hijo. Al inicio de cada vuelta revisa si se apretó Ctrl+C. Al final cierra y borra los pipes.

---

## 6. Decisiones de diseño

### Uso de map y vector
El plan se guarda en un `map<string, Actividad>` con el ID como clave, lo que permite encontrar cualquier actividad rápidamente a partir de su ID. Para las listas se usa `vector`.

### Contador de dependencias en vez de buscar cada vez
En una primera versión, para saber qué actividad se podía ejecutar se recorría todo el plan en cada vuelta. Con pocas actividades funcionaba bien, pero con 10000 actividades independientes tardó más de 98 segundos y hubo que cancelarla. Para solucionarlo, cada actividad lleva un contador con las dependencias que le faltan, y cuando llega a 0 pasa a una lista de actividades listas. Así ya no es necesario recorrer todo el plan. Con el mismo caso de 10000 actividades, el tiempo bajó a unos 3 segundos.

### Límite K
El padre lleva la cuenta de los hijos que están corriendo. Si ya hay K, no crea más y espera a que termine alguno.

### Sin espera activa
Cuando no se puede lanzar ninguna actividad, el padre queda bloqueado en `waitpid`, por lo que no gasta CPU dando vueltas.

### Validación antes de ejecutar
Antes de crear cualquier proceso se revisa que no haya ciclos ni dependencias inexistentes. Así se evita que el programa quede esperando para siempre.

### Pipes con nombre
Se usó un pipe con nombre por actividad porque así cualquier hijo puede escribirle a sus dependientes sin tener que heredar descriptores de otros procesos. El padre abre cada pipe para lectura sin bloquearse (`O_NONBLOCK`), de modo que los hijos pueden abrirlo para escribir sin quedarse esperando. Los mensajes son cortos (solo el ID de la actividad que terminó). Al finalizar, el padre cierra y borra todos los pipes, para no dejar archivos sobrantes en la carpeta.

### Falla de una actividad
El padre revisa el código de salida de cada hijo. Si una actividad termina con un código distinto de 0, se marca como `ABORTADA` y solo se abortan las que dependían de ella. Todo lo que no tiene relación con esa rama sigue funcionando.

### Ctrl+C (SIGINT)
El manejador de la señal solo cambia una variable, ya que dentro de un manejador conviene hacer lo mínimo posible. El ciclo principal revisa esa variable en cada vuelta. Si está activa, el padre envía `SIGTERM` a todos los hijos que siguen corriendo, los espera con `waitpid` para que no queden procesos zombis, cierra los pipes y termina el programa. Los hijos restauran el comportamiento normal de `SIGINT`, para que mueran y no sigan ejecutándose.

---

## 7. Pruebas realizadas

- Plan de ejemplo (`plan.txt`) con K=3: las actividades terminan respetando las dependencias y `comenzar_fiesta` termina al final.
- Plan con ciclo: se detecta el error y no se ejecuta nada.
- Plan con una dependencia inexistente: se avisa qué actividad tiene el problema.
- Plan con tiempos vacíos: se asignan tiempos al azar y las dependencias se respetan igual.
- Argumentos incorrectos (sin argumentos, K=0 o archivo inexistente): el programa avisa y termina con error.
- Límite K: con 4 actividades de 2 segundos y K=2, el programa tardó 4 segundos, es decir, nunca hubo más de 2 al mismo tiempo.
- Falla de una actividad (se probó con una línea temporal que hacía fallar la actividad 4): se abortaron solo sus dependientes (5, 7 y 8) y las demás terminaron bien.
- Ctrl+C durante la ejecución: el programa se detiene, no quedan procesos hijos, zombis ni archivos de pipes.
- 10000 actividades en cadena (cada una depende de la anterior) con K=3: unos 14 segundos, cerca del mínimo posible porque cada una dura 1 ms y se ejecutan una tras otra.
- 10000 actividades sin dependencias con K=50: unos 3 segundos, con las 10000 terminadas correctamente.

---

## 8. Integrantes

- Javier Zavaleta
- Martina Veloso
