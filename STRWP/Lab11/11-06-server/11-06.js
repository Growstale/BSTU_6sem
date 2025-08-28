const WebSocket = require('ws');
const readline = require('readline');

const PORT = 4000;

const subscriptions = new Map([ // Подписки
    ['A', new Set()],
    ['B', new Set()],
    ['C', new Set()]
]);

const wss = new WebSocket.Server({ port: PORT });
console.log(`WebSocket Pub/Sub server started on ws://localhost:${PORT}`);
console.log("Enter A, B, or C in the console to trigger events.");

wss.on('connection', (ws) => {
    console.log('Client connected');
    ws.on('message', (message) => {
        try {
            const request = JSON.parse(message.toString());
            console.log('Received message from client:', request);

            if (request.type === 'subscribe' && request.event) {
                const eventName = request.event.toUpperCase();
                if (subscriptions.has(eventName)) {
                    const subscribers = subscriptions.get(eventName);
                    subscribers.add(ws); // Добавляем ws в Set подписчиков на это событие
                    console.log(`Client subscribed to event: ${eventName}`);
                    ws.send(JSON.stringify({ type: 'ack', status: 'subscribed', event: eventName }));
                } else {
                    console.warn(`Client tried to subscribe to unknown event: ${eventName}`);
                     ws.send(JSON.stringify({ type: 'error', message: `Unknown event: ${eventName}`}));
                }
            }
            else if (request.type === 'unsubscribe' && request.event) {
                const eventName = request.event.toUpperCase();
                 if (subscriptions.has(eventName)) {
                    const subscribers = subscriptions.get(eventName);
                    if (subscribers.delete(ws)) { 
                         console.log(`Client unsubscribed from event: ${eventName}`);
                         ws.send(JSON.stringify({ type: 'ack', status: 'unsubscribed', event: eventName }));
                    }
                 }
            }
            else {
                console.warn('Received unknown message type from client:', request);
            }

        } catch (e) {
             console.error('Failed to process message or invalid JSON:', e);
        }
    });

    ws.on('close', () => {
        console.log('Client disconnected');
        // удаляем из подписок
        subscriptions.forEach((subscribers, eventName) => {
            if (subscribers.delete(ws)) {
                console.log(`Removed disconnected client from event ${eventName} subscription.`);
            }
        });
    });

    ws.on('error', (error) => {
        console.error('WebSocket client error:', error);
        subscriptions.forEach((subscribers, eventName) => {
            subscribers.delete(ws);
        });
    });
});

function broadcastEvent(eventName) {
    eventName = eventName.toUpperCase();
    if (!subscriptions.has(eventName)) {
        console.log(`Event ${eventName} triggered, but no event type registered.`);
        return;
    }

    const subscribers = subscriptions.get(eventName);
    if (subscribers.size === 0) {
        console.log(`Event ${eventName} triggered, but no clients subscribed.`);
        return;
    }

    console.log(`Broadcasting event ${eventName} to ${subscribers.size} subscribers.`);
    const message = JSON.stringify({
        type: 'event',
        event: eventName,
        timestamp: new Date().toISOString()
    });

    subscribers.forEach(client => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(message, (err) => {
                if (err) {
                    console.error(`Error sending event ${eventName} to a client:`, err);
                }
            });
        } else {
            console.warn(`Removing non-open client from event ${eventName} subscription during broadcast.`);
            subscribers.delete(client);
        }
    });
}

// --- Чтение ввода с консоли ---
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  prompt: '> ' 
});

rl.prompt(); // Показать первое приглашение

rl.on('line', (line) => {
  const input = line.trim().toUpperCase();
  switch (input) {
    case 'A':
    case 'B':
    case 'C':
      console.log(`\nConsole input received: ${input}. Triggering event...`);
      broadcastEvent(input);
      break;
    case 'EXIT':
        console.log('Exiting server...');
        rl.close();
        wss.close(() => {
            console.log('WebSocket server closed.');
            process.exit(0);
        });
        break;
    default:
      console.log(`Unknown command: "${line.trim()}". Enter A, B, C, or EXIT.`);
      break;
  }
  if (input !== 'EXIT') {
      rl.prompt();
  }
}).on('close', () => {
  console.log('Console input closed.');
  if (!wss.closing) {
      wss.close(() => process.exit(0));
  }
});

wss.on('error', (error) => {
    console.error('WebSocket Server Error:', error);
    rl.close();
});