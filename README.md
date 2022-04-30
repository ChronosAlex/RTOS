# RTOS
arduino exo

Nous avions plusieurs tâches à effectuer, récupérer les valeurs d'un potentiomètre analogique et de deux boutons numériques puis les envoyer dans des queues. 
<br />La troisième tache combine les valeurs des queues précédentes, y ajoute un temps dans une structure puis envoie cette donnée dans une nouvelle queue.
<br />La tâche 4 reçoit la valeur de la structure et utilise le port série pour l’afficher, ensuite envoie cette structure à la tâche 5
<br />La tâche 5 transforme la valeur du temps dans la structure en minutes, ensuite elle doit afficher cette nouvelle structure à travers le port série.
<br />Nous avons utilisé les sémaphores permettant d'empécher la modification de la variable par un autre processus et permettant un bon partage des ressources.
