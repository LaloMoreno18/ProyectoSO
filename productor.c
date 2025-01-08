#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mosquitto.h>
#include <json-c/json.h>

// Definición de la estructura del estudiante
typedef struct {
    char nombre[50];
    char primer_apellido[50];
    char segundo_apellido[50];
    char matricula[20];
    char carrera[50];
    int num_materias;
    char materias[10][50];
    float calificaciones[10];
} Estudiante;

// Función para crear un mensaje JSON desde la estructura
char *crearMensaje(Estudiante *estudiante) {
    struct json_object *json = json_object_new_object();
    json_object_object_add(json, "nombre", json_object_new_string(estudiante->nombre));
    json_object_object_add(json, "primer_apellido", json_object_new_string(estudiante->primer_apellido));
    json_object_object_add(json, "segundo_apellido", json_object_new_string(estudiante->segundo_apellido));
    json_object_object_add(json, "matricula", json_object_new_string(estudiante->matricula));
    json_object_object_add(json, "carrera", json_object_new_string(estudiante->carrera));
    json_object_object_add(json, "num_materias", json_object_new_int(estudiante->num_materias));

    struct json_object *materias_array = json_object_new_array();
    struct json_object *calificaciones_array = json_object_new_array();
    for (int i = 0; i < estudiante->num_materias; i++) {
        json_object_array_add(materias_array, json_object_new_string(estudiante->materias[i]));
        json_object_array_add(calificaciones_array, json_object_new_double(estudiante->calificaciones[i]));
    }
    json_object_object_add(json, "materias", materias_array);
    json_object_object_add(json, "calificaciones", calificaciones_array);

    const char *json_str = json_object_to_json_string(json);
    char *resultado = strdup(json_str); // Copia el string para retornarlo
    json_object_put(json);             // Libera el objeto JSON
    return resultado;
}

// Publicar el mensaje en Mosquitto
void publicarMensaje(struct mosquitto *mosq, const char *topic, const char *mensaje) {
    int ret = mosquitto_publish(mosq, NULL, topic, strlen(mensaje), mensaje, 0, false);
    if (ret != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error publicando mensaje: %s\n", mosquitto_strerror(ret));
    }
}

int main() {
    Estudiante estudiantes[2];

    // Datos del estudiante Eduardo Moreno Florin
    strncpy(estudiantes[0].nombre, "Eduardo", sizeof(estudiantes[0].nombre) - 1);
    strncpy(estudiantes[0].primer_apellido, "Moreno", sizeof(estudiantes[0].primer_apellido) - 1);
    strncpy(estudiantes[0].segundo_apellido, "Florin", sizeof(estudiantes[0].segundo_apellido) - 1);
    strncpy(estudiantes[0].matricula, "226020082", sizeof(estudiantes[0].matricula) - 1);
    strncpy(estudiantes[0].carrera, "ISC", sizeof(estudiantes[0].carrera) - 1);
    estudiantes[0].num_materias = 2;
    strncpy(estudiantes[0].materias[0], "Sistemas Operativos", sizeof(estudiantes[0].materias[0]) - 1);
    strncpy(estudiantes[0].materias[1], "Software", sizeof(estudiantes[0].materias[1]) - 1);
    estudiantes[0].calificaciones[0] = 80.0;
    estudiantes[0].calificaciones[1] = 95.0;
  // Datos del estudiante Sharis Montoya Puebla
    strncpy(estudiantes[1].nombre, "Sharis", sizeof(estudiantes[1].nombre) - 1);
    strncpy(estudiantes[1].primer_apellido, "Montoya", sizeof(estudiantes[1].primer_apellido) - 1);
    strncpy(estudiantes[1].segundo_apellido, "Puebla", sizeof(estudiantes[1].segundo_apellido) - 1);
    strncpy(estudiantes[1].matricula, "226020060", sizeof(estudiantes[1].matricula) - 1);
    strncpy(estudiantes[1].carrera, "ISC", sizeof(estudiantes[1].carrera) - 1);
    estudiantes[1].num_materias = 2;
    strncpy(estudiantes[1].materias[0], "Sistemas Operativos", sizeof(estudiantes[1].materias[0]) - 1);
    strncpy(estudiantes[1].materias[1], "Software", sizeof(estudiantes[1].materias[1]) - 1);
    estudiantes[1].calificaciones[0] = 100.0;
    estudiantes[1].calificaciones[1] = 90.0;

    mosquitto_lib_init();
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error inicializando Mosquitto\n");
        return 1;
    }

    if (mosquitto_connect(mosq, "localhost", 1883, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error conectando al broker Mosquitto\n");
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    // Publicar mensajes para cada estudiante
    for (int i = 0; i < 2; i++) {
        char *mensaje = crearMensaje(&estudiantes[i]);
        printf("Publicando mensaje para %s: %s\n", estudiantes[i].nombre, mensaje);
        publicarMensaje(mosq, "estudiantes", mensaje);
        free(mensaje);
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

