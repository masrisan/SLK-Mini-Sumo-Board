
#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];


// =====================================================
// CONTROLLER CONNECTED
// =====================================================

void onConnectedController(ControllerPtr ctl) {

    Serial.println();
    Serial.println("========================================");
    Serial.println("       CONTROLLER CONNECTED");
    Serial.println("========================================");

    Serial.printf(
        "Index : %d\n",
        ctl->index()
    );

    Serial.printf(
        "Model : %s\n",
        ctl->getModelName().c_str()
    );


    // Get controller properties

    ControllerProperties properties =
        ctl->getProperties();


    Serial.printf(
        "VID   : 0x%04X\n",
        properties.vendor_id
    );

    Serial.printf(
        "PID   : 0x%04X\n",
        properties.product_id
    );


    // -------------------------------------------------
    // PRINT BLUETOOTH ADDRESS
    // -------------------------------------------------

    Serial.print("BT Address: ");

    for (int i = 0; i < 6; i++) {

        if (i > 0) {
            Serial.print(":");
        }

        Serial.printf(
            "%02X",
            properties.btaddr[i]
        );
    }

    Serial.println();


    Serial.println("----------------------------------------");
    Serial.println("Copy the BT Address above.");
    Serial.println("----------------------------------------");
}


// =====================================================
// CONTROLLER DISCONNECTED
// =====================================================

void onDisconnectedController(ControllerPtr ctl) {

    Serial.println();
    Serial.println("========================================");
    Serial.println("       CONTROLLER DISCONNECTED");
    Serial.println("========================================");

    Serial.printf(
        "Index: %d\n",
        ctl->index()
    );

    Serial.println();
}


// =====================================================
// SETUP
// =====================================================

void setup() {

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("   SLK MINI SUMO - BT MAC CHECKER");
    Serial.println("   T3 X3 + Bluepad32");
    Serial.println("========================================");
    Serial.println();

    Serial.println(
        "Waiting for Bluetooth controller..."
    );


    // -------------------------------------------------
    // BLUEPAD32 SETUP
    // -------------------------------------------------

    Serial.printf(
        "Bluepad32 Firmware: %s\n",
        BP32.firmwareVersion()
    );


    const uint8_t* addr =
        BP32.localBdAddress();


    Serial.printf(
        "ESP32 BT Address: "
        "%02X:%02X:%02X:%02X:%02X:%02X:%02X\n",

        addr[0],
        addr[1],
        addr[2],
        addr[3],
        addr[4],
        addr[5]
    );


    // Register callbacks

    BP32.setup(
        &onConnectedController,
        &onDisconnectedController
    );


    // Don't forget Bluetooth keys

    BP32.enableVirtualDevice(false);
}


// =====================================================
// LOOP
// =====================================================

void loop() {

    BP32.update();

    delay(10);
}


