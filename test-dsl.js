import fetch from 'node-fetch';

const url = 'http://localhost:3000/contract';

const samples = [
    {
        name: 'Matching Work & Sensor (Payment RELEASED)',
        dsl: 'CONTRACT CT-101 CONTRACTOR Rajesh REG_WORK BUILD-001 SENSOR_DATA BUILD-001 VALIDATE GENERATE_TX'
    },
    {
        name: 'Mismatched Work & Sensor (Payment LOCKED)',
        dsl: 'CONTRACT CT-102 CONTRACTOR Suresh REG_WORK BUILD-002 SENSOR_DATA ERROR-404 VALIDATE'
    }
];

async function runTests() {
    for (const sample of samples) {
        console.log(`\nTesting: ${sample.name}`);
        try {
            const res = await fetch(url, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ dsl: sample.dsl })
            });
            const data = await res.json();
            console.log('Response Status:', res.status);
            // console.log('Data:', JSON.stringify(data, null, 2));
        } catch (err) {
            console.error('Test Failed:', err.message);
        }
    }
}

runTests();
