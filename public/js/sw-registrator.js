navigator.serviceWorker.register('js/sw.js').then(
    () => console.log('Service worker has been installed!'),
    (error) => console.warn('Service worker has not been installed!', error));