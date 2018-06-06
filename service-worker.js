var CACHE_NAME = 'rope-cache-3'
var filesToCache = [
    '/',
    'index.html',
    'favicon-16x16.png',
    'favicon.ico',
    'manifest.json',
    'browserconfig.xml',
    'android-chrome-192x192.png',
    'android-chrome-384x384.png',
    'android-chrome-530x530.png',
    'app.js',
    'bluetooth/bluetooth.js',
    'assets/highlight.svg',
    'assets/advance.svg',
    'assets/advance_exit.svg',
    'assets/go_active.svg',
    'assets/go_back_exit.svg',
    'assets/go_back.svg',
    'assets/go.svg',
    'assets/ico_48.svg',
    'assets/ico36.png',
    'assets/ico48.png',
    'assets/magnifying.svg',
    'assets/placeholder.svg',
    'assets/pointer.svg',
    'assets/rope_2.svg', 
    'assets/rope_hide.svg',
    'assets/rope_magnifying.svg',
    'assets/rope_not_found_2.svg',
    'assets/rope_not_found.svg',
    'assets/rope.svg',
    'assets/snapsound.mp3',
    'assets/startsound.wav',
    'assets/steps.svg',
    'assets/stopsound.wav',
    'assets/turn_left_exit.svg',
    'assets/turn_left.svg',
    'assets/turn_right_exit.svg',
    'assets/turn_right.svg',
    'assets/searching.svg',
    'assets/highlight.svg',
    'assets/next.wav',
    'jquery/jquery-3.3.1.min.js',
    'jquery/jquery-ui.min.js',
    'jquery/jquery.ui.touch-punch.min.js',
    'connecting/connecting.css',
    'connecting/connecting.js',
    'mstile-150x150.png',
    'programming/programming.css',
    'programming/programming.js',
    'programming/geometry.js',
    'style/buttons.css',
    'style/reset.css',
    'style/general.css'
]

self.addEventListener('install', function(event) {
    console.log('A *new* Service Worker is installing.');
    event.waitUntil(
        caches.open(CACHE_NAME)
        .then(function(cache){
            cache.addAll(filesToCache)
        })
    )
});
  
self.addEventListener('activate', function(event) {
    console.log('Finally i\'m active. Ready to start serving content!');
});

self.addEventListener('fetch', function(event){
    event.respondWith(
        caches.match(event.request)
        .then(function(response){
            if(response)
                return response
            return fetch(event.request)
        })
    )
})
