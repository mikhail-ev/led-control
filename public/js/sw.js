self.addEventListener('install', function (event) {
    event.waitUntil(
        caches.open('v1').then(function (cache) {
            return cache.addAll([
                '/',
                '/index.html',
                '/js/controls.js',
            ]).then(() => console.log('App cached'), (e) => console.warn(e));
        })
    )
});

self.addEventListener('fetch', (event) => {
    event.respondWith(
        caches.match(event.request).then((resp) => {
            console.log((!!resp ? 'From cache: ' : 'Fetching: ') + event.request.url);
            return resp || fetch(event.request);
        })
    );
});