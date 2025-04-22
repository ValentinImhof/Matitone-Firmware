import asyncio
from bleak import BleakScanner, BleakClient

# UUIDs de votre Arduino
SERVICE_UUID = "19B10000-E8F2-537E-4F6C-D104768A1214"
CHARACTERISTIC_UUID = "19B10001-E8F2-537E-4F6C-D104768A1214"
DEVICE_NAME = "Nano33BLE"  # Nom défini dans votre code Arduino

async def scan_and_connect():
    try:
        print("🔍 Recherche de périphériques BLE...")
        devices = await BleakScanner.discover()
        arduino_address = None
        for device in devices:
            if device.name and DEVICE_NAME in device.name:
                arduino_address = device.address
                print(f"✅ Périphérique trouvé : {device.name} ({device.address})")
                break

        if not arduino_address:
            print("❌ Aucun périphérique Arduino trouvé !")
            return

        async with BleakClient(arduino_address, timeout=10.0) as client:
            print("🔗 Connecté à l'Arduino BLE !")
            while True:
                data = await client.read_gatt_char(CHARACTERISTIC_UUID)  # Lecture manuelle
                print(f"📩 Message reçu : {data.decode()}")
                await asyncio.sleep(1)

    except Exception as e:
        print(f"❌ Erreur : {e}")

asyncio.run(scan_and_connect())
