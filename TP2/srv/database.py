"""
Fichier : database.py
-> Créer la base de données SQLite et charger les données depuis un fichier CSV.
Auteurs : Grégoire MAHON, Armand LELONG
Promo EI2I 4 II Groupe B
"""

import sqlite3
import csv

def create_table(db_file):
    """Créer la table wifi_networks dans la base de données SQLite."""
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()

    cursor.execute('DROP TABLE IF EXISTS wifi_networks')
    cursor.execute('''
    CREATE TABLE wifi_networks (
        id INTEGER PRIMARY KEY,
        ssid TEXT NOT NULL,
        rssi INTEGER NOT NULL,
        bssid TEXT NOT NULL,
        latitude REAL NOT NULL,
        longitude REAL NOT NULL
    )
    ''')
    conn.commit()
    conn.close()

def load_data_from_csv(db_file, csv_file):
    """Charger les données depuis un fichier CSV dans la base de données SQLite."""
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    
    with open(csv_file, newline='', encoding='utf-8') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            
            cursor.execute('''
                INSERT INTO wifi_networks (ssid, rssi, bssid, latitude, longitude) 
                VALUES (?, ?, ?, ?, ?)
            ''', (row['SSID'], row['RSSI'], row['MAC'], row['CurrentLatitude'], row['CurrentLongitude']))

    conn.commit()
    conn.close()

db_file = 'wifi_networks.db'
csv_file = 'toLoad.csv'

create_table(db_file)
load_data_from_csv(db_file, csv_file)

print(f"Base de données créée à l'emplacement : {db_file}")
