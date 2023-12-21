from flask import Flask, request, jsonify
import json
import sqlite3
from statistics import mean

app = Flask(__name__)

db_file = 'wifi_networks.db'

def get_closest_location(bssid, rssi):
    """Trouver la position la plus proche pour un BSSID donné et une valeur RSSI."""
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    # Prend la valeur de RSSI la plus forte pour l'addresse mac donnée
    cursor.execute('''
        SELECT latitude, longitude, ABS(rssi - ?) as rssi_diff 
        FROM wifi_networks 
        WHERE bssid = ? 
        ORDER BY rssi_diff ASC 
        LIMIT 1
    ''', (rssi, bssid))

    row = cursor.fetchone()
    conn.close()

    if row:
        return (row[0], row[1])
    else:
        return None

def get_rssi_weighted_positions(bssid, rssi):
    """Récupérer les positions pondérées par RSSI pour un BSSID donné."""
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute('SELECT latitude, longitude, rssi FROM wifi_networks WHERE bssid = ?', (bssid,))

    weighted_positions = []
    for row in cursor.fetchall():
        db_lat, db_lon, db_rssi = row
        rssi_diff = abs(db_rssi - rssi)
        weight = 1 / (rssi_diff + 1)  # Éviter la division par zéro
        weighted_positions.append((db_lat * weight, db_lon * weight, weight))

    conn.close()
    return weighted_positions

def locate_device(wifi_networks):
    """Calculer la position moyenne pondérée en fonction des valeurs RSSI des réseaux WiFi."""
    all_weighted_positions = []
    total_weight = 0

    for network in wifi_networks:
        bssid = network.get('bssid')
        rssi = network.get('rssi')
        weighted_positions = get_rssi_weighted_positions(bssid, rssi)
        # Dans le cas ou aucune position n'a été retournée à partir d'une adresse mac et d'un rssi : 
        if len(weighted_positions) == 0:
            # Essaie de récupérer les positions pondérées avec une adresse mac en minuscule
            weighted_positions = get_rssi_weighted_positions(bssid.lower(), rssi)
        #weighted_positions = get_rssi_weighted_positions(bssid.lower(), rssi)
        all_weighted_positions.extend(weighted_positions)

    for pos in all_weighted_positions:
        total_weight += pos[2]

    if total_weight > 0:
        avg_lat = sum(pos[0] for pos in all_weighted_positions) / total_weight
        avg_lon = sum(pos[1] for pos in all_weighted_positions) / total_weight
        return (avg_lat, avg_lon)
    else:
        return (0, 0)


@app.route('/get_location', methods=['GET'])
def get_location():

    raw_data = request.get_data(as_text=True)
    try:
        wifi_networks = json.loads(raw_data)
    except json.JSONDecodeError:
        return jsonify({"error": "Invalid JSON format"}), 400

    (current_latitude, current_longitude) = locate_device(wifi_networks)

    formatted_data = {
        "name": "Device",
        "lat": current_latitude,
        "lon": current_longitude,
    }
    return jsonify(formatted_data)

if __name__ == '__main__':
    app.run(debug=True)
