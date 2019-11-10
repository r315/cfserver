

const prompt = 'sissi > '
const port = 3000
const TAG = 'app'

const log = (msg, async) => {if(async != undefined)commandOut(`\n${TAG}: ${msg}\n${prompt}`); else commandOut(msg);}
const commandOut = (msg) => process.stdout.write(msg)

const bodyParser = require('body-parser')
const express = require('express')
const app = express()
const path = require('path');
const fs = require('fs')

const schedulesFile = path.join(__dirname+'/../public/schedule.json')

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
let schedules = []

function loadSchedules(){
    fs.readFile(schedulesFile, (err, data)=> {
        if(!err && data.length > 0)
            schedules = JSON.parse(data)
        else
            schedules = []
    })
}

function saveSchedules(cb){
    fs.writeFile(schedulesFile, JSON.stringify(schedules), (err)=>{
        if(!err)
            cb("ok")
        else
            cb("fail")
    })
}

app.use(bodyParser.json());
//app.use(bodyParser.urlencoded({extended: true}));

app.get('/',(req, res) => {
    //res.setHeader("content-encoding","gzip")
    res.sendFile(path.join(__dirname+'/../public/index.html'));
})

app.post('/',(req, res) => {
    log(`Dispense: ${req.body.qnt}`, true);
    //res.contentType = "text/plain"
    res.header("Content-Type", "text/plain");
    res.end("ok")
})


app.get('/schedule',(req, res) => {
    //res.sendFile(path.join(__dirname+'/../public/schedule.json'));
    res.send(JSON.stringify(schedules))
})

app.post('/schedule',(req, res) => {
    sch = req.body
    log(sch.dateTime, true)
    schedules.push(sch)
    saveSchedules((msg)=> { res.send(msg)})
})

app.delete('/schedule', (req, res) => {
    let index = req.body.index - 1 // remove index offset
    log(`delete request for index ${index}`, true)
    schedules.splice(index,1)
    res.end()
})

process.openStdin().addListener('data', commandInputHandler)

commandOut(prompt)

app.listen(port, () => log(`Example app listening on port ${port}!`, true));
//startServer(process.argv[2])

loadSchedules()