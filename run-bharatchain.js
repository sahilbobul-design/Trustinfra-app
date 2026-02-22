import { Blockchain } from './blockchain.js';
import { Interpreter } from './interpreter.js';

const dslScript = `# ---------------- Contracts ----------------
CONTRACT BRIDGE001
CONTRACTOR ABC_CONTRACTOR

CONTRACT BRIDGE002
CONTRACTOR XYZ_CONTRACTOR

CONTRACT BRIDGE003
CONTRACTOR LMN_CONTRACTOR

# --------------- Daily Work ----------------
REGISTER_WORK Layer1Bricks 100
REGISTER_WORK Layer2Bricks 150
REGISTER_WORK Layer3CementBags 50

REGISTER_WORK Layer1Bricks 120
REGISTER_WORK Layer2Bricks 130
REGISTER_WORK Layer3CementBags 60

REGISTER_WORK Layer1Bricks 110
REGISTER_WORK Layer2Bricks 140
REGISTER_WORK Layer3CementBags 55

# ------------- IoT / Sensor Data -------------
SENSOR_DATA 12 0.85 30
SENSOR_DATA 14 0.88 31
SENSOR_DATA 11 0.80 29

SENSOR_DATA 15 0.90 32
SENSOR_DATA 13 0.87 30
SENSOR_DATA 12 0.84 28

SENSOR_DATA 14 0.89 31
SENSOR_DATA 12 0.85 30
SENSOR_DATA 13 0.86 29

# ------------- Hash Details ----------------
HASH_DETAIL ContractID BRIDGE001
HASH_DETAIL ContractorName ABC_CONTRACTOR
HASH_DETAIL Work Layer1Bricks
HASH_DETAIL Quantity 100
HASH_DETAIL Moisture 12
HASH_DETAIL Ultrasonic 0.85
HASH_DETAIL Temperature 30

HASH_DETAIL ContractID BRIDGE002
HASH_DETAIL ContractorName XYZ_CONTRACTOR
HASH_DETAIL Work Layer2Bricks
HASH_DETAIL Quantity 150
HASH_DETAIL Moisture 14
HASH_DETAIL Ultrasonic 0.88
HASH_DETAIL Temperature 31

HASH_DETAIL ContractID BRIDGE003
HASH_DETAIL ContractorName LMN_CONTRACTOR
HASH_DETAIL Work Layer3CementBags
HASH_DETAIL Quantity 50
HASH_DETAIL Moisture 11
HASH_DETAIL Ultrasonic 0.80
HASH_DETAIL Temperature 29

HASH_DETAIL ContractID BRIDGE001
HASH_DETAIL ContractorName ABC_CONTRACTOR
HASH_DETAIL Work Layer1Bricks
HASH_DETAIL Quantity 120
HASH_DETAIL Moisture 15
HASH_DETAIL Ultrasonic 0.90
HASH_DETAIL Temperature 32

HASH_DETAIL ContractID BRIDGE002
HASH_DETAIL ContractorName XYZ_CONTRACTOR
HASH_DETAIL Work Layer2Bricks
HASH_DETAIL Quantity 130
HASH_DETAIL Moisture 13
HASH_DETAIL Ultrasonic 0.87
HASH_DETAIL Temperature 30

HASH_DETAIL ContractID BRIDGE003
HASH_DETAIL ContractorName LMN_CONTRACTOR
HASH_DETAIL Work Layer3CementBags
HASH_DETAIL Quantity 60
HASH_DETAIL Moisture 12
HASH_DETAIL Ultrasonic 0.84
HASH_DETAIL Temperature 28

HASH_DETAIL ContractID BRIDGE001
HASH_DETAIL ContractorName ABC_CONTRACTOR
HASH_DETAIL Work Layer1Bricks
HASH_DETAIL Quantity 110
HASH_DETAIL Moisture 14
HASH_DETAIL Ultrasonic 0.89
HASH_DETAIL Temperature 31

HASH_DETAIL ContractID BRIDGE002
HASH_DETAIL ContractorName XYZ_CONTRACTOR
HASH_DETAIL Work Layer2Bricks
HASH_DETAIL Quantity 140
HASH_DETAIL Moisture 12
HASH_DETAIL Ultrasonic 0.85
HASH_DETAIL Temperature 30

HASH_DETAIL ContractID BRIDGE003
HASH_DETAIL ContractorName LMN_CONTRACTOR
HASH_DETAIL Work Layer3CementBags
HASH_DETAIL Quantity 55
HASH_DETAIL Moisture 13
HASH_DETAIL Ultrasonic 0.86
HASH_DETAIL Temperature 29

# ------------- Validate & Transaction -------------
VALIDATE
GENERATE_TX
CALCULATE_GAS 0.85
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 1.32
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.65
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.85
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.90
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.75
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.95
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.82
DISPLAY

VALIDATE
GENERATE_TX
CALCULATE_GAS 0.88
DISPLAY

# ---------------- Latest Layer4 Steel Work ----------------
REGISTER_WORK Layer4Steel 30
SENSOR_DATA 16 0.92 33
HASH_DETAIL ContractID BRIDGE001
HASH_DETAIL ContractorName ABC_CONTRACTOR
HASH_DETAIL Work Layer4Steel
HASH_DETAIL Quantity 30
HASH_DETAIL Moisture 16
HASH_DETAIL Ultrasonic 0.92
HASH_DETAIL Temperature 33

VALIDATE
GENERATE_TX
CALCULATE_GAS 1.05
DISPLAY`;

async function main() {
    const bc = new Blockchain();
    const interpreter = new Interpreter(bc);

    console.log('--- BharatChain Runtime Interpreter ---');
    await interpreter.run(dslScript);
    console.log('--- Real-time Deployment Success ---');
    console.log(`Final Wallet Balance: ₹${bc.getWallet().balance}`);
    console.log(`Total Gas Spent: ₹${bc.getWallet().gasSpent}`);
}

main().catch(console.error);
