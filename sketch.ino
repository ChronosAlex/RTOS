#include <Arduino_FreeRTOS.h>
#include "queue.h"
#include "semphr.h"

#define POTENTIOMETRE A0 
#define BOUTON1 5 
#define BOUTON2 6 

struct valeurCapteurs {
  int analogique;
  int numerique;
  double tempsEnMillisecondes;
};

// Déclaration du semaphore
SemaphoreHandle_t xSerialSemaphore = NULL;

QueueHandle_t PotentiometreQueue; //queue envoyant la donnée de la tâche 1
QueueHandle_t BoutonQueue; //queue envoyant la donnée  de la tâche 2
QueueHandle_t RRQueue; //queue envoyant la donnée de la tâche 3
QueueHandle_t ASQueue; //queue envoyant la donnée  de la tâche 4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Initialisation du port série

  while (!Serial);

  if ( xSerialSemaphore == NULL ) 
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) ); 
  }

  PotentiometreQueue = xQueueCreate(1,sizeof(int));
  BoutonQueue = xQueueCreate(1,sizeof(int));
  RRQueue = xQueueCreate(1,sizeof(valeurCapteurs)); 
  ASQueue = xQueueCreate(1,sizeof(valeurCapteurs)); 

  xTaskCreate(Task_Potentiometre,"Valeur_Potentiometre",100,NULL,1,NULL); //première tâche
  xTaskCreate(Task_Bouton,"Valeur_Bouton",100,NULL,1,NULL); //deuxième tâche
  xTaskCreate(Task_RR,"Receveur_Renvoyeur",100,NULL,1,NULL); //troisième tâche
  xTaskCreate(Task_AS,"Afficheur_Serie",100,NULL,1,NULL); //quatrième tâche
  xTaskCreate(Task_AT,"Afficheur_Temps",100,NULL,1,NULL); //cinquième tâche 
}

void Task_Potentiometre(void *pvParameters) // tache 1
{ 
  pinMode(POTENTIOMETRE,INPUT); //broche du potentiomètre en entrée
  int valeur_potentiometre;
  while(1)
  {
    valeur_potentiometre = analogRead(POTENTIOMETRE); //valeur du potentiomètre
    xQueueSend( PotentiometreQueue, &valeur_potentiometre,0 );// on envoie la donnée dans la queue
    vTaskDelay(200);
  }
}

void Task_Bouton(void *pvParameters) // tache 2
{
  pinMode(BOUTON1, INPUT); // broche du bouton 1 en entrée
  pinMode(BOUTON2, INPUT); // broche du bouton 2 en entrée
  int valeur_bouton;
  valeur_bouton =(int)pvParameters;
  while(1)
  {
    valeur_bouton = digitalRead(BOUTON1); // Lecture de l'état du bouton 1
    valeur_bouton = valeur_bouton + digitalRead(BOUTON2); // Lecture de l'état du bouton 2 et ajout du bouton 2
    xQueueSend(BoutonQueue, &valeur_bouton,0 );  // on envoie la donnée dans la queue
    vTaskDelay(200);
  }
}

void Task_RR(void *pvParameters) // tache 3 
{
   int val_analog;
   int val_num;
   valeurCapteurs capteur;
   while(1){
    xQueueReceive(PotentiometreQueue, &val_analog, 0);// on reçoit la donnée de la queue PotentiometreQueue
    xQueueReceive(BoutonQueue, &val_num, 0);// on reçoit la donnée de la queue BoutonQueue
    capteur.analogique = val_analog;
    capteur.numerique = val_num;
    capteur.tempsEnMillisecondes = millis();
    xQueueSend(RRQueue, &capteur, 0);// on envoie la donnée dans la queue
    vTaskDelay(200);
  }
}

void Task_AS(void *pvParameters) // tache 4
{
  valeurCapteurs capteur;
  while(1){
    xQueueReceive(RRQueue, &capteur, 0); // on reçoit la donnée de la queue RRQueue
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ) //utilise le port série pour afficher le contenu de la queue RRQueue
    {
      Serial.print("Valeur Analogique : ");
      Serial.print(capteur.analogique);
      Serial.print(" Valeur Numerique : ");
      Serial.print(capteur.numerique);
      Serial.print(" Millis : ");
      Serial.print(capteur.tempsEnMillisecondes);
      Serial.print("\n");
      xSemaphoreGive( xSerialSemaphore );
    }
    xQueueSend(ASQueue, &capteur, 0);// on envoie la donnée dans la queue

    vTaskDelay(200);
  }
}

void Task_AT(void *pvParameters) // tache 5
{
 valeurCapteurs capteur;
 valeurCapteurs capteurbis;
  while(1)
  {
    xQueueReceive(ASQueue,&capteur,0);// on reçoit la donnée de la ASQueue
    capteurbis = {capteur.analogique,capteur.numerique,capteur.tempsEnMillisecondes/60000};
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )//utilise le port série pour afficher le contenu de la queue ASQueue
    {
      Serial.print("Nouvelle structure : ");
      Serial.print(capteurbis.analogique);
      Serial.print(" , ");
      Serial.print(capteurbis.numerique);
      Serial.print(" , ");
      Serial.println(capteurbis.tempsEnMillisecondes);
      xSemaphoreGive( xSerialSemaphore );
    }
    vTaskDelay( 200  );
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}