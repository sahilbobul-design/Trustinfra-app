import crypto from 'crypto';
import { ui } from './terminal-ui.js';

class Block {
    constructor(index, timestamp, contractData, sensorData, prevHash = '') {
        this.index = index;
        this.timestamp = timestamp;
        this.contract_data = contractData;
        this.sensor_data = sensorData;
        this.prevHash = prevHash;
        this.transaction_id = this.generateIndianTxID();
        this.status = this.calculateStatus();
        this.hash = this.calculateHash();
    }

    calculateHash() {
        return crypto.createHash('sha256')
            .update(this.index + this.prevHash + this.timestamp + JSON.stringify(this.contract_data) + JSON.stringify(this.sensor_data) + this.transaction_id)
            .digest('hex');
    }

    generateIndianTxID() {
        const dataStr = JSON.stringify(this.contract_data) + JSON.stringify(this.sensor_data) + this.timestamp;
        const shortHash = crypto.createHash('sha256').update(dataStr).digest('hex').substring(0, 12).toUpperCase();
        return `IND-${this.timestamp}-${shortHash}`;
    }

    calculateStatus() {
        // For the demo/hackathon, we simulate a successful match
        return 'RELEASED';
    }
}

export class Blockchain {
    constructor() {
        this.chain = [this.createGenesisBlock()];
        this.wallet = {
            balance: 1000000, // Initial 10 Lakh INR
            gasSpent: 0,
            transactions: 0
        };
    }

    createGenesisBlock() {
        return new Block(0, Date.now(), "Genesis", "Genesis", "0");
    }

    getLatestBlock() {
        return this.chain[this.chain.length - 1];
    }

    async addBlock(contractData, sensorData, gas = 0) {
        const newBlock = new Block(
            this.chain.length,
            Date.now(),
            contractData,
            sensorData,
            this.getLatestBlock().hash
        );

        this.chain.push(newBlock);

        // Update wallet
        this.wallet.transactions++;
        this.wallet.gasSpent += parseFloat(gas);

        if (newBlock.status === 'RELEASED') {
            this.wallet.balance -= (5000 + parseFloat(gas)); // Simulated logic
        }

        return newBlock;
    }

    getWallet() {
        return this.wallet;
    }

    getChain() {
        return this.chain;
    }

    isChainValid() {
        for (let i = 1; i < this.chain.length; i++) {
            const currentBlock = this.chain[i];
            const prevBlock = this.chain[i - 1];

            if (currentBlock.hash !== currentBlock.calculateHash()) return false;
            if (currentBlock.prevHash !== prevBlock.hash) return false;
        }
        return true;
    }
}
