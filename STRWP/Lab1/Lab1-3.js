var http = require('http')

let h = (r) => {
    let rc = ''
    for (key in r.headers)
        rc += '<div>' + key + ':' + r.headers[key] + '</div>'
    return rc
}

http.createServer(function(request, response) {
    let b = '';
    request.on('data', str => {
        b += str;
        console.log('data', b);
    })
    response.writeHead(200, {'Content-Type': 'text/html'})
    request.on('end', () => response.end(
        '<!DOCTYPE html>' +
        '<html>' + 
            '<head></head>' +
                '<body>' +
                    '<h1>The structure of request</h1>' +
                    '<h2>' + 'Method: ' + request.method + '</h2>' +
                    '<h2>' + 'URL: ' + request.url + '</h2>' +
                    '<h2>' + 'Version: ' + request.httpVersion + '</h2>' +
                    '<h2>' + 'Headers' + '</h2>' +
                    h(request) +
                    '<h2>' + 'Body: ' + b + '</h2>' +
                '</body>' +
        '</html>'
    ));
}).listen(3000)