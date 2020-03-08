self.addEventListener('install', function (event) {
    event.waitUntil(
        caches.open('v1').then(function (cache) {
            return cache.addAll([
                './static/js/bundle.js',
                './static/js/0.chunk.js',
                './static/js/main.chunk.js',
                './manifest.json',
                './favicon.ico',
                './logo192.png',
            ]).then(() => console.log('App cached'));
        })
    )
});

self.addEventListener('fetch', (event) => {
    event.respondWith(
        caches.match(event.request).then((resp) => {
            console.log((!!resp ? 'From cache: ' : 'Fetching: ') + event.request.url);
            resp.body.getReader().read().then(console.log);
            return resp || fetch(event.request);
        })
    );
});