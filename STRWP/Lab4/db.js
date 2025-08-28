const util = require("util")
const ee = require("events")

let db_data = [
  { id: 1, name: "James Carter", bday: "1995-07-14" },
  { id: 2, name: "Emily Richardson", bday: "2008-03-22" },
  { id: 3, name: "William Harrison", bday: "2016-11-09" },
  { id: 4, name: "Sophia Bennett", bday: "1983-05-30" },
  { id: 5, name: "Daniel Cooper", bday: "2001-08-17" },
];

function DB() {
  this.select = () => {
    return db_data
  };

  this.insert = (row) => {
    if (row.name.trim() === "" || row.bday.trim() === "") {
      return JSON.parse('{"Error": "You sent empty data"}')
    }
    row.id = db_data.length ? Math.max(...db_data.map((item) => item.id)) + 1 : 1
    db_data.push(row)
    return row
  };

  this.update = (row) => {
    var id = parseInt(row.id)
    var index = db_data.findIndex((el) => el.id === id)
    if (index === -1) return JSON.parse('{"Ошибка": "id не найден"}')
    db_data[index] = row
    return row
  };

  this.delete = (row) => {
    var id = parseInt(row)
    var index = db_data.findIndex((el) => el.id === id)
    if (index === -1) return JSON.parse(`{"Ошибка": "id ${id} не найден"}`)
    var row = db_data[index]
    db_data.splice(index, 1)
    db_data.forEach((item, idx) => (item.id = idx + 1))
    return row
  };
}

util.inherits(DB, ee.EventEmitter)
exports.DB = DB
