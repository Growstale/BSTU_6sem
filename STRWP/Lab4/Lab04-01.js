const http = require("http")
const url = require("url")

let data = require("./db.js")
let db = new data.DB()

db.on("GET", (request, response) => {
  console.log("GET");
  response.end(JSON.stringify(db.select()));
});

db.on("POST", (request, response) => {
  console.log("POST")
  request.on("data", (data) => {
    let row = JSON.parse(data)
    response.end(JSON.stringify(db.insert(row)))
  });
});

db.on("PUT", (request, response) => {
  console.log("PUT");
  request.on("data", (data) => {
    let row = JSON.parse(data);
    response.end(JSON.stringify(db.update(row)));
  });
});

db.on("DELETE", (request, response) => {
    console.log("DELETE request received");
    
    const query = url.parse(request.url, true).query;
    const id = Number(query.id);
  
    if (!query.id) {
      response.writeHead(400, { "Content-Type": "application/json" });
      return response.end(JSON.stringify({ "Error": "ID is not provided" }));
    }
  
    if (!Number.isInteger(id)) {
      response.writeHead(400, { "Content-Type": "application/json" });
      return response.end(JSON.stringify({ "Error": "Invalid ID format" }));
    }
  
    const result = db.delete(id);
    response.writeHead(200, { "Content-Type": "application/json" });
    response.end(JSON.stringify(result));
});

http.createServer(function (request, response) {
    const parsedUrl = url.parse(request.url, true);
    if (parsedUrl.pathname === "/api/db") {
        db.emit(request.method.toUpperCase(), request, response)
    } else {
      response.statusCode = 404;
      response.setHeader("Content-Type", "application/json");
      response.end('{"Error": "The page isn`t found"}');
    }
  }).listen(5000);
