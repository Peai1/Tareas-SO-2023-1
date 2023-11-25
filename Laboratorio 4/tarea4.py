from threading import *
from datetime import datetime
import time
import random

NUM_ESTUDIANTES = 500

DEPARTAMENTOS = {
    'Matematicas': {'capacidad': 10, 'tam_fila': 20, 'duracion': 9},
    'Informatica': {'capacidad': 2, 'tam_fila': 8, 'duracion': 5},
    'Fisica': {'capacidad': 5, 'tam_fila': 15, 'duracion': 7},
    'Quimica': {'capacidad': 3, 'tam_fila': 6, 'duracion': 4},
    'DEFIDER': {'capacidad': 5, 'tam_fila': 6, 'duracion': 1},
    'Mecanica': {'capacidad': 4, 'tam_fila': 9, 'duracion': 4},
    'Minas': {'capacidad': 2, 'tam_fila': 7, 'duracion': 5}
}

SEMAFOROS_FILAS = {
    'Matematicas': Semaphore(20),
    'Informatica': Semaphore(8),
    'Fisica': Semaphore(15),
    'Quimica': Semaphore(6),
    'DEFIDER': Semaphore(6),
    'Mecanica': Semaphore(9),
    'Minas': Semaphore(7),
}

SEMAFOROS_CAPACIDADES = {
    'Matematicas': Semaphore(10),
    'Informatica': Semaphore(2),
    'Fisica': Semaphore(5),
    'Quimica': Semaphore(3),
    'DEFIDER': Semaphore(5),
    'Mecanica': Semaphore(4),
    'Minas': Semaphore(2),
}

LOCKS_ESCRITURA = {
    'PdLamparas': Lock(),
    'Matematicas': Lock(),
    'Informatica': Lock(),
    'Fisica': Lock(),
    'Quimica': Lock(),
    'DEFIDER': Lock(),
    'Mecanica': Lock(),
    'Minas': Lock(),
    'Salida': Lock(),
}

CONTADORES_FILAS = {
    'Matematicas': 0,
    'Informatica': 0,
    'Fisica': 0,
    'Quimica': 0,
    'DEFIDER': 0,
    'Mecanica': 0,
    'Minas': 0,
}

COND_CONTADORES = {
    'Matematicas': Condition(Lock()),
    'Informatica': Condition(Lock()),
    'Fisica': Condition(Lock()),
    'Quimica': Condition(Lock()),
    'DEFIDER': Condition(Lock()),
    'Mecanica':Condition(Lock()),
    'Minas': Condition(Lock())
}

COND_RESET = {
    'Matematicas': Condition(Lock()),
    'Informatica': Condition(Lock()),
    'Fisica': Condition(Lock()),
    'Quimica': Condition(Lock()),
    'DEFIDER': Condition(Lock()),
    'Mecanica':Condition(Lock()),
    'Minas': Condition(Lock())
}

CONTADORES_DEPARTAMENTOS = {
    'Matematicas': 0,
    'Informatica': 0,
    'Fisica': 0,
    'Quimica': 0,
    'DEFIDER': 0,
    'Mecanica': 0,
    'Minas': 0,
}

LOCKS_DEPARTAMENTOS = {
    'Matematicas': Lock(),
    'Informatica': Lock(),
    'Fisica': Lock(),
    'Quimica': Lock(),
    'DEFIDER': Lock(),
    'Mecanica':Lock(),
    'Minas': Lock(),
}

# Función que registra en su debido departamento, la hora de entrada a fila y el tiempo de entrada al departamento
# Parametros:
#    dep: nombre del departamento
#    id: id que corresponde al número de la persona
#    tiempo_entrada_fila: tiempo que entra a la fila del departamento
#    tiempo_entrada_departamento: tiempo que entra al departamento
#    numero: si es el primer o segundo departamento al que ingresa

def registrar_evento(dep, id, tiempo_entrada_fila, tiempo_entrada_departamento, numero):
    LOCKS_ESCRITURA[dep].acquire()
    if dep == "DEFIDER":
        with open(f"{dep}.txt", "a") as file:
            file.write(f"Persona{id+1}, {tiempo_entrada_fila}, {tiempo_entrada_departamento}, {numero}\n")
    else:
        with open(f"Departamento_de_{dep}.txt", "a") as file:
            file.write(f"Persona{id+1}, {tiempo_entrada_fila}, {tiempo_entrada_departamento}, {numero}\n")
    LOCKS_ESCRITURA[dep].release()
        
# Función que registra en patiolamparas.txt, la hora de entrada a fila de ambos departamentos
# Parametros:
#    id: id que corresponde al número de la persona
#    dep1: nombre del primer departamento que asistió
#    tiempo_entrada_fila1: tiempo que entra a la fila del departamento 1
#    dep2: nombre del segundo departamento que asistió
#    tiempo_entrada_fila1: tiempo que entra a la fila del departamento 2

def registrar_en_patio(id, tiempo_ingreso, dep1, tiempo_entrada_fila1, dep2, tiempo_entrada_fila2):
    LOCKS_ESCRITURA['PdLamparas'].acquire()
    
    if dep1 == "DEFIDER":
        dep1 = "DEFIDER"
    else:
        dep1 = "Departamento de " + dep1

    if dep2 == "DEFIDER":
        dep2 = "DEFIDER"
    else:
        dep2 = "Departamento de " + dep2
        
    with open("PdLamparas.txt", "a") as file:
        file.write(f"Persona{id+1}, {tiempo_ingreso}, {dep1}, {tiempo_entrada_fila1}, {dep2}, {tiempo_entrada_fila2}\n")
    LOCKS_ESCRITURA['PdLamparas'].release()

# Función que simula la fila y el acceso al departamento de un estudiante (hebra)
# Parametros:
#    dep: nombre del departamento que asiste
#    id: id que corresponde al número de la persona
#    numero: si es el primer o segundo departamento que ingresa

def consultar(dep, id, numero):
    # Semaforo de la fila y capacidad del departamento que desea acceder

    semaforo_fila = SEMAFOROS_FILAS[dep]
    semaforo_capacidad = SEMAFOROS_CAPACIDADES[dep]
    duracion = DEPARTAMENTOS[dep]['duracion']
    cond = COND_CONTADORES[dep]
    cond2 = COND_RESET[dep]
    
    # Espera para entrar a fila
    semaforo_fila.acquire()
    tiempo_entrada_fila = datetime.now().strftime("%H:%M:%S.%f")

    # Semaforo departamento
    semaforo_capacidad.acquire()   
    semaforo_fila.release()

    with cond:   
        CONTADORES_FILAS[dep] += 1
        if CONTADORES_FILAS[dep] == DEPARTAMENTOS[dep]['capacidad']:
            cond.notify_all()  # Hay suficientes personas para comenzar la consulta

        if CONTADORES_DEPARTAMENTOS[dep] >= DEPARTAMENTOS[dep]['capacidad']:          
            while CONTADORES_FILAS[dep] < DEPARTAMENTOS[dep]['capacidad']:
                cond.wait()  # Esperar a que haya suficientes estudiantes o hasta que todos hayan ingresado

    tiempo_entrada_departamento = datetime.now().strftime("%H:%M:%S.%f")
    registrar_evento(dep, id, tiempo_entrada_fila, tiempo_entrada_departamento, numero)

    # Resto ya que estudiante ya asistió a departamento
    with LOCKS_DEPARTAMENTOS[dep]:
        CONTADORES_DEPARTAMENTOS[dep] -= 1

    # Simula la consulta
    time.sleep(duracion)

    with cond:
        CONTADORES_FILAS[dep] -= 1

    with cond2:
        if CONTADORES_FILAS[dep] == 0:
            cond2.notify_all()  # Notificar cuando el último estudiante sale
        else:
            while CONTADORES_FILAS[dep] > 0:
                cond2.wait()  # Esperar si todavía hay estudiantes en el departamento
    time.sleep(0.1)
    semaforo_capacidad.release()

    return tiempo_entrada_fila

# Funcion que simula la llegada al departamento del estudiante, cuando realiza la dos consultas, la hora que sale y registra la salida en el .txt
# Parametros:
#    id: id que corresponde al número de la persona
#    dep1: nombre del primer departamento que asiste
#    dep2: nombre del segundo departamento que asiste

def ingresar(id, dep1, dep2):
    tiempo_ingreso = datetime.now().strftime("%H:%M:%S.%f")

    # Realizar dos consultas
    tiempo_entrada_fila1 = consultar(dep1, id, 1)
    tiempo_entrada_fila2 = consultar(dep2, id, 2)

    # Estudiante sale
    tiempo = datetime.now().strftime("%H:%M:%S.%f")

    # Registrar en patio
    registrar_en_patio(id, tiempo_ingreso, dep1, tiempo_entrada_fila1, dep2, tiempo_entrada_fila2)

    # Registrar salida
    with LOCKS_ESCRITURA['Salida']:
        with open(f"Salida.txt", "a") as file:
            file.write(f"Persona{id+1}, {tiempo}\n")

# Función que carga todas las variables, prepara archivos de escritura, eligue los departamentos random a los que asisten los estudiantes e inicia funciones de las hebras

def iniciar_simulacion():

    estudiantes = []

    with open("PdLamparas.txt", "w") as f:
        f.write("")
    with open("Salida.txt", "w") as f:
        f.write("")

    for i in DEPARTAMENTOS:
        if i == "DEFIDER":
            open(f"DEFIDER.txt","w")
        else:
            open(f"Departamento_de_{i}.txt","w")


    for i in range(NUM_ESTUDIANTES):
        # Elige dos departamentos aleatorios
        dep1, dep2 = random.sample(list(DEPARTAMENTOS.keys()), 2)

        # Contador de cantidad de estudiantes que asistirá a un departamento
        with LOCKS_DEPARTAMENTOS[dep1]:
            CONTADORES_DEPARTAMENTOS[dep1] += 1
        with LOCKS_DEPARTAMENTOS[dep2]:
            CONTADORES_DEPARTAMENTOS[dep2] += 1

        estudiantes.append(Thread(target=ingresar, args=(i,dep1,dep2)))
    
    for i in range(NUM_ESTUDIANTES):
        estudiantes[i].start()

    for thread in estudiantes:
        thread.join()

    print("Todos los estudiantes han terminado sus apelaciones.")


iniciar_simulacion()
