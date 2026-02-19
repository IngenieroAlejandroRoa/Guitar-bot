# Guitar Bot

Sistema robótico guitarrista controlado por ESP32. Utiliza servomotores para posicionamiento de acordes y motores paso a paso para punteo y rasgado, permitiendo la ejecución automatizada de piezas musicales mediante control embebido.

## Descripción

Guitar Bot es un robot capaz de tocar una guitarra de forma autónoma mediante la coordinación de tres subsistemas principales:

- **Sistema de acordes**: 9 servomotores posicionan los dedos mecánicos sobre las cuerdas para formar diferentes acordes
- **Sistema de rasgueo**: Motor paso a paso y servomotor que ejecutan el movimiento de rasgueo de las cuerdas
- **Sistema de punteo**: Motor paso a paso que posiciona el mecanismo de punteo y servomotor que ejecuta el golpe sobre cuerdas individuales

## Características Técnicas

### Hardware

- **Microcontrolador**: ESP32
- **Servomotores de acordes**: 9 unidades (pines 12, 14, 27, 26, 25, 33, 32, 2, 4)
- **Servomotor de rasgueo**: 1 unidad (pin 18)
- **Motor paso a paso de rasgueo**: 
  - Pines STEP: 23, DIR: 22
  - Finales de carrera: pin 21
  - Recorrido: 60-105 mm
- **Motor paso a paso de punteo**:
  - Pines STEP: 5, DIR: 15
  - Finales de carrera: pin 13
  - Recorrido: hasta 66 mm
- **Resolución**: 6.2348 pasos por mm
- **Velocidad de paso**: 580 microsegundos entre pulsos

### Software

- **Plataforma**: Arduino IDE con soporte ESP32
- **Librería**: ESP32Servo
- **Frecuencia PWM**: 50 Hz
- **Rango de pulso**: 500-2500 microsegundos

## Funcionamiento

### Secuencia de Operación

1. **Inicialización**: 
   - Todos los servos se posicionan en 90 grados
   - Los motores paso a paso ejecutan homing (retorno al origen mediante finales de carrera)
   - El sistema de rasgueo se posiciona a 105 mm

2. **Ciclo de rasgueo**:
   - Se ejecutan 4 acordes diferentes
   - Cada acorde se rasguea 4 veces (total 16 rasgueos)
   - El rasgueo alterna entre posiciones 105 mm y 45 mm

3. **Secuencia de punteo**:
   - Tras completar los 16 rasgueos, se ejecuta una secuencia de punteo
   - El motor paso a paso posiciona el mecanismo en diferentes cuerdas
   - El servomotor ejecuta el golpe de punteo
   - La secuencia incluye 12 movimientos programados

4. **Repetición**: El ciclo se reinicia indefinidamente

### Acordes Programados

El sistema incluye 4 acordes predefinidos, cada uno con configuraciones específicas de ángulos para los 9 servomotores:

- Acorde 0: Configuración base
- Acorde 1: Variante con servos 3 y 6 activos
- Acorde 2: Variante con servos 3 y 8 activos
- Acorde 3: Variante con servos 4, 5 y 8 activos

## Instalación

### Requisitos Previos

- Arduino IDE 1.8.x o superior
- Soporte para placa ESP32 instalado en Arduino IDE
- Librería ESP32Servo

### Instalación de Dependencias

1. Instalar el soporte para ESP32 en Arduino IDE:
   - Archivo > Preferencias
   - Agregar URL: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Herramientas > Placa > Gestor de placas > Buscar "ESP32" e instalar

2. Instalar la librería ESP32Servo:
   - Herramientas > Administrar bibliotecas
   - Buscar "ESP32Servo" e instalar

### Carga del Programa

1. Abrir `GuitarBot.ino` en Arduino IDE
2. Seleccionar la placa ESP32 correspondiente
3. Seleccionar el puerto serial correcto
4. Compilar y cargar el sketch

## Configuración

### Calibración de Servomotores

Los servomotores están configurados con un rango de pulso de 500-2500 microsegundos. Si es necesario ajustar:

```cpp
servos[i].attach(pinesServos[i], min_pulse, max_pulse);
```

### Ajuste de Velocidad

Modificar la variable `stepDelay_us` para cambiar la velocidad de los motores paso a paso:

```cpp
int stepDelay_us = 580;  // Reducir para mayor velocidad
```

### Modificación de Acordes

Editar el array `chordAngles` para programar nuevos acordes:

```cpp
int chordAngles[numChords][9] = {
  { 90, 90, 90, 0, 180, 90, 90, 90, 90 },  // Acorde 0
  // ... más acordes
};
```

## Monitoreo Serial

El sistema proporciona información de depuración a través del puerto serial (115200 baud):

- Eventos de homing
- Cambios de acordes
- Movimientos de rasgueo
- Secuencia de punteo

## Estructura del Código

```
GuitarBot.ino
├── Configuración de hardware (pines y constantes)
├── Definición de acordes
├── setup()
│   ├── Inicialización de servos
│   ├── Configuración de pines
│   └── Homing inicial
├── loop()
│   ├── Control de acordes
│   ├── Ejecución de rasgueo
│   └── Llamada a secuencia de punteo
└── Funciones auxiliares
    ├── stepMotorStrum() / stepMotorPick()
    ├── moveStepsStrum() / moveStepsPick()
    ├── performHomingStrum() / performHomingPick()
    └── executePunteo()
```

## Seguridad

- Los finales de carrera previenen movimientos fuera de rango
- El sistema ejecuta homing al inicio para garantizar posición conocida
- Los servos están limitados por software a sus rangos seguros de operación

## Licencia

Ver archivo LICENSE para más detalles.

## Video de Demostración

Ver `GuitarBot.mp4` para una demostración del sistema en funcionamiento.

## Contribuciones

Las contribuciones son bienvenidas. Por favor, sigue estas pautas:

- Mantener el estilo de código consistente
- Documentar cambios significativos
- Probar exhaustivamente antes de enviar pull requests

## Contacto

Para preguntas, sugerencias o reporte de problemas, por favor abre un issue en el repositorio.
