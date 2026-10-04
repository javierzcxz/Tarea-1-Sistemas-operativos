<div align="center">

# Tarea 1 — Planificador Dieciochero

Simulador que ejecuta actividades usando procesos, pipes y señales.

---

**Universidad Diego Portales** · Escuela de Informática y Telecomunicaciones

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Plataforma](https://img.shields.io/badge/Plataforma-Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

</div>

---

## 1. De qué trata el proyecto

El señor Loyola quiere celebrar las Fiestas Patrias toda la semana y lo quiere tener todo bien organizado. Para eso hicimos un programa que lee un archivo con las actividades del día (por ejemplo "prender el carbón", "comprar la carne", "asar la longaniza") y las va ejecutando en el orden correcto.

Cada actividad tiene un ID, un nombre, un tiempo en milisegundos y una lista de dependencias. Una dependencia es una actividad que tiene que terminar antes de que la otra pueda empezar. Por ejemplo, no se puede asar la longaniza si todavía no está prendido el carbón.

El programa recibe el archivo con el plan y un número `K`, que es la cantidad máxima de actividades que pueden estar corriendo al mismo tiempo.

> [!IMPORTANT]
> No usamos hilos (threads). Todo se hace con procesos, pipes y señales.

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
├── plan.txt
└── README.md
```

| Archivo | Para qué sirve |
|:---|:---|
| `actividad.h` | Define cómo guardamos una actividad. |
| `leerplan.cpp` / `leerplan.h` | Lee el archivo del plan y lo guarda en memoria. |
| `grafo.cpp` / `grafo.h` | Revisa que el plan sea válido y decide qué actividades ya se pueden ejecutar. |
| `main.cpp` | Crea los procesos, controla el límite K y espera a que terminen. |
| `plan.txt` | Plan de ejemplo para probar. |

---

## 3. Cómo compilar

Desde la carpeta del proyecto:

```bash
g++ -Wall -Wextra -std=c++17 main.cpp leerplan.cpp grafo.cpp -o planificador -lpthread
```

> [!NOTE]
> Ponemos `-lpthread` porque la pauta lo pide para compilar, pero no usamos hilos en ninguna parte del código.

---

## 4. Cómo ejecutar

```bash
./planificador <archivo.txt> <K>
```

| Parámetro | Qué es |
|:---|:---|
| `<archivo.txt>` | El archivo con las actividades. |
| `<K>` | Máximo de procesos al mismo tiempo. Tiene que ser mayor que 0. |

Ejemplo:

```bash
./planificador plan.txt 3
```

Esto ejecuta el plan de `plan.txt` con máximo 3 actividades al mismo tiempo.

Si el plan tiene un ciclo o una dependencia que no existe, el programa avisa del error y no ejecuta nada.

---

## 5. Formato del archivo

Cada línea es una actividad:

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

- Si una actividad no depende de nadie, el último campo se deja vacío.
- Si no se pone el tiempo, el programa elige uno al azar entre 100 y 5000 ms.
- Las líneas vacías se ignoran.

---

## 6. Qué hace cada función

### actividad.h
- `Actividad`: es un struct con el id, el nombre, el tiempo en ms y un vector con las dependencias.

### leerplan.cpp
- `limpiar`: le saca los espacios del principio y del final a un texto. Lo necesitamos porque en el archivo las líneas vienen con espacios y el ID `"1 "` no es igual a `"1"`.
- `leerPlan`: lee el archivo línea por línea, separa los 4 campos usando `:` y guarda cada actividad en un `map` usando su ID como clave. Si falta el tiempo, pone uno al azar.

### grafo.cpp
- `dependenciasValidas`: revisa que todas las dependencias existan en el plan.
- `hayCiclo`: revisa si el plan tiene un ciclo (por ejemplo, la 1 depende de la 2 y la 2 depende de la 1). Si hubiera uno, nada podría empezar nunca.
- `revisarGrafo`: junta las dos revisiones de arriba. Se llama una vez al principio.
- `iniciarEstados`: deja todas las actividades en `PENDIENTE`.
- `prepararEspera`: cuenta cuántas dependencias le faltan a cada actividad, anota quién depende de quién y deja en una lista (`listas`) las actividades que no dependen de nadie, que son las primeras que se pueden ejecutar.
- `avisarTerminada`: cuando una actividad termina, le resta 1 al contador de las que dependían de ella. Si a alguna le llega a 0, ya está lista y pasa a `listas`.
- `abortarDependientes`: si una actividad falla, marca como `ABORTADA` a todas las que dependían de ella, directa o indirectamente. Las demás siguen normal.

### main.cpp
- `crearAct`: crea un proceso hijo con `fork()`. El hijo simula la actividad con `usleep` durante su tiempo. El padre guarda el PID para saber qué actividad es cuál.
- `esperaract`: espera con `waitpid` a que termine un hijo, revisa cómo terminó y actualiza el estado. Si terminó bien avisa a sus dependientes, y si falló aborta su rama.
- `main`: revisa los argumentos, lee y valida el plan, y después repite lo siguiente: si hay una actividad lista y hay menos de K procesos corriendo, lanza una; si no, espera a que termine algún hijo.

### Parte de Martina

> rellenar compañera MARTINA: funciones de pipes, SIGINT (Ctrl+C) y cómo falla una actividad de verdad (si se agregó algo en el hijo, explicarlo acá).

---

## 7. Decisiones que tomamos

### Usar map y vector
Guardamos el plan en un `map<string, Actividad>` con el ID como clave. Es fácil de usar y así encontramos cualquier actividad por su ID rápido. Para las listas usamos `vector`.

### Contador de dependencias en vez de buscar cada vez
Al principio, para saber qué actividad se podía ejecutar, recorríamos todo el plan cada vez. Con pocas actividades andaba bien, pero con 10000 actividades independientes se demoró más de 98 segundos y la tuvimos que cancelar. Lo cambiamos: ahora cada actividad tiene un contador de dependencias que le faltan, y cuando llega a 0 pasa a una lista de "listas". Así ya no hay que recorrer todo el plan. Con el mismo caso de 10000 actividades bajó a unos 1,5 segundos.

### Límite K
El padre cuenta cuántos hijos están corriendo. Si ya hay K, no crea más y espera a que termine uno.

### Sin espera activa
Cuando no se puede lanzar nada, el padre se queda bloqueado en `waitpid`, así que no gasta CPU dando vueltas.

### Revisar el plan antes de ejecutar
Antes de crear cualquier proceso se revisa que no haya ciclos ni dependencias que no existan. Así evitamos que el programa se quede esperando para siempre.

### Si una actividad falla
Solo se abortan las actividades que dependían de ella. Todo lo que no tiene relación sigue funcionando.

### Ctrl+C y pipes
> rellenar compañera MARTINA: explicar cómo manejó SIGINT y los pipes, y por qué lo hizo así.

---

## 8. Pruebas que hicimos

- El plan de ejemplo (`plan.txt`) con K=3: las actividades terminan respetando las dependencias y la última (`comenzar_fiesta`) sale al final.
- Plan con ciclo: detecta el error y no ejecuta nada.
- Plan con una dependencia que no existe: avisa cuál actividad tiene el problema.
- Plan con tiempos vacíos: asigna tiempos al azar y las dependencias se respetan igual.
- Falla de una actividad (la probamos con una línea temporal que hacía fallar la actividad 4): se abortaron solo sus dependientes (5, 7 y 8) y las otras terminaron bien.
- 10000 actividades en cadena (cada una depende de la anterior) con K=3: unos 10 segundos, que es lo mínimo posible porque cada una dura 1 ms y van una tras otra.
- 10000 actividades sin dependencias con K=50: unos 1,5 segundos.

> rellenar compañera MARTINA: pruebas de pipes y Ctrl+C.

---

## 9. Integrantes

- **[TU NOMBRE]**: lectura del plan, grafo y dependencias.
- **Martina [APELLIDO]**: procesos, pipes y señales.

---

<div align="center">

Hecho para la asignatura de **Sistemas Operativos** · 2026

</div>
