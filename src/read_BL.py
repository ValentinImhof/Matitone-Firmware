import asyncio
from bleak import BleakScanner, BleakClient
from functools import partial # Pour passer des arguments supplémentaires au handler

SERVICE_UUID = "19B10000-E8F2-537E-4F6C-D104768A1214"
DATA_CHARACTERISTIC_UUID = "19B10001-E8F2-537E-4F6C-D104768A1214"  # Pour "Button1", "AR2UP", etc.
TRIGGER_CHARACTERISTIC_UUID = "19B10002-E8F2-537E-4F6C-D104768A1214" # Pour les notifications de déclenchement
DEVICE_NAME = "Nano33BLE"

async def trigger_notification_handler(client: BleakClient, sender_handle: int, data: bytearray):
    """
    Callback pour les notifications de la caractéristique TRIGGER.
    Lit ensuite la caractéristique DATA pour obtenir l'instruction réelle.
    """
    
    try:
        # Lire la caractéristique de données qui contient l'instruction réelle ("Button1", etc.)
        actual_instruction_bytes = await client.read_gatt_char(DATA_CHARACTERISTIC_UUID)
        instruction = actual_instruction_bytes.decode()
        
        print(f"Instruction reçue: \"{instruction}\"")
        
    except Exception as e:
        print(f"❌ Erreur lors de la lecture de la caractéristique de données après trigger: {e}")


async def scan_and_connect():
    try:
        print("🔍 Recherche de périphériques BLE...")
        device = await BleakScanner.find_device_by_name(DEVICE_NAME, timeout=20.0) # Augmentation du timeout
        
        if not device:
            print(f"❌ Aucun périphérique '{DEVICE_NAME}' trouvé !")
            return

        print(f"✅ Périphérique trouvé : {device.name} ({device.address})")

        async with BleakClient(device.address, timeout=20.0) as client: # Augmentation du timeout
            print(f"🔗 Connecté à {device.name}!")
            
            # Créer un handler lié qui a accès au 'client'
            # 'partial' permet de "pré-remplir" le premier argument 'client' de trigger_notification_handler
            bound_trigger_handler = partial(trigger_notification_handler, client)
            
            await client.start_notify(TRIGGER_CHARACTERISTIC_UUID, bound_trigger_handler)
            
            print("✅ Notifications activées pour TRIGGER. En attente d'événements...")
            
            # Garder le script en exécution pour recevoir les notifications
            while True:
                if not client.is_connected: # Vérifier si le client est toujours connecté
                    print("Client déconnecté.")
                    break
                await asyncio.sleep(1) # Boucle pour maintenir la connexion active et permettre aux tâches asynchrones de s'exécuter

    except Exception as e:
        print(f"❌ Erreur globale : {e}")
    finally:
        print("Script terminé.")

asyncio.run(scan_and_connect())