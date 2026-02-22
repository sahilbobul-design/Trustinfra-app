import { ui } from './terminal-ui.js';
import crypto from 'crypto';

export class Interpreter {
    constructor(blockchain) {
        this.blockchain = blockchain;
        this.state = {
            contract: 'N/A',
            contractor: 'N/A',
            work: 'N/A',
            qty: '0',
            sensor: { moisture: '0', ultrasonic: '0', temperature: '0' },
            hashes: {},
            lastTx: null,
            latestGas: 0
        };
    }

    async run(input) {
        const lines = input.split('\n');
        for (let line of lines) {
            line = line.trim();
            if (!line || line.startsWith('#')) continue;

            const parts = line.split(/\s+/);
            const cmd = parts[0].toUpperCase();
            const args = parts.slice(1);

            switch (cmd) {
                case 'CONTRACT':
                    this.state.contract = args[0];
                    break;
                case 'CONTRACTOR':
                    this.state.contractor = args[0];
                    break;
                case 'REGISTER_WORK':
                    this.state.work = args[0];
                    this.state.qty = args[1] || '0';
                    break;
                case 'SENSOR_DATA':
                    this.state.sensor = {
                        moisture: args[0],
                        ultrasonic: args[1],
                        temperature: args[2]
                    };
                    break;
                case 'HASH_DETAIL':
                    const key = args[0];
                    const val = args[1];
                    // We mock the hex display as per requirements
                    this.state.hashes[key] = crypto.createHash('sha256').update(val || '').digest('hex').substring(0, 12);
                    break;
                case 'VALIDATE':
                    // In a real system, we'd compare hashes here. 
                    // For the demo, we assume success as per user requirements.
                    break;
                case 'GENERATE_TX':
                    const contractData = { id: this.state.contract, contractor: this.state.contractor, work: this.state.work, qty: this.state.qty };
                    const sensorData = this.state.sensor;
                    this.state.lastTx = await this.blockchain.addBlock(contractData, sensorData, this.state.latestGas);
                    break;
                case 'CALCULATE_GAS':
                    this.state.latestGas = args[0];
                    break;
                case 'DISPLAY':
                    ui.display({
                        contract: this.state.contract,
                        contractor: this.state.contractor,
                        work: this.state.work,
                        qty: this.state.qty,
                        sensor: this.state.sensor,
                        hashes: this.state.hashes,
                        tx: {
                            id: this.state.lastTx?.transaction_id || 'N/A',
                            gas: this.state.latestGas,
                            status: this.state.lastTx?.status || 'LOCKED'
                        }
                    });
                    break;
            }
        }
    }
}
