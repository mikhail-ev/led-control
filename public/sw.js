console.log('sw!');

self.addEventListener('install', function(event) {
    event.waitUntil(
        caches.open('v1.1').then(function(cache) {
            return cache.addAll([
                '/',
                '/manifest.json',
                '/favicon.ico',
                '/js/controls.js',
                '/images/icons/icon-144x144.png'
            ]).then(() => console.log('App cached'))
                .catch((e) => console.log(e));
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

self.addEventListener('activate', function(event) {
    console.log('Claiming control', event);
    return self.clients.claim();
});