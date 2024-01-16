# Cours sur HTTP, REST, MQTT et CoAP

## HTTP et RESTful API
- **HTTP** : Protocole client-serveur, utilisé pour les requêtes Web.
- **REST (Representational State Transfer)**: Style d'architecture basé sur HTTP.
- Utilisation des méthodes HTTP : `GET`, `PUT`, `POST`, `DELETE`.
- Avantage : Standardisé et largement utilisé.
- Inconvénient : Verbosité et forte consommation de bande passante.

## MQTT (Message Queue Telemetry Transport)
- Protocole de messagerie léger et ouvert.
- Conçu pour les communications M2M (Machine to Machine) dans les réseaux IoT.
- Architecture de type publish/subscribe.
- **QoS (Quality of Service)** : Différents niveaux de garantie de livraison des messages.
- Utilisation sur TCP, possibilité de sécurisation via SSL/TLS.

## CoAP (Constrained Application Protocol)
- Protocole de transfert pour environnements contraints.
- Basé sur UDP, optimisé pour les réseaux de capteurs sans fil.
- Modèle client/serveur similaire à HTTP mais plus léger.
- **DTLS (Datagram Transport Layer Security)** pour la sécurité.
- Gestion de la qualité de service avec des messages confirmables.

## Comparaison et Utilisation Pratique
- **HTTP vs MQTT** : Choix basé sur les besoins de bande passante, la latence, et la complexité.
- **CoAP vs MQTT** : CoAP pour les dispositifs avec des ressources limitées, MQTT pour une meilleure garantie de livraison des messages.
