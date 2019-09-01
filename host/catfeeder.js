

const prompt = 'sissi > '
const port = 3000
const TAG = 'app'

const log = (msg, async) => {if(async != undefined)commandOut(`\n${TAG}: ${msg}\n${prompt}`); else commandOut(msg);}
const commandOut = (msg) => process.stdout.write(msg)

const bodyParser = require('body-parser')
const express = require('express')
const app = express()
const path = require('path');

/**
 * Console commands
 */
const commands = {
    'help' : function() { log(
        "\nhelp\tthis message \
        \nexit\tquit application \
        \n")},
    'exit' : function () {log('Exiting...'); process.exit()},
}

/**
 * @param {stdin buffer} buffer 
 */
function commandInputHandler(buffer){    
    let cmd = commands[buffer.toString().split()[0].trim()]
    
    if(cmd == undefined){
        commandOut('Invalid command\n')        
    }else{
        cmd()
    }
    commandOut(prompt)
}


/**
 * Application entry point
 */

//app.get('/',(req, res) => {res.send('hello from server')})
app.use(bodyParser());
app.get('/',(req, res) => {
    //res.setHeader("content-encoding","gzip")
    res.sendFile(path.join(__dirname+'/../public/index.html'));
})

app.get('/schedules',(req, res) => {
    let sch = [
        {
        "qnt" : 0,
        "repeat": 24,
        "active" : true,
        "dateTime": "20:00"        
    },
    {
        "qnt" : 0,
        "repeat": 24,
        "active" : true,
        "dateTime": "08:00"        
    },
]
    res.send(JSON.stringify(sch));
})

app.post('/',(req, res) => {
    log(`Dispense: ${req.body.qnt}`, true);
    //res.contentType = "text/plain"
    res.header("Content-Type", "text/plain");
    res.end("ok")})
process.openStdin().addListener('data', commandInputHandler)
commandOut(prompt)
app.listen(port, () => log(`Example app listening on port ${port}!`, true));
//startServer(process.argv[2])
