const http = require("http")
const fs = require("fs")
const url = require("url")

let data = require("./db.js")
let db = new data.DB()

let statsInterval;
let statsStartTime;
let statsEndTime;
let statsDuration;
let collectingStats = false;

let stats = {
  startTime: null,
  endTime: null,
  requestCount: 0,
  commitCount: 0,
};

db.on("COMMIT", () => {
    db.commit();
    if (collectingStats) {
      stats.commitCount++;
    }
});

db.on("GET", (request, response) => {
  console.log("GET");
  if (collectingStats) {
    stats.requestCount++;
  }
  response.end(JSON.stringify(db.select()));
});

db.on("POST", (request, response) => {
  console.log("POST")
  if (collectingStats) {
    stats.requestCount++;
  }
  request.on("data", (data) => {
    let row = JSON.parse(data)
    response.end(JSON.stringify(db.insert(row)))
  });
});

db.on("PUT", (request, response) => {
  console.log("PUT");
  if (collectingStats) {
    stats.requestCount++;
  }
  request.on("data", (data) => {
    let row = JSON.parse(data);
    response.end(JSON.stringify(db.update(row)));
  });
});

db.on("DELETE", (request, response) => {
  console.log("DELETE");
  if (collectingStats) {
    stats.requestCount++;
  }
  if (url.parse(request.url, true).query.id === undefined) {
    response.end('{"ERROR": "parameter not provided"}');
  }
  if (url.parse(request.url, true).query.id !== null) {
    let id = +url.parse(request.url, true).query.id;
    if (Number.isInteger(id)) {
      response.end(JSON.stringify(db.delete(id)));
    }
  }
});

const server = http.createServer(function (request, response) {
    const parsedUrl = url.parse(request.url, true);
    if (parsedUrl.pathname === "/api/db") {
        db.emit(request.method.toUpperCase(), request, response)
    } else if (parsedUrl.pathname === "/api/ss") {
      response.writeHead(200, { "Content-Type": "application/json" });
      response.end(JSON.stringify(stats));
    }
    else if (parsedUrl.pathname === "/") {
      response.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
      response.end(
        fs.readFileSync("D:\\6sem_BSTU\\STRWP\\Lab5\\index.html")
      );
    } else {
      response.statusCode = 404;
      response.setHeader("Content-Type", "application/json");
      response.end('{"Error": "The page isn`t found"}');
    }
})

server.listen(5000);

let sdTimeout;
let scInterval;

function stopStatsCollection() {
  if (collectingStats) {
    collectingStats = false;
    clearInterval(statsInterval);
    statsEndTime = new Date();
    statsDuration = (statsEndTime - statsStartTime) / 1000;
    console.log(`Statistics collected for ${statsDuration} seconds`);
    stats.endTime = statsEndTime;
  }
}

function startStatsCollection(duration) {
  if (collectingStats) {
    stopStatsCollection();
  }
  if (duration) {
    stats = {
      startTime: new Date(),
      endTime: null,
      requestCount: 0,
      commitCount: 0,
    };

    collectingStats = true;
    console.log(`Collecting statistics for ${duration} seconds...`);
    statsStartTime = new Date();

    statsInterval = setTimeout(() => {
      stopStatsCollection();
    }, duration * 1000);

    statsInterval.unref();
  }
}


process.stdin.on('data', (inputBuffer) => {
  const input = inputBuffer.toString().trim();
  const parts = input.split(" ");
  const command = parts[0];
  const argument = parts[1] ? parseInt(parts[1]) : null;

  switch (command) {
    case "sd": // остановить сервер через x секунд
      if (sdTimeout) {
        clearTimeout(sdTimeout); 
        console.log("Server shutdown timer cancelled");
      }

      if (argument) {
        sdTimeout = setTimeout(() => {
          console.log(`Server shutting down after ${argument} seconds...`);
          server.close(() => {
            process.exit(0);
          });
        }, argument * 1000);

        sdTimeout.unref();
        console.log(`Server will shut down in ${argument} seconds`);
      } else {
        console.log("Server shutdown cancelled");
      }
      break;

    case "sc": // коммит бд каджые x секунд
      if (scInterval) {
        clearInterval(scInterval);
        console.log("Periodic commit stopped");
      }

      if (argument) {
        scInterval = setInterval(() => {
          db.emit("COMMIT");
        }, argument * 1000);

        scInterval.unref(); 
        console.log(`Periodic commit started every ${argument} seconds`);
      } 
      break;

      case "ss": // сбор статистики x секунд
        if (argument === null) {
          stopStatsCollection();
        } else {
          startStatsCollection(argument);
        }
        break;


    default:
      console.log("Invalid command");
  }
});

console.log("Server running on port 5000.  Enter commands (sd, sc, ss)");