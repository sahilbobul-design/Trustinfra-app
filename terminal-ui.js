import chalk from 'chalk';
import boxen from 'boxen';
import ora from 'ora';

const boxOptions = {
    padding: 0.5,
    margin: 1,
    borderStyle: 'round',
    borderColor: 'yellow',
    titleAlignment: 'center'
};

export const ui = {
    info: (msg) => console.log(chalk.blue('ℹ ') + chalk.blue(msg)),
    success: (msg) => console.log(chalk.green('✅ ') + chalk.green(msg)),
    error: (msg) => console.log(chalk.red('❌ ') + chalk.red(msg)),
    warning: (msg) => console.log(chalk.yellow('⚠️ ') + chalk.yellow(msg)),

    display: (data) => {

        // Contract Info
        console.log(boxen(
            chalk.white(`Contract: ${data.contract}\n`) +
            chalk.white(`Contractor: ${data.contractor}\n`) +
            chalk.white(`Work: ${data.work}, Qty: ${data.qty}`),
            { ...boxOptions, title: 'Contract Info', borderColor: 'blue' }
        ));

        // Sensor Data
        console.log(boxen(
            chalk.white(`Moisture: ${data.sensor.moisture}\n`) +
            chalk.white(`Ultrasonic: ${data.sensor.ultrasonic}\n`) +
            chalk.white(`Temperature: ${data.sensor.temperature}`),
            { ...boxOptions, title: 'Sensor Data', borderColor: 'cyan' }
        ));

        // Hash Details
        console.log(boxen(
            chalk.white(`ContractID: ${data.hashes.ContractID}\n`) +
            chalk.white(`ContractorName: ${data.hashes.ContractorName}\n`) +
            chalk.white(`Work: ${data.hashes.Work}\n`) +
            chalk.white(`Quantity: ${data.hashes.Quantity}\n`) +
            chalk.white(`Moisture: ${data.hashes.Moisture}\n`) +
            chalk.white(`Ultrasonic: ${data.hashes.Ultrasonic}\n`) +
            chalk.white(`Temperature: ${data.hashes.Temperature}`),
            { ...boxOptions, title: 'Hash Details', borderColor: 'magenta' }
        ));

        // Blockchain / TX
        const statusIcon = data.tx.status === 'RELEASED' ? '✅' : '🔒';
        console.log(boxen(
            chalk.white(`Transaction ID: ${data.tx.id}\n`) +
            chalk.white(`Gas Value: ₹${data.tx.gas}\n`) +
            chalk.white(`Status: ${statusIcon} ${data.tx.status}`),
            { ...boxOptions, title: 'Blockchain / TX', borderColor: data.tx.status === 'RELEASED' ? 'green' : 'red' }
        ));
    },

    spinner: (msg) => ora(msg).start()
};
