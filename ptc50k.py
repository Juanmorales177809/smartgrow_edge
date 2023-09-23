"""Codigo para calibrar sensor de Temperatura del agua"""

import numpy as np
import matplotlib.pyplot as plt
# Datos de muestra

class Ajuste:
    def __init__(self, patron,arduino):
        self.patron = patron
        self.arduino = arduino
        self.n = len(self.patron)
        self.time = np.linspace(5,self.n*5, num=self.n)
        self.m =  self.pendiente()#Pendiente
        self.b = self.interseccion() #interseccion
        # inversa = np.linalg.inv(arduino.T@ arduino + (sigma**2)*lam)
        # return inversa@T.T@y
    
    def pendiente(self):
        m = np.sum((arduino - np.mean(arduino)) * (patron - np.mean(patron))) / np.sum((arduino - np.mean(arduino))**2)
        return m
    
    def interseccion(self):
        b = np.mean(patron) - self.m * np.mean(arduino)
        return b
    
    def calibrar_temperatura(self,lectura_arduino):
        return self.m * lectura_arduino + self.b
    
    def regresion_lineal(self):
        datos_calibrados = []
        for i in self.arduino:
            datos_calibrados.append(self.calibrar_temperatura(i))
        return datos_calibrados
    # def regresion_bayesiana(self):
    #     self.miu
    # def miu(T,sigma,lam,y): #Calculo de miu
    #     inversa = np.linalg.inv(T.T@ T + (sigma**2)*lam)
    #     return inversa@T.T@y
    # def sigma_su(T,sigma,lam,y): #Sigma mayuscula
    #     inversa = np.linalg.inv(T.T@ T + (sigma**2)*lam)
    #     return (sigma**2)*inversa
    # def bayesiana(T,miu,sigma_su): #estimación de parametros
    #     return T @ np.random.multivariate_normal(mean=miu, cov=sigma_su)   
   
   
   
    def plot_datos(self,datos_calibrados,x_label,y_label):
        fig, ax = plt.subplots(nrows=1, ncols=2)
        fig.set_size_inches(8,4)
        ax[0].plot(self.time,self.arduino,marker = 'o')
        ax[0].grid(color='0.95', linestyle='-', linewidth=2)
        ax[0].plot(self.time,self.patron,marker = 'o')
        ax[0].set_xlabel(x_label)
        ax[0].set_ylabel(y_label)
        ax[1].plot(self.time,datos_calibrados,marker = 'o')
        ax[1].grid(color='0.95', linestyle='-', linewidth=2)
        ax[1].plot(self.time,self.patron)
        ax[1].set_xlabel(x_label)
        ax[1].set_ylabel(y_label)
        plt.show()



if "__main__" == __name__:
    patron = np.array([69.64, 62.5, 57.2, 53.4, 49.5, 46.6, 44.5, 42.3, 40.5, 39, 37.6, 36.3, 35.4, 34.5, 33.7, 32.9, 32.3, 31.7, 31.2, 30.8, 30.4, 30])
    arduino = np.array([65.9, 56.12, 51.07, 47.6, 44.04, 41.17, 39.47, 37.44, 35.53, 34.52, 33.11, 32.04, 31.74, 30.04, 29.49, 28.57, 27.96, 27.51, 26.81, 26.35, 26.14, 25.45])
    diferencia = patron-arduino
    x_label = "Tiempo [m]"
    y_label = "Temperatura [°C]"
    calibrar = Ajuste(patron,arduino)
    datos = calibrar.regresion_lineal()
    calibrar.plot_datos(datos,x_label,y_label)
    print(diferencia)
    print(patron-datos)