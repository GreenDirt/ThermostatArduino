# ThermostatArduino

Projet dans le but de gérer automatiquement les différents paliers de brassage de la bière avec une gestion du temps défini.

Prérequis :
- 1 carte Arduino
- 1 sonde étanche DS18b20
- 1 relais 220V adapté à la puissance de votre résistance
- 1 résistance (type chauffe eau, pour ma part 3000W)

Le fichier .ino doit être téléversé sur la carte Arduino,
Il gère le contrôle de la température en activant / désactivant périodiquement la chauffe avec une forme de PID.

L'interface graphique est conçue pour Unity, permet la gestion temporelle de la chauffe en envoyant à Arduino la température voulue,
un suivi de la chauffe est effectué(affiche la température et l'état de la chauffe)
