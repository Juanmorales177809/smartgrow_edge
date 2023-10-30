//la libreria bytes solo guardara los emoticones como atributos para ser accedidos desde el main


#include "EmoticonDisplay.h"




EmoticonDisplay::EmoticonDisplay(){
    
}

const byte* EmoticonDisplay::getCelsius(){
    return Celsius;
}

const byte* EmoticonDisplay::getSmiley(){
    return smiley;
}   

const byte* EmoticonDisplay::getFrownie(){
    return frownie;
}   

const byte* EmoticonDisplay::getEmail(){
    return email;
}
