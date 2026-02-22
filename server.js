import express from 'express';
import cors from 'cors';
import { Blockchain } from './blockchain.js';
import { Interpreter } from './interpreter.js';
import { ui } from './terminal-ui.js';

const app = express();
const port = 3000;

app.use(cors());
app.use(express.json());

const blockchain = new Blockchain();
const interpreter = new Interpreter();

process.on('unhandledRejection', (reason, promise) => {
    ui.error(`Unhandled Rejection: ${reason}`);
});

process.on('uncaughtException', (err) => {
    ui.error(`Uncaught Exception: ${err.message}`);
    process.exit(1);
});

app.use((req, res, next) => {
    ui.info(`${req.method} ${req.url}`);
    next();
});

app.get('/', (req, res) => {
    res.send(`
        <div style="font-family: sans-serif; text-align: center; padding: 50px; background: #0f172a; color: #38bdf8; min-height: 100vh;">
            <h1>🇮🇳 Bharat Chain Backend Ready</h1>
            <p style="color: #94a3b8;">Hackanovation 2.0 Prototype</p>
            <div style="background: #1e293b; padding: 20px; border-radius: 12px; display: inline-block; margin-top: 20px; border: 1px solid #334155;">
                <p><strong>Status:</strong> Active ✅</p>
                <p><strong>Endpoints:</strong> /contract, /blockchain, /wallet</p>
            </div>
        </div>
    `);
});

app.post('/contract', async (req, res) => {
    const { dsl } = req.body;
    if (!dsl) return res.status(400).json({ error: 'Missing DSL input' });

    try {
        const parsed = interpreter.interpret(dsl);
        ui.contract({ id: parsed.contract, contractor: parsed.contractor });
        if (parsed.sensor) ui.sensor(parsed.sensor);

        if (parsed.actions.includes('VALIDATE') || parsed.actions.includes('GENERATE_TX')) {
            const block = await blockchain.addBlock(parsed.work, parsed.sensor);
            return res.json({ message: 'Success', block, wallet: blockchain.wallet });
        }
        res.json({ parsed });
    } catch (err) {
        ui.error(`Error: ${err.message}`);
        res.status(500).json({ error: err.message });
    }
});

app.get('/blockchain', (req, res) => res.json(blockchain.chain));
app.get('/wallet', (req, res) => res.json(blockchain.wallet));

app.use((req, res) => {
    ui.error(`404: ${req.url}`);
    res.status(404).json({ error: 'Not Found' });
});

app.listen(port, '0.0.0.0', () => {
    console.clear();
    ui.success(`Bharat Chain Backend running at http://localhost:${port}`);
});
