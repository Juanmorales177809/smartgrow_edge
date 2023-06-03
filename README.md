Para calibrar el sensor de Arduino y obtener una ecuación que ajuste los datos a los valores del patrón, se utiliza el ajuste de curva por medio de regresión lineal que encuentra la relación entre los valores del sensor de Arduino y los valores del patrón.

Para calcular la pendiente (m) y la intersección en el eje (b) se utiliza la fórmula:

\[
m = \frac{{\sum (x_i - \bar{x})}}{{\sum (x_i - \bar{x})^2}} \quad [1]
\]

\[b = \bar{y} - m\bar{x} \quad [2]
\]

Donde:
\(x_i\): Valores del sensor
\(y_i\): Valores del patrón
\(\bar{x}\): Promedio de los valores del sensor.
\(\bar{y}\): Promedio de los valores del patrón.

La ecuación [3] para la calibración será de la forma:

\[
\text{calibrado} = m \cdot \text{sensor} + b \quad [3]
\]