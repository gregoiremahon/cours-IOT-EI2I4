"""
Fichier : http-server.py
-> Serveur HTTP Flask pour récupérer la position d'un appareil à partir de ses réseaux WiFi.
Auteurs : Grégoire MAHON, Armand LELONG
Promo EI2I 4 II Groupe B
"""

from flask import Flask, request, jsonify
import json
import sqlite3
from statistics import mean

# Initialisation de l'application Flask
app = Flask(__name__)

# Chemin vers le fichier de la base de données SQLite contenant les données des réseaux WiFi
db_file = 'wifi_networks.db'


def get_closest_location(bssid, rssi):
    """
    Trouver la position la plus proche pour un BSSID donné et une valeur RSSI.
    """
    # Connexion à la base de données SQLite
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    
    # requête SQL pour trouver la position la plus proche
    cursor.execute('''
        SELECT latitude, longitude, ABS(rssi - ?) as rssi_diff 
        FROM wifi_networks 
        WHERE bssid = ? 
        ORDER BY rssi_diff ASC 
        LIMIT 1
    ''', (rssi, bssid))

    row = cursor.fetchone()
    conn.close()

    # Retourne les coordonnées si une correspondance est trouvée, sinon None
    return (row[0], row[1]) if row else None


def get_rssi_weighted_positions(bssid, rssi):
    """
    Récupérer les positions pondérées par RSSI pour un BSSID donné.
    """
    # Connexion et interrogation de la base de données, poour récupérer les positions et le rssi à partir du bssid.
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute('SELECT latitude, longitude, rssi FROM wifi_networks WHERE bssid = ?', (bssid,))

    weighted_positions = []
    # Calcul des positions pondérées
    for row in cursor.fetchall():
        db_lat, db_lon, db_rssi = row
        rssi_diff = abs(db_rssi - rssi)
        weight = 1 / (rssi_diff + 1)  # Pour éviter la division par zéro
        weighted_positions.append((db_lat * weight, db_lon * weight, weight))

    conn.close()
    return weighted_positions

# Fonction pour calculer la position moyenne pondérée
def locate_device(wifi_networks):
    """
    Calculer la position moyenne pondérée en fonction des valeurs RSSI des réseaux WiFi.
    """
    all_weighted_positions = []
    total_weight = 0

    # Traitement de chaque réseau WiFi
    for network in wifi_networks:
        bssid = network.get('bssid')
        rssi = network.get('rssi')
        weighted_positions = get_rssi_weighted_positions(bssid, rssi)
        all_weighted_positions.extend(weighted_positions)

    # Calcul de la position moyenne pondérée
    for pos in all_weighted_positions:
        total_weight += pos[2]

    if total_weight > 0:
        avg_lat = sum(pos[0] for pos in all_weighted_positions) / total_weight
        avg_lon = sum(pos[1] for pos in all_weighted_positions) / total_weight
        return (avg_lat, avg_lon)
    else:
        return (0, 0) # Si la somme des poids est nulle, retourne la position (0, 0)

# Définition de la route pour la récupération de la position
@app.route('/get_location', methods=['GET'])
def get_location():
    # Récupération des données JSON de la requête
    raw_data = request.get_data(as_text=True)
    try:
        wifi_networks = json.loads(raw_data)
    except json.JSONDecodeError:
        return jsonify({"error": "Invalid JSON format"}), 400

    # Calcul de la position actuelle et formatage des données pour la réponse à node-red
    (current_latitude, current_longitude) = locate_device(wifi_networks)
    formatted_data = {
        "name": "Device",
        "lat": current_latitude,
        "lon": current_longitude,
    }
    return jsonify(formatted_data)

if __name__ == '__main__':
    app.run(debug=True)
