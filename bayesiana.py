import pymc3 as pm
import numpy as np

# Datos de entrada
patron = np.array([69.64, 62.5, 57.2, 53.4, 49.5, 46.6, 44.5, 42.3, 40.5, 39, 37.6, 36.3, 35.4, 34.5, 33.7, 32.9, 32.3, 31.7, 31.2, 30.8, 30.4, 30])
esp = np.array([65.9, 56.12, 51.07, 47.6, 44.04, 41.17, 39.47, 37.44, 35.53, 34.52, 33.11, 32.04, 31.74, 30.04, 26.49, 28.57, 27.96, 27.51, 26.81, 26.35, 26.14, 25.45])
hora = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22])

# Crear modelo de regresión bayesiana
with pm.Model() as model:
    # Definir priori para los coeficientes
    alpha = pm.Normal('alpha', mu=0, sd=10)
    beta = pm.Normal('beta', mu=0, sd=10)
    sigma = pm.HalfCauchy('sigma', beta=10)
    
    # Modelo lineal
    mu = alpha + beta * hora
    
    # Definir la verosimilitud
    esp_obs = pm.Normal('esp_obs', mu=mu, sd=sigma, observed=esp)
    
    # Realizar muestreo de la distribución posterior
    trace = pm.sample(2000, tune=1000)
    
# Obtener los valores posteriores de los parámetros
alpha_post = trace['alpha'].mean()
beta_post = trace['beta'].mean()

# Calibrar los valores del sensor de Arduino
esp_calibrado = esp - (alpha_post + beta_post * hora)

# Imprimir los valores calibrados
print("Valores calibrados:")
print(esp_calibrado)