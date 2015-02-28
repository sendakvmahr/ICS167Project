// Server link
var Server;

// Rendering globals
var canvas = document.getElementById('canvas');
var ctx = canvas.getContext('2d');
canvas.width = 1000;
canvas.height = 600;
ctx.fillStyle = "#FFFFFF";
ctx.font = "30px sans-serif";
ctx.textAlign = "center";

// Dealing with game state and prediction
var latency = 0;            
var timeOfLastUpdate = 0;
var gameStarted = false;
var amPlayer1 = false;
var pastGameState = new GameState("0 0 0 0 0 0 0 0 0 0 0 0", false);
var currentGameState = new GameState("0 0 0 0 0 0 0 0 0 0 0 0", false);
var predictedStateMovements = new PredictedMovements();
var amMoving = false;
var movingUp = false;
var lastMovementSent = new Date();
var paddleSpeed = 5;

// Time Variables
/*
var initialTime = Date.now();
var initialSyncTime;
var timeOffset;
*/

// Class for storing calculated movement data
function PredictedMovements() {
    this.paddle1Movement = 0;
    this.paddle2Movement = 0;
    this.ballMovement = [0, 0]
}

PredictedMovements.prototype.calculateMovements = function (oldState, newState, ball_dirx, ball_diry) {
    var os = oldState;
    var ns = newState;
    this.paddle1Movement = ns.paddle1[1] - os.paddle1[1];
    this.paddle2Movement = ns.paddle2[1] - os.paddle2[1];
    this.timeDifference = msDifference(os.receivedAt, ns.receivedAt);
    this.ballMovement = [ball_dirx, ball_diry];
}

PredictedMovements.update = function(ballx, bally) {
    
}

// Class for storing the information in a game state
function GameState(dataString, isPrediction) {
    var info = dataString.split(" ");
    this.receivedAt = new Date();
    this.paddle1 = [Number(info[0]), Number(info[1])];
    this.paddle2 = [Number(info[2]), Number(info[3])];
    this.ball = [Number(info[4]), Number(info[5])];
    this.scores = [info[6], info[7]];
    this.player1ID = info[8];
    this.player2ID = info[9];
    this.sentAt = info[10];
    // Debugging
    this.prediction = isPrediction;
}

GameState.prototype.update = function(dataString) {
    var info = dataString.split(" ");
    this.receivedAt = new Date();
    this.paddle1 = [Number(info[0]), Number(info[1])];
    this.paddle2 = [Number(info[2]), Number(info[3])];
    this.ball = [Number(info[4]), Number(info[5])];
    this.scores = [info[6], info[7]];
    this.player1ID = info[8];
    this.player2ID = info[9];
    this.sentAt = info[10];
}
            
GameState.prototype.render = function(ctx, cw, ch) {
    ctx.clearRect(0, 0, cw, ch);
    // if (this.prediction) {
        // ctx.fillStyle = "#FF0000";
        // ctx.fillRect(0, 0, cw, ch);
        // ctx.fillStyle = "#FFFFFF";
    // }
    ctx.fillRect(this.paddle1[0], this.paddle1[1], 20, ch/4);
    ctx.fillRect(this.paddle2[0], this.paddle2[1], 20, ch/4);
    ctx.fillRect(this.ball[0], this.ball[1], 20, 20)
    ctx.fillText(this.scores[0], 200, 100);
    ctx.fillText(this.player1ID, 200, 50);
    ctx.fillText(this.scores[1], cw - 190, 100);
    ctx.fillText(this.player2ID, cw - 190, 50);
    ctx.fillText(latency, 450, 50);
}

GameState.prototype.applyPrediction = function(prediction, timeElapsed) {
    var result = new GameState("0 0 0 0 0 0 0 0 0 0 0 0", true);
    var proportion = timeElapsed / prediction.timeDifference;
    result.paddle1 = [this.paddle1[0], this.paddle1[1] + prediction.paddle1Movement * proportion];
    result.paddle2 = [this.paddle2[0], this.paddle2[1] + prediction.paddle2Movement * proportion];
    result.ball = [this.ball[0] + prediction.ballMovement[0] * proportion,  this.ball[1] + prediction.ballMovement[1] * proportion];
    result.player1ID = this.player1ID;
    result.player2ID = this.player2ID;
    return result;
}
GameState.prototype.copy = function(gamestate2) {
    this.receivedAt = gamestate2.receivedAt;
    this.paddle1 = gamestate2.paddle1;
    this.paddle2 = gamestate2.paddle2;
    this.ball = gamestate2.ball;
    this.scores = gamestate2.scores;
    this.player1ID = gamestate2.player1ID;
    amPlayer1 = (this.player1ID === document.getElementById('nickname').value);
    this.player2ID = gamestate2.player2ID;
    this.sentAt = gamestate2.sentAt;
}

// Time calculations
function msDifference(time1, time2) {
    var result = time1.getMilliseconds() - time2.getMilliseconds();
    result = result < 0 ? result + 1000 : result;
    return result;
}

function toCString (date) {
    // Format a date to send to c++ server
    return date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds() + ":" + date.getMilliseconds()
}
/*
$.ajax({
    url: 'http://www.timeapi.org/utc/now.json',
    dataType: 'jsonp'
})
.done(function(response) {
    initialSyncTime = new Date(response.dateString);
    initialTime = new Date();
    latencyOffset = - msDifference(initialSyncTime, initialTime);// "2012-03-06T02:18:25+00:00"
});
*/
// Server communication 
function send( text ) {
    Server.send( 'message', text );
}

function disconnect() {
    Server.disconnect();   
}

function checkRequirements() {
    // TODO - better checking on the ips and ports
    return document.getElementById('nickname').value.length > 0 
            && document.getElementById('ip').value.length > 0 
            && document.getElementById('port').value.length > 0;
}

function connect(){
    if (checkRequirements()) {
        Server = new FancyWebSocket('ws://' + document.getElementById('ip').value + ':' + document.getElementById('port').value);
        
        // Let the user know we're connected
        Server.bind('open', function() {
            document.getElementById("cntBtn").disabled = true;
        });

        // Disconnection occurred.
        Server.bind('close', function( data ) {
            document.getElementById("cntBtn").disabled = false;
            gameStarted = false;
        });

        //Log any messages sent from server
        Server.bind('message', function( payload ) {
            var isLatencyCalculation = payload.slice(0, 8) === "echo key";
            if (isLatencyCalculation) { // echo for a keyboard - use times to calculate latencyt
                var now = new Date();
                // Just to make everything a date object - will mess up at midnight for 1 frame
                var appendForParsing = now.getFullYear() + "/" + now.getMonth() + "/" + now.getDate() + " "; 
                var splitReply = payload.split(" ");
                var sentDate = new Date(appendForParsing + splitReply[4]);
                var receivedDate = new Date(appendForParsing + splitReply[3]);
                var travelTimeToServer = msDifference(sentDate, receivedDate);
                var travelTimeToClient = msDifference(now, receivedDate);
                latency = (travelTimeToServer + travelTimeToClient) / 2;
            }
            else if (payload.split(" ").length === 13){ // gamestate update
                // Example paddle1x paddle1y paddle2x paddle2y ballx bally score1 score2 p1name p2name 13:51:29:892
                // Numbers at the end are the time the server sent the update out
                var timeOfLastUpdate = new Date();
                var splitPayload = payload.split(" ");
                var echoedTime = splitPayload[10];
                send("Time:" + toCString(timeOfLastUpdate) + ": Time:" + echoedTime + ":");
                pastGameState.copy(currentGameState);
                currentGameState.update(payload);
                predictedStateMovements.calculateMovements(pastGameState, currentGameState, splitPayload[11], splitPayload[12]);
                currentGameState.render(ctx, canvas.width, canvas.height);
                gameStarted = true;
            }
        });
        
        Server.connect();
        timeOfLastUpdate = new Date();
        
        function update() {
            // Called based on time, does prediction if an update has not been received for long enough
            var now = new Date();
            var timeSinceLastUpdate = msDifference(now, timeOfLastUpdate);
            if ((timeSinceLastUpdate > 33) && (gameStarted)) {
                var predict = currentGameState.applyPrediction(predictedStateMovements, timeSinceLastUpdate);
                predict.render(ctx, canvas.width, canvas.height);
            }
            if ((msDifference(new Date(), lastMovementSent) > 33) && (gameStarted)) {
                lastMovementSent = new Date();
                send("paddlePosition: " + (amPlayer1 ? 10 : 270) + " " + (amPlayer1 ? currentGameState.paddle1[1] : currentGameState.paddle2[1]));
                //"paddlePosition: 10 225"
            }
            window.requestAnimationFrame(update);
        }
        
        update();
        
    }
    else {
        alert("Please fill out all the required fields.");
    }
}

function startGame() {
    // up is 87, 38
    // down is 40, 83
    $(document).keydown(function(key) {
        amMoving = true;
        var timeString = toCString(new Date());
        if (key.which === 87 || key.which  == 38) { // up
            // send("keydown up " + timeString);
            movingUp = true;
        }
        else if (key.which === 40 || key.which  == 83) { // down
            // send("keydown down " + timeString);
            movingUp = false;
        }
    });
    
    $(document).keyup(function(key) {
        amMoving = false;
        var timeString = toCString(new Date());
        if (key.which === 87 || key.which  == 38) { // up
            // send("keyup up " + timeString);
        }
        else if (key.which === 40 || key.which  == 83) { // down
            // send("keyup down " + timeString);
        }
    });
}

