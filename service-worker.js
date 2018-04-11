/* global caches self */
var cacheName = 'rope-cache-3'
var filesToCache = [
    'index.html',
    'favicon-16x16.png',
    'favicon.ico',
    'manifest.json',
    'browserconfig.xml',
    'android-chrome-192x192.png',
    'android-chrome-384x384.png',
    'app/app.js',
    'app/bluetooth.js',
    'assets/advance.svg',
    'assets/go_back.svg',
    'assets/ico36.png',
    'assets/ico48.png',
    'assets/ico_48.svg',
    'assets/magnifying.svg',
    'assets/rope.svg',
    'assets/rope_2.svg',
    'assets/rope_hide.svg',
    'assets/rope_magnifying.svg',
    'assets/rope_not_found.svg',
    'assets/rope_not_found_2.svg',
    'assets/searching.svg',
    'assets/turn_left.svg',
    'assets/turn_right.svg',
    'connecting/connecting.css',
    'connecting/connecting.css.map',
    'connecting/connecting.js',
    'connecting/connecting.sass',
    'mstile-150x150.png',
    'programming/programming.css',
    'programming/programming.js',
    'programming/programming.sass',
    'style/buttons.css',
    'style/buttons.css.map',
    'style/buttons.sass',
    'style/reset.css'
]

self.addEventListener('install', (e) => {
    console.log('[ServiceWorker] Install')
    e.waitUntil(
        caches.open(cacheName).then((cache) => {
            console.log('[ServiceWorker] Caching app shell')
            return cache.addAll(filesToCache)
        })
    )
})

self.addEventListener('activate', (e) => {
    console.log('[ServiceWorker] Activate')
    e.waitUntil(
        caches.keys().then((keyList) => {
            return Promise.all(keyList.map((key) => {
                if (key !== cacheName) {
                    console.log('[ServiceWorker] Removing old cache', key)
                    return caches.delete(key)
                }
            }))
        })
    )
    return self.clients.claim()
})

self.addEventListener('fetch', (e) => {
    console.log('[ServiceWorker] Fetch', e.request.url)
    e.respondWith(
        caches.match(e.request).then((response) => {
            return response || fetch(e.request)
        })
    )
})
