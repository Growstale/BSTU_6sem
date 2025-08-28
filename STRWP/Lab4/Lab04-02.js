const http = require("http")
const fs = require("fs")
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
  console.log("DELETE");
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


http.createServer(function (request, response) {
    const parsedUrl = url.parse(request.url, true);
    if (parsedUrl.pathname === "/api/db") {
        db.emit(request.method.toUpperCase(), request, response)
    } else if (parsedUrl.pathname === "/") {
      response.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
      response.end(
        fs.readFileSync("D:\\6sem_BSTU\\STRWP\\Lab4\\index.html")
      );
    } else {
      response.statusCode = 404;
      response.setHeader("Content-Type", "application/json");
      response.end('{"Error": "The page isn`t found"}');
    }
  }).listen(5000);
